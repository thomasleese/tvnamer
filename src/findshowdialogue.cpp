#include <QFileDialog>

#include "thetvdb.h"
#include "findshowdialogue.h"
#include "ui_findshowdialogue.h"

FindShowDialogue::FindShowDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindShowDialogue) {
    ui->setupUi(this);
    
    connect(this, SIGNAL(accepted()), this, SLOT(on_FindShowDialogue_accepted()));
    
    mDatabase = new TheTVDB(this);
    connect(mDatabase, SIGNAL(foundShows(int,QList<Show>)), this, SLOT(on_mDatabase_foundShows(int,QList<Show>)));
}

FindShowDialogue::~FindShowDialogue() {
    delete ui;
    delete mDatabase;
}

Show FindShowDialogue::getCurrentShow() const {
    Show show = mShows[ui->listShows->currentRow()];
    show.autoDir = ui->textAutoDir->text();
    return show;
}

void FindShowDialogue::on_mDatabase_foundShows(int id, const QList<Show> &shows) {
    Q_UNUSED(id)
    
    mShows = shows;
    
    ui->listShows->clear();
    foreach (Show show, mShows) {
        ui->listShows->addItem(show.name);
    }
}

void FindShowDialogue::on_textShow_textEdited(const QString &text) {
    mDatabase->searchShows(text);
}

void FindShowDialogue::on_listShows_doubleClicked(const QModelIndex &index) {
    Q_UNUSED(index)
    
    accept();
}

void FindShowDialogue::on_FindShowDialogue_accepted() {
    if (ui->listShows->currentRow() < 0) {
        setResult(QDialog::Rejected);
    }
}

void FindShowDialogue::on_btnBrowse_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose directory"));
    if (!dir.isNull()) {
        ui->textAutoDir->setText(dir);
    }
}
