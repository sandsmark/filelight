//Author:    Max Howell <max.howell@methylblue.com>, (C) 2003-4
//Copyright: See COPYING file that comes with this distribution

#include "mainWindow.h"
#include "part/part.h"
#include "historyAction.h"

#include <cstdlib>            //std::exit()
// #include <kaccel.h>           //KStandardShortcut namespace
// #include <kaction.h>
#include <kapplication.h>     //setupActions()
#include <kcombobox.h>        //locationbar
#include <KHistoryComboBox>
#include <KRecentFilesAction>
#include <kconfig.h>
#include <kdirselectdialog.h> //slotScanDirectory
#include <kedittoolbar.h>     //for editToolbar dialog
#include <QLineEdit>
#include <KStandardShortcut>
#include <KFileDialog>
// #include <kkeydialog.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kshell.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kurl.h>
#include <kurlcompletion.h>   //locationbar
#include <qobject.h>
#include <q3popupmenu.h>
#include <qtooltip.h>
#include <KGlobal>
#include <KConfigGroup>
#include <KShortcutsDialog>
#include <KSharedConfig>
#include <KStandardAction>
#include <KActionCollection>



namespace Filelight {

MainWindow::MainWindow()
        : KParts::MainWindow()
        , m_part( 0 )
{
//    KLibFactory *factory = KLibLoader::self()->factory( "libfilelight" );
    KPluginFactory *factory = KPluginLoader("libfilelight").factory();
    

    if (!factory) {
       KMessageBox::error(this, i18n("Unable to load the Filelight Part.\nPlease make sure Filelight was correctly installed."));
       QApplication::exit();
//       std::exit(1); //don't use QApplication::exit() - it causes a crash
    }

//    m_part = static_cast<KParts::ReadWritePart *>(factory->create(this, "FilelightPart"));
    m_part = (Part *)factory->create( this, "filelightpart", QStringList() );

    if (m_part) {
        setCentralWidget(m_part->widget());
        setStandardToolBarMenuEnabled(true);
        setupActions();
        createGUI(m_part);
    
        stateChanged("scan_failed"); //bah! doesn't affect the parts' actions, should I add them to the actionCollection here?
    
        QObjectList buttons = toolBar()->queryList("KToolBarButton");
        if (buttons.isEmpty())
            KMessageBox::error(this, i18n("Filelight is not installed properly, consequently its menus and toolbars will appear reduced or even empty"));
        delete &buttons;
    
        connect(m_part, SIGNAL(started(KIO::Job*)), SLOT(scanStarted()));
        connect(m_part, SIGNAL(completed()), SLOT(scanCompleted()));
        connect(m_part, SIGNAL(canceled(const QString&)), SLOT(scanFailed()));
    
        //TODO test these
        connect(m_part, SIGNAL(canceled(const QString&)), m_histories, SLOT(stop()));
        connect(BrowserExtension::childObject(m_part), SIGNAL(openURLNotify()), SLOT(urlAboutToChange()));
    
        const KConfigGroup config = KGlobal::config()->group("general");
        m_combo->setHistoryItems(config.readPathEntry("comboHistory", QStringList()));
        applyMainWindowSettings(config, "window");
    } else {
	    KMessageBox::error(this, i18n("Unable to create part widget."));
       	    QApplication::exit();
    }
}

inline void
MainWindow::setupActions() //singleton function
{
    KActionCollection *const ac = actionCollection();

    m_combo = new KHistoryComboBox(this);
    m_combo->setCompletionObject(new KUrlCompletion(KUrlCompletion::DirCompletion));
    m_combo->setAutoDeleteCompletionObject(true);
    m_combo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    m_combo->setDuplicatesEnabled(false);

    KStandardAction::open(this, SLOT(slotScanDirectory()), ac);
    KStandardAction::quit(this, SLOT(close()), ac);
    KStandardAction::up(this, SLOT(slotUp()), ac);
    KStandardAction::configureToolbars(this, SLOT(configToolbars()), ac);
    KStandardAction::keyBindings(this, SLOT(configKeys()), ac);
    
    KAction* action;

    //KAction(KIcon("folder_home"), i18n( "Scan &Home Directory" ), this, SLOT(slotScanHomeDirectory()), ac, "scan_home" )
    action = ac->addAction("scan_home", this, SLOT(slotScanHomeDirectory())); 
    action->setText(i18n("Scan &Home Directory"));
    action->setIcon(KIcon("folder_home"));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Home));

    //new KAction( i18n( "Scan &Root Directory" ), "folder_red", 0, this, SLOT(slotScanRootDirectory()), ac, "scan_root" );
    action = ac->addAction("scan_root", this, SLOT(slotScanRootDirectory())); 
    action->setText(i18n("Scan &Root Directory"));
    action->setIcon(KIcon("folder_red"));

    //new KAction( i18n( "Rescan" ), "reload", KStandardShortcut::reload(), m_part, SLOT(rescan()), ac, "scan_rescan" );
    action = ac->addAction("scan_rescan", this, SLOT(rescan())); 
    action->setText(i18n("Rescan"));
    action->setIcon(KIcon("reload"));
    action->setShortcut(KStandardShortcut::reload());


    //new KAction( i18n( "Stop" ), "stop", Qt::Key_Escape, this, SLOT(slotAbortScan()), ac, "scan_stop" );
    action = ac->addAction("scan_stop", this, SLOT(slotAbortScan())); 
    action->setText(i18n("Stop"));
    action->setIcon(KIcon("stop"));
    action->setShortcut(Qt::Key_Escape);

    //new KAction( i18n( "Clear Location Bar" ), KApplication::reverseLayout() ? "clear_left" : "locationbar_erase", 0, m_combo, SLOT(clearEdit()), ac, "clear_location" );

    //new KAction( i18n( "Go" ), "key_enter", 0, m_combo, SIGNAL(returnPressed()), ac, "go" );
    action = ac->addAction("go", m_combo, SIGNAL(returnPressed())); 
    action->setText(i18n("Go"));
    action->setIcon(KIcon("key_enter"));

    //K3WidgetAction *combo = new K3WidgetAction( m_combo, i18n( "Location Bar" ), 0, 0, 0, ac, "location_bar" ); //TODO: Wtf was this good for?

    //m_recentScans = new KRecentFilesAction( i18n( "&Recent Scans" ), 0, ac, "scan_recent", 8 );
    m_recentScans = new KRecentFilesAction(i18n("&Recent Scans"), ac);
    m_recentScans->setMaxItems(8);

    m_histories = new HistoryCollection(ac, this, "history_collection");

    ac->action("scan_directory")->setText(i18n("&Scan Directory..."));
    m_recentScans->loadEntries(KGlobal::config()->group(""));
    //combo->setAutoSized( true ); //FIXME what does this do?

    connect(m_recentScans, SIGNAL(urlSelected( const KUrl&)), SLOT(slotScanUrl(const KUrl&)));
    connect(m_combo, SIGNAL(returnPressed()), SLOT(slotComboScan()));
    connect(m_histories, SIGNAL(activated(const KUrl&)), SLOT(slotScanUrl(const KUrl&)));
}

