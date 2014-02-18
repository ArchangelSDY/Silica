#include "SQLiteLocalDatabase.h"

const char *DB_NAME = "local.db";
const char *SQL_CREATE_PLAYLISTS_TABLE = "create table playlists ("
        "id integer primary key autoincrement, "
        "name text,"
        "cover_path text)";
const char *SQL_CREATE_PLAYLIST_IMAGE_TABLE = "create table playlist_images ("
        "id integer primary key autoincrement, "
        "playlist_id integer, "
        "image_url text)";
const char *SQL_INSERT_PLAYLIST = "insert into playlists(name, cover_path) "
        "values (?, ?)";
const char *SQL_INSERT_PLAYLIST_IMAGES = "insert into "
        "playlist_images(playlist_id, image_url) values (?, ?)";
const char *SQL_QUERY_PLAYLISTS = "select name, cover_path from playlists";
const char *SQL_QUERY_PLAYLIST_ID_BY_NAME = "select id, name from playlists "
        "where name = ?";
const char *SQL_QUERY_IMAGE_URLS_BY_PLAYLIST_ID = "select playlist_id, image_url from "
        "playlist_images where playlist_id = ?";

SQLiteLocalDatabase::SQLiteLocalDatabase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(DB_NAME);
    if (!m_db.open()) {
        qWarning("Unable to local open database!");
    }
}

QList<PlayListRecord> SQLiteLocalDatabase::playListRecords()
{
    QList<PlayListRecord> records;

    if (!m_db.isOpen()) {
        return records;
    }

    QSqlQuery q(SQL_QUERY_PLAYLISTS);
    while (q.next()) {
        QString name = q.value(0).toString();
        QString coverPath = q.value(1).toString();
        records << PlayListRecord(name, coverPath);
    }

    return records;
}

QStringList SQLiteLocalDatabase::imageUrlsForPlayList(const QString &name)
{
    QStringList imageUrls;

    if (!m_db.isOpen()) {
        return imageUrls;
    }

    QSqlQuery q;
    q.prepare(SQL_QUERY_PLAYLIST_ID_BY_NAME);
    q.addBindValue(name);
    if (!q.exec() || !q.first()) {
        qWarning() << q.lastError();
        qWarning() << q.lastQuery();
        return imageUrls;
    }

    QVariant playListId = q.value(0);

    q.prepare(SQL_QUERY_IMAGE_URLS_BY_PLAYLIST_ID);
    q.addBindValue(playListId);
    if (!q.exec()) {
        qWarning() << q.lastError();
        qWarning() << q.lastQuery();
        return imageUrls;
    }

    while (q.next()) {
        QString imageUrl = q.value(1).toString();
        imageUrls << imageUrl;
    }

    return imageUrls;
}

bool SQLiteLocalDatabase::savePlayListRecord(PlayListRecord *playListRecord)
{
    if (!m_db.isOpen()) {
        return false;
    }

    QStringList tables = m_db.tables();
    if (!tables.contains("playlists") ||
        !tables.contains("playlist_images")) {
        createTables();
    }

    QSqlQuery q;
    q.prepare(SQL_INSERT_PLAYLIST);
    q.addBindValue(playListRecord->name());
    q.addBindValue(playListRecord->coverPath());
    if (!q.exec()) {
        qWarning() << q.lastError();
        return false;
    }

    QVariant playListId = q.lastInsertId();
    q.prepare(SQL_INSERT_PLAYLIST_IMAGES);

    QVariantList playListIds;
    for (int i = 0; i < playListRecord->playList()->length(); ++i) {
        playListIds << playListId;
    }
    q.addBindValue(playListIds);

    QVariantList imageUrls;
    foreach (QSharedPointer<Image> image, *playListRecord->playList()) {
        imageUrls << image->source()->url();
    }
    q.addBindValue(imageUrls);

    if (!q.execBatch()) {
        qWarning() << q.lastError();
        return false;
    }

    return true;
}

void SQLiteLocalDatabase::createTables()
{
    QSqlQuery q;
    if (!q.exec(SQL_CREATE_PLAYLISTS_TABLE)) {
        qWarning() << q.lastError();
    }

    if (!q.exec(SQL_CREATE_PLAYLIST_IMAGE_TABLE)) {
        qWarning() << q.lastError();
    }
}
