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
 * @class BaseModel basemodel.h
 */
#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <QAbstractItemModel>
#include <QStringList>

class QTreeView;
class BaseType;
class DownloadQueue;
class File;

/**
 * @brief Base class for the internal models used by the program
 *
 * This is an abstract class, and cannot be instantiated by itself.
 * See Qt documentation for QAbstractItemModel for it's member documentation.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class BaseModel : public QAbstractItemModel
{
    Q_OBJECT

    public:

        BaseModel( QTreeView *parent );
        virtual ~BaseModel() = 0;
        virtual QList< File* > cleanSelection( QModelIndexList &selection ) const;
        virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    protected:
        QStringList rootItem;
        QTreeView *view;
        virtual void changeCheckState( const QModelIndex &index, Qt::CheckState state, BaseType *base = NULL );

    protected Q_SLOTS:

        /**
         * Slot for interacting with the clicked item.
         *
         * @param index
         *      The item that was clicked.
         */
        virtual void clicked( const QModelIndex &index );

    private:
        DownloadQueue *downloadqueue;

};

#endif
