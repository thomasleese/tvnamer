import sys

from PySide import QtCore, QtGui


class DirectoryDropWidget(QtGui.QLabel):
    def __init__(self):
        super().__init__()
        self.setAcceptDrops(True)
        self.setStyleSheet("background-color:black;")
        self.setText("Hello.")

    def dragEnterEvent(self, e):
        if e.mimeData().hasUrls() and len(e.mimeData().urls()) == 1 and \
                all(map(QtCore.QUrl.isLocalFile, e.mimeData().urls())):
            e.accept()
        else:
            e.ignore()

    def dropEvent(self, e):
        url = e.mimeData().urls()[0]
        print(url)


class MainWindow(QtGui.QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("TVNamer")
        self.setMinimumSize(480, 480)

        layout = QtGui.QGridLayout()
        layout.addWidget(DirectoryDropWidget(), 0, 0)
        layout.setColumnStretch(0, 1)
        layout.setRowStretch(0, 1)
        self.setLayout(layout)

        self.show()


def main():
    app = QtGui.QApplication(sys.argv)
    window = MainWindow()
    sys.exit(app.exec_())
