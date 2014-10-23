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
    def __init__(self, directory, api_key, parent=None):
        super().__init__(parent)
        self.setWindowTitle(directory)
        self.setMinimumWidth(480)

        self.directory = directory
        self.api_key = api_key

        self.input_regex_text = QtGui.QLineEdit()
        self.input_regex_text.setPlaceholderText("Input Regular Expression")

        self.output_format_text = QtGui.QLineEdit()
        self.output_format_text.setPlaceholderText("Output Format")

        self.extras_form = QtGui.QGroupBox("Default Parameters")
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

    @property
    def renamer(self):
        input_regex = self.input_regex_text.text()
        output_format = self.output_format_text.text()
        default_params = {
            "series_name": self.series_name_text.text(),
            "season_number": self.season_number_text.value(),
            "episode_number": self.episode_number_text.value(),
            "episode_name": self.episode_name_text.text(),
        }

        # remove empty values
        default_params = {k: v for k, v in default_params.items() if v}

        return Renamer(self.api_key, self.directory, input_regex,
                       output_format, default_params)


class RenameTable(QtGui.QListWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.setStyleSheet("QListWidget::item { margin: 2px; }")

    def set_table(self, table):
        self.clear()

        for old, new in table:
            item = QtGui.QListWidgetItem("{}\n↳ {}".format(old, new))
            font = item.font()
            font.setPointSize(int(font.pointSize() * 1.3))
            item.setFont(font)
            self.addItem(item)


class InfoStatusBar(QtGui.QStatusBar):
    clear_clicked = QtCore.Signal()
    rename_clicked = QtCore.Signal()

    def __init__(self, parent=None):
        super().__init__(parent)

        self.clear_button = QtGui.QPushButton("Clear", self)
        self.clear_button.clicked.connect(lambda: self.clear_clicked.emit())
        self.addWidget(self.clear_button, 0)

        self.count_label = QtGui.QLabel("0 items", self)
        self.count_label.setAlignment(QtCore.Qt.AlignCenter)
        self.addWidget(self.count_label, 1)

        self.rename_button = QtGui.QPushButton("Rename", self)
        self.rename_button.clicked.connect(lambda: self.rename_clicked.emit())
        self.addWidget(self.rename_button, 0)

    def update(self, count):
        if count == 1:
            self.count_label.setText("1 item")
        else:
            self.count_label.setText("{} items".format(count))


class MainWindow(QtGui.QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("TV Namer")
        self.setMinimumSize(500, 400)

        self.init_menu_bar(self.menuBar())

        self.drop_target = VideoFileFolderDropTarget(self)
        self.drop_target.dropped.connect(self.on_drop_target_dropped)
        self.rename_table = RenameTable(self)
        self.stacked_widget = QtGui.QStackedWidget()
        self.stacked_widget.addWidget(self.drop_target)
        self.stacked_widget.addWidget(self.rename_table)
        self.stacked_widget.setCurrentWidget(self.drop_target)
        self.setCentralWidget(self.stacked_widget)

        self.status_bar = InfoStatusBar(self)
        self.status_bar.clear_clicked.connect(self.on_clear_clicked)
        self.status_bar.rename_clicked.connect(self.on_rename_clicked)
        self.status_bar.hide()
        self.setStatusBar(self.status_bar)

        self.show()

        self.settings = QtCore.QSettings()
        if not self.settings.contains("api_key"):
            self.set_api_key()

    def set_api_key(self):
        api_key, ok = QtGui.QInputDialog.getText(self, "Enter API key",
                                                "API key:",
                                                QtGui.QLineEdit.Normal,
                                                self.settings.value("api_key"))
        if ok:
            self.settings.setValue("api_key", api_key)

    def init_menu_bar(self, menu_bar=None):
        tools_menu = menu_bar.addMenu("Tools")
        api_key_action = QtGui.QAction("Set API Key", self)
        api_key_action.activated.connect(self.set_api_key)
        tools_menu.addAction(api_key_action)

    @QtCore.Slot(str)
    def on_drop_target_dropped(self, path):
        dialogue = SetUpRenamerDialogue(path, self.settings.value("api_key"))
        if dialogue.exec_() == QtGui.QDialog.DialogCode.Accepted:
            self.renamer = dialogue.renamer
            self.table = list(self.renamer.table)
            self.rename_table.set_table(self.table)
            self.stacked_widget.setCurrentWidget(self.rename_table)
            self.status_bar.update(len(self.table))
            self.status_bar.show()

    def on_clear_clicked(self):
        self.stacked_widget.setCurrentWidget(self.drop_target)
        self.status_bar.hide()

    def on_rename_clicked(self):
        self.renamer.perform_rename(self.table)

        box = QtGui.QMessageBox()
        box.setText("Your files have been renamed.")
        box.exec_()

        self.stacked_widget.setCurrentWidget(self.drop_target)
        self.status_bar.hide()


def main():
    app = QtGui.QApplication(sys.argv)
    app.setOrganizationName("Tom Leese")
    app.setOrganizationDomain("tomleese.me.uk")
    app.setApplicationName("TV Namer")
    window = MainWindow()
    sys.exit(app.exec_())
