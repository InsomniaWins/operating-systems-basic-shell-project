Basic Shell Program
by Andrew Ingram, Andrew Kalathra, Caleb Joiner


The purpose of this program is to serve as a basic shell over linux.
It allows the user to enter linux commands in a terminal and processes
them in a forked process (part 1) or a seperate thread (part 2).


All shared functions/constants/variables are declared and initialized in
the src/shared.cpp file. The rest of the code required for each part has
been contained in their own cpp files. (src/part_1.cpp and src/part_2.cpp).


To compile this program, enter the base directory for this program (the one
containing this README.txt file) and enter one of the following commands:
    for part 1: g++ "src/part_1.cpp" -lpthread -o "out/part_1"
    for part 2: g++ "src/part_2.cpp" -lpthread -o "out/part_2"
To run this program, remain in the base directory and enter one of the
following commands:
    for part 1: ./"out/part_1"
    for part 2: ./"out/part_2"


- HOW TO USE -
    Please refrain from using quotes. Quotes support was not implemented before
the project deadline. If your file names must have a space, replace the space
with an underscore character '_'. If you use quotes, the quotes will be included
in the parameters inputted, and any space between the quotes will end and begin
parameters.

1)  Run the program.
2)  From there you may enter any normal linux command.
        You may not run a program relative to the current path with a command
        such as "./program_name". We could not implement this in time.
    Custom commands:
        cd <directory>
            changes the current working directory relative to the current
            working directory
        quit
            quits the shell program
