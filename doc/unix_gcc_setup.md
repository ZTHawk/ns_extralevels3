## Setting up Unix system for compilation (gcc)

### Install GCC:

Run the following command to install all required tools:  
`apt update && apt install gcc git build-essential gcc-multilib g++-multilib`

### Clone repositories (EL3 + metamod-p)

Clone EL3 repository:  
`git clone https://github.com/ZTHawk/ns_extralevels3.git`

Clone metamod-p repository:  
`git clone https://github.com/Bots-United/metamod-p.git`

### Compiling

To create a debug build run:  
`make`

To create a release build run:  
`make opt`