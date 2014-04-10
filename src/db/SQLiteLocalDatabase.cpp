#include "QtDBMigration.h"

#include "GlobalConfig.h"
#include "PlayList.h"
#include "SQLiteLocalDatabase.h"

const char *SQL_ENABLE_FOREIGN_KEYS = "pragma foreign_keys=on";

const char *SQL_INSERT_PLAYLIST = "insert into playlists(name, cover_path, type) values (?, ?, ?)";

const char *SQL_INSERT_PLAYLIST_IMAGES = "insert into playlist_images(playlist_id, image_id) values ("
        "?, (select id from images where hash = ?))";

const char *SQL_QUERY_PLAYLISTS = "select id, name, cover_path, type from playlists order by name";

const char *SQL_QUERY_PLAYLIST_ID_BY_NAME = "select id, name from playlists where name = ?";

const char *SQL_REMOVE_PLAYLIST_BY_ID = "delete from playlists where id = ?";

const char *SQL_UPDATE_PLAYLIST_BY_ID = "update playlists set name = ?, cover_path = ? where id = ?";

const char *SQL_QUERY_IMAGE_URLS_BY_PLAYLIST_ID = "select images.url, playlists.name from images "
        "left join playlist_images on images.id = playlist_images.image_id "
        "join playlists on playlists.id = playlist_images.playlist_id "
        "where playlists.id = ?";

const char *SQL_INSERT_IMAGE = "insert or ignore into images(hash, name, url) values (?, ?, ?)";

const char *SQL_QUERY_IMAGES_COUNT = "select count(id) from images";

const char *SQL_QUERY_IMAGE_BY_HASH = "select url from images where hash = ? limit 1";

SQLiteLocalDatabase::SQLiteLocalDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(GlobalConfig::instance()->localDatabasePath());
    if (!m_db.open()) {
        qWarning("Unable to local open database!");
        return;
    }

    QtDBMigration migration(GlobalConfig::instance()->migrationConfigPath());
    if (!migration.migrate()) {
        qCritical() << "Fail to migrate local database!";
    }

    m_db.exec(SQL_ENABLE_FOREIGN_KEYS);
}

QList<PlayListRecord *> SQLiteLocalDatabase::queryPlayListRecords()
{
    QList<PlayListRecord *> records;

    if (!m_db.isOpen()) {
        return records;
    }

    QSqlQuery q(SQL_QUERY_PLAYLISTS);
    while (q.next()) {
        int id = q.value(0).toInt();
        QString name = q.value(1).toString();
        QString coverPath = q.value(2).toString();
        PlayListRecord::PlayListType type =
            static_cast<PlayListRecord::PlayListType>(q.value(3).toInt());

        PlayListRecord *record = PlayListRecord::create(type, name, coverPath);
        if (record) {
            record->setId(id);
            records << record;
        }
    }

    return records;
}

QStringList SQLiteLocalDatabase::queryImageUrlsForLocalPlayListRecord(
    int playListId)
{
    QStringList imageUrls;

    if (!m_db.isOpen()) {
        return imageUrls;
    }

    QSqlQuery q;
    q.prepare(SQL_QUERY_IMAGE_URLS_BY_PLAYLIST_ID);
    q.addBindValue(playListId);
    if (!q.exec()) {
        qWarning() << q.lastError();
        qWarning() << q.lastQuery();
        return imageUrls;
    }

    while (q.next()) {
        QString imageUrl = q.value(0).toString();
        imageUrls << imageUrl;
    }

    return imageUrls;
}

bool SQLiteLocalDatabase::insertPlayListRecord(PlayListRecord *playListRecord)
{
    if (!m_db.isOpen()) {
        return false;
    }

    // Insert playlist
    QSqlQuery qInsertPlayList;
    qInsertPlayList.prepare(SQL_INSERT_PLAYLIST);
    qInsertPlayList.addBindValue(playListRecord->name());
    qInsertPlayList.addBindValue(playListRecord->coverPath());
    qInsertPlayList.addBindValue(static_cast<int>(playListRecord->type()));
    if (!qInsertPlayList.exec()) {
        qWarning() << qInsertPlayList.lastError()
                   << qInsertPlayList.lastQuery();
        return false;
    }
    QVariant playListId = qInsertPlayList.lastInsertId();

    // Only Local PlayList Record needs inserting related images
    if (playListRecord->type() == PlayListRecord::LocalPlayList) {
        // Insert images
        PlayList *pl = playListRecord->playList();
        PlayList::const_iterator it;
        for (it = pl->begin(); it != pl->end(); ++it) {
            insertImage(it->data());
        }

        // Insert relationship records
        QSqlQuery qInsertPlayListImages;
        qInsertPlayListImages.prepare(SQL_INSERT_PLAYLIST_IMAGES);
        QVariantList playListIds;
        QVariantList imageHashes;
        for (it = pl->begin(); it != pl->end(); ++it) {
            playListIds << playListId;
            imageHashes << it->data()->source()->hashStr();
        }
        qInsertPlayListImages.addBindValue(playListIds);
        qInsertPlayListImages.addBindValue(imageHashes);

        if (!qInsertPlayListImages.execBatch()) {
            qWarning() << qInsertPlayListImages.lastError()
                       << qInsertPlayListImages.lastQuery();
            return false;
        }
    }

    return true;
}

bool SQLiteLocalDatabase::removePlayListRecord(PlayListRecord *playListRecord)
{
    QSqlQuery q;
    q.prepare(SQL_REMOVE_PLAYLIST_BY_ID);
    q.addBindValue(playListRecord->id());
    return q.exec();
}

bool SQLiteLocalDatabase::updatePlayListRecord(PlayListRecord *playListRecord)
{
    QSqlQuery q;
    q.prepare(SQL_UPDATE_PLAYLIST_BY_ID);
    q.addBindValue(playListRecord->name());
    q.addBindValue(playListRecord->coverPath());
    q.addBindValue(playListRecord->id());
    return q.exec();
}

int SQLiteLocalDatabase::queryImagesCount()
{
    if (!m_db.isOpen()) {
        return 0;
    }

    QSqlQuery q(SQL_QUERY_IMAGES_COUNT);
    if (q.first()) {
        return q.value(0).toInt();
    } else {
        return 0;
    }
}

bool SQLiteLocalDatabase::insertImage(Image *image)
{
    if (!m_db.isOpen()) {
        return false;
    }

    QSqlQuery q;
    q.prepare(SQL_INSERT_IMAGE);
    q.addBindValue(image->source()->hashStr());
    q.addBindValue(image->name());
    q.addBindValue(image->source()->url().toString());

    if (!q.exec()) {
        qWarning() << q.lastError() << q.lastQuery();
        return false;
    }

    return true;
}

Image *SQLiteLocalDatabase::queryImageByHashStr(const QString &hashStr)
{
    if (!m_db.isOpen()) {
        return 0;
    }

    QSqlQuery q;
    q.prepare(SQL_QUERY_IMAGE_BY_HASH);
    q.addBindValue(hashStr);

    if (!q.exec() || !q.first()) {
        qWarning() << q.lastError() << q.lastQuery();
        return 0;
    } else {
        QString urlStr = q.value(0).toString();
        return new Image(QUrl(urlStr));
    }
}
