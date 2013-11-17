#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>

#include "thetvdb.h"
#include "seasonwidget.h"
#include "findshowdialogue.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    
    mDatabase = new TheTVDB(this);
    connect(mDatabase, SIGNAL(foundSeasons(QList<Season>)), this, SLOT(on_mDatabase_foundSeasons(QList<Season>)));
    connect(mDatabase, SIGNAL(foundShows(int,QList<Show>)), this, SLOT(on_mDatabase_foundShows(int,QList<Show>)));
    
    ui->treeSeasons->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeSeasons->addAction(ui->actionRemove);
    
    mSettings = new QSettings("Tim32", "TV-Namer", this);
    if (mSettings->value("first_run", true).toBool()) {
        QMessageBox::warning(this, "Welcome!", "As with all great things, this piece of software comes with a warning.<br /><br />Once you press Save Changes, your files will be renamed: It is important that you have checked all the files before racing ahead and pressing Save Changes.<br /><br />To get started, hover over each of the 3 main buttons at the top and ensure you have read the tooltips.");
        mSettings->setValue("first_run", false);
    }
}

MainWindow::~MainWindow() {
    delete ui;
    delete mSettings;
    delete mDatabase;
}

void MainWindow::on_mDatabase_foundShows(int id, const QList<Show> &shows) {
    if (shows.count() > 0) {
        Show show = shows[0];
        show.autoDir = mTempAutoDirs[id];
        mDatabase->getSeasons(show);
    }
}

void MainWindow::on_mDatabase_foundSeasons(const QList<Season> &seasons) {
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
                
                mSeasonWidgets[item] = widget;
                ui->stackedSeasons->addWidget(widget);
                
                root->addChild(item);
            }
        }
        
        ui->treeSeasons->addTopLevelItem(root);
    }
}

void MainWindow::on_treeSeasons_itemActivated(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)
    
    SeasonWidget *widget = mSeasonWidgets[item];
    if (widget != NULL) {
        ui->stackedSeasons->setCurrentWidget(widget);
    }
}

void MainWindow::on_actionAutoAddShows_triggered() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select TV Videos directory"));
    if (!dir.isNull()) {
        foreach (QFileInfo info, QDir(dir).entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs)) {
            int id = mDatabase->searchShows(info.fileName());
            mTempAutoDirs[id] = info.absoluteFilePath();
        }
    }
}

void MainWindow::on_actionFindShow_triggered() {
    FindShowDialogue dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        mDatabase->getSeasons(dialog.getCurrentShow());
    }
}

void MainWindow::on_actionSaveChanges_triggered() {
    if (QMessageBox::question(this, "Are you sure?", "This will rename all the files currently open. Please ensure you have verified they are correct before continuing.", QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok) {
        for (int i = 0; i < ui->stackedSeasons->count(); i++) {
            SeasonWidget *widget = static_cast<SeasonWidget *>(ui->stackedSeasons->widget(i));
            widget->saveChanges();
        }
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
    
    SeasonWidget *widget = mSeasonWidgets[item];
    mSeasonWidgets.remove(item);
    if (widget != NULL) {
        delete widget;
    }
    
    delete item;
}
