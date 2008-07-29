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
#include "nzbfile.h"

NzbFile::NzbFile( const QString &filename, quint32 bytes )
    : QList<File*>(), BaseType(), m_filename( filename ), m_bytes( bytes )
{
}

NzbFile::NzbFile( NzbFile &nzbFile )
    : QList<File*>(),
      BaseType(),
      m_filename( nzbFile.filename() ),
      m_bytes( nzbFile.bytes() )
{
}

NzbFile::~NzbFile()
{
}

NzbFile& NzbFile::operator=( NzbFile &nzbFile )
{
    if( this != &nzbFile ){
        m_filename = nzbFile.filename();
        m_bytes = nzbFile.bytes();
    }

    return *this;
}

// QDataStream& operator>>( QDataStream &in, NzbFile &data )
// {
//     in >> data;
//     return in;
// }
// 
// QDataStream& operator<<( QDataStream &out, const NzbFile &data )
// {
//     out << data;
//     return out;
// }
