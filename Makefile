.PHONY: all
all: shared demo

.PHONY: shared
shared: overwrite-dl.so
overwrite-dl.so: overwrite-dl.c
	gcc -shared -fPIC overwrite-dl.c -o overwrite-dl.so -ldl

.PHONY: demo
demo: dynamic_loading
dynamic_loading: dynamic_loading.c
	gcc dynamic_loading.c -o dynamic_loading -ldl

.PHONY: clean
clean:
	rm overwrite-dl.so
	rm dynamic_loading
