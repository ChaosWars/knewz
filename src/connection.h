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
#ifndef CONNECTION_H
#define CONNECTION_H

#include <QThread>
#include <QWaitCondition>

class Socket;

/**
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */

class Connection : public QThread
{
    Q_OBJECT

    public:
        Connection(QObject *parent = 0);
        ~Connection();
        void close();

    protected:
        void run();

    private:
        bool quit;
        Socket *socket;
        QWaitCondition cond;
		QStringList commandQueue;
};

#endif
