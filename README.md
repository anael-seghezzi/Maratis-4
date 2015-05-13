Maratis 4
=========

Maratis is a simple cross-platform 3d engine and visual game development tool

Warning: This repository is experimental and in-progress.<br>
To get the last stable version of Maratis (version 3.x) please visit http://maratis3d.org


License
-------

Maratis is free and open source.

- Maratis Engine is licensed under the zlib/libpng License
- Maratis Editor is licensed under the GNU General Public License

Dependencies
----------------

https://github.com/anael-seghezzi/Maratis-Tiny-C-library

Getting source
--------------

    git clone https://github.com/anael-seghezzi/Maratis-Tiny-C-library.git
    git clone https://github.com/anael-seghezzi/Maratis-4.git

Building
--------

**Unix:**

run:

    build_unix.sh
    
or type:

    mkdir Build
    cd Build
    cmake -G "Unix Makefiles" ../ -DCMAKE_INSTALL_PREFIX=../bin
    make
    make install

**Windows:**

    mkdir Build
    cd Build
    cmake -G "Visual Studio 11" ../ -DCMAKE_INSTALL_PREFIX=../bin

or:

    mkdir Build
    cd Build
    cmake -G "MinGW Makefiles" ../ -DCMAKE_INSTALL_PREFIX=../bin
    make
    make install
  
Contact
-------

Anaël Seghezzi
anael (a) maratis3d.com


Contributors
------------

Davide Bacchet
Skumancer
Sergey Pershin
Mario Pišpek
Nistur
Skaiware
Sponk
Dahnielson
Jurgel
...