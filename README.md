# Overview

CPPAD is a simple command-line text editor like nano.  It is built with C++, CMake, and NCurses (to interface with the command-line). GoogleTest is also used for unit-testing.

To run the project first make sure [Docker](https://www.docker.com/) is installed. Then, on the project's directory build the docker image and run the shell:

```
docker build -t cppad .
docker run --rm -it -v "${PWD}:/app" -w /app cppad
```

Once the shell is running you can run CPPAD:

```
cd build
./cppad file
```

The program expects a single argument `filename`, which is either the name of the file to be edited in the current directory, or a path to a file in some other directory. If the file doesn't exist, it will be created.

# Design

The text data structure is `CONTENT`, which is a list of `LINE`s. A `LINE` consists of a list of characters and a data structure `TABS` to track the tabs in each line. A `SCREEN` object is initialized with the `CONTENT` object and the dimensions of the screen and it exposes the string to be printed on the screen. `NCurses` is used to actually print the string on the terminal.

A `CURSOR` is initialized with the `SCREEN` and `CONTENT` object and it acts as interlocutor between two objects within it: `CONTENT_CURSOR` and `SCREEN_CURSOR`. The `SCREEN_CURSOR` tracks the position of the cursor on the screen. The `CONTENT_CURSOR` tracks the position of the cursor in the `CONTENT` object. The `CURSOR` exposes methods to move the cursor and edit the text, which it translates into calls to the `CONTENT_CURSOR` and `SCREEN_CURSOR`. The `CURSOR` object is initialized to be at the top-left corner of the screen.

## `CONTENT`

The `CONTENT` object stores the text that is being edited and handles modifications of that text. Text lends itself naturally to a contiguous data structure, so the choice is between a linked-list and an array. Since we have to support moving the cursor backward and forward as well as editing at any cursor position, a doubly linked-list makes the most sense. Using an array would mean that each edit could be O(# of chars) in the worst case. We use the standard library `std::list` implementation of a doubly linked-list.

It also makes sense to have a separate linked-list for each line of the text, and for each of these linked-lists to be stored in a doubly linked-list to allow fast movement of the cursor across lines. Thus, the text being edited is stored in an object of type `std::list<LINE>`.

The `LINE` object stores a single line of text. It consists of a `std::list<char>` to store the characters in the line, and a `TABS` object to track the tabs in the line.

### Tabs

Tracking tabs in text calls for a specialized data structure because of the way tab characters work. A tab doesn't always correspond to a fixed number of spaces, but rather advances the cursor to the next tab stop. Thus, the number of spaces a tab corresponds to depends on where it is in the line. For example, if the tab stops are at multiples of 8 spaces, then a tab at position 0 corresponds to 8 spaces, while a tab at position 5 corresponds to 3 spaces.

`TABS` is the data structure we use to track tabs in a line. It is a list where each element corresponds to a tab, except for the last element which corresponds to the characters after the last tab. Each element keeps track of how many characters are between it and the previous tab, and how many spaces the tab takes up. The number of characters between it and previous tab needs to tracked since the number of spaces will be: `tab_width - (chars_after_prev_tab % tab_width)`.

To quickly modify the `TABS` object when a character is inserted or deleted, `CURSOR` keeps track of the position of the cursor in the `TABS` object as well. This allows for O(1) insertion and deletion of tabs and other characters.

## `CSCREEN`