# KlemmUI 3

A C++20 UI library (maybe even a framework?) for creating lightwheight cross platform GUI apps.

## Major changes from KlemmUI 2:

- SDL was removed as a dependency, all windowing code was rewritten.
- CMake now is the only build system.
- Added resource generator.
- More options for styling backgrounds.
- Better support for translation.


## Library features

- Support for creating GUI apps on Windows and Linux. (Currently only X11 is supported)
- A language for describing UI layouts that is compiled to C++ headers that can be included into regular code.
- A resource generator that can translate files into C code that can be used to embed files into applications.

