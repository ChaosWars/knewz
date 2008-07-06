/***************************************************************************
 *   Copyright (C) 2006 by Lawrence Lee                                    *
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

/**
 * \class Segment segment.h
 */
#ifndef SEGMENT_H
#define SEGMENT_H

#include <QtCore>
#include "basetype.h"

class File;

/**
 * \brief Represents a segment of a file entry in a *.nzb file.
 *
 * \author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class Segment : public BaseType
{
    public:

        /**
         * 
         * @param parent 
         * @param id 
         * @param nr 
         * @param bytes 
         */
        Segment( File *parent, const QString &id, int nr, quint32 bytes );
        ~Segment();

        /**
         * 
         * @return 
         */
        quint32 bytes(){ return m_bytes; }

        /**
         * 
         * @return 
         */
        const QString id(){ return m_id; }

        /**
         * 
         * @return 
         */
        int nr(){ return m_nr; }

        /**
         * 
         * @return 
         */
        File *parent() const{ return m_parent; }

        /**
         * 
         * @return 
         */
        const QString type() const{ return QString( "Segment" ); }

    private:
        File *m_parent;
        QString m_id;
        int m_nr;
        quint32 m_bytes;
};

#endif
