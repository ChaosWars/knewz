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
/**
 * \class NzbModel nzbmodel.h
 */

#ifndef _MODELTESTMODEL_H_
#define _MODELTESTMODEL_H_

#include <QAbstractItemModel>
#include <QStringList>

class QTreeView;
class NzbFile;

/**
 * \brief
 *
 * \author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class NzbModel : public QAbstractItemModel
{
    Q_OBJECT

    public:

        /**
         * 
         * @param parent 
         * @param nzbfiles 
         */
        NzbModel( QTreeView *parent, const QList<NzbFile*> &nzbfiles );
        ~NzbModel();

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

        /**
         * 
         * @param index 
         * @return 
         */
        QModelIndex parent( const QModelIndex &index ) const;

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
        QList< NzbFile* > m_nzbFiles;
};

#endif
