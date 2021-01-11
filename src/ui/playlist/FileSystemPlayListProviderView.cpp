#include "FileSystemPlayListProviderView.h"

#include <QFileInfo>
#include <QLineEdit>

#include "playlist/FileSystemPlayListProvider.h"

FileSystemPlayListProviderView::FileSystemPlayListProviderView(FileSystemPlayListProvider *provider) :
    m_provider(provider)
{
}

void FileSystemPlayListProviderView::setUpToolBar(QToolBar *toolBar)
{
    toolBar->setStyleSheet("QToolBar::separator { background-color: #59616A; width: 1px } QToolButton { padding: -2px; }");

    auto actSep = toolBar->addSeparator();
    m_toolBarObjects << actSep;

    auto provider = m_provider;
    auto actUp = toolBar->addAction(QIcon(":/res/toolbar/up.png"), QObject::tr("Up"), [provider]() {
        QFileInfo rootPathInfo(provider->rootPath());
        provider->setRootPath(rootPathInfo.absolutePath());
    });
    QObject::connect(provider, &FileSystemPlayListProvider::rootPathChanged, actUp, [actUp](const QString &rootPath) {
        QFileInfo rootPathInfo(rootPath);
        actUp->setEnabled(!rootPathInfo.isRoot());
    });
    m_toolBarObjects << actUp;

    auto pathEdit = new QLineEdit(toolBar);
    pathEdit->setText(provider->rootPath());
    pathEdit->setStyleSheet("QLineEdit { border-radius: 0.1em; padding: 0.2em 0.5em; }");
    toolBar->addWidget(pathEdit);
    QObject::connect(provider, &FileSystemPlayListProvider::rootPathChanged, pathEdit, [pathEdit](const QString &rootPath) {
        pathEdit->setText(rootPath);
    });
    QObject::connect(pathEdit, &QLineEdit::editingFinished, pathEdit, [provider, pathEdit]() {
        provider->setRootPath(pathEdit->text());
    });
    m_toolBarObjects << pathEdit;
}

void FileSystemPlayListProviderView::tearDownToolBar(QToolBar *toolBar)
{
    toolBar->clear();
    qDeleteAll(m_toolBarObjects);
    m_toolBarObjects.clear();
}
