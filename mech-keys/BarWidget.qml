import QtQuick
import Quickshell
import qs.Commons
import qs.Modules.Bar.Extras
import qs.Services.UI
import qs.Widgets

Item {
  id: root

  property var pluginApi: null

  property ShellScreen screen
  property string widgetId: ""
  property string section: ""
  property int sectionWidgetIndex: -1
  property int sectionWidgetsCount: 0

  readonly property var mainInstance: pluginApi?.mainInstance
  readonly property bool enabledSound: mainInstance ? mainInstance.enabledSound : false
  readonly property string currentPack: mainInstance ? mainInstance.soundPack : "creams"
  readonly property string packName: mainInstance ? mainInstance.getSoundPackName(currentPack) : currentPack

  implicitWidth: pill.width
  implicitHeight: pill.height

  BarPill {
    id: pill

    screen: root.screen
    oppositeDirection: BarService.getPillDirection(root)

    icon: root.enabledSound ? "keyboard" : "keyboard-off"
    customIconColor: root.enabledSound ? Color.mPrimary : Qt.alpha(Color.mOnSurfaceVariant, 0.4)
    customTextColor: Color.mPrimary

    text: ""

    tooltipText: {
      if (root.enabledSound) {
        var str = pluginApi?.tr("bar.tooltip-on") || "Mechanical Keyboard: Active ({pack})\n• Left-click: Open Settings\n• Right-click: Mute / Unmute\n• Scroll: Adjust Volume";
        return str.replace("{pack}", root.packName);
      } else {
        return pluginApi?.tr("bar.tooltip-off") || "Mechanical Keyboard: Muted\n• Left-click: Open Settings\n• Right-click: Unmute";
      }
    }

    onClicked: {
      if (pluginApi && pluginApi.manifest) {
        BarService.openPluginSettings(root.screen, pluginApi.manifest);
      }
    }

    onRightClicked: {
      mainInstance?.toggleSound();
    }

    onWheel: function(delta) {
      if (!mainInstance) return;
      var step = (delta > 0 ? 0.05 : -0.05);
      var newVol = Math.max(0.0, Math.min(1.0, mainInstance.volume + step));
      mainInstance.setVolume(newVol);
    }
  }
}
