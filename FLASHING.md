# Flashing Firmware

This guide covers how to flash firmware to the Corne Eclipse keyboard. Each half of the keyboard must be flashed separately.

## Entering Bootloader Mode

There are two ways to put a keyboard half into bootloader mode:

### Option 1: From the System Layer

1. Activate the **SYSTEM** layer (hold both lower and raise keys simultaneously)
2. Press the **Bootloader** key on the side you want to flash
   - The left **Bootloader** key puts the **left half** into bootloader mode
   - The right **Bootloader** key puts the **right half** into bootloader mode

### Option 2: Using the Physical Reset Switch

If the keyboard is unresponsive or you can't access the system layer:

1. Locate the **reset switch** on top of the board, above the power switch
2. Press the reset switch **twice in quick succession** (double-tap)
3. The keyboard half will enter bootloader mode

## Flashing the Firmware

Once a keyboard half is in bootloader mode, it will appear as a USB drive on your computer (named **NICENANO**).

1. Download the firmware zip for your desired default typing layout from the [releases](https://github.com/Frosthaven/zmk-config-corne-eclipse/releases) page
2. Extract the zip to get the left and right `.uf2` files
3. Put the **left half** into bootloader mode
4. Drag and drop the **left** `.uf2` file onto the **NICENANO** drive
5. The drive will disconnect automatically when flashing is complete
6. Put the **right half** into bootloader mode
7. Drag and drop the **right** `.uf2` file onto the **NICENANO** drive
8. The drive will disconnect automatically when flashing is complete

Both halves should now be running the new firmware.

## Resetting to Factory Settings

If your keyboard is misbehaving (keys not working, Bluetooth issues, etc.), you can flash the **settings reset firmware** to clear all stored settings:

1. Download `settings_reset-nice_nano_v2-zmk.uf2` from the [releases](https://github.com/Frosthaven/zmk-config-corne-eclipse/releases) page
2. Flash this file to **both halves** using the steps above
3. After resetting both halves, flash your desired layout firmware to both halves again
