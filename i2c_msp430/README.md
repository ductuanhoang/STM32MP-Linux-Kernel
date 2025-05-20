I2C MPS430 Application
=================================

The I2C connections for this example depend on the running platform:
 - **ConnectCore MP15 DVK**: MikroBus connector (J31).
   - VCC: Pin 7
   - GND: Pin 8
   - I2C-6 SDA: Pin 6
   - I2C-6 SCL: Pin 5

Running the application
-----------------------
Once the binary is in the target, launch the application:

```
~# ./i2c_msp430
Usage: ./i2c_msp430 [-a address] [-d data] [-l len] [-w (write)] [-r (read)]
Example:
Read: [-a address] [-r] [-l len] 
./i2c_msp430 -a 0x20 -r -l 10
Write: [-a address] [-w] [-d data][-l len]
./i2c_msp430 -a 0x20 -w -d DEAD -l 2

Aliases for I2C can be configured in the library config file
```
If no arguments are provided, the example will use the default values:
 - For the interfaces, default values are configured in `/etc/libdigiapix.conf`.
 - Specific application default values are defined in the main file.

Compiling the application
-------------------------
This example can be compiled using a Digi Embedded Yocto based toolchain. Make
sure to source the corresponding toolchain of the platform you are using,
for example, for ConnectCore MP15 DVK:

```
~$ . /opt/dey/4.0-r5/ccmp15-dvk/environment-setup-cortexa7t2hf-neon-vfpv4-dey-linux-gnueabi
~$ make
```