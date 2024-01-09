# Palm Portable Keyboard Bluetooth Adapter

This project is a DIY bluetooth adapter for the Palm Portable Keyboards (PPK), so you can use it with any phone, tablet or laptop that supports BLE (i.e. Bluetooth 4.0 or later). The cost of the materials are around 15~35 US dollars, depending on which version you build.

I've made a small batch of the version 2 adapter for sale. Here is the link to the product: https://www.tindie.com/products/26595/

Demo on Youtube:

[![Bluetooth Palm Portable Keyboard demo](http://img.youtube.com/vi/gXvH2a8Eoss/0.jpg)](https://www.youtube.com/watch?v=gXvH2a8Eoss "An Update on the Palm Portable Keyboard Bluetooth Adapter")

This project is inspired by [cy384](https://github.com/cy384/ppk_usb)'s USB PPK adapter, and [Christian](https://hackaday.io/project/181800-palm-pilot-keyboard-bluetooth-conversion)'s ESP32-based PPK bluetooth adapter. 

I've made two versions of the adapter: version 1 is based on an Adafruit Feather nRF52840 Express board, version 2 is based on a LilyGo-T-OI-PLUS ESP32-C3 board. Here are the differences of these two versions, you can choose which one to build. I personally recommend version 2, because it is easier to build and more convenient to charge.

|  version 1  |               version 2                |
| :-------: | :-----------------------------------------------------------------: |
| nRF52840 chip | ESP32-C3 chip |
| $25 for the main board | $5 for the main board |
| Micro-USB charge port | Type-C charge port |
| Only charges when connected to the keyboard | Charges independently |
| more power efficient | less power efficient, so needs a larger battery to achieve similar battery life to version 1|
| more components, harder to solder | less components, easier to solder |
| has a button to forget the pairing | no button, has to forget the pairing on the host side |
| no need to sleep | sleeps after idling for 30 minutes to save power, press any key to wake up |
| Supports Palm III and Palm V keyboards | Supports Palm III, Palm V, Palm M500, and Handspring Visor keyboards |

- Supported keyboard types
  - Palm III keyboard (p/n P10713U or 3C10317)
  - Palm V keyboard (p/n 3C10439)
  - Palm M500 keyboard (p/n P10802U)
  - Handspring Visor (Targus PA800 or PA800U)
- Unsupported keyboard types
  - connectors for HP Jornada, Compaq iPaq PocketPC, other PDA/phones, IrDA version.

Tutorials to build the adapter
-----------

[How to build the version 1 adapter](version1_tutorial.md)

[How to build the version 2 adapter](version2_tutorial.md).  [Manual of the version 2 adapter](docs/ppk_bluetooth_manual.pdf).


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

