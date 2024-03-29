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

#ifndef BROWSER_H
#define BROWSER_H

#include <QNetworkCookieJar>
#include <QWidget>
#include "ui_browserwidget.h"

class QMovie;
class QNetworkReply;
class KNewz;

class KNewzCookieJar : public QNetworkCookieJar
{
	Q_OBJECT

    public:
        KNewzCookieJar(QObject *parent = 0);
        ~KNewzCookieJar();
};


/**
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */

class BrowserWidget : public QWidget, Ui::BrowserWidget
{
	Q_OBJECT

    public:
        explicit BrowserWidget(KNewz *mainWindow, QWidget *parent = 0);
        ~BrowserWidget();

    public Q_SLOTS:
        void load(const QString &string);

    private:
        KNewz *m_main;
        QAction *backAction, *forwardAction, *reloadAction, *stopAction;
        QNetworkReply *reply;
        QMovie *idleMovie, *loadingMovie;

    private Q_SLOTS:
        void loadStarted();
        void loadFinished(bool ok);
        void unsupportedContent(QNetworkReply *reply);
        void finishedDownload();
};

#endif
