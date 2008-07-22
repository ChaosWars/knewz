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

class KRecentFilesAction;
class QTreeView;
class KNewzModel;

/**
 */
class KNewz : public KXmlGuiWindow
{
    Q_OBJECT

    public:
        KNewz( QWidget *parent = 0 );
        virtual ~KNewz();

    protected:
        void openUrl( const KUrl& url );
//         void readProperties( const KConfigGroup &config );
//         void saveProperties( KConfigGroup &config );

    private Q_SLOTS:
//         void applyNewToolbarConfig();
//         void addRecentFile( const KUrl &url );
//         void optionsConfigureKeys();
//         void optionsConfigureSettings();
//         void optionsConfigureToolbars();
        void openRecentFile( const KUrl &url );
        void optionsPreferences();
        void settingsChanged();
        void urlOpen();

    private:
        KSharedConfigPtr config;
        KRecentFilesAction *recentFiles;
        QTreeView *view;
        KNewzModel *model;
//         void setupAccel();
        void setupActions();
};

#endif

