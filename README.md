# Overview

CPPAD is a simple command-line text editor like nano.  It is built with C++, CMake, and NCurses (to interface with the command-line). GoogleTest is also used for unit-testing.

To run the project first make sure [Docker](https://www.docker.com/) is installed. Then, on the project's directory build the docker image and run the shell:

```
docker build -t cppad .
docker run -it --rm cppad bash
```

Once the shell is running you can run CPPAD:

```
cd build
./cppad file
```

The program expects a single argument `filename`, which is either the name of the file to be edited in the current directory, or a path to a file in some other directory. If the file doesn't exist, it will be created.

# Design

The core modules of the CPPAD editor are the `CSCREEN` and `CONTENT` classes. The `CONTENT` class handles storing the text that is being edited and modifying it according to the edit events that are kicked off by the user's keystrokes. The `CSCREEN` class handles transforming the text that is being edited to the text that is shown on the screen. Handling user input and loading/saving the file is handled in the `CPPAD` class, which acts as a wrapper for the whole program.

## `CONTENT`

The `CONTENT` class stores the text that is being edited and handles modifications of that text. Text lends itself naturally to a contiguous data structure, so the choice is between a linked-list and an array. Since we have to support moving the cursor backward and forward as well as editing at any cursor position, a doubly linked-list makes the most sense. Using an array would mean that each edit could be O(# of chars) in the worst case. We use the standard library `std::list` implementation of a doubly linked-list.

It also makes sense to have a separate linked-list for each line of the text, and for each of these linked-lists to be stored in a doubly linked-list to allow fast movement of the cursor across lines. Then, the text being edited is stored in an object of type `std::list<std::list<char>>`.

### Tabs

A tab character advances the cursor to the next tab stop. We will define the tab stops in our program to be at multiples of 8 spaces on each line.

For alignment it helps to know the number of spaces in each line, so, when we handle a tab append, it makes sense to add the necessary whitespaces individually. To stay faithful to the characters that the user inputs, however, we also mark the place where the tab was added in a separate data structure, so that when the user saves the file the `\t` character is saved instead of the individual whitespaces. Keeping track of where tabs are also helps determine how many spaces to move when moving the cursor with the arrow keys.

Our tab data structure should support the following operations:

- Insert/delete a tab in a line.
- Determine if a position is the start/end of a tab.
- Merge the tabs of two lines.
- Split the tabs of a line into two at an arbitrary position.

To implement this data structure we have an array `vector<int> tabs`, where `tabs[i]` encodes whether the `i` tab stop has been tabbed, and if so how many trailing whitespaces it includes. Concretely, if the `i` tab stop has not been tabbed then `tabs[i] = 0`. Otherwise, `tabs[i] = # of trailing whitespaces`.

## `CSCREEN`

