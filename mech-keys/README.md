# Mechanical Keyboard Plugin for Noctalia

Mechanical keyboard typing sounds for Noctalia Shell. Plays switch click sounds with low latency as you type.

The core audio engine runs in pure C using `miniaudio` and Linux kernel `evdev` (`/dev/input/`), so it works on Wayland (Niri, Hyprland, Sway, etc.) with sub-2ms audio latency. It can also be run standalone without Noctalia.

## Features

- **Low latency:** Pure C engine with a 2ms hardware period buffer.
- **Wayland support:** Reads from `/dev/input/event*`, bypassing Wayland key-isolation.
- **Debounced repeat:** Does not play repeating sounds when holding down keys.
- **Bar widget:** Bar pill with mute status indicator, left-click for settings, right-click to mute, scroll wheel for volume.
- **Settings panel:** Profile selector, volume slider, device selector, and live test button.
- **10 Switch profiles:** Kailh Box Jade, NovelKeys Creams, Banana Split, Cherry Blue, Akko Lavender Purple, Cherry Brown, Topre Purple Hybrid, Speed Silver, Cherry Red, Razer Green.

## Requirements

- Noctalia Desktop Shell (v3.6.0+)
- User in the `input` group:
  ```bash
  sudo usermod -aG input $USER
  ```

## License

MIT License.
