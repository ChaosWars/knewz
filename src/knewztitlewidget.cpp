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
#include <QDockWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEngine>
#include <QPainter>
#include <QResizeEvent>
#include "knewztitlewidget.h"

KNewzTitleWidget::KNewzTitleWidget(QWidget *parent)
 : KTitleWidget(parent)
{
    QFrame *frame = new QFrame( this );
    frame->setFrameShape( QFrame::StyledPanel );
    frame->setFrameShadow( QFrame::Raised );
    QHBoxLayout *layout = new QHBoxLayout( frame );
    QLabel *text = new QLabel( "Queue Manager", frame );
    text->setFont( QFont( "Sans", 10, QFont::Bold ) );
    layout->addWidget( text );
    layout->addStretch();
    QLabel *icon = new QLabel( frame );
    icon->setPixmap( KIcon( "view-choose" ).pixmap( 32, 32 ) );
    layout->addWidget( icon );
    frame->setLayout( layout );
    setWidget( frame );
//     setText( "Queue Manager" );
//     setComment( "Manage the download queue" );
//     setPixmap( KIcon( "view-choose" ).pixmap( 32, 32 ), KTitleWidget::ImageRight );
}

KNewzTitleWidget::~KNewzTitleWidget()
{
}

void KNewzTitleWidget::mouseDoubleClickEvent( QMouseEvent *event )
{
    event->ignore();
}

void KNewzTitleWidget::mouseMoveEvent( QMouseEvent *event )
{
    event->ignore();
}

void KNewzTitleWidget::mousePressEvent( QMouseEvent *event )
{
    event->ignore();
}

void KNewzTitleWidget::mouseReleaseEvent( QMouseEvent *event )
{
    event->ignore();
}

void KNewzTitleWidget::resizeEvent( QResizeEvent *event )
{
    QDockWidget *dockWidget = qobject_cast<QDockWidget*>(parentWidget());
    if (dockWidget->features() & QDockWidget::DockWidgetVerticalTitleBar) {
//         text->resize( text->height(), text->width() );
//         icon->paintEngine()->painter()->rotate( -90 );
//         text->paintEngine()->painter()->rotate( -90 );
    }else{
    }
//     event->accept();
}

#include "knewztitlewidget.moc"
