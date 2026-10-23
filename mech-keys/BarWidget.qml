import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Quickshell
import qs.Commons
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

  readonly property string screenName: screen ? screen.name : ""
  readonly property string barPosition: Settings.getBarPositionForScreen(screenName)
  readonly property bool isVertical: barPosition === "left" || barPosition === "right"
  readonly property real barHeight: Style.getBarHeightForScreen(screenName)
  readonly property real capsuleHeight: Style.getCapsuleHeightForScreen(screenName)
  readonly property real barFontSize: Style.getBarFontSizeForScreen(screenName)

  readonly property var mainInstance: pluginApi?.mainInstance
  readonly property bool enabledSound: mainInstance ? mainInstance.enabledSound : false
  readonly property string currentPack: mainInstance ? mainInstance.soundPack : "boxjade"

  readonly property real contentWidth: isVertical ? Style.capsuleHeight : Style.capsuleHeight
  readonly property real contentHeight: Style.capsuleHeight

  implicitWidth: contentWidth
  implicitHeight: contentHeight

  Layout.alignment: Qt.AlignVCenter

  Rectangle {
    id: visualCapsule
    anchors.centerIn: parent
    width: root.contentWidth
    height: root.contentHeight
    radius: Style.radiusM
    color: mouseArea.containsMouse ? Color.mHover : Style.capsuleColor
    border.color: root.enabledSound ? Color.mPrimary : Style.capsuleBorderColor
    border.width: root.enabledSound ? 1.5 : Style.capsuleBorderWidth

    NIcon {
      anchors.centerIn: parent
      icon: root.enabledSound ? "keyboard" : "keyboard-off"
      color: root.enabledSound ? Color.mPrimary : Qt.alpha(Color.mOnSurfaceVariant, 0.4)
    }

    MouseArea {
      id: mouseArea
      anchors.fill: parent
      hoverEnabled: true
      acceptedButtons: Qt.LeftButton | Qt.RightButton

      ToolTip.visible: containsMouse
      ToolTip.delay: 400
      ToolTip.text: {
        const packName = mainInstance ? mainInstance.getSoundPackName(root.currentPack) : root.currentPack;
        if (root.enabledSound) {
          const tpl = pluginApi?.tr("bar.tooltip-on") || "Mechanical Keyboard: Active ({pack})\nLeft-click: Toggle Mute | Right-click: Switch Profile";
          return tpl.replace("{pack}", packName);
        } else {
          return pluginApi?.tr("bar.tooltip-off") || "Mechanical Keyboard: Muted\nLeft-click: Enable Sound";
        }
      }

      onClicked: mouse => {
        if (mouse.button === Qt.LeftButton) {
          mainInstance?.toggleSound();
        } else if (mouse.button === Qt.RightButton) {
          mainInstance?.cycleSoundPack();
        }
      }
    }
  }
}
