#######################################################################
# Copyright 2008-2009  Martin Sandsmark <sandsmark@samfundet.no>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License or (at your option) version 3 or any later version
# accepted by the membership of KDE e.V. (or its successor approved
# by the membership of KDE e.V.), which shall act as a proxy
# defined in Section 14 of version 3 of the license.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#######################################################################

include_directories(${CMAKE_SOURCE_DIR}/filelight/src/part ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

set(radialmap_STAT_SRCS
    widget.cpp
    builder.cpp
    map.cpp
    widgetEvents.cpp
    labels.cpp
    segmentTip.cpp)

kde4_add_library(radialmap STATIC ${radialmap_STAT_SRCS} )
find_package(Blitz REQUIRED)

