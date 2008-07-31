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
#include "downloadqueue.h"
#include "file.h"
#include "nzbfile.h"
#include "segment.h"

DownloadQueue* DownloadQueue::m_instance = NULL;
QMutex DownloadQueue::m_mutex;

DownloadQueue::DownloadQueue()
{
}

DownloadQueue::~DownloadQueue()
{
    kDebug();
    clear();
}

DownloadQueue* DownloadQueue::Instance()
{
    if( !m_instance ){
        QMutexLocker mutexLock( &m_mutex );
        /* Make sure that the instance wasn't created while we were
        waiting for the lock */
        if( !m_instance ){
            m_instance = new DownloadQueue();
        }
    }

    return m_instance;
}

void DownloadQueue::dumpQueue()
{
    for( int i = 0, count = DownloadQueue::Instance()->size(); i < count; i++ ){
        DownloadQueue::Instance()->at( i )->dumpQueue();
    }
}