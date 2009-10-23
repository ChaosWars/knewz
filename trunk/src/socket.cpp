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
#include <QThread>
#include "knewzsettings.h"
#include "socket.h"

Socket::Socket(QObject *parent)
        : QSslSocket(parent)
{
    m_server = KNewzSettings::server();
    m_port = KNewzSettings::port();
    m_timeout = KNewzSettings::timeout() * 1000;
    m_retry_attempts = KNewzSettings::retryAttempts();
    m_retry_delay = KNewzSettings::retryDelay();
    m_ssl = KNewzSettings::ssl();
}

Socket::~Socket()
{
}

void Socket::connectToHost()
{
    m_ssl ? QSslSocket::connectToHostEncrypted(m_server, m_port) :
            QSslSocket::connectToHost(m_server, m_port);
}

void Socket::parseReply(const QString &reply)
{
	Q_UNUSED(reply);
}

#include "socket.moc"
