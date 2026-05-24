[README](README.md) | [Keymap](KEYMAP.md) | [MX Parts](PARTS_MX.md) | [Building](BUILDING.md) | [Tricks](TRICKS.md)

# Wiring a Trackpad

The Corne Eclipse can host a trackpad on either half (the PCB is reversible). The same wiring approach applies to either side; just hand-wire to the pads on the half where the trackpad is installed.

Two trackpad modules are supported:

- **Azoteq TPS43** (rectangular, 43x40 mm). EOL as of April 2024; aftermarket stock only.
- **Cirque Pinnacle** (GlidePoint Circle TM040040 40 mm round / TM035035 35 mm round). Actively produced, recommended for new builds.

Both modules use the same I2C interface and the same interrupt pin. A single trackpad firmware variant per half (`left_trackpad_corne_eclipse_<layout>.zmk.uf2` or `right_trackpad_corne_eclipse_<layout>.zmk.uf2`) bundles both drivers and auto-detects whichever module is wired up.

## What gets disabled

The trackpad communicates over I2C using pins P0.17 (SDA) and P0.20 (SCL). The nice!view display on that half uses these same physical pins for SPI. The nRF52840 cannot run both I2C and SPI on the same pins simultaneously, so **the half with the trackpad cannot use a nice!view display**. The OLED display is also disabled in the trackpad firmware to avoid conflicts.

The other half's display is unaffected.

The encoder is **not** affected: the trackpad RDY/DR signal lives on a different pad (the SPI_SCK net = pad 14 = pro_micro D16 / P0.10) than the encoder pins, so the rotary encoder still works alongside a trackpad. A key switch in the encoder position also still works.

---

## Wiring

### Prerequisites

