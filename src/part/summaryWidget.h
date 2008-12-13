//Author:    Max Howell <max.howell@methylblue.com>, (C) 2004
//Copyright: See COPYING file that comes with this distribution

#ifndef FILELIGHTSUMMARY_H
#define FILELIGHTSUMMARY_H

#include <qwidget.h>


class SummaryWidget : public QWidget
{
    Q_OBJECT

public:
    SummaryWidget(QWidget *parent);
    ~SummaryWidget();

signals:
    void activated(const KUrl&);

private:
    void createDiskMaps();
};

#endif
