// how to compile: gcc dynamic_loading.c -o dynamic_loading -ldl
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
 
int main(int argc, char **argv) {
    void *handle;
    void (*go)(char *);
    char *encoded, *decoded;
    int encoded_length;
 
    printf("begin\n");
    // get a handle to the library that contains 'puts' function
    handle = dlopen ("/lib64/libc.so.6", RTLD_LAZY);

    // each character in 'otsr' plus 1 in ascii is 'puts'
    encoded = "otsr";
    encoded_length = strlen(encoded);
    decoded = (char*)malloc((encoded_length+1) * sizeof(char));
    for (int i = 0; i < encoded_length; i++){
        decoded[i] = encoded[i]+1;
    }
    decoded[strlen(encoded)] = '\0';

    // reference to the dynamically-resolved function 'puts'
    go = dlsym(handle, decoded);
 
    go("hi"); // == puts("hi");
 
    go("goodbye");
    // cleanup
    // free(decoded);
    dlclose(handle);
}