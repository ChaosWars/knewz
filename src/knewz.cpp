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
#include <KDE/KFileDialog>
#include <KDE/KConfig>
#include <KDE/KUrl>
#include <KDE/KEditToolBar>
#include <KDE/KAction>
#include <KDE/KStandardAction>
#include <KDE/KRecentFilesAction>
#include <KDE/KActionCollection>
#include <KDE/KPluginLoader>
#include <KDE/KPluginFactory>
#include <KDE/KMessageBox>
#include <KDE/KStatusBar>
#include <KDE/KLocale>
#include <KDE/KGlobal>
#include <QTreeView>
#include "knewz.h"
#include "nzbreader.h"
#include "downloadqueue.h"

KNewz::KNewz( QWidget *parent ) : KXmlGuiWindow( parent )
{
    view = new QTreeView( this );
    setCentralWidget( view );
    setupActions();
    setupAccel();
    config = KGlobal::config();
    createGUI( "knewz2/knewz2ui.rc" );
    setAutoSaveSettings();
}

KNewz::~KNewz()
{
    //Save the recent files entries
    KConfigGroup configGroup( config, "RecentFiles" );
    recentFiles->saveEntries( configGroup );
}

void KNewz::applyNewToolbarConfig()
{
    applyMainWindowSettings( KConfigGroup( KGlobal::config(), autoSaveGroup() ) );
}

void KNewz::addRecentFile( const KUrl &url )
{
    recentFiles->addUrl( url );
}

void KNewz::openRecentFile( const KUrl &url )
{
    openUrl( url );
}

void KNewz::openUrl( const KUrl& url )
{
    addRecentFile( url );
    NzbReader reader;
    DownloadQueue::append( reader.parseData( url.url() ) );
}

void KNewz::optionsConfigureKeys()
{
}

void KNewz::optionsConfigureSettings()
{
//     if( KConfigDialog::showDialog( "settings" ) )
//         return;
    //
//     settings = new Settings( m_widget, "settings", config );
//     connect( settings, SIGNAL( settingsChanged() ), this, SLOT( readSettings() ) );
//     settings->show();

}

void KNewz::optionsConfigureToolbars()
{
    saveMainWindowSettings( KConfigGroup( KGlobal::config(), autoSaveGroup() ) );

    // use the standard toolbar editor
    KEditToolBar dlg(factory());
    connect( &dlg, SIGNAL( newToolbarConfig() ), this, SLOT( applyNewToolbarConfig() ) );
    dlg.exec();
}

void KNewz::readProperties( const KConfigGroup &config )
{
}

void KNewz::saveProperties( KConfigGroup &config )
{
}

void KNewz::setupAccel()
{
}

void KNewz::setupActions()
{
    setStandardToolBarMenuEnabled( true );
    createStandardStatusBarAction();
    KStandardAction::open( this, SLOT( urlOpen() ), actionCollection() );
    KStandardAction::quit( this, SLOT( close() ), actionCollection() );
    KStandardAction::keyBindings( this, SLOT( optionsConfigureKeys() ), actionCollection() );
    KStandardAction::configureToolbars( this, SLOT( optionsConfigureToolbars() ), actionCollection() );
    recentFiles = KStandardAction::openRecent( this, SLOT( openRecentFile( const KUrl& ) ), actionCollection() );

    configureAction = new KAction( KIcon( "configure" ), i18n( "&Configure KNewz..." ), actionCollection() );
    actionCollection()->addAction( "options_configure", configureAction );
    connect( configureAction, SIGNAL( triggered( bool ) ), this, SLOT( optionsConfigureSettings() ) );
}

void KNewz::urlOpen()
{
    QStringList files =  KFileDialog::getOpenFileNames( KUrl(), "*.nzb *.NZB |NZB Files", this, "Open NZB File" );

    if( files.size() > 0 ){
        NzbReader reader;

        for( int i = 0; i < files.size(); i++ ){
            openUrl( files.at( i ) );
        }

    }
}

#include "knewz.moc"
