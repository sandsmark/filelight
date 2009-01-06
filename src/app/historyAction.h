// Maintainer:         Martin Sandsmark <sandsmark@samfundet.no> (C) 2008
// Original author:    Max Howell <max.howell@methylblue.com>, (C) 2003-4
// Copyright:          See COPYING file that comes with this distribution

#ifndef HISTORYACTION_H
#define HISTORYACTION_H

#include <kaction.h>
#include <kurl.h>
#include <qstringlist.h>

class KConfigGroup;
class KActionCollection;


/// defined in mainWindow.cpp
void setActionMenuTextOnly(KAction *a, QString const &suffix);


class HistoryAction : KAction
{
    HistoryAction(const QString &text, KActionCollection *ac);

    friend class HistoryCollection;

public:
    virtual void setEnabled(bool b = true) { KAction::setEnabled(b ? !m_list.isEmpty() : false); }

    void clear() { m_list.clear(); KAction::setText(m_text); }

private:
    void setText();

    void push(const QString &path);
    QString pop();

    const QString m_text;
    QStringList m_list;
};


class HistoryCollection : public QObject
{
Q_OBJECT

public:
    HistoryCollection(KActionCollection *ac, QObject *parent);

    void save(KConfigGroup &configgroup);
    void restore(const KConfigGroup &configgroup);

public slots:
    void push(const KUrl&);
    void stop() { m_receiver = 0; }

signals:
    void activated(const KUrl&);

private slots:
    void pop();

private:
    HistoryAction *m_b, *m_f, *m_receiver;
};

#endif
