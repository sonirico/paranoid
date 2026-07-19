BUILD_TYPE ?= Debug
BUILD_DIR  := build/$(shell echo $(BUILD_TYPE) | tr '[:upper:]' '[:lower:]')
JOBS       ?= $(shell nproc)
CMAKE_FLAGS ?=
PREFIX     ?= /usr
BINDIR     := $(PREFIX)/games
DATADIR    := $(PREFIX)/share/games/paranoid
APPDIR     := $(PREFIX)/share/applications
ICONDIR    := $(PREFIX)/share/icons/hicolor/scalable/apps

.PHONY: help configure build run test release debug clean format format-check leak-check sanitize install uninstall

help: ## Show this help
	@grep -hE '^[a-zA-Z_-]+:.*## ' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*## "}; {printf "  \033[36m%-14s\033[0m %s\n", $$1, $$2}'

configure: ## Generate the CMake build tree
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CMAKE_FLAGS)

build: configure ## Compile the game and tests (BUILD_TYPE=Debug|Release)
	cmake --build $(BUILD_DIR) -j $(JOBS)

run: build ## Build and launch the game
	$(BUILD_DIR)/paranoid

test: build ## Build and run the unit test suite
	ctest --test-dir $(BUILD_DIR) --output-on-failure

release: ## Compile an optimized Release build
	$(MAKE) BUILD_TYPE=Release build

debug: build ## Alias for the default Debug build

install: release ## Install the game, assets and desktop entry (PREFIX=/usr, DESTDIR aware)
	install -Dm755 build/release/paranoid $(DESTDIR)$(DATADIR)/paranoid
	cp -r media $(DESTDIR)$(DATADIR)/
	install -d $(DESTDIR)$(BINDIR)
	printf '#!/bin/sh\ncd %s && exec ./paranoid "$$@"\n' '$(DATADIR)' > $(DESTDIR)$(BINDIR)/paranoid
	chmod 755 $(DESTDIR)$(BINDIR)/paranoid
	install -d $(DESTDIR)$(APPDIR)
	sed 's|@BINDIR@|$(BINDIR)|' packaging/paranoid.desktop > $(DESTDIR)$(APPDIR)/paranoid.desktop
	install -Dm644 packaging/paranoid.svg $(DESTDIR)$(ICONDIR)/paranoid.svg
	-update-desktop-database $(DESTDIR)$(APPDIR) 2>/dev/null
	-gtk-update-icon-cache -q $(DESTDIR)$(PREFIX)/share/icons/hicolor 2>/dev/null

uninstall: ## Remove the installed game, assets and desktop entry
	rm -f $(DESTDIR)$(BINDIR)/paranoid
	rm -rf $(DESTDIR)$(DATADIR)
	rm -f $(DESTDIR)$(APPDIR)/paranoid.desktop
	rm -f $(DESTDIR)$(ICONDIR)/paranoid.svg
	-update-desktop-database $(DESTDIR)$(APPDIR) 2>/dev/null
	-gtk-update-icon-cache -q $(DESTDIR)$(PREFIX)/share/icons/hicolor 2>/dev/null

dev: ## Build and launch with developer cheats (DEV_MODE)
	$(MAKE) BUILD_DIR=build/dev CMAKE_FLAGS=-DPARANOID_DEV_MODE=ON run

sanitize: ## Build and run tests under ASan/UBSan
	$(MAKE) BUILD_DIR=build/asan CMAKE_FLAGS=-DPARANOID_SANITIZE=ON test

leak-check: build ## Run tests and a game smoke frame under valgrind
	valgrind --leak-check=full --errors-for-leak-kinds=definite \
		--error-exitcode=1 $(BUILD_DIR)/tests/paranoid_tests
	valgrind --leak-check=full --errors-for-leak-kinds=definite \
		--error-exitcode=1 $(BUILD_DIR)/paranoid --smoke

SOURCES := $(shell find src tests -name '*.cpp' -o -name '*.hpp' -o -name '*.h')

format: ## Reformat all sources in place with clang-format
	clang-format -i $(SOURCES)

format-check: ## Fail if any source is not clang-format clean
	clang-format --dry-run --Werror $(SOURCES)

clean: ## Remove all build trees
	rm -rf build
