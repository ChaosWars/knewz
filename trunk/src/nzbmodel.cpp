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
#include <QMutableListIterator>
#include <QTreeView>
#include "file.h"
#include "nzbfile.h"
#include "nzbmodel.h"

NzbModel::NzbModel( QTreeView *parent, const QList<NzbFile*> &nzbfiles )
    : QAbstractItemModel( parent ),
      view( parent ),
      m_nzbFiles( nzbfiles )
{
    rootItem << "" << i18n( "Subject" ) << i18n( "Size (MiB)" );
    connect( parent, SIGNAL( clicked( const QModelIndex& ) ), this, SLOT( clicked( const QModelIndex& ) ) );
}

NzbModel::~NzbModel()
{
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

QVariant NzbModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        return rootItem.at( section );

    return QVariant();
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
    if( parent.column() > 1 )
        return 0;

    if ( !parent.isValid() )
        return m_nzbFiles.size();

    if( !parent.parent().isValid() )
        return m_nzbFiles.at( parent.row() )->size();

    return 0;
}

void NzbModel::trimNzbFiles()
{
    QMutableListIterator< NzbFile* > it( m_nzbFiles );
    while( it.hasNext() ){

        NzbFile *nzbFile = it.next();
        if( nzbFile->state() != Qt::Checked ){

            QMutableListIterator< File* > its( *nzbFile );
            while( its.hasNext() ){

                File *file = its.next();
                if( file->state() == Qt::Unchecked ){
                    its.remove();
                }
            }

        }

        if( nzbFile->state() == Qt::Unchecked ){
            it.remove();
        }else if( nzbFile->state() == Qt::PartiallyChecked ){
            nzbFile->setState( Qt::Checked );
        }

    }
}

void NzbModel::clicked( const QModelIndex& index )
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
    QModelIndexList selection = view->selectionModel()->selectedRows();

    for( int selected = 0, size = selection.size(); selected < size; selected++ ){
        QModelIndex idx = selection.at( selected );
        BaseType *base = static_cast< BaseType* >( idx.internalPointer() );
        base->setState( Qt::Checked );
        view->update( idx );

        if( base->type() == "NzbFile" ){
            NzbFile *nzbFile = static_cast< NzbFile* >( idx.internalPointer() );

            for( int i = 0, size = nzbFile->size(); i < size; i++ ){
                nzbFile->at( i )->setState( Qt::Checked );
                view->update( index( i, 0, idx ) );
            }
        }
    }
}

void NzbModel::uncheckSelected()
{
    QModelIndexList selection = view->selectionModel()->selectedRows();

    for( int selected = 0, size = selection.size(); selected < size; selected++ ){
        QModelIndex idx = selection.at( selected );
        BaseType *base = static_cast< BaseType* >( idx.internalPointer() );
        base->setState( Qt::Unchecked );
        view->update( idx );

        if( base->type() == "NzbFile" ){
            NzbFile *nzbFile = static_cast< NzbFile* >( idx.internalPointer() );

            for( int i = 0, size = nzbFile->size(); i < size; i++ ){
                nzbFile->at( i )->setState( Qt::Unchecked );
                view->update( index( i, 0, idx ) );
            }
        }
    }
}

void NzbModel::invertSelection()
{
}

#include "nzbmodel.moc"
