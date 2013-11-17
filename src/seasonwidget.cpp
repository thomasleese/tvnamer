#include <QFileDialog>
#include <QDebug>

#include "thetvdb.h"
#include "seasonwidget.h"
#include "ui_seasonwidget.h"

QString findBestSeasonFile(QDir dir, Season season) {
    foreach (QFileInfo info, dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (info.fileName().contains(QString::number(season.num))) {
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
    
    QString num = episode.num < 10 ? "0" + QString::number(episode.num) : QString::number(episode.num);
    
    // File.S06E01
    foreach (QFileInfo info, list) {
        if (info.fileName().toLower().contains("e" + num)) {
            return info.absoluteFilePath();
        }
    }
    
    // File.06x01
    foreach (QFileInfo info, list) {
        if (info.fileName().contains(num)) {
            return info.absoluteFilePath();
        }
    }
    
    // File.6x1
    foreach (QFileInfo info, list) {
        if (info.fileName().contains(QString::number(episode.num))) {
            return info.absoluteFilePath();
        }
    }
    
    return QString();
}

SeasonWidget::SeasonWidget(Season season, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SeasonWidget) {
    ui->setupUi(this);
    
    if (!season.show.autoDir.isNull()) {
        QDir autoDir(season.show.autoDir);
        mDir = findBestSeasonFile(autoDir, season);
    }
    
    mDatabase = new TheTVDB(this);
    mDatabase->getEpisodes(season);
    connect(mDatabase, SIGNAL(foundEpisodes(QList<Episode>)), this, SLOT(on_mDatabase_foundEpisodes(QList<Episode>)));
}

SeasonWidget::~SeasonWidget() {
    delete ui;
}

void SeasonWidget::saveChanges() {
    for (int i = 0; i < ui->treeEpisodes->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = ui->treeEpisodes->topLevelItem(i);
        
        QString oldFilename = item->text(0);
        QString newFilename = item->text(1);
        
        if (!QFile(oldFilename).rename(newFilename)) {
            qDebug() << "failed to rename" << oldFilename << "to" << newFilename;
        }
        
        ui->textDirectory->setText(ui->textDirectory->text());
    }
}

void SeasonWidget::on_mDatabase_foundEpisodes(const QList<Episode> &episodes) {
    mEpisodes = episodes;
    ui->textDirectory->setText(mDir);
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
    foreach (Episode episode, mEpisodes) {
        QString oldFilename = findBestEpisodeFile(dir, episode);
        if (!oldFilename.isNull()) {
            QString ext = oldFilename.right(oldFilename.length() - oldFilename.lastIndexOf("."));
            QString num = episode.num < 10 ? "0" + QString::number(episode.num) : QString::number(episode.num);
            QString newFilename = dir.filePath(num + " - " + episode.name + ext);
            
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << oldFilename << newFilename);
            
            if (newFilename == oldFilename) {
                item->setTextColor(0, QColor("green"));
                item->setTextColor(1, QColor("green"));
            } else {
                item->setTextColor(0, QColor("red"));
                item->setTextColor(1, QColor("red"));
            }

            ui->treeEpisodes->addTopLevelItem(item);
        }
    }
    
    ui->treeEpisodes->resizeColumnToContents(0);
    ui->treeEpisodes->resizeColumnToContents(1);
}
