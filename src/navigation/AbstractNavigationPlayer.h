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

    virtual void goNext() = 0;
    virtual void goPrev() = 0;
    virtual void reset();

    enum Type {
        NormalType,
        HotspotsType,
        ExpandingType,
        FixedRegionType,
        CascadeClassifierType,
    };

    virtual Type type() const = 0;

    virtual QDialog *configureDialog() const;
    virtual bool isConfigurable() const;

protected:
    Navigator *m_navigator;

};

#endif // ABSTRACTNAVIGATIONPLAYER_H
