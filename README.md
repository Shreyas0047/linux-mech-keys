# ⌨️ Mechanical Keyboard Typing Sounds (`mech-keys`)

[![Noctalia Shell](https://img.shields.io/badge/Noctalia%20Shell-v3.6%2B%20Plugin-blue?style=for-the-badge&logo=gnome)](https://github.com/noctalia-dev/noctalia-shell)
[![Standalone](https://img.shields.io/badge/Standalone-Any%20Linux%20Desktop%20%2F%20WM-orange?style=for-the-badge&logo=linux)](https://kernel.org)
[![Platform](https://img.shields.io/badge/Platform-Wayland%20%26%20X11-purple?style=for-the-badge&logo=linux)](https://kernel.org)
[![Audio Backend](https://img.shields.io/badge/Latency-%3C2ms%20Hardware%20Buffer-success?style=for-the-badge&logo=c)](https://miniaud.io)
[![Binary Size](https://img.shields.io/badge/Binary%20Size-%3C500KB-informational?style=for-the-badge&logo=webassembly)](https://github.com)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)](https://opensource.org/licenses/MIT)

Transform your Linux typing into an authentic, tactile mechanical keyboard soundscape.

> [!NOTE]
> **Origins & Compatibility:** This project was originally developed as a plugin for the **Noctalia Desktop Shell**, but has been designed with a modular architecture so that it can also be used across **any Linux distribution and desktop environment / window manager** (Hyprland, Sway, i3, GNOME, KDE Plasma, XFCE, etc.) without requiring Noctalia!

Designed with a dual architecture:
1. **Full Noctalia Shell Plugin:** Interactive top/bottom bar pill widget, full GUI settings popup, volume slider, and toast notices.
2. **Standalone Daemon for Any Linux Setup:** Can be used on **any desktop environment or window manager** without Noctalia installed!

Whether you're typing on a flat laptop keyboard or miss the deep acoustic thock and crisp click of enthusiast mechanical switches, `mech-keys` brings instant, sub-millisecond polyphonic feedback for every keypress with zero latency, zero bloat, and zero external dependencies.

---

## 🌟 Comprehensive Feature List

### ⚡ 1. Audio Engine & Performance
- **Sub-Millisecond Response (<2ms):** Pure C audio engine utilizing `miniaudio` with a direct hardware buffer (`periodSizeInMilliseconds = 2`) on ALSA, PipeWire, PulseAudio, or JACK.
- **Polyphonic Multi-Voice Mixing:** Fast typists never experience cut-off audio; multiple key sounds overlap simultaneously with natural acoustic decay.
- **Normalized Studio Acoustics:** All switch sounds are audio-mastered and amplitude-normalized (85% peak) to deliver punchy, crisp sound through both laptop speakers and high-end headphones.
- **Zero Disk I/O on Keypress:** All sound samples are pre-indexed into memory for instantaneous actuation without frame drops or audio stuttering.

### 🛡️ 2. Cross-Desktop & Wayland Input System
- **Wayland-Independent Key Capture:** Listens directly to Linux kernel `evdev` (`/dev/input/event*`), completely bypassing Wayland's global keystroke isolation.
- **Universal Desktop Compatibility:** Works out-of-the-box on **Niri**, **Hyprland**, **Sway**, **GNOME**, **KDE Plasma**, **i3**, **bspwm**, **XFCE**, **River**, **DWL**, or raw Linux TTY.
- **Multi-Device Hardware Auto-Detection:** Dynamically scans `/dev/input/` and auto-listens to all connected keyboards simultaneously (internal laptop keyboard, external USB mechanical boards, and wireless Bluetooth keyboards).
- **Dedicated Device Locking:** Optional argument/setting to lock audio triggers strictly to a specific physical keyboard hardware path.

### 🔇 3. Smart Key-Repeat Debounce
- **No Machine-Gun Noise:** Automatically filters out Linux kernel key-repeat events (`value == 2`). Holding down Backspace, Space, Enter, or arrows produces a clean, natural single click rather than an unnatural continuous roar.

### 🎨 4. Native Noctalia Bar Widget (When using Noctalia)
- **Authentic Noctalia Design:** Built with Noctalia's native `BarPill` component, automatically adapting to your active theme, color scheme, and bar placement.
- **Dynamic Status Indicator:** Clean keyboard glyph (`keyboard` when active, dimmed `keyboard-off` when muted).
- **Three-in-One Quick Controls:**
  - **Left-Click:** Opens the Plugin Settings dialog directly to customize switches and audio.
  - **Right-Click:** Toggles sound ON / OFF (Mute / Unmute) instantly.
  - **Scroll Wheel:** Smoothly increases or decreases volume on the fly with live step increments.
- **Informative Tooltip:** Displays active sound profile, volume state, and mouse shortcut guide.

### 🎧 5. 10 Curated Mechanical Switch Soundpacks
Every major switch category is included out-of-the-box:

| Switch Profile | Switch Class | Acoustic Signature |
| :--- | :--- | :--- |
| **Kailh Box Jade** | Clickbar Clicky | Extremely sharp, crisp, high-pitched mechanical clickbar tactility. |
| **NovelKeys Creams** | POM Linear | Signature deep enthusiast "thock" with full-POM housing resonance. |
| **Banana Split Lubed** | Custom Linear | Buttery-smooth, creamy bottom-out pop beloved by keyboard builders. |
| **Cherry MX Blue** | Iconic Clicky | The legendary standard mechanical click with sharp tactile actuation. |
| **Akko Lavender Purple** | Tactile Bump | Snappy early tactile bump with a bright, clacky report. |
| **Cherry MX Brown** | Subtle Tactile | Classic, understated tactile feel suited for quiet office environments. |
| **Topre Purple Hybrid** | Electro-Capacitive | Deep acoustic "thock-pop" characteristic of Japanese domes. |
| **Cherry MX Speed Silver** | Short-Travel Linear | Crisp, feather-light bottom-out tailored for fast competitive typing. |
| **Cherry MX Red** | Smooth Linear | Soft, lightweight linear glide without click resistance. |
| **Razer Green** | Loud Gaming Clicky | Punchy, clicky gaming switch sound with pronounced return clack. |

### 🎛️ 6. Full Noctalia Settings Panel
- **Master Toggle:** Enable or disable typing sound effects.
- **Sound Profile Selector:** Select any of the 10 switch profiles with instant live switching.
- **Volume Slider:** Fine-tuned 0% to 100% volume slider with real-time audio gain adjustment.
- **Keyboard Device Selector:** Choose between auto-detecting all keyboards or binding to a specific device path.
- **Notification Toasts Toggle:** Enable or disable visual popups when changing profiles or toggling mute.
- **"Test Switch Profile" Button:** Preview your active switch sound directly inside settings before typing.

### 🔔 7. Desktop Toast Notifications
- Clean, non-intrusive Noctalia toasts confirming when sound is muted, unmuted, or when a new switch profile is activated.

### ⌨️ 8. Quickshell IPC Commands
Control `mech-keys` via shell scripts or bind commands directly to compositor shortcuts (e.g. in `niri.kdl` or `hyprland.conf`):

```bash
# Open Settings Dialog directly
qs -c noctalia-shell ipc call plugin openSettings "mech-keys"

# Toggle Mute / Unmute
qs -c noctalia-shell ipc call "plugin:mech-keys" toggle

# Cycle to the next switch soundpack in order
qs -c noctalia-shell ipc call "plugin:mech-keys" cycle

# Play a test keypress sound
qs -c noctalia-shell ipc call "plugin:mech-keys" test
```

### 🪶 9. Minimalist & Bloat-Free Footprint
- **Stripped Binary:** Stripped pure C binary compiled with only the required WAV decoder (`<500KB`).
- **Zero Runtime Dependencies:** No Python, no Node.js, no Electron, no heavy background daemons.
- **Self-Healing / Auto-Compilation:** `Main.qml` automatically tests the binary via `--ping`; if running on a non-x86_64 architecture (e.g. ARM64 / Raspberry Pi / Asahi Linux), it automatically runs `make -C backend` on startup.
- **Total Package Size:** Less than 30MB total for all 10 uncompressed high-fidelity soundpacks combined.

### 📂 10. Custom Soundpack Extensibility
- Drop custom `.wav` files into `mech-keys/sounds/<pack-name>/`.
- Compatible with Mechvibes soundpacks and custom recordings.
- Intelligent keycode fallback: dedicated sounds for `Space`, `Enter`, and `Backspace`, with automatic fallback for all other keys.

---

## 🔒 Permission Setup (evdev access)

To allow reading keystrokes from `/dev/input/event*` without root privileges, your user account must be part of the `input` group:

```bash
sudo usermod -aG input $USER
```

> **Note:** If you just added yourself to the `input` group, log out and log back in (or run `newgrp input`) for the group membership to take effect.

---

## 🚀 Installation & Usage

### Mode A: With Noctalia Shell (GUI + Bar Widget)

#### Via Noctalia Plugin Source (Recommended)
1. Open **Noctalia Settings** (`Super + ,` or from Bar / Launcher).
2. Go to **Plugins** → **Sources** tab.
3. Click **Add Source** and enter your repository URL:
   ```text
   https://github.com/<your-username>/linux-mech-keys
   ```
4. Switch to the **Explore** tab, search for **Mechanical Keyboard** (`mech-keys`), and click **Install**.
5. Add the **Mechanical Keyboard** widget to your bar in **Settings** → **Bar** → **Widgets**.

#### Via Manual Symlink
```bash
# 1. Clone repository
git clone https://github.com/<your-username>/linux-mech-keys.git ~/linux-mech-keys

# 2. Symlink into Noctalia
mkdir -p ~/.config/noctalia/plugins
ln -s ~/linux-mech-keys/mech-keys ~/.config/noctalia/plugins/mech-keys

# 3. Reload Noctalia
qs -c noctalia-shell kill && sleep 1 && qs -c noctalia-shell -d
```

---

### Mode B: Standalone Usage (Without Noctalia — Any Linux Desktop / WM)

You do **not** need Noctalia to use this sound engine! The backend `mech-sound` is a standalone, ultra-low latency C binary that runs on **any** Linux distribution and desktop environment (Hyprland, Sway, i3, GNOME, KDE Plasma, XFCE, bspwm, etc.).

#### 1. Compile the Binary (One command, zero external libraries)
```bash
git clone https://github.com/<your-username>/linux-mech-keys.git ~/linux-mech-keys
cd ~/linux-mech-keys/mech-keys/backend
make
```

#### 2. Run Directly from Terminal
```bash
# Run with any switch soundpack (e.g. NovelKeys Creams):
~/linux-mech-keys/mech-keys/bin/mech-sound --pack ~/linux-mech-keys/mech-keys/sounds/creams --volume 0.85

# Or choose any of the 10 soundpacks:
# akko-lavender, banana-split, boxjade, cherry-blue, cherry-brown, cherry-red, creams, razer-green, speed-silver, topre
```

#### 3. CLI Arguments Reference
| Flag | Example | Description |
| :--- | :--- | :--- |
| `--pack`, `-p` | `--pack ./sounds/topre` | Path to switch soundpack directory. |
| `--volume`, `-v` | `--volume 0.75` | Playback volume (`0.0` to `1.0`). |
| `--device`, `-d` | `--device /dev/input/event3` | Bind strictly to one keyboard (default: auto-detects all keyboards). |
| `--mute`, `-m` | `--mute` | Start muted. |
| `--list-devices`, `-l` | `--list-devices` | Outputs JSON list of all detected keyboard hardware. |
| `--ping` | `--ping` | Prints `PONG` and exits (quick architecture/health check). |

#### 4. Real-Time Stdin Control
The standalone engine accepts runtime commands piped through `stdin`:
```bash
VOLUME 0.6    # Adjust volume to 60%
MUTE 1        # Mute audio feedback
MUTE 0        # Unmute audio feedback
PACK /path    # Switch soundpack on the fly
TEST          # Play a test key click
QUIT          # Terminate engine cleanly
```

#### 5. Autostart on Any Desktop / Window Manager
To automatically start typing sounds when you log in to your desktop:

- **Hyprland (`~/.config/hypr/hyprland.conf`):**
  ```ini
  exec-once = ~/linux-mech-keys/mech-keys/bin/mech-sound --pack ~/linux-mech-keys/mech-keys/sounds/creams --volume 0.85
  ```

- **Sway / i3 (`~/.config/sway/config` or `~/.config/i3/config`):**
  ```ini
  exec ~/linux-mech-keys/mech-keys/bin/mech-sound --pack ~/linux-mech-keys/mech-keys/sounds/creams --volume 0.85
  ```

- **GNOME / KDE / XFCE (`~/.config/autostart/mech-sound.desktop`):**
  ```ini
  [Desktop Entry]
  Type=Application
  Name=Mechanical Keyboard Sounds
  Exec=sh -c "$HOME/linux-mech-keys/mech-keys/bin/mech-sound --pack $HOME/linux-mech-keys/mech-keys/sounds/creams --volume 0.85"
  Hidden=false
  NoDisplay=false
  X-GNOME-Autostart-enabled=true
  ```

- **Systemd User Service (`~/.config/systemd/user/mech-sound.service`):**
  ```ini
  [Unit]
  Description=Mechanical Keyboard Sound Daemon
  After=sound.target

  [Service]
  ExecStart=%h/linux-mech-keys/mech-keys/bin/mech-sound --pack %h/linux-mech-keys/mech-keys/sounds/creams --volume 0.85
  Restart=on-failure

  [Install]
  WantedBy=default.target
  ```
  Enable and start it with: `systemctl --user enable --now mech-sound`

---

## 🏗️ Architecture

```
┌────────────────────────────────────────────────────────┐
│     Frontend UI Layer (Noctalia Shell / Quickshell)    │
│  • BarWidget.qml (Visual Pill, Left/Right/Scroll acts) │
│  • Settings.qml  (Sound Profile, Volume, Device, Test) │
│  • Main.qml      (Lifecycle, Auto-build, IPC Handler)  │
└───────────────────────────┬────────────────────────────┘
                            │ Stdin Stream (or CLI Direct Invocation)
┌───────────────────────────▼────────────────────────────┐
│      mech-sound Standalone Engine (Pure C / miniaudio) │
│  • epoll-driven /dev/input/event* non-blocking polling │
│  • 2ms ALSA/PipeWire/PulseAudio low-latency buffer     │
│  • Multi-voice polyphonic WAV mixing                   │
│  • EV_KEY repeat debounce filter                       │
└────────────────────────────────────────────────────────┘
```

---

## 📜 License

Distributed under the [MIT License](LICENSE). Built for the Linux keyboard enthusiast community with ❤️.
