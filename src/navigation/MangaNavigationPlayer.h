#pragma once

#include "AbstractNavigationPlayer.h"

class MangaNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
public:
    explicit MangaNavigationPlayer(Navigator *navigator,
                                   QWidget *view,
                                   QObject *parent = nullptr);

    virtual QString name() const override;
    virtual void goNext() override;
    virtual void goPrev() override;
    virtual void onEnter() override;
    virtual void onLeave() override;
    // virtual void reset() override;

private:
    enum class FocusPolicy
    {
        TopRight,
        BottomLeft,
    };

    void focus(FocusPolicy policy);

    QWidget *m_view;
};