bool
MainWindow::queryExit()
{
    if(!m_part) //apparently std::exit() still calls this function, and abort() causes a crash..
       return true;

    KConfigGroup config = KGlobal::config()->group("general");

    saveMainWindowSettings(KGlobal::config()->group("window"));
    m_recentScans->saveEntries( config );
    config.writePathEntry("comboHistory", m_combo->historyItems());
    config.sync();

    return true;
}

inline void
MainWindow::configToolbars() //slot
{
    KEditToolBar dialog(factory(), this);
    // dialog.showButtonApply( false ); //TODO: Is this still needed?

    if (dialog.exec())
    {
        createGUI( m_part );
        applyMainWindowSettings(KGlobal::config()->group("window"));
    }
}

inline void
MainWindow::configKeys() //slot
{
    KShortcutsDialog::configure(actionCollection(), KShortcutsEditor::LetterShortcutsAllowed, this, true);
}

inline void
MainWindow::slotScanDirectory()
{
    slotScanUrl(KFileDialog::getExistingDirectoryUrl(m_part->url(), this, QString("Select directory to scan...")));
}

inline void MainWindow::slotScanHomeDirectory() { slotScanPath(getenv("HOME")); }
inline void MainWindow::slotScanRootDirectory() { slotScanPath("/"); }
inline void MainWindow::slotUp()                { slotScanUrl(m_part->url().upUrl()); }

