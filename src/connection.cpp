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
        : QThread(parent), quit(false), socket(NULL)
{
}


Connection::~Connection()
{
	if(socket)
	{
		delete socket;
	}
}

void Connection::run()
{
    socket = new Socket();
    int retry_attempts = socket->retryAttempts();
    int retry_delay = socket->retryDelay();
	int timeout = socket->timeout();
	int retry = 0;
	socket->connectToHost();

    while(!socket->waitForConnected(timeout))
    {
        kdDebug() << socket->errorString();

        if(retry < retry_attempts)
        {
			kdDebug() << "Sleeping on connection for " << retry_delay << " seconds";
            sleep(retry_delay);
            retry++;
        }
        else
        {
			kdDebug() << "Finished retrying connection";
            break;
        }

		kdDebug() << "Retrying connect: attempt #" << retry;
    }

	kdDebug() << "Socket state: " << socket->state();

    if(socket->useSsl())
    {
        kdDebug() << "Trying to secure SSL connection";
        retry = 0;

		while(!socket->waitForEncrypted(timeout))
        {
			kdDebug() << "SSL errors: " << socket->sslErrors();

            if(retry < retry_attempts)
            {
				kdDebug() << "Sleeping on ssl connection for " << retry_delay << " seconds";
                sleep(retry_delay);
                retry++;
            }
            else
            {
				kdDebug() << "Finished retrying ssl connection";
                break;
            }

            kdDebug() << "Retrying ssl connection: attempt #" << retry;
        }

		kdDebug() << "Socket mode: " << socket->mode();
		kdDebug() << "Socket state: " << socket->state();
    }

    if(socket->state() == QSslSocket::ConnectedState)
    {
		kdDebug() << "Connected";

		while (!quit)
        {
			
			while(socket->waitForReadyRead(timeout) && !quit)
			{
				socket->parseReply(socket->readAll());
				kdDebug() << "wait for ready read";
			}

			kdDebug() << "Not quitting";
        }

		kdDebug() << "Quitting";
	}
	else
	{
		kdDebug() << "Not connected";
	}

    socket->close();
}

void Connection::close()
{
	quit = true;
}

#include "connection.moc"
