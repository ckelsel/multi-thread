REM  rmdir /q /s build
md build
cd build
cmake ..
cmake --build .
if errorlevel 1 (
    goto end
)
ctest -VV -C debug
:end
cd ..


