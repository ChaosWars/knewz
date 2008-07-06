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
#ifndef DOWNLOADQUEUE_H
#define DOWNLOADQUEUE_H

#include <QList>
#include <QMutex>
#include <QDebug>

class NzbFile;

/**
 * \class DownloadQueue downloadqueue.h
 *
 * \brief Provides a download queue for the application
 *
 * Provides a static mechanism for accessing the download queue.
 * This obviates the need for passing an instance of the download queue to all
 * classes that need to either read from it or append files to it.
 *
 *  @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class DownloadQueue{
    public:
        DownloadQueue(){}
        ~DownloadQueue(){}

        /**
         * Returns the download queue.
         * @return
         *      The download queue.
         */
        static QList<NzbFile*> queue(){ return m_queue; }

        /**
         * Appends a list of NzbFiles to the download queue. In a multithreaded
         * program, any attempts to add files to the download queue using this
         * method should aquire a lock in the mutex provided by mutex() beforehand.
         *
         * \see mutex
         *
         * @param nzbFiles
         *      The list of NzbFiles to append to the download queue.
         */
        static void append( const QList<NzbFile*> &nzbFiles ){ m_queue += nzbFiles; }

        /**
         * Returns the internal mutex that should be used to obtain a lock on the
         * download queue.
         *
         * @return
         *      The internal for locking the download queue mutex.
         */
        static QMutex& mutex(){ return m_mutex; }

    private:
        static QList<NzbFile*> m_queue;
        static QMutex m_mutex;
};

#endif
