# ⌨️ Noctalia Mechanical Keyboard Plugin (`mech-keys`)

[![Noctalia Shell](https://img.shields.io/badge/Noctalia%20Shell-v3.6%2B-blue?style=for-the-badge&logo=gnome)](https://github.com/noctalia-dev/noctalia-shell)
[![Platform](https://img.shields.io/badge/Platform-Linux%20Wayland%20%2F%20X11-purple?style=for-the-badge&logo=linux)](https://kernel.org)
[![Audio Backend](https://img.shields.io/badge/Latency-%3C2ms%20Hardware%20Buffer-success?style=for-the-badge&logo=c)](https://miniaud.io)
[![Binary Size](https://img.shields.io/badge/Binary%20Size-%3C500KB-informational?style=for-the-badge&logo=webassembly)](https://github.com)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)](https://opensource.org/licenses/MIT)

Transform your Linux typing into an authentic, tactile mechanical keyboard soundscape directly inside the **Noctalia Desktop Shell**.

Whether you're typing on a flat laptop keyboard or miss the deep acoustic thock and crisp click of enthusiast mechanical switches, `mech-keys` brings instant, sub-millisecond polyphonic feedback for every keypress with zero latency, zero bloat, and zero external dependencies.

---

## 🌟 Comprehensive Feature List

### ⚡ 1. Audio Engine & Performance
- **Sub-Millisecond Response (<2ms):** Pure C audio engine utilizing `miniaudio` with a direct hardware buffer (`periodSizeInMilliseconds = 2`) on ALSA and PipeWire.
- **Polyphonic Multi-Voice Mixing:** Fast typists never experience cut-off audio; multiple key sounds overlap simultaneously with natural decay.
- **Normalized Studio Acoustics:** All switch sounds are audio-mastered and amplitude-normalized (85% peak) to deliver punchy, crisp sound through both laptop speakers and high-end headphones.
- **Zero Disk I/O on Keypress:** All sound samples are pre-indexed into memory for instantaneous actuation without frame drops or audio stuttering.

### 🛡️ 2. Cross-Desktop & Wayland Input System
- **Wayland-Independent Key Capture:** Listens directly to Linux kernel `evdev` (`/dev/input/event*`), completely bypassing Wayland's global keystroke isolation.
- **Universal Desktop Compatibility:** Works out-of-the-box on **Niri**, **Hyprland**, **Sway**, **GNOME**, **KDE**, **River**, **DWL**, or raw Linux TTY.
- **Multi-Device Hardware Auto-Detection:** Automatically discovers and listens to all connected keyboards simultaneously (internal laptop keyboard, external USB mechanical boards, and wireless Bluetooth keyboards).
- **Dedicated Device Locking:** Optional setting to lock audio triggers strictly to a specific physical keyboard device.

### 🔇 3. Smart Key-Repeat Debounce
- **No Machine-Gun Noise:** Automatically filters out Linux kernel key-repeat events (`value == 2`). Holding down Backspace, Space, Enter, or arrows produces a clean, natural single click rather than an unnatural continuous roar.

### 🎨 4. Native Noctalia Bar Widget
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
| **🟢 Kailh Box Jade** | Clickbar Clicky | Extremely sharp, crisp, high-pitched mechanical clickbar tactility. |
| **🟡 NovelKeys Creams** | POM Linear | Signature deep enthusiast "thock" with full-POM housing resonance. |
| **🍌 Banana Split Lubed** | Custom Linear | Buttery-smooth, creamy bottom-out pop beloved by keyboard builders. |
| **🔵 Cherry MX Blue** | Iconic Clicky | The legendary standard mechanical click with sharp tactile actuation. |
| **💜 Akko Lavender Purple** | Tactile Bump | Snappy early tactile bump with a bright, clacky report. |
| **🟤 Cherry MX Brown** | Subtle Tactile | Classic, understated tactile feel suited for quiet office environments. |
| **👑 Topre Purple Hybrid** | Electro-Capacitive | Deep acoustic "thock-pop" characteristic of Japanese domes. |
| **⚡ Cherry MX Speed Silver** | Short-Travel Linear | Crisp, feather-light bottom-out tailored for fast competitive typing. |
| **🔴 Cherry MX Red** | Smooth Linear | Soft, lightweight linear glide without click resistance. |
| **🐍 Razer Green** | Loud Gaming Clicky | Punchy, clicky gaming switch sound with pronounced return clack. |

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
- **Self-Healing / Auto-Compilation:** `Main.qml` automatically checks if the binary is present; if running on a non-x86_64 architecture (e.g. ARM64), it automatically runs `make -C backend` on startup.
- **Total Package Size:** Less than 30MB total for all 10 uncompressed high-fidelity soundpacks combined.

### 📂 10. Custom Soundpack Extensibility
- Drop custom `.wav` files into `mech-keys/sounds/<pack-name>/`.
- Compatible with Mechvibes soundpacks and custom recordings.
- Intelligent keycode fallback: dedicated sounds for `Space`, `Enter`, and `Backspace`, with automatic round-robin fallback for all other keys.

---

## 🚀 Installation

### Method 1: Noctalia Plugin Source (Recommended)

1. Open **Noctalia Settings** (`Super + ,` or from the Bar / Launcher).
2. Go to **Plugins** → **Sources** tab.
3. Click **Add Source** and enter your repository URL:
   ```text
   https://github.com/<your-username>/noctalia-mech-keys
   ```
4. Switch to the **Explore** tab, search for **Mechanical Keyboard** (`mech-keys`), and click **Install**.
5. Add the **Mechanical Keyboard** widget to your bar in **Settings** → **Bar** → **Widgets**.

---

### Method 2: Manual Installation (One-Liner Symlink)

```bash
# 1. Clone the repository
git clone https://github.com/<your-username>/noctalia-mech-keys.git ~/noctalia-mech-keys

# 2. Symlink into Noctalia's plugin directory
mkdir -p ~/.config/noctalia/plugins
ln -s ~/noctalia-mech-keys/mech-keys ~/.config/noctalia/plugins/mech-keys

# 3. Reload Noctalia Shell
qs -c noctalia-shell kill && sleep 1 && qs -c noctalia-shell -d
```

---

## 🔒 Permission Setup (evdev access)

To allow the plugin engine to read keystrokes from `/dev/input/event*` without root privileges, your user account must be part of the `input` group:

```bash
sudo usermod -aG input $USER
```

> **Note:** If you just added yourself to the `input` group, log out and log back in (or run `newgrp input`) for the group membership to take effect.

---

## 🏗️ Architecture

```
┌────────────────────────────────────────────────────────┐
│         Noctalia Desktop Shell (Quickshell / QML)       │
│  • BarWidget.qml (Visual Pill, Left/Right/Scroll acts) │
│  • Settings.qml  (Sound Profile, Volume, Device, Test) │
│  • Main.qml      (Lifecycle, Auto-build, IPC Handler)  │
└───────────────────────────┬────────────────────────────┘
                            │ Stdin IPC Stream (VOLUME, MUTE, PACK, TEST)
┌───────────────────────────▼────────────────────────────┐
│         mech-sound Engine (Pure C / miniaudio)         │
│  • epoll-driven /dev/input/event* non-blocking polling │
│  • 2ms ALSA/PipeWire low-latency hardware buffer       │
│  • Multi-voice polyphonic WAV mixing                   │
│  • EV_KEY repeat debounce filter                       │
└────────────────────────────────────────────────────────┘
```

---

## 📜 License

Distributed under the [MIT License](LICENSE). Built for the Noctalia community with ❤️.
