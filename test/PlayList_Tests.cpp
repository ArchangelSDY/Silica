#include <QImage>
#include <QSignalSpy>
#include <QStringList>
#include <QTest>

#include "../src/PlayList.h"
#include "../src/GlobalConfig.h"
#include "../src/db/LocalDatabase.h"
#include "../src/image/ImageRank.h"
#include "../src/image/ImageSource.h"
#include "../src/playlist/EqualRankFilter.h"
#include "../src/playlist/MinRankFilter.h"
#include "../src/playlist/NotEqualRankFilter.h"

class MockImageSource : public ImageSource
{
public:
    MockImageSource(const QString &name) :
        ImageSource(0)
    {
        m_name = name;
    }

    bool open() { return true; }
    bool copy(const QString &) { return true; }
};

class TestPlayList : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void sortByName();
    void sortByName_data();

    void sortByAspectRatio();
    void sortByAspectRatio_data();

    void sortByThumbHist();
    void sortByThumbHist_data();

    void setFilter();
    void setFilter_data();

private:
    QTemporaryDir m_tmpDir;
};

Q_DECLARE_METATYPE(AbstractPlayListFilter *)

void TestPlayList::initTestCase()
{
    Q_INIT_RESOURCE(silica);
    Q_INIT_RESOURCE(silicatest);
    GlobalConfig::create();
    if (!LocalDatabase::instance()->migrate()) {
        QFAIL("Fail to migrate database! Will exit now.");
    }

    const QString &tmpDirPath = m_tmpDir.path();

    QImage imgLisbeth(100, 100, QImage::Format_ARGB32);
    imgLisbeth.fill(Qt::white);
    imgLisbeth.save(tmpDirPath + QDir::separator() + "lisbeth.png");

    QImage imgSilica(100, 200, QImage::Format_ARGB32);
    imgSilica.fill(Qt::blue);
    imgSilica.save(tmpDirPath + QDir::separator() + "silica.png");

    QImage imgAsuna(200, 100, QImage::Format_ARGB32);
    imgAsuna.fill(Qt::red);
    imgAsuna.save(tmpDirPath + QDir::separator() + "asuna.png");
}

void TestPlayList::sortByName()
{
    QFETCH(QStringList, unsortedNames);
    QFETCH(QStringList, sortedNames);

    PlayList pl;
    foreach (const QString &name, unsortedNames) {
        pl << QSharedPointer<Image>::create(new MockImageSource(name));
    }

    pl.sortByName();

    QCOMPARE(pl.count(), sortedNames.count());
    for (int i = 0; i < pl.count(); ++i) {
        QCOMPARE(pl[i]->name(), sortedNames[i]);
    }
}

void TestPlayList::sortByName_data()
{
    QTest::addColumn<QStringList>("unsortedNames");
    QTest::addColumn<QStringList>("sortedNames");

    QTest::newRow("silica asuna")
        << (QStringList() << "silica.png" << "asuna.png")
        << (QStringList() << "asuna.png" << "silica.png");

    QTest::newRow("asuna silica")
        << (QStringList() << "asuna.png" << "silica.png")
        << (QStringList() << "asuna.png" << "silica.png");

    QTest::newRow("numbers in name")
        << (QStringList() << "silica_10.png"
                          << "silica_1.png"
                          << "silica_2.png")
        << (QStringList() << "silica_1.png"
                          << "silica_2.png"
                          << "silica_10.png");

    QTest::newRow("numbers in name 2")
        << (QStringList() << "IMG_20150101_0030_1.jpg"
                          << "IMG_20150101_0030_10.jpg"
                          << "IMG_20150101_0030_2.jpg")
        << (QStringList() << "IMG_20150101_0030_1.jpg"
                          << "IMG_20150101_0030_2.jpg"
                          << "IMG_20150101_0030_10.jpg");
}

