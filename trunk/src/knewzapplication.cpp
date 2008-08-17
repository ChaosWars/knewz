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

#include <KDE/KCmdLineArgs>
#include "downloadqueue.h"
#include "knewz.h"
#include "knewzapplication.h"
#include "knewzsearchmodel.h"
#include "knewzsettings.h"

#if defined Q_WS_X11
#include <KDE/KStartupInfo>
#endif

KNewz* KNewzApplication::mainWindow = 0;
DownloadQueue* KNewzApplication::downloadqueue = 0;
KNewzSearchModel* KNewzApplication::searchModel = 0;

KNewzApplication::KNewzApplication()
 : KUniqueApplication()
{
    setQuitOnLastWindowClosed( false );

    //Initialize the static classes
    if( !downloadqueue )
        downloadqueue = DownloadQueue::Instance();

    if( !searchModel ){
        searchModel = KNewzSearchModel::self();
        searchModel->loadEngines();
    }
}

KNewzApplication::~KNewzApplication()
{
    downloadqueue->detach();
}

int KNewzApplication::newInstance()
{
    if( !mainWindow ){
        QList<KMainWindow*> allWindows = KMainWindow::memberList();

        if (!allWindows.isEmpty()) {
            mainWindow = dynamic_cast< KNewz* >( allWindows.first() );
            KUniqueApplication::newInstance();
        }

    }else{
        KCmdLineArgs::setCwd( QDir::currentPath().toUtf8() );
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        if( args->count() > 0 ){
            //Parse the command line options
            mainWindow->parseCommandLineArgs();

        }else{
            //Let KUniqueApplication show the window
            KUniqueApplication::newInstance();
        }

    }

    return 0;
}
