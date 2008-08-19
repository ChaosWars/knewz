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
#include "nzbhandler.h"
#include "file.h"
#include "nzbfile.h"
#include "segment.h"

NzbHandler::NzbHandler()
    : m_nzbFile( 0 ), currentFile( 0 ), file_bytes( 0 ), nzbfile_bytes( 0 ), currentBytes( 0 )
{
}

NzbHandler::~NzbHandler()
{
}

bool NzbHandler::startDocument()
{
    if( m_filename.isEmpty() )
        return false;

    return true;
}

bool NzbHandler::endDocument()
{
    if( m_nzbFile->size() == 0 )
        return false;

    return true;
}

bool NzbHandler::startElement( const QString &/*namespaceURI*/, const QString &/*localName*/,
                               const QString &qName, const QXmlAttributes &atts )
{
    if( qName == "file" ){
        currentFile = new File( m_nzbFile, atts.value( "subject" ) );
        return true;
    }

    if( qName == "nzb" ){
        m_nzbFile = new NzbFile( m_filename );
        return true;
    }

    if( qName == "segment" ){
        currentBytes = atts.value( "bytes" ).toULong();
        file_bytes += currentBytes;
        currentNumber = atts.value( "number" );
        return true;
    }

    if( qName == "groups" || qName == "group" || qName == "segments" ){
        return true;
    }

    return false;
}

bool NzbHandler::endElement( const QString &/*namespaceURI*/, const QString &/*localName*/, const QString &qName )
{
    if( qName == "group" ){
        if( !groups.contains( currentText ) ){
            groups.append( currentText );
        }

        currentText.clear();
        return true;
    }

    if( qName == "file" ){
        currentFile->setBytes( file_bytes );
        currentFile->setGroups( groups );
        m_nzbFile->append( currentFile );
        nzbfile_bytes += static_cast<quint64>( file_bytes );
        groups.clear();
        file_bytes = 0;
        return true;
    }

    if( qName == "segment" ){
        currentFile->append( new Segment( currentFile, currentText, currentNumber.toInt(), currentBytes ) );
        currentBytes = 0;
        currentText.clear();
        return true;
    }

    if( qName == "nzb" ){
        m_nzbFile->setBytes( nzbfile_bytes );
        nzbfile_bytes = 0;

        for( int i = 0, size = m_nzbFile->size(); i < size; i++ ){
            m_nzbFile->at( i )->setParent( m_nzbFile );
        }

        return true;
    }

    if( qName == "groups" || qName == "segments" ){
        return true;
    }

    return false;
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