inline void
MainWindow::slotComboScan()
{
   const QString path = KShell::tildeExpand(m_combo->lineEdit()->text());
   if (slotScanPath(path))
      m_combo->addToHistory(path);
}

inline bool
MainWindow::slotScanPath(const QString &path)
{
   return slotScanUrl(KUrl::fromPathOrUrl(path));
}

bool
MainWindow::slotScanUrl(const KUrl &url)
{
   const KUrl oldUrl = m_part->url();
   const bool b = m_part->openURL( url );

   if (b) {
      m_histories->push( oldUrl );
      action("go_back")->setEnabled( false ); } //FIXME

   return b;
}

inline void
MainWindow::slotAbortScan()
{
    if(m_part->closeURL()) action("scan_stop")->setEnabled(false);
}

inline void
MainWindow::scanStarted()
{
    stateChanged("scan_started");
    m_combo->clearFocus();
}

inline void
MainWindow::scanFailed()
{
    stateChanged("scan_failed");
    setActionMenuTextOnly(qobject_cast<KAction *>(action("go_up")), QString());
    m_combo->lineEdit()->clear();
}

void
MainWindow::scanCompleted()
{
    KAction *goUp  = qobject_cast<KAction *>(action("go_up"));
    const KUrl url = m_part->url();

    stateChanged("scan_complete");

    m_combo->lineEdit()->setText(m_part->prettyUrl());

    if (url.path(KUrl::LeaveTrailingSlash) == "/") {
        goUp->setEnabled(false);
        setActionMenuTextOnly(goUp, QString());
    }
    else
        setActionMenuTextOnly(goUp, url.upUrl().path(KUrl::LeaveTrailingSlash));

    m_recentScans->addUrl(url); //FIXME doesn't set the tick
}

inline void
MainWindow::urlAboutToChange()
{
   //called when part's URL is about to change internally
   //the part will then create the Map and emit completed()

   m_histories->push(m_part->url());
}


/**********************************************
  SESSION MANAGEMENT
 **********************************************/

void
MainWindow::saveProperties(KConfig *config) //virtual
{
   m_histories->save(config);
   config->group("General").writeEntry("currentMap", m_part->url().path());
}

void
MainWindow::readProperties(KConfig *config) //virtual
{
   m_histories->restore(config);
   slotScanPath(config->group("General").readEntry("currentMap", QString()));
}

} //namespace Filelight



/// declared in historyAction.h

void setActionMenuTextOnly(KAction *a, QString const &suffix)
{
    QString const menu_text = suffix.isEmpty()
            ? a->text()
            : i18nc("&Up: /home/mxcl", "%1: %2").arg(a->text(), suffix);

    for (int i = 0; i < a->associatedWidgets().count(); ++i) {
        QWidget *w = a->associatedWidgets().value(i);
//        int const id = a->itemId(i);

        /*if (w->inherits("QPopupMenu"))
            static_cast<Q3PopupMenu*>(w)->changeItem(id, menu_text);

        else */if (w->inherits("KToolBar")) {
            QWidget *button = static_cast<KToolBar*>(w);
            if (button->inherits("KToolBarButton"))
                QToolTip::add(button, suffix);
        }
    }
}

#include "mainWindow.moc"
