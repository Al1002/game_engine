# Installing game engine

First, clone the project directory using `git clone git@github.com:Al1002/game_engine.git`.

### On linux

Install the dependencies by running `install_dependencies.sh`

Then, run `cmake_build_alias.sh` to run CMake and build the project.

This will generate a debian package called `game_engine-x.x-Linux.deb`, install it with `sudo dpkg -i`. 

You can see sample usage in `example_project`.

### On windows

The engine can be crosscompiled using MinGW and used as a MinGW Windows library.