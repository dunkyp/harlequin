import QtQuick 2.0

Item {
    id: colourWheel
    anchors.centerIn: parent
    property int parentHeight: parent.height
    property real brightness: 70
    enum Space {
        HSP = 0, LAB = 1
    }
    property var activeSpace: ColourWheel.Space.HSP

    height: parent.height - 6
    width: parent.height - 6
    antialiasing: true
    layer.enabled: true
    layer.effect: ShaderEffect {
        property var shaderMap: ["qrc:/shaders/hsp_wheel_fragment.glsl",
                                 "qrc:/shaders/lab_wheel_fragment.glsl"];
        property variant brightness: colourWheel.brightness
        vertexShader: "qrc:/shaders/colour_wheel_vertex.glsl"
        fragmentShader: shaderMap[activeSpace]
    }
}
