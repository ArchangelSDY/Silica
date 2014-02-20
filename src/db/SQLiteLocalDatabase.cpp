#include "SQLiteLocalDatabase.h"

const char *DB_NAME = "local.db";

const char *SQL_ENABLE_FOREIGN_KEYS = "pragma foreign_keys=on";

const char *SQL_CREATE_PLAYLISTS_TABLE = "create table playlists ("
        "id integer primary key autoincrement, "
        "name text,"
        "cover_path text)";

const char *SQL_CREATE_PLAYLIST_IMAGES_TABLE = "create table playlist_images ("
        "id integer primary key autoincrement, "
        "playlist_id integer references playlists on delete cascade, "
        "image_id integer references images on delete cascade)";

const char *SQL_INSERT_PLAYLIST = "replace into playlists(name, cover_path) values (?, ?)";

const char *SQL_INSERT_PLAYLIST_IMAGES = "replace into playlist_images(playlist_id, image_id) values ("
        "?, (select id from images where hash = ?))";

const char *SQL_QUERY_PLAYLISTS = "select name, cover_path from playlists";

const char *SQL_QUERY_PLAYLIST_ID_BY_NAME = "select id, name from playlists where name = ?";

const char *SQL_QUERY_IMAGE_URLS_BY_PLAYLIST_NAME = "select images.url, playlists.name from images "
        "left join playlist_images on images.id = playlist_images.image_id "
        "join playlists on playlists.id = playlist_images.playlist_id "
        "where playlists.name = ?";

const char *SQL_CREATE_IMAGES_TABLE = "create table images ("
        "id integer primary key autoincrement, "
        "hash text unique, "
        "name text, "
        "url text)";

const char *SQL_CREATE_IMAGES_INDEX = "create index images_index_hash on images (hash)";

const char *SQL_INSERT_IMAGE = "replace into images(hash, name, url) values (?, ?, ?)";

const char *SQL_QUERY_IMAGES_COUNT = "select count(id) from images";

const char *SQL_QUERY_IMAGE_BY_HASH = "select url from images where hash = ? limit 1";

SQLiteLocalDatabase::SQLiteLocalDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(DB_NAME);
    if (!m_db.open()) {
        qWarning("Unable to local open database!");
        return;
    }
    m_db.exec(SQL_ENABLE_FOREIGN_KEYS);
}

void SQLiteLocalDatabase::createTablesIfNecessary()
{
    QStringList tables = m_db.tables();
    QSqlQuery q;

    if (!tables.contains("playlists")) {
        if (!q.exec(SQL_CREATE_PLAYLISTS_TABLE)) {
            qWarning() << q.lastError() << q.lastQuery();
        }
    }

    if (!tables.contains("playlist_images")) {
        if (!q.exec(SQL_CREATE_PLAYLIST_IMAGES_TABLE)) {
            qWarning() << q.lastError() << q.lastQuery();
        }
    }

    if (!tables.contains("images")) {
        if (!q.exec(SQL_CREATE_IMAGES_TABLE)) {
            qWarning() << q.lastError() << q.lastQuery();
        }

        if (!q.exec(SQL_CREATE_IMAGES_INDEX)) {
            qWarning() << q.lastError() << q.lastQuery();
        }
    }
}

QList<PlayListRecord> SQLiteLocalDatabase::queryPlayListRecords()
{
    QList<PlayListRecord> records;

    if (!m_db.isOpen()) {
        return records;
    }

    createTablesIfNecessary();

    QSqlQuery q(SQL_QUERY_PLAYLISTS);
    while (q.next()) {
        QString name = q.value(0).toString();
        QString coverPath = q.value(1).toString();
        records << PlayListRecord(name, coverPath);
    }

    return records;
}

QStringList SQLiteLocalDatabase::queryImageUrlsForPlayList(const QString &name)
{
    QStringList imageUrls;

    if (!m_db.isOpen()) {
        return imageUrls;
    }

    createTablesIfNecessary();

    QSqlQuery q;
    q.prepare(SQL_QUERY_IMAGE_URLS_BY_PLAYLIST_NAME);
    q.addBindValue(name);
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

    createTablesIfNecessary();

    // Insert playlist
    QSqlQuery qInsertPlayList;
    qInsertPlayList.prepare(SQL_INSERT_PLAYLIST);
    qInsertPlayList.addBindValue(playListRecord->name());
    qInsertPlayList.addBindValue(playListRecord->coverPath());
    if (!qInsertPlayList.exec()) {
        qWarning() << qInsertPlayList.lastError()
                   << qInsertPlayList.lastQuery();
        return false;
    }
    QVariant playListId = qInsertPlayList.lastInsertId();

    // Insert images
    foreach (QSharedPointer<Image> image, *playListRecord->playList()) {
        insertImage(image.data());
    }

    // Insert relationship records
    QSqlQuery qInsertPlayListImages;
    qInsertPlayListImages.prepare(SQL_INSERT_PLAYLIST_IMAGES);
    QVariantList playListIds;
    QVariantList imageHashes;
    foreach (QSharedPointer<Image> image, *playListRecord->playList()) {
        playListIds << playListId;
        imageHashes << image->source()->hashStr();
    }
    qInsertPlayListImages.addBindValue(playListIds);
    qInsertPlayListImages.addBindValue(imageHashes);

    if (!qInsertPlayListImages.execBatch()) {
        qWarning() << qInsertPlayListImages.lastError()
                   << qInsertPlayListImages.lastQuery();
        return false;
    }

    return true;
}

int SQLiteLocalDatabase::queryImagesCount()
{
    if (!m_db.isOpen()) {
        return 0;
    }

    createTablesIfNecessary();

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

    createTablesIfNecessary();

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

    createTablesIfNecessary();

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
