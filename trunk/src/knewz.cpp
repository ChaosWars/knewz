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
#include <KDE/KApplication>
#include <KDE/KCmdLineArgs>
#include <KDE/KConfig>
#include <KDE/KConfigDialog>
#include <KDE/KFileDialog>
#include <KDE/KGlobal>
#include <KDE/KLocale>
#include <KDE/KRecentFilesAction>
#include <KDE/KStandardAction>
#include <KDE/KSystemTrayIcon>
#include <QHeaderView>
#include <QMenu>
#include "downloadqueue.h"
#include "knewz.h"
#include "knewzmodel.h"
#include "knewzview.h"
#include "modeltest.h"
#include "nzbdialog.h"
#include "nzbfile.h"
#include "nzbreader.h"
#include "serverwidget.h"
#include "settings.h"

KNewz::KNewz( QWidget *parent )
    : KXmlGuiWindow( parent ),
      view( new KNewzView( this ) ),
      model( new KNewzModel( view ) ),
      modeltest( new ModelTest( model, this ) ),
      downloadqueue( DownloadQueue::Instance() ),
      ok_to_close( false )
{
    view->setModel( model );
    view->header()->setResizeMode( 0, QHeaderView::ResizeToContents );
    setCentralWidget( view );
    setupActions();
    setupGUI();
    setAutoSaveSettings();
    config = KGlobal::config();
    KConfigGroup configGroup( config, "RecentFiles" );
    recentFiles->loadEntries( configGroup );
    trayIcon = new KSystemTrayIcon( "applications-internet", this );
    connect( trayIcon, SIGNAL( quitSelected() ), SLOT( exit() ) );
    trayIcon->contextMenu()->addAction( openFiles );
    trayIcon->contextMenu()->addAction( recentFiles );
    trayIcon->contextMenu()->addAction( preferences );
    trayIcon->show();
}

KNewz::~KNewz()
{
    //Save the recent files entries
    KConfigGroup configGroup( config, "RecentFiles" );
    recentFiles->saveEntries( configGroup );
    delete modeltest;
    delete model;
    delete view;
}

void KNewz::openRecentFile( const KUrl &url )
{
    NzbReader reader;
    QList<NzbFile*> nzbFiles;
    NzbFile *nzbFile = reader.parseData( url.path() );

    if( nzbFile->size() > 0 ){
        nzbFiles.append( nzbFile );
        NzbDialog *nzbDialog;
        nzbDialog = new NzbDialog( this, nzbFiles );
        nzbDialog->exec();

        if( nzbDialog->result() == QDialog::Accepted ){
            downloadqueue->mutex().lock();
            int row = downloadqueue->size();
            int count = nzbDialog->files().size();

            if( count < 1 )
                return;

            model->insertRows( row, count );

            foreach( NzbFile *nzbFile, nzbDialog->files() ){
//                 downloadqueue->append( nzbFile );
                QModelIndex parent = model->index( row, 0 );
                model->insertRows( 0, nzbFile->size(), parent );
                model->setData( parent, QVariant::fromValue( *nzbFile ) );
                row++;
            }

            downloadqueue->mutex().unlock();
//             model->changed();
        }
    }
}

void KNewz::optionsConfigure()
{
    if ( KConfigDialog::showDialog( "settings" ) )  {
        return;
    }

    KConfigDialog *dialog = new KConfigDialog( this, "settings", Settings::self() );
    QWidget *generalSettingsDlg = new QWidget();
    new ServerWidget( generalSettingsDlg );
    dialog->addPage( generalSettingsDlg, i18n( "Server" ), "preferences-system-network" );
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    connect( dialog, SIGNAL( settingsChanged( QString ) ), this, SLOT( settingsChanged() ) );
    dialog->show();
}

void KNewz::settingsChanged()
{
}

void KNewz::setupActions()
{
    setStandardToolBarMenuEnabled( true );
    createStandardStatusBarAction();
    openFiles = KStandardAction::open( this, SLOT( urlOpen() ), actionCollection() );
    recentFiles = KStandardAction::openRecent( this, SLOT( openRecentFile( KUrl ) ), actionCollection() );
    KStandardAction::quit( this, SLOT( exit() ), actionCollection() );
    preferences = KStandardAction::preferences( this, SLOT( optionsConfigure() ), actionCollection() );
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
        nzbDialog = new NzbDialog( this, nzbFiles );
        nzbDialog->exec();

        if( nzbDialog->result() == QDialog::Accepted ){
            int count = nzbDialog->files().size();

            if( count < 1 )
                return;

            downloadqueue->mutex().lock();
            int row = downloadqueue->size();
            model->insertRows( row, count );

            foreach( NzbFile *nzbFile, nzbDialog->files() ){
                QModelIndex parent = model->index( row, 0 );
                model->insertRows( 0, nzbFile->size(), parent );
                model->setData( parent, QVariant::fromValue( *nzbFile ) );
                row++;
            }

            downloadqueue->mutex().unlock();
        }
    }

//     downloadqueue->dumpQueue();
}

void KNewz::urlOpen()
{
    QStringList files =  KFileDialog::getOpenFileNames( KUrl(), "*.nzb *.NZB |NZB Files", this, i18n( "Open NZB File" ) );

    if( files.size() > 0 ){
        showFileOpenDialog( files );
    }
}

bool KNewz::queryClose()
{
    if( !ok_to_close ){
        hide();
    }

    return ok_to_close;
}

bool KNewz::queryExit()
{
    return true;
}

void KNewz::exit()
{
    ok_to_close = true;
    kapp->setQuitOnLastWindowClosed( true );
    close();
}

#include "knewz.moc"
