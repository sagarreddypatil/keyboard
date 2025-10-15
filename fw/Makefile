CMAKE_FLAGS := -GNinja

build: CMAKE_FLAGS += -DCMAKE_BUILD_TYPE=Debug
build: cmake

release: CMAKE_FLAGS += -DCMAKE_BUILD_TYPE=Release
release: cmake

cmake: deps
	mkdir -p build
	@cd build && cmake $(CMAKE_FLAGS) .. --log-level=STATUS && ninja

clean:
	rm -rf build

deps: lib/pico-sdk

clean-deps:
	rm -rf lib

lib/pico-sdk:
	git clone --depth 1 --branch 2.2.0 https://github.com/raspberrypi/pico-sdk.git lib/pico-sdk
	cd lib/pico-sdk && git submodule update --init lib/tinyusb

.PHONY: build release cmake clean deps clean-deps