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
#include <QEvent>
#include <QKeyEvent>
#include <QModelIndexList>
#include <QMutexLocker>
#include "basetype.h"
#include "downloadqueue.h"
#include "file.h"
#include "knewzview.h"
#include "nzbfile.h"
#include "nzbmimedata.h"

KNewzViewEventFilter::KNewzViewEventFilter( KNewzView *parent ) : m_parent( parent )
{
}

bool KNewzViewEventFilter::eventFilter( QObject *obj, QEvent *event )
{
    if( event->type() == QEvent::KeyPress ){
        QKeyEvent *keyEvent = dynamic_cast< QKeyEvent* >( event );

        if( keyEvent->key() == Qt::Key_Delete ){
            DownloadQueue *downloadqueue = DownloadQueue::Instance();
            QModelIndexList list = m_parent->selectionModel()->selectedRows();
            QList< BaseType* > rows;

            foreach( QModelIndex index, list ){
                BaseType *base = static_cast< BaseType* >( index.internalPointer() );

                if( base->type() == "File" ){
                    File *file = dynamic_cast< File* >( base );

                    if( file ){
                        /* If the current files parent is also in the list, then we don't want to process it.
                         * A return value of -1 means it's parent wasn't found, so we can add it.
                         */
                        if( list.indexOf( m_parent->model()->index( downloadqueue->indexOf( file->parent() ), 0 ) ) == -1 ){
                            rows.append( base );
                        }
                    }
                }else{
                    rows.append( base );
                }

            }

            QMutexLocker lock( &downloadqueue->mutex() );

            foreach( BaseType *base, rows ){

                if( base->type() == "NzbFile" ){
                    NzbFile *nzbFile = dynamic_cast< NzbFile* >( base );

                    if( nzbFile ){
                        m_parent->model()->removeRows( downloadqueue->indexOf( nzbFile ), 1 );
                    }

                }else{
                    File *file = dynamic_cast< File* >( base );

                    if( file ){
                        QModelIndex parent = m_parent->model()->index( downloadqueue->indexOf( file->parent() ), 0 );
                        m_parent->model()->removeRows( file->parent()->indexOf( file ), 1, parent );
                    }
                }

            }

        }else{
            return QObject::eventFilter( obj, event );
        }
    }

    return QObject::eventFilter( obj, event );
}

KNewzView::KNewzView( QWidget *parent )
 : QTreeView( parent )
{
    setUniformRowHeights( true );
    setDragEnabled( true );
    setAcceptDrops( true );
    setDropIndicatorShown( true );
    setSelectionMode( QAbstractItemView::ExtendedSelection );
    eventFilterObject = new KNewzViewEventFilter( this );
    installEventFilter( eventFilterObject );
}

KNewzView::~KNewzView()
{
    delete eventFilterObject;
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

// void KNewzView::mousePressEvent( QMouseEvent *event )
// {
//     if (event->button() == Qt::LeftButton)
//         dragStartPosition = event->pos();
// 
//     QTreeView::mousePressEvent( event );
// }

#include "knewzview.moc"
