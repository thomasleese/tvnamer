#ifndef THETVDB_H
#define THETVDB_H

#include <QObject>
#include <QMetaType>
#include <QNetworkAccessManager>
#include <QSettings>


struct Show {
    int id;
    QString name;
    QString autoDir; // only used if an auto-dir is chosen
};

Q_DECLARE_METATYPE(Show)


struct Season {
    Show show;
    int id;
    int num;
};

Q_DECLARE_METATYPE(Season)


struct Episode {
    Season season;
    int id;
    QString name;
    int num;
};

Q_DECLARE_METATYPE(Episode)


class TheTVDB : public QObject {
    Q_OBJECT

public:
    explicit TheTVDB(QObject *parent = 0);

    int searchShows(QString name);
    void getSeasons(Show show);
    void getEpisodes(Season season);

private:
    class TheTVDBPrivate *d;

private slots:
    void on_reply_finished();

signals:
    void foundShows(int id, const QList<Show> &shows);
    void foundSeasons(const QList<Season> &seasons);
    void foundEpisodes(const QList<Episode> &episodes);

};

#endif // THETVDB_H
