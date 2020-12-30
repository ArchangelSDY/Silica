#include "SQLiteLocalDatabase.h"

#include <QApplication>
#include <QtConcurrent>
#include <QUuid>

#include "deps/QtDBMigration/src/QtDBMigration.h"
#include "image/ImageRank.h"
#include "image/ImageSource.h"
#include "GlobalConfig.h"
#include "../PlayList.h"

const char *SQL_ENABLE_FOREIGN_KEYS = "pragma foreign_keys=on";

const char *SQL_INSERT_PLAYLIST = "insert into playlists(name, cover_path, type, count) values (?, ?, ?, ?)";

const char *SQL_INSERT_PLAYLIST_IMAGES = "insert into playlist_images(playlist_id, image_id) values ("
"?, (select id from images where hash = ?))";

const char *SQL_REMOVE_PLAYLIST_IMAGE_BY_HASH = "delete from playlist_images "
"where playlist_id=? and image_id in "
"(select id from images where hash = ?)";

const char *SQL_QUERY_PLAYLISTS_BY_TYPE =
"select "
"playlists.id, playlists.name, playlists.cover_path, playlists.type, "
"count(playlist_images.id), playlists.count "
"from playlists "
"left outer join playlist_images on "
"playlist_images.playlist_id = playlists.id "
"where playlists.type = ? "
"group by playlists.id order by playlists.name";

const char *SQL_QUERY_PLAYLIST_ID_BY_NAME = "select id, name from playlists where name = ?";

const char *SQL_REMOVE_PLAYLIST_BY_ID = "delete from playlists where id = ?";

const char *SQL_UPDATE_PLAYLIST_BY_ID =
"update playlists "
"set name = ?, cover_path = ?, count = ? "
"where id = ?";

const char *SQL_QUERY_IMAGE_URLS_BY_PLAYLIST_ID = "select images.url from images "
"left join playlist_images on images.id = playlist_images.image_id "
"where playlist_images.playlist_id = ? "
"order by images.url";

const char *SQL_INSERT_IMAGE = "insert or ignore into images(hash, name, url) values (?, ?, ?)";

const char *SQL_QUERY_IMAGES_COUNT = "select count(id) from images";

const char *SQL_QUERY_IMAGE_BY_HASH = "select url from images where hash = ? limit 1";

const char *SQL_UPDATE_IMAGE_URL = "update images set url = ? where url = ?";

const char *SQL_QUERY_IMAGE_RANK_BY_IMAGE_HASH = "select rank from image_ranks where image_hash = ?";

const char *SQL_UPDATE_IMAGE_RANK_BY_IMAGE_HASH = "insert or replace into image_ranks (rank, image_hash) values (?, ?)";

const char *SQL_INSERT_PLUGIN_PLAYLIST_PROVIDER =
"insert into plugin_playlist_providers(name) values (?)";

const char *SQL_QUERY_PLUGIN_PLAYLIST_PROVIDER =
"select rowid, name from plugin_playlist_providers where name = ?";

const char *SQL_INSERT_OR_REPLACE_TASK_PROGRESS_TIME_CONSUMPTION =
"insert or replace into task_progresses (task_key, last_time_consumption) values (?, ?)";

const char *SQL_QUERY_TASK_PROGRESS_TIME_CONSUMPTION =
"select last_time_consumption from task_progresses where task_key = ?";

// #define OPEN_BACKGROUND_DATABASE(NAME) \
//     QString dbConnName = QStringLiteral(NAME) + QUuid::createUuid().toString(); \
//     bool isMainThread = QThread::currentThread() == QApplication::instance()->thread(); \
//     { \
//         QSqlDatabase db = isMainThread ? m_db : QSqlDatabase::cloneDatabase(m_db, dbConnName); \
//         if (!isMainThread && !db.open()) { \
//             return false; \
//         }
// 
// #define CLOSE_BACKGROUND_DATABASE \
//     } \
//     if (!isMainThread) QSqlDatabase::removeDatabase(dbConnName);

