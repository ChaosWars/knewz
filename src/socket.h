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
#ifndef SOCKET_H
#define SOCKET_H

#include <QSslSocket>

/**
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
namespace ConnectionStatus
{
	enum Status
	{
		IDLE = 0,
		DOWNLOADING = 1
	};
}

class Socket : public QSslSocket
{
    Q_OBJECT

    public:
        Socket(QObject *parent = 0);
        ~Socket();
        void connectToHost();
        void parseReply(const QString &reply);
        QString server() const { return m_server; }
        void setServer(const QString &server) { m_server = server; }
        int port() const { return m_port; }
        void setPort(int port) { m_port = port; }
        int timeout() const { return m_timeout; }
        int retryAttempts() const { return m_retry_attempts; }
        int retryDelay() const { return m_retry_delay; }
        bool useSsl() const { return m_ssl; }
        void setUseSsl(bool useSsl) { m_ssl = useSsl; }

    private:
        QString m_server;
        int m_port;
        int m_timeout;
        int m_retry_attempts;
        int m_retry_delay;
        bool m_ssl;
};

#endif
