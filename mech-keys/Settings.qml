import QtQuick
import QtQuick.Layouts
import qs.Commons
import qs.Widgets

ColumnLayout {
  id: root

  property var pluginApi: null

  readonly property var mainInstance: pluginApi?.mainInstance
  readonly property var cfg: pluginApi?.pluginSettings || ({})
  readonly property var defaults: pluginApi?.manifest?.metadata?.defaultSettings || ({})

  property bool enabledSound: mainInstance ? mainInstance.enabledSound : (cfg.enabledSound ?? defaults.enabledSound ?? true)
  property string soundPack: mainInstance ? mainInstance.soundPack : (cfg.soundPack ?? defaults.soundPack ?? "boxjade")
  property real volume: mainInstance ? mainInstance.volume : (cfg.volume ?? defaults.volume ?? 0.85)
  property string inputDevice: mainInstance ? mainInstance.inputDevice : (cfg.inputDevice ?? defaults.inputDevice ?? "")
  property bool enableToasts: mainInstance ? mainInstance.enableToasts : (cfg.enableToasts ?? defaults.enableToasts ?? true)

  spacing: Style.marginL

  ColumnLayout {
    spacing: Style.marginM
    Layout.fillWidth: true

    NToggle {
      label: pluginApi?.tr("settings.enabled.label") || "Enable Mechanical Typing Sounds"
      description: pluginApi?.tr("settings.enabled.desc") || "Play authentic mechanical switch sound feedback whenever typing"
      checked: root.enabledSound
      onToggled: checked => {
        root.enabledSound = checked;
        if (mainInstance) {
          mainInstance.enabledSound = checked;
          mainInstance.sendCommand("MUTE " + (checked ? "0" : "1"));
        }
      }
    }

    NComboBox {
      Layout.fillWidth: true
      label: pluginApi?.tr("settings.soundPack.label") || "Switch Sound Profile"
      description: pluginApi?.tr("settings.soundPack.desc") || "Choose your preferred mechanical keyboard switch type"

      model: mainInstance ? mainInstance.soundPacks : [
        { key: "boxjade", name: "Kailh Box Jade (Sharp Clicky)" },
        { key: "creams", name: "NovelKeys Creams (Deep Thock)" },
        { key: "banana-split", name: "Banana Split Lubed (Buttery Linear)" },
        { key: "cherry-blue", name: "Cherry MX Blue (Iconic Clicky)" },
        { key: "akko-lavender", name: "Akko Lavender Purple (Tactile Bump)" },
        { key: "cherry-brown", name: "Cherry MX Brown (Classic Tactile)" },
        { key: "topre", name: "Topre Purple Hybrid (Deep Thock-Pop)" },
        { key: "speed-silver", name: "Cherry MX Speed Silver (Fast Linear)" },
        { key: "cherry-red", name: "Cherry MX Red (Smooth Linear)" },
        { key: "razer-green", name: "Razer Green (Loud Clicky)" }
      ]

      currentKey: root.soundPack
      onSelected: key => {
        root.soundPack = key;
        if (mainInstance) {
          mainInstance.setSoundPack(key);
        }
      }
    }

    NValueSlider {
      label: pluginApi?.tr("settings.volume.label") || "Sound Volume"
      description: pluginApi?.tr("settings.volume.desc") || "Adjust playback loudness of typing sound effects"
      from: 0.0
      to: 1.0
      stepSize: 0.05
      value: root.volume
      defaultValue: defaults.volume ?? 0.85
      text: Math.round(root.volume * 100) + "%"

      onMoved: val => {
        root.volume = val;
        if (mainInstance) {
          mainInstance.setVolume(val);
        }
      }
    }

    NComboBox {
      Layout.fillWidth: true
      label: pluginApi?.tr("settings.inputDevice.label") || "Keyboard Device"
      description: pluginApi?.tr("settings.inputDevice.desc") || "Select specific keyboard hardware or auto-listen to all keyboards"

      model: mainInstance ? mainInstance.availableDevices : [
        { key: "", name: "Auto-detect all keyboards (Recommended)" }
      ]

      currentKey: root.inputDevice
      onSelected: key => {
        root.inputDevice = key;
        if (mainInstance) {
          mainInstance.setInputDevice(key);
        }
      }
    }

    NToggle {
      label: pluginApi?.tr("settings.enableToasts.label") || "Notification Toasts"
      description: pluginApi?.tr("settings.enableToasts.desc") || "Display feedback toast notifications when toggling sound or switching profiles"
      checked: root.enableToasts
      onToggled: checked => {
        root.enableToasts = checked;
        if (mainInstance) {
          mainInstance.enableToasts = checked;
        }
      }
    }

    RowLayout {
      spacing: Style.marginM
      Layout.fillWidth: true

      Item {
        Layout.fillWidth: true
      }

      NButton {
        text: pluginApi?.tr("settings.testButton.button") || "Test Switch Profile"
        backgroundColor: Color.mPrimary
        textColor: Color.mOnPrimary
        onClicked: {
          if (mainInstance) {
            mainInstance.testSound();
          }
        }
      }
    }
  }

  function saveSettings() {
    if (!pluginApi) return;
    pluginApi.pluginSettings.enabledSound = root.enabledSound;
    pluginApi.pluginSettings.soundPack = root.soundPack;
    pluginApi.pluginSettings.volume = root.volume;
    pluginApi.pluginSettings.inputDevice = root.inputDevice;
    pluginApi.pluginSettings.enableToasts = root.enableToasts;
    pluginApi.saveSettings();

    if (mainInstance) {
      mainInstance.saveSettings();
    }
    Logger.i("MechKeys", "Settings saved successfully");
  }
}
