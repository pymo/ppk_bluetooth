# How to make the version 2 adapter (ESP32-C3 based)

Bill of Materials
----------
- [WCH Micro Kbd board](https://) board (either fabricate it yourself using the gerber file, or buy one from Tindie.)
- 22 AWG single core wire for making the connector.
  - or, a [custom interface PCB](https://oshwhub.com/chen_xin_ming/palm-m500-connector) if you are making for Palm M500 keyboards. You can fabricate it yourself, or buy it from my [Tindie store](https://www.tindie.com/products/35183/).
- Some thin, flexible wire for the switch connection. I use 30 AWG PVDF insulated single core wire.
- 802040 Li-Po battery 700mAh (40mm x 20mm x 8mm in dimensions).
- One travel limit switch, model No. [KFC-V-204F or MS-V-204F](https://www.aliexpress.com/item/3256804072080898.html). Please use this exact model, because the enclosure is specifically designed around it.
- 3D printed enclosure (stl models are in the 3d_print/ folder).
- 2 screws with the length around 5mm and the outer diameter around 2mm. You can also use other size if you are willing to modify the enclosure (The thread density does not matter because it self-taps into the hole)
- Double-sided tape, and super glue.

Schematic
----------
ESP32-C3's hardware serial port has built-in invertion function, and its GPIO has built-in pullup/pulldown, so it does not need any standalone components like the version 1 does.

![Schematic](/images/version2/schematic.jpg "Schematic")

Some notes about the schematic:

- cy384 explained the keyboard's pinout and protocol in his [project page](http://www.cy384.com/projects/palm-keyboard.html) very well. And here is the official [Hardware and Electronics Reference](http://www.splorp.com/pdf/stowawayhwref.pdf) for the keyboard.
- We directly use GPIO as the "VCC" and "GND" of the keyboard, this way we can easily reboot the keyboard. The keyboard uses less than 2.5mA, so it's OK.
- I used a travel limit switch, and embeded it inside the socket which connects to the keyboard, so that it will be automatically on when the adapter is plugged in.

3D printed parts
----------------
See the stl files in 3d_print/ folder. It comes in 3 files: the upper cap, the middle frame and two board clips. The board clips need to be super-glued to the middle frame during the assembly.

I used Tinkercad to create these models. If you are interested in modifying the 3D models, you can visit this link: https://www.tinkercad.com/things/iDDUoFQ8ppK

![Printed parts before assembly](/images/version2/3d_print.jpg "Printed parts before assembly")

Assembly and wiring
-------------------

Solder two 5 cm lenth thin wires to the pad where the switch was, and the other ends of wires to the travel limit switch.

![wiring pic](/images/version2/wiring2.jpg)

If you are making adapters for Palm III or V, or Handspring Visor:

- Cut the single core wire into five (for Palm III or V) or three (for Handspring Visor) 3.2 cm sections, leave 6mm of the insulation on them.

  - Palm III or V: solder the wires to pin [4] [5] [6] [7] [10].

  - Handspring Visor: solder the wires to pin [4] [6] [10].

If you are making adapters for Palm M500:

- Use a 5 pin header, solder them to the custom PCB, insert the PCB into the middle frame. Do not solder the pins to the main board yet.

![wiring for Palm M500 keyboards](/images/version2/wiring5.jpg)

Solder the positive of the Li-Po battery to pad [+], the negative to the [GND] pad.

Use a double-sided tape to glue the battery spacer to the back side of the board, then use a double-sided tape to glue the battery to the battery spacer.

![wiring pic](/images/version2/wiring3.jpg)

Put some super glue and install the switch inside the middle frame, glue it in-place.

![Glue the switch](/images/version2/glue_switch.jpg)

Insert the wires into the holes of the middle frame, make sure you are inserting them into the correct hole by cross-check with the schematic.

- Palm III or V: from left to right, pin [4] [5] [6] [7] [10] goes to hole 2, 3, 4, 7, 10

- Handspring: from left to right, pin [4] [6] [10] goes to hole 1, 5, 8

- Palm M500: put the mainboard+battery assembly on the middle frame, the pins should go through the corresponding pads on main board.

![wiring pic](/images/version2/wiring6.jpg)

Glue the two board clips to the middle frame, the board clips hold the board like this:

![wiring pic](/images/version2/middle_assembly.jpg)

Trim the wires to suitable length, then bend the wires and fold them inside. This is what contacts the keyboard connector.

![wiring pic](/images/version2/wiring7.jpg)

If you are making adapters for Palm M500, now solder the pins to the main board, and trim them flat.


Put the cap over the middle frame. Make sure it is the correct orientation (the USB type-C port should align with the opening on the cap).

Screw the 2 screws, and you are done!

![wiring pic](/images/version2/bottom.jpg)

Programming
-----------
Follow [this tutorial](https://github.com/pymo/wch_micro_kbd/blob/main/doc/wch_isp_tool.md) to flash the firmware. The firmware can be downloaded from the release page of that github. You need Windows PC to flash the firmware.

LED Functionality
---------------------
- Green + Red LED constant on: The adapter is handshaking with the keyboard.
  - If it is stuck at this state for more than 3 seconds, it means the adapter is not communicating correctly with the keyboard. Please consider the following possibilities:
    - The adapter is not seated properly, please re-seat the adapter, or check the contacts visually.
    - The keyboard may be broken and not responding to the handshake.
    - The adapter may be low on battery, try to charge it for a while.
- Green LED fast blink: Bluetooth pairing mode.
- Green LED slow blink: Paired to the host device, normal usage.
- Blue LED on: charging battery.
- Blue LED off: charge complete.
- Red LED one blink: battery is below 20%.
- Red LED dual blink: battery is below 15%.
- Red LED triple blink: battery is below 10%.
  - Please recharge as soon as Red LED starts blinking (<20%), to maintain the battery health.

Features and quirks
---------------------
- As with any device using Li-Po battery, don't discharge the battery to a very low state. Please charge the battery as soon as you see the red LED blinks. Otherwise the battery health may be affected.

