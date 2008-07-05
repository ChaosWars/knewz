/***************************************************************************
 *   Copyright (C) 2006 by Lawrence Lee   *
 *   valheru@facticius.net   *
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
#ifndef SEGMENT_H
#define SEGMENT_H

#include <QtCore>
#include "basetype.h"

class File;

/**
 *  Class that represents a part of a file entry in a *.nzb file.
 */
class Segment : public BaseType
{
    public:
        Segment( File *parent, const QString &id, int nr, quint32 bytes );
        ~Segment();
        quint32 bytes(){ return m_bytes; }
        const QString id(){ return m_id; }
        int nr(){ return m_nr; }
        File *parent() const{ return m_parent; }
        const QString type() const{ return QString( "Segment" ); }

    private:
        File *m_parent;
        QString m_id;
        int m_nr;
        quint32 m_bytes;
};

#endif
