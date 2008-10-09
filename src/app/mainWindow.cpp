//Author:    Max Howell <max.howell@methylblue.com>, (C) 2003-4
//Copyright: See COPYING file that comes with this distribution

#include "mainWindow.h"
#include "part/part.h"
#include "historyAction.h"

#include <cstdlib>            //std::exit()
#include <kaccel.h>           //KStandardShortcut namespace
#include <kaction.h>
#include <kapplication.h>     //setupActions()
#include <kcombobox.h>        //locationbar
#include <kconfig.h>
#include <kdirselectdialog.h> //slotScanDirectory
#include <kedittoolbar.h>     //for editToolbar dialog
#include <kkeydialog.h>
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
#include <kglobal.h>
#include <KShortcutsDialog>



namespace Filelight {

MainWindow::MainWindow()
        : KParts::MainWindow()
        , m_part( 0 )
{
    KLibFactory *factory = KLibLoader::self()->factory( "libfilelight" );

    if (!factory) {
       KMessageBox::error( this, i18n("KDE could not find the Filelight Part, or the Filelight Part could not be started. Did you make install?") );
       //exit() seems to not exist inside the std namespace for some users!
       using namespace std;
       exit( 1 ); //don't use QApplication::exit() - it causes a crash
    }

    m_part = (Part *)factory->create( this, "part", "KParts::ReadOnlyPart" );

    setCentralWidget( m_part->widget() );
    setStandardToolBarMenuEnabled( true );
    setupActions();
    createGUI( m_part );

    stateChanged( "scan_failed" ); //bah! doesn't affect the parts' actions, should I add them to the actionCollection here?

    QObjectList *buttons = toolBar()->queryList( "KToolBarButton" );
    if (buttons->isEmpty())
        KMessageBox::error( this, i18n("Filelight is not installed properly, consequently its menus and toolbars will appear reduced or even empty") );
    delete buttons;

    connect( m_part, SIGNAL(started( KIO::Job* )), SLOT(scanStarted()) );
    connect( m_part, SIGNAL(completed()), SLOT(scanCompleted()) );
    connect( m_part, SIGNAL(canceled( const QString& )), SLOT(scanFailed()) );

    //TODO test these
    connect( m_part, SIGNAL(canceled( const QString& )), m_histories, SLOT(stop()) );
    connect( BrowserExtension::childObject( m_part ), SIGNAL(openURLNotify()), SLOT(urlAboutToChange()) );

    KConfig* const config = KGlobal::config();
    config->setGroup( "general" );
    m_combo->setHistoryItems( config->readPathEntry( "comboHistory", QStringList() ) );
    applyMainWindowSettings( config, "window" );
}

inline void
MainWindow::setupActions() //singleton function
{
    KActionCollection *const ac = actionCollection();

    m_combo = new KHistoryComboBox( this, "history_combo" );
    m_combo->setCompletionObject( new KUrlCompletion( KUrlCompletion::DirCompletion ) );
    m_combo->setAutoDeleteCompletionObject( true );
    m_combo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    m_combo->setDuplicatesEnabled( false );

    KStandardAction::open( this, SLOT(slotScanDirectory()), ac, "scan_directory" );
    KStandardAction::quit( this, SLOT(close()), ac );
    KStandardAction::up( this, SLOT(slotUp()), ac );
    KStandardAction::configureToolbars(this, SLOT(configToolbars()), ac);
    KStandardAction::keyBindings(this, SLOT(configKeys()), ac);

    new KAction( i18n( "Scan &Home Directory" ), "folder_home", CTRL+Key_Home, this, SLOT(slotScanHomeDirectory()), ac, "scan_home" );
    new KAction( i18n( "Scan &Root Directory" ), "folder_red", 0, this, SLOT(slotScanRootDirectory()), ac, "scan_root" );
    new KAction( i18n( "Rescan" ), "reload", KStandardShortcut::reload(), m_part, SLOT(rescan()), ac, "scan_rescan" );
    new KAction( i18n( "Stop" ), "stop", Qt::Key_Escape, this, SLOT(slotAbortScan()), ac, "scan_stop" );
    new KAction( i18n( "Clear Location Bar" ), KApplication::reverseLayout() ? "clear_left" : "locationbar_erase", 0, m_combo, SLOT(clearEdit()), ac, "clear_location" );
    new KAction( i18n( "Go" ), "key_enter", 0, m_combo, SIGNAL(returnPressed()), ac, "go" );

    K3WidgetAction *combo = new K3WidgetAction( m_combo, i18n( "Location Bar" ), 0, 0, 0, ac, "location_bar" );
    m_recentScans = new KRecentFilesAction( i18n( "&Recent Scans" ), 0, ac, "scan_recent", 8 );
    m_histories = new HistoryCollection( ac, this, "history_collection" );

    ac->action( "scan_directory" )->setText( i18n( "&Scan Directory..." ) );
    m_recentScans->loadEntries( KGlobal::config() );
    combo->setAutoSized( true ); //FIXME what does this do?

    connect( m_recentScans, SIGNAL(urlSelected( const KUrl& )), SLOT(slotScanUrl( const KUrl& )) );
    connect( m_combo, SIGNAL(returnPressed()), SLOT(slotComboScan()) );
    connect( m_histories, SIGNAL(activated( const KUrl& )), SLOT(slotScanUrl( const KUrl& )) );
}

bool
MainWindow::queryExit()
{
    if( !m_part ) //apparently std::exit() still calls this function, and abort() causes a crash..
       return true;

    KConfig* const config = KGlobal::config();

    saveMainWindowSettings( config, "window" );
    m_recentScans->saveEntries( config );
    config->setGroup( "general" );
    config->writePathEntry( "comboHistory", m_combo->historyItems() );
    config->sync();

    return true;
}

inline void
MainWindow::configToolbars() //slot
{
    KEditToolBar dialog( factory(), this );
    dialog.showButtonApply( false );

    if( dialog.exec() )
    {
        createGUI( m_part );
        applyMainWindowSettings( KGlobal::config(), "window" );
    }
}

inline void
MainWindow::configKeys() //slot
{
    KShortcutsDialog::configure( actionCollection(), this );
}

inline void
MainWindow::slotScanDirectory()
{
    slotScanUrl( KDirSelectDialog::selectDirectory( m_part->url().path(), false, this ) );
}

inline void MainWindow::slotScanHomeDirectory() { slotScanPath( getenv( "HOME" ) ); }
inline void MainWindow::slotScanRootDirectory() { slotScanPath( "/" ); }
inline void MainWindow::slotUp()                { slotScanUrl( m_part->url().upUrl() ); }

inline void
MainWindow::slotComboScan()
{
   const QString path = KShell::tildeExpand(m_combo->lineEdit()->text());
   if (slotScanPath( path ))
      m_combo->addToHistory( path );
}

inline bool
MainWindow::slotScanPath( const QString &path )
{
   return slotScanUrl( KUrl::fromPathOrUrl( path ) );
}

bool
MainWindow::slotScanUrl( const KUrl &url )
{
   const KUrl oldUrl = m_part->url();
   const bool b = m_part->openURL( url );

   if (b) {
      m_histories->push( oldUrl );
      action( "go_back" )->KAction::setEnabled( false ); } //FIXME

   return b;
}

inline void
MainWindow::slotAbortScan()
{
    if( m_part->closeURL() ) action( "scan_stop" )->setEnabled( false );
}

inline void
MainWindow::scanStarted()
{
    stateChanged( "scan_started" );
    m_combo->clearFocus();
}

inline void
MainWindow::scanFailed()
{
    stateChanged( "scan_failed" );
    setActionMenuTextOnly( action( "go_up" ), QString::null );
    m_combo->lineEdit()->clear();
}

void
MainWindow::scanCompleted()
{
    KAction *goUp  = action( "go_up" );
    const KUrl url = m_part->url();

    stateChanged( "scan_complete" );

    m_combo->lineEdit()->setText( m_part->prettyUrl() );

    if ( url.path( 1 ) == "/") {
        goUp->setEnabled( false );
        setActionMenuTextOnly( goUp, QString() );
    }
    else
        setActionMenuTextOnly( goUp, url.upUrl().path( 1 ) );

    m_recentScans->addUrl( url ); //FIXME doesn't set the tick
}

inline void
MainWindow::urlAboutToChange()
{
   //called when part's URL is about to change internally
   //the part will then create the Map and emit completed()

   m_histories->push( m_part->url() );
}


/**********************************************
  SESSION MANAGEMENT
 **********************************************/

void
MainWindow::saveProperties( KConfig *config ) //virtual
{
   m_histories->save( config );
   config->writeEntry( "currentMap", m_part->url().path() );
}

void
MainWindow::readProperties( KConfig *config ) //virtual
{
   m_histories->restore( config );
   slotScanPath( config->readEntry( "currentMap", QString::null ) );
}

} //namespace Filelight



/// declared in historyAction.h

void setActionMenuTextOnly( KAction *a, QString const &suffix )
{
    QString const menu_text = suffix.isEmpty()
            ? a->text()
            : i18n( "&Up: /home/mxcl", "%1: %2" ).arg( a->text(), suffix );

    for (int i = 0; i < a->containerCount(); ++i) {
        QWidget *w = a->container( i );
        int const id = a->itemId( i );

        if (w->inherits( "QPopupMenu" ))
            static_cast<Q3PopupMenu*>(w)->changeItem( id, menu_text );

        else if (w->inherits( "KToolBar" )) {
            QWidget *button = static_cast<KToolBar*>(w)->getWidget( id );
            if (button->inherits( "KToolBarButton" ))
                QToolTip::add( button, suffix );
        }
    }
}

#include "mainWindow.moc"
