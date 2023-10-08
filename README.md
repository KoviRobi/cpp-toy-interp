This is a simple C++ project, as a C++ refresher.

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
> let inc = fn x x + 1
fn x ( x ) + ( 1 )
> let x = 1
1
> inc x
2
> inc ( inc x )
3
```

You can as usual achieve recursive functions using the applicative Y
combinator, see the complicated function test in `test/tests.cpp`

```console
> let Y = fn f { ( fn x { f ( fn a { ( x x ) a } ) } ) ( fn x { f ( fn a { ( x x ) a } ) } ) }
fn f { ( fn x { ( f ) ( fn a { ( ( x ) ( x ) ) ( a ) } ) } ) ( fn x { ( f ) ( fn a { ( ( x ) ( x ) ) ( a ) } ) } ) }

> let sum_n = Y ( fn sum_n { fn n { if n == 0 then 0 else n + sum_n ( n - 1 )  } } )
fn n { if ( ( n ) == ( 0 ) ) then ( 0 ) else ( ( n ) + ( ( sum_n ) ( ( n ) - ( 1 ) ) ) ) }

> sum_n 10
55
```
