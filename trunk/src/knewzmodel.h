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
class NzbFile;

/**
 * \brief Provides a model for the main view of the program
 *
 * This is the main model for the main view of the program. Here the information
 * of the current download queue is represented, giving an overview of the state
 * and progress of the loaded NZB files.
 *
 *  \author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class KNewzModel : public QAbstractItemModel
{
    Q_OBJECT

    public:

        KNewzModel( QTreeView *parent );
        ~KNewzModel();

        /**
         * 
         */
        void changed();

        /**
         * 
         * @param parent 
         * @return 
         */
        int columnCount( const QModelIndex &parent = QModelIndex() ) const;

        /**
         * 
         * @param index 
         * @param role 
         * @return 
         */
        QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

        /**
         * 
         * @param index 
         * @return 
         */
        Qt::ItemFlags flags( const QModelIndex &index ) const;

        /**
         * 
         * @param section 
         * @param orientation 
         * @param role 
         * @return 
         */
        QVariant headerData( int section, Qt::Orientation orientation,
                             int role = Qt::DisplayRole ) const;

        /**
         * 
         * @param row 
         * @param column 
         * @param parent 
         * @return 
         */
        QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;

//         bool insertRows( int row, int count, const QModelIndex &parent = QModelIndex() );

        /**
         * 
         * @param index 
         * @return 
         */
        QModelIndex parent( const QModelIndex &index ) const;

//         bool removeRows( int row, int count, const QModelIndex &parent = QModelIndex() );

        /**
         * 
         * @param parent 
         * @return 
         */
        int rowCount( const QModelIndex &parent = QModelIndex() ) const;

    public Q_SLOTS:

        /**
         * 
         * @param index 
         */
        void clicked( const QModelIndex &index );

    private:
        QTreeView *view;
        QStringList rootItem;
};

#endif
