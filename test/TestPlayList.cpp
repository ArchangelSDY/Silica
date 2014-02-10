#include <QImage>
#include <QSignalSpy>
#include <QTest>

#include "../src/PlayList.h"

#include "TestPlayList.h"

void TestPlayList::initTestCase()
{
    const QString &tmpDirPath = m_tmpDir.path();

    QImage imgSilica(100, 200, QImage::Format_ARGB32);
    imgSilica.fill(Qt::blue);
    imgSilica.save(tmpDirPath + QDir::separator() + "silica.png");

    QImage imgAsuna(200, 100, QImage::Format_ARGB32);
    imgAsuna.fill(Qt::red);
    imgAsuna.save(tmpDirPath + QDir::separator() + "asuna.png");
}

void TestPlayList::sortByName()
{
    QFETCH(QList<QUrl>, srcUrls);
    QFETCH(QString, firstItemName);
    QFETCH(QString, secondItemName);

    PlayList pl(srcUrls);
    pl.sortByName();

    QCOMPARE(pl.at(0)->name(), firstItemName);
    QCOMPARE(pl.at(1)->name(), secondItemName);
}

void TestPlayList::sortByName_data()
{
    const QString &tmpDirPath = m_tmpDir.path();
    QUrl silicaUrl = QUrl::fromLocalFile(
        tmpDirPath + QDir::separator() + "silica.png");
    QUrl asunaUrl = QUrl::fromLocalFile(
        tmpDirPath + QDir::separator() + "asuna.png");

    QTest::addColumn<QList<QUrl> >("srcUrls");
    QTest::addColumn<QString>("firstItemName");
    QTest::addColumn<QString>("secondItemName");

    QTest::newRow("silica asuna")
        << (QList<QUrl>() << silicaUrl << asunaUrl)
        << "asuna.png"
        << "silica.png";

    QTest::newRow("asuna silica")
        << (QList<QUrl>() << asunaUrl << silicaUrl)
        << "asuna.png"
        << "silica.png";
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