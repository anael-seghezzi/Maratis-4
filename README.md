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

Building (CMake)
----------------

(from Maratis-4 directory)

**Unix:**

    mkdir Build
    cd Build
    cmake -G "Unix Makefiles" ../ -DCMAKE_INSTALL_PREFIX=../bin -DCMAKE_BUILD_TYPE=Release
    make
    make install

**Windows:**

    mkdir Build
    cd Build
    cmake -G "Visual Studio 11" ../ -DCMAKE_INSTALL_PREFIX=../bin
  
**Options:**

SDL2 backend :

    -DUSE_SDL2=ON -DDIRECTX=OFF -DRENDER_D3D=OFF
  
Contact
-------

anael (a) maratis3d.com


Contributors
------------

Davide Bacchet<br>
Skumancer<br>
Sergey Pershin<br>
Mario Pišpek<br>
Nistur<br>
Skaiware<br>
Sponk<br>
Dahnielson<br>
Jurgel<br>
...
