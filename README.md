# unibz-76088A-materials

Lecture materials for Embedded Systems Design and Implementation (76088A) course at UniBZ.

Please also check out [FAQ.md](./FAQ.md), in which I will put some common problems and pitfalls that we meet during the course.

## Environment setup

### ESP32-C3 SDK

Some of the projects in this repository require the SDK (Software Development Kit) for ESP32-C3. Please follow the [Get Started Guide](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/get-started/index.html) from Espresif.

### Native compilation

Some of the projects in this repository require a GNU compilation toolchain (which contains programs such as `gcc`, `ld`, `ar`, `make`, etc.), as well as some other programs such as `git` and `cmake`.

#### Docker

For those of you who are familiar with Docker, we provide a Dockerfile in the root of this repository containing all the dependencies that are needed for this course.

#### For Windows users

If you use Windows, we suggest to use MSYS2. You can follow the [official installation guide](https://www.msys2.org/).
If everything went smoothly and you reached step 8, then, as step 8 tells,

> To start building using the mingw-w64 GCC, close this window and run "MSYS MinGW 64-bit" from Start menu. Now you can call make or gcc to build software for Windows.

you should have the GNU compilation toolchain installed in your "MSYS MinGW 64-bit" environment.

Then you just need to issue two additional commands:

```sh
$ pacman -S git
$ pacman -S cmake
```

to install Git and CMake. Now you're all set to go.
