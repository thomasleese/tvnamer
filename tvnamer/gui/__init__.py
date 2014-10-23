import sys

from PySide import QtCore, QtGui

from ..renamer import Renamer


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


class SetUpRenamerDialogue(QtGui.QDialog):
    def __init__(self, directory, parent=None):
        super().__init__(parent)
        self.setWindowTitle(directory)

        self.directory = directory

        self.api_key_text = QtGui.QLineEdit()
        self.api_key_text.setPlaceholderText("API key")

        self.input_regex_text = QtGui.QLineEdit()
        self.input_regex_text.setPlaceholderText("Input Regular Expression")

        self.output_format_text = QtGui.QLineEdit()
        self.output_format_text.setPlaceholderText("Output Format")

        self.extras_form = QtGui.QWidget()
        self.series_name_text = QtGui.QLineEdit()
        self.season_number_text = QtGui.QSpinBox()
        self.season_number_text.setMinimum(0)
        self.episode_number_text = QtGui.QSpinBox()
        self.episode_number_text.setMinimum(0)
        self.episode_name_text = QtGui.QLineEdit()
        self.extras_form_layout = QtGui.QFormLayout(self)
        self.extras_form_layout.addRow("Series Name", self.series_name_text)
        self.extras_form_layout.addRow("Season Number", self.season_number_text)
        self.extras_form_layout.addRow("Episode Number", self.episode_number_text)
        self.extras_form_layout.addRow("Episode Name", self.episode_name_text)
        self.extras_form.setLayout(self.extras_form_layout)

        self.button_box = QtGui.QDialogButtonBox(QtGui.QDialogButtonBox.Ok|QtGui.QDialogButtonBox.Cancel, QtCore.Qt.Horizontal, self)
        self.button_box.accepted.connect(self.accept)
        self.button_box.rejected.connect(self.reject)

        layout = QtGui.QVBoxLayout()
        layout.addWidget(self.api_key_text)
        layout.addWidget(self.input_regex_text)
        layout.addWidget(self.output_format_text)
        layout.addWidget(self.extras_form)
        layout.addWidget(self.button_box)
        self.setLayout(layout)

    @staticmethod
    def create_left_column(text):
        item = QtGui.QTableWidgetItem(text)
        flags = QtCore.Qt.ItemFlags()
        flags != QtCore.Qt.ItemIsEnabled
        item.setFlags(flags)
        return item

    def build_renamer(self):
        api_key = self.api_key_text.text()
        input_regex = self.input_regex_text.text()
        output_format = self.output_format_text.text()
        default_params = {
            "series_name": self.series_name_text.text(),
            "season_number": self.season_number_text.value(),
            "episode_number": self.episode_number_text.value(),
            "episode_name": self.episode_name_text.text(),
        }

        return Renamer(api_key, self.directory, input_regex, output_format,
                       default_params)


class MainWindow(QtGui.QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("TVNamer")
        self.setMinimumSize(500, 400)

        self.menu_bar = self.build_menu_bar()
        self.setMenuBar(self.menu_bar)

        self.drop_target = VideoFileFolderDropTarget(self)
        self.drop_target.dropped.connect(self.on_drop_target_dropped)
        self.setCentralWidget(self.drop_target)

        self.status_bar = QtGui.QStatusBar(self)
        self.setStatusBar(self.status_bar)

        self.show()

    def build_menu_bar(self):
        menu_bar = QtGui.QMenuBar(self)
        tools_menu = menu_bar.addMenu("Tools")
        tools_menu.addAction(QtGui.QAction("Set API Key", self))
        return menu_bar

    @QtCore.Slot(str)
    def on_drop_target_dropped(self, path):
        dialogue = SetUpRenamerDialogue(path)
        if dialogue.exec_() == QtGui.QDialog.DialogCode.Accepted:
            renamer = dialogue.build_renamer()
            print(list(renamer.table))


def main():
    app = QtGui.QApplication(sys.argv)
    window = MainWindow()
    sys.exit(app.exec_())
