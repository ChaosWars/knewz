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

NzbFile::NzbFile( const QString &filename, quint32 bytes )
    : QList<File*>(), BaseType(), m_filename( filename ), m_bytes( bytes )
{
}

NzbFile::NzbFile( const NzbFile &other )
    : QList<File*>( other ),
      BaseType( other )
{
    m_filename = other.m_filename;
    m_bytes = other.m_bytes;
}

NzbFile::~NzbFile()
{
}

void NzbFile::dumpQueue()
{
    print();

    for( int i = 0, size = this->size(); i < size; i++ ){
        at( i )->dumpQueue();
    }
}

void NzbFile::print()
{
    printf( "filename: %s\n", m_filename.toStdString().c_str() );
    printf( "bytes: %d\n", m_bytes );
}

NzbFile& NzbFile::operator=( const NzbFile &other )
{
    if( this != &other ){
        m_filename = other.m_filename;
        m_bytes = other.m_bytes;
        BaseType::operator=( other );
        QList<File*>::operator=( other );

        foreach( File *file, *this ){
            file->m_parent = this;
        }

//         for( int i = 0, size = this->size(); i < size; i++ ){
//             (*this)[i]->m_parent = this;
//         }
    }

    return *this;
}

// QDataStream& operator>>( QDataStream &in, NzbFile &data )
// {
//     in >> data.m_filename;
//     in >> data.m_bytes;
//     in >> data;
//     return in;
// }
// 
// QDataStream& operator<<( QDataStream &out, const NzbFile &data )
// {
//     out << data.m_filename;
//     out << data.m_bytes;
//     out << data;
//     return out;
// }
