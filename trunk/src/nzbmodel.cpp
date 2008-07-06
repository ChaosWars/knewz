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

#include <QTreeView>
#include "nzbmodel.h"
#include "nzbfile.h"
#include "file.h"

NzbModel::NzbModel( QTreeView *parent, const QList<NzbFile*> &nzbfiles )
    : QAbstractItemModel( parent ), view( parent ), m_nzbFiles( nzbfiles )
{
    rootItem << "" << tr( "Subject" ) << tr( "Size (MiB)" );
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
    if ( !index.isValid() )
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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

void NzbModel::clicked( const QModelIndex& index )
{
    if( !( index.column() == 0 ) )
        return;

    BaseType *base = static_cast< BaseType* >( index.internalPointer() );
    Qt::CheckState checkstate;

    switch( base->state() ){
        case Qt::Checked:
            checkstate = Qt::Unchecked;
            break;
        case Qt::Unchecked:
        case Qt::PartiallyChecked:
        default:
            checkstate = Qt::Checked;
            break;
    }

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

        if( nzbFile->state() != Qt::PartiallyChecked ){
            nzbFile->setState( Qt::PartiallyChecked );
        }else{
            Qt::CheckState state = nzbFile->first()->state();

            for( int i = 1, size = nzbFile->size(); i < size; i++ ){
                if( nzbFile->at( i )->state() != state  )
                    break;
            }

            nzbFile->setState( state );
        }

        view->update( index.parent() );
    }

}

#include "nzbmodel.moc"
