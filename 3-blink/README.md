# Blink Example

Step by step example to show how work with the ESP32-C3, starting from a blink example.

Adapted from [the blink example from esp-idf](https://github.com/espressif/esp-idf/blob/97fb98a91b308d4f4db54d6dd1644117607e9692/examples/get-started/blink/).

## Useful reference links

* [ESP32-C3-DevKitM-1](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/hw-reference/esp32c3/user-guide-devkitm-1.html)
* [Documentation of the ESP32-flavoured CMake](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/build-system.html)
    * [idf_component_register](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/build-system.html#component-requirements)
    * [Kconfig](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-guides/build-system.html#example-component-cmakelists)
* [Further details of Kconfig](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/kconfig.html)
* [WS2812B](http://www.world-semi.com/Certifications/WS2812B.html): the RGB LED controller module
     * [Download the datasheet](http://www.world-semi.com/DownLoadFile/108)
* [RMT (Remote Control) module API reference](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/api-reference/peripherals/rmt.html): the RMT component is used by the `led_strip` component to communicate with the WS2812B, i.e. to control the RGB LEDs.

## Initial remarks

* The `led_strip` component in `components/` is the driver of WS2812B, i.e. the RGB LED controller that is present on the [ESP32-C3-DevKitM-1](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/hw-reference/esp32c3/user-guide-devkitm-1.html), which is the board that we're using. It is taken from https://github.com/espressif/esp-idf/blob/master/examples/common_components/led_strip.
* With the exception of the step 0 in `1-better_project_setup/`, all the steps can be built using the following commands.

    ```sh
    # Configure the project build system for ESP32C3.
    $ idf.py set-target "esp32c3"
    # Build the project
    $ idf.py build
    ```

    Alternatively, instead of `idf.py build`, you can use the following command to build the project, flash the built firmware on the ESP32C3 attached to your PC and open the serial port that you can use to communicate with the ESP32C3. You may need to pass an additional flag `-p <PORT>` to specify the port to which your ESP32C3 is connected.

    ```sh
    $ idf.py flash monitor
    ```
