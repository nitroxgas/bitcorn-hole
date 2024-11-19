![image](https://github.com/user-attachments/assets/6203ecbb-de3d-4308-81ec-74ea63075224)

# Bitcorn Hole 🌽🕳️

**Bitcorn Hole** is a physical hardware game where players earn bitcoin every time they score a point! Bean bags are tossed into the cornhole set and detected using microcontrollers with time-of-flight sensors or load cells(a scale). Each point scored triggers an **instant** payment of sats to the player’s Lightning Address! ⚡

## Live Demo
See how the game works in action [here](https://x.com/D_plus__plus/status/1807889900093821104) and [here](https://x.com/D_plus__plus/status/1845504750739218734).

## What You’ll Need

To build your own Bitcorn Hole setup, you’ll need the following:

Set with a ToF sensor:
- [Cornhole set](https://www.amazon.com/dp/B07S1PYYTV) — comes with two boards
- 2x [Wifi-enabled microcontrollers](https://www.amazon.com/dp/B0C8V88Z9D) —
  I used the Arduino Uno R4, but you could refactor the code to work with ESP32
- 2x [Time-of-flight (ToF) sensors](https://a.co/d/5bQHzve)
- [Jumper wires](https://www.amazon.com/California-JOS-Breadboard-Optional-Multicolored/dp/B0BRTJQZRD) and USB-C cables
- [Battery packs](https://www.amazon.com/gp/product/B08LH26PFT) or AC adapters
- [DIY enclosures](ENCLOSURES.md) —
  These are placed under each cornhole board to house the sensors and detect bean bags
- A computer with a screen and speakers to run the game

Massive thanks to [Dani Go](https://www.youtube.com/@bitcoineando) for her soldering help!

Set with scales, or load cell sensor:
- [Cornhole set](https://www.amazon.com/dp/B07S1PYYTV) — comes with two boards
- 2x [Wifi-enabled microcontrollers](https://pt.aliexpress.com/item/1005006389637966.html?channel=twinner) —
  I used the ESP32 38PINS Board with an expansion board.
- 2x [Load cells sensors, kit with HX711 module and disk suport, 10kg](https://s.click.aliexpress.com/e/_m0FvR6F).
- **Or** 2x [Kitchen Scales (sensor 5-10kg, you could use the plastic structure)](https://s.click.aliexpress.com/e/_m00y0y3) with 2x [HX711 Modules](https://s.click.aliexpress.com/e/_mNYNK8j)
- [Jumper wires (buy some female-female and female-male)](https://www.amazon.com/California-JOS-Breadboard-Optional-Multicolored/dp/B0BRTJQZRD) and USB-C cables
- [Battery packs](https://www.amazon.com/gp/product/B08LH26PFT) or AC adapters
- [DIY enclosures](ENCLOSURES.md) —
  These are placed under each cornhole board to house the sensors and detect bean bags
- A computer with a screen and speakers to run the game
- Optional, but a great addition - LED Bars to display the points in front of the set:
- [10 LEDs per set - Individually Addressable LED strip - WS2812b 5v 30/m](https://s.click.aliexpress.com/e/_msIFqu7) - 3 wires - 5v, Data and GND
- [LED bar Aluminum Channels - 50 or 30cm V Style](https://s.click.aliexpress.com/e/_mLCfd0P)
- Old usb cables to connect the LED bar to the microcontroller.
  

## Code and Setup

There are three main code files in this repository:

1. **Frontend Webpage**: This displays the game interface and real-time scoring updates.  
   [Frontend Code - Coming Soon](#link-to-frontend-code) | [View Screenshot](https://github.com/dplusplus1024/bitcorn-hole/blob/main/screenshot.png)

2. **Microcontroller Code**: This code runs on each microcontroller, hosting a web server and interfacing with the ToF sensor to detect when a bean bag is present.  
   [View Microcontroller Code](https://github.com/nitroxgas/bitcorn-hole/blob/main/src/microcontroller_webserver_with_tof/microcontroller_webserver_with_tof.ino) 
   **NEED UPDATE IF MERGED**

5. **API to Pay Sats**: This handles the instant payments to players' Lightning Addresses. You can substitute your own API endpoint, and either run your own LND node or use a custodial API like ZBD.  
   [API Code - Coming Soon](#link-to-api-code)

## Gameplay Instructions

- For detailed gameplay instructions, view the [instruction sheet](INSTRUCTIONS.md).
- For help with getting a Lightning Address, check out this [quick guide](LIGHTNING.md).

## Bitcorn Hole in Your Community

Did you bring Bitcorn Hole to your conference, community, or event? Brag about it [here](COMMUNITY.md)!

## Disclaimer

This is experimental software. Use at your own risk. 🌽

