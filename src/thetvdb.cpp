#include <QNetworkReply>
#include <QXmlStreamReader>

#include <QDebug>

#include "thetvdb.h"

#define API_KEY "enter your api key here"

static int sID = 0;

struct EpisodeDef {
    int seasonId;
    int seasonNum;
    int episodeId;
    QString episodeName;
    int episodeNum;
};

TheTVDB::TheTVDB(QObject *parent) :
    QObject(parent) {
}

int TheTVDB::searchShows(QString name) {
    sID++;
    
    QString url = "http://www.thetvdb.com/api/GetSeries.php?seriesname=" + name;
    QNetworkReply *reply = mNetAccessMgr.get(QNetworkRequest(url));
    reply->setProperty("id", QVariant(sID));
    connect(reply, SIGNAL(finished()), this, SLOT(on_reply_finished()));
    
    return sID;
}

void TheTVDB::getSeasons(Show show) {
    QString url = "http://www.thetvdb.com/api/" API_KEY "/series/" + QString::number(show.id) + "/all/en.xml";
    QNetworkReply *reply = mNetAccessMgr.get(QNetworkRequest(url));
    reply->setProperty("show", QVariant::fromValue(show));
    connect(reply, SIGNAL(finished()), this, SLOT(on_reply_finished()));
}

void TheTVDB::getEpisodes(Season season) {
    QString url = "http://www.thetvdb.com/api/" API_KEY "/series/" + QString::number(season.show.id) + "/all/en.xml";
    QNetworkReply *reply = mNetAccessMgr.get(QNetworkRequest(url));
    reply->setProperty("season", QVariant::fromValue(season));
    connect(reply, SIGNAL(finished()), this, SLOT(on_reply_finished()));
}

void TheTVDB::on_reply_finished() {
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
    } else if (reply->url().path().startsWith("/api/" API_KEY "/series/")) {
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
