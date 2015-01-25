#ifndef ABSTRACTNAVIGATIONPLAYER_H
#define ABSTRACTNAVIGATIONPLAYER_H

#include <QObject>

class Navigator;

class AbstractNavigationPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AbstractNavigationPlayer(Navigator *navigator, QObject *parent = 0);

    virtual void goNext() = 0;
    virtual void goPrev() = 0;
    virtual void reset();

    enum Type {
        NormalType,
        HotspotsType,
        ExpandingType,
    };

    virtual Type type() const = 0;

protected:
    Navigator *m_navigator;

};

#endif // ABSTRACTNAVIGATIONPLAYER_H
