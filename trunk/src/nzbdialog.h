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
  * \class NzbDialog nzbdialog.h
  */

#ifndef NZBDIALOG_H
#define NZBDIALOG_H

#include <QDialog>

class NzbFile;
class NzbModel;
class QTreeView;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QButtonGroup;

/**
 * \brief Dialog for displaying the contents of an NZB file.
 *
 * \author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class NzbDialog : public QDialog
{
    Q_OBJECT

    public:

        /**
         * 
         * @param parent 
         * @param nzbfiles 
         */
        NzbDialog( QWidget *parent, const QList<NzbFile*> &nzbfiles );
        ~NzbDialog();

    private:
        NzbModel *model;
        QTreeView *view;
        QVBoxLayout *layout;
        QHBoxLayout *buttonLayout;
        QButtonGroup *defaultButtonGroup;
        QButtonGroup *selectButtonGroup;
        QPushButton *ok;
        QPushButton *cancel;
        QPushButton *checkSelected;
        QPushButton *uncheckSelected;
        QPushButton *checkAll;
        QPushButton *checkNone;
        QPushButton *invertSelection;
};

#endif
