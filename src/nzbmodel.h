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
#include <QStringList>

class QTreeView;
class NzbFile;

class NzbModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
        NzbModel( QTreeView *parent, const QList<NzbFile*> &nzbfiles );
        ~NzbModel();
        int columnCount( const QModelIndex &parent = QModelIndex() ) const;
        QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
        Qt::ItemFlags flags( const QModelIndex &index ) const;
        QVariant headerData( int section, Qt::Orientation orientation,
                             int role = Qt::DisplayRole ) const;
        QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
        QModelIndex parent( const QModelIndex &index ) const;
        int rowCount( const QModelIndex &parent = QModelIndex() ) const;

    public Q_SLOTS:
        void clicked( const QModelIndex &index );

    private:
        QTreeView *view;
        QStringList rootItem;
        QList< NzbFile* > m_nzbFiles;
};

#endif
