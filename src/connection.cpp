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
        qDebug() << socket->errorString();

        if(retry < retry_attempts)
        {
			qDebug() << "Sleeping on connection for " << retry_delay << " seconds";
            sleep(retry_delay);
            retry++;
        }
        else
        {
			qDebug() << "Finished retrying connection";
            break;
        }

		qDebug() << "Retrying connect: attempt #" << retry;
    }

	qDebug() << "Socket state: " << socket->state();

    if(socket->useSsl())
    {
        qDebug() << "Trying to secure SSL connection";
        retry = 0;

		while(!socket->waitForEncrypted(timeout))
        {
			qDebug() << "SSL errors: " << socket->sslErrors();

            if(retry < retry_attempts)
            {
				qDebug() << "Sleeping on ssl connection for " << retry_delay << " seconds";
                sleep(retry_delay);
                retry++;
            }
            else
            {
				qDebug() << "Finished retrying ssl connection";
                break;
            }

            kDebug() << "Retrying ssl connection: attempt #" << retry;
        }

		qDebug() << "Socket mode: " << socket->mode();
		qDebug() << "Socket state: " << socket->state();
    }

    if(socket->state() == QSslSocket::ConnectedState)
    {
		qDebug() << "Connected";

		while (!quit)
        {
			
			while(socket->waitForReadyRead(timeout) && !quit)
			{
				socket->parseReply(socket->readAll());
				qDebug() << "wait for ready read";
			}

			qDebug() << "Not quitting";
        }

		qDebug() << "Quitting";
	}
	else
	{
		qDebug() << "Not connected";
	}

    socket->close();
}

void Connection::close()
{
	quit = true;
}

#include "connection.moc"
