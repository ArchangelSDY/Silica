#pragma once

#include <gmock/gmock.h>

#include "../src/db/LocalDatabase.h"

class MockLocalDatabase : public LocalDatabase
{
public:
    MOCK_METHOD0(migrate, bool());

    MOCK_METHOD1(queryPlayListEntities, QList<PlayListEntityData>(int type));
    MOCK_METHOD1(queryPlayListEntity, PlayListEntityData(int playListId));
    MOCK_METHOD1(insertPlayListEntity, bool(PlayListEntityData &data));
    MOCK_METHOD1(removePlayListEntity, bool(int playListId));
    MOCK_METHOD1(updatePlayListEntity, bool(const PlayListEntityData &data));

    MOCK_METHOD1(queryLocalPlayListEntityImageUrls, QList<QUrl>(int playListId));
    MOCK_METHOD2(insertLocalPlayListEntityImageUrls, bool(int playListId, const QList<QUrl> &images));
    MOCK_METHOD2(removeLocalPlayListEntityImageUrls, bool(int playListId, const QList<QUrl> &images));

    MOCK_METHOD0(queryImagesCount, int());
    MOCK_METHOD1(insertImage, bool(Image *image));
    MOCK_METHOD1(insertImages, bool(const ImageList &images));
    MOCK_METHOD1(queryImageByHashStr, Image*(const QString &hashStr));
    MOCK_METHOD2(updateImageUrl, bool(const QUrl &oldUrl, const QUrl &newUrl));

    MOCK_METHOD1(queryImageRankValue, int(Image *image));
    MOCK_METHOD2(updateImageRank, bool(Image *image, int rank));

    MOCK_METHOD1(insertPluginPlayListProviderType, int(const QString &name));
    MOCK_METHOD1(queryPluginPlayListProviderType, int(const QString &name));

    MOCK_METHOD2(saveTaskProgressTimeConsumption, bool(const QString &key, qint64 timeConsumption));
    MOCK_METHOD1(queryTaskProgressTimeConsumption, qint64(const QString &key));
};
