/***************************************************************************
 *   Copyright (C) 2007 by Lawrence Lee   *
 *   valheru@facticius.net   *
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
#include "modeltestmodel.h"
#include "downloadqueue.h"

ModelTestModel::ModelTestModel( QObject *parent )
    : QAbstractItemModel( parent )
{
    m_data = DownloadQueue::queue();
    rootData << "Subject" << "Size";
}

ModelTestModel::~ModelTestModel()
{
}

int ModelTestModel::columnCount( const QModelIndex &parent ) const
{
    return 2;
}

QVariant ModelTestModel::data( const QModelIndex &index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();

    if( role == Qt::CheckStateRole && index.column() == 0 ){
        NzbFile *file = static_cast<NzbFile*>( index.internalPointer() );

    }

    if ( role != Qt::DisplayRole )
        return QVariant();

    ModelTestModelItem *item = static_cast<ModelTestModelItem*>( index.internalPointer() );
    return item->data( index.column() );
}

Qt::ItemFlags ModelTestModel::flags( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

QVariant ModelTestModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        return rootData.at( section );

    return QVariant();
}

QModelIndex ModelTestModel::index( int row, int column, const QModelIndex &parent ) const
{
    ModelTestModelItem *parentItem;

    if ( !parent.isValid() )
        parentItem = rootItem;
    else
        parentItem = static_cast<ModelTestModelItem*>( parent.internalPointer() );

    ModelTestModelItem *childItem = parentItem->child( row );
    if ( childItem )
        return createIndex( row, column, childItem );
    else
        return QModelIndex();
}

bool ModelTestModel::insertRows( int row, int count, const QModelIndex &parent )
{
    emit beginInsertRows( parent, row, count );
    QStringList nzbfileData;
    nzbfileData << "NZB File" << "Size";

    for( int i = 0; i < count; i++ ){
        rootItem->appendChild( new ModelTestModelItem( nzbfileData, Qt::Checked, rootItem ) );
    }

    emit endInsertRows();
}

QModelIndex ModelTestModel::parent( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return QModelIndex();

    ModelTestModelItem *childItem = static_cast<ModelTestModelItem*>( index.internalPointer() );
    ModelTestModelItem *parentItem = childItem->parent();

    if ( parentItem == rootItem )
        return QModelIndex();

    return createIndex( parentItem->row(), 0, parentItem );
}

int ModelTestModel::rowCount( const QModelIndex &parent ) const
{
    ModelTestModelItem * parentItem;

    if ( !parent.isValid() )
        parentItem = rootItem;
    else
        parentItem = static_cast<ModelTestModelItem*>( parent.internalPointer() );

    return parentItem->childCount();
}

bool ModelTestModel::setData( const QModelIndex &index, const QVariant& value, int role )
{
    if( !index.isValid() )
        return false;

    ModelTestModelItem *item = static_cast<ModelTestModelItem*>( index.internalPointer() );

    if( item->checkState() == Qt::Checked )
        item->setCheckState( Qt::Unchecked );
    else
        item->setCheckState( Qt::Checked );

    emit dataChanged( index, index );
    return true;
}

#include "modeltestmodel.moc"
