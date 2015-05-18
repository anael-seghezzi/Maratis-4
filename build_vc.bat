mkdir BuildVC

cd BuildVC
cmake -G "Visual Studio 11" ../ -DCMAKE_INSTALL_PREFIX=../bin

pause