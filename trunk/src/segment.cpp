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
#include "segment.h"

Segment::Segment(File *parent, const QString &id, int nr, quint32 bytes)
        : m_parent(parent), m_id(id), m_nr(nr), m_bytes(bytes)
{
}

// Segment::Segment(const Segment &other)
// {
//     m_parent = other.m_parent;
//     m_id = other.m_id;
//     m_nr = other.m_nr;
//     m_bytes = other.m_bytes;
// }

Segment::~Segment()
{
    m_parent->setBytes(m_parent->bytes() - m_bytes);
}

void Segment::print()
{
    printf("id: %s\n", m_id.toStdString().c_str());
    printf("nr: %d\n", m_nr);
    printf("parent: %p\n", m_parent);
    printf("bytes: %d\n", m_bytes);
}

/*Segment& Segment::operator=(const Segment &other)
{
    if(this != &other)
	{
        m_parent = other.m_parent;
        m_id = other.m_id;
        m_nr = other.m_nr;
        m_bytes = other.m_bytes;
    }

    return *this;
}*/

// QDataStream& operator>>( QDataStream &in, Segment &data )
// {
//     in >> data.m_id;
//     in >> data.m_nr;
//     in >> data.m_bytes;
//     return in;
// }
//
// QDataStream& operator<<( QDataStream &out, const Segment &data )
// {
//     out << data.m_id;
//     out << data.m_nr;
//     out << data.m_bytes;
//     return out;
// }
