This is a simple C++ project, as I hadn't touched C++ in a long while.

To build

```console
$ nix shell . # If you have nix installed, otherwise make # sure you have
              # CMake, C compiler etc installed
nix-shell$ cmake -B build -G Ninja
nix-shell$ cmake --build build
```

To run

```console
$ ./build/tiny-interp
```
