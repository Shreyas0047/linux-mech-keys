# linux-mech-keys

A lightweight mechanical keyboard sound simulator for Linux. Works as a native Noctalia Shell plugin or as a standalone background daemon on any desktop environment / window manager (Wayland & X11).

## Why I made this

I use a laptop with limited desk space, but I missed the sound and feel of typing on a physical mechanical keyboard.

Most existing alternatives on Linux (like Mechvibes) are heavy Electron applications that consume 200MB+ RAM and frequently break on modern Wayland compositors because they depend on legacy X11 global input hooks. Other tools were either CLI-only, had perceptible audio latency, or lacked desktop shell integration.

This project solves those issues:
- **Low latency:** Written in pure C using `miniaudio` with a ~2ms hardware period buffer.
- **Wayland-native:** Reads keystrokes directly from the Linux kernel via `/dev/input/event*` (evdev). Works everywhere: Niri, Hyprland, Sway, GNOME, KDE, or raw TTY.
- **No machine-gun repeat:** Automatically filters out kernel key-repeat events (`value == 2`), so holding down Space or Backspace sounds natural.
- **Polyphonic:** Plays multiple overlapping key sounds when typing fast without cutting off previous audio.
- **Lightweight:** The core engine is under 500KB compiled, uses ~8MB of RAM, and has zero external runtime dependencies.
- **Dual mode:** Can run as a standalone CLI daemon or integrated into the Noctalia desktop shell with a bar pill widget, volume controls, and a settings dialog.

---

## Switch Profiles Included

The repository includes 10 switch soundpacks out of the box:
- Kailh Box Jade (Clickbar Clicky)
- NovelKeys Creams (POM Linear Thock)
- Banana Split Lubed (Smooth Linear)
- Cherry MX Blue (Clicky)
- Akko Lavender Purple (Tactile)
- Cherry MX Brown (Subtle Tactile)
- Topre Purple Hybrid (Electro-Capacitive)
- Cherry MX Speed Silver (Fast Linear)
- Cherry MX Red (Classic Linear)
- Razer Green (Clicky)

You can also drop in your own custom soundpacks or existing Mechvibes packs.

---

## Setup & Permissions

Reading from `/dev/input/` requires your user account to have read access to the input devices. On most distributions, you just need to add yourself to the `input` group:

```bash
sudo usermod -aG input $USER
```

*Note: You may need to log out and log back in (or run `newgrp input`) for the group change to take effect.*

---

## Usage

### Option 1: Standalone CLI Daemon (Any Desktop / WM)

You don't need Noctalia to use this. You can compile the C engine in two seconds and run it anywhere (Hyprland, Sway, i3, GNOME, KDE, XFCE):

```bash
# Clone and build
git clone https://github.com/<your-username>/linux-mech-keys.git ~/linux-mech-keys
cd ~/linux-mech-keys/mech-keys/backend
make

# Run with a switch pack
../bin/mech-sound --pack ../sounds/creams --volume 0.8
```

#### CLI Options
```text
  --pack, -p <dir>      Path to soundpack folder (default: sounds/boxjade)
  --volume, -v <0.0-1.0> Set volume (default: 1.0)
  --device, -d <path>   Bind to a specific keyboard (default: auto-detect all)
  --mute, -m            Start muted
  --list-devices, -l    Print detected keyboards as JSON
  --ping                Health-check (prints PONG and exits)
```

#### Autostarting on login
- **Hyprland (`hyprland.conf`):**
  ```ini
  exec-once = ~/linux-mech-keys/mech-keys/bin/mech-sound --pack ~/linux-mech-keys/mech-keys/sounds/creams --volume 0.8
  ```
- **Sway / i3 (`config`):**
  ```ini
  exec ~/linux-mech-keys/mech-keys/bin/mech-sound --pack ~/linux-mech-keys/mech-keys/sounds/creams --volume 0.8
  ```
- **Systemd user service (`~/.config/systemd/user/mech-sound.service`):**
  ```ini
  [Unit]
  Description=Mechanical Keyboard Sound Daemon
  After=sound.target

  [Service]
  ExecStart=%h/linux-mech-keys/mech-keys/bin/mech-sound --pack %h/linux-mech-keys/mech-keys/sounds/creams --volume 0.8
  Restart=on-failure

  [Install]
  WantedBy=default.target
  ```
  Enable with: `systemctl --user enable --now mech-sound`

---

### Option 2: Noctalia Shell Plugin (GUI & Bar Widget)

If you use [Noctalia Shell](https://github.com/noctalia-dev/noctalia-shell):

#### Install via Plugin Sources:
1. Open Noctalia Settings -> Plugins -> Sources tab.
2. Add: `https://github.com/<your-username>/linux-mech-keys`
3. In the Explore tab, find **Mechanical Keyboard** and click Install.
4. Add the widget to your bar in Settings -> Bar -> Widgets.

#### Or manually symlink:
```bash
mkdir -p ~/.config/noctalia/plugins
ln -s ~/linux-mech-keys/mech-keys ~/.config/noctalia/plugins/mech-keys
qs -c noctalia-shell kill && sleep 1 && qs -c noctalia-shell -d
```

#### Controls:
- **Left-Click on bar widget:** Opens settings dialog to switch soundpacks, adjust volume, or test switches.
- **Right-Click:** Mute / Unmute toggle.
- **Scroll Wheel:** Adjust volume up/down.

---

## Adding Custom Soundpacks

You can add your own soundpacks or download packs made for Mechvibes:

1. Create a folder under `mech-keys/sounds/<pack-name>/`.
2. Add `.wav` files:
   - `space.wav` (Spacebar)
   - `enter.wav` (Enter key)
   - `backspace.wav` (Backspace key)
   - `default.wav` (Generic key sound used for everything else)
3. If using Noctalia, register the pack in `Settings.qml` and `Main.qml`. If using standalone CLI, just point `--pack` to your folder.

---

## License

MIT License. Key soundpacks provided under Creative Commons / MIT.
