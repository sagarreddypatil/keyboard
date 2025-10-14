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

deps: external/pico-sdk

clean-deps:
	rm -rf external

external/pico-sdk:
	git clone --depth 1 --branch 2.2.0 https://github.com/raspberrypi/pico-sdk.git external/pico-sdk
	cd external/pico-sdk && git submodule update --init lib/tinyusb

.PHONY: cmake clean deps clean-deps