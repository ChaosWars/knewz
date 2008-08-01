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

bool KNewzModel::dropMimeData( const QMimeData *data, Qt::DropAction action, int /*row*/, int column, const QModelIndex &parent )
{
    if( column > 4 )
        return false;

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

            downloadqueue->mutex().lock();
            if( parent.isValid() ){

                int parentRow;
                if( parent.parent().isValid() ){
                    parentRow = parent.parent().row();
                }else{
                    parentRow = parent.row();
                }

                beginInsertRows( QModelIndex(), parentRow, parentRow + nzbFiles.size() - 1 );
                foreach( NzbFile *nzbFile, nzbFiles ){
                    downloadqueue->insert( parentRow++, nzbFile );
                }

            }else{
                int endRow = downloadqueue->size();
                beginInsertRows( parent, endRow, endRow + nzbFiles.size() - 1 );

                foreach( NzbFile *nzbFile, nzbFiles ){
                    downloadqueue->append( nzbFile );
                }

            }

            endInsertRows();
            downloadqueue->mutex().unlock();
        }

        return true;

    }

    if( data->hasFormat( "text/x-nzb" ) ){
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

bool KNewzModel::insertRows( int row, int count, const QModelIndex &parent )
{
    if( row < 0 )
        return false;

    if( count < 1 )
        return false;

    if( parent.parent().isValid() )
        return false;

//     kDebug() << "row:" << row;
//     kDebug() << "count:" << count;
//     kDebug() << "parent:" << parent;
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

// QMimeData* KNewzModel::mimeData(const QModelIndexList &indexes) const
// {
// }

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

    int rows = row + count - 1;
    int beginRow = row;

    if( rows < 1 )
        return false;

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
//     kDebug() << "index:" << index;
    if( !index.isValid() )
        return false;

    if( role != Qt::EditRole )
        return false;

    BaseType *base = static_cast<BaseType*>( index.internalPointer() );

    if( value.canConvert<NzbFile>() && ( base->type() == "NzbFile" ) ){
        NzbFile data = value.value<NzbFile>();
        *(*downloadqueue)[index.row()] = data;
//         QModelIndex parent = this->index( index.row(), 0 );
//         emit dataChanged( parent, this->index( index.row(), columnCount( index ) ) );
//         emit dataChanged( this->index( 0, 0, parent ), this->index( rowCount( parent ), columnCount( parent ), parent ) );
        return true;
    }else if( value.canConvert<File>() && ( base->type() == "File" ) ){
        File data = value.value<File>();
//         emit dataChanged(  );
        return true;
    }

    return false;
}

Qt::DropActions KNewzModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

void KNewzModel::clicked( const QModelIndex& index )
{
    if( index.column() > 0 )
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
