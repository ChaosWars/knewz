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
#include <QApplication>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include "knewzview.h"
#include "nzbmimedata.h"

KNewzView::KNewzView( QWidget *parent )
 : QTreeView( parent )
{
    setDragEnabled( true );
    setAcceptDrops( true );
    setDropIndicatorShown( true );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
}

KNewzView::~KNewzView()
{
}

void KNewzView::dragEnterEvent( QDragEnterEvent *event )
{
    if( event->mimeData()->hasFormat( "text/uri-list" ) || event->mimeData()->hasFormat( "text/x-nzb" ) ){
//         QTreeView::dragEnterEvent( event );
        event->acceptProposedAction();
    }
}

void KNewzView::dragMoveEvent( QDragMoveEvent *event )
{
    if( event->mimeData()->hasFormat( "text/uri-list" ) || event->mimeData()->hasFormat( "text/x-nzb" ) ){
//         QTreeView::dragMoveEvent( event );
        event->acceptProposedAction();
    }
}

void KNewzView::dropEvent( QDropEvent *event )
{
    kDebug() << event->mimeData()->formats();
    if( event->mimeData()->hasFormat( "text/uri-list" ) ){
        model()->dropMimeData( event->mimeData(), event->proposedAction(), -1, -1, indexAt( event->pos() ) );
//         QTreeView::dropEvent( event );
        event->acceptProposedAction();
    }else if( event->mimeData()->hasFormat( "text/x-nzb" ) ){
        const NzbMimeData *mimeData = qobject_cast< const NzbMimeData* >( event->mimeData() );

        if( mimeData ){
            model()->dropMimeData( mimeData, event->proposedAction(), -1, -1, indexAt( event->pos() ) );
            event->acceptProposedAction();
        }
    }

}

// void KNewzView::mouseMoveEvent( QMouseEvent *event )
// {
//     if( !( event->buttons() & Qt::LeftButton ) )
//         return;
// 
//     QDrag *drag = new QDrag( this );
//     NzbMimeData *mimeData = new NzbMimeData();
//     QList< NzbFile* > data;
//     mimeData->setNzbData( data );
//     drag->setMimeData( mimeData );
// 
//     Qt::DropAction dropAction = drag->exec();
// }

// void KNewzView::mousePressEvent( QMouseEvent *event )
// {
//     if (event->button() == Qt::LeftButton)
//         dragStartPosition = event->pos();
// 
//     QTreeView::mousePressEvent( event );
// }

#include "knewzview.moc"
