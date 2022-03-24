# FAQ

## Environment

### Windows

#### It doesn't build when it should

1. The first thing to try is to build the hello world example, which you can find in `<esp-idf>/esp/esp-idf/examples/get-started/hello_world/`, where `<esp-idf>` is the path where you installed the SDK.
2. If the `hello_world` example builds fine, then we can be reasonably sure that at least your SDK is properly installed.
3. [The SDK documentation](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32c3/get-started/windows-setup.html#introduction) warns you that the SDK's installation path must not contain spaces, special characters, etc. However it doesn't warn you that **neither the path to your project should contains spaces, special characters, etc.**.
