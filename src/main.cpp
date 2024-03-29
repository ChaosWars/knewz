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

#include <KDE/KLocalizedString>
#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>
#include <KDE/KDebug>
// #include <KDE/KUniqueApplication>
#include "file.h"
#include "knewz.h"
#include "knewzapplication.h"
#include "knewzsettings.h"
#include "nzbfile.h"
#include "segment.h"

static const char description[] = I18N_NOOP("Binary newsgroup program for KDE");
static const char version[] = "1.0";

int main(int argc, char **argv)
{
    KCmdLineOptions options;
    options.add("+files", ki18n("NZB files to open. You can list multiple files here."));
    KAboutData about("knewz", "knewz", ki18n("KNewz"), version, ki18n(description), KAboutData::License_GPL,
                     ki18n("Copyright (c) 2008 Lawrence Lee"), ki18n("Application for downloading the contents of NZB files"),
                     "http://knewz.googlecode.com", "http://code.google.com/p/knewz/issues/list");
    about.addAuthor(ki18n("Lawrence Lee"), ki18n("Lead programmer"), "valheru.ashen.shugar@gmail.com", "http://knewz.googlecode.com");
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs::addTempFileOption();
    KNewzApplication::addCmdLineOptions();

    if (!KNewzApplication::start())
    {
        exit(EXIT_FAILURE);
    }

    KNewzApplication app;

    qRegisterMetaType<NzbFile>("NzbFile");
    qRegisterMetaType<File>("File");
    qRegisterMetaType<Segment>("Segment");
//     qRegisterMetaTypeStreamOperators<NzbFile>( "NzbFile" );
//     qRegisterMetaTypeStreamOperators<File>( "File" );
//     qRegisterMetaTypeStreamOperators<Segment>( "Segment" );
//     app.setQuitOnLastWindowClosed( false );
    KNewz *mainWindow = new KNewz();
    KNewzSettings::startMinimized() ? mainWindow->hide() : mainWindow->show();
    return app.exec();
}
