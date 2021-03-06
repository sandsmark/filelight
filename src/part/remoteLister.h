/***********************************************************************
* Copyright 2003-2004  Max Howell <max.howell@methylblue.com>
* Copyright 2008-2009  Martin Sandsmark <sandsmark@samfundet.no>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
* by the membership of KDE e.V.), which shall act as a proxy
* defined in Section 14 of version 3 of the license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#ifndef REMOTELISTER_H
#define REMOTELISTER_H

#include <KDirLister>

namespace Filelight
{
class RemoteLister : public KDirLister
{
    Q_OBJECT
public:
    RemoteLister(const KUrl &url, QWidget *parent);
    ~RemoteLister();

private slots:
    void completed();
    void _completed();
    void canceled();

private:
    class Store *m_root, *m_store;
};
}

#endif
