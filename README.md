# Wrap dl(open|sym|close)

The purpose of this project is to monitor calls to dlopen/dlsym in order to keep track of shared object files that are
loaded during a projects runtime.  These calls cannot always be statically analyzed due to obfuscation through function
calls.

## Usefulness

* **RPM packagers**: When packaging programs from languages such as Python finding all dependencies of the program so
  that they can easily state them as requirements
* **Containerized Development**: when running a program in a container it is best to find a container which already
  provides all required libraries in the correct versions which provide all required symbols.
* **You**: if you find this tool useful and don't fall into the above categories, open an issue and say what you use it
  for? :)

## How to

1. `$ make`
2. `$ LD_PRELOAD=$PWD/overwrite-dl.so <insert your command>`
3. `$ cat /tmp/dll_sym_<pid>`

**NOTE:** step 3 can be run before program terminates

## Limitations

As with any dynamic information, this may not give a complete picture of all possible `dl(open|sym)` usage if during
execution it misses conditional branches with these function calls.

## Implementation

This tool relies on the LD_PRELOAD "trick", which you can read more about
[here](https://rafalcieslak.wordpress.com/2013/04/02/dynamic-linker-tricks-using-ld_preload-to-cheat-inject-features-and-investigate-programs/)

Using this trick allows the user to overwrite any library functions and redefine them in your own shared object file.  This is extremely useful for program monitoring because it can catch and log interesting function calls without changing anything about the program it is being used inside of.

For this use case we need to change the behavior of "dlopen", "dlsym", and "dlclose".  The reason we need to alter all
three is because the `void * handle` returned by `dlopen` is completely opaque and has no guarantees as to
implementation, so it was necessary to maintain a basic linked list for looking up filenames based off of pointers.
"dlclose" was altered to simply free all of this information as dlclose is called.

It also required minor changes to the LD_PRELOAD trick because the trick itself uses `dlsym`, so it needs to use the
underlying `_dlsym` as to not result in infinite recursion.

## Questions

If you have any questions or requests feel free to open an issue. :)
