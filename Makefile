CC = gcc
CFLAGS = -g -Wall

generated = dynamic_loading overwrite-dl.so

all: overwrite-dl.so dynamic_loading

shared: overwrite-dl.so

overwrite-dl.so: overwrite-dl.c
	$(CC) $(CFLAGS) -shared -fPIC $< -o $@ -ldl

dynamic_loading: dynamic_loading.c
	$(CC) $(CFLAGS) $< -o $@ -ldl

clean:
	rm -f $(generated)
