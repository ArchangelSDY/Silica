#pragma once

#include "AbstractNavigationPlayer.h"

class MangaNavigationPlayer : public AbstractNavigationPlayer
{
    Q_OBJECT
    Q_CLASSINFO("Name", "Manga Player")
public:
    explicit MangaNavigationPlayer(Navigator *navigator,
                                   QWidget *view,
                                   QObject *parent = nullptr);

    virtual void goNext() override;
    virtual void goPrev() override;
    virtual void goIndexUntilSuccess(int index, int delta) override;
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