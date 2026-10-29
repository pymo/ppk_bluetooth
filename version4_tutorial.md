# How to install the version 4 adapter (internal mod)

Bill of Materials
----------
- PCB main board, you can buy it from my [Tindie store](https://www.tindie.com/products/35183/).
- 401025 Li-Po battery 90mAh (4mm x 10mm x 25mm in dimensions).
- 3M 9703 Z-axis double-sided tape for flex-to-PCB connection.
- Double-sided tape for battery fixation.
- 3D printed enclosure (stl models are in the 3d_print/ folder).

![BOM](/images/version4/bom.jpg)

Tools Needed
----------
- precision philips screw driver
- utility knife

Assembly Process
-------------------

Solder the battery to the PCB. (If you bought from me, this step is already done.)

There is a product label at the back side of the keyboard. It is a metal sheet. Use the utility knife to lift open the corner, then peel it off. Do not bend it too much, we need to install it back later.

![label removal](/images/version4/label_removal.jpg)

There is one screw under the label, and two on the other side, remove the 3 screws.

![screw removal](/images/version4/screw_removal.jpg)

Remove the plastic cap, you can see the original PCB and the keyboard flex cables.

![PCB removal](/images/version4/PCB_removal.jpg)

Hold the PCB, rotate it downwards to peel flex cables off of it. Be gentle, do not use excess force and tear the cables.

There are some glue residue on the back side of the flex cable, use your finger nail to gentlely scrape them off. Leave a clean surface under the flex.

![glue removal](/images/version4/glue_removal.jpg)

Remove one side of the film on the Z-axis double sided tape, then place it on the golder contacts of the new PCB. Be careful not to scramble the tape during the process.

![zaxis tape install](/images/version4/zaxis_install.jpg)

Place the PCB into the keyboard, remove the other side of protection film on the Z-axis double sided tape. Carefully align the flex cable's trace with the PCB's trace, and glue them down. The trace must be aligned.

![flex cable install](/images/version4/flex_install.jpg)

Use your finger nail to gentlely press on the top of the flex cable, so the glue can make good contact with the PCB.

Use the double-sided tape to glue the battery on the keyboard.

![battery install](/images/version4/battery_install.jpg)

Place the 3-D printed enclosure onto the PCB. Screw the 3 screws.

![enclosure install](/images/version4/enclosure_install.jpg)

Test the keyboard by pairing it to a phone or PC, and test all the keys to make sure they are working. One common failure is that the Z-axis tape did not get enough clamping force, some traces are not contacting properly, so some keys are not working. Try to press on the enclosure's bottom part to get a better contact.

If everything works well, install the metal label back. And you are done!

![enclosure install](/images/version4/label_install.jpg)

Updating Firmware
-----------
The PCB comes with firmware programmed. If you wish to customize some key mappings, you can recompile the firmware and follow [this tutorial](https://github.com/pymo/wch_micro_kbd/blob/main/doc/wch_isp_tool.md) to flash the firmware. You need a Windows PC to flash the firmware.

Warning
---------------------
- Do not pierce, short or damage the Li-Po battery, otherwise it will catch fire. Don't discharge the battery to a very low state. Please charge the battery as soon as you see the red LED blinks. Otherwise the battery health may be affected.

