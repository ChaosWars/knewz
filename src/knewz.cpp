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
#include <KDE/KMessageBox>
#include <KDE/KRecentFilesAction>
#include <KDE/KStandardAction>
#include <KDE/KSystemTrayIcon>
#include <QPushButton>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QMenu>
#include "dockbuttonwidget.h"
#include "downloadqueue.h"
#include "knewz.h"
#include "knewzconfigdialog.h"
#include "knewzmodel.h"
#include "knewzsettings.h"
#include "knewzview.h"
#include "knewzwallet.h"
#include "modeltest.h"
#include "nzbdialog.h"
#include "nzbfile.h"
#include "nzbreader.h"

using namespace KWallet;

KNewz::KNewz( QWidget *parent )
    : KXmlGuiWindow( parent ),
      view( new KNewzView( this ) ),
      model( new KNewzModel( view ) ),
      modeltest( new ModelTest( model, this ) ),
      downloadqueue( DownloadQueue::Instance() ),
      knewzwallet( NULL ),
      ok_to_close( false )
{
    view->setModel( model );
    view->header()->setResizeMode( 0, QHeaderView::ResizeToContents );
    setCentralWidget( view );
    createDockWidget();
    setupActions();
    setupGUI();
    setAutoSaveSettings();
    checkDirectories();
    config = KGlobal::config();
    configGroup = new KConfigGroup( config, "RecentFiles" );
    recentFiles->loadEntries( *configGroup );
    trayIcon = new KSystemTrayIcon( "knewz", this );
    trayIcon->contextMenu()->addAction( openFiles );
    trayIcon->contextMenu()->addAction( recentFiles );
    trayIcon->contextMenu()->addAction( preferences );
    connect( trayIcon, SIGNAL( quitSelected() ), SLOT( exit() ) );
    trayIcon->show();
    //Parse command line arguments
    parseCommandLineArgs();
    //Give the main window time to show before popping up the KWallet password dialog
    QTimer::singleShot( 1000, this, SLOT( loadSettings() ) );
}

KNewz::~KNewz()
{
    //Save the recent files entries
    configGroup->changeGroup( "RecentFiles" );
    recentFiles->saveEntries( *configGroup );

    if( knewzwallet )
        knewzwallet->close();

    delete configGroup;
    delete dock;
    delete modeltest;
    delete model;
    delete view;
    delete downloadqueue;
    delete trayIcon;
}

void KNewz::checkDirectories()
{
    QDir tempDir( KNewzSettings::tempDirectory().path() );
    if( !tempDir.exists() ){

        if( !tempDir.mkpath( tempDir.path() ) ){
            KMessageBox::error( this, "Error creating temp directory" );
        }

    }

    QDir downloadDir( KNewzSettings::tempDirectory().path() );
    if( !downloadDir.exists() ){

        if( !downloadDir.mkpath( downloadDir.path() ) ){
            KMessageBox::error( this, "Error creating download directory" );
        }

    }
}

void KNewz::createDockWidget()
{
    dock = new QDockWidget( "Queue Manager", this );
    dock->setObjectName( "DockWidget" );
    dock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    dockButtonWidget = new DockButtonWidget( dock );
    dock->setWidget( dockButtonWidget );
    addDockWidget( Qt::LeftDockWidgetArea, dock );
}

void KNewz::loadSettings()
{
    if( KNewzSettings::saveEncrypted() && KNewzSettings::authentication() ){

        if( !knewzwallet ){
            setupWallet();
        }

    }

    KNewzSettings::headerOrientationHorizontal() ?
            dock->setFeatures( QDockWidget::DockWidgetClosable |
                               QDockWidget::DockWidgetMovable |
                               QDockWidget::DockWidgetFloatable ) :
            dock->setFeatures( QDockWidget::DockWidgetClosable |
                               QDockWidget::DockWidgetMovable |
                               QDockWidget::DockWidgetFloatable |
                               QDockWidget::DockWidgetVerticalTitleBar );
    view->setAnimated( KNewzSettings::animatedExpantion() );
    view->setExpandsOnDoubleClick( KNewzSettings::expandOnDoubleClick() );
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
            int row = model->rowCount();
            int count = nzbDialog->files().size();

            if( count < 1 )
                return;

            model->insertRows( row, count );

            foreach( NzbFile *nzbFile, nzbDialog->files() ){
                QModelIndex idx = model->index( row, 0 );
                model->setData( idx, QVariant::fromValue( *nzbFile ) );
                row++;
            }

            downloadqueue->mutex().unlock();
        }
    }
}

