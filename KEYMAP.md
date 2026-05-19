[README](README.md) | Keymap | [MX Parts](PARTS_MX.md) | [Building](BUILDING.md) | [Tricks](TRICKS.md)

## Keymap

Inspired by Cyberpunk's story of Johnny Silverhand, the Corne Eclipse comes with
the SAMURAI layout. I've used this layout since 2020 to slay everything from
video game corporations in a dystopian city to software development on the job.

The strategy I used when developing this layout is much like the strategy used
by vim/neovim: keep keys grouped by concept as much as possible. You can see
this in part by how movement is managed:

- The same key locations for movement in video games are reused for the arrow
  keys and again for mouse direction.
- The numpad is laid out like a full keyboard's numpad.

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

<details>
<summary>Omni-shortcuts for Navigation and Text Editing</summary>


Ideally, you'd want to get used to the different keybinds on different operating
systems to save yourself a headache later. However, you can use
Karabiner-Elements to bridge the divide between Linux/Windows and Mac for most
things.

Install [Karabiner-Elements](https://karabiner-elements.pqrs.org/) via Homebrew:

```bash
brew install --cask karabiner-elements
```

Then paste the following URL into your browser address bar to import the
Omni-shortcuts:

```
karabiner://karabiner/assets/complex_modifications/import?url=https://raw.githubusercontent.com/Frosthaven/zmk-config-corne-eclipse/main/assets/references/karabiner-windows-shortcuts.json
```

*Note: Make sure that Corne Eclipse has both `Modify events` and `Ignore vendor
events` toggled on in Karabiner-Elements' Devices tab.*

Three rule groups — **Text Navigation**, **Text Editing**, and **App Shortcuts** — remap common shortcuts so Linux/Windows muscle memory carries over on macOS. All rules exclude terminal apps to preserve signals like `Ctrl+C`. Note: `Ctrl+Left/Right` overrides macOS Space switching (previous/next desktop).

| Key | Action |
|-----|--------|
| `Home` | Beginning of line |
| `End` | End of line |
| `Shift+Home` / `Shift+End` | Select to line start/end |
| `Ctrl+Home` / `Ctrl+End` | Beginning/end of document |
| `Shift+Ctrl+Home` / `Shift+Ctrl+End` | Select to document start/end |
| `Ctrl+Left` / `Ctrl+Right` | Jump word left / right |
| `Ctrl+Shift+Left` / `Ctrl+Shift+Right` | Select word left / right |
| `Ctrl+Backspace` | Delete word backward |
| `Ctrl+Delete` | Delete word forward |
| `Ctrl+C` / `X` / `V` | Copy / Cut / Paste |
| `Ctrl+Z` / `Ctrl+Y` / `Ctrl+Shift+Z` | Undo / Redo |
| `Ctrl+A` | Select All |
| `Ctrl+S` / `Ctrl+Shift+S` | Save / Save As |
| `Ctrl+F` | Find |
| `Ctrl+R` | Reload |
| `Ctrl+W` | Close tab |
| `Ctrl+T` | New tab |
| `Ctrl+N` / `Ctrl+Shift+N` | New window / Private window |
| `Ctrl+O` | Open file |
| `Ctrl+L` | Focus address bar |
| `Ctrl+P` / `Ctrl+Shift+P` | Print / Command palette |
| `Ctrl+K` | In-app search |
| `Ctrl+D` | Bookmark / select next occurrence |
| `Ctrl+B` | Bold / toggle sidebar |
| `Ctrl+I` | Italic |

</details>

<details>
<summary>Tap Command alone → launch Raycast</summary>


A modifier-tap rule turns the Command key into a [Raycast](https://www.raycast.com/) launcher when tapped on its own: hold Command and it behaves normally for shortcuts (e.g. `Cmd+S`), tap and release it alone and it fires `Control+Space` (Raycast's default global hotkey). Released using Karabiner's `to_if_alone` with `lazy: true`, so holding Command never registers as a key event until another key joins it — there's no menu flash or stray modifier press.

Bundled into the same `karabiner-windows-shortcuts.json` import URL as the omni-shortcuts above. The rule shows up in Karabiner as a separate enable/disable toggle, so you can leave it off if you'd rather not lose tap-Command.

| Key | Action |
|-----|--------|
| Tap `Cmd` (no other key) | Launch / toggle Raycast |
| Hold `Cmd` + any key | Normal Command behavior |

*Note: requires Raycast's global hotkey to be set to `Control+Space` (the default). If you've changed it, update the `to_if_alone` binding in the rule to match.*

</details>

## Linux Specific Notes

<details>
<summary>Using the Emoji key</summary>


The emoji key on the `MSESYM` layer is a macro that presses the keys necessary
to launch the emoji picker on Windows and on MacOS. For Linux, you may need to
assign this key to your emoji picker of choice if one is not already assigned
(I'm a big fan of `smile` under Wayland). When you press the emoji key, it will
likely be detected as `Super + period`.

![Linux Emoji Picker](/assets/images/linux-emoji-picker.png)

</details>
