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
#include "knewzsettings.h"
#include "connection.h"
#include "socket.h"

Connection::Connection(QObject *parent)
        : QThread(parent), quit(false)
{
}


Connection::~Connection()
{
}

void Connection::run()
{
    socket = new Socket();
    socket->connectToHost();
    int retry_attempts = socket->retryAttempts();
    int retry = 0;
    int retry_delay = socket->retryDelay();

    while(!socket->waitForConnected(socket->timeout()))
    {
        kDebug() << socket->errorString();

        if(retry < retry_attempts)
        {
            kDebug() << "Sleeping on connection for " << retry_delay << " seconds";
            sleep(retry_delay);
            retry++;
        }
        else
        {
            kDebug() << "Finished retrying connection";
            break;
        }

        kDebug() << "Retrying connect: attempt #" << retry;
    }

    kDebug() << "Socket state: " << socket->state();

    if(socket->useSsl())
    {
        kDebug() << "Trying to secure SSL connection";
        int retry_attempts = socket->retryAttempts();
        int retry = 0;
        int retry_delay = socket->retryDelay();

        while(!socket->waitForEncrypted(socket->timeout()))
        {
            kDebug() << "SSL errors: " << socket->sslErrors();

            if(retry < retry_attempts)
            {
                kDebug() << "Sleeping on ssl connection for " << retry_delay << " seconds";
                sleep(retry_delay);
                retry++;
            }
            else
            {
                kDebug() << "Finished retrying ssl connection";
                break;
            }

            kDebug() << "Retrying ssl connection: attempt #" << retry;
        }

        kDebug() << "Socket mode: " << socket->mode();
        kDebug() << "Socket state: " << socket->state();
    }

    while (!quit)
    {
    }

    socket->close();
}

#include "connection.moc"
