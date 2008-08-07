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
 * @class NzbDialog nzbdialog.h
 */
#ifndef NZBDIALOG_H
#define NZBDIALOG_H

#include <KDE/KConfig>
#include <QDialog>
#include "ui_nzbdialog.h"
#include "nzbmodel.h"

// class KConfigGroup;
// class QButtonGroup;
// class QHBoxLayout;
// class QPushButton;
// class QTreeView;
// class QVBoxLayout;
// class ModelTest;
// class NzbFile;

/**
 * @brief Dialog for displaying the contents of an NZB file.
 *
 * Displays the contents of multiple NZB files, providing the user with a
 * way to interact with the contents of the files in a manner that enables
 * him to choose which of the contents he wishes to download.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class NzbDialog : public QDialog, public Ui::NzbDialog
{
    Q_OBJECT

    public:

        /**
         * Constructor
         * @param parent
         *      Parent widget.
         *
         * @param nzbfiles
         *      List of nzb files that the user wants to open.
         */
        NzbDialog( QWidget *parent, const QList<NzbFile*> &nzbfiles );
        ~NzbDialog();

        /**
         * Returns the selected files in the model.
         * @return
         *      The currently selected files in the model.
         */
        const QList< NzbFile* >& files(){ return model->files(); }

    private:
        KSharedConfigPtr config;
        KConfigGroup *configGroup;
//         QTreeView *view;
        NzbModel *model;
        ModelTest *modeltest;
//         QVBoxLayout *layout;
//         QHBoxLayout *buttonLayout;
//         QButtonGroup *defaultButtonGroup;
//         QButtonGroup *selectButtonGroup;
//         QPushButton *checkAll;
//         QPushButton *checkNone;
//         QPushButton *checkSelected;
//         QPushButton *uncheckSelected;
//         QPushButton *invertSelection;
//         QPushButton *ok;
//         QPushButton *cancel;

    private Q_SLOTS:
        void okSlot();
};

#endif
