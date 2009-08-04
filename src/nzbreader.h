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
 * @class NzbReader nzbreader.h
 */
#ifndef _NZBREADER_H_
#define _NZBREADER_H_

#include <QtXml>
#include "nzbhandler.h"

class NzbFile;

class QNetworkReply;

/**
 * @brief Xml reader which handles the setup neccessary to read a NZB file.
 *
 * Performs the setup up of QXmlSimpleReader and QXmlInputSource in order to
 * parse a xml file - in this case, a *.nzb file.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 *
 * @see NzbHandler
 */

class NzbReader
{
    public:

        NzbReader();
        ~NzbReader();

        /**
         *  Returns a QList\<File*\> that contain the data read from the *.nzb file.
         *
         * @param path
         *      The NZB file to read.
         *
         * @return
         *      The data read from the *.nzb file. The data consists of a QList\<File*\>.
         *      Each File is a QList\<Segment*\> and represents a "part" that a binary file is often split into
         *      that is made up of one or more parts. The whole represents the total contents of a *.nzb file.
         *      If the parsing fails, the returned list will be empty. The size of the returned list should
         *      therefore always be checked and acted upon accordingly.
         */
        NzbFile* parseLocalData(const QString &path);

        /**
         * Overloaded function for reading nzb files downloaded from the built in web browser.
         * @param data
         *      The data residing in memory to read.
         * @return
         *      The parsed data.
         */
        NzbFile* parseNetworkData(QByteArray &data, const QString &filename);

    private:
        QXmlSimpleReader reader;
        NzbHandler handler;

};

#endif
