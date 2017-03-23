#ifndef FIXEDREGIONNAVIGATIONPLAYER_H
#define FIXEDREGIONNAVIGATIONPLAYER_H

#include <QList>

#include "AbstractNavigationPlayer.h"

class FixedRegionNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
public:
    explicit FixedRegionNavigationPlayer(Navigator *navigator,
                                         QWidget *view,
                                         QObject *parent = 0);

    QString name() const override;
    void goNext() override;
    void goPrev() override;
    void onEnter() override;

    QDialog *createConfigureDialog() override;
    bool isConfigurable() const override;

private:
    QRectF calcFocusedRect() const;

    QWidget *m_view;
    QList<qreal> m_centers;
    int m_curCenterIndex;
};

#endif // FIXEDREGIONNAVIGATIONPLAYER_H
