// Maintainer:         Martin Sandsmark <sandsmark@samfundet.no> (C) 2008
// Original author:    Max Howell <max.howell@methylblue.com>, (C) 2003-4
// Copyright:          See COPYING file that comes with this distribution

#ifndef Config_H
#define Config_H

#include <QStringList>

class KConfig;


namespace Filelight
{
    enum MapScheme { Rainbow, HighContrast, KDE, FileDensity, ModTime };

    class Config
    {
        static KConfig& kconfig();

    public:
        static void read();
        static void write();

        //keep everything positive, avoid using DON'T, NOT or NO

        static bool scanAcrossMounts;
        static bool scanRemoteMounts;
        static bool scanRemovableMedia;
        static bool varyLabelFontSizes;
        static bool showSmallFiles;
        static uint contrast;
        static uint antiAliasFactor;
        static uint minFontPitch;
        static uint defaultRingDepth;

        static MapScheme scheme;
        static QStringList skipList;
    };
}

using Filelight::Config;

#endif
