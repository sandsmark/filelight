// Maintainer:         Martin Sandsmark <sandsmark@samfundet.no> (C) 2008
// Original author:    Max Howell <max.howell@methylblue.com>, (C) 2003-4
// Copyright:          See COPYING file that comes with this distribution

#ifndef FILELIGHT_H
#define FILELIGHT_H

#include <kparts/mainwindow.h>

class KSqueezedTextLabel;
class KHistoryComboBox;
class KAction;
class KRecentFilesAction;

class ScanProgressBox;
class HistoryCollection;


namespace Filelight {

class Part;

class MainWindow : public KParts::MainWindow
{
  Q_OBJECT

  public:
    MainWindow();

    void scan(const KUrl &u) { slotScanUrl(u); }

  private slots:
    void slotUp();
    void slotComboScan();
    void slotScanDirectory();
    void slotScanHomeDirectory();
    void slotScanRootDirectory();
    bool slotScanUrl( const KUrl& );
    bool slotScanPath( const QString& );
    void slotAbortScan();

    void configToolbars();
    void configKeys();

    void scanStarted();
    void scanFailed();
    void scanCompleted();

    void urlAboutToChange();

  protected:
    virtual void saveProperties(KConfigGroup&);
    virtual void readProperties(const KConfigGroup&);
    virtual bool queryExit();

  private:
    Filelight::Part *m_part;

    KSqueezedTextLabel *m_status[2];
    KHistoryComboBox   *m_combo;
    HistoryCollection  *m_histories;
    KRecentFilesAction *m_recentScans;

    void setupStatusBar();
    void setupActions();
};

} // namespace Filelight

#endif
