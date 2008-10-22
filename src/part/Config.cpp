
#include "Config.h"
#include <kconfig.h>
#include <KConfigGroup>
#include <KSharedConfig>
#include <kglobal.h>


bool Config::scanAcrossMounts;
bool Config::scanRemoteMounts;
bool Config::scanRemovableMedia;
bool Config::varyLabelFontSizes;
bool Config::showSmallFiles;
uint Config::contrast;
uint Config::antiAliasFactor;
uint Config::minFontPitch;
uint Config::defaultRingDepth;
Filelight::MapScheme Config::scheme;
QStringList Config::skipList;

void
Filelight::Config::read()
{
    const KConfigGroup config = KGlobal::config()->group("filelight_part");

    scanAcrossMounts   = config.readEntry( "scanAcrossMounts", false );
    scanRemoteMounts   = config.readEntry( "scanRemoteMounts", false );
    scanRemovableMedia = config.readEntry( "scanRemovableMedia", false );
    varyLabelFontSizes = config.readEntry( "varyLabelFontSizes", true );
    showSmallFiles     = config.readEntry( "showSmallFiles", false );
    contrast           = config.readEntry( "contrast", 75 );
    antiAliasFactor    = config.readEntry( "antiAliasFactor", 2 );
    minFontPitch       = config.readEntry( "minFontPitch", QFont().pointSize() - 3);
    scheme = (MapScheme) config.readEntry( "scheme", 0 );
    skipList           = config.readEntry( "skipList", QStringList() );

    defaultRingDepth   = 4;
}

void
Filelight::Config::write()
{
    KConfigGroup config = KGlobal::config()->group("filelight_part");

    config.writeEntry( "scanAcrossMounts", scanAcrossMounts );
    config.writeEntry( "scanRemoteMounts", scanRemoteMounts );
    config.writeEntry( "scanRemovableMedia", scanRemovableMedia );
    config.writeEntry( "varyLabelFontSizes", varyLabelFontSizes );
    config.writeEntry( "showSmallFiles", showSmallFiles);
    config.writeEntry( "contrast", contrast );
    config.writeEntry( "antiAliasFactor", antiAliasFactor );
    config.writeEntry( "minFontPitch", minFontPitch );
    config.writeEntry( "scheme", (int)scheme ); // TODO: make the enum belong to a qwidget, 
    						//and use magic macros to make it save this properly
    config.writePathEntry( "skipList", skipList );
}
