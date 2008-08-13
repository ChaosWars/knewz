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
#include <QMutexLocker>
#include "downloadqueue.h"
#include "file.h"
#include "knewzmodel.h"
#include "knewzview.h"
#include "nzbfile.h"
#include "nzbmimedata.h"
#include "nzbreader.h"

KNewzModel::KNewzModel( KNewzView *parent )
    : BaseModel( parent ),
      m_parent( parent ),
      downloadqueue( DownloadQueue::Instance() )
{
    rootItem << "" << i18n( "Subject" ) << i18n( "Size (MiB)" ) << i18n( "Status" ) << i18n( "ETA" );
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

bool KNewzModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int /*row*/, int /*column*/, const QModelIndex &parent )
{
    if( action == Qt::IgnoreAction )
        return true;

    //External drop, ie. drag and drop from exploder/dolphin/konqueror/whatever Jobs is screwing you over with...
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

            if( nzbFiles.size() == 0 )
                return false;

            QMutexLocker lock( &downloadqueue->mutex() );
            if( parent.isValid() ){

                /* We only want to add stuff to the top level if we are dropping NZB files */
                int m_row = parent.parent().isValid() ? parent.parent().row() : parent.row();
                insertRows( m_row, nzbFiles.size() );

                foreach( NzbFile *nzbFile, nzbFiles ){
                    QModelIndex idx = index( m_row, 0 );

                    if( !idx.isValid() )
                        continue;

                    setData( idx, QVariant::fromValue( *nzbFile ) );
                    m_row++;
                }

            }else{
                /* Top level drop */
                int m_row = rowCount();
                insertRows( m_row, nzbFiles.size() );

                foreach( NzbFile *nzbFile, nzbFiles ){
                    QModelIndex idx = index( m_row, 0 );

                    if( !idx.isValid() )
                        continue;

                    setData( idx, QVariant::fromValue( *nzbFile ) );
                    m_row++;
                }

            }
        }

        return true;

    }

    //Internal drop, ie. user dragging and dropping rows in the view
    if( data->hasFormat( "text/x-nzb" ) ){
        const NzbMimeData *nzbMimeData = dynamic_cast< const NzbMimeData* >( data );
        QList< BaseType* > nzbData = nzbMimeData->getNzbData();

        if( !nzbMimeData )
            return false;

        QMutexLocker lock( &downloadqueue->mutex() );
        /* Note: The list is garuanteed to contain only parents who's children are not in the list.
           See mimeData()*/
        foreach( BaseType *base, nzbData ){

            if( base->type() == "NzbFile" ){
                NzbFile *nzbFile = dynamic_cast< NzbFile* >( base );

                if( !nzbFile )
                    continue;

                /* We can do this without an operator== in NzbFile since although the Qt docs claim
                   you must have it, the compiler just compares the memory address for pointers.
                   Since a copy operation on a QList of pointers just copies the pointer, the memory
                   in all lists will point to the same object. */
                int row = downloadqueue->indexOf( nzbFile );

                if( row >= 0 && row < downloadqueue->size() ){
                    /*We can't use remove/insertRows() here, and we need to implement the
                      begin/endRemove/InsertRows to avoid corruption of the model/view */
                    QModelIndex idx = index( row, 0 );

                    if( !idx.isValid() )
                        break;

                    bool expanded = view->isExpanded( idx );
                    beginRemoveRows( QModelIndex(), row, row );
                    //Essentially take yourself...oh, the innuendo inherent in this operation :p
                    nzbFile = downloadqueue->takeAt( row );
                    int rows = nzbFile->size();
                    endRemoveRows();
                    /* Chaining ternary expressions...besides looking cool, it also deals with wankers who think dropping
                       top-level items on other peoples children is funny */
                    row = parent.parent().isValid() ? parent.parent().row() : ( parent.isValid() ? parent.row() : rowCount() );

                    if( ( row < 0 ) || ( row > downloadqueue->size() ) )
                        break;

                    beginInsertRows( QModelIndex(), row, row );
                    downloadqueue->insert( row, nzbFile );
                    endInsertRows();
                    QModelIndex newIdx = index( row, 0 );

                    if( !newIdx.isValid() )
                        continue;

                    emit dataChanged( newIdx, index( rows, columnCount(), newIdx ) );

                    if( expanded )
                        view->setExpanded( newIdx, true );
                }
            }else{
                File *file = dynamic_cast< File* >( base );

                //Only process the drop if it is dropped on it's parent or on another child
                if( file->parent() == parent.internalPointer() || file->parent() == parent.parent().internalPointer() ){
                    NzbFile *nzbFile = file->parent();
                    int row = nzbFile->indexOf( file );

                    if( row >= 0 && row < nzbFile->size() ){
                        QModelIndex parentIdx = index( downloadqueue->indexOf( nzbFile ), 0 );

                        if( !parentIdx.isValid() )
                            break;

                        QModelIndex idx = index( row, 0, parentIdx );

                        if( !idx.isValid() )
                            break;

                        beginRemoveRows( parentIdx, row, row );
                        file = nzbFile->takeAt( row );
                        endRemoveRows();
                        row = file->parent() == parent.internalPointer() ? 0 : parent.row();

                        if( ( row < 0 ) ||  row > nzbFile->size() )
                            break;

                        beginInsertRows( parentIdx, row, row );
                        nzbFile->insert( row, file );
                        endInsertRows();
                        emit dataChanged( index( row, 0, parentIdx ), index( row, columnCount(), parentIdx ) );
                    }

                }
            }

        }
        //Unselect rows, otherwise the wrong rows become selected, which is odd and confusing.
        view->selectionModel()->clearSelection();
        return true;
    }

    return false;
}

