//Author:    Max Howell <max.howell@methylblue.com>, (C) 2004
//Copyright: See COPYING file that comes with this distribution

#include "Config.h"
#include "fileTree.h"
#include <kcursor.h>
#include <kiconeffect.h> //MyRadialMap::mousePressEvent()
#include <kiconloader.h>
#include <KIcon>
#include <klocale.h>
#include <qlabel.h>
#include <qlayout.h>
#include <q3textstream.h>
#include <QApplication>
//Added by qt3to4:
#include <Q3CString>
#include <Q3ValueList>
#include <QMouseEvent>
#include <QTextOStream>
#include "radialMap/radialMap.h"
#include "radialMap/widget.h"
#include "summaryWidget.h"
#include "summaryWidget.moc"


static Filelight::MapScheme oldScheme;


struct Disk
{
   QString device;
   QString type;
   QString mount;
   QString icon;

   int size;
   int used;
   int free; //NOTE used+avail != size (clustersize!)

   void guessIconName();
};


struct DiskList : Q3ValueList<Disk>
{
   DiskList();
};


class MyRadialMap : public RadialMap::Widget
{
public:
    MyRadialMap(QWidget *parent)
            : RadialMap::Widget(parent)
    {
    }

    virtual void setCursor(const QCursor &c)
    {
        if(focusSegment() && focusSegment()->file()->name() == "Used")
            RadialMap::Widget::setCursor(c);
        else
            unsetCursor();
    }

    virtual void mousePressEvent(QMouseEvent *e)
    {
        const RadialMap::Segment *segment = focusSegment();

        //we will allow right clicks to the center circle
        if(segment == rootSegment())
            RadialMap::Widget::mousePressEvent(e);

        //and clicks to the used segment
        else if(segment && segment->file()->name() == "Used") {
            const QRect rect(e->x() - 20, e->y() - 20, 40, 40);
//            KIconEffect::visualActivate(this, rect); TODO: Re-enable
            emit activated(url());
        }
    }
};



SummaryWidget::SummaryWidget(QWidget *parent)
        : QWidget(parent)
{
    qApp->setOverrideCursor(Qt::WaitCursor);
    setLayout(new QHBoxLayout(this));
    createDiskMaps();
    qApp->restoreOverrideCursor();
}

SummaryWidget::~SummaryWidget()
{
    Config::scheme = oldScheme;
}

void SummaryWidget::createDiskMaps()
{
    DiskList disks;

    const Q3CString free = i18n("Free").toLocal8Bit();
    const Q3CString used = i18n("Used").toLocal8Bit();

    KIconLoader loader;

    oldScheme = Config::scheme;
    Config::scheme = (Filelight::MapScheme)2000;

    for (DiskList::ConstIterator it = disks.begin(), end = disks.end(); it != end; ++it)
    {
        Disk const &disk = *it;

        if (disk.free == 0 && disk.used == 0)
            continue;

        QWidget *box = new QWidget(this);
	box->setLayout(new QVBoxLayout);
        box->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
        RadialMap::Widget *map = new MyRadialMap(box);

//        QString text; QTextOStream(&text)
//            << "<img src='" << loader.iconPath(disk.icon, KIconLoader::Toolbar) << "'>"
//            << " &nbsp;" << disk.mount << " "
//            << "<i>(" << disk.device << ")</i>";

        QLabel *label = new QLabel(disk.mount + " (" + disk.device + ")", box);
        label->setAlignment(Qt::AlignCenter);
        label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

	QLabel *icon = new QLabel();
	icon->setPixmap(KIcon::KIcon(disk.icon).pixmap(32,32));
	box->layout()->addWidget(icon);

	layout()->addWidget(box);
        box->show(); // will show its children too

        Directory *tree = new Directory(disk.mount.toLocal8Bit());
        tree->append(free, disk.free);
        tree->append(used, disk.used);

        map->create(tree); //must be done when visible

        connect(map, SIGNAL(activated(const KUrl&)), SIGNAL(activated(const KUrl&)));
    }
}


#if defined(_OS_LINUX_)
#define DF_ARGS       "-kT"
#else
#define DF_ARGS       "-k"
#define NO_FS_TYPE
#endif


DiskList::DiskList()
{
    //FIXME bug prone
    setenv("LANG", "en_US", 1);
    setenv("LC_ALL", "en_US", 1);
    setenv("LC_MESSAGES", "en_US", 1);
    setenv("LC_TYPE", "en_US", 1);
    setenv("LANGUAGE", "en_US", 1);

    char buffer[4096];
    FILE *df = popen("env LC_ALL=POSIX df " DF_ARGS, "r");
    int const N = fread((void*)buffer, sizeof(char), 4096, df);
    buffer[ N ] = '\0';
    pclose(df);

    QString output = QString::fromLocal8Bit(buffer);
    Q3TextStream t(&output, QIODevice::ReadOnly);
    QString const BLANK(QChar(' '));

    while (!t.atEnd()) {
        QString s = t.readLine();
        s = s.simplified();

        if (s.isEmpty())
            continue;

        if (s.indexOf(BLANK) < 0) // devicename was too long, rest in next line
            if (!t.eof()) { // just appends the next line
                QString v = t.readLine();
                s = s.append(v.toLatin1());
                s = s.simplified();
            }

        Disk disk;
        disk.device = s.left(s.indexOf(BLANK));
        s = s.remove(0, s.indexOf(BLANK) + 1);

    #ifndef NO_FS_TYPE
        disk.type = s.left(s.indexOf(BLANK));
        s = s.remove(0, s.indexOf(BLANK) + 1);
    #endif

        int n = s.indexOf(BLANK);
        disk.size = s.left(n).toInt();
        s = s.remove(0, n + 1);

        n = s.indexOf(BLANK);
        disk.used = s.left(n).toInt();
        s = s.remove(0, n + 1);

        n = s.indexOf(BLANK);
        disk.free = s.left(n).toInt();
        s = s.remove(0, n + 1);

        s = s.remove(0, s.indexOf(BLANK) + 1);  // delete the capacity 94%
        disk.mount = s;

        disk.guessIconName();

        *this += disk;
    }
}


void Disk::guessIconName()
{
   if(mount.contains("cdrom"))        icon = "media-optical";
   else if(device.contains("cdrom"))  icon = "media-optical";
   else if(mount.contains("writer"))  icon = "media-optical-recordable";
   else if(device.contains("writer")) icon = "media-optical-recordable";
//   else if(mount.contains("mo"))      icon = "mo";
//   else if(device.contains("mo"))     icon = "mo";
   else if(device.contains("fd")) {
//      if(device.contains("360"))      icon = "media-floppy";
//      if(device.contains("1200"))     icon = "media-floppy";
//      else
         icon = "media-floppy";
   }
   else if(mount.contains("floppy"))  icon = "media-floppy";
   else if(mount.contains("zip"))     icon = "media-floppy";
   else if(type.contains("nfs"))      icon = "network-server-database";
   else
      icon = "drive-harddisk";

//   icon += /*mounted() ? */"_mount"/* : "_unmount"*/;
}
