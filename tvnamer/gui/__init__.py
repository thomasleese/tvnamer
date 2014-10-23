from PySide import QtCore, QtGui


class MainWindow(QtGui.QWidget):
    pass


def main():
    app = QtGui.QApplication(sys.argv)
    window = MainWindow()
    sys.exit(app.exec_())
