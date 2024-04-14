# ESP32 Serial Demo

Demo of serial protocols for use in evaluating oscilloscope serial decoders.

Protocols supported:

- I2C
- SPI
- UART

## Setup

Minimal setup is required to use with a scope. With the exception of I2C, no
wiring is required. Flash a dev board using ESP-IDF or PlatoformIO and attach scope
probes to the GPIO pins as documented below.

Optionally connect GPIO 4 to GPIO 6 (I2C SCA) and GPIO 5 to GPIO 7 (I2C SCK) if you
want to observe the controller sending data to a simulated peripheral. If this
isn't done, you will only be able to observe the controller initiating a write to
the device's address, but it won't transmit the payload.

## Signals

Payload:

All of the protocols write a raw 32bit integer followed by a block of lorem ipsum.
The integer is incremented with every packet. Note: the integer is sent little
endian, so you will see the first byte rapidly moving from 00 to FF, and then the
second byte increments, etc. You can use this to play with triggering on the data
of the first byte.

### I2C

The controller writes the payload described above on SCA. The peripheral does not
respond other than the I2C ack.

Pins

- SCA: GPIO 4
- SCL: GPIO 5

Signal parameters

- Clock frequency: 100KHz
- I2C address: 0x29
- Delay between packets: 10ms

### SPI

The controller writes the payload described above on MOSI. The peripheral does not
respond.

Pins:

- CS: GPIO 9
- CLK: GPIO 10
- MISO: GPIO 11
- MOSI: GPIO 12

Signal parameters:

- Clock frequency: 10MHz
- Delay between packets: 10ms

### UART

The controller writes the payload described above on TX. The peripheral does not
respond.

Pins:

- TX: GPIO 17
- RX: GPIO 16

Signal parameters:

- Baud rate: 115200
- Data bits: 8
- Parity: None
- Stop bits: 1
- Delay between packets: 1000ms

## Compiling

### ESP-IDF

Set the target and build.

```sh
# only need to set the target once
idf.py idf.py set-target esp32s3

idf.py build
idf.py flash -p <path to your device>
```

### PlatformIO

```~~
pio run -t upload
```

## Misc Notes

I developed this on an ESP32S3, but it should be easily portable to any
other ESP chip and dev board. As I just whipped this together to evaluate
a new scope, the pins and clock speeds are not configurable other than in the
source files directly. Maybe one day I'll add user input and turn it into a real
scope demo board.

If you are a nix user, there is a flake.nix and .envrc that will install the
version of esp-idf and platformio I used during development.

The lorem ipsum text is lengthy in order to test signal decoding of packets that
don't fully fit on the screen of the scope. Annoyingly, there are some scopes that
will only decode from what's currently shown on the screen despite having the full
packet in memory.
