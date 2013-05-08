saturn
======

Galaxy's emulator.

Saturn requires SFML for much of its external interaction.
In the Ubuntu default repo's for apt-get, only the 1.6 version is available.
You can add a Launchpad PPA, however, if you still wish to use apt-get.

First do;
`sudo apt-add-repository -y \"deb http://ppa.launchpad.net/sonkun/sfml-stable/ubuntu raring main\"`
thence
`sudo apt-get update`
and finally
`sudo apt-get install libsfml-dev`

Saturn also relies on a compiler compatible with C++11

