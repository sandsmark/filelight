//Author:    Max Howell <max.howell@methylblue.com>, (C) 2003-4
//Copyright: See COPYING file that comes with this distribution

#include "fileTree.h"
#include <kcursor.h>
#include "localLister.h"
#include <qapplication.h>
#include <KDebug>
//Added by qt3to4:
#include <QCustomEvent>
#include "remoteLister.h"
#include "scan.h"


namespace Filelight
{
   bool ScanManager::s_abort = false;
   uint ScanManager::s_files = 0;

   ScanManager::ScanManager(QObject *parent)
      : QObject(parent)
      , m_thread(0)
      , m_cache(new Chain<Directory>)
   {
      Filelight::LocalLister::readMounts();
   }

   ScanManager::~ScanManager()
   {
      if(m_thread) {
         kDebug() << "Attempting to abort scan operation..." << endl;
         s_abort = true;
         m_thread->wait();
      }

      delete m_cache;

      //RemoteListers are QObjects and get automatically deleted
   }

   bool ScanManager::running() const
   {
      //FIXME not complete
      return m_thread && m_thread->isRunning();
   }

   bool ScanManager::start(const KUrl &url)
   {
      //url is guarenteed clean and safe

      kDebug() << "Scan requested for: " << url.prettyUrl() << endl;

      if(running()) {
         //shouldn't happen, but lets prevent mega-disasters just in case eh?
         kWarning() << "Attempted to run 2 scans concurrently!\n";
         //TODO give user an error
         return false;
      }

      s_files = 0;
      s_abort = false;

      if(url.protocol() == "file")
      {
         const QString path = url.path( KUrl::AddTrailingSlash );

         Chain<Directory> *trees = new Chain<Directory>;

         /* CHECK CACHE
         *   user wants: /usr/local/
         *   cached:     /usr/
         *
         *   user wants: /usr/
         *   cached:     /usr/local/, /usr/include/
         */

         for( Iterator<Directory> it = m_cache->iterator(); it != m_cache->end(); ++it )
         {
            QString cachePath = (*it)->name();

            if( path.startsWith( cachePath ) ) //then whole tree already scanned
            {
               //find a pointer to the requested branch

               kDebug() << "Cache-(a)hit: " << cachePath << endl;

               QStringList split = path.mid(cachePath.length()).split('/');
               Directory *d = *it;
               Iterator<File> jt;

               while(!split.isEmpty() && d != NULL) //if NULL we have got lost so abort!!
               {
                  jt = d->iterator();

                  const Link<File> *end = d->end();
                  QString s = split.first(); s += '/';

                  for (d = 0; jt != end; ++jt)
                  if (s == (*jt)->name())
                  {
                     d = (Directory*)*jt;
                     break;
                  }

                  split.pop_front();
               }

               if(d)
               {
                  delete trees;

                  //we found a completed tree, thus no need to scan
                  kDebug() << "Found cache-handle, generating map.." << endl;

                  appendTree(d, true);

                  return true;
               }
               else
               {
                  //something went wrong, we couldn't find the directory we were expecting
                  kError() << "Didn't find " << path << " in the cache!\n";
                  delete it.remove(); //safest to get rid of it
                  break; //do a full scan
               }
            }
            else if(cachePath.startsWith(path)) //then part of the requested tree is already scanned
            {
               kDebug() << "Cache-(b)hit: " << cachePath << endl;
               it.transferTo(*trees);
            }
         }

         m_url.setPath(path); //FIXME stop switching between paths and KURLs all the time
         QApplication::setOverrideCursor(Qt::BusyCursor);
         //starts listing by itself
         m_thread = new Filelight::LocalLister(path, trees, this);
         return true;
      }

      m_url = url;
      QApplication::setOverrideCursor(Qt::BusyCursor);
      //will start listing straight away
      QObject *o = new Filelight::RemoteLister(url, (QWidget*)parent());
      o->setParent(this);
      o->setObjectName("remote_lister");
      return true;
   }

   bool
   ScanManager::abort()
   {
      s_abort = true;

      delete findChild<RemoteLister *>("remote_lister");

      return m_thread && m_thread->isRunning();
   }

   void
   ScanManager::emptyCache()
   {
      s_abort = true;

      if(m_thread && m_thread->isRunning())
         m_thread->wait();

      emit aboutToEmptyCache();

      m_cache->empty();
   }

   void
   ScanManager::appendTree(Directory *tree, bool finished)
   {
/*      Directory *tree = (Directory*)e->data(); */

      if(m_thread) {
          m_thread->terminate();
          m_thread->wait();
          delete m_thread; //note the lister deletes itself
          m_thread = 0;
      }

      emit completed(tree);

      if(tree) {
         //we don't cache foreign stuff
         //we don't recache stuff (thus only type 1000 events)
         if(m_url.protocol() == "file" && finished)
            //TODO sanity check the cache
            m_cache->append(tree);
      }
      else //scan failed
         m_cache->empty(); //FIXME this is safe but annoying

      QApplication::restoreOverrideCursor();
   }
}

#include "scan.moc"
