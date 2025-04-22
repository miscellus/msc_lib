@echo off
mkdir build
cd build
rem cl /Od /Fe:test.exe /Zi /nologo ..\test_msc_ht.cpp

cl /Od /Fe:test.exe /Zi /nologo ..\test_msc_arena.cpp
