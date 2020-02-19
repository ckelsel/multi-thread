rmdir /q /s build
md build
cd build
cmake ..
cmake --build .
ctest -VV -C debug
cd ..


