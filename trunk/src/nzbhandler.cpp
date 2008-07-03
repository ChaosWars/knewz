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
#include "nzbhandler.h"
#include "file.h"
#include "nzbfile.h"
#include "segment.h"

NzbHandler::NzbHandler()
{
    file_bytes = nzbfile_bytes = 0;
}

NzbHandler::~NzbHandler()
{
}

bool NzbHandler::startDocument()
{
    return true;
}

bool NzbHandler::endDocument()
{
    return true;
}

bool NzbHandler::startElement( const QString &/*namespaceURI*/, const QString &/*localName*/,
                               const QString &qName, const QXmlAttributes &atts )
{
    if( qName == "file" ){
        currentFile = new File();
        currentSubject = atts.value( "subject" );
        currentFile->setSubject( currentSubject );
    }

    if( qName == "segment" ){
        currentBytes = atts.value( "bytes" );
        file_bytes += atts.value( "bytes" ).toULong();
        currentNumber = atts.value( "number" );
    }

    return true;
}

bool NzbHandler::endElement( const QString &/*namespaceURI*/, const QString &/*localName*/, const QString &qName )
{
    if( qName == "group" ){
        if( !groups.contains( currentText ) )
            groups.append( currentText );

        currentText.clear();
    }

    if( qName == "file" ){
        currentFile->setBytes( file_bytes );
        currentFile->setGroups( groups );
        files.append( currentFile );
        nzbfile_bytes += file_bytes;
        currentText.clear();
        file_bytes = 0;
    }

    if( qName == "segment" ){
        currentFile->append( new Segment( currentText, currentNumber.toInt(), currentBytes.toULong() ) );
        currentText.clear();
    }

    if( qName == "nzb" ){
        nzbfile_bytes = 0;
    }

    return true;
}

bool NzbHandler::characters ( const QString &ch )
{
    currentText = ch;
    return true;
}

bool NzbHandler::fatalError( const QXmlParseException& exception )
{
    qWarning() << "Fatal error on line" << exception.lineNumber()
               << ", column" << exception.columnNumber() << ":"
               << exception.message();

    return false;
}

NzbFile* NzbHandler::nzbFile()
{
    return &files;
}
