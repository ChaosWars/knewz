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

#include <KDE/KActionCollection>
#include <KDE/KConfig>
#include <KDE/KConfigDialog>
#include <KDE/KFileDialog>
#include <KDE/KGlobal>
#include <KDE/KLocale>
#include <KDE/KRecentFilesAction>
#include <KDE/KStandardAction>
#include <QTreeView>
#include "downloadqueue.h"
#include "file.h"
#include "knewz.h"
#include "knewzexception.h"
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
    setupGUI();
    setAutoSaveSettings();
    config = KGlobal::config();
    KConfigGroup configGroup( config, "RecentFiles" );
    recentFiles->loadEntries( configGroup );
}

KNewz::~KNewz()
{
    //Save the recent files entries
    KConfigGroup configGroup( config, "RecentFiles" );
    recentFiles->saveEntries( configGroup );
    delete model;
    delete view;
}

void KNewz::openRecentFile( const KUrl &url )
{
    showFileOpenDialog( url.path(), false );
}

void KNewz::openUrl( const KUrl& url )
{
    NzbReader reader;
    QList<NzbFile*> nzbFiles;
    NzbFile *nzbFile = reader.parseData( url.url() );

    if( nzbFile->size() > 0 ){
        recentFiles->addUrl( KUrl( url ) );
        nzbFiles.append( nzbFile );
        DownloadQueue::append( nzbFiles );
        model->changed();
    }else{
        delete nzbFile;
    }
}

void KNewz::optionsPreferences()
{
    if ( KConfigDialog::showDialog( "settings" ) )  {
        return;
    }

    KConfigDialog *dialog = new KConfigDialog( this, "settings", Settings::self() );
    QWidget *generalSettingsDlg = new QWidget();
    new ServerWidget( generalSettingsDlg );
    dialog->addPage( generalSettingsDlg, i18n( "Server" ), "preferences-system-network" );
    connect( dialog, SIGNAL( settingsChanged( QString ) ), this, SLOT( settingsChanged() ) );
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->show();
}

void KNewz::settingsChanged()
{
}

void KNewz::setupActions()
{
    setStandardToolBarMenuEnabled( true );
    createStandardStatusBarAction();
    KStandardAction::open( this, SLOT( urlOpen() ), actionCollection() );
    recentFiles = KStandardAction::openRecent( this, SLOT( openRecentFile( KUrl ) ), actionCollection() );
    KStandardAction::quit( qApp, SLOT( closeAllWindows() ), actionCollection() );
    KStandardAction::preferences( this, SLOT( optionsPreferences() ), actionCollection() );
}

void KNewz::showFileOpenDialog( const QString &file, bool addToRecentFiles )
{
    NzbReader reader;
    QList<NzbFile*> nzbFiles;
    NzbFile *nzbFile = reader.parseData( file );

    if( nzbFile->size() > 0 ){
        nzbFiles.append( nzbFile );
        NzbDialog *nzbDialog;

        try{
            nzbDialog = new NzbDialog( this, nzbFiles );
            nzbDialog->exec();

            if( nzbDialog->result() == QDialog::Accepted ){

                if( addToRecentFiles ){
                    recentFiles->addUrl( KUrl( file ) );
                }

                DownloadQueue::append( nzbFiles );
                model->changed();
            }

        }catch( ConstructionException &e ){
            kDebug() << e.what();
            delete nzbDialog;
        }
    }
}

void KNewz::showFileOpenDialog( const QStringList &files )
{
    NzbReader reader;
    QList<NzbFile*> nzbFiles;

    for( int i = 0, size = files.size(); i < size; i++ ){

        if( files.at( i ).size() > 0 ){
            QString file( files.at( i ) );

            NzbFile *nzbFile = reader.parseData( file );

            if( nzbFile->size() > 0 ){
                recentFiles->addUrl( KUrl( file ) );
                nzbFiles.append( nzbFile );
            }
        }

    }

    if( nzbFiles.size() > 0 ){

        NzbDialog *nzbDialog;
        try{
            nzbDialog = new NzbDialog( this, nzbFiles );
            nzbDialog->exec();

            if( nzbDialog->result() == QDialog::Accepted ){
                DownloadQueue::append( nzbFiles );
                model->changed();
            }

        }catch( ConstructionException &e ){
            kDebug() << e.what();
            delete nzbDialog;
        }
    }
}

void KNewz::urlOpen()
{
    QStringList files =  KFileDialog::getOpenFileNames( KUrl(), "*.nzb *.NZB |NZB Files", this, i18n( "Open NZB File" ) );

    if( files.size() > 0 ){
        showFileOpenDialog( files );
    }
}

#include "knewz.moc"
