# Palm Portable Keyboard Bluetooth Adapter

This project is a DIY Bluetooth module for the Palm Portable Keyboards (PPK), so you can assemble it to your PPK and use it with any host that supports BLE (Bluetooth 4.0 and later). The cost of the materials are around 15~35 US dollars, depending on which version you build.

I've made a small batch of the version 3 adapter for sale. Here is the link to the product: https://www.tindie.com/products/26595/

Demo on Youtube:

[![Bluetooth Palm Portable Keyboard demo](http://img.youtube.com/vi/qPznzD5e45k/0.jpg)](https://www.youtube.com/watch?v=qPznzD5e45k "Version 3 of my Palm Portable Keyboard Bluetooth Adapter (CH582F based)")

This project is inspired by [cy384](https://github.com/cy384/ppk_usb)'s USB PPK adapter, and [Christian](https://hackaday.io/project/181800-palm-pilot-keyboard-bluetooth-conversion)'s ESP32-based PPK bluetooth adapter. 

I've made 3 versions of the adapter: version 1 is based on an Adafruit Feather nRF52840 Express board, version 2 is based on a LilyGo-T-OI-PLUS ESP32-C3 board. version 3 is based on my custom designed CH582F board. Here are the differences of these versions.

version 3 is the current one I sell in my Tindie store. If you want to DIY and don't want to buy my board, you can choose version 2. version 1 is really old and no longer supported.

| version | Build Tutorial | Highlights | 
| :-------: | :-----------------------------------------------------------------: | :------: |
| Version 1 | [Tutorial](version1_tutorial.md) | Based on an Adafruit Feather nRF52840 Express board. Rudimentary, no longer supported. Hard to make. Expensive. Micro USB charging. |
| Version 2 | [Tutorial](version2_tutorial.md) | Based on a LilyGo-T-OI-PLUS ESP32-C3 board. Easy to build, cheap, power hungry, no USB HID support. Type-C charging. |
| Version 3 | [Tutorial](version3_tutorial.md) | Based on my custom designed PCB using CH582F chip. Easier to build, cheap, power efficient, USB HID support. Multi-host support. Type-C charging. |

- Supported keyboard types
  - Palm III keyboard (p/n P10713U or 3C10317)
  - Palm V keyboard (p/n 3C10439 or Belkin F8E458)
  - Palm M500 keyboard (p/n P10802U or Belkin F8P3501)
  - Handspring Visor (Targus PA800 or PA800U)
- Work in progress
  - HP Jornada (Targus PA820)
  - Compaq iPaq PocketPC (Taugus PA840)
- Unsupported keyboard types
  - connectors for CLIÉ, Treo, other PDA/phones, IrDA version.

Special Key mapping
-----------
- Fn+Tab for Esc
- Fn+number keys for F1-F10, Fn+- for F11, Fn+= for F12
- Fn+up for volume up, Fn+down for volume down
- Fn+left for brightness down, and Fn+right for brightness up.
- Fn+Cmd is Homepage key (Home button in iOS and Android).
- Cmd is mapped to super (aka Windows/Apple key).
- Date is mapped to Home, Phone is mapped to End.
- To-Do is mapped to PageUp, Memo is mapped to PageDown.
- Done is mapped to Insert.

If you are using a different keyboard layout, adding to or modifying the mapping is straightforward, just edit the `config_keymap` and `config_fnkeymap` functions in the firmware source code.

Coverage
-----------
Here are some blog posts covering this project:
- [Palm Portable Keyboard Goes Wireless](https://hackaday.com/2022/02/08/palm-portable-keyboard-goes-wireless/) on Hackaday.
- [Upcycling a Palm Portable Keyboard Into a Bluetooth-Enabled Accessory](https://www.hackster.io/news/upcycling-a-palm-portable-keyboard-into-a-bluetooth-enabled-accessory-9ac6c659c1f3) on hackster.io.
- [A Palm Portable Keyboard Bluetooth Adapter](https://www.hackster.io/news/upcycling-a-palm-portable-keyboard-into-a-bluetooth-enabled-accessory-9ac6c659c1f3) on Adafruit blog.

![Bluetooth Palm Portable Keyboard's stand supporting an iPhone](/images/ppk_demo_iphone.jpg "Bluetooth Palm Portable Keyboard working with a phone")

