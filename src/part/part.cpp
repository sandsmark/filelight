//Author:    Max Howell <max.howell@methylblue.com>, (C) 2003-4
//Copyright: See COPYING file that comes with this distribution

#include "Config.h"
#include "define.h"
#include "fileTree.h"
#include "part.h"
#include "progressBox.h"
#include "radialMap/widget.h"
#include "scan.h"
#include "settingsDialog.h"
#include "summaryWidget.h"

#include <kaboutdata.h>   //::createAboutData()
#include <kaction.h>
#include <kparts/browserextension.h>
#include <klocale.h>
#include <kmessagebox.h>  //::start()
#include <KActionCollection>
//#include <konq_operations.h>
#include <kparts/genericfactory.h>
#include <kstatusbar.h>
#include <kstandardaction.h>
#include <qfile.h>        //encodeName()
#include <qtimer.h>       //postInit() hack
#include <q3vbox.h>
//Added by qt3to4:
#include <Q3CString>
#include <QPixmap>
#include <unistd.h>       //access()


K_PLUGIN_FACTORY(filelightPartFactory, registerPlugin<Filelight::Part>();)  // produce a factory
K_EXPORT_PLUGIN(filelightPartFactory("filelight", "filelight") )

namespace Filelight {


//typedef KParts::GenericFactory<Filelight::Part> Factory;
//K_EXPORT_COMPONENT_FACTORY( libfilelight, Filelight::Factory )


BrowserExtension::BrowserExtension( Part *parent )
  : KParts::BrowserExtension( parent )
{}


Part::Part(QWidget *parentWidget, QObject *parent, const QList<QVariant>&)
        : ReadOnlyPart(parent)
        , m_ext(new BrowserExtension(this))
        , m_statusbar(new StatusBarExtension(this))
        , m_map(0)
        , m_manager(new ScanManager(this))
        , m_started(false)
{
    Config::read();
    KGlobal::locale()->insertCatalog("filelight");
    // we need an instance
    setComponentData( filelightPartFactory::componentData() );


//    setInstance(mainComponent());
    setWidget( new QWidget( parentWidget ) );
    setXMLFile( "filelight_partui.rc" );

    m_map = new RadialMap::Widget( widget() );
    m_map->hide();

    KStandardAction::zoomIn( m_map, SLOT(zoomIn()), actionCollection() );
    KStandardAction::zoomOut( m_map, SLOT(zoomOut()), actionCollection() );
    KStandardAction::preferences( this, SLOT(configFilelight()), actionCollection() )->setText( i18n( "Configure Filelight..." ) );

    connect( m_map, SIGNAL(created( const Directory* )), SIGNAL(completed()) );
    connect( m_map, SIGNAL(created( const Directory* )), SLOT(mapChanged( const Directory* )) );
    connect( m_map, SIGNAL(activated( const KUrl& )), SLOT(updateURL( const KUrl& )) );

    // TODO make better system
    connect( m_map, SIGNAL(giveMeTreeFor( const KUrl& )), SLOT(updateURL( const KUrl& )) );
    connect( m_map, SIGNAL(giveMeTreeFor( const KUrl& )), SLOT(openURL( const KUrl& )) );

    connect( m_manager, SIGNAL(completed( Directory* )), SLOT(scanCompleted( Directory* )) );
    connect( m_manager, SIGNAL(aboutToEmptyCache()), m_map, SLOT(invalidate()) );

    QTimer::singleShot( 0, this, SLOT(postInit()) );
}

void
Part::postInit()
{
   if( url().isEmpty() ) //if url is not empty openURL() has been called immediately after ctor, which happens
   {
      QWidget *summary = new SummaryWidget(widget());
      connect( summary, SIGNAL(activated( const KUrl& )), SLOT(openURL( const KUrl& )) );
      summary->show();

      //FIXME KXMLGUI is b0rked, it should allow us to set this
      //BEFORE createGUI is called but it doesn't
      stateChanged( "scan_failed" );
   }
}

bool
Part::openURL( const KUrl &u )
{
   //we don't want to be using the summary screen anymore
   delete widget()->child( "summaryWidget" );
   m_map->show();

   //TODO everyone hates dialogs, instead render the text in big fonts on the Map
   //TODO should have an empty KUrl until scan is confirmed successful
   //TODO probably should set caption to QString::null while map is unusable

   #define KMSG( s ) KMessageBox::information( widget(), s )

   KUrl uri = u;
   uri.cleanPath( KUrl::SimplifyDirSeparators );
   const QString path = uri.path( KUrl::RemoveTrailingSlash );
   const Q3CString path8bit = QFile::encodeName( path );
   const bool isLocal = uri.protocol() == "file";

   if( uri.isEmpty() )
   {
      //do nothing, chances are the user accidently pressed ENTER
   }
   else if( !uri.isValid() )
   {
      KMSG( i18n( "The entered URL cannot be parsed; it is invalid." ) );
   }
   else if( path[0] != '/' )
   {
      KMSG( i18n( "Filelight only accepts absolute paths, eg. /%1" ).arg( path ) );
   }
   else if( isLocal && access( path8bit, F_OK ) != 0 ) //stat( path, &statbuf ) == 0
   {
      KMSG( i18n( "Directory not found: %1" ).arg( path ) );
   }
   else if( isLocal && access( path8bit, R_OK | X_OK ) != 0 )
   {
      KMSG( i18n( "Unable to enter: %1\nYou do not have access rights to this location." ).arg( path ) );
   }
   else
   {
      if( uri == url() )
         m_manager->emptyCache(); //same as rescan()

      return start( uri );
   }

   return false;
}

bool
Part::closeURL()
{
   if( m_manager->abort() )
      statusBar()->message( i18n( "Aborting Scan..." ) );

   setUrl(KUrl());

   return true;
}

void
Part::updateURL( const KUrl &u )
{
   //the map has changed internally, update the interface to reflect this
   emit m_ext->openUrlNotify(); //must be done first
   emit m_ext->setLocationBarUrl( u.prettyUrl() );

   //do this last, or it breaks Konqi location bar
   setUrl(u);
}

void
Part::configFilelight()
{
    QWidget *dialog = new SettingsDialog( widget(), "settings_dialog" );

    connect( dialog, SIGNAL(canvasIsDirty( int )), m_map, SLOT(refresh( int )) );
    connect( dialog, SIGNAL(mapIsInvalid()), m_manager, SLOT(emptyCache()) );

    dialog->show(); //deletes itself
}

KAboutData*
Part::createAboutData()
{
    return new KAboutData(
		    "filelight",
		    0, 
		    ki18n("Filelight"),
		    APP_VERSION,
		    ki18n( "Displays file usage in an easy to understand way." ),
		    KAboutData::License_GPL,
		    ki18n( "(c) 2002-2004 Max Howell\n\
			    (c) 2008 Martin T. Sandsmark"), 
		    ki18n("Please report bugs."), 
		    "http://iskrembilen.com/", 
		    "sandsmark@iskrembilen.com" );
}

bool
Part::start( const KUrl &url )
{
   if( !m_started ) {
      m_statusbar->addStatusBarItem( new ProgressBox( statusBar(), this ), 0, true );
      connect( m_map, SIGNAL(mouseHover( const QString& )), statusBar(), SLOT(message( const QString& )) );
      connect( m_map, SIGNAL(created( const Directory* )), statusBar(), SLOT(clear()) );
      m_started = true;
   }

   if( m_manager->start( url ) ) {
      setUrl(url);

      const QString s = i18n( "Scanning: %1" ).arg( prettyUrl() );
      stateChanged( "scan_started" );
      emit started( 0 ); //as a Part, we have to do this
      emit setWindowCaption( s );
      statusBar()->message( s );
      m_map->invalidate(); //to maintain ui consistency

      return true;
   }

   return false;
}

void
Part::rescan()
{
   //FIXME we have to empty the cache because otherwise rescan picks up the old tree..
   m_manager->emptyCache(); //causes canvas to invalidate
   start( url() );
}

void
Part::scanCompleted( Directory *tree )
{
   if( tree ) {
      statusBar()->message( i18n( "Scan completed, generating map..." ) );

      m_map->create( tree );

      //do after creating map
      stateChanged( "scan_complete" );
   }
   else {
      stateChanged( "scan_failed" );
      emit canceled( i18n( "Scan failed: %1" ).arg( prettyUrl() ) );
      emit setWindowCaption( QString::null );

      statusBar()->clear();
//      QTimer::singleShot( 2000, statusBar(), SLOT(clear()) );

      setUrl(KUrl());
   }
}

void
Part::mapChanged( const Directory *tree )
{
   //IMPORTANT -> url() has already been set

   emit setWindowCaption( prettyUrl() );

   ProgressBox *progress = static_cast<ProgressBox *>(statusBar()->child( "ProgressBox" ));

   if( progress )
      progress->setText( tree->children() );
}

} //namespace Filelight

#include "part.moc"
