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
#ifndef KNEWZ_H
#define KNEWZ_H

#include <KDE/KXmlGuiWindow>
#include <KDE/KUrl>

class KAction;
class KRecentFilesAction;
class KToggleAction;
class QTreeView;

class KNewz : public KXmlGuiWindow
{
    Q_OBJECT

    public:
        KNewz( QWidget *parent = NULL );
        ~KNewz();

    protected:
        void openUrl( const KUrl& url );
        void readProperties( const KConfigGroup &config );
        void saveProperties( KConfigGroup &config );

    private Q_SLOTS:
        void applyNewToolbarConfig();
        void addRecentFile( const KUrl &url );
        void openRecentFile( const KUrl &url );
        void optionsConfigureKeys();
        void optionsConfigureSettings();
        void optionsConfigureToolbars();
        void urlOpen();

    private:
        KSharedConfigPtr config;
        KRecentFilesAction *recentFiles;
        KAction *configureAction;
        QTreeView *view;
        void setupAccel();
        void setupActions();
};

#endif

