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
#include "segment.h"

File::File( NzbFile *parent, quint32 bytes, const QStringList &groups, const QString &subject )
    : QList<Segment*>(),
      BaseType(),
      m_parent( parent ),
      m_bytes( bytes ),
      m_groups( groups ),
      m_subject( subject )
{
}

// File::File( const File &file )
//     : QList<Segment*>( file ),
//       BaseType( file ),
//       m_parent( parent ),
//       m_bytes( file.bytes() ),
//       m_groups( file.groups() ),
//       m_subject( file.subject() )
// {
// }

File::~File()
{
}

void File::dumpQueue()
{
    kDebug() << "parent:" << m_parent;
    kDebug() << "bytes:" << m_bytes;
    kDebug() << "groups:" << m_groups;
    kDebug() << "subject" << m_subject;
    for( int i = 0, size = this->size(); i < size; i++ ){
        at( i )->dumpQueue();
    }
}

void File::setParent( NzbFile *parent )
{
    if( m_parent ){
        m_parent->setBytes( m_parent->bytes() - m_bytes );
    }

    m_parent = parent;
    if( parent ){
        parent->setBytes( parent->bytes() + m_bytes );
    }
}

// File& File::operator=( const File &other )
// {
//     if( this != &other ){
//         m_parent = other.parent();
//         m_bytes = other.bytes();
//         m_groups = other.groups();
//         m_subject = other.subject();
//         BaseType::operator=( other );
//         QList<Segment*>::operator=( other );
//     }
//
//     return *this;
// }

// QDataStream& operator>>( QDataStream &in, File &data )
// {
//     in >> data.m_bytes;
//     in >> data.m_groups;
//     in >> data.m_subject;
//     in >> data;
//     return in;
// }
//
// QDataStream& operator<<( QDataStream &out, const File &data )
// {
//     out << data.m_bytes;
//     out << data.m_groups;
//     out << data.m_subject;
//     out << data;
//     return out;
// }