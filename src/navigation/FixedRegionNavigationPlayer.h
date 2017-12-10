#ifndef FIXEDREGIONNAVIGATIONPLAYER_H
#define FIXEDREGIONNAVIGATIONPLAYER_H

#include <QList>

#include "AbstractNavigationPlayer.h"

class FixedRegionNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
    Q_CLASSINFO("Name", "Fixed Region Player")
public:
    explicit FixedRegionNavigationPlayer(Navigator *navigator,
                                         QWidget *view,
                                         QObject *parent = 0);

    void goNext() override;
    void goPrev() override;
    void goIndexUntilSuccess(int index, int delta) override;
    void onEnter() override;

    QDialog *createConfigureDialog() override;
    bool isConfigurable() const override;
    void cloneConfigurationFrom(AbstractNavigationPlayer *player) override;

private:
    QRectF calcFocusedRect() const;

    QWidget *m_view;
    QList<qreal> m_centers;
    int m_curCenterIndex;
};

#endif // FIXEDREGIONNAVIGATIONPLAYER_H
