# How to make the version 2 adapter (ESP32-C3 based)

Bill of Materials
----------
- [LilyGo-T-OI-PLUS ESP32-C3](https://www.aliexpress.com/item/1005003348936965.html) board ($5.23+shipping)
- About 26 AWG solid core wire (wires that used for breadboard are ideal, they have a tinned layer to prevent oxidize)
- Some thin, flexible wire for switch connection.
- 802040 Li-Po battery 700mAh (40mm x 20mm x 8mm in dimensions). (The enclosure is specifically designed for this battery size. If you want a larger battery, you may need to modify the enclosure.)
- One switch (I use a tiny travel limit switch, model No. [KFC-V-204F or MS-V-204F](https://www.aliexpress.com/i/32918743539.html), you can use any other switch, but you need to modify the enclosure)
- 3D printed enclosure (stl models are in the 3d_print/ folder).
- 2 screws with the length around 5mm and the outer diameter around 2mm. You can also use other size if you are willing to modify the enclosure (The thread density does not matter because it self-taps into the hole)

Schematic
----------
ESP32-C3's hardware serial port has built-in invertion function, and its GPIO has built-in pullup/pulldown, so it does not need any standalone components like the version 1 does.

![Schematic](/images/version2/schematic.jpg "Schematic")

Some notes about the schematic:

- cy384 explained the keyboard's pinout and protocol in his [project page](http://www.cy384.com/projects/palm-keyboard.html) very well. And here is the official [Hardware and Electronics Reference](http://www.splorp.com/pdf/stowawayhwref.pdf) for the keyboard.
- Red dots are where the wires need to be soldered to the pad.
- We directly use GPIO as the "VCC" and "GND" of the keyboard, this way we can easily reboot the keyboard. The keyboard uses less than 2.5mA, so it's OK.
- I used a travel limit switch, because I can embed it inside the socket which connects to the keyboard, so that it will be on automatically when the adapter is plugged in.

3D printed parts
----------------
See the stl files in 3d_print/ folder. It comes in 4 parts: the upper cap, the middle frame, a battery spacer and two board clips. The board clips need to be super-glued to the middle frame after the print. The battery spacer is to prevent the battery to be overheated by the ESP32-C3 chip (just a pre-caution, the board only get slightly warm during normal usage).

![Printed parts before assembly](/images/version2/3d_print.jpg "Printed parts before assembly")

I used [Tinkercad](https://www.tinkercad.com/) to create these models. If you are interested in modifying the 3D models, you can visit this link: https://www.tinkercad.com/things/cnyYmhGWcyD

Assembly and wiring
-------------------

Desolder the 4-pin connector and the power switch.

(optional) Desolder the resistor next to the red LED, solder a 2K ohm resistor between the [9] pad and the positive pin of the red LED. This would re-purpose the red LED to be used by the GPIO9, instead of always on.

![wiring pic](/images/version2/wiring1.jpg)

Cut the single core wire into five (for Palm III or V) or three (for Handspring Visor) 4cm sections, leave 8mm of the insulation.

Palm III or V: solder the wires to pin [4] [5] [6] [7] [10].

Handspring: solder the wires to pin [4] [6] [10].

![wiring pic](/images/version2/wiring2.jpg)

Solder two wires to the pad where the switch was, and the other ends of wires to the travel limit switch we provided.

![wiring pic](/images/version2/wiring3.jpg)

Solder the positive of the Li-Po battery to pad [+], the negative to the [GND] pad.

![wiring pic](/images/version2/wiring4.jpg)

Use a double-sided tape to glue the battery spacer to the back side of the board, then use a double-sided tape to glue the battery to the battery spacer.

![wiring pic](/images/version2/wiring5.jpg)

Put some glue and install the switch inside the middle frame, glue it in-place.

![Glue the switch](/images/version2/glue_switch.jpg)

Insert the wires into the holes of the middle frame, make sure you are inserting them into the correct hole by cross-check with the schematic.

![wiring pic](/images/version2/wiring6.jpg)

Trim and bend the wire, and fold them inside. This is what contacts the keyboard connector.

![wiring pic](/images/version2/wiring7.jpg)

Glue the two board clips to the middle frame, the board clips hold the board like this:

![wiring pic](/images/version2/middle_assembly.jpg)

Put the cap over the middle frame. Make sure it is the correct orientation (the USB type-C port should align with the opening on the cap).

![wiring pic](/images/version2/upper_case.jpg)

Screw the 2 screws, and you are done!

![wiring pic](/images/version2/screws.jpg)

Fully assembled unit:

![wiring pic](/images/version2/front.jpg)

![wiring pic](/images/version2/charge_port.jpg)

Programming
-----------
You need to install the library [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino), refer to the "Installation" section in its README.

If you are compiling for Handspring Visor keyboard, uncomment the line `#define HANDSPRING` in the file `version2_esp32c3.ino`. If you are compiling for Palm III or V, keep it commented out.

Avoid using a USB hub for programming the board, because it may not provide enough current. Use a USB port that is directly on the PC instead.

Once you know how to program the ESP32-C3 board, open up `version2_esp32c3.ino`, plug in the USB type-C cable, and hit upload, wait for the programming to be done. Then you can test it on the keyboard!

Features and quirks
---------------------
- The red LED blinks when the battery is below 20%. It double-blinks when the battery is below 10%, and triple-blinks when the battery is below 5%.
- It can only connect to one device at a time. It automatically connects to the last device it is paired to (if that device is in range). If you want to connect it to another device, you must manually disconnect it in the last connected device's operating system.
- The ADC on this board is not very accurate, change the `adc_correction ratio` in the code if needed.
