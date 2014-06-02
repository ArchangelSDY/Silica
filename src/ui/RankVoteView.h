#ifndef RANKVOTEVIEW_H
#define RANKVOTEVIEW_H

#include <QKeyEvent>
#include <QObject>

class Navigator;

class RankVoteView : public QObject
{
    Q_OBJECT
public:
    explicit RankVoteView(Navigator **navigator, QWidget *parentWidget);

    void keyPressEvent(QKeyEvent *event);

private:
    void upVote();
    void downVote();
    void showNotification();

    QWidget *m_parentWidget;
    Navigator **m_navigator;
};

#endif // RANKVOTEVIEW_H
