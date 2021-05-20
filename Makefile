.PHONY: all
all: format build

.PHONY: format
format:
	clang-format src/* include/* -i

.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake .. && \
	$(MAKE)

.PHONY: debug
debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=debug .. && \
	$(MAKE)

.PHONY: clean
clean:
	rm -rf build

.PHONY: launch
launch:
	docker-compose run --rm --service-ports system-monitor
	docker-compose down --volumes --remove-orphans
