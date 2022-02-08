# Palm Portable Keyboard Bluetooth Adapter

This project is a DIY bluetooth adapter for the Palm Portable Keyboards (PPK), so you can use it with any phone, tablet or laptop that has bluetooth. The cost of the materials are around 30 US dollars.

Demo on Youtube:

[![Bluetooth Palm Portable Keyboard demo](http://img.youtube.com/vi/o8ccRMmagCI/0.jpg)](http://www.youtube.com/watch?v=o8ccRMmagCI "Converting a Palm Portable Keyboard into a Bluetooth keyboard!
")

This project works with the Palm III keyboard (black plastic, p/n P10713U) and the Palm V keyboard (dark grey metal, p/n 3C10439). You need to print different middle frames for these two, but the circuit/firmware are the same. Other variants of this keyboard (connectors for Palm M500, Handspring Visor, HP Jornada, Compaq iPaq PocketPC etc.) are not supported.

This project is inspired by [cy384](https://github.com/cy384/ppk_usb)'s USB PPK adapter, and [Christian](https://hackaday.io/project/181800-palm-pilot-keyboard-bluetooth-conversion)'s ESP32-based PPK bluetooth adapter. It differs from the above two projects in that it uses an Adafruit Feather nRF52840 Express board, which has Bluetooth 5.0 with BLE support. Compared to existing solutions, this board has the following advantages:

- Very power efficient (compared to ESP32), so we can use a much smaller Li-Po battery and the final product is smaller in size.
- Easier to program and up-to-date bluetooth version (compared to ATmega16U4 + Bluetooth module)
- Has a built-in Li-Po charge circuit and a battery voltage reading pin.

The board is however slightly more expensive and larger than some ESP32 boards, so that can be a downside.

Bill of Materials
----------
- [Adafruit Feather nRF52840 Express](https://www.adafruit.com/product/4062) board
- About 26 AWG solid core wire (wires that used for breadboard are ideal, they have a tinned layer to prevent oxidize)
- Some thin, flexible wire for battery and RX connection.
- 502030 Li-Po battery 250mAh (30mm x 20mm x 5mm in dimensions)
- Three 10K ohm resistors (two SMD 0805 size, one axial)
- One NPN transistor in SMD package (I use 2N3904)
- One switch (I use a tiny travel limit switch, model No. [KFC-V-204F or MS-V-204F](https://www.aliexpress.com/i/32918743539.html), you can use any other switch, but you need to modify the enclosure)
- 3D printed enclosure (stl models are in the 3d_print/ folder).
- 4 screws with the length around 5mm and the outer diameter around 2mm. You can also use other size if you are willing to modify the enclosure (The thread density does not matter because it self-taps into the hole)

Schematic
----------
This solution uses the hardware serial port instead of software serial, so it's more power efficient (MCU can sleep more without constantly bit-banging the GPIO to emulate software serial). But the PPK speaks RS-232 instead of TTL, this means we need to solder some extra components onto the board to make a voltage level inverter.

![Schematic](/images/schematic.jpg "Schematic")

![Circuit](/images/circuit.jpg "Actual circuit")

Some notes about the schematic:

- cy384 explained the keyboard's pinout in his [project page](https://github.com/cy384/ppk_usb/blob/master/README.md) very well.
- Red dots are where the wires need to be soldered to the pad.
- Though wires are soldered to pad A0, this GPIO is not used in the software, it is only soldered there to provide structural support of the wire.
- A5 is the "GND" of the keyboard, we could have soldered it to the GND of the board, but then the position would be inconvenient. Instead we pull down A5 in software to act as GND.
- R2 is a pull down resistor for the RTS line, it is required as per the keyboard manual. I chose an axial resistor because its physical length allows it to conveniently "jump" between A0 and A5.
- I used a travel limit switch, because I can embed it inside the socket which connects to the keyboard, so that it will be on automatically when the adapter is plugged in. However, this means the adapter can only be charged when it is plugged to the keyboard. If you don't want this quirky feature, you can use a normal switch and modify the enclosure.

3D printed parts
----------------
See the stl files in 3d_print/ folder. It comes in three parts: the upper cap, the middle frame, and two buttons. The middle frame needs some support under the middle platform when printing.

![Printed parts before assembly](/images/3d_print.jpg "Printed parts before assembly")

I used [Tinkercad](https://www.tinkercad.com/) to create these models. If you are interested in modifying the 3D models, you can visit this link: https://www.tinkercad.com/things/cnyYmhGWcyD

Assembly and wiring
-------------------

Cut the single core wire into five 4cm sections, strip the insulation and solder it to pad [3V] [A0] [A1] [A3] [A5], respectively.

![wiring pic](/images/wiring1.jpg)

This is the most difficult step: place the transistor and R0, R1 on the board as shown below, and solder them according to the schematic. You have to place the component exactly like I did or it will not reach some pads. You can use thin strips of heat-resistant tape to help fix the components in place when soldering. Double check the connectivity of every junction with multi-meter to make sure there is no short nor disconnect.

![wiring pic](/images/wiring2.jpg)

Solder a thin wire from the R0-transistor connection to the [RX] pad.

Solder R2 between [A0] and [A5] on the top of the board. I would recommend putting a heat-shrink tube on the resistor to prevent shorting with the A1 and A3 pin.

![wiring pic](/images/wiring3.jpg)

Solder the positive of the Li-Po battery to pad [Bat], the negative to one end of the switch, and the other end of the switch connects to the GND pad of the board as shown in the schematic. Make sure the wires to the switch are at least 5cm long, otherwise it may cause trouble during assembly.

![wiring pic](/images/wiring4.jpg)

Manage the wire so it is cleanly tucked on the back side of the board. Then use a double-sided tape to glue the battery to the back side of the board.

![wiring pic](/images/wiring5.jpg)

Put some glue and install the switch inside the middle frame, glue it in-place.

![Glue the switch](/images/glue_switch.jpg)

Put around 4 mm of insulation on each of the wires, then insert the wires into the holes of the middle frame, make sure you are inserting them into the correct hole by cross-check with the schematic.

![wiring pic](/images/wiring6.jpg)

Bend the wire sticking out of the hole and fold it inside. This is what contacts the keyboard connector.

![wiring pic](/images/wiring7.jpg)

Here is what the entire middle frame assembly looks like:

![wiring pic](/images/middle_assembly.jpg)

Place the two buttons into the upper cap, then push the middle frame assembly into the cap. Make sure it is the correct orientation (the buttons should align with the buttons on the board).

![wiring pic](/images/upper_case.jpg)

Screw the 4 screws, and you are done!

![wiring pic](/images/screws.jpg)

Fully assembled unit:

![wiring pic](/images/front.jpg)

![wiring pic](/images/charge_port.jpg)

Programming
-----------
If you have never programmed the Adafruit Feather nRF52840 Express board before, please follow the official tutorial to try programming some basic examples.

https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather

Once you know how to program the nRF52840, open up `blehid_keyboard.ino`. Set the device to "Arduino Feather nRF52840 Express" in the IDE, plug in the Micro USB cable, and hit upload, wait for the programming to be done. Then you can test it on the keyboard!

Special Key mapping
-----------
- Fn+Tab for Esc
- Fn+number keys for F1-F10, Fn+- for F11, Fn+= for F12
- Fn+up for volume up, Fn+down for volume down
- Fn+left for brightness down, and Fn+right for brightness up.
- Fn+Cmd is Homepage key (Home button in iOS).
- Cmd is mapped to super (aka Windows/Apple key).
- Date is mapped to Home, Phone is mapped to End.
- To-Do is mapped to PageUp, Memo is mapped to PageDown.
- Done is mapped to Insert.

Adding to or modifying the mapping is straightforward, just edit the `config_keymap` and `config_fnkeymap` functions.

Features and quirks
---------------------
- The red LED blinks when the battery is below 60%. It double-blinks when the battery is below 40%, and triple-blinks when the battery is below 20%.
- Long press the "PAIR" button for 1 second to clear all the remembered devices on the adapter, then you can press the "RESET" button to start the pairing again.
- Press both the "RESET" and "PAIR" button and release both, the board enters DFU mode to allow force firmware update, this is useful if the current firmware is corrupted.
- Due to the way the switch is installed, the adapter can only be charged when it is plugged onto the keyboard.
- It can only connect to one device at a time. It auto connects to the last device it is paired to (if that device is in range). You can manually disconnect it in the last connected device's operating system, then you can connect it to another device.
- The stand on the PPK itself can be used to support a phone. My phone is heavy (an iPhone 12 Pro Max) but it seems to support it OK. A TPU/rubber phone case would be ideal to prevent sliding.

![Bluetooth Palm Portable Keyboard's stand supporting an iPhone](/images/ppk_demo_iphone.jpg "Bluetooth Palm Portable Keyboard working with a phone")

Possible improvements
---------------------

On some devices (my Thinkpad laptop with Debian, and Oneplus 5T, for example), the host receives key reports very slowly (almost as low as 10 reports per second). I have added a coalescing method in the software to alleviate it. But the reason of the slowness is still unkown. If you know why the slowness happens, please let me know by opening an issue.

It's possible to support USB HID, so it can be used with both USB and Bluetooth, and even switch on-the-fly by hot-keys.

Adding support for battery service is also possible.

