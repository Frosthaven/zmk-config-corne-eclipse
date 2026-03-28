[README](README.md) | Keymap | [Parts](PARTS.md) | [Fabrication](FABRICATION.md) | [Building](BUILDING.md) | [Tricks](TRICKS.md)

## ZMK Studio

![ZMK Studio](/assets/images/zmk-studio.png)

This firmware supports [ZMK Studio](https://zmk.studio/) for changing *most* key
bindings without reflashing. You can read more about its features and
limitations on the [official documentation page](https://zmk.dev/docs/features/studio).

### Usage

1. Connect the left half via USB
2. Make sure the keyboard is in USB mode (use the `BT or USB` key on the `SYSTEM` layer to toggle)
3. Open ZMK Studio and connect
4. When prompted to unlock, press the `ZMK AUTH` key on the `SYSTEM` layer

On Linux, you may need serial port access (log out and back in after):

```bash
sudo usermod -aG uucp $USER` and re-login.
```

### Reset To Firmware Default Keymap

To revert to the firmware's default keymap, use the "Restore Stock Settings"
option in ZMK Studio.

### Tips

The firmare grants 5 separate typing layer toggles. While I recommend keeping
QWERTY around even for Colemak-DH enthusiasts like me, that leaves 3-4 extra
layers available that you may never need. This is a great place to shove a lot
of custom keyboard modes into your setup.

## Keyboard Layers

The Corne Eclipse comes with the SAMURAI layout. I designed this layout in 2020
in an attempt to bridge the three mental models I was often in - Programming,
gaming, or productivity. I've used this layout ever since, and have slain
everything from dragons to spreadsheets with it.

The default typing layer is decided by which firmware package you install from
the [releases](https://github.com/Frosthaven/zmk-config-corne-eclipse/releases)
page.

Regardless of which layout you install as the default, you can use the system
layer to swap to another at anytime.

### Typing Layers

![Typing Layer](/assets/images/samurai-legend-typing.png)

### Utility Layers

*Note: The order of the typing layer toggles on the system layer shown below may
be different on your board based on the default typing layer you installed. The
first typing layer choice will always be your default layout.*

![Utility Layer](/assets/images/samurai-legend-utility.png)

## MacOS Specific Notes

While you can use the above as-is, I personally prefer swapping control and
command when connecting to a mac. This allows copy/cut/paste etc to feel more
natural.

- Head over to System Settings → Keyboard
- Click on the `Keyboard Shortcuts` button
- Click on the `Modifier Keys`
  - Select the keyboard named `Corne Eclipse`
  - Set `Control key` to `Command`
  - Set `Command key` to `Control`

![Mac Keyboard Modifiers Settings](/assets/images/mac-rebind-modifiers.png)

## Linux Specific Notes

The emoji key on the symbol layer is a macro that presses the keys necessary to
launch the emoji picker on Windows and on MacOS. For linux, you may need to
assign this key to your emoji picker of choice if one is not already assigned
(I'm a big fan of `smile` under Wayland). When pressed the shortcut will likely
be detected as `Super + period`.
