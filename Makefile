.PHONY: build test sanitize clean rebuild

build:
	cmake -B build
	cmake --build build

test: build
	./build/tests/serdec_tests

sanitize:
	cmake -B build -DSERDEC_ENABLE_SANITIZERS=ON
	cmake --build build
	./build/tests/serdec_tests

clean:
	rm -rf build

rebuild: clean build
