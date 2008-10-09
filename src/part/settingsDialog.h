//Author:    Max Howell <max.howell@methylblue.com>, (C) 2003-4
//Copyright: See COPYING file that comes with this distribution

#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include "dialog.h" //generated by uic
#include <qtimer.h>
//Added by qt3to4:
#include <QCloseEvent>


class SettingsDialog : public Dialog
{
Q_OBJECT

public:
  SettingsDialog( QWidget* =0, const char* =0 );

protected:
  virtual void closeEvent( QCloseEvent * );
  virtual void reject();

public slots:
  void addDirectory();
  void removeDirectory();
  void toggleScanAcrossMounts( bool );
  void toggleDontScanRemoteMounts( bool );
  void toggleDontScanRemovableMedia( bool );
  void reset();
  void startTimer();
  void toggleUseAntialiasing( bool = true );
  void toggleVaryLabelFontSizes( bool );
  void changeContrast( int );
  void changeScheme( int );
  void changeMinFontPitch( int );
  void toggleShowSmallFiles( bool );
  void slotSliderReleased();

signals:
  void mapIsInvalid();
  void canvasIsDirty( int );

private:
  QTimer m_timer;

  static const uint TIMEOUT=1000;
};

#endif
