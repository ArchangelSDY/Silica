#ifndef HOTSPOTSEDITORVIEWSTATEMANAGER_H
#define HOTSPOTSEDITORVIEWSTATEMANAGER_H

#include "ViewStateManager.h"

class HotspotsEditorViewStateManager : public ViewStateManager
{
    Q_OBJECT
public:
    explicit HotspotsEditorViewStateManager(QObject *parent = 0);

    void navigationChange(int);
};

#endif // HOTSPOTSEDITORVIEWSTATEMANAGER_H
