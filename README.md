# Project 2: Command Line Shell

See: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html

## About
The outermost layer of the operating system is called the shell. In Unix-based systems, the shell is generally a command line interface. Most Linux distributions ship with bash as the default (there are several others: csh, ksh, sh, tcsh, zsh). This program in an implementation of a Unix-based shell named sheesh (shee shell).
## To compile and run:

```bash
make
./sheesh
Example 1: echo "hello world! > output.txt
Example 2: cat /etc/passwd | sort > sorted_pwd.txt 

```
## Included Files

* **shell.c**: Driver for the shell program.
* **shell.h**: Header file containing built in functions for the shell.
* **ui.c**: Text-based UI functionality. 
* **ui.h**: Header file, contains functions primarily concerned with interaction
  with the readline library.
* **history.c**: Command historyy functionality
* **history.h**: Header file contains functions for operating on the history
  circular array.
* **shellper.c**: Includes functions to get information about the system, hardware,
  running tasks, other processes, and relevant process information (count,
  state, PID, name, user).
* **shellper.h**: Header file for shellper.c.
* **Makefile**: included to compile and run the program.


## Program Output
$ ./inspector

```
ui.c:107:init_ui(): Initializing UI...
ui.c:110:init_ui(): Setting locale: en_US.UTF-8
[:-)]-[0]-[kdcurrie@keaton-beacon:~/P2-kdcurrie]>
```

## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
* Run all test cases:
make test

* Run a specific test case:
make test run=4

* Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'

```

