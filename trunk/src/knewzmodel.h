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

/**
 * \class KNewzModel mainmodel.h
 */
#ifndef KNEWZMODEL_H
#define KNEWZMODEL_H

#include <QAbstractItemModel>
#include <QStringList>

class QTreeView;
class DownloadQueue;
class NzbFile;

/**
 * \brief Provides a model for the main view of the program
 *
 * This is the main model for the main view of the program. Here the information
 * of the current download queue is represented, giving an overview of the state
 * and progress of the loaded NZB files.
 * See the Qt documentation for QAbstractItemModel for documentation of the
 * implemented functions.
 *
 *  \author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class KNewzModel : public QAbstractItemModel
{
    Q_OBJECT

    public:

        /**
         * Constructor
         *
         * @param parent
         *      The tree view parent of the model.
         */
        KNewzModel( QTreeView *parent );
        ~KNewzModel();

        /**
         * Called when information has been added to or removed from the model.
         * Emits the layoutChanged() signal.
         */
        void changed();
        virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
        virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
        virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent );
        virtual Qt::ItemFlags flags( const QModelIndex &index ) const;
        virtual QVariant headerData( int section, Qt::Orientation orientation,
                             int role = Qt::DisplayRole ) const;
        virtual QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
//         QMap<int, QVariant> itemData( const QModelIndex &index ) const;
        virtual bool insertRows( int row, int count, const QModelIndex &parent = QModelIndex() );
//         virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
        virtual QStringList mimeTypes() const;
        virtual QModelIndex parent( const QModelIndex &index ) const;
        virtual bool removeRows( int row, int count, const QModelIndex &parent = QModelIndex() );
        virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;
        virtual bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );
        virtual Qt::DropActions supportedDropActions() const;

    public Q_SLOTS:
        void clicked( const QModelIndex &index );

    private:
        QTreeView *view;
        DownloadQueue *downloadqueue;
        QStringList rootItem;
};

#endif
