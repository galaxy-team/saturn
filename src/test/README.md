# Tests

The `src/tests/` directory contains the source code for saturn's tests.
Before you attempt to install gtest, cmake must be available on your system path

## GTest

### Obtaining Source

#### Ubuntu

It relies on gtest, which, on ubuntu can be installed like this;
`sudo apt-get install libgtest-dev`
Then switch to the folder containing the source: `cd /usr/src/gtest`

#### Other
On other systems, unless you have a similar package in your distributions package manager,
you will need to download the source from Google Code like so:
`svn checkout http://googletest.googlecode.com/svn/trunk/ googletest`

### Installation

Once you have done so for your system, cd to the directory containing the source and run the following commands;

```shell
sudo cmake CMakeLists.txt
sudo make
 
# copy or symlink libgtest.a and libgtest_main.a to your /usr/lib folder
sudo cp *.a /usr/lib
```

Base instructions taken from: http://www.thebigblob.com/getting-started-with-google-test-on-ubuntu/
