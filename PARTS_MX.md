[README](README.md) | [Keymap](KEYMAP.md) | MX Parts | [Building](BUILDING.md) | [Tricks](TRICKS.md)

# MX Parts

## Electronics

| Required | Quantity | Part | Notes |
|----------|---------|------|-------|
| ✅ | 2 | [nice!nano v2 Microcontroller](https://nicekeyboards.com/nice-nano/) | Bluetooth-enabled. Solder at 270-300C to avoid damaging the nRF52840 chip. |
| ✅ | 2 | [JST Connector (S2B-PH-SM4-TB)](https://www.digikey.com/en/products/detail/jst-sales-america-inc/S2B-PH-SM4-TB/926655) | For battery connection with line passthrough. |
| ✅ | 2 | [SPDT Right-Angle Slide Switch](https://www.digikey.com/en/products/detail/eao/09-10290-01/8733108) | Power switch. |
| ✅ | 2 | [750mAh Battery (403450)](https://www.amazon.com/Liter-energybattery-Battery-Rechargeable-Connector/dp/B09FSY7N9G) | 3.7V LiPo, 4 x 34 x 50mm. JST PH 2.0 terminated. Important: The hot (red) line of the battery should go on the right side when looking down at either corne pcb from above. |
| ✅ | 100+ | [SMD Diodes](https://boardsource.xyz/products/diode-smd) | 1N4148W or equivalent. |
| ✅ | 42+ | [MX Hotswap Sockets](https://cannonkeys.com/products/kailh-mx-hotswap-sockets) | For plugging your switches into. |
| ✅ | 2 | [Mill Max Low Profile Sockets & Pins](https://splitkb.com/products/mill-max-low-profile-sockets?variant=31945995845709) | For socketing the nice!nano. Make sure you get the Mil Max pins with it! |
| ✅ | 2 | [Reset Switches](https://splitkb.com/products/reset-buttons) | Tactile SMD reset switches. |
| ❌ | 1-2 | [nice!view Display](https://nicekeyboards.com/nice-view/) | The left is the most important, as it shows active bluetooth state and profile. Be sure to get pins/sockets that accomodate your socketing height of the nice!nano. |
| ❌ | 1-2 | [Alps EC11 Rotary Encoder](https://www.mouser.com/ProductDetail/Alps-Alpine/EC11N1524402?qs=W%2FMpXkg%252BdQ5mmk2EdvtXAA%3D%3D) | Alps Alpine EC11N1524402 or EC11 compatible encoder. |
| ❌ | 1 | [Azoteq TPS43 Touchpad Module](https://holykeebs.com/products/touchpad-module) | Replaces the right encoder. Requires 5 wires (SDA, SCL, RDY, VCC, GND) to the nice!nano. See [wiring guide](WIRING_TRACKPAD.md). |

## Switches & Keycaps

| Required | Quantity | Part | Notes |
|----------|---------|------|-------|
| ✅ | 42+ | MX Switches | MX-compatible switch set. |
| ✅ | 42+ | MX Keycaps | MX-compatible keycap set. |
| ❌ | 1-2 | [Encoder Rotary Knobs](https://www.gloriousgaming.com/products/gmmk-pro-keyboard-rotary-knob) | Any EC11-compatible knobs if you installed an encoder. |

## Plates & Case

| Required | Quantity | Part | Notes |
|----------|---------|------|-------|
| ❌ | 2 | [OLED Cover Kit](https://keebd.com/en-us/products/corne-acrylic-oled-cover-kit) | Protective acrylic covers. If you find these without the standoffs and screws, you'll need to get those yourself. |
| ❌ | 2 | [SplitKB Tenting Pucks](https://splitkb.com/products/tenting-puck?_pos=1&_sid=b9942abea&_ss=r) | Mounted to the PCB via 4 included screws. May have options that include the Manfrotto MP3-BK. |
| ❌ | 2 | [Small Desk Tripod](https://www.manfrotto.com/global-en/pocket-support-large-black-mp3-bk/) | Manfrotto MP3-BK Pocket Tripod or similar. Only if you didn't get it bundled above. |

## Hardware

I haven't yet verified the exact screws, so this list may not be exact. YMMV.

| Required | Quantity | Part | Notes |
|----------|---------|------|-------|
| ✅ | 10 | [M2x8mm Countersunk Screws](https://www.aliexpress.us/item/2251832781782755.html) | Standoff bottom case screws. |
| ✅ | 36 | [M2x5mm Button Head Screws](https://www.aliexpress.us/item/2251832780910689.html) | Standoff top plate screws. |
| ✅ | 10 | [M2x8mm Female-Female Standoffs](https://www.amazon.com/dp/B0FLGTHV32?th=1) |  |

## Fabrication

While the sections above cover items you can buy, this section documents
what needs to be created.

The circuit board, plates, and shields need to be fabricated by a PCB and 3D
printing company like [Elecrow](https://www.elecrow.com/),
[JLCPCB](https://jlcpcb.com/), [PCBWay](https://www.pcbway.com/), or
[OSHPark](https://oshpark.com/). I've had good work from Elecrow in the past.

### Circuit Board

![Circuit Board](/assets/images/corne-eclipse-kicad-circuitboard.png)

Some vendors won't render the outside negative space of the circuitboard
correctly in their uploader tool, but it is still fine.

Provide these files to the fabricator:
- [corne-eclipse-circuitboard.zip](/fabrication/!for-fabricator-production/corne-eclipse-circuitboard.zip)

### Plates

![Plates](/assets/images/corne-eclipse-kicad-plates.png)

This will need to be cut the same way that circuitboards are. There may be an
extra charge for adding break-away points to the plate print.

Provide these files to the fabricator:
- [corne-eclipse-plates.zip](/fabrication/!for-fabricator-production/corne-eclipse-plates.zip)

### Shield Case

![Shield](/assets/images/corne-eclipse-shield.png)

Provide these files to the fabricator:
- [corne-eclipse-shield-left.stl](/fabrication/!for-fabricator-production/corne-eclipse-shield-left.stl)
- [corne-eclipse-shield-right.stl](/fabrication/!for-fabricator-production/corne-eclipse-shield-right.stl)

*Note: Some fabricators have gotten pretty good at printing this out in metal.*
