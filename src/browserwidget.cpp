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

#include <KDE/KIcon>
#include <QUrl>
#include "browserwidget.h"

BrowserWidget::BrowserWidget(QWidget *parent)
 : QWidget(parent)
{
    setupUi( this );
    kcfg_navbar->setDuplicatesEnabled( false );
    back->setIcon( KIcon( "go-previous" ) );
    forward->setIcon( KIcon( "go-next" ) );
    reload->setIcon( KIcon( "view-refresh" ) );
    stop->setIcon( KIcon( "process-stop" ) );
    go->setIcon( KIcon( "go-jump-locationbar" ) );
    backAction = webView->page()->action( QWebPage::Back );
    forwardAction = webView->page()->action( QWebPage::Forward );
    reloadAction = webView->page()->action( QWebPage::Reload );
    stopAction = webView->page()->action( QWebPage::Stop );
    connect( kcfg_navbar, SIGNAL( activated( const QString& ) ), this, SLOT( load( const QString& ) ) );
    connect( kcfg_navbar, SIGNAL( activated( const QString& ) ), kcfg_navbar, SLOT( addToHistory( const QString& ) ) );
    connect( back, SIGNAL( clicked() ), backAction, SIGNAL( triggered() ) );
    connect( forward, SIGNAL( clicked() ), forwardAction, SIGNAL( triggered() ) );
    connect( reload, SIGNAL( clicked() ), reloadAction, SIGNAL( triggered() ) );
    connect( go, SIGNAL( clicked() ), this, SLOT( goSlot() ) );
}

BrowserWidget::~BrowserWidget()
{
}

void BrowserWidget::goSlot()
{
    QString url( kcfg_navbar->currentText() );

    if( url.left( 6 ) != "http://" )
            url.prepend( "http://" );

    kcfg_navbar->changeURL( KUrl( url ), 0 );
    webView->load( QUrl(  ) );
}

void BrowserWidget::load( const QString &string )
{
    QString url = string;

    if( url.left( 6 ) != "http://" )
        url.prepend( "http://" );

    kcfg_navbar->changeURL( KUrl( url ), 0 );
    webView->load( QUrl( url ) );
}

#include "browserwidget.moc"
