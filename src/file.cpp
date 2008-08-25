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

File::File( NzbFile *parent, const QString &subject, quint32 bytes, const QStringList &groups )
    : QList<Segment*>(),
      BaseType( BaseType::file ),
      m_parent( parent ),
      m_subject( subject ),
      m_bytes( bytes ),
      m_status( 0 ),
      m_groups( groups )
{
}

// File::File( const File &other )
//     : QList<Segment*>( other ),
//       BaseType( other )
// {
//     m_parent = other.m_parent;
//     m_bytes = other.m_bytes;
//     m_groups = other.m_groups;
//     m_subject = other.m_subject;
// }

File::~File()
{
    m_parent->setBytes( m_parent->bytes() - m_bytes );
}

void File::dumpQueue()
{
    print();

    for( int i = 0, size = this->size(); i < size; i++ ){
        at( i )->print();
    }
}

void File::print()
{
    printf( "subject: %s\n", m_subject.toStdString().c_str() );
    printf( "parent: %p\n", m_parent );
    printf( "bytes: %d\n", m_bytes );
    printf( "groups: %s\n", m_groups.join( "," ).toStdString().c_str() );
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
//         m_state = other.m_state;
//         m_parent = other.m_parent;
//         m_bytes = other.m_bytes;
//         m_groups = other.m_groups;
//         m_subject = other.m_subject;
//         BaseType::operator=( other );
//         QList<Segment*>::operator=( other );
// 
//         foreach( Segment *segment, *this ){
//             segment->m_parent = this;
//         }
// 
// //         for( int i = 0, size = this->size(); i < size; i++ ){
// //             (*this)[i]->m_parent = this;
// //         }
// 
//     }
//
//    return *this;
//}

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
