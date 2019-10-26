## Boost Logger

### A Boost.Log Test Project

This repository serves as a testing ground for Boost.Log prototyping. 

The project is configured with CMake, and built with Visual Studio 2019. This project also makes extensive use of the Boost C++ libraries.

To configure and build this project, install CMake, then run the following from the command line.

    cd BoostLogger && mkdir build && cd build && cmake ..
	
If CMake is not able to find your local installation of Boost, run this instead:

    cd BoostLogger && mkdir build && cd build && cmake -DBOOST_ROOT=/path/to/boost/root ..
	
Then, you can open the generated MS Visual Studio solution and build the project.


