#include <QNetworkReply>
#include <QXmlStreamReader>

#include <QDebug>

#include "thetvdb.h"

static int sID = 0;

struct EpisodeDef {
    int seasonId;
    int seasonNum;
    int episodeId;
    QString episodeName;
    int episodeNum;
};


class TheTVDBPrivate {
public:
    QNetworkAccessManager netAccessMgr;
};


TheTVDB::TheTVDB(QObject *parent) :
    QObject(parent), d(new TheTVDBPrivate) {

}

TheTVDB::~TheTVDB() {

}

int TheTVDB::searchShows(QString name) {
    sID++;

    QString url = "http://www.thetvdb.com/api/GetSeries.php?seriesname=" + name;
    QNetworkReply *reply = d->netAccessMgr.get(QNetworkRequest(url));
    reply->setProperty("id", QVariant(sID));
    connect(reply, &QNetworkReply::finished, this, &TheTVDB::on_reply_finished);

    return sID;
}

void TheTVDB::getSeasons(Show show) {
    QString key = QSettings(this).value("api_key").toString();
    QString url = "http://www.thetvdb.com/api/" + key + "/series/" + QString::number(show.id) + "/all/en.xml";
    QNetworkReply *reply = d->netAccessMgr.get(QNetworkRequest(url));
    reply->setProperty("show", QVariant::fromValue(show));
    connect(reply, &QNetworkReply::finished, this, &TheTVDB::on_reply_finished);
}

void TheTVDB::getEpisodes(Season season) {
    QString key = QSettings(this).value("api_key").toString();
    QString url = "http://www.thetvdb.com/api/" + key + "/series/" + QString::number(season.show.id) + "/all/en.xml";
    QNetworkReply *reply = d->netAccessMgr.get(QNetworkRequest(url));
    reply->setProperty("season", QVariant::fromValue(season));
    connect(reply, &QNetworkReply::finished, this, &TheTVDB::on_reply_finished);
}

void TheTVDB::on_reply_finished() {
    QString key = QSettings(this).value("api_key").toString();
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QXmlStreamReader xml(reply);

    if (reply->url().path() == "/api/GetSeries.php") {
        QList<Show> shows;
        Show curShow;

        while (!xml.atEnd()) {
            xml.readNext();

            if (xml.name() == "Series") {
                if (xml.isStartElement()) {
                    curShow = Show();
                } else if (xml.isEndElement()) {
                    shows.append(curShow);
                }
            } else if (xml.name() == "seriesid") {
                curShow.id = xml.readElementText().toInt();
            } else if (xml.name() == "SeriesName") {
                curShow.name = xml.readElementText();
            }
        }

        emit foundShows(reply->property("id").toInt(), shows);
    } else if (reply->url().path().startsWith("/api/" + key + "/series/")) {
        QList<EpisodeDef> episodesDefs;
        EpisodeDef curEpisodeDef;

        while (!xml.atEnd()) {
            xml.readNext();

            if (xml.name() == "Episode") {
                if (xml.isStartElement()) {
                    curEpisodeDef = EpisodeDef();
                } else if (xml.isEndElement()) {
                    episodesDefs.append(curEpisodeDef);
                }
            } else if (xml.name() == "SeasonNumber") {
                curEpisodeDef.seasonNum = xml.readElementText().toInt();
            } else if (xml.name() == "seasonid") {
                curEpisodeDef.seasonId = xml.readElementText().toInt();
            } else if (xml.name() == "id") {
                curEpisodeDef.episodeId = xml.readElementText().toInt();
            } else if (xml.name() == "EpisodeNumber") {
                curEpisodeDef.episodeNum = xml.readElementText().toInt();
            } else if (xml.name() == "EpisodeName") {
                curEpisodeDef.episodeName = xml.readElementText();
            }
        }

        if (!reply->property("show").isNull()) {
            // clearly we are meant to be listing all the seasons for a show :)
            Show show = reply->property("show").value<Show>();
            QList<Season> seasons;

            foreach (EpisodeDef def, episodesDefs) {
                bool inList = false;
                foreach (Season season, seasons) {
                    if (season.id == def.seasonId) {
                        inList = true;
                    }
                }

                // for this, we don't really want the specials
                if (!inList && def.seasonNum > 0) {
                    Season season;
                    season.id = def.seasonId;
                    season.num = def.seasonNum;
                    season.show = show;
                    seasons.append(season);
                }
            }

            emit foundSeasons(seasons);
        } else if (!reply->property("season").isNull()) {
            Season season = reply->property("season").value<Season>();
            QList<Episode> episodes;

            foreach (EpisodeDef def, episodesDefs) {
                if (def.seasonId == season.id) {
                    Episode episode;
                    episode.id = def.episodeId;
                    episode.name = def.episodeName;
                    episode.num = def.episodeNum;
                    episode.season = season;
                    episodes.append(episode);
                }
            }

            emit foundEpisodes(episodes);
        }
    } else {
        qDebug() << reply->url().toString();
    }
}