- A trackpad module of your choice:
  - Azoteq TPS43-201A-B (limited aftermarket availability, e.g. [HolyKeebs](https://holykeebs.com/products/touchpad-module))
  - Cirque GlidePoint Circle **TM040040** (40 mm) or **TM035035** (35 mm), available from [keycapsss](https://keycapsss.com/keyboard-parts/parts/211/glidepoint-cirque-trackpad-tm040040-tm035035), DigiKey, Mouser, or Cirque directly
- 5 thin wires (28-30 AWG recommended)
- Soldering iron and solder
- The nice!view display on that half must be removed (the trackpad reuses its I2C pins)

Cirque modules ship in a few form factors, most commonly a flat self-adhesive flex or an FFC ribbon. The electrical connections are the same across them; only the physical pad layout or connector differs. Match each wire below by its signal name on your specific module.

### Wiring Diagram

| Trackpad Signal | PCB Pad / Net | nice!nano Pin | Notes |
|---|---|---|---|
| SDA | I2C data | P0.17 (pro_micro SDA / pad 2) | Reuses the display's SDA pad |
| SCL | I2C clock | P0.20 (pro_micro SCL / pad 3) | Reuses the display's SCL pad |
| RDY (Azoteq) / DR (Cirque) | **SPI_SCK net (pad 14)** | P0.10 (pro_micro D16) | Active high. Hand-wire to the pad labeled **SPI_SCK** on the PCB (silkscreen pad 14). |
| VCC / VDD / 3V3 | 3.3 V | 3.3 V rail | From the nice!nano 3.3 V rail. Labeled `3V3` on Azoteq, `VDD` on Cirque. Cirque ASIC is 3.3 V only; do not wire to 5 V. |
| GND | Ground | GND | Any ground pad |

The signal names differ slightly between modules (RDY on Azoteq, DR on Cirque) but go to the same nice!nano pin and serve the same purpose: a data-ready interrupt from the trackpad.

### Connection points on the PCB

**SDA and SCL:** Solder to the nice!nano's pro_micro SDA and SCL pads. The pads where the nice!view display connected on that half are a convenient solder point.

**RDY / DR:** Solder to the PCB pad labeled **SPI_SCK** (silkscreen pad 14). This pad normally carries the nice!view display's SPI clock signal. With the display removed and the trackpad firmware loaded, the SPI peripheral no longer uses this pad and it becomes the I2C interrupt input. The encoder pins are untouched.

**Power:** Use the nice!nano's 3.3 V and GND pads on that half.

### Notes

- Azoteq TPS43 I2C address is **0x74**; Cirque Pinnacle is **0x2A** by default. If your Cirque module has the R1 solder jumper bridged the address becomes **0x2C**, in which case you need to edit `reg = <0x2a>` to `<0x2c>` in the overlay before building from source.
- RDY / DR is **active high**; no pull-up resistor is needed.
- The TPS43 requires a **1 mm dielectric overlay** (glass or acrylic) for proper capacitive sensing. The HolyKeebs kit includes a matte grey glass overlay.
- Keep wires as short as practical to reduce I2C noise.
- Cursor direction (X/Y polarity) depends on how you mount the module relative to the keyboard. If movement feels inverted after flashing, adjust the `input-processors` line for the relevant trackpad in `config/boards/shields/corne_eclipse/corne_eclipse_<half>.overlay` and rebuild from source.

## Firmware

**A trackpad is flashed as a coordinated pair, not one half at a time.** The left half is the split central (the half that talks to your computer); the right half is the peripheral. A trackpad on the peripheral can't reach the computer on its own - it forwards its input to the central over the split link - so the central's firmware has to be built to receive it. Flashing only the trackpad half will result in no cursor movement.

Each release zip is organized into scenario folders. Pick the one that matches where your trackpad is, and flash **both** `.uf2` files in that folder (left to the left half, right to the right half):

| Your build | Folder | Flash to left | Flash to right |
|---|---|---|---|
| No trackpad | `no trackpads/` | `standard_left_…` | `standard_right_…` |
| Trackpad on the **right** (peripheral) | `trackpad on peripheral side/` | `trackpad_peripheral_left_…` | `trackpad_peripheral_right_…` |
| Trackpad on the **left** (central) | `trackpad on central side/` | `trackpad_central_left_…` | `trackpad_central_right_…` |

The half that physically has the trackpad can't drive a nice!view display (the trackpad reuses the display's I2C pins). If the other half has a display fitted, the status widgets render there. Each trackpad firmware bundles both the Azoteq IQS5xx and Cirque Pinnacle drivers, so one build works for either module. See [FLASHING.md](FLASHING.md) for flashing instructions.

> If you flash mismatched halves (e.g. a `trackpad_peripheral_right` with a `standard_left`), the trackpad will not work - the central won't be listening for the forwarded input. Always flash both halves from the same folder.

## Trackpad Features

Both trackpads support:

- Single-finger cursor movement
- Single-finger tap for left click
- Two-finger vertical and horizontal scroll

The **Azoteq TPS43** has a multi-touch gesture engine and additionally supports:

- Two-finger tap for right click
- Three-finger tap for middle click
- **Press-and-hold drag-lock** (press and hold to grab, then drag; it holds
  across finger lifts and is released by a tap). Starting the drag from a
  press-and-hold rather than a tap means it can grab a multi-file selection
  without the tap deselecting it.

> **Pinch / expand zoom is experimental.** It is implemented (the chip's zoom
> gesture, mapped to Ctrl+scroll on the host), but the IQS5xx's zoom competes
> with two-finger scroll and is finicky to trigger — it may need tuning and may
> not fire reliably on every unit. No other firmware (QMK, the rwalkr Rust crate,
> the Linux driver) exercises it either.

The **Cirque Pinnacle** (GlidePoint Circle) is **single-touch hardware** — it can only track one finger at a time. It therefore cannot do any two-finger or three-finger gesture, and double-tap-drag is not implemented for it; it provides cursor movement, tap-to-click, and scroll. This is a hardware limitation of the Pinnacle, not a firmware choice.
