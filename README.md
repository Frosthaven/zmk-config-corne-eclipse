README | [Parts](PARTS.md) | [Fabrication](FABRICATION.md) | [Building](BUILDING.md) | [Tricks](TRICKS.md)

# Corne Eclipse

![Corne Eclipse](/assets/images/corne-eclipse.jpg)

The Corne Eclipse is a highly customized wireless Corne MX with the following:

- Custom circuit board design
    - JST port with line passthrough for undercarriage battery storage
    - Native support for nice!view displays
    - Native support for dual EC11 encoders
    - Native support for side mounted power switches
    - Native support for tenting puck
- Custom shield casing design (based on Void's case)
    - Undercarriage battery storage
    - Optimized material usage for bottom
    - Native support for tenting puck

*This was a project I finished in 2021. Finally updated and open sourced!*

## ZMK Studio

![ZMK Studio](/assets/images/zmk-studio.png)

This firmware supports [ZMK Studio](https://zmk.studio/) for changing *most*
key bindings without reflashing. To use it:

1. Connect the left half via USB
2. Make sure the keyboard is in USB mode (use the `BT or USB` key on the `SYSTEM` layer to toggle)
3. Open ZMK Studio and connect
4. When prompted to unlock, press the `ZMK Studio` key on the `SYSTEM` layer

On Linux, you may need serial port access. Run `sudo usermod -aG uucp $USER` and re-login.

To revert to the firmware's default keymap, use the "Restore Stock Settings" option in ZMK Studio.

## Keyboard Layers

This is my custom SAMURAI layout, designed for gaming, programming, and swift
touch typing. The default typing layer is Colemak DH. A QWERTY-first variant
is also available on the [releases](https://github.com/Frosthaven/zmk-config-corne-eclipse/releases) page.

![Typing Layer](/assets/images/samurai-legend-typing.png)

![Utility Layer](/assets/images/samurai-legend-utility.png)

## Notes
- The Bootloader and Reset keys will trigger for the side that the button is pressed on.
