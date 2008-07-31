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
 * \class KNewz knewz.h
 *
 * \mainpage KNewz
 *      A program for downloading the contents of NZB files for KDE4.
 */
#ifndef KNEWZ_H
#define KNEWZ_H

#include <KDE/KXmlGuiWindow>
#include <KDE/KUrl>

// class KAction;
class KAction;
class KRecentFilesAction;
class KNewzModel;
class KSystemTrayIcon;
class QTreeView;
class DownloadQueue;
class ModelTest;
/**
 * \brief The main window of the program.
 *
 * Provides a main window for the program, using KXmlGuiWindow to set up and
 * track changes to the menubar and toolbar.
 *
 * \author Lawrence Lee <valheru.ashen.shugar@gmail.com
 */
class KNewz : public KXmlGuiWindow
{
    Q_OBJECT

    public:
        KNewz( QWidget *parent = 0 );
        virtual ~KNewz();

    protected:
        virtual bool queryClose();
        virtual bool queryExit();

    private Q_SLOTS:
        void openRecentFile( const KUrl &url );
        void optionsConfigure();
        void settingsChanged();
        void urlOpen();
        void exit();

    public Q_SLOTS:
        void showFileOpenDialog( const QStringList &files);

    private:
        QTreeView *view;
        KNewzModel *model;
        ModelTest *modeltest;
        DownloadQueue *downloadqueue;
        KSharedConfigPtr config;
        KSystemTrayIcon *trayIcon;
        KRecentFilesAction *recentFiles;
        KAction *openFiles, *preferences;
        bool ok_to_close;
        void setupActions();
};

#endif
