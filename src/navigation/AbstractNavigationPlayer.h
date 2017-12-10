#ifndef ABSTRACTNAVIGATIONPLAYER_H
#define ABSTRACTNAVIGATIONPLAYER_H

#include <QObject>

class QDialog;
class Navigator;

class AbstractNavigationPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AbstractNavigationPlayer(Navigator *navigator, QObject *parent = 0);
    virtual ~AbstractNavigationPlayer();

    virtual QString name() const;
    virtual void goNext() = 0;
    virtual void goPrev() = 0;
    virtual void goIndexUntilSuccess(int index, int delta);
    virtual void onEnter();
    virtual void onLeave();
    virtual void reset();

    int stepSize() const;
    void setStepSize(int stepSize);
    virtual QDialog *createConfigureDialog();
    virtual bool isConfigurable() const;
    virtual void cloneConfigurationFrom(AbstractNavigationPlayer *player);

protected:
    Navigator *m_navigator;
    int m_stepSize;
};

#endif // ABSTRACTNAVIGATIONPLAYER_H
