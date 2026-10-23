# Mechanical Keyboard Sounds Plugin for Noctalia

Bring back the satisfying tactile vibe of typing on a real mechanical keyboard to your Linux desktop!

Designed specifically for **Noctalia Shell** (and Wayland environments like Niri, Hyprland, Sway), this plugin provides instant, sub-5ms mechanical keyboard sound feedback with on/off bar toggle and authentic switch sound profiles.

---

## Features

* **3 Authentic Switch Profiles:**
  * 🟢 **Kailh Box Jade** — Crisp, tactile, loud clicky switches
  * 🟡 **NovelKeys Creams** — Smooth, deep, buttery "thock" linear switches
  * 🔴 **Cherry MX Red** — Classic smooth linear typing sound
* **Top/Bottom Bar Integration:**
  * Click to toggle sound ON / OFF instantly.
  * Right-click to cycle switch profiles on the fly.
  * Native Noctalia tooltip & status badge.
* **Full Noctalia Settings Panel:**
  * Volume slider (0% – 100%)
  * Sound profile selector
  * Keyboard hardware picker (Internal laptop keyboard or external USB/Bluetooth keyboards)
  * Dynamic toast notification feedback
* **Ultra-Low Latency (~5ms):**
  * Standalone C engine using `miniaudio` and Linux kernel `/dev/input/` events.
  * Key sounds pre-cached into memory for zero disk stuttering while fast-typing.
  * Subtle pitch randomization ($\pm 2\%$) for natural typing acoustic variance.
  * **Zero dependencies:** Compiles with standard GCC, no Python libraries required.

---

## Requirements

* **Noctalia Desktop Shell** (v3.6.0+)
* User in the `input` group:
  ```bash
  sudo usermod -aG input $USER
  ```
  *(Log out and back in if you just added yourself)*

---

## License

MIT License. Key soundpacks provided under Creative Commons / MIT.
