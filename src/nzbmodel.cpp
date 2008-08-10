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
#include <QListIterator>
#include <QTreeView>
#include "file.h"
#include "nzbfile.h"
#include "nzbmodel.h"

NzbModel::NzbModel( QTreeView *parent, const QList<NzbFile*> &nzbFiles )
    : BaseModel( parent ),
      m_nzbFiles( nzbFiles )
{
    rootItem << "" << i18n( "Subject" ) << i18n( "Size (MiB)" );
}

int NzbModel::columnCount( const QModelIndex &/*parent*/ ) const
{
    return 3;
}

QVariant NzbModel::data( const QModelIndex &index, int role ) const
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
                return m_nzbFiles.at( index.row() )->filename();
                break;
            case 2:
                return QString().setNum( m_nzbFiles.at( index.row() )->bytes() /1048576.00, 'f', 2 );
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
                return m_nzbFiles.at( parentIndex.row() )->at( index.row() )->subject();
                break;
            case 2:
                return QString().setNum( m_nzbFiles.at( parentIndex.row() )->at( index.row() )->bytes() /1048576.00, 'f', 2 );
                break;
            default:
                return QVariant();
                break;
        }

    }

    return QVariant();
}

Qt::ItemFlags NzbModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags( index );

    if ( !index.isValid() )
        return 0;

    return defaultFlags;
}

QModelIndex NzbModel::index( int row, int column, const QModelIndex &parent ) const
{
    if( !hasIndex( row, column, parent ) )
        return QModelIndex();

    if( !parent.isValid() )
        return createIndex( row, column, m_nzbFiles.at( row ) );

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

QModelIndex NzbModel::parent( const QModelIndex &index ) const
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
    return createIndex( m_nzbFiles.indexOf( nzbFile ), 0, nzbFile );
}

int NzbModel::rowCount( const QModelIndex &parent ) const
{
    if( parent.column() > 0 )
        return 0;

    if ( !parent.isValid() )
        return m_nzbFiles.size();

    if( !parent.parent().isValid() )
        return m_nzbFiles.at( parent.row() )->size();

    return 0;
}

void NzbModel::trimNzbFiles()
{
    for( int i = 0, size = m_nzbFiles.size(); i < size; i++ ){
        NzbFile *currentNzbFile = m_nzbFiles.at( i );
        NzbFile *nzbFile;

        switch( currentNzbFile->state() ){
            case Qt::Checked:
                m_files.append( currentNzbFile );
                break;
            case Qt::PartiallyChecked:
                nzbFile = new NzbFile( currentNzbFile->filename() );

                for( int j = 0, size = currentNzbFile->size(); j < size; j++ ){
                    File *file = currentNzbFile->at( j );
                    if( file->state() == Qt::Checked ){
                        nzbFile->append( new File( *file ) );
                        nzbFile->last()->setParent( nzbFile );
                    }
                }

                m_files.append( nzbFile );
                break;
            default:
                break;
        }

    }
}

void NzbModel::checkAll()
{
    for( int file = 0, size = rowCount(); file < size; file++ ){
        QModelIndex fileIndex = index( file, 0 );
        NzbFile *nzbFile = static_cast< NzbFile* >( fileIndex.internalPointer() );
        nzbFile->setState( Qt::Checked );
        view->update( fileIndex );

        for( int part = 0, size = rowCount( fileIndex ); part < size; part++ ){
            nzbFile->at( part )->setState( Qt::Checked );
            view->update( index( part, 0, fileIndex ) );
        }
    }
}

void NzbModel::checkNone()
{
    for( int file = 0, size = rowCount(); file < size; file++ ){
        QModelIndex fileIndex = index( file, 0 );
        NzbFile *nzbFile = static_cast< NzbFile* >( fileIndex.internalPointer() );
        nzbFile->setState( Qt::Unchecked );
        view->update( fileIndex );

        for( int part = 0, size = rowCount( fileIndex ); part < size; part++ ){
            nzbFile->at( part )->setState( Qt::Unchecked );
            view->update( index( part, 0, fileIndex ) );
        }
    }
}

void NzbModel::checkSelected()
{
    QModelIndexList list = view->selectionModel()->selectedRows();

    foreach( QModelIndex idx, list ){
        changeCheckState( idx, Qt::Checked );
    }
}

void NzbModel::uncheckSelected()
{
    QModelIndexList list = view->selectionModel()->selectedRows();

    foreach( QModelIndex idx, list ){
        changeCheckState( idx, Qt::Unchecked );
    }
}

void NzbModel::invertSelection()
{
    for( int i = 0, size = rowCount(); i < size; i++ ){

        switch( m_nzbFiles.at( i )->state() ){
            case Qt::Unchecked:
                    changeCheckState( index( i, 0 ), Qt::Checked );
                break;
            case Qt::Checked:
                changeCheckState( index( i, 0 ), Qt::Unchecked );
                break;
            case Qt::PartiallyChecked:
                for( int j = 0, sizej = m_nzbFiles.at( i )->size(); j < sizej; j++ ){
                    changeCheckState( index( j, 0, index( i, 0 ) ), m_nzbFiles.at( i )->at( j )->state() == Qt::Checked ? Qt::Unchecked : Qt::Checked );
                }
                break;
            default:
                break;
        }
    }
}

void NzbModel::invertSelectedRows()
{
    QModelIndexList list = view->selectionModel()->selectedRows();

    /* We want to filter the index list here, since we cannot allow a selection to contain both
    root items and children of those root items. Here we traverse the list, filtering out any
    children who's parents are also in the list */
    foreach( QModelIndex idx, list ){
        BaseType *base = static_cast< BaseType* >( idx.internalPointer() );

        if( base->type() == "File" ){
            File *file = dynamic_cast< File* >( base );

            if( file ){
                /* If the current files parent is also in the list, then we don't want to process it.
                 */
                if( list.indexOf( index( m_nzbFiles.indexOf( file->parent() ), 0 ) ) != -1 ){
                    list.removeAt( list.indexOf( idx ) );
                }
            }
        }

    }

    foreach( QModelIndex idx, list ){
        BaseType *base = static_cast< BaseType* >( idx.internalPointer() );

        /* We know the children are all filtered out, so we traverse them all now and flip their checkstates */
        if( base->type() =="NzbFile" ){
            NzbFile *nzbFile = dynamic_cast< NzbFile* >( base );

            if( nzbFile ){
                for( int i = 0, size = nzbFile->size(); i < size; i++ ){
                    changeCheckState( index( i, 0, idx ), nzbFile->at( i )->state() == Qt::Checked ? Qt::Unchecked : Qt::Checked );
                }
            }
        }else{
            /* Dealing with selected files now */
            changeCheckState( idx, base->state() == Qt::Checked ? Qt::Unchecked : Qt::Checked );
        }
    }
}

#include "nzbmodel.moc"
