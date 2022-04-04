# Step: base

Goal: from the example copied from the master branch of ESP-IDF, we:

1. fix the CMake so that it builds;
2. remove what doesn't concern us, to make things easier;
3. try to improve a few CMake related things;
4. so that we finally have a good base project that can be used for the next steps.

## 0

We start by copying the example project from [esp-idf](https://github.com/espressif/esp-idf/blob/97fb98a91b308d4f4db54d6dd1644117607e9692/examples/get-started/blink/).
Note that the example is available only in the master branch of esp-idf (commit 97fb98a91b308d4f4db54d6dd1644117607e9692 at the time of writing). In this course we're using the version v4.4 of esp-idf, which doesn't contain this example.

If we try to build we get

```sh
$ idf.py set-target "esp32c3"
$ idf.py build
# Result ../main/blink_example_main.c:14:10: fatal error: led_strip.h: No such file or directory
# #include "led_strip.h"
#         ^~~~~~~~~~~~~
```

* We already know this error from [2-basics/1-hello_world](https://github.com/chenlijun99/unibz-76088A-materials/blob/main/2-basics/1-hello_world/README.md#a-invocation-of-the-compiler-may-produce-errors-from-different-sources) and from [2-basics/2-modularity/2-header_file](https://github.com/chenlijun99/unibz-76088A-materials/blob/main/2-basics/2-modularity/2-header_file/README.md#6). The preprocessor cannot find the header file. We must tell it about additional include paths. Furthermore, since we're using the functions declared in this header file, we must also include the source files that implement the declared functions into the build, otherwise we would have a linker error "undefined reference to whatever we use".
* We also know that that idf.py under the hood invokes CMake and in fact we can also see that in our project there is a top level `CMakeLists.txt` file.
* We know from [2-basics/2-modularity/3-cmake](https://github.com/chenlijun99/unibz-76088A-materials/blob/main/2-basics/2-modularity/3-cmake/README.md#2) that in CMake typically modules are created as static libraries targets using `add_library` and each static library targets contains its own usage requirements: include paths, compiler macro definitions, etc and whether these must be propagated to the targets who link them.
* So, probably we should find a static library target named presumably `led_strip` in some `CMakeLists.txt` file in ESP-IDF and then we should use `add_subdirectory` to include that `CMakeLists.txt`. Right? Nope. In "normal CMake" we would do that, but ESP-IDF provides a higher abstraction over vanilla CMake.

**Takeaways**:

* "ESP32 flavored CMake": see [Documentation of the ESP32-flavoured CMake](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/build-system.html)
* In short:
    * By using `include($ENV{IDF_PATH}/tools/cmake/project.cmake)` in the top-level CMakeLists.txt, we introduce ESP32 flavored CMake.
    * It "hijacks" the built-in CMake `project()` command with its own `project()` command, in which all the magic happens. Basically, instead of manually creating the project hierarchy and modularity using `add_subdirectory`, ESP32 flavored CMake does that automatically for us by searching in component search directories and including components as necessary.
    * It introduces the concept of "component"
        * A component is basically a static library target.
        * Each component resides in its own directory and contains a CMakeLists.txt file in which the function [idf_component_register](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/build-system.html#component-requirements) must be used to specify all the details (and usage requirements) about the component.
        * Components are searched in component search directories. Are notable one is `<esp-idf-root>/components`, in which all the components from the SDK are included.

### 1

**CHANGELOG**:

* We fixed `EXTRA_COMPONENT_DIRS` so that it finds our `led_strip` component.
* We optimize the build, by including in the compilation only the main component. All the required components will be recursively included by "ESP32 flavored CMake".
* In the `main` component's `CMakeLists`, in `idf_component_register` we specify that the `main` component depends on what we use in `blink_example_main.c`, i.e. `freertos`, esp32c3 hardware drivers as well as `led_strip`.
* We removed some useless files
* We formatted `blink_example_main.c` using our opinionated `clang-format` configuration.

### 2

We know from [2-basics/2-modularity/3-cmake](https://github.com/chenlijun99/unibz-76088A-materials/blob/main/2-basics/2-modularity/3-cmake/README.md#1) that with CMake we can use cache variables to customize our build. As the problem of "customization points" becomes large, it is unwieldy to pass all those CACHE variables by hand on the command line. That's why ESP-IDF introduced the Kconfig mechanism.

You can use this command to access the Kconfig menu and configure the myriad build configurations from ESP-IDf and all the components.

```sh
$ idf.py menuconfig
```

**Takeaways:**:

* Use `idf.py menuconfig` to access the configuration menu and configure the project build
* Each component can specify a [Kconfig](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/build-system.html#example-component-cmakelists) file in which component customization points are specified. These customization points appear then in the Kconfig menu.
    * Using `Kconfig.projbuild`, a component can specify project-wide configuration.
* When building the project, `idf.py` generates (among many other things) the files `sdkconfig` and `<cmake_build_directory>/config/sdkconfig.h`.
    * The first represents the final combined project build configuration.
    * The second contains the same information, but all the configuration items are specified as macros, so that in C we can include `sdkconfig.h` and perform conditional compilation based on the project build configuration.  
        * As an example, in our `blink_example_main.c` we include `sdkconfig.h` and based on whether `CONFIG_BLINK_LED_RMT` or `CONFIG_BLINK_LED_GPIO` is defined, we conditionally compile the LED initialization and blink function.
            * In our case our on-board LED cannot be controlled by a simple GPIO. You can also see [in the code](https://github.com/chenlijun99/unibz-76088A-materials/blob/main/3-blink/1-better_project_setup/2/main/Kconfig.projbuild#L5-L6) that the default config is to used BLINK_LED_RMT, unless we are targeting ESP32. Since we're targeting ESP32C3, we can expect and thus CONFIG_BLINK_LED_RMT to be defined in `sdkconfig.h`.

The only problem: the annoying sdkconfig that is generated in our project root. As always, since it is generated we don't want to track it in Git. But if we had multiple build directories (as we saw in [2-basics/2-modularity/3-cmake](https://github.com/chenlijun99/unibz-76088A-materials/blob/main/2-basics/2-modularity/3-cmake/README.md#2), the usefulness of out-of-source build), the fact that each build writes to the same sdkconfig file in the project root causes conflict between these builds (sdkconfig is overwritten each time we switch build directory).

**CHANGELOG**:

* We set the output path of `sdkconfig` to be inside `${CMAKE_BINARY_DIR}`, which is a built-in variable from CMake whose value is the build directory.
* We also removed the conditionally compiled "normal LED" code that doesn't concern us.
