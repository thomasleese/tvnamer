#include <QFileDialog>
#include <QInputDialog>
#include <QDebug>

#include "thetvdb.h"
#include "seasonwidget.h"
#include "ui_seasonwidget.h"

static QString pad(int number) {
    if (number < 10) {
        return "0" + QString::number(number);
    }

    return QString::number(number);
}

QString findBestSeasonFile(QDir dir, Season season) {
    foreach (QFileInfo info, dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (info.fileName().contains(QString::number(season.num))) {
            return info.absoluteFilePath();
        } else if (info.fileName().contains(QChar(season.num + 64))) {
            return info.absoluteFilePath();
        }
    }

    return QString();
}

QString findBestEpisodeFile(QDir dir, Episode episode) {
    QStringList filters;
    filters << "*.avi" << "*.mkv" << "*.m4v" << "*.mp4" << "*.webm" << "*.ogg"
            << "*.ogv" << "*.mpg" << "*.wmv" << "*.vob" << "*.flv" << "*.mov"
            << "*.rm" << "*.swf" << "*.3gp" << "*.wtv";

    QFileInfoList list = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

    QString num = pad(episode.num);
    QStringList prospectiveFiles;

    // Episode Name
    foreach (QFileInfo info, list) {
        if (info.fileName().toLower().contains(episode.name.toLower())) {
            return info.absoluteFilePath(); // this is clearly a proper match
        }
    }

    // File.S06E01
    foreach (QFileInfo info, list) {
        if (info.fileName().toLower().contains("e" + num)) {
            prospectiveFiles << info.absoluteFilePath();
        }
    }

    // File.06x01
    foreach (QFileInfo info, list) {
        if (info.fileName().contains(num)) {
            prospectiveFiles << info.absoluteFilePath();
        }
    }

    // File.6x1
    foreach (QFileInfo info, list) {
        if (info.fileName().contains(QString::number(episode.num))) {
            prospectiveFiles << info.absoluteFilePath();
        }
    }

    prospectiveFiles = prospectiveFiles.toSet().toList();

    // pick the result
    if (prospectiveFiles.size() == 1) {
        return prospectiveFiles[0];
    } else if (prospectiveFiles.size() == 0) {
        return QString();
    } else {
        bool ok;
        QString item = QInputDialog::getItem(0, "Pick best file", episode.season.show.name + " - " + pad(episode.season.num) + " - " + pad(episode.num) + " - " + episode.name, prospectiveFiles, 0, true, &ok);
        if (ok) {
            return item;
        } else {
            return QString();
        }
    }
}


class SeasonWidgetPrivate {
public:
    QList<Episode> episodes;
    TheTVDB *database;
    QString dir;
};


SeasonWidget::SeasonWidget(Season season, QWidget *parent) :
    QWidget(parent), d(new SeasonWidgetPrivate), ui(new Ui::SeasonWidget) {
    ui->setupUi(this);

    if (!season.show.autoDir.isNull()) {
        QDir autoDir(season.show.autoDir);
        d->dir = findBestSeasonFile(autoDir, season);
    }

    d->database = new TheTVDB(this);
    d->database->getEpisodes(season);
    connect(d->database, SIGNAL(foundEpisodes(QList<Episode>)), this, SLOT(on_mDatabase_foundEpisodes(QList<Episode>)));
}

SeasonWidget::~SeasonWidget() {
    delete ui;
    delete d;
}

void SeasonWidget::saveChanges() {
    for (int i = 0; i < ui->treeEpisodes->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = ui->treeEpisodes->topLevelItem(i);

        QString oldFilename = item->text(1);
        QString newFilename = item->text(2);

        if (!oldFilename.isEmpty() && !newFilename.isEmpty() && oldFilename != newFilename) {
            qDebug() << "renaming" << oldFilename << "to" << newFilename;
            if (!QFile(oldFilename).rename(newFilename)) {
                qDebug() << "failed to rename" << oldFilename << "to" << newFilename;
            }
        }

        ui->textDirectory->setText(ui->textDirectory->text());
    }
}

void SeasonWidget::on_mDatabase_foundEpisodes(const QList<Episode> &episodes) {
    d->episodes = episodes;
    ui->textDirectory->setText(d->dir);
    setEnabled(true);
}

void SeasonWidget::on_btnBrowse_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Videos Directory");
    if (!dir.isNull()) {
        ui->textDirectory->setText(dir);
    }
}

void SeasonWidget::on_textDirectory_textChanged(const QString &path) {
    ui->treeEpisodes->model()->removeRows(0, ui->treeEpisodes->topLevelItemCount());

    QDir dir(path);
    foreach (Episode episode, d->episodes) {
        QString oldFilename = findBestEpisodeFile(dir, episode);

        QTreeWidgetItem *item;
        if (oldFilename.isNull()) {
            item = new QTreeWidgetItem(QStringList() << pad(episode.num) + " - " + episode.name << path << path);
            item->setTextColor(1, QColor("red"));
            item->setTextColor(2, QColor("red"));
        } else {
            QString ext = oldFilename.right(oldFilename.length() - oldFilename.lastIndexOf("."));
            QString num = episode.num < 10 ? "0" + QString::number(episode.num) : QString::number(episode.num);
            QString newFilename = dir.filePath(num + " - " + episode.name + ext);

            item = new QTreeWidgetItem(QStringList() << pad(episode.num) + " - " + episode.name << oldFilename << newFilename);
            if (newFilename == oldFilename) {
                item->setTextColor(1, QColor("green"));
                item->setTextColor(2, QColor("green"));
            } else {
                item->setTextColor(1, QColor("orange"));
                item->setTextColor(2, QColor("orange"));
            }
        }

        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
        ui->treeEpisodes->addTopLevelItem(item);
    }

    ui->treeEpisodes->resizeColumnToContents(0);
    ui->treeEpisodes->resizeColumnToContents(1);
    ui->treeEpisodes->resizeColumnToContents(2);
}
