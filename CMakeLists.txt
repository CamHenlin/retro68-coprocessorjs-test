# HOW TO BUILD USING CMAKE as a standalone project:
# mkdir build
# cd build
# cmake .. -DCMAKE_TOOLCHAIN_FILE=path/to/Retro68-build/toolchain/cmake/retro68.toolchain.cmake
# make

cmake_minimum_required(VERSION 2.8)

project (coprocessor_js_test)

add_application(CoprocessorJSTest
    output_js.h
    coprocessorjs.h
    coprocessorjs.c
	hello.c
	CONSOLE
   )
