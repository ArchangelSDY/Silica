#include "CommandInterpreter.h"

CommandInterpreter::CommandInterpreter(Navigator *navigator, RemoteDatabase *database,
                                       QObject *parent) :
    QObject(parent) ,
    m_navigator(navigator) ,
    m_database(database)
{
}

void CommandInterpreter::keyPress(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Return) {
        run();
        reset();
    } else if (ev->key() == Qt::Key_Backspace) {
        m_cmd = m_cmd.left(m_cmd.length() - 1);
    } else {
        m_cmd += ev->text();
    }

    emit commandChange(m_cmd);
}

void CommandInterpreter::run()
{
    // Query images
    if (m_cmd.startsWith("/")) {
        QString tag = m_cmd.mid(1);
        m_navigator->clearPlayList();
        m_database->queryByTag(tag);
    }
}

void CommandInterpreter::reset()
{
    m_cmd.clear();
    emit commandChange(m_cmd);
}

bool CommandInterpreter::isEmpty() const
{
    return m_cmd.isEmpty();
}
