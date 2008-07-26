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
#include <KDE/KLocalizedString>
#include <KDE/KUrl>
#include <QTreeView>
#include "downloadqueue.h"
#include "file.h"
#include "knewzmodel.h"
#include "nzbfile.h"
#include "nzbreader.h"

KNewzModel::KNewzModel( QTreeView *parent )
    : QAbstractItemModel( parent ),
      view( parent ),
      downloadqueue( DownloadQueue::Instance() )
{
    rootItem << "" << i18n( "Subject" ) << i18n( "Size (MiB)" ) << i18n( "Status" ) << i18n( "ETA" );
    connect( parent, SIGNAL( clicked( const QModelIndex& ) ), SLOT( clicked( const QModelIndex& ) ) );
}

KNewzModel::~KNewzModel()
{
}

void KNewzModel::changed()
{
    emit layoutChanged();
}

int KNewzModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return 5;
}

QVariant KNewzModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if( role == Qt::CheckStateRole && index.column() == 0 ){
        BaseType *base = static_cast< BaseType* >( index.internalPointer() );
        return base->state();
    }

    if ( role != Qt::DisplayRole )
        return QVariant();

    const QModelIndex parentIndex = index.parent();

    if( !parentIndex.isValid() ){

        switch( index.column() ){
            case 0:
                break;
            case 1:
                return downloadqueue->at( index.row() )->filename();
                break;
            case 2:
                return QString().setNum( downloadqueue->at( index.row() )->bytes() /1048576.00, 'f', 2 );
                break;
            case 3:
                break;
            case 4:
                break;
            default:
                return QVariant();
                break;
        }

    }else{

        switch( index.column() ){
            case 0:
                break;
            case 1:
                return downloadqueue->at( parentIndex.row() )->at( index.row() )->subject();
                break;
            case 2:
                return QString().setNum( downloadqueue->at( parentIndex.row() )->at( index.row() )->bytes() /1048576.00, 'f', 2 );
                break;
            case 3:
                break;
            case 4:
                break;
            default:
                return QVariant();
                break;
        }

    }

    return QVariant();
}

bool KNewzModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent )
{
    if( data->hasFormat( "text/uri-list" ) ){

        if( data->hasUrls() ){
            QStringList files;

            foreach( QUrl url, data->urls() ){
                files << url.path();
            }

            NzbReader reader;
            QList<NzbFile*> nzbFiles;

            for( int i = 0, size = files.size(); i < size; i++ ){

                if( files.at( i ).size() > 0 ){
                    QString file( files.at( i ) );

                    NzbFile *nzbFile = reader.parseData( file );

                    if( nzbFile->size() > 0 ){
                        nzbFiles.append( nzbFile );
                    }
                }

            }

            if( parent.isValid() ){

                if( parent.parent().isValid() ){
                    downloadqueue->mutex().lock();

                    foreach( NzbFile *nzbFile, nzbFiles ){
                        downloadqueue->insert( parent.parent().row(), nzbFile );
                    }

                    downloadqueue->mutex().unlock();
                }else{
                    downloadqueue->mutex().lock();

                    foreach( NzbFile *nzbFile, nzbFiles ){
                        downloadqueue->insert( parent.row(), nzbFile );
                    }

                    downloadqueue->mutex().unlock();
                }

            }else{
                downloadqueue->mutex().lock();

                foreach( NzbFile *nzbFile, nzbFiles ){
                    downloadqueue->append( nzbFile );
                }

                downloadqueue->mutex().unlock();
            }
        }

        emit layoutChanged();
        return true;

    }

    if( data->hasFormat( "text/x-nzb" ) ){
        emit layoutChanged();
        return true;
    }

    return false;
}

Qt::ItemFlags KNewzModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags( index );

    if ( index.isValid() ){
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    }

    return Qt::ItemIsDropEnabled | defaultFlags;
}

QVariant KNewzModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        return rootItem.at( section );

    return QVariant();
}

QModelIndex KNewzModel::index( int row, int column, const QModelIndex &parent ) const
{
    if( !hasIndex( row, column, parent ) )
        return QModelIndex();

    if( !parent.isValid() )
        return createIndex( row, column, downloadqueue->at( row ) );

    BaseType *base = static_cast< BaseType* >( parent.internalPointer() );
    NzbFile *nzbFile;

    if( base->type() == "NzbFile" ){
        nzbFile = static_cast< NzbFile* >( parent.internalPointer() );
    }else{
        return QModelIndex();
    }

    File *file = nzbFile->value( row );

    if( !file )
        return QModelIndex();

    return createIndex( row, column, file );
}

bool KNewzModel::insertRows( int row, int count, const QModelIndex &parent )
{
    beginInsertRows( parent, row, row + count - 1 );
    endInsertRows();
    return true;
}

QStringList KNewzModel::mimeTypes() const
{
    QStringList mimetypes;
    mimetypes << "text/x-nzb";
    mimetypes << "text/uri-list";
    return mimetypes;
}

QModelIndex KNewzModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return QModelIndex();

    BaseType *base = static_cast< BaseType* >( index.internalPointer() );
    File *file;

    if( base->type() == "File" ){
        file = static_cast< File* >( index.internalPointer() );
    }else{
        return QModelIndex();
    }

    NzbFile *nzbFile = file->parent();
    return createIndex( downloadqueue->indexOf( nzbFile ), 0, nzbFile );
}

bool KNewzModel::removeRows( int row, int count, const QModelIndex &parent )
{
    int rows = row + count - 1;
    beginRemoveRows( parent, row, rows );

    if( parent.isValid() ){
        NzbFile *nzbFile = static_cast< NzbFile* >( parent.internalPointer() );

        while( row <= rows ){
            nzbFile->removeAt( row );
            rows--;
        }
    }else{

        while( row <= rows ){
            QMutableListIterator< File* > it( *(*downloadqueue)[row] );

            while( it.hasNext() ){
                it.next();
                it.remove();
            }

            downloadqueue->removeAt( row );
            rows--;
        }
    }

    endRemoveRows();
    return true;
}

int KNewzModel::rowCount( const QModelIndex &parent ) const
{
    if( parent.column() > 1 )
        return 0;

    if ( !parent.isValid() )
        return downloadqueue->size();

    if( !parent.parent().isValid() )
        return downloadqueue->at( parent.row() )->size();

    return 0;
}

Qt::DropActions KNewzModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

void KNewzModel::clicked( const QModelIndex& index )
{
    if( !( index.column() == 0 ) )
        return;

    BaseType *base = static_cast< BaseType* >( index.internalPointer() );
    Qt::CheckState checkstate = base->state() == Qt::Checked ? Qt::Unchecked : Qt::Checked;
    base->setState( checkstate );
    view->update( index );

    if( base->type() == "NzbFile" ){

        NzbFile *nzbFile = static_cast< NzbFile* >( index.internalPointer() );

        for( int i = 0, size = nzbFile->size(); i < size; i++ ){
            nzbFile->at( i )->setState( checkstate );
            view->update( index.child( i, 0 ) );
        }

    }else{
        NzbFile *nzbFile = static_cast< NzbFile* >( index.parent().internalPointer() );
        Qt::CheckState state = nzbFile->first()->state();
        int counter = 0;

        for( int i = 0, size = nzbFile->size(); i < size; i++ ){
            if( nzbFile->at( i )->state() == state  ){
                counter++;
            }
        }

        counter == nzbFile->size() ? nzbFile->setState( state ) : nzbFile->setState( Qt::PartiallyChecked );
        view->update( index.parent() );
    }

}

#include "knewzmodel.moc"