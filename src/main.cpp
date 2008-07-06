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

#include <KDE/KIcon>
#include <KDE/KLocalizedString>
#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>
#include <KDE/KUniqueApplication>
#include "knewz.h"

static const char version[] = "1.0";

int main ( int argc, char **argv )
{
    KCmdLineOptions options;
    options.add( "+files", ki18n( "NZB files to open. You can list multiple files here." ) );
    KAboutData about( "KNewz", QByteArray(), ki18n( "KNewz" ), version, ki18n( "A program for downloading the contents of NZB files from newsgroups for KDE4" ),
                        KAboutData::License_GPL, ki18n( "(C) 2007 Lawrence Lee" ), ki18n(""), "", "valheru@facticius.net" );
    about.addAuthor( ki18n( "Lawrence Lee" ), ki18n( "Lead programmer" ), "valheru@facticius.net", "knewz.googlecode.com" );
    KCmdLineArgs::init( argc, argv, &about );
    KCmdLineArgs::addCmdLineOptions( options );
    KCmdLineArgs::addTempFileOption();
    KUniqueApplication::addCmdLineOptions();

    KUniqueApplication app;
    KNewz *m = new KNewz();
    m->show();
    return app.exec();

}
