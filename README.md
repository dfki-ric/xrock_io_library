# Software Name

The xrock_io_library is a plugin for the [xrock_gui](https://github.com/dfki-ric/xrock_gui_model) the facilitates the usage of Xtypes in the xrock_gui and the database access via xdbi.

xrock_io_library was initiated and is currently developed at the
[Robotics Innovation Center](http://robotik.dfki-bremen.de/en/startpage.html) of the [German Research Center for Artificial Intelligence (DFKI)](http://www.dfki.de) in Bremen, together with the [Robotics Group](http://www.informatik.uni-bremen.de/robotik/index_en.php) of the [University of Bremen](http://www.uni-bremen.de/en.html).

xrock_io_library has been funded by the DLR and the Federal Ministry for Economic Affairs and Climate Action. FKZ: 50RA2107 (DFKI), 50RA2108 (Uni Bremen).

## Getting Started

After installation you can simply open XRock-Gui and will find access to your XTypes database in the header bar.

## Requirements
- CMake >=3.10
- [X-Types-Generator](https://github.com/dfki-ric/xtypes_generator)
- [xrock_gui_model](https://github.com/dfki-ric/xrock_gui_model)
- [XDBI](https://github.com/dfki-ric/xdbi)
- python3-markdown
- [lib_manager](https://github.com/rock-simulatoin/lib_manager)
- [mars](https://github.com/rock-simulation/mars)

## Installation
Either via CMake:
```bash
mkdir build
cd build
cmake ..
make install
```

As soon as this package is integrated into one of our package_sets, you can of course also use autoproj's amake

## Bug Reports

To search for bugs or report them, please use GitHubs issue tracker at:

https://github.com/dfki-ric/xrock_io_library/issues

## Referencing

[//]: <> (TODO preferred way of referencing this software, e.g., use publication ...)

## Releases
No releases, yet.

[//]: <> (TODO release guidelines)

[//]: <> (TODO describe the versioning approach, for example:)