#define OPEN_BACKGROUND_DATABASE                                                                        \
    QString dbConnName = QStringLiteral("%1_%2").arg(                                                   \
        __func__, reinterpret_cast<uint64_t>(QThread::currentThreadId()));                              \
    bool dbCloned = false;                                                                              \
    do {                                                                                                \
        QSqlDatabase db = QSqlDatabase::database(dbConnName);                                           \
        if (!db.isValid()) {                                                                            \
            dbCloned = true;                                                                            \
            db = QSqlDatabase::cloneDatabase(m_db, dbConnName);                                         \
            if (!db.open()) {                                                                           \
                qWarning() << __func__ << "lastError:" << db.lastError();                               \
                break;                                                                                  \
            }                                                                                           \
        }

#define CLOSE_BACKGROUND_DATABASE                                                                       \
    } while (0);                                                                                        \
    QSqlDatabase::removeDatabase(dbConnName);

#define LOG_QUERY_ERROR                                                                                 \
    qWarning() << __func__ << "lastError:" << q.lastError() << "lastQuery:" << q.lastQuery();

SQLiteLocalDatabase::SQLiteLocalDatabase()
{
    QString dbPath = GlobalConfig::instance()->localDatabasePath();
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        qWarning("Unable to open local database!");
        return;
    }
    m_db.exec(SQL_ENABLE_FOREIGN_KEYS);
}

bool SQLiteLocalDatabase::migrate()
{
    QtDBMigration migration(GlobalConfig::instance()->migrationConfigPath());
    return migration.migrate();
}

QList<PlayListEntityData> SQLiteLocalDatabase::queryPlayListEntities(int type)
{
    QList<PlayListEntityData> lst;

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_QUERY_PLAYLISTS_BY_TYPE);
    q.addBindValue(type);
    if (!q.exec()) {
        LOG_QUERY_ERROR
        break;
    }
    while (q.next()) {
        PlayListEntityData data;
        data.id = q.value(0).toInt();
        data.name = q.value(1).toString();
        data.coverPath = q.value(2).toString();
        data.type = q.value(3).toInt();
        int dbImagesCount = q.value(4).toInt();
        int recordImagesCount = q.value(5).toInt();
        data.count = dbImagesCount > 0 ? dbImagesCount : recordImagesCount;
        lst << data;
    }

    CLOSE_BACKGROUND_DATABASE

    return lst;
}

QList<QUrl> SQLiteLocalDatabase::queryImageUrlsForLocalPlayListEntity(
    int playListId)
{
    QList<QUrl> imageUrls;

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_QUERY_IMAGE_URLS_BY_PLAYLIST_ID);
    q.addBindValue(playListId);
    if (!q.exec()) {
        LOG_QUERY_ERROR
        break;
    }

    while (q.next()) {
        QString imageUrl = q.value(0).toString();
        imageUrls << QUrl(imageUrl);
    }

    CLOSE_BACKGROUND_DATABASE

    return imageUrls;
}