void KNewz::optionsConfigure()
{
    if ( KNewzConfigDialog::showDialog( "settings" ) )  {
        return;
    }

    KNewzConfigDialog *dialog = new KNewzConfigDialog( this, "settings", KNewzSettings::self() );
    connect( dialog, SIGNAL( settingsChanged( QString ) ), this, SLOT( loadSettings() ) );
    dialog->show();
}

void KNewz::parseCommandLineArgs()
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    NzbReader reader;
    QList<NzbFile*> nzbFiles;

    for( int i = 0, size = args->count(); i < size; i++ ){

        if( !args->arg( i ).isEmpty() ){

            NzbFile *nzbFile = reader.parseData( args->arg( i ) );

            if( nzbFile->size() > 0 ){
                nzbFiles.append( nzbFile );
            }
        }
    }

    if( nzbFiles.size() > 0 ){

        if( !KNewzSettings::openFilesSilently() ){

            NzbDialog *nzbDialog;
            nzbDialog = new NzbDialog( this, nzbFiles );
            nzbDialog->exec();

            if( nzbDialog->result() == QDialog::Accepted ){
                int count = nzbDialog->files().size();

                if( count < 1 )
                    return;

                downloadqueue->mutex().lock();
                int row = model->rowCount();
                model->insertRows( row, count );

                foreach( NzbFile *nzbFile, nzbDialog->files() ){
                    QModelIndex idx = model->index( row, 0 );
                    model->setData( idx, QVariant::fromValue( *nzbFile ) );
                    row++;
                }

                downloadqueue->mutex().unlock();
            }

        }else{
            int count = nzbFiles.size();
            downloadqueue->mutex().lock();
            int row = model->rowCount();
            model->insertRows( row, count );

            foreach( NzbFile *nzbFile, nzbFiles ){
                QModelIndex idx = model->index( row, 0 );
                model->setData( idx, QVariant::fromValue( *nzbFile ) );
                row++;
            }

            downloadqueue->mutex().unlock();
        }
    }

    args->clear();
}

void KNewz::setupActions()
{
    setStandardToolBarMenuEnabled( true );
    createStandardStatusBarAction();
    toggleDock = dock->toggleViewAction();
    toggleDock->setObjectName( "toggle_dock" );
    toggleDock->setText( "Show &Dock" );
    actionCollection()->addAction( "toggle_dock", toggleDock );
    openFiles = KStandardAction::open( this, SLOT( urlOpen() ), actionCollection() );
    recentFiles = KStandardAction::openRecent( this, SLOT( openRecentFile( KUrl ) ), actionCollection() );
    KStandardAction::quit( this, SLOT( exit() ), actionCollection() );
    preferences = KStandardAction::preferences( this, SLOT( optionsConfigure() ), actionCollection() );
}

void KNewz::setupWallet()
{
    if( KNewzSettings::saveEncrypted() && KNewzSettings::authentication() ){
        knewzwallet = KNewzWallet::Instance( this );
        connect( knewzwallet, SIGNAL( walletClosed() ), SLOT( walletClosed() ) );
    }
}

void KNewz::showFileOpenDialog( const QStringList &files )
{
    NzbReader reader;
    QList<NzbFile*> nzbFiles;

    for( int i = 0, size = files.size(); i < size; i++ ){

        if( !files.at( i ).isEmpty() ){
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
            int row = model->rowCount();
            model->insertRows( row, count );

            foreach( NzbFile *nzbFile, nzbDialog->files() ){
                QModelIndex idx = model->index( row, 0 );
                model->setData( idx, QVariant::fromValue( *nzbFile ) );
                row++;
            }

            downloadqueue->mutex().unlock();
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

void KNewz::walletClosed()
{
}

#include "knewz.moc"
