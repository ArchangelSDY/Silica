#include "SQLiteLocalDatabase.h"

#include "deps/QtDBMigration/src/QtDBMigration.h"
#include "image/ImageRank.h"
#include "image/ImageSource.h"
#include "playlist/PlayListProviderManager.h"
#include "GlobalConfig.h"
#include "PlayList.h"

const char *SQL_ENABLE_FOREIGN_KEYS = "pragma foreign_keys=on";

const char *SQL_INSERT_PLAYLIST = "insert into playlists(name, cover_path, type) values (?, ?, ?)";

const char *SQL_INSERT_PLAYLIST_IMAGES = "insert into playlist_images(playlist_id, image_id) values ("
        "?, (select id from images where hash = ?))";

const char *SQL_REMOVE_PLAYLIST_IMAGE_BY_HASH = "delete from playlist_images "
        "where playlist_id=? and image_id in "
        "(select id from images where hash = ?)";

const char *SQL_QUERY_PLAYLISTS =
    "select playlists.id, playlists.name, playlists.cover_path, playlists.type, "
    "count(playlists.id) from playlists left outer join playlist_images "
    "on playlist_images.playlist_id = playlists.id "
    "group by playlists.id order by playlists.name";

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

const char *SQL_UPDATE_IMAGE_SIZE = "update images set width = ?, height = ? where hash = ?";

const char *SQL_QUERY_IMAGE_SIZE = "select width, height from images where hash = ? limit 1";

const char *SQL_INSERT_IMAGE_HOTSPOT = "insert into image_hotspots(image_hash, left, top, width, height) "
        "values (?, ?, ?, ?, ?)";

const char *SQL_REMOVE_IMAGE_HOTSPOT_BY_ID = "delete from image_hotspots where id = ?";

const char *SQL_QUERY_IMAGE_HOTSPOTS = "select id, left, top, width, height from image_hotspots where image_hash = ?";

const char *SQL_QUERY_IMAGE_RANK_BY_IMAGE_HASH = "select rank from image_ranks where image_hash = ?";

const char *SQL_UPDATE_IMAGE_RANK_BY_IMAGE_HASH = "insert or replace into image_ranks (rank, image_hash) values (?, ?)";

const char *SQL_INSERT_PLUGIN_PLAYLIST_PROVIDER =
        "insert into plugin_playlist_providers(name) values (?)";

const char *SQL_QUERY_PLUGIN_PLAYLIST_PROVIDER =
        "select rowid, name from plugin_playlist_providers where name = ?";

SQLiteLocalDatabase::SQLiteLocalDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(GlobalConfig::instance()->localDatabasePath());
    if (!m_db.open()) {
        qWarning("Unable to local open database!");
        return;
    }

    m_db.exec(SQL_ENABLE_FOREIGN_KEYS);
}

bool SQLiteLocalDatabase::migrate()
{
    QtDBMigration migration(GlobalConfig::instance()->migrationConfigPath());
    return migration.migrate();
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
        int type = q.value(3).toInt();
        int count = q.value(4).toInt();

        PlayListProvider *provider =
            PlayListProviderManager::instance()->create(type);
        if (provider) {
            PlayListRecordBuilder recordBuilder;
            recordBuilder
                .setId(id)
                .setName(name)
                .setCoverPath(coverPath)
                .setType(type)
                .setCount(count)
                .setProvider(provider);
            records << recordBuilder.obtain();
        }
    }

    return records;
}

