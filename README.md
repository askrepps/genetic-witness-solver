# genetic-witness-solver
GPU-accelerated Witness puzzle solver using genetic algorithms (EN.605.417.SP18 final project)

## Build Instructions
1. If building on Linux, edit the OpenCL include and library paths (`OPENCL_INCDIR` and `OPENCL_LIBDIR`, respectively) in the Makefile if necessary (Note: Mac OSX builds should work without any Makefile changes, and Windows build are not supported at this time)
2. Run `make` to create the executable in the build directory

## Usage
To run the program, simply run `build/genWitnessSolver <puzzle file>` where `<puzzle file>` is the path to a text file containing the puzzle description

## Puzzle File Format
A puzzle file is a text file with the following contents:
1. A line containing the width of the puzzle (the number of points per row)
2. A line containing the height of the puzzle (the number of points per column)
3. Interleaved data for the contents of all points, edges and spaces. Rows of text alternate between (1) points and horizontal edges and (2) vertical edges and spaces until the bottom of the puzzle is reached.

Point data values:
1. `o`: Open point
2. `x`: The point does not exist in the puzzle
3. `.`: The point contains a dot (the solution must pass through this point)
4. `s`: The point can be the start of a solution (there can be multiple choices in one puzzle)
5. `e`: The point can be the end of a solution (there can be multiple choices in one puzzle)

Edge data values:
1. `o`: Open edge
2. `x`: The edge is blocked off or does not exist in the puzzle
3. `.`: The edge contains a dot (the solution must pass through this edge)

Space data values:
1. `_`: The space is blank
2. `w`: The space contains a white mark (the entire partition containing this space must have only white marks)
3. `b`: The space contains a black mark (the entire partition containing this space must have only black marks)

See the [data](data) folder for examples of puzzle files with accompanying images.

## TODO
Most of the core functionality has been implemented. Some stages of the genetic algorithm (initial generation and crossover/mutation) could be moved inside an OpenCL kernel for performance, but the basic structure is in place and works. The following items have not yet been implemented:
- [ ] OpenCL kernel implementation of random solution generation
- [ ] OpenCL kernel implementation of population crossover/mutation
