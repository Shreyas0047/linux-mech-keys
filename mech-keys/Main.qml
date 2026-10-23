import QtQuick
import Quickshell
import Quickshell.Io
import qs.Commons
import qs.Services.UI

Item {
  id: root

  property var pluginApi: null

  // Plugin Settings
  readonly property var cfg: pluginApi?.pluginSettings || ({})
  readonly property var defaults: pluginApi?.manifest?.metadata?.defaultSettings || ({})

  property bool enabledSound: cfg.enabledSound ?? defaults.enabledSound ?? true
  property string soundPack: cfg.soundPack ?? defaults.soundPack ?? "boxjade"
  property real volume: cfg.volume ?? defaults.volume ?? 0.85
  property string inputDevice: cfg.inputDevice ?? defaults.inputDevice ?? ""
  property bool showBarIcon: cfg.showBarIcon ?? defaults.showBarIcon ?? true
  property bool enableToasts: cfg.enableToasts ?? defaults.enableToasts ?? true

  IpcHandler {
    target: "plugin:mech-keys"
    function test() {
      root.testSound();
    }
    function toggle() {
      root.toggleSound();
    }
    function cycle() {
      root.cycleSoundPack();
    }
  }

  // Device list fetched from engine
  property var availableDevices: [
    { key: "", name: "Auto-detect all keyboards (Recommended)" }
  ]

  // Sound pack profiles
  readonly property var soundPacks: [
    { key: "boxjade", name: "🟢 Kailh Box Jade (Sharp Clicky)" },
    { key: "creams", name: "🟡 NovelKeys Creams (Deep Thock)" },
    { key: "banana-split", name: "🍌 Banana Split Lubed (Buttery Linear)" },
    { key: "cherry-blue", name: "🔵 Cherry MX Blue (Iconic Clicky)" },
    { key: "akko-lavender", name: "💜 Akko Lavender Purple (Tactile Bump)" },
    { key: "cherry-brown", name: "🟤 Cherry MX Brown (Classic Tactile)" },
    { key: "topre", name: "👑 Topre Purple Hybrid (Deep Thock-Pop)" },
    { key: "speed-silver", name: "⚡ Cherry MX Speed Silver (Fast Linear)" },
    { key: "cherry-red", name: "🔴 Cherry MX Red (Smooth Linear)" },
    { key: "razer-green", name: "🐍 Razer Green (Loud Clicky)" }
  ]

  function getSoundPackName(key) {
    for (var i = 0; i < soundPacks.length; i++) {
      if (soundPacks[i].key === key) return soundPacks[i].name;
    }
    return key;
  }

  function getPluginDir() {
    return pluginApi?.pluginDir || "";
  }

  function getPackDirectory(key) {
    const dir = getPluginDir();
    return dir ? (dir + "/sounds/" + key) : "";
  }

  function getBinaryPath() {
    const dir = getPluginDir();
    return dir ? (dir + "/bin/mech-sound") : "";
  }

  // Engine process
  Process {
    id: soundProc
    stdinEnabled: true
    running: false

    command: {
      const bin = root.getBinaryPath();
      if (!bin) return [];
      const packPath = root.getPackDirectory(root.soundPack);
      const args = [bin, "--pack", packPath, "--volume", String(root.volume)];
      if (root.inputDevice && root.inputDevice.length > 0) {
        args.push("--device", root.inputDevice);
      }
      if (!root.enabledSound) {
        args.push("--mute");
      }
      return args;
    }

    stdout: SplitParser {
      onRead: data => {
        Logger.d("MechKeys", "stdout: " + data);
      }
    }

    stderr: SplitParser {
      onRead: data => {
        Logger.d("MechKeys", "stderr: " + data);
      }
    }

    onRunningChanged: {
      Logger.i("MechKeys", "mech-sound running: " + running);
    }

    onExited: exitCode => {
      Logger.w("MechKeys", "mech-sound process exited with code " + exitCode);
      if (root.enabledSound) {
        restartTimer.restart();
      }
    }
  }

  // Device discovery process
  Process {
    id: listDevicesProc
    running: false
    command: {
      const bin = root.getBinaryPath();
      return bin ? [bin, "--list-devices"] : [];
    }

    stdout: StdioCollector {
      onStreamFinished: {
        try {
          const list = JSON.parse(text);
          if (Array.isArray(list)) {
            const devOptions = [
              { key: "", name: "Auto-detect all keyboards (Recommended)" }
            ];
            for (var i = 0; i < list.length; i++) {
              devOptions.push({
                key: list[i].path,
                name: list[i].name + " (" + list[i].path + ")"
              });
            }
            root.availableDevices = devOptions;
            Logger.i("MechKeys", "Discovered " + list.length + " keyboard devices");
          }
        } catch (e) {
          Logger.w("MechKeys", "Could not parse keyboard list: " + e);
        }
      }
    }
  }

  // Auto-build binary if missing
  Process {
    id: buildProc
    running: false
    command: ["make", "-C", root.getPluginDir() + "/backend"]

    onExited: exitCode => {
      if (exitCode === 0) {
        Logger.i("MechKeys", "Backend built successfully");
        startEngine();
      } else {
        Logger.e("MechKeys", "Backend build failed with code " + exitCode);
      }
    }
  }

  Timer {
    id: restartTimer
    interval: 2500
    repeat: false
    onTriggered: {
      if (!soundProc.running) {
        startEngine();
      }
    }
  }

  function startEngine() {
    const bin = getBinaryPath();
    if (!bin) return;

    // Check if binary exists or build it
    soundProc.running = true;
    listDevicesProc.running = true;
  }

  Component.onCompleted: {
    startEngine();
  }

  Component.onDestruction: {
    if (soundProc.running) {
      soundProc.write("QUIT\n");
      soundProc.running = false;
    }
  }

  function sendCommand(cmd) {
    if (soundProc.running) {
      soundProc.write(cmd + "\n");
    }
  }

  function toggleSound() {
    enabledSound = !enabledSound;
    saveSettings();
    sendCommand("MUTE " + (enabledSound ? "0" : "1"));

    if (enableToasts) {
      const title = pluginApi?.tr("plugin.name") || "Mechanical Keyboard";
      const msg = enabledSound
        ? (pluginApi?.tr("toast.enabled") || "Typing sounds enabled")
        : (pluginApi?.tr("toast.muted") || "Typing sounds muted");
      ToastService.showNotice(title, msg, enabledSound ? "keyboard" : "keyboard-off");
    }
  }

  function cycleSoundPack() {
    var currentIndex = 0;
    for (var i = 0; i < soundPacks.length; i++) {
      if (soundPacks[i].key === soundPack) {
        currentIndex = i;
        break;
      }
    }
    var nextIndex = (currentIndex + 1) % soundPacks.length;
    setSoundPack(soundPacks[nextIndex].key);
  }

  function setSoundPack(packKey) {
    soundPack = packKey;
    saveSettings();
    const packPath = getPackDirectory(packKey);
    sendCommand("PACK " + packPath);

    if (enableToasts) {
      const title = pluginApi?.tr("plugin.name") || "Mechanical Keyboard";
      const packName = getSoundPackName(packKey);
      const msg = (pluginApi?.tr("toast.switched") || "Switched to {pack}").replace("{pack}", packName);
      ToastService.showNotice(title, msg, "keyboard");
    }
  }

  function setVolume(vol) {
    volume = vol;
    saveSettings();
    sendCommand("VOLUME " + volume);
  }

  function setInputDevice(dev) {
    if (inputDevice !== dev) {
      inputDevice = dev;
      saveSettings();
      // Restart engine process with updated device argument
      soundProc.running = false;
      Qt.callLater(() => {
        soundProc.running = true;
      });
    }
  }

  function testSound() {
    // Send a test sound click
    sendCommand("PACK " + getPackDirectory(soundPack));
    sendCommand("VOLUME " + volume);
    sendCommand("MUTE 0");
    sendCommand("TEST");
    const title = pluginApi?.tr("plugin.name") || "Mechanical Keyboard";
    ToastService.showNotice(title, "Profile active: " + getSoundPackName(soundPack), "keyboard");
  }

  function saveSettings() {
    if (!pluginApi || !pluginApi.pluginSettings) return;
    pluginApi.pluginSettings.enabledSound = root.enabledSound;
    pluginApi.pluginSettings.soundPack = root.soundPack;
    pluginApi.pluginSettings.volume = root.volume;
    pluginApi.pluginSettings.inputDevice = root.inputDevice;
    pluginApi.pluginSettings.showBarIcon = root.showBarIcon;
    pluginApi.pluginSettings.enableToasts = root.enableToasts;
    pluginApi.saveSettings();
  }
}
