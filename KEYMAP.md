[README](README.md) | Keymap | [Parts](PARTS.md) | [Fabrication](FABRICATION.md) | [Building](BUILDING.md) | [Tricks](TRICKS.md)

## Keymap

Inspired by Cyberpunk's story of Johnny Silverhand, the Corne Eclipse comes with
the SAMURAI layout. I've used this layout since 2020 to slay everything from
video game corporations in a dystopian city to software development on the job.

The strategy I used when developing this layout is much like the strategy used
by vim/neovim: keep keys grouped by concept as much as possible. You can see
this in part by how movement is managed:

- The same key locations for movement in video games are reused for the arrow
  keys and again for mouse direction.
- The numpad is layed out like a full keyboard's numpad.

With this strategy, it makes leveraging all of the keys a lot more intuitive
than it seems at first glance.

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
sudo usermod -aG uucp $USER
```

### Reset To Firmware Default Keymap

To revert to the firmware's default keymap, use the "Restore Stock Settings"
option in ZMK Studio.

## Keyboard Layers

### Typing Layers

Beyond your default typing layout, you can activate layouts 2, 3, 4, and 5 on
the `SYSTEM` layer. By default all layouts are filled with alternative keymaps:

- QWERTY
- Colemak-DH
- Colemak
- Dvorak
- Workman

The default typing layout is decided by which firmware package you install from
the [releases](https://github.com/Frosthaven/zmk-config-corne-eclipse/releases)
page. You can always return to your default layout by pressing the `LAYOUT 1`
key on the `SYSTEM LAYER`.

![Typing Layer](/assets/images/samurai-legend-typing.png)

### Utility Layers

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

The emoji key on the `NAVSYM` layer is a macro that presses the keys necessary
to launch the emoji picker on Windows and on MacOS. For linux, you may need to
assign this key to your emoji picker of choice if one is not already assigned
(I'm a big fan of `smile` under Wayland). When pressed the shortcut will likely
be detected as `Super + period`.
