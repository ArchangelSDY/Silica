#ifndef COMMANDINTERPRETER_H
#define COMMANDINTERPRETER_H

#include <QObject>

#include "Navigator.h"

class CommandInterpreter : public QObject
{
    Q_OBJECT
public:
    explicit CommandInterpreter(Navigator *, QObject *parent = 0);

    void keyPress(QKeyEvent *);
    void reset();

    bool isEmpty() const;

signals:
    void commandChange(QString cmd);

private:
    void run();

    QString m_cmd;

    Navigator *m_navigator;
};

#endif // COMMANDINTERPRETER_H
