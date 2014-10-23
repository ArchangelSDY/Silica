#include "ImageRank.h"
#include "Navigator.h"
#include "NotificationWidget.h"
#include "RankVoteView.h"

RankVoteView::RankVoteView(Navigator **navigator, QWidget *parentWidget) :
    QObject(parentWidget) ,
    m_parentWidget(parentWidget),
    m_navigator(navigator)
{
}

void RankVoteView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Equal:
        upVote();
        event->accept();
        break;
    case Qt::Key_Minus:
        downVote();
        event->accept();
        break;
    }
}

void RankVoteView::upVote()
{
    Navigator *navigator = *m_navigator;
    if (navigator && navigator->currentImage()) {
        navigator->currentImage()->rank()->upVote();
        showNotification();
    }
}

void RankVoteView::downVote()
{
    Navigator *navigator = *m_navigator;
    if (navigator && navigator->currentImage()) {
        navigator->currentImage()->rank()->downVote();
        showNotification();
    }
}

void RankVoteView::showNotification()
{
    Navigator *navigator = *m_navigator;
    if (navigator && navigator->currentImage()) {
        NotificationWidget *w = new NotificationWidget(m_parentWidget);

        w->setTheme(NotificationWidget::ThemeMessage);
        int rank = navigator->currentImage()->rank()->value();
        w->setMessage(QString("Rank %1").arg(rank));

        w->showOnce();
    }
}
