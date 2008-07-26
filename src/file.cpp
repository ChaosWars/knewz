/***************************************************************************
 *   Copyright (C) 2007 by Lawrence Lee                                    *
 *   valheru.ashen.shugar@gmail.com                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <KDE/KDebug>
#include "file.h"
#include "nzbfile.h"

File::File( NzbFile *parent, quint32 bytes, const QStringList &groups, const QString &subject )
    : m_parent( parent ), m_bytes( bytes ), m_groups( groups ), m_subject( subject )
{
}

File::File( const File &file, NzbFile *parent )
{
    m_parent = parent;
    m_bytes = file.bytes();
    m_groups = file.groups();
    m_subject = file.subject();

    if( parent )
        parent->setBytes( parent->bytes() + m_bytes );
}

File::~File()
{
    kDebug() << "Destructor called";
//     qDeleteAll( begin(), end() );
//     clear();
}
