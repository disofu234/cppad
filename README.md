# Overview

CPPAD is a simple command-line text editor like nano.  It is built with C++, CMake, and NCurses (to interface with the command-line). GoogleTest is also used for unit-testing.

To run the project first make sure [Docker](https://www.docker.com/) is installed. Then, on the project's directory build the docker image and run the shell:

```
docker build -t cppad .
docker run -it --rm cppad bash
```

Once the shell is running you can run the editor:

```
cd build
./cppad file
```