Qt::ItemFlags KNewzModel::flags( const QModelIndex &index ) const
{
    if ( index.isValid() )
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags( index );

    return Qt::ItemIsDropEnabled | QAbstractItemModel::flags( index );
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

bool KNewzModel::insertChildren( const QModelIndex &parent, const NzbFile &nzbFile, int row )
{
    if( !parent.isValid() )
        return false;

    if( parent.column() > 0 )
        return false;

    insertRows( row, nzbFile.size(), parent );

    foreach( File *file, nzbFile ){;
        setData( index( row, 0 ), QVariant::fromValue( *file ) );
        row++;
    }

    return true;
}

bool KNewzModel::insertRows( int row, int count, const QModelIndex &parent )
{
    if( row < 0 )
        return false;

    if( count < 1 )
        return false;

    if( parent.parent().isValid() )
        return false;

    beginInsertRows( parent, row, row + count - 1 );

    if( parent.isValid() ){

        for( int i = 0; i < count; i++ ){
            NzbFile *nzbFile = downloadqueue->at( parent.row() );
            File *file = new File();
            file->setParent( nzbFile );

            if( row >= nzbFile->size() )
                nzbFile->append( file );
            else
                nzbFile->insert( row, file );
        }

    }else{

        bool append = ( row >= downloadqueue->size() );

        for( int i = 0; i < count; i++ ){
            if( !append )
                downloadqueue->insert( row, new NzbFile() );
            else
                downloadqueue->append( new NzbFile() );
        }

    }

    endInsertRows();
    return true;
}

QMimeData* KNewzModel::mimeData(const QModelIndexList &indexes) const
{
    QList< BaseType*> data;
    NzbMimeData *nzbMimeData = new NzbMimeData();
    QModelIndexList list = indexes;
    cleanSelection( list );

    foreach( const QModelIndex &index, list ){
        if( index.column() > 0 )
            continue;

        BaseType *base = static_cast< BaseType* >( index.internalPointer() );
        data.append( base );
    }

    nzbMimeData->setNzbData( data );
    return nzbMimeData;
}

QStringList KNewzModel::mimeTypes() const
{
    QStringList mimetypes;
    mimetypes << "text/x-nzb";
    return mimetypes;
}

void KNewzModel::moveToTop()
{
    QModelIndexList selection = view->selectionModel()->selectedIndexes();
    QList< File* > files = cleanSelection( selection );
    QMutexLocker lock( &downloadqueue->mutex() );
    NzbFile *root = downloadqueue->first();

    foreach( const QModelIndex &idx, selection ){
        BaseType *base = static_cast< BaseType* >( idx.internalPointer() );

        if( base->type() == "NzbFile" ){
            NzbFile *nzbFile = dynamic_cast< NzbFile* >( base );

            if( !nzbFile )
                continue;

            int row = downloadqueue->indexOf( nzbFile );

            if( row >= 0 && row < downloadqueue->size() ){
                QModelIndex idx = index( row, 0 );
                bool expanded = view->isExpanded( idx );
                beginRemoveRows( QModelIndex(), row, row );
                nzbFile = downloadqueue->takeAt( row );
                int rows = nzbFile->size();
                endRemoveRows();
                row = downloadqueue->indexOf( root );
                beginInsertRows( QModelIndex(), row, row );
                downloadqueue->insert( row, nzbFile );
                endInsertRows();
                QModelIndex newIdx = index( row, 0 );
                emit dataChanged( newIdx, index( rows, columnCount(), newIdx ) );

                if( files.size() > 0 ){
                    QModelIndex firstChild = index( 0, 0, newIdx );

                    if( firstChild.isValid() ){
                        /* Check if any children were selected. We can't just ignore them now,
                           because this action can legally operate on both children and their top-level
                           parent simultaneously */
                        QMutableListIterator< File* > it( files );

                        while( it.hasNext() ){
                            File *file = it.next();

                            if( file->parent() == nzbFile ){
                                it.remove();
                                int row = nzbFile->indexOf( file );

                                if( row >= 0 && row < nzbFile->size() ){
                                    beginRemoveRows( newIdx, row, row );
                                    file = nzbFile->takeAt( row );
                                    endRemoveRows();
                                    row = firstChild.row();
                                    beginInsertRows( newIdx, row, row );
                                    nzbFile->insert( row, file );
                                    endInsertRows();
                                    emit dataChanged( index( row, 0, newIdx ), index( row, columnCount(), newIdx ) );
                                }
                            }
                        }
                    }
                }

                if( expanded )
                    view->setExpanded( newIdx, true );
            }
        }else{
            File *file = dynamic_cast< File* >( base );
            NzbFile *nzbFile = file->parent();
            QModelIndex parent = index( downloadqueue->indexOf( nzbFile ), 0 );

            if( !parent.isValid() )
                break;

            QModelIndex firstChild = index( 0, 0, parent );

            if( !firstChild.isValid() )
                break;

            int row = nzbFile->indexOf( file );

            if( row >= 0 && row < nzbFile->size() ){
                beginRemoveRows( parent, row, row );
                file = nzbFile->takeAt( row );
                endRemoveRows();
                row = firstChild.row();
                beginInsertRows( parent, row, row );
                nzbFile->insert( row, file );
                endInsertRows();
                emit dataChanged( index( row, 0, parent ), index( row, columnCount(), parent ) );
            }
        }

    }

    view->selectionModel()->clearSelection();
}

void KNewzModel::moveUp()
{
    QModelIndexList selection = view->selectionModel()->selectedIndexes();
    QList< File* > files = cleanSelection( selection );
}

void KNewzModel::moveDown()
{
    QModelIndexList selection = view->selectionModel()->selectedIndexes();
    QList< File* > files = cleanSelection( selection );
}

void KNewzModel::moveToBottom()
{
    QModelIndexList selection = view->selectionModel()->selectedIndexes();
    QList< File* > files = cleanSelection( selection );
    QMutexLocker lock( &downloadqueue->mutex() );
    NzbFile *root = downloadqueue->last();

    foreach( const QModelIndex &idx, selection ){
        BaseType *base = static_cast< BaseType* >( idx.internalPointer() );

        if( base->type() == "NzbFile" ){
            NzbFile *nzbFile = dynamic_cast< NzbFile* >( base );

            if( !nzbFile )
                continue;

            int row = downloadqueue->indexOf( nzbFile );

            if( row >= 0 && row < downloadqueue->size() ){
                QModelIndex idx = index( row, 0 );
                bool expanded = view->isExpanded( idx );
                beginRemoveRows( QModelIndex(), row, row );
                nzbFile = downloadqueue->takeAt( row );
                int rows = nzbFile->size();
                endRemoveRows();
                row = downloadqueue->size();
                beginInsertRows( QModelIndex(), row, row );
                downloadqueue->append( nzbFile );
                endInsertRows();
                QModelIndex newIdx = index( row, 0 );
                emit dataChanged( newIdx, index( rows, columnCount(), newIdx ) );

                if( files.size() > 0 ){
                    QModelIndex firstChild = index( 0, 0, newIdx );

                    if( firstChild.isValid() ){
                        /* Check if any children were selected. We can't just ignore them now,
                        because this action can legally operate on both children and their top-level
                        parent simultaneously */
                        QMutableListIterator< File* > it( files );

                        while( it.hasNext() ){
                            File *file = it.next();

                            if( file->parent() == nzbFile ){
                                it.remove();
                                int row = nzbFile->indexOf( file );

                                if( row >= 0 && row < nzbFile->size() ){
                                    beginRemoveRows( newIdx, row, row );
                                    file = nzbFile->takeAt( row );
                                    endRemoveRows();
                                    row = firstChild.row();
                                    beginInsertRows( newIdx, row, row );
                                    nzbFile->insert( row, file );
                                    endInsertRows();
                                    emit dataChanged( index( row, 0, newIdx ), index( row, columnCount(), newIdx ) );
                                }
                            }
                        }
                    }
                }

                if( expanded )
                    view->setExpanded( newIdx, true );
            }

            root = nzbFile;
        }else{
            File *file = dynamic_cast< File* >( base );
            NzbFile *nzbFile = file->parent();
            QModelIndex parent = index( downloadqueue->indexOf( nzbFile ), 0 );

            if( !parent.isValid() )
                break;

            QModelIndex firstChild = index( 0, 0, parent );

            if( !firstChild.isValid() )
                break;

            int row = nzbFile->indexOf( file );

            if( row >= 0 && row < nzbFile->size() ){
                beginRemoveRows( parent, row, row );
                file = nzbFile->takeAt( row );
                endRemoveRows();
                row = firstChild.row();
                beginInsertRows( parent, row, row );
                nzbFile->insert( row, file );
                endInsertRows();
                emit dataChanged( index( row, 0, parent ), index( row, columnCount(), parent ) );
            }
        }

    }

    view->selectionModel()->clearSelection();
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
    if( row < 0 )
        return false;

    if( count < 1 )
        return false;

    int rows = row + count - 1;
    int beginRow = row;

    beginRemoveRows( parent, row, rows );

    if( parent.isValid() ){
        NzbFile *nzbFile = static_cast< NzbFile* >( parent.internalPointer() );

        while( beginRow <= rows ){
            nzbFile->removeAt( row );
            beginRow++;
        }

    }else{

        while( beginRow <= rows ){
            QMutableListIterator< File* > it( *(downloadqueue->at( row ) ) );

            while( it.hasNext() ){
                it.next();
                it.remove();
            }

            downloadqueue->removeAt( row );
            beginRow++;
        }

    }

    endRemoveRows();
    return true;
}

int KNewzModel::rowCount( const QModelIndex &parent ) const
{
    if( parent.column() > 0 )
        return 0;

    if ( !parent.isValid() )
        return downloadqueue->size();

    if( !parent.parent().isValid() )
        return downloadqueue->at( parent.row() )->size();

    return 0;
}

bool KNewzModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    if( !index.isValid() )
        return false;

    if( role != Qt::EditRole )
        return false;

    BaseType *base = static_cast<BaseType*>( index.internalPointer() );

    if( value.canConvert<NzbFile>() && ( base->type() == "NzbFile" ) ){
        NzbFile n_data = value.value<NzbFile>();
        /* Wacky syntax is due to downloadqueue being a pointer to a list
           of pointers to objects.
           QList<NzbFile*>::operator[](int) returns NzbFile* in this case.
           So (*downloadqueue) gives us an actual QList, then the []
           operator gives us a NzbFile*, from which we obtain the NzbFile
           which we need by using the * operand.
        */
        *(*downloadqueue)[index.row()] = n_data;
        emit dataChanged( index, this->index( 0, columnCount(), index ) );
        /* Note that since File is also a registered data type, all child rows have
           now been inserted and had their data set as well */
        return true;
    }else if( value.canConvert<File>() && ( base->type() == "File" ) ){
        File data = value.value<File>();
        QModelIndex parent = index.parent();
        /* Same story here as above, except we need to go one step further and
           get a File. I'm sure you can follow it by now :)
         */
        *(*(*downloadqueue)[parent.row()])[index.row()] = data;
        emit dataChanged( this->index( 0, 0, parent ), this->index( rowCount( parent ), columnCount( parent ), parent ) );
        return true;
    }

    return false;
}

Qt::DropActions KNewzModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

#include "knewzmodel.moc"
