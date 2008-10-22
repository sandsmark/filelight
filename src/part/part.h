// Author:    Max Howell <max.howell@methylblue.com>, (C) 2003-4
// Copyright: See COPYING file that comes with this distribution

#ifndef FILELIGHTPART_H
#define FILELIGHTPART_H

#include <kparts/browserextension.h>
#include <kparts/statusbarextension.h>
#include <kparts/part.h>
#include <kurl.h>

class KAboutData;
using KParts::StatusBarExtension;
namespace RadialMap { class Widget; }
class Directory;


namespace Filelight
{
   class Part;

   class BrowserExtension : public KParts::BrowserExtension
   {
   public:
      BrowserExtension( Part* );
   };


   class Part : public KParts::ReadOnlyPart
   {
      Q_OBJECT

   public:
      Part( QWidget *, QObject *, const QStringList& );

      virtual bool openFile() { return false; } //pure virtual in base class
      virtual bool closeURL();

      QString prettyUrl() const { return url().protocol() == "file" ? url().path() : url().prettyUrl(); }

      static KAboutData *createAboutData();

   public slots:
      virtual bool openURL( const KUrl& );
      void configFilelight();
      void rescan();

   private slots:
      void postInit();
      void scanCompleted( Directory* );
      void mapChanged( const Directory* );

   private:
      KStatusBar *statusBar() { return m_statusbar->statusBar(); }

      BrowserExtension   *m_ext;
      StatusBarExtension *m_statusbar;
      RadialMap::Widget  *m_map;
      class ScanManager  *m_manager;

      bool m_started;

   private:
      bool start( const KUrl& );

   private slots:
      void updateURL( const KUrl & );
   };
}

#endif
