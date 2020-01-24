# Contributing Guide



## Guidelines

1. Follow the [code of conduct](#code-of-conduct) listed below.
2. Format your code before committing it.
3. Always separate declarations and implementations (if possible).
4. Do not commit build artifacts.
   -  Anything generated by `cmake`.
   -  Anything compiled by `make`.
5. Do not commit print-debugging statements.



## Code of Conduct

- No curse words or offensive slurs.
- General professionalism.



## Modules

The project is divided into relatively-independent modules, which are detected and built by our `cmake` build system.

### Ignoring Modules

You can use the `-DSKIP_MODULES='module1;module2'` CMake argument to disable building for certain modules.

### Creating Modules

A module is a directory in `modules` with a `CMakeLists.txt` file inside.
Build targets for a module are created using the `arepa_create_*` functions, and can be interacted with like standard cmake targets.

#### Static Libraries

Static libraries are defined using the `arepa_create_module(target_name module_prefix)` function. This function will create a CMake target that can be statically linked in other modules or executables.

| Argument        | Description                                                  |
| --------------- | ------------------------------------------------------------ |
| `target_name`   | The target name.                                             |
| `export_prefix` | The library's export prefix path.<br/>If this is set to "example", headers would be included as:<br/> `#include "example/MyHeader.hpp"` |

```cmake
include("${AREPA_CONFIG_CXX}")       # Include the build system functions.

arepa_create_module(my_module "mod") # Creates a static library module.
arepa_target_cxx(my_module)          # Configures the module to use C++.
```

#### Executables

Executes are defined using the `arepa_create_executable(target_name executable_name)` function. This will create an executable file.

| Argument          | Description                             |
| ----------------- | --------------------------------------- |
| `target_name`     | The target name.                        |
| `executable_name` | The output filename for the executable. |

```cmake
include("${AREPA_CONFIG_CXX}")   # Include the build system functions

arepa_create_executable(my_executable "executable")
arepa_target_cxx(my_executable)  # Configures the module to use C++
```

### Using Libraries

Various libraries can be easily included using simple `arepa_use_*` functions.

```cmake
arepa_use_threads(TARGET)     # pthreads
arepa_use_curses(TARGET)      # ncurses
arepa_use_boost(TARGET)       # Boost
arepa_use_boost_beast(TARGET) # Boost::Beast
```


