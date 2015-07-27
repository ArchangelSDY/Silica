#include "ImagePathCorrectorClientImpl.h"

#include <QApplication>
#include <QEvent>
#include <QEventLoop>

#include "ui/ImagePathCorrectorDialog.h"

class ImagePathCorrectorClientImpl::UIDelegate : public QObject
{
    Q_OBJECT
public:
    UIDelegate(ImagePathCorrectorClientImpl *client,
               QList<ImagePathCorrector::PathPatch> &patches)
        : m_client(client), m_patches(patches) {}

signals:
    void finished();

protected:
    virtual bool event(QEvent *event);

private:
    void execDialog();

    ImagePathCorrectorClientImpl *m_client;
    QList<ImagePathCorrector::PathPatch> &m_patches;
};

bool ImagePathCorrectorClientImpl::UIDelegate::event(QEvent *event)
{
    if (event->type() == QEvent::User) {
        execDialog();
        return true;
    } else {
        return false;
    }
}

void ImagePathCorrectorClientImpl::UIDelegate::execDialog()
{
    ImagePathCorrectorDialog dialog(m_patches, m_client->m_parent);
    if (dialog.exec() != QDialog::Accepted) {
        for (int i = 0; i < m_patches.count(); ++i) {
            m_patches[i].shouldApply = false;
        }
    }

    emit finished();
}

ImagePathCorrectorClientImpl::ImagePathCorrectorClientImpl(QWidget *parent) :
    m_parent(parent)
{
}

void ImagePathCorrectorClientImpl::prompt(
        QList<ImagePathCorrector::PathPatch> &patches)
{
    ImagePathCorrectorClientImpl::UIDelegate delegate(this, patches);
    delegate.moveToThread(QApplication::instance()->thread());

    QEventLoop waiter;
    QObject::connect(&delegate, SIGNAL(finished()),
                     &waiter, SLOT(quit()));

    QCoreApplication::postEvent(&delegate, new QEvent(QEvent::User));

    waiter.exec();
}

#include "ImagePathCorrectorClientImpl.moc"
