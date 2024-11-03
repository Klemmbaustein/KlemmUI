# KlemmUI 3

A C++20 UI library (maybe even a framework?) for creating lightweight cross platform GUI apps.

## Major changes from KlemmUI 2:

- SDL removed as a dependency, all windowing code is rewritten.
- CMake now is the only build system.
- Added resource generator.
- More options for styling backgrounds.
- Better support for translation.
- Added UIBlurBackground and UISpinner elements.
- Improved how the UITextField element works.
- Changed the way the resource generator and UI language are used in CMake to better work with
  the Ninja generator.


## Library features

- Support for creating GUI apps on Windows and Linux. (Currently only X11 is supported by default)
- A language for describing UI layouts that is compiled to C++ headers that can be included into regular code.
- A resource generator that can translate files into C code that can be used to embed files into applications.

