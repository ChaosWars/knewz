/***************************************************************************
 *   Copyright (C) 2007 by Lawrence Lee   *
 *   valheru@valheru@facticius.net   *
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
 *  @file
 *      This file defines the class File.
 *  @author
 *      Lawrence Lee
 */

#ifndef FILE_H
#define FILE_H

#include <QtCore>
#include "basetype.h"

class NzbFile;
class Segment;

/**
 *  Class implementing a QList of Segment that represents a file entry in a *.nzb file.
 */
class File : public QList<Segment*>, BaseType
{
    public:

        /**
         * The default constructor.
         *
         * @param parent
         * @param bytes
         * @param groups
         * @param subject
         */
        File( NzbFile *parent = NULL, quint32 bytes = 0, const QStringList &groups = QStringList(), const QString &subject = QString() );

        /**
         * Default destructor
         */
        ~File();

        /**
         *  Access function for the size of the file.
         *  @return
         *      the size of the file in bytes.
         */
        quint32 bytes() const{ return m_bytes; }

        /**
         *  Sets the size of the file in bytes.
         *  @param bytes
         *      the size of the file in bytes.
         */
        void setBytes( quint32 bytes ){ m_bytes = bytes; }

        /**
         *  Access function for the groups of the file.
         *  @return
         *      The groups where the file can be downloaded.
         */
        const QStringList groups() const{ return m_groups; }

        /**
         *  Sets the groups for the file.
         *  @param bytes
         *      The groups where the file can be downloaded.
         */
        void setGroups( const QStringList &groups ){ m_groups = groups; }

        /**
         * Access function for the NZB file the file belongs to
         * @return
         *      The parent of the file.
         */
        NzbFile* parent(){ return m_parent; }

        /**
         * Sets the parent of the file
         * @param parent
         *      The parent of the file.
         */
        void setParent( NzbFile *parent ){ m_parent = parent; }

        /**
         *  Access function for the subject of the file.
         *  @return
         *      The subject of the file.
         */
        const QString subject() const{ return m_subject; }

        /**
         *  Sets the subject of the file.
         *  @param subject
         *      the subject of the file.
         */
        void setSubject( const QString &subject ){ m_subject = subject; }

        /**
         * The type of the object. This is the implementation of the inherited function from
         * the base class BaseType.
         * @return
         *      The type File.
         */
        const QString type() const{ return QString( "File" ); };

    private:
        NzbFile *m_parent;
        quint32 m_bytes;
        QStringList m_groups;
        QString m_subject;
};

#endif
