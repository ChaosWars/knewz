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
#include <KDE/KIcon>
#include <KDE/KStandardDirs>
#include <QFile>
#include <QDataStream>
#include <QNetworkCookie>
#include <QUrl>
#include "browserwidget.h"

KNewzCookieJar::KNewzCookieJar( QObject *parent )
    : QNetworkCookieJar( parent )
{
    QFile file( KStandardDirs::locateLocal( "appdata", "cookies/" ) + "cookies.dat" );
    QByteArray cookies;

    if( file.open( QIODevice::ReadOnly ) ){
        QDataStream stream( &file );
        stream >> cookies;
    }

    file.close();
    kDebug() << cookies;
    QList< QNetworkCookie > cookieList = QNetworkCookie::parseCookies( cookies );
    kDebug() << cookieList;
    setAllCookies( cookieList );
}

KNewzCookieJar::~KNewzCookieJar()
{
    QFile file( KStandardDirs::locateLocal( "appdata", "cookies/" ) + "cookies.dat" );

    if( file.open( QIODevice::WriteOnly ) ){
        QDataStream stream( &file );
        QList< QNetworkCookie > cookies = allCookies();

        foreach( const QNetworkCookie &cookie, cookies ){
            stream << cookie.toRawForm();
        }
    }
    file.close();
}

BrowserWidget::BrowserWidget(QWidget *parent)
 : QWidget(parent)
{
    setupUi( this );
    QNetworkAccessManager *nam = webView->page()->networkAccessManager();
    nam->setCookieJar( new KNewzCookieJar( nam ) );
    back->setIcon( KIcon( "go-previous" ) );
    forward->setIcon( KIcon( "go-next" ) );
    reload->setIcon( KIcon( "view-refresh" ) );
    stop->setIcon( KIcon( "process-stop" ) );
    backAction = webView->page()->action( QWebPage::Back );
    forwardAction = webView->page()->action( QWebPage::Forward );
    reloadAction = webView->page()->action( QWebPage::Reload );
    stopAction = webView->page()->action( QWebPage::Stop );
    connect( back, SIGNAL( clicked() ), backAction, SIGNAL( triggered() ) );
    connect( forward, SIGNAL( clicked() ), forwardAction, SIGNAL( triggered() ) );
    connect( reload, SIGNAL( clicked() ), reloadAction, SIGNAL( triggered() ) );
}

BrowserWidget::~BrowserWidget()
{
}

void BrowserWidget::load( const QString &string )
{
    QString url = string;

    if( url.left( 7 ) != "http://" )
        url.prepend( "http://" );

    webView->load( QUrl( url ) );
}

#include "browserwidget.moc"
