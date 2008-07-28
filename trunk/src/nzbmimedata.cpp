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
#include "nzbmimedata.h"
#include "nzbfile.h"

NzbMimeData::NzbMimeData()
 : QMimeData()
{
}

NzbMimeData::~NzbMimeData()
{
}

void NzbMimeData::setNzbData( const QList< NzbFile* > &data )
{
    m_data = data;
}

QList< NzbFile* > NzbMimeData::getNzbData()
{
    return m_data;
}

QStringList NzbMimeData::formats() const
{
    QStringList m_formats;
    m_formats << "text/x-nzb";
    return m_formats;
}

bool NzbMimeData::hasFormat( const QString &mimeType ) const
{
    if( mimeType == "text/x-nzb" ){
        return true;
    }

    return false;
}

// QVariant NzbMimeData::retrieveData( const QString &mimeType, QVariant::Type type) const
// {
//     kDebug();
// 
//     if( mimeType != "text/x-nzb" )
//         return QVariant();
// 
//     if( type != QVariant::UserType )
//         return QVariant();
// 
//     return QVariant();
// 
// }

#include "nzbmimedata.moc"
