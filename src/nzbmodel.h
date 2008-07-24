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
 * \class NzbModel nzbmodel.h
 */
#ifndef _MODELTESTMODEL_H_
#define _MODELTESTMODEL_H_

#include <QAbstractItemModel>
#include <QStringList>

class QTreeView;
class DownloadQueue;
class NzbFile;

/**
 * \brief The model for the dialog displaying the contents of the NZB file.
 *
 * See the Qt documentation for QAbstractItemModel for documentation of the
 * implemented functions.
 *
 * \author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class NzbModel : public QAbstractItemModel
{
    Q_OBJECT

    public:

        /**
         * Constructor
         *
         * @param parent
         *      The tree view parent of the model.
         *
         * @param nzbfiles
         *      The NZB files of which the content should be displayed.
         */
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

        /**
         * Slot for interacting with the clicked item.
         *
         * @param index
         *      The item that was clicked.
         */
        void clicked( const QModelIndex &index );

        void checkAll();

        void checkNone();

        void checkSelected();

        void uncheckSelected();

        void invertSelection();

    private:
        QTreeView *view;
        QStringList rootItem;
        QList< NzbFile* > m_nzbFiles;
};

#endif
