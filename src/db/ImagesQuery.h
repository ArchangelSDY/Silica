#ifndef IMAGESQUERY_H
#define IMAGESQUERY_H

#include "AbstractQuery.h"
#include "PlayList.h"

class ImagesQuery : public AbstractQuery
{
    Q_OBJECT
public:
    explicit ImagesQuery(QString tag, QObject *parent = 0);

    PlayList *playList();

protected:
    void parseReply(QNetworkReply *);

private:
    QQueue<QSharedPointer<Image> > m_queuedImages;
};

#endif // IMAGESQUERY_H
