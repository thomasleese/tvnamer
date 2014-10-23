import sys

from PySide import QtCore, QtGui


class VideoFileFolderDropTarget(QtGui.QLabel, QtCore.QObject):
    dropped = QtCore.Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setAcceptDrops(True)
        self.setStyleSheet("background: white;")
        self.setAlignment(QtCore.Qt.AlignCenter)
        self.setPixmap("tvnamer/gui/resources/drop_target.svg")

    def dragEnterEvent(self, e):
        if e.mimeData().hasUrls() and len(e.mimeData().urls()) == 1 and \
                all(map(QtCore.QUrl.isLocalFile, e.mimeData().urls())):
            e.accept()
        else:
            e.ignore()

    def dropEvent(self, e):
        url = e.mimeData().urls()[0]
        self.dropped.emit(url.path())


class MainWindow(QtGui.QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("TVNamer")
        self.setMinimumSize(512, 512)

        self.drop_target = VideoFileFolderDropTarget(self)
        self.drop_target.dropped.connect(self.on_drop_target_dropped)

        self.status_bar = QtGui.QStatusBar(self)

        self.setCentralWidget(self.drop_target)
        self.setStatusBar(self.status_bar)

        self.show()

    @QtCore.Slot(str)
    def on_drop_target_dropped(self, path):
        print(path)


def main():
    app = QtGui.QApplication(sys.argv)
    window = MainWindow()
    sys.exit(app.exec_())
