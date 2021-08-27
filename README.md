# **Simpletron**

***Simpletron*** is simple machine which run simple programs with a defined number of instructions.
You can write programs using the ***Simpletron Machine Language (SML)*** and load these programs
on the Simpletron.

*Updated: 27 Ago 2021*

<br/>

## How to run the project

You can compile and run this project using GNU gcc. Follow the steps below to install the required tools:

```sh
$ sudo apt update
```

```sh
$ sudo apt install build-essential
```

Build and Run Simpletron:

```sh
$ gcc simpletron.c -o simpletron && ./simpletron
```

<br/>

## Simpletron Machine Language (SML)
Below are the instruction available on the SML:
<br/>
<br/>
| Instruction | Name     | Description|
|:-----------:|:---------|:-----------|
|Input/Output |
|    **10**   | READ     | Reads a word from the terminal into a specific location in memory |
|    **11**   | WRITE    | Writes a word from a specific location in memory to the terminal |
|Load/Store   |
|    **20**   | LOAD     | Loads a word from a specific location in memory into the accumulator |
|    **21**   | STORE    | Stores a word from the accumulator into a specific location in memory |
|Arithmetic   |
|    **30**   | ADD      | Adds a word from a specific location in memory to the word in the accumulator (leaves result in the accumulator) |
|    **31**   | SUBTRACT | Subtracts a word from a specific location in memory to the word in the accumulator (leaves result in the accumulator) |
|    **32**   | DIVIDE   | Divides a word from a specific location in memory to the word in the accumulator (leaves result in the accumulator)  |
|    **33**   | MULTIPLY | Multiplies a word from a specific location in memory to the word in the accumulator (leaves result in the accumulator)  |
|Transfer of Control|
|    **40**   | BRANCH   | Branchs to a specific memory location |
|    **41**   | BRANCHNEG| Branchs to a specific memory location if the accumulator is negative |
|    **42**   |BRANCHZERO| Branchs to a specific memory location if the accumulator is zero |
|    **43**   | HALT     | Quit the program that completed the tasks |

<br/>
<br/>

### Simple SML Program
```SML
    1099    // Read A
    1098    // Read B
    2099    // Load A
    3098    // Add A to B
    2197    // Store the result
    1197    // Write the result on screen
    4300    // Quit the program
   -9999    // End Program
```