void TestPlayList::sortByAspectRatio()
{
    QFETCH(QList<QUrl>, srcUrls);
    QFETCH(QString, firstItemName);
    QFETCH(QString, secondItemName);

    PlayList pl(srcUrls);

    pl.at(0)->load();
    QSignalSpy spyFirst(pl.at(0).data(), SIGNAL(loaded()));
    spyFirst.wait();

    pl.at(1)->load();
    QSignalSpy spySecond(pl.at(1).data(), SIGNAL(loaded()));
    spySecond.wait();

    pl.sortByAspectRatio();

    QCOMPARE(pl.at(0)->name(), firstItemName);
    QCOMPARE(pl.at(1)->name(), secondItemName);
}

void TestPlayList::sortByAspectRatio_data()
{
    const QString &tmpDirPath = m_tmpDir.path();
    QUrl silicaUrl = QUrl::fromLocalFile(
        tmpDirPath + QDir::separator() + "silica.png");
    QUrl asunaUrl = QUrl::fromLocalFile(
        tmpDirPath + QDir::separator() + "asuna.png");

    QTest::addColumn<QList<QUrl> >("srcUrls");
    QTest::addColumn<QString>("firstItemName");
    QTest::addColumn<QString>("secondItemName");

    QTest::newRow("(100, 200) (200, 100)")
        << (QList<QUrl>() << silicaUrl << asunaUrl)
        << "silica.png"
        << "asuna.png";

    QTest::newRow("(200, 100) (100, 200)")
        << (QList<QUrl>() << asunaUrl << silicaUrl)
        << "silica.png"
        << "asuna.png";
}

void TestPlayList::sortByThumbHist()
{
    QFETCH(QList<QUrl>, srcUrls);
    QFETCH(QStringList, sortedImageNames);

    PlayList pl(srcUrls);
    QCOMPARE(pl.size(), srcUrls.size());

    for (int i = 0; i < pl.size(); ++i) {
        ImagePtr image = pl[i];
        QSignalSpy spy(image.data(), SIGNAL(thumbnailLoaded()));
        image->loadThumbnail(true);
        spy.wait();
    }

    pl.sortByGroup();

    QStringList actSortedImageNames;
    for (int i = 0; i < pl.size(); ++i) {
        actSortedImageNames << pl[i]->name();
    }

    QCOMPARE(actSortedImageNames, sortedImageNames);
}

void TestPlayList::sortByThumbHist_data()
{
    const QString &tmpDirPath = m_tmpDir.path();
    QUrl silicaUrl = QUrl::fromLocalFile(
        tmpDirPath + QDir::separator() + "silica.png");
    QUrl asunaUrl = QUrl::fromLocalFile(
        tmpDirPath + QDir::separator() + "asuna.png");

    QTest::addColumn<QList<QUrl> >("srcUrls");
    QTest::addColumn<QStringList>("sortedImageNames");

    QTest::newRow("Normal")
        << (QList<QUrl>()
            << silicaUrl << asunaUrl << silicaUrl)
        << (QStringList()
            << "silica.png" << "silica.png" << "asuna.png");
}

void TestPlayList::setFilter()
{
    QFETCH(ImageList, images);
    QFETCH(AbstractPlayListFilter *, filter);
    QFETCH(int, initialCount);
    QFETCH(int, filteredCount);

    PlayList pl;
    pl << images;

    // Disable filters first
    pl.setFilter(0);

    QCOMPARE(pl.count(), initialCount);
    QCOMPARE(pl.size(), initialCount);

    QSignalSpy spyItemsChanged(&pl, SIGNAL(itemsChanged()));

    // Apply filters
    pl.setFilter(filter);

    QCOMPARE(spyItemsChanged.count(), 1);
    QCOMPARE(pl.count(), filteredCount);
    QCOMPARE(pl.size(), filteredCount);

    // Sorting has no effect on filter
    pl.sortByAspectRatio();

    QCOMPARE(pl.count(), filteredCount);
    QCOMPARE(pl.size(), filteredCount);

    pl.sortByName();

    QCOMPARE(pl.count(), filteredCount);
    QCOMPARE(pl.size(), filteredCount);
}