bool SQLiteLocalDatabase::insertPlayListRecord(const PlayListEntityData &data)
{
    bool ret = false;

    OPEN_BACKGROUND_DATABASE

    // int count = 0;
    // if (playListRecord->playList()) {
    //     count = playListRecord->playList()->count();
    // }

    // Insert playlist
    QSqlQuery q(db);
    q.prepare(SQL_INSERT_PLAYLIST);
    q.addBindValue(data.name);
    q.addBindValue(data.coverPath);
    q.addBindValue(data.type);
    q.addBindValue(data.count);
    if (!q.exec()) {
        LOG_QUERY_ERROR
        break;
    }

    // TODO
    // int playListId = qInsertPlayList.lastInsertId().toInt();
    // playListRecord->setId(playListId);

    ret = true;

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

bool SQLiteLocalDatabase::removePlayListRecord(const PlayListEntityData &data)
{
    bool ret = false;

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_REMOVE_PLAYLIST_BY_ID);
    q.addBindValue(data.id);
    ret = q.exec();

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

bool SQLiteLocalDatabase::updatePlayListRecord(const PlayListEntityData &data)
{
    bool ret = false;

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_UPDATE_PLAYLIST_BY_ID);
    q.addBindValue(data.name);
    q.addBindValue(data.coverPath);
    q.addBindValue(data.count);
    q.addBindValue(data.id);

    ret = q.exec();

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

bool SQLiteLocalDatabase::insertImagesForLocalPlayListProvider(
    const PlayListEntityData &data, const ImageList &images)
{
    if (images.isEmpty()) {
        return true;
    }

    bool ret = false;

    OPEN_BACKGROUND_DATABASE

    // Insert relationship records
    QSqlQuery q(db);
    q.prepare(SQL_INSERT_PLAYLIST_IMAGES);

    QVariantList playListIds;
    QVariantList imageHashes;
    for (ImagePtr image : images) {
        playListIds << data.id;
        imageHashes << image->source()->hashStr();
    }
    q.addBindValue(playListIds);
    q.addBindValue(imageHashes);

    ret = q.execBatch();
    if (!ret) {
        LOG_QUERY_ERROR
        break;
    }

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

bool SQLiteLocalDatabase::removeImagesForLocalPlayListProvider(
    const PlayListEntityData &data, const ImageList &images)
{
    if (images.isEmpty()) {
        return true;
    }

    bool ret = false;

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_REMOVE_PLAYLIST_IMAGE_BY_HASH);

    QVariantList plrIds;
    QVariantList imgHashes;
    for (ImagePtr image : images) {
        plrIds << data.id;
        imgHashes << image->source()->hashStr();
    }

    q.addBindValue(plrIds);
    q.addBindValue(imgHashes);

    ret = q.execBatch();
    if (!ret) {
        LOG_QUERY_ERROR
        break;
    }
    CLOSE_BACKGROUND_DATABASE

    return ret;
}

int SQLiteLocalDatabase::queryImagesCount()
{
    int ret = 0;
    if (!m_db.isOpen()) {
        return ret;
    }

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(SQL_QUERY_IMAGES_COUNT, db);
    if (q.first()) {
        ret = q.value(0).toInt();
    }

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

bool SQLiteLocalDatabase::insertImage(Image *image)
{
    bool ret = false;
    if (!m_db.isOpen()) {
        return ret;
    }

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_INSERT_IMAGE);
    q.addBindValue(image->source()->hashStr());
    q.addBindValue(image->name());
    q.addBindValue(image->source()->url().toString());

    if (!q.exec()) {
        LOG_QUERY_ERROR
        break;
    }
    ret = true;

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

bool SQLiteLocalDatabase::insertImages(const ImageList &images)
{
    if (images.isEmpty()) {
        return true;
    }

    bool ret = false;

    OPEN_BACKGROUND_DATABASE
    QSqlQuery q(db);
    q.prepare(SQL_INSERT_IMAGE);

    QVariantList hashes;
    QVariantList names;
    QVariantList urls;

    for (const ImagePtr &image : images) {
        hashes << image->source()->hashStr();
        names << image->name();
        urls << image->source()->url().toString();
    }
    q.addBindValue(hashes);
    q.addBindValue(names);
    q.addBindValue(urls);

    ret = q.execBatch();
    if (!ret) {
        LOG_QUERY_ERROR
        break;
    }

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

Image *SQLiteLocalDatabase::queryImageByHashStr(const QString &hashStr)
{
    Image *ret = nullptr;
    if (!m_db.isOpen()) {
        return ret;
    }

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_QUERY_IMAGE_BY_HASH);
    q.addBindValue(hashStr);

    if (!q.exec() || !q.first()) {
        LOG_QUERY_ERROR
        break;
    } else {
        QString urlStr = q.value(0).toString();
        ret = new Image(QUrl(urlStr));
    }

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

bool SQLiteLocalDatabase::updateImageUrl(const QUrl &oldUrl, const QUrl &newUrl)
{
    bool ret = false;
    if (!m_db.isOpen()) {
        return ret;
    }

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_UPDATE_IMAGE_URL);
    q.addBindValue(newUrl.toString());
    q.addBindValue(oldUrl.toString());

    if (!q.exec()) {
        LOG_QUERY_ERROR
        break;
    }
    ret = true;

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

int SQLiteLocalDatabase::queryImageRankValue(Image *image)
{
    int ret = ImageRank::DEFAULT_VALUE;
    if (!m_db.isOpen() || !image || !image->source()) {
        return ret;
    }

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_QUERY_IMAGE_RANK_BY_IMAGE_HASH);
    q.addBindValue(image->source()->hashStr());

    if (!q.exec()) {
        LOG_QUERY_ERROR
        break;
    }

    while (q.next()) {
        ret = q.value("rank").toInt();
    }

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

bool SQLiteLocalDatabase::updateImageRank(Image *image, int rank)
{
    bool ret = false;
    if (!m_db.isOpen() || image == 0) {
        return ret;
    }

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_UPDATE_IMAGE_RANK_BY_IMAGE_HASH);
    q.addBindValue(rank);
    q.addBindValue(image->source()->hashStr());

    if (!q.exec()) {
        LOG_QUERY_ERROR
        break;
    }
    ret = true;

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

int SQLiteLocalDatabase::insertPluginPlayListProviderType(const QString &name)
{
    // TODO: const
    int ret = -1;
    if (!m_db.isOpen()) {
        return ret;
    }

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_INSERT_PLUGIN_PLAYLIST_PROVIDER);
    q.addBindValue(name);

    if (!q.exec()) {
        LOG_QUERY_ERROR
        break;
    }
    ret = q.lastInsertId().toInt() + PLUGIN_PLAYLIST_PROVIDER_TYPE_OFFSET;

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

int SQLiteLocalDatabase::queryPluginPlayListProviderType(const QString &name)
{
    // TODO: const
    int ret = -1;
    if (!m_db.isOpen()) {
        return ret;
    }

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_QUERY_PLUGIN_PLAYLIST_PROVIDER);
    q.addBindValue(name);

    if (!q.exec()) {
        LOG_QUERY_ERROR
        break;
    }

    while (q.next()) {
        ret = q.value("rowid").toInt() + PLUGIN_PLAYLIST_PROVIDER_TYPE_OFFSET;
    }

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

bool SQLiteLocalDatabase::saveTaskProgressTimeConsumption(const QString &key, qint64 timeConsumption)
{
    bool ret = false;
    if (!m_db.isOpen() || key.isEmpty()) {
        return ret;
    }

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_INSERT_OR_REPLACE_TASK_PROGRESS_TIME_CONSUMPTION);
    q.addBindValue(key);
    q.addBindValue(timeConsumption);

    if (!q.exec()) {
        LOG_QUERY_ERROR
        break;
    }
    ret = true;

    CLOSE_BACKGROUND_DATABASE

    return ret;
}

qint64 SQLiteLocalDatabase::queryTaskProgressTimeConsumption(const QString &key)
{
    qint64 ret = 0;
    if (!m_db.isOpen() || key.isEmpty()) {
        return ret;
    }

    OPEN_BACKGROUND_DATABASE

    QSqlQuery q(db);
    q.prepare(SQL_QUERY_TASK_PROGRESS_TIME_CONSUMPTION);
    q.addBindValue(key);

    if (!q.exec()) {
        LOG_QUERY_ERROR
        break;
    }

    while (q.next()) {
        ret = q.value("last_time_consumption").toInt();
    }

    CLOSE_BACKGROUND_DATABASE

    return ret;
}
