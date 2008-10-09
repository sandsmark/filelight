//Author:    Max Howell <max.howell@methylblue.com>, (C) 2003-4
//Copyright: See COPYING file that comes with this distribution

#ifndef LOCALLISTER_H
#define LOCALLISTER_H

#include <qthread.h>
//Added by qt3to4:
#include <Q3CString>

class Directory;
template<class T> class Chain;

namespace Filelight
{
   class LocalLister : public QThread
   {
   public:
      LocalLister( const QString &path, Chain<Directory> *cachedTrees, QObject *parent );

      static bool readMounts();

   private:
      QString m_path;
      Chain<Directory> *m_trees;
      QObject *m_parent;

   private:
      virtual void run();
      Directory *scan( const Q3CString&, const Q3CString& );

   private:
      static QStringList s_localMounts, s_remoteMounts; //TODO namespace
   };
}

#endif
