# Turing Machine Simulator

Inspired in closed source [Turing Machine Simulator](https://turingmachinesimulator.com/).

Simulator written in C with low memory usage aimed to run multiple parallel Turing Machines taking advantage of CPU resources.

- Designed to be resource friendly and fast
- Built-in intuitive interpreter to automata definition
- Share easily your Turing Machines in a single text file
- Run Multiple Turing Machines in a single script to check strings
- Non-deterministic Turing Machines Support
- Multithread support through [OpenMP](https://www.openmp.org/)
- Builds for x86_64 Linux/Windows and arm64 Linux
- GCC, clang and MINGW
- Highly portable to other platforms and devices

## Build Instructions

Built with [GCC](https://gcc.gnu.org/) for Linux and [Minimalist GNU for Windows](https://sourceforge.net/projects/mingw/) (`MINGW`) for Windows.

It is also necessary to have [GNU Make](https://www.gnu.org/software/make/) for compiling.

To build with [GCC](https://gcc.gnu.org/), run:

```
make
```

To build with [clang](https://clang.llvm.org/), run:

```
make CLANG=1
```

To build for MS windows use [MSYS2](https://www.msys2.org/) and a `MINGW` environment, then run:

```
make MINGW=1
```

You can also build and run simulations under bash with `MSYS2`.

To install it into your computer under linux, run:

```
sudo make install
```

or simply add it to your `$PATH` environment variable.

## Multithread Mode

If your compiler has OpenMP Support, you will be able to build the simulator adding `OPENMP=1` in make:

```
make OPENMP=1
```

## How to use

Just hit help with:

```
./tmsim --help
```

For scripting, check [this example](sample/example.txt) folder with examples and instructions.

## Limitations

- There are more turing machines optimizations to be implemented to make it faster/lighter
- Console based, no GUI
- No more than 256 states
- Tapes String with 256 maximum characters length
- No more than 256 moves
- No more than 255 simulations in a single script for Deterministic Turing Machines
- No more than \(2^{32} - 1\) instances for Non-Deterministic Turing Machines
- No Multithread Support (yet) for Non-deterministic Turing Machines
- No more than 256 characters for text interpreter to parse per line
- Tape Strings must be composed by ASCII characters, except commas

## TO DO List

- [ ] Multithread Support for Non-deterministic Turing Machines
- [ ] REPL mode for online Step Movement
- [ ] State Breakpoints Support
- [ ] "Always Write" Symbol Command
- [ ] Support for Turing Machines With Multiple Tapes
- [ ] Script Preprocessor for TM interpreter
- [ ] Trace for Non-deterministic Turing Machines when running in non-verbose mode
