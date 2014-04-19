#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QInputDialog>

#include "thetvdb.h"
#include "seasonwidget.h"
#include "findshowdialogue.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"


class MainWindowPrivate {
public:
    TheTVDB *database;
    QMap<QTreeWidgetItem *, SeasonWidget *> seasonWidgets;
    QMap<int, QString> tempAutoDirs;
    QSettings *settings;
};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), d(new MainWindowPrivate), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    d->settings = new QSettings(this);

    d->database = new TheTVDB(this);
    connect(d->database, &TheTVDB::foundSeasons, this, &MainWindow::on_database_foundSeasons);
    connect(d->database, &TheTVDB::foundShows, this, &MainWindow::on_database_foundShows);

    ui->treeSeasons->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeSeasons->addAction(ui->actionRemove);

    if (d->settings->value("api_key", QString()).toString().isEmpty()) {
        QMessageBox::warning(this, "Welcome!", "As with all great things, this piece of software comes with a warning.<br /><br />Once you press Save Changes, your files will be renamed: It is important that you have checked all the files before racing ahead and pressing Save Changes.<br /><br />To get started, hover over each of the 3 main buttons at the top and ensure you have read the tooltips.");

        bool ok;
        QString key = QInputDialog::getText(this, "API Key", "TheTVDB.com API Key", QLineEdit::Normal, QString(), &ok);
        if (!ok || key.isEmpty()) {
            QMessageBox::warning(this, "Invalid Key", "A valid key will need to be entered before using this software.");
        }

        d->settings->setValue("api_key", key);
    }
}

MainWindow::~MainWindow() {
    delete ui;
    delete d->settings;
    delete d->database;
    delete d;
}

void MainWindow::on_database_foundShows(int id, const QList<Show> &shows) {
    if (shows.count() > 0) {
        Show show = shows[0];
        show.autoDir = d->tempAutoDirs[id];
        d->database->getSeasons(show);
    }
}

void MainWindow::on_database_foundSeasons(const QList<Season> &seasons) {
    if (seasons.count() > 0) {
        Show show = seasons[0].show;

        QList<QTreeWidgetItem *> existingItems = ui->treeSeasons->findItems(show.name, Qt::MatchFixedString);
        QTreeWidgetItem *root = NULL;
        if (existingItems.count() > 0) {
            root = existingItems[0];
        } else {
            root = new QTreeWidgetItem(QStringList() << show.name);
        }

        foreach (Season season, seasons) {
            QString title = "Season " + QString::number(season.num);

            bool thereAlready = false;
            for (int i = 0; i < root->childCount(); i++) {
                if (root->child(i)->text(0) == title) {
                    thereAlready = true;
                }
            }

            if (!thereAlready) {
                QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << title);
                SeasonWidget *widget = new SeasonWidget(season, this);

                d->seasonWidgets[item] = widget;
                ui->stackedSeasons->addWidget(widget);

                root->addChild(item);
            }
        }

        ui->treeSeasons->addTopLevelItem(root);
    }
}

void MainWindow::on_treeSeasons_itemActivated(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)

    SeasonWidget *widget = d->seasonWidgets[item];
    if (widget != NULL) {
        ui->stackedSeasons->setCurrentWidget(widget);
    }
}

void MainWindow::on_actionAutoAddShows_triggered() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select TV Videos directory"));
    if (!dir.isNull()) {
        foreach (QFileInfo info, QDir(dir).entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs)) {
            int id = d->database->searchShows(info.fileName());
            d->tempAutoDirs[id] = info.absoluteFilePath();
        }
    }
}

void MainWindow::on_actionFindShow_triggered() {
    FindShowDialogue dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        d->database->getSeasons(dialog.getCurrentShow());
    }
}

void MainWindow::on_actionSaveChanges_triggered() {
    if (QMessageBox::question(this, "Are you sure?", "This will rename all the files currently open. Please ensure you have verified they are correct before continuing.", QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok) {
        for (int i = 0; i < ui->stackedSeasons->count(); i++) {
            SeasonWidget *widget = static_cast<SeasonWidget *>(ui->stackedSeasons->widget(i));
            widget->saveChanges();
        }

        QMessageBox::information(this, "Complete", "All files have been renamed.");
    }
}

void MainWindow::on_actionRemove_triggered() {
    QTreeWidgetItem *item = ui->treeSeasons->currentItem();
    removeItem(item);
}

void MainWindow::removeItem(QTreeWidgetItem *item) {
    for (int i = 0; i < item->childCount(); i++) {
        removeItem(item->child(i));
    }

    SeasonWidget *widget = d->seasonWidgets[item];
    d->seasonWidgets.remove(item);
    if (widget != NULL) {
        delete widget;
    }

    delete item;
}
