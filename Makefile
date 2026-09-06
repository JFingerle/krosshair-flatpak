LIBRARY = lib/krosshair.so

CCFLAGS = -Wall -std=c99 -fPIC -shared -I./include/
LDFLAGS = -ldl -lm

# Debug build by default; use `make release` or `make RELEASE=1` for release
ifdef RELEASE
CCFLAGS += -O2 -DNDEBUG
else
CCFLAGS += -ggdb
endif

SOURCES = $(shell find src -type f -name "*.c")

BUILD_DIR = build
FLATPAK_BUILD_DIR = $(BUILD_DIR)/flatpak
FLATPAK_BUILD_DIR_INTERMEDIATE = $(FLATPAK_BUILD_DIR)/intermediate
FLATPAK_EXPORT_DIR = $(FLATPAK_BUILD_DIR_INTERMEDIATE)/export

FLATPAK_BUNDLE_ID = org.freedesktop.Platform.VulkanLayer.krosshair
FLATPAK_VERSIONS = 24.08 25.08 26.08
FLATPAK_LIB_PATH = /usr/lib/extensions/vulkan/krosshair/lib/krosshair.so
FLATPAK_MANIST_FILE = flatpak/$(FLATPAK_BUNDLE_ID).yml


.PHONY: all release clean install flatpak flatpak-install

all:
	mkdir -p lib
	$(CC) $(CCFLAGS) $(SOURCES) $(LDFLAGS) -o $(LIBRARY)

release:
	$(MAKE) RELEASE=1 all

install:
	sudo mkdir -p /usr/lib/krosshair
	sudo cp $(LIBRARY) /usr/lib/krosshair/krosshair.so
	sudo cp krosshair.json /usr/share/vulkan/implicit_layer.d/krosshair.json

clean:
	rm -f $(LIBRARY)
	rm -rf $(BUILD_DIR)

# Builds the flatpak into $(BUILD_DIR) (does NOT install it on the host).
flatpak:
	mkdir -p $(FLATPAK_BUILD_DIR)
	git diff --exit-code $(FLATPAK_MANIST_FILE) || { echo "ERROR: $(FLATPAK_MANIST_FILE) has uncommitted changes"; exit 1; }
	for VER in $(FLATPAK_VERSIONS); do \
		echo -e "\n-----\nBuilding flatpak for version \"$$VER\"...\n-----\n"; \
		mkdir -p $(FLATPAK_BUILD_DIR_INTERMEDIATE)/$$VER || exit 1; \
		sed -i "s|^branch:.*|branch: \"$$VER\"|" $(FLATPAK_MANIST_FILE) || exit 1; \
		sed -i "s|^runtime-version:.*|runtime-version: \"$$VER\"|" $(FLATPAK_MANIST_FILE) || exit 1; \
		# flatpak SDK install \
		echo -e "\n-----\nRunning flatpak install for \"org.freedesktop.Sdk//$$VER\"...\n-----\n"; \
		flatpak install -y flathub org.freedesktop.Sdk//$$VER || exit 1; \
		# flatpak-builder \
		echo -e "\n-----\nRunning flatpak-builder for \"$$VER\"...\n-----\n"; \
		flatpak-builder --force-clean $(FLATPAK_BUILD_DIR_INTERMEDIATE)/$$VER $(FLATPAK_MANIST_FILE) || exit 1; \
		# flatpak build-export \
		echo -e "\n-----\nRunning flatpak build-export for \"$$VER\"...\n-----\n"; \
		flatpak build-export $(FLATPAK_EXPORT_DIR) $(FLATPAK_BUILD_DIR_INTERMEDIATE)/$$VER || exit 1; \
		# flatpak build-bundle \
		echo -e "\n-----\nRunning flatpak build-bundle for \"$$VER\"...\n-----\n"; \
		flatpak build-bundle $(FLATPAK_EXPORT_DIR) $(FLATPAK_BUILD_DIR)/$(FLATPAK_BUNDLE_ID)_$$VER.flatpak runtime/$(FLATPAK_BUNDLE_ID)/x86_64/master || exit 1; \
	done
	git checkout -- $(MANIFEST)

# Runs inside the flatpak-builder sandbox (invoked from the .yml).
# Builds the layer and stages files into the flatpak output (/app).
flatpak-install: release
	sed -i 's|"library_path"[[:space:]]*:[[:space:]]*"[^"]*"|"library_path": "$(FLATPAK_LIB_PATH)"|' krosshair.json
	install -Dm755 $(LIBRARY) -t /app/lib/
	install -Dm644 krosshair.json -t /app/share/vulkan/implicit_layer.d/
	grep -H library_path /app/share/vulkan/implicit_layer.d/krosshair.json
