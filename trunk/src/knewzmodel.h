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
 * @class KNewzModel mainmodel.h
 */
#ifndef KNEWZMODEL_H
#define KNEWZMODEL_H

#include "basemodel.h"

class DownloadQueue;
class KNewzView;
class NzbFile;

/**
 * @brief Provides a model for the main view of the program
 *
 * This is the main model for the main view of the program. Here the information
 * of the current download queue is represented, giving an overview of the state
 * and progress of the loaded NZB files.
 * See the Qt documentation for QAbstractItemModel for documentation of the
 * implemented functions.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class KNewzModel : public BaseModel
{
    Q_OBJECT

    public:
        KNewzModel( KNewzView *parent );
        virtual ~KNewzModel(){};
        virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
        virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
        virtual bool dropMimeData( const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent );
        virtual Qt::ItemFlags flags( const QModelIndex &index ) const;
        virtual QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
        virtual bool insertRows( int row, int count, const QModelIndex &parent = QModelIndex() );

        /**
         * Insert rows into a top level item. This both inserts a number of rows and sets their data.
         * @param parent
         *      The parent to add the children to. The parent must be valid and must be in column 0, ie.
         *      parent.column() must return 0.
         * @param nzbFile
         *      The nzbFile which contains the data for the children.
         * @param row
         *      This parameter is the row at which to start inserting the children. If you call this function
         *      immediately after adding a top level item with that item as \p parent, then just leave the
         *      value of this variable as 0. The only time you will want to set this variable is when you are
         *      adding children to a top level item that already has children. In practice you will only set
         *      this variable during internal drag and drop operations of children.
         * @return
         *      \c false if \p parent was not valid or if parent.column() was larger than 0.
         */
        virtual bool insertChildren( const QModelIndex &parent, const NzbFile &nzbFile, int row = 0 );
        virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
        virtual QStringList mimeTypes() const;
        virtual QModelIndex parent( const QModelIndex &index ) const;
        virtual bool removeRows( int row, int count, const QModelIndex &parent = QModelIndex() );
        virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;
        virtual bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );
        virtual Qt::DropActions supportedDropActions() const;

    public Q_SLOTS:
        void moveToTop( QModelIndexList &selection );
        void moveUp( QModelIndexList &selection );
        void moveDown( QModelIndexList &selection );
        void moveToBottom( QModelIndexList &selection );

    private:
        KNewzView *m_parent;
        DownloadQueue *downloadqueue;
        void cleanSelection( QModelIndexList &selection ) const;
};

#endif
