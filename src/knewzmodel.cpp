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
                    setData( idx, QVariant::fromValue( *nzbFile ) );
                    m_row++;
                }

            }else{
                /* Top level drop */
                int m_row = rowCount();
                insertRows( m_row, nzbFiles.size() );

                foreach( NzbFile *nzbFile, nzbFiles ){
                    QModelIndex idx = index( m_row, 0 );
                    setData( idx, QVariant::fromValue( *nzbFile ) );
                    m_row++;
                }

            }
        }

        return true;

    }

    if( data->hasFormat( "text/x-nzb" ) ){
        const NzbMimeData *nzbMimeData = dynamic_cast< const NzbMimeData* >( data );

        if( !nzbMimeData )
            return false;

        for( int i = 0, size = nzbMimeData->getNzbData().size(); i < size; i++  ){

//             if( nzbMimeData->getNzbData().at( i )->type() == "NzbFile" ){
//                 NzbFile *nzbFile = dynamic_cast< NzbFile* >( nzbMimeData->getNzbData().at( i ) );
//                 removeRows( downloadqueue->indexOf( nzbFile ), 1 );
//                 downloadqueue->dumpQueueTopLevel();
//             }
        }

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

// QMap<int, QVariant> KNewzModel::itemData( const QModelIndex &index ) const
// {
//     QMap<int, QVariant> roles = QAbstractItemModel::itemData( index );
//     kDebug() << roles;
//     BaseType *base = static_cast<BaseType*>( index.internalPointer() );
//     kDebug() << base;
//
//     if( base->type() == "NzbFile" ){
//         NzbFile *nzbFile = static_cast< NzbFile* >( index.internalPointer() );
//         kDebug() << "NzbFile:" << nzbFile;
//         roles.insert( Qt::UserRole, QVariant::fromValue( *nzbFile ) );
//         kDebug() << "roles:" << roles;
//     }else if( base->type() == "File" ){
//         File *file = static_cast< File* >( index.internalPointer() );
//         kDebug() << "file:" << file;
//         roles.insert( Qt::UserRole, QVariant::fromValue( *file ) );
//         kDebug() << "roles:" << roles;
//     }
//     return roles;
// }

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

    /* We want to filter the index list here, since we cannot allow a selection to contain both
       root items and children of those root items. Here we traverse the list, filtering out any
       children who's parents are also in the list */
    foreach( QModelIndex index, list ){
        BaseType *base = static_cast< BaseType* >( index.internalPointer() );

        if( base->type() == "File" ){
            File *file = dynamic_cast< File* >( base );

            if( file ){
                //If the current files parent is also in the list, then we don't want to process it
                if( list.indexOf( this->index( downloadqueue->indexOf( file->parent() ), 0 ) ) != -1 ){
                    list.removeAt( list.indexOf( index ) );
                }
            }
        }

    }

    foreach( QModelIndex index, list ){
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

        downloadqueue->mutex().lock();
        while( beginRow <= rows ){
            nzbFile->removeAt( row );
            beginRow++;
        }

        downloadqueue->mutex().unlock();
    }else{

        downloadqueue->mutex().lock();
        while( beginRow <= rows ){
            QMutableListIterator< File* > it( *(downloadqueue->at( row ) ) );

            while( it.hasNext() ){
                it.next();
                it.remove();
            }

            downloadqueue->removeAt( row );
            beginRow++;
        }

        downloadqueue->mutex().unlock();
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
