#ifndef FINDSHOWSDIALOGUE_H
#define FINDSHOWSDIALOGUE_H

#include <QDialog>

struct Show;
class TheTVDB;

class QModelIndex;

namespace Ui {
    class FindShowDialogue;
}

class FindShowDialogue : public QDialog
{
    Q_OBJECT
    
public:
    explicit FindShowDialogue(QWidget *parent = 0);
    ~FindShowDialogue();
    
    Show getCurrentShow() const;
    
private slots:
    void on_mDatabase_foundShows(int id, const QList<Show> &shows);
    void on_textShow_textEdited(const QString &arg1);
    void on_listShows_doubleClicked(const QModelIndex &index);
    void on_FindShowDialogue_accepted();
    void on_btnBrowse_clicked();
    
private:
    Ui::FindShowDialogue *ui;
    TheTVDB *mDatabase;
    QList<Show> mShows;
    
};

#endif // FINDSHOWSDIALOGUE_H
