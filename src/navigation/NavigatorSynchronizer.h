#pragma once

#include <QObject>
#include <QSharedPointer>

class Navigator;
class PlayList;

class NavigatorSynchronizer : public QObject
{
    Q_OBJECT
public:
    NavigatorSynchronizer(Navigator *primary, Navigator *secondary);

    int offset() const;
    void setOffset(int offset);

private slots:
    void onPlayListChange(QSharedPointer<PlayList>);
    void onNavigationChange(int index);

private:
    Navigator *m_primary;
    Navigator *m_secondary;
    int m_offset;
};