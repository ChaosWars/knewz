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
#include "nzbreader.h"

NzbReader::NzbReader()
{
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);
}

NzbReader::~NzbReader()
{
}

NzbFile* NzbReader::parseNetworkData(QByteArray &data, const QString &filename)
{
    QXmlInputSource source;
    source.setData(data);
    handler.setFilename(filename);

    if (reader.parse(&source))
    {
        return handler.nzbFile();
    }
    else
    {
        return new NzbFile();
    }
}

NzbFile* NzbReader::parseLocalData(const QString &path)
{
    QString p(path.split("/").last());
    p.chop(4);
    handler.setFilename(p);
    QFile file(path);
    QXmlInputSource source(&file);

    if (reader.parse(&source))
    {
        return handler.nzbFile();
    }
    else
    {
        return new NzbFile();
    }
}
