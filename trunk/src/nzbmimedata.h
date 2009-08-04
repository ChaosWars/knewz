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
 * @class NzbMimeData nzbmimedata.h
 */
#ifndef NZBMIMEDATA_H
#define NZBMIMEDATA_H

#include <QMimeData>

class BaseType;

/**
 * @brief Custom NZB mime data for internal drag and drop.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */

class NzbMimeData : public QMimeData
{
        Q_OBJECT

    public:
        NzbMimeData();
        ~NzbMimeData();
        void setNzbData(const QList< BaseType* > &data);
        QList< BaseType* > getNzbData() const;
        virtual QStringList formats() const;
        virtual bool hasFormat(const QString &mimeType) const;

//     protected:
//         virtual QVariant retrieveData( const QString &mimeType, QVariant::Type type) const;

    private:
        QList< BaseType* > m_data;
};

#endif
