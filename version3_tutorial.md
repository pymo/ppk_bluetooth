# How to make the version 3 adapter (CH582F based)

If you don't want to make it yourself, you can skip this tutorial and buy the fully assembled adapter at https://www.tindie.com/products/26595/

Bill of Materials
----------
- [WCH Micro Kbd board](https://github.com/pymo/wch_micro_kbd/), you can buy it from my [Tindie store](https://www.tindie.com/products/35183/). You can also fabricate it yourself using the gerber file in [my other repo](https://github.com/pymo/wch_micro_kbd/tree/main/pcb/).
- 22 AWG single core wire for making the connector.
  - or, a [custom interface PCB](https://oshwhub.com/chen_xin_ming/palm-m500-connector) if you are making for Palm M500 keyboards. You can fabricate it yourself, or buy it from my [Tindie store](https://www.tindie.com/products/35183/).
- Some thin, flexible wire for the switch connection. I use 30 AWG PVDF insulated single core wire.
- 601230 Li-Po battery 200mAh (6mm x 12mm x 30mm in dimensions).
- One travel limit switch, model No. [KFC-V-204F or MS-V-204F](https://www.aliexpress.com/item/3256804072080898.html). Please use this exact model, because the enclosure is specifically designed around it.
- 3D printed enclosure (stl models are in the 3d_print/ folder).
- Two M2 screws, 4mm long.
- Double-sided tape, and super glue.

Schematic
----------

![Schematic](/images/version3/schematic.jpg "Schematic")

Some notes about the schematic:

- cy384 explained the keyboard's pinout and protocol in his [project page](http://www.cy384.com/projects/palm-keyboard.html) very well. And here is the official [Hardware and Electronics Reference](http://www.splorp.com/pdf/stowawayhwref.pdf) for the keyboard.

3D printed parts
----------------
See the stl files in 3d_print/ folder. It comes in 3 files: the upper cap, the middle frame and two board clips. The board clips need to be super-glued to the middle frame during the assembly.

I used Tinkercad to create these models. If you are interested in modifying the 3D models, you can visit this link: https://www.tinkercad.com/things/hjglHkgPCuu

![Printed parts before assembly](/images/version3/3d_print.jpg "Printed parts before assembly")

Assembly and wiring
-------------------

Solder two 4 cm lenth thin wires to the SW1 pads, and the other ends of wires to the travel limit switch.

![wiring pic](/images/version3/wiring2.jpg)

If you are making adapters for Palm III or V, or Handspring Visor:

- Cut the single core wire into five (for Palm III or V) or three (for Handspring Visor) 3.2 cm sections, leave 5mm of the insulation on them.

  - Palm III or V: solder the wires to pin [1] [2] [3] [4] [5].

  - Handspring Visor: solder the wires to pin [1] [5] [6].

![wiring pic](/images/version3/wiring1.jpg)

If you are making adapters for Palm M500:

- Use a 5 pin header, solder them to the custom PCB, remove the plastic from the pin header, trim them to 2mm long, put Kapton or electrical tape on the solder joints to insulate.

If you are making adapters for HP Jornada:

- Use a L-shape 5 pin header, trim it and solder it to the custom PCB, put electrical tape on the solder joints to insulate.


![wiring for Palm M500 keyboards](/images/version3/wiring5.jpg)

Solder the positive of the Li-Po battery to pad [BAT+], the negative to pad [BAT-].

Use a double-sided tape to glue the battery spacer to the back side of the board, then use a double-sided tape to glue the battery to the battery spacer.

![wiring pic](/images/version3/wiring3.jpg)

Put some super glue and install the switch inside the middle frame, glue it in-place.

![Glue the switch](/images/version3/glue_switch.jpg)

Insert the wires into the holes of the middle frame, make sure you are inserting them into the correct hole by cross-check with the schematic.

- Palm III or V: from left to right, pin [1] [2] [3] [4] [5] goes to hole 2, 3, 4, 7, 10

- Handspring: from left to right, pin [1] [5] [6] goes to hole 1, 5, 8

- Palm M500: plug the connector PCB into the middle frame, then put the mainboard & battery assembly on the middle frame, the pins should go through the corresponding pads on main board. Do not solder the pins to the mainboard yet.

- HP Jornada: screw the connector PCB into the middle frame, then put the mainboard & battery assembly on the middle frame, the pins should go through the corresponding pads on main board. Do not solder the pins to the mainboard yet.

Glue the two board clips to the middle frame, the board clips hold the board like this:

![wiring pic](/images/version3/middle_assembly.jpg)

- Palm III, V or Handspring: Trim the wires to suitable length, then bend the wires and fold them inside. This is what contacts the keyboard connector.

- Palm M500 or HP Jornada: solder the pins to the mainboard.

![wiring pic](/images/version3/wiring7.jpg)

Put the cap over the middle frame. Make sure it is the correct orientation (the USB type-C port should align with the opening on the cap).

Screw the 2 screws, and you are done!

![wiring pic](/images/version3/bottom.jpg)

Programming
-----------
Follow [this tutorial](https://github.com/pymo/wch_micro_kbd/blob/main/doc/wch_isp_tool.md) to flash the firmware. The firmware can be downloaded from the release page of that github. You need Windows PC to flash the firmware.

Features and quirks
---------------------
- As with any device using Li-Po battery, don't discharge the battery to a very low state. Please charge the battery as soon as you see the red LED blinks. Otherwise the battery health may be affected.

