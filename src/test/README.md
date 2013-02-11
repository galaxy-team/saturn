The `src/tests/` directory contains the source code for saturn's tests.

It relies on gtest, which, on ubuntu can be installed like this;

`sudo apt-get install libgtest-dev`

Surprisingly, you must then compile it manually, like so;

```shell
sudo apt-get install cmake # install cmake
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
 
# copy or symlink libgtest.a and libgtest_main.a to your /usr/lib folder
sudo cp *.a /usr/lib
```

Instructions taken from: http://www.thebigblob.com/getting-started-with-google-test-on-ubuntu/
