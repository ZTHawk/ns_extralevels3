# Setting up Unix system for compilation (gcc)

Tested on debian 12 (bookworm).

## Install GCC (and other tools):

Run the following command to install all required tools:
```
apt update && apt install git gcc build-essential gcc-multilib g++-multilib
```

## Clone repositories

### Clone EL3 repository:
```
git clone https://github.com/ZTHawk/ns_extralevels3.git
```

### Clone metamod-p repository:

This is a fork but maintained (v1.21p44 as of 2026.04.05):
```
git clone https://github.com/APGRoboCop/metamod-p.git
```

or the original one (v1.21p38)
```
git clone https://github.com/Bots-United/metamod-p.git
```

## Compiling

Move into EL3 folder:
```
cd ns_extralevels3
```

### NS 3.3 (b9)

To create a debug build, run this command:
```
make
```

To create a release build, run this command:
```
make opt
```

### NS 3.2

To create a debug build, run this command:
```
make NS=3_2
```

To create a release build, run this command:
```
make opt NS=3_2
```