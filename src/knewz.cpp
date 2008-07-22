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

#include <KDE/KAction>
#include <KDE/KActionCollection>
#include <KDE/KConfig>
#include <KDE/KConfigDialog>
#include <KDE/KEditToolBar>
#include <KDE/KFileDialog>
#include <KDE/KGlobal>
#include <KDE/KLocale>
#include <KDE/KRecentFilesAction>
#include <KDE/KStandardAction>
#include <KDE/KStatusBar>
#include <KDE/KUrl>
#include <QTreeView>
#include "downloadqueue.h"
#include "file.h"
#include "knewz.h"
#include "knewzmodel.h"
#include "nzbdialog.h"
#include "nzbreader.h"
#include "segment.h"
#include "serverwidget.h"
#include "settings.h"

KNewz::KNewz( QWidget *parent )
    : KXmlGuiWindow( parent ), view( new QTreeView( this ) ), model( new KNewzModel( view ) )
{
    setAcceptDrops(true);
    view->setModel( model );
    setCentralWidget( view );
    setupActions();
    statusBar()->show();
    setupGUI();
    config = KGlobal::config();
    KConfigGroup configGroup( config, "RecentFiles" );
    recentFiles->loadEntries( configGroup );
//     setupAccel();
    setAutoSaveSettings();
}

KNewz::~KNewz()
{
    //Save the recent files entries
    KConfigGroup configGroup( config, "RecentFiles" );
    recentFiles->saveEntries( configGroup );
    delete model;
    delete view;
}

// void KNewz::applyNewToolbarConfig()
// {
//     applyMainWindowSettings( KConfigGroup( KGlobal::config(), autoSaveGroup() ) );
// }

// void KNewz::addRecentFile( const KUrl &url )
// {
//     recentFiles->addUrl( url );
// }

void KNewz::openRecentFile( const KUrl &url )
{
    NzbReader reader;
    QList<NzbFile*> nzbFile;
    nzbFile.append( reader.parseData( url.path() ) );
    NzbDialog *nzbDialog = new NzbDialog( this, nzbFile );
    nzbDialog->exec();

    if( nzbDialog->result() == QDialog::Accepted ){
        DownloadQueue::append( nzbFile );
        model->changed();
    }
}

void KNewz::openUrl( const KUrl& url )
{
    recentFiles->addUrl( KUrl( url ) );
    NzbReader reader;
    QList<NzbFile*> nzbFile;
    nzbFile.append( reader.parseData( url.url() ) );
    DownloadQueue::append( nzbFile );
    model->changed();
}

void KNewz::optionsPreferences()
{
    if ( KConfigDialog::showDialog( "settings" ) )  {
        return;
    }

    KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());
    QWidget *generalSettingsDlg = new QWidget;
    new ServerWidget( generalSettingsDlg );
    dialog->addPage(generalSettingsDlg, i18n( "Server" ), "preferences-system-network");
    connect(dialog, SIGNAL(settingsChanged(QString)), this, SLOT(settingsChanged()));
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->show();
}

// void KNewz::optionsConfigureKeys()
// {
// }

// void KNewz::optionsConfigureSettings()
// {
//     if( KConfigDialog::showDialog( "settings" ) )
//         return;
    //
//     settings = new Settings( m_widget, "settings", config );
//     connect( settings, SIGNAL( settingsChanged() ), this, SLOT( readSettings() ) );
//     settings->show();

// }

// void KNewz::optionsConfigureToolbars()
// {
//     saveMainWindowSettings( KConfigGroup( KGlobal::config(), autoSaveGroup() ) );
// 
//     // use the standard toolbar editor
//     KEditToolBar dlg(factory());
//     connect( &dlg, SIGNAL( newToolbarConfig() ), this, SLOT( applyNewToolbarConfig() ) );
//     dlg.exec();
// }

// void KNewz::readProperties( const KConfigGroup &config )
// {
// }
// 
// void KNewz::saveProperties( KConfigGroup &config )
// {
// }
// 
// void KNewz::setupAccel()
// {
// }

void KNewz::settingsChanged()
{
    kDebug() << "Settings changed";
}

void KNewz::setupActions()
{
//     setStandardToolBarMenuEnabled( true );
//     createStandardStatusBarAction();
//     KStandardAction::open( this, SLOT( urlOpen() ), actionCollection() );
//     KStandardAction::quit( this, SLOT( close() ), actionCollection() );
//     KStandardAction::keyBindings( this, SLOT( optionsConfigureKeys() ), actionCollection() );
//     KStandardAction::configureToolbars( this, SLOT( optionsConfigureToolbars() ), actionCollection() );
//     recentFiles = KStandardAction::openRecent( this, SLOT( openRecentFile( const KUrl& ) ), actionCollection() );
// 
//     configureAction = new KAction( KIcon( "configure" ), i18n( "&Configure KNewz..." ), actionCollection() );
//     actionCollection()->addAction( "options_configure", configureAction );
//     connect( configureAction, SIGNAL( triggered( bool ) ), this, SLOT( optionsConfigureSettings() ) );

    KStandardAction::open( this, SLOT( urlOpen() ), actionCollection() );
    recentFiles = KStandardAction::openRecent( this, SLOT( openRecentFile( KUrl ) ), actionCollection() );
    KStandardAction::quit( qApp, SLOT( closeAllWindows() ), actionCollection() );
    KStandardAction::preferences( this, SLOT( optionsPreferences() ), actionCollection() );
}

void KNewz::urlOpen()
{
    QStringList files =  KFileDialog::getOpenFileNames( KUrl(), "*.nzb *.NZB |NZB Files", this, "Open NZB File" );

    if( files.size() > 0 ){
        NzbReader reader;
        QList<NzbFile*> nzbFiles;

        for( int i = 0, size = files.size(); i < size; i++ ){
            QString file( files.at( i ) );
            recentFiles->addUrl( KUrl( file ) );
            nzbFiles.append( reader.parseData( file ) );
        }

        NzbDialog *nzbDialog = new NzbDialog( this, nzbFiles );
        nzbDialog->exec();

        if( nzbDialog->result() == QDialog::Accepted ){
            DownloadQueue::append( nzbFiles );
            model->changed();
        }

    }
}

#include "knewz.moc"
