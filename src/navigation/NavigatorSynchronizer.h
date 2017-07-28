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

    bool isEnabled() const;
    void setEnabled(bool enabled);
    void setLoop(bool loop);

private slots:
    void onPlayListChange(QSharedPointer<PlayList>);
    void onNavigationChange(int index);

private:
    static int OFFSET;

    Navigator *m_primary;
    Navigator *m_secondary;
    bool m_enabled;
};