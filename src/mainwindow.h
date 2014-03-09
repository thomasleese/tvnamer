#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class TheTVDB;
class QSettings;
class SeasonWidget;
struct Season;
struct Show;

class QTreeWidgetItem;
class QNetworkReply;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {

    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_mDatabase_foundShows(int id, const QList<Show> &shows);
    void on_mDatabase_foundSeasons(const QList<Season> &seasons);
    void on_treeSeasons_itemActivated(QTreeWidgetItem *item, int column);
    void on_actionAutoAddShows_triggered();
    void on_actionFindShow_triggered();
    void on_actionSaveChanges_triggered();
    void on_actionRemove_triggered();
    
private:
    void removeItem(QTreeWidgetItem *item);
    
    Ui::MainWindow *ui;
    TheTVDB *mDatabase;
    QMap<QTreeWidgetItem *, SeasonWidget *> mSeasonWidgets;
    QMap<int, QString> mTempAutoDirs;
    QSettings *mSettings;

};

#endif // MAINWINDOW_H
