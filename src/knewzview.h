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
 * @class KNewzView knewzview.h
 */

#ifndef KNEWZVIEW_H
#define KNEWZVIEW_H

#include <QTreeView>

/**
 * @brief A subclassed QTreeView providing drag and drop support.
 *
 * This class implements custom a QMimeType to deal with internal dragging
 * and dropping of Files and NzbFiles.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class KNewzView : public QTreeView
{
    Q_OBJECT

    public:

        /**
         * Constructor
         * @param parent
         *      Parent widget.
         */
        KNewzView( QWidget *parent );
        ~KNewzView();

    protected:
        virtual void dragEnterEvent ( QDragEnterEvent * event );
        virtual void dragMoveEvent ( QDragMoveEvent * event );
        virtual void dropEvent ( QDropEvent * event );
//         virtual void mouseMoveEvent( QMouseEvent *event );
//         virtual void mousePressEvent( QMouseEvent *event );

//     private:
//         QPoint dragStartPosition;
};

#endif
