# ⌨️ Mechanical Keyboard Sounds Plugin for Noctalia

Bring back the satisfying tactile vibe of typing on a real mechanical keyboard to your Linux desktop!

Designed specifically for **Noctalia Shell** (and Wayland environments like Niri, Hyprland, Sway), this plugin provides instant, sub-millisecond mechanical keyboard sound feedback with native bar integration, volume control, and 10 hand-curated audiophile switch profiles.

---

## ✨ Features

- **⚡ Sub-Millisecond Latency:** Pure C audio engine with direct 2ms hardware period buffering via `miniaudio`. Zero perceptible delay.
- **🛡️ Cross-Desktop Kernel evdev:** Reads `/dev/input/event*` directly. Completely compositor-agnostic — works on **Niri**, **Hyprland**, **Sway**, **GNOME**, and **KDE**.
- **🔇 No Repeat Machine-Gun Noise:** Smart debounce ignores key-repeat events when holding keys like Backspace or Space.
- **🎨 Interactive Native Bar Pill Widget:**
  - **Left-Click:** Opens the Plugin Settings dialog directly to change sound profiles and adjust options.
  - **Right-Click:** Instantly toggles sound ON / OFF (Mute / Unmute).
  - **Scroll Wheel:** Smoothly increases or decreases playback volume on the fly.
- **🔊 10 Curated & Normalized Switch Profiles:**
  - **Kailh Box Jade** — Sharp, crisp clickbar tactility
  - **NovelKeys Creams** — Smooth, deep, POM linear "thock"
  - **Banana Split Lubed** — Buttery-smooth enthusiast linear pop
  - **Cherry MX Blue** — Iconic, classic mechanical click
  - **Akko Lavender Purple** — Snappy early tactile bump
  - **Cherry MX Brown** — Subtle, quiet tactile feel
  - **Topre Purple Hybrid** — Deep electro-capacitive thock-pop
  - **Cherry MX Speed Silver** — Fast short-travel linear bottom-out
  - **Cherry MX Red** — Smooth, classic linear glide
  - **Razer Green** — Loud gaming clicky with crisp tactile report
- **🎛️ Settings Dialog:**
  - Switch profile picker dropdown with live test button
  - Volume slider (0% to 100%)
  - Keyboard hardware device selector (auto-detects all internal & external keyboards)
  - Toast notification toggle for mute/profile feedback
- **🧩 Zero Dependencies:**
  - Built-in automatic C compilation on startup if binary is missing.
  - Zero external runtime libraries (no Python, Electron, or heavy audio daemons).

---

## 🔒 Requirements

* **Noctalia Desktop Shell** (v3.6.0+)
* User in the `input` group:
  ```bash
  sudo usermod -aG input $USER
  ```
  *(Log out and back in if you just added yourself)*

---

## 📜 License

MIT License. Key soundpacks provided under Creative Commons / MIT.
