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
 * @class Segment segment.h
 */
#ifndef SEGMENT_H
#define SEGMENT_H

#include <QtCore>

class File;

/**
 * @brief Represents a segment of a file entry in a *.nzb file.
 *
 * This represents a part of a NNTP multipart file.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class Segment
{
    public:

//         enum{ FileRole = Qt::UserRole + 2 };

        /**
         * Constructor
         * @param parent
         *      File parent of the Segment.
         * @param id
         *      The string identifying the Segment on the NNTP server.
         * @param nr
         *      The part number in the File.
         * @param bytes
         *      The size of the Segment.
         */
        Segment( File *parent = NULL, const QString &id = QString(), int nr = 0, quint32 bytes = 0 );

        Segment( const Segment &other );
        ~Segment();

        /**
         * Debug function. Prints the content of the queue via kDebug();
         */
        void dumpQueue();

        /**
         * Segment size in bytes.
         * @return
         *      The size of the segment.
         */
        quint32 bytes() const{ return m_bytes; }

        /**
         * The id of the Segment. This corresponds to the parts id on the
         * NNTP server, and can be used with a GET command to retrieve it.
         * @return
         *      The id of the segment.
         */
        const QString id() const{ return m_id; }

        /**
         * The number of the segment in the File.
         * @return
         *      The number of the segment.
         */
        int nr() const{ return m_nr; }

        /**
         * The parent of the Segment. This is the multipart file opf which
         * this part is a child.
         * @return
         *      The parent of the Segment.
         */
        File *parent() const{ return m_parent; }

        /**
         * Returns the type of the Segment.
         *
         * @return
         *      The string "Segment".
         *
         * \see NzbFile, File
         */
        const QString type() const{ return QString( "Segment" ); }

        /* QVariant stream operators */
        Segment& operator=( const Segment &other );
//         friend QDataStream& operator>>( QDataStream &in, Segment &data );
// 
//         friend QDataStream& operator<<( QDataStream &out, const Segment &data );
        
        friend class File;

    private:
        File *m_parent;
        QString m_id;
        int m_nr;
        quint32 m_bytes;
};

Q_DECLARE_METATYPE(Segment);

#endif
