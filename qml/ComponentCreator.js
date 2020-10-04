function createMessageModal(parent, title, body) {
    var comp = Qt.createComponent("qrc:/qml/Modals/MessageModal.qml")
    comp.createObject(parent, {title, body})
}

function createNotificationModal(parent, title, body) {
    var comp = Qt.createComponent("qrc:/qml/Modals/NotificationModal.qml")
    comp.createObject(parent, {title, body})
}