void TestPlayList::setFilter_data()
{
    QTest::addColumn<ImageList>("images");
    QTest::addColumn<AbstractPlayListFilter *>("filter");
    QTest::addColumn<int>("initialCount");
    QTest::addColumn<int>("filteredCount");

    const QString &tmpDirPath = m_tmpDir.path();

    QUrl silicaUrl = QUrl::fromLocalFile(
        tmpDirPath + QDir::separator() + "silica.png");
    ImagePtr imgSilica(new Image(silicaUrl));
    imgSilica->rank()->setValue(3);

    QUrl lisbethUrl = QUrl::fromLocalFile(
        tmpDirPath + QDir::separator() + "lisbeth.png");
    ImagePtr imgLisbeth(new Image(lisbethUrl));
    imgLisbeth->rank()->setValue(2);

    QUrl asunaUrl = QUrl::fromLocalFile(
        tmpDirPath + QDir::separator() + "asuna.png");
    ImagePtr imgAsuna(new Image(asunaUrl));
    imgAsuna->rank()->setValue(5);

    QTest::newRow("Empty image list with no filter")
        << (ImageList())
        << static_cast<AbstractPlayListFilter *>(0)
        << 0
        << 0;
    QTest::newRow("Not empty image list with no filter")
        << (ImageList() << imgSilica)
        << static_cast<AbstractPlayListFilter *>(0)
        << 1
        << 1;

    QTest::newRow("MinRankFilter above")
        << (ImageList() << imgSilica)
        << static_cast<AbstractPlayListFilter *>(new MinRankFilter(2))
        << 1
        << 1;
    QTest::newRow("MinRankFilter equal")
        << (ImageList() << imgSilica)
        << static_cast<AbstractPlayListFilter *>(new MinRankFilter(3))
        << 1
        << 1;
    QTest::newRow("MinRankFilter below")
        << (ImageList() << imgSilica)
        << static_cast<AbstractPlayListFilter *>(new MinRankFilter(4))
        << 1
        << 0;

    QTest::newRow("EqualRankFilter equal")
        << (ImageList() << imgSilica)
        << static_cast<AbstractPlayListFilter *>(
            new EqualRankFilter(imgSilica->rank()->value()))
        << 1
        << 1;
    QTest::newRow("EqualRankFilter not equal")
        << (ImageList() << imgSilica)
        << static_cast<AbstractPlayListFilter *>(
            new EqualRankFilter(imgSilica->rank()->value() - 1))
        << 1
        << 0;

    QTest::newRow("NotEqualRankFilter not equal")
        << (ImageList() << imgSilica)
        << static_cast<AbstractPlayListFilter *>(
            new NotEqualRankFilter(imgSilica->rank()->value() - 1))
        << 1
        << 1;
    QTest::newRow("NotEqualRankFilter equal")
        << (ImageList() << imgSilica)
        << static_cast<AbstractPlayListFilter *>(
            new NotEqualRankFilter(imgSilica->rank()->value()))
        << 1
        << 0;

    QTest::newRow("Multiple NotEqualRankFilter only rank 3,4,5 pass")
        << (ImageList() << imgLisbeth << imgSilica << imgAsuna)
        << static_cast<AbstractPlayListFilter *>(
            new NotEqualRankFilter(1,
            new NotEqualRankFilter(2)))
        << 3
        << 2;
    QTest::newRow("Multiple NotEqualRankFilter only rank 1,4 pass")
        << (ImageList() << imgLisbeth << imgSilica << imgAsuna)
        << static_cast<AbstractPlayListFilter *>(
            new NotEqualRankFilter(2,
            new NotEqualRankFilter(3,
            new NotEqualRankFilter(5))))
        << 3
        << 0;
}

QTEST_MAIN(TestPlayList)
 #include "PlayList_Tests.moc"
