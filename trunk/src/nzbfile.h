/***************************************************************************
 *   Copyright (C) 2007 by Lawrence Lee   *
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
#ifndef NZBFILE_H
#define NZBFILE_H

#include <QtCore>
#include "basetype.h"

class File;

/**
	@author Lawrence Lee <valheru@facticius.net>
*/
class NzbFile : public QList<File*>, BaseType
{
    public:

        /**
         * Default constructor
         */
        NzbFile( const QString &filename = QString(), quint32 bytes = 0 );

        /**
         * Default destructor
         */
        ~NzbFile();

        /**
         *  Sets the filename of the file.
         *  @param filename
         *      the filename of the file.
         */
        void setFilename( const QString &filename ){ m_filename = filename; }

        /**
         *  Access function for the filename of the file.
         *  @return
         *      The filename of the file.
         */
        const QString filename() const{ return m_filename; }

        /**
         *  Sets the size of the file in bytes.
         *  @param bytes
         *      the size of the file in bytes.
         */
        void setBytes( quint32 bytes ){ m_bytes = bytes; }

        /**
         *  Access function for the size of the file.
         *  @return
         *      the size of the file in bytes.
         */
        quint32 bytes() const{ return m_bytes; }

        const QString type() const{ return QString( "NzbFile" ); }

    private:
        QString m_filename;
        quint32 m_bytes;
};

#endif
