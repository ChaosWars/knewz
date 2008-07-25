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
 *  \class File file.h
 */
#ifndef FILE_H
#define FILE_H

#include <QtCore>
#include "basetype.h"

class NzbFile;
class Segment;

/**
 *  \brief The segments in a file entry in a NZB file.
 *
 * Implements a QList of type Segment that represents the segments in a file
 * entry in a NZB file.
 *
 *  \author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class File : public QList<Segment*>, public BaseType
{
    public:

        /**
         * The default constructor.
         *
         * @param parent
         *      The NzbFile this File belongs to.
         *
         * @param bytes
         *      The size of the File.
         *
         * @param groups
         *      The groups where this file can be downloaded.
         *
         * @param subject
         *      The subject of the file.
         */
        File( NzbFile *parent = NULL, quint32 bytes = 0, const QStringList &groups = QStringList(),
              const QString &subject = QString() );

        /**
         * Overloaded version of the above function.
         * Note that the parent is set to NULL for this File, and should immediately
         * be set with setParent() after the File is created.
         */
        File( const File &file );
        ~File();

        /**
         *  The size of the file.
         *
         *  @return
         *      The size of the file in bytes.
         */
        quint32 bytes() const{ return m_bytes; }

        /**
         *  The groups of the file.
         *
         *  @return
         *      The groups where the file can be downloaded.
         */
        const QStringList groups() const{ return m_groups; }

        /**
         * The NZB file the file belongs to.
         * @return
         *      The parent of the file.
         */
        NzbFile* parent(){ return m_parent; }

        /**
         *  The subject of the file.
         *
         *  @return
         *      The subject of the file.
         */
        const QString subject() const{ return m_subject; }

        /**
         * Set the file size.
         *
         * @param bytes
         *      The file size in bytes.
         */
        void setBytes( quint32 bytes ){ m_bytes = bytes; }

        /**
         * Set the groups that the file can be downloaded from.
         *
         * @param groups
         *      The list of groups.
         */
        void setGroups( const QStringList &groups ){ m_groups = groups; }

        /**
         * The NZB file that the file belongs to.
         *
         * @param parent
         *      The parent of the file.
         *
         * \see NzbFile
         */
        void setParent( NzbFile *parent ){ m_parent = parent; }

        /**
         * The subject of the file found in the NZB file.
         *
         * @param subject
         *      The subject of the file.
         */
        void setSubject( const QString &subject ){ m_subject = subject; }

        /**
         * Returns the type of the object.
         *
         * @return
         *      The string "File".
         *
         * \see NzbFile, Segment
         */
        const QString type() const{ return QString( "File" ); }

    private:
        NzbFile *m_parent;
        quint32 m_bytes;
        QStringList m_groups;
        QString m_subject;
};

#endif
