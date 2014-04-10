#ifndef SEASONWIDGET_H
#define SEASONWIDGET_H

#include <QWidget>

class TheTVDB;

struct Season;
struct Episode;

namespace Ui {
    class SeasonWidget;
}

class SeasonWidget : public QWidget {
    Q_OBJECT

public:
    explicit SeasonWidget(Season season, QWidget *parent = 0);
    ~SeasonWidget();

    void saveChanges();

private slots:
    void on_mDatabase_foundEpisodes(const QList<Episode> &episodes);
    void on_btnBrowse_clicked();
    void on_textDirectory_textChanged(const QString &arg1);

private:
    Ui::SeasonWidget *ui;
    class SeasonWidgetPrivate*d;
};

#endif // SEASONWIDGET_H
