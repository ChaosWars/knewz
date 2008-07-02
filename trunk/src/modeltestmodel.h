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
#ifndef _MODELTESTMODEL_H_
#define _MODELTESTMODEL_H_

#include <QAbstractItemModel>

class NzbFile;

class ModelTestModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
        ModelTestModel( QObject *parent );
        ~ModelTestModel();
        int columnCount( const QModelIndex &parent = QModelIndex() ) const;
        QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
        Qt::ItemFlags flags( const QModelIndex &index ) const;
        QVariant headerData( int section, Qt::Orientation orientation,
                             int role = Qt::DisplayRole ) const;
        QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
        bool insertRows( int row, int count, const QModelIndex &parent = QModelIndex() );
        QModelIndex parent( const QModelIndex &index ) const;
        int rowCount( const QModelIndex &parent = QModelIndex() ) const;
        bool setData(  const QModelIndex & index, const QVariant & value, int role = Qt::CheckStateRole );

    private:
        QStringList rootData;
        QList<NzbFile*> *m_data;
};

#endif
