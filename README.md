# MiniLang Compiler

This project contains a simple MiniLang compiler implementation that supports object‑oriented programming features such as classes, inheritance, and methods. The compiler translates MiniLang source files into C++ code, which can then be compiled into an executable program.

## Project Structure

- **AST.hpp** - Defines the abstract syntax tree (AST) for MiniLang.
- **Lexer.hpp / Lexer.cpp** - Tokenizes the MiniLang source code.
- **Parser.hpp / Parser.cpp** - Parses tokens into an AST.
- **CodeGenerator.hpp / CodeGenerator.cpp** - Generates equivalent C++ code from the AST.
- **Builtins.cpp** - Contains runtime support for built-in functions.
- **main.cpp** - The entry point for the MiniLang compiler.
- **Build.sh** - Bash script to build the MiniLang compiler.
- **Launch.sh** - Bash script to compile the generated C++ code and run the resulting program.
- **README.md** - This documentation file.

## Building the Compiler

To build the MiniLang compiler, open a terminal and run:

```bash
./Build.sh
```

This script compiles the source files into an executable named **mini_compiler**.

## Using the Compiler

After building the compiler, you can compile a MiniLang source file (for example, `example.minilang`) by running:

```bash
./mini_compiler example.minilang
```

This command generates a C++ source file named **compiled.cpp**.

## Compiling and Running the Generated Program

To compile the generated C++ code (along with **Builtins.cpp**) and run the resulting executable, run:

```bash
./Launch.sh
```

This script compiles **compiled.cpp** into an executable named **program** and then runs it.

## Example MiniLang Source

Below is an example of a MiniLang source file (`oop_inheritance_example.minilang`):

```minilang
// Define a base class Animal.
class Animal {
    let name = "Unnamed";

    function setName(newName) {
        name = newName;
        return name;
    }

    function greet() {
        return "Hello, I'm an animal named " + name;
    }
}

// Define a subclass Dog extending Animal.
class Dog extends Animal {
    function greet() {
        return "Woof! I'm " + name;
    }

    function bark() {
        return "Woof!";
    }
}

let d = new Dog();
print d.greet();
print d.setName("Buddy");
print d.greet();
print d.bark();
```

## Dependencies

- GNU Compiler Collection (g++)
- Bash shell

Ensure that your system has the necessary dependencies installed.

## License

This project is licensed under the MIT License.