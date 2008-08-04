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
 * @class NzbModel nzbmodel.h
 */
#ifndef _MODELTESTMODEL_H_
#define _MODELTESTMODEL_H_

#include "basemodel.h"

class BaseType;
class NzbFile;

/**
 * @brief The model for the dialog displaying the contents of the NZB file.
 *
 * See the Qt documentation for QAbstractItemModel for documentation of the
 * implemented functions.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class NzbModel : public BaseModel
{
    Q_OBJECT

    public:

        /**
         * Constructor
         * @param parent
         *      The tree view parent of the model.
         *
         * @param nzbFiles
         *      The NZB files of which the content should be displayed.
         */
        NzbModel( QTreeView *parent, const QList<NzbFile*> &nzbFiles );
        virtual ~NzbModel(){};
        virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;
        virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
        virtual Qt::ItemFlags flags( const QModelIndex &index ) const;
        virtual QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
        virtual QModelIndex parent( const QModelIndex &index ) const;
        virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;

        /**
         * 
         */
        void trimNzbFiles();

    public Q_SLOTS:

        /**
         * Check all items.
         */
        void checkAll();

        /**
         * Uncheck all items.
         */
        void checkNone();

        /**
         * Check the selected items.
         */
        void checkSelected();

        /**
         * Uncheck the selected items.
         */
        void uncheckSelected();

        /**
         * Invert the checked state of the selected items. If the selected
         * item is a NZB file, then the checked status of the multiparts
         * is inverted.
         */
        void invertSelection();

        /**
         * Obtains the selected files in the model.
         * @return
         *      The selected files in the model.
         */
        const QList< NzbFile* >& files(){ return m_files; }

    private:
        QList< NzbFile* > m_nzbFiles;
        QList< NzbFile* > m_files;
};

#endif
