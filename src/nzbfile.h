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

/**
 * @class NzbFile nzbfile.h
 */
#ifndef NZBFILE_H
#define NZBFILE_H

#include <QtCore>
#include "basetype.h"

class File;

/**
 * @brief Container for the contents of an NZB file.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 *
 * @see File, Segment
 */

class NzbFile : public QList<File*>, public BaseType
{
    public:
//         enum{ NzbFileRole = Qt::UserRole };

        /**
         * Constructor.
         * @param filename
         *      The filename of the NZB file. This is it's name on the
         *      filesystem.
         * @param bytes
         *      The total size of the parts in the NZB file.
         */
        explicit NzbFile(const QString &filename = QString(), quint64 bytes = 0);

//         NzbFile( const NzbFile &other );
        ~NzbFile();

        /**
         * Debug function. Prints the content of the queue via kDebug();
         */
        virtual void dumpQueue();

        virtual void print();

        /**
         *  Sets the filename of the file.
         *  @param filename
         *      the filename of the file.
         */
        void setFilename(const QString &filename) { m_filename = filename; }

        /**
         *  Access function for the filename of the file.
         *  @return
         *      The filename of the file.
         */
        const QString filename() const { return m_filename; }

        /**
         *  Sets the size of the file in bytes.
         *  @param bytes
         *      the size of the file in bytes.
         */
        void setBytes(quint64 bytes) { m_bytes = bytes; }

        /**
         *  Access function for the size of the file.
         *  @return
         *      the size of the file in bytes.
         */
        quint64 bytes() const { return m_bytes; }

        void setStatus(quint64 status) { m_status = status; }

        quint64 status() const { return m_status; }

        /**
         * Returns the type of the object.
         *
         * @return
         *      The type of the object. This is the same as the name of the class,
         *      in this case "NzbFile"
         */
//         const QString type() const{ return QString( "NzbFile" ); }

        NzbFile& operator=(const NzbFile &other);

        /* QVariant stream operators */
//         friend QDataStream& operator>>( QDataStream &in, NzbFile &data );
//
//         friend QDataStream& operator<<( QDataStream &out, const NzbFile &data );

    private:
        QString m_filename;
        quint64 m_bytes;
        quint64 m_status;
};

Q_DECLARE_METATYPE(NzbFile);

#endif
