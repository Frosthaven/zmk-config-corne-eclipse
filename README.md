README | [Keymap](KEYMAP.md) | [MX Parts](PARTS_MX.md) | [Building](BUILDING.md) | [Tricks](TRICKS.md)

# Corne Eclipse

![Corne Eclipse With Ceramic Keys](/assets/images/corne-eclipse.webp)

---

![Corne Eclipse Front And Back](/assets/images/corne-eclipse.png)

The Corne Eclipse is a highly customized wireless Corne MX.

*This was a project I initially finished in 2021. Finally updated and open sourced!*

## Custom Circuit Board Design

- JST port with line passthrough for undercarriage battery storage
- Native support for dual nice!view displays
- Native support for dual Alps EC11/EC12 encoders
- Native support for Azoteq TPS43 trackpad (right half, replaces encoder and display)
- Native support for side mounted power switches
- Native support for tenting puck

## Custom Shield Casing Design

Based on Void's case.

- Undercarriage battery storage
- Optimized material usage for bottom
- Native support for tenting puck

## Peripheral-side Trackpad Support

- TPS43 (and related Azoteq IQS5xx modules) via [AYM1607/zmk-driver-azoteq-iqs5xx](https://github.com/AYM1607/zmk-driver-azoteq-iqs5xx)
- Cirque Touchpad via [petejohanson/cirque-input-module](https://github.com/petejohanson/cirque-input-module)

## ZMK Modifications

### External Display Upgrades

- Lithium curve LUT applied for a 403450 to get more accurate numeric charge data (PR reference: [ZMK PR 2066](https://github.com/zmkfirmware/zmk/pull/2066))
- Track battery status of both halves on a single nice!view
- Capslock indicator

![nice!view Display Widgets](/assets/images/nice!view-display-widgets.gif)

### RGB Battery Optimizations

Powered by [zmk-ext-power-smart-idle](https://github.com/Frosthaven/zmk-ext-power-smart-idle).

- 20% brightness when not plugged in
- RGB auto-off on low battery when not plugged in
- Lights stay on when plugged in
- Eclipse v3 reversible variant uses a MOSFET that prevents drain when LEDs are off (coming soon)

## Default Typing Layouts

Available in [releases](https://github.com/Frosthaven/zmk-config-corne-eclipse/releases).

- QWERTY
- Colemak Mod DH (personal choice!)
- Colemak
- Workman
- DVORAK