QList<QUrl> SQLiteLocalDatabase::queryImageUrlsForLocalPlayListRecord(
    int playListId)
{
    QList<QUrl> imageUrls;

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
        imageUrls << QUrl(imageUrl);
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
    qInsertPlayList.addBindValue(playListRecord->type());
    if (!qInsertPlayList.exec()) {
        qWarning() << qInsertPlayList.lastError()
                   << qInsertPlayList.lastQuery();
        return false;
    }
    int playListId = qInsertPlayList.lastInsertId().toInt();
    playListRecord->setId(playListId);

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

bool SQLiteLocalDatabase::insertImagesForLocalPlayListProvider(
        const PlayListRecordInfo &plrInfo, const QList<ImageInfo> &imgInfos)
{
    // Insert images
    foreach (const ImageInfo &info, imgInfos) {
        insertImage(info);
    }

    // Insert relationship records
    int playListId = plrInfo.id;
    QSqlQuery q;
    q.prepare(SQL_INSERT_PLAYLIST_IMAGES);

    QVariantList playListIds;
    QVariantList imageHashes;
    foreach (const ImageInfo &info, imgInfos) {
        playListIds << playListId;
        imageHashes << info.hashStr;
    }
    q.addBindValue(playListIds);
    q.addBindValue(imageHashes);

    if (!q.execBatch()) {
        qWarning() << q.lastError()
                   << q.lastQuery();
        return false;
    }

    return true;
}

bool SQLiteLocalDatabase::removeImagesForLocalPlayListProvider(
        const PlayListRecordInfo &plrInfo, const QList<ImageInfo> &imgInfos)
{
    if (!m_db.isOpen() || imgInfos.isEmpty()) {
        return false;
    }

    QSqlQuery q;
    q.prepare(SQL_REMOVE_PLAYLIST_IMAGE_BY_HASH);

    QVariantList plrIds;
    QVariantList imgHashes;
    foreach (const ImageInfo &info, imgInfos) {
        plrIds << plrInfo.id;
        imgHashes << info.hashStr;
    }

    q.addBindValue(plrIds);
    q.addBindValue(imgHashes);

    if (!q.execBatch()) {
        qWarning() << q.lastError() << q.lastQuery();
        return false;
    }

    return true;
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

bool SQLiteLocalDatabase::insertImage(const ImageInfo &info)
{
    if (!m_db.isOpen()) {
        return false;
    }

    QSqlQuery q;
    q.prepare(SQL_INSERT_IMAGE);
    q.addBindValue(info.hashStr);
    q.addBindValue(info.name);
    q.addBindValue(info.url.toString());

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

bool SQLiteLocalDatabase::updateImageSize(Image *image)
{
    if (!m_db.isOpen() || !image) {
        return false;
    }

    QSqlQuery q;
    q.prepare(SQL_UPDATE_IMAGE_SIZE);
    q.addBindValue(image->width());
    q.addBindValue(image->height());
    q.addBindValue(image->source()->hashStr());

    if (!q.exec()) {
        qWarning() << q.lastError() << q.lastQuery();
        return false;
    }

    return true;
}

QSize SQLiteLocalDatabase::queryImageSize(Image *image)
{
    if (!m_db.isOpen() || !image || !image->source()) {
        return Image::UNKNOWN_SIZE;
    }

    QSqlQuery q;
    q.prepare(SQL_QUERY_IMAGE_SIZE);
    q.addBindValue(image->source()->hashStr());

    if (!q.exec() || !q.first()) {
        return Image::UNKNOWN_SIZE;
    } else {
        int width = q.value("width").toInt();
        int height = q.value("height").toInt();
        return QSize(width, height);
    }
}

bool SQLiteLocalDatabase::insertImageHotspot(ImageHotspot *hotspot)
{
    if (!m_db.isOpen()) {
        return false;
    }

    QSqlQuery q;
    q.prepare(SQL_INSERT_IMAGE_HOTSPOT);
    QRect rect = hotspot->rect();
    q.addBindValue(hotspot->image()->source()->hashStr());
    q.addBindValue(rect.left());
    q.addBindValue(rect.top());
    q.addBindValue(rect.width());
    q.addBindValue(rect.height());

    if (!q.exec()) {
        qWarning() << q.lastError() << q.lastQuery();
        return false;
    }

    int id = q.lastInsertId().toInt();
    hotspot->setId(id);

    return true;
}

bool SQLiteLocalDatabase::removeImageHotspot(ImageHotspot *hotspot)
{
    if (!m_db.isOpen()) {
        return false;
    }

    QSqlQuery q;
    q.prepare(SQL_REMOVE_IMAGE_HOTSPOT_BY_ID);
    q.addBindValue(hotspot->id());

    if (!q.exec()) {
        qWarning() << q.lastError() << q.lastQuery();
        return false;
    }

    return true;
}

QList<ImageHotspot *> SQLiteLocalDatabase::queryImageHotspots(Image *image)
{
    QList<ImageHotspot *> hotspots;

    if (!m_db.isOpen()) {
        return hotspots;
    }

    QSqlQuery q;
    q.prepare(SQL_QUERY_IMAGE_HOTSPOTS);
    q.addBindValue(image->source()->hashStr());

    if (!q.exec()) {
        qWarning() << q.lastError() << q.lastQuery();
        return hotspots;
    }

    while (q.next()) {
        int id = q.value("id").toInt();
        int left = q.value("left").toInt();
        int top = q.value("top").toInt();
        int width = q.value("width").toInt();
        int height = q.value("height").toInt();
        ImageHotspot *hotspot =
            new ImageHotspot(image, QRect(left, top, width, height));
        hotspot->setId(id);
        hotspots << hotspot;
    }

    return hotspots;
}

int SQLiteLocalDatabase::queryImageRankValue(Image *image)
{
    if (!m_db.isOpen() || !image || !image->source()) {
        return ImageRank::DEFAULT_VALUE;
    }

    QSqlQuery q;
    q.prepare(SQL_QUERY_IMAGE_RANK_BY_IMAGE_HASH);
    q.addBindValue(image->source()->hashStr());

    if (!q.exec()) {
        qWarning() << q.lastError() << q.lastQuery();
        return ImageRank::DEFAULT_VALUE;
    }

    while (q.next()) {
        return q.value("rank").toInt();
    }

    return ImageRank::DEFAULT_VALUE;
}

bool SQLiteLocalDatabase::updateImageRank(Image *image, int rank)
{
    if (!m_db.isOpen() || image == 0) {
        return false;
    }

    QSqlQuery q;
    q.prepare(SQL_UPDATE_IMAGE_RANK_BY_IMAGE_HASH);
    q.addBindValue(rank);
    q.addBindValue(image->source()->hashStr());

    if (!q.exec()) {
        qWarning() << q.lastError() << q.lastQuery();
        return false;
    }

    return true;
}

int SQLiteLocalDatabase::insertPluginPlayListProviderType(const QString &name)
{
    if (!m_db.isOpen()) {
        return PlayListRecord::UNKNOWN_TYPE;
    }

    QSqlQuery q;
    q.prepare(SQL_INSERT_PLUGIN_PLAYLIST_PROVIDER);
    q.addBindValue(name);

    if (!q.exec()) {
        qWarning() << q.lastError() << q.lastQuery();
        return PlayListRecord::UNKNOWN_TYPE;
    }

    return q.lastInsertId().toInt() + PLUGIN_PLAYLIST_PROVIDER_TYPE_OFFSET;
}

int SQLiteLocalDatabase::queryPluginPlayListProviderType(const QString &name)
{
    if (!m_db.isOpen()) {
        return PlayListRecord::UNKNOWN_TYPE;
    }

    QSqlQuery q;
    q.prepare(SQL_QUERY_PLUGIN_PLAYLIST_PROVIDER);
    q.addBindValue(name);

    if (!q.exec()) {
        qWarning() << q.lastError() << q.lastQuery();
        return PlayListRecord::UNKNOWN_TYPE;
    }

    while (q.next()) {
        return q.value("rowid").toInt() + PLUGIN_PLAYLIST_PROVIDER_TYPE_OFFSET;
    }

    return PlayListRecord::UNKNOWN_TYPE;
}
