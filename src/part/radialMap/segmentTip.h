// Author: Max Howell <max.howell@methylblue.com>, (C) 2004
// Copyright: See COPYING file that comes with this distribution

#ifndef SEGMENTTIP_H
#define SEGMENTTIP_H

#include <qwidget.h>
//Added by qt3to4:
#include <QEvent>

class File;
class Directory;

namespace RadialMap
{
    class SegmentTip : public QWidget
    {
    public:
        SegmentTip( uint );

        void updateTip( const File*, const Directory* );
        void moveTo( QPoint, const QWidget&, bool );

    private:
        virtual bool eventFilter( QObject*, QEvent* );
        virtual bool event( QEvent* );

        uint    m_cursorHeight;
        QPixmap m_pixmap;
        QString m_text;
        bool    m_backing_store;
    };
}

#endif
