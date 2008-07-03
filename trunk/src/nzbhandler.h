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

/**
 *   @file
 *       This file defines the class NzbHandler, for the purpose of parsing
 *       *.nzb files.
 *   @author
 *       Lawrence Lee
 */

#ifndef _NZBHANDLER_H
#define _NZBHANDLER_H

#include <QtXml>
#include <QStringList>
#include "nzbfile.h"

class File;

/**
 *  Class implementing QXmlDefaultHandler in order to parse *.nzb files.
 */
class NzbHandler : public QXmlDefaultHandler{
    public:
        /**
         *  Default constructor
         */
        NzbHandler();

        /**
         * Default destructor
         */
        ~NzbHandler();

        /**
         *  Reimplemented virtual function from QXmlDefaultHandler.
         *  @return @c true
         *      if the beginning of a valid xml document was encountered.
         */
        bool startDocument();

        /**
         *  Reimplemented virtual function from QXmlDefaultHandler.
         *  @return @c true
         *      if the end of a valid xml document was encountered.
         */
        bool endDocument();

        /**
         *  Reimplemented virtual function from QXmlDefaultHandler.
         *  None of the paramaters are used by the function internally.
         *  @param namespaceURI
         *      namespace URI
         *  @param localName
         *      local name
         *  @param qName
         *      qualified name
         *  @param atts
         *      attributes
         *  @return @c true
         *      if the beginning of an element is encountered.
         */
        bool startElement ( const QString &namespaceURI, const QString &localName,
                            const QString &qName, const QXmlAttributes &atts );

        /**
         *  Reimplemented virtual function from QXmlDefaultHandler.
         *  None of the paramaters are used by the function internally.
         *  @param namespaceURI
         *      namespace URI
         *  @param localName
         *      local name
         *  @param qName
         *      qualified name
         *  @return @c true
         *      if the beginning of an element is encountered.
         */
        bool endElement ( const QString &namespaceURI, const QString &localName, const QString &qName );

        /**
         *  Reimplemented virtual function from QXmlDefaultHandler.
         *  Reads the characters being read from the current element.
         *  @param ch
         *      the variable to set to the characters of the current element.
         *  @return
         *      true when finished reading the characters of the current element.
         */
        bool characters ( const QString &ch );

        /**
         *  Reimplemented from QXmlErrorHandler.
         *  Function is called when the handler wants to report a fatal error.
         *  @param exception
         *      details about the error are stored here.
         *  @return @c false
         *      since the errors cause this file to be called are such that the
         *      handler is unable to continue parsing the file.
         */
        bool fatalError ( const QXmlParseException &exception );

        /**
         *  Function that provides a QList of overloaded QList\< QStringLists\> that
         *  contain the data read from the *.nzb file.
         *
         *  @return
         *      The data read from the *.nzb file. The data consists of a QList
         *      that contains overloaded QList\< QStringList \>. Each QStringList represents
         *      a "part" that a binary file is often split into, and each QList\< QStringList\>
         *      represents a file that is made up of one or more parts. The whole represents
         *      the total contents of a *.nzb file.
         */
        NzbFile* nzbFile();

        /**
         * Set the file to be processed. It is absolutely imperative that you call this function
         * before processing a file.
         * @param file
         *      The file that is to be processed.
         */
        void setFilename( const QString &filename ){ m_filename = filename; }

    private:
        QString currentSubject, currentBytes, currentNumber, currentText, m_filename;
        QStringList groups;
        File *currentFile;
        NzbFile files;
        quint32 file_bytes, nzbfile_bytes;
};

#endif
