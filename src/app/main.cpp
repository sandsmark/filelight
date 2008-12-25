// Maintainer:         Martin Sandsmark <sandsmark@samfundet.no> (C) 2008
// Original author:    Max Howell <max.howell@methylblue.com>, (C) 2003-4
// Copyright:          See COPYING file that comes with this distribution

#include "define.h"
#include "mainWindow.h"

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kurl.h>

static KAboutData about(
        APP_NAME,
	0,
	ki18n(APP_PRETTYNAME),
	APP_VERSION,
        ki18n("Graphical disk-usage information"),
	KAboutData::License_GPL_V2,
        ki18n("(C) 2006 Max Howell\n(C) 2008 Martin Sandsmark"),
	ki18n("You like it, you buy it!"),
        "http://www.methylblue.com/filelight/");


int main(int argc, char *argv[])
{
    using Filelight::MainWindow;

    about.addAuthor(ki18n("Martin Sandsmark"), ki18n("Maintainer"), "sandsmark@iskrembilen.com", "http://iskrembilen.com/");
    about.addAuthor(ki18n("Max Howell"),       ki18n("Original author"), "max.howell@methylblue.com", "http://www.methylblue.com/");
    about.addAuthor(ki18n("Mike Diehl"),       ki18n("Documentation"), 0, 0);
    about.addCredit(ki18n("Steffen Gerlach"),  ki18n("Inspiration"), 0, "http://www.steffengerlach.de/");
    about.addCredit(ki18n("André Somers"),     ki18n("Internationalization"));
    about.addCredit(ki18n("Stephanie James"),  ki18n("Testing"));
    about.addCredit(ki18n("Marcus Camen"),     ki18n("Bravery in the face of unreadable code"));

    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[path]", ki18n("Scan 'path'"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    if (!app.isSessionRestored()) {
        MainWindow *mw = new MainWindow();

        KCmdLineArgs* const args = KCmdLineArgs::parsedArgs();
        if (args->count() > 0) mw->scan( args->url(0));
        args->clear();

        mw->show();
    }
    else RESTORE(MainWindow);

    return app.exec();
}
