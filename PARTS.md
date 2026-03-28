[README](README.md) | Parts | [Fabrication](FABRICATION.md) | [Building](BUILDING.md) | [Tricks](TRICKS.md)

# Parts

## Electronics

| Required | Quantity | Part | Notes |
|----------|---------|------|-------|
| ✅ | 2 | [nice!nano v2 Microcontroller](https://nicekeyboards.com/nice-nano/) | Bluetooth-enabled. Solder at 270-300C to avoid damaging the nRF52840 chip. |
| ✅ | 2 | [JST Connector (S2B-PH-SM4-TB)](https://www.digikey.com/en/products/detail/jst-sales-america-inc/S2B-PH-SM4-TB/926655) | For battery connection with line passthrough. |
| ✅ | 2 | [SPDT Right-Angle Slide Switch](https://www.digikey.com/en/products/detail/eao/09-10290-01/8733108) | Power switch. |
| ✅ | 2 | [750mAh Battery (403450)](https://www.amazon.com/Liter-energybattery-Battery-Rechargeable-Connector/dp/B09FSY7N9G) | 3.7V LiPo, 4 x 34 x 50mm. JST PH 2.0 terminated. Important: The hot (red) line of the battery should go on the right side when looking down at either corne pcb from above. |
| ✅ | 100+ | [SMD Diodes](https://boardsource.xyz/products/diode-smd) | 1N4148W or equivalent. |
| ✅ | 42+ | [MX Hotswap Sockets](https://cannonkeys.com/products/kailh-mx-hotswap-sockets) | For plugging your switches into. |
| ✅ | 2 | [Low Profile MCU Sockets](https://splitkb.com/products/mill-max-low-profile-sockets) | For socketing the nice!nano (not permanent solder). |
| ✅ | 2 | [Raised MCU Headers](https://typeractive.xyz/products/machine-sockets-and-pins) | Paired with low profile sockets. |
| ✅ | 2 | [Reset Switches](https://splitkb.com/products/reset-buttons) | Tactile SMD reset switches. |
| ❌ | 1-2 | [nice!view Display](https://nicekeyboards.com/nice-view/) | Directly replaces OLED, uses TRRS data pin (008) for SPI since wireless builds don't use TRRS. |
| ❌ | 1-2 | [Alps EC11 Rotary Encoder](https://www.mouser.com/ProductDetail/Alps-Alpine/EC11N1524402?qs=W%2FMpXkg%252BdQ5mmk2EdvtXAA%3D%3D) | Alps Alpine EC11N1524402 or EC11 compatible encoder. |

## Switches & Keycaps

| Required | Quantity | Part | Notes |
|----------|---------|------|-------|
| ✅ | 42+ | MX Switches | MX-compatible switch set. |
| ✅ | 42+ | MX Keycaps | MX-compatible keycap set. |
| ❌ | 1-2 | [Encoder Rotary Knobs](https://www.gloriousgaming.com/products/gmmk-pro-keyboard-rotary-knob) | Any EC11-compatible knobs if you installed an encoder. |

## Plates & Case

| Required | Quantity | Part | Notes |
|----------|---------|------|-------|
| ❌ | 2 | [OLED Cover Kit](https://keebd.com/en-us/products/corne-acrylic-oled-cover-kit) | Protective acrylic covers. |
| ❌ | 2 | [SplitKB Tenting Pucks](https://splitkb.com/products/tenting-puck?_pos=1&_sid=b9942abea&_ss=r) | Mounted to the PCB via 4 screw holes. |
| ❌ | 2 | [Small Desk Tripod](https://www.manfrotto.com/global-en/pocket-support-large-black-mp3-bk/) | Manfrotto MP3-BK Pocket Tripod or similar. |

## Hardware

| Required | Quantity | Part | Notes |
|----------|---------|------|-------|
| ✅ | 10 | [M2x8mm Countersunk Screws](https://www.aliexpress.us/item/2251832781782755.html) | Use M2x4mm if using flat no-battery case. |
| ✅ | 36 | [M2x5mm Button Head Screws](https://www.aliexpress.us/item/2251832780910689.html) | 28 if not using tenting pucks. |
| ✅ | 10 | [M2x8mm Female-Female Standoffs](https://www.aliexpress.us/item/2255801085594177.html) | For FR4 plate spacing. |

## Fabricated Parts

You will also need the following parts from the [Fabrication](FABRICATION.md) page:
- 1x Circuit board set (left and right)
- 1x Plate set (left and right)
- 1x Shield casing (left and right)

