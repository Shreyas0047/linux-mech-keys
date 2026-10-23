# ⌨️ Noctalia Mechanical Keyboard Plugin (`mech-keys`)

[![Noctalia Shell](https://img.shields.io/badge/Noctalia%20Shell-v3.6%2B-blue?style=for-the-badge&logo=gnome)](https://github.com/noctalia-dev/noctalia-shell)
[![Platform](https://img.shields.io/badge/Platform-Linux%20Wayland%20%2F%20X11-purple?style=for-the-badge&logo=linux)](https://kernel.org)
[![Audio Backend](https://img.shields.io/badge/Latency-%3C2ms%20Buffer-success?style=for-the-badge&logo=c)](https://miniaud.io)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)](https://opensource.org/licenses/MIT)

Transform your typing experience into an authentic mechanical keyboard soundscape directly inside the **Noctalia Desktop Shell**.

Whether you're on a laptop keyboard or miss the satisfying tactile click and deep thock of custom enthusiast switches, `mech-keys` delivers instant, sub-millisecond audio feedback for every keypress.

---

## ✨ Features

- **⚡ Sub-Millisecond Ultra-Low Latency:** Written in pure C with direct hardware period buffering (`periodSizeInMilliseconds = 2`) via `miniaudio`. Zero perceptible typing delay.
- **🛡️ Universal Cross-Desktop Compatibility:** Reads Linux kernel `evdev` events directly from `/dev/input/`. Completely immune to Wayland key-isolation restrictions — works seamlessly on **Niri**, **Hyprland**, **Sway**, **GNOME**, **KDE**, or raw TTY.
- **🔇 Smart Key-Repeat Handling:** Automatic debounce suppresses repeating machine-gun clicks when holding down Backspace, Space, or navigation keys.
- **🎨 Native Noctalia Bar Widget:**
  - **Left-Click:** Opens the Plugin Settings dialog directly to switch profiles or tune options.
  - **Right-Click:** Instantly toggles typing sounds (Mute / Unmute).
  - **Scroll Wheel:** Smoothly increases or decreases playback volume.
- **🔊 10 Audiophile Switch Profiles:** Mastered and amplitude-normalized for crisp acoustics through both laptop speakers and high-end headphones.
- **🧩 Zero External Runtime Dependencies:** Everything is self-contained. No heavy Python, Electron, or external audio servers required.

---

## 🎧 Curated Switch Profiles

| Switch Profile | Type | Sound Profile Description |
| :--- | :--- | :--- |
| **🟢 Kailh Box Jade** | Clickbar Clicky | Extremely sharp, crisp, high-pitched mechanical clickbar tactility. |
| **🟡 NovelKeys Creams** | POM Linear | Signature deep enthusiast "thock" with full-POM housing resonance. |
| **🍌 Banana Split Lubed** | Custom Linear | Buttery-smooth, creamy bottom-out pop beloved by keyboard builders. |
| **🔵 Cherry MX Blue** | Iconic Clicky | The legendary standard mechanical click with sharp tactile actuation. |
| **💜 Akko Lavender Purple** | Tactile Bump | Snappy early tactile bump with a bright, clacky report. |
| **🟤 Cherry MX Brown** | Subtle Tactile | Classic, understated tactile feel suited for quiet office environments. |
| **👑 Topre Purple Hybrid** | Electro-Capacitive | Deep acoustic "thock-pop" characteristic of Japanese domes. |
| **⚡ Cherry MX Speed Silver** | Short-Travel Linear | Crisp, responsive, feather-light bottom-out tailored for fast gaming. |
| **🔴 Cherry MX Red** | Smooth Linear | Smooth, lightweight linear glide without click resistance. |
| **🐍 Razer Green** | Loud Gaming Clicky | Punchy, clicky gaming switch sound with pronounced return clack. |

---

## 🚀 Installation

### Option 1: Noctalia Plugin Source (Recommended)

1. Open **Noctalia Settings** (`Super + ,` or from the Bar / Launcher).
2. Navigate to **Plugins** → **Sources** tab.
3. Click **Add Source** and paste this GitHub repository URL:
   ```text
   https://github.com/<your-username>/noctalia-mech-keys
   ```
4. Switch to the **Explore** tab, search for **Mechanical Keyboard** (`mech-keys`), and click **Install**.
5. Add the **Mechanical Keyboard** widget to your bar in **Settings** → **Bar** → **Widgets**.

---

### Option 2: Manual Installation

Clone or symlink the plugin folder directly into your Noctalia plugins directory:

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

To allow Noctalia to capture global keystrokes without root privileges, your user account must have read access to `/dev/input/event*`:

```bash
# Add your user to the input group
sudo usermod -aG input $USER
```

> **Note:** If you just added yourself to the `input` group, log out and log back in (or run `newgrp input`) for the group membership to take effect.

---

## ⚙️ Configuration & IPC Controls

You can control `mech-keys` visually via the Settings dialog or programmatically through Quickshell's IPC interface.

### Settings UI
- **Switch Profile:** Choose between all 10 switch profiles from the dropdown.
- **Volume Slider:** Adjust sound volume from 0% to 100%.
- **Keyboard Device Selector:** Auto-detects all plugged keyboards, or lock to a specific hardware device.
- **Notification Toasts:** Toggle visual feedback when switching switches or muting.
- **Test Switch:** Live audio preview button inside the settings panel.

### Quickshell IPC Commands
Bind these to compositor keybindings (in `niri.kdl`, `hyprland.conf`, etc.) or invoke via shell:

```bash
# Open Settings Dialog
qs -c noctalia-shell ipc call plugin openSettings "mech-keys"

# Toggle Mute / Unmute
qs -c noctalia-shell ipc call "plugin:mech-keys" toggle

# Cycle to the next switch soundpack
qs -c noctalia-shell ipc call "plugin:mech-keys" cycle

# Play a test keypress click
qs -c noctalia-shell ipc call "plugin:mech-keys" test
```

---

## 🛠️ Adding Custom Soundpacks

Adding custom soundpacks (e.g. from Mechvibes or your own recordings) is as simple as dropping `.wav` files into a directory:

1. Create a new folder inside `mech-keys/sounds/<pack-name>/`.
2. Add audio files named after key names or numbers:
   - `space.wav` (Spacebar)
   - `enter.wav` (Return key)
   - `backspace.wav` (Backspace key)
   - `default.wav` or `1.wav` .. `10.wav` (General alpha/numeric keys)
3. Register the profile in `mech-keys/Settings.qml` and `mech-keys/Main.qml`.

---

## 🏗️ Architecture

```
┌──────────────────────────────────────────────┐
│  Noctalia Desktop Shell (Quickshell / QML)   │
│  • BarWidget.qml (Visual Pill & Controls)    │
│  • Settings.qml  (Sound Profile & Volume)    │
│  • Main.qml      (Lifecycle & IPC Manager)   │
└──────────────────────┬───────────────────────┘
                       │ Stdin IPC Stream (VOLUME, MUTE, PACK)
┌──────────────────────▼───────────────────────┐
│  mech-sound Engine (Pure C / miniaudio)      │
│  • epoll-driven /dev/input/ event polling    │
│  • 2ms ALSA/PipeWire low-latency buffer      │
│  • Multi-voice polyphonic WAV mixing         │
└──────────────────────────────────────────────┘
```

---

## 📜 License

Distributed under the [MIT License](LICENSE). Built for the Noctalia community with ❤️.
