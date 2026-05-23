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

Flash the trackpad variant for the half where the trackpad is installed. The opposite half can use either the default or trackpad variant independently (each half is flashed separately):

- Left half with trackpad: `left_trackpad_corne_eclipse_<layout>.zmk.uf2`
- Right half with trackpad: `right_trackpad_corne_eclipse_<layout>.zmk.uf2`

Both variants bundle the Azoteq IQS5xx and Cirque Pinnacle drivers, so a single firmware works for either module type. See [FLASHING.md](FLASHING.md) for flashing instructions.

## Trackpad Features

With the default firmware configuration, both trackpads support:

- Single-finger cursor movement
- Single-finger tap for left click
- Two-finger tap for right click
- Press and hold for drag
- Two-finger vertical and horizontal scroll
