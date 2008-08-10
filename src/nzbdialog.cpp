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

#include <KDE/KConfigGroup>
#include <KDE/KDebug>
#include <KDE/KGlobal>
#include <KDE/KIcon>
#include <KDE/KLocalizedString>
#include <QHeaderView>
#include "knewzsettings.h"
#include "modeltest.h"
#include "nzbdialog.h"
#include "nzbfile.h"

NzbDialog::NzbDialog( QWidget *parent, const QList<NzbFile*> &nzbFiles )
    : QDialog( parent )
{
    setupUi( this );
    ok->setIcon( KIcon( "dialog-ok" ) );
    cancel->setIcon( KIcon( "dialog-cancel" ) ); 
    model = new NzbModel( view, nzbFiles );
    modeltest = new ModelTest( model );
    view->setModel( model );
    view->header()->setResizeMode( 0, QHeaderView::ResizeToContents );
    view->setAnimated( KNewzSettings::animatedExpantion() );
    view->setExpandsOnDoubleClick( KNewzSettings::expandOnDoubleClick() );
    //Set up the signals
    connect( ok, SIGNAL( clicked() ), SLOT( okSlot() ) );
    connect( checkAll, SIGNAL( clicked() ), model, SLOT( checkAll() ) );
    connect( checkNone, SIGNAL( clicked() ), model, SLOT( checkNone() ) );
    connect( checkSelected, SIGNAL( clicked() ), model, SLOT( checkSelected() ) );
    connect( uncheckSelected, SIGNAL( clicked() ), model, SLOT( uncheckSelected() ) );
    connect( invertSelection, SIGNAL( clicked() ), model, SLOT( invertSelection() ) );
    //Read the saved configuration
    config = KGlobal::config();
    configGroup = new KConfigGroup( config, "NzbFileDialog" );
    QVariant size( configGroup->readEntry( "size", QSize( 750, 500 ) ) );
    resize( size.toSize() );
    view->resizeColumnToContents( 1 );
}


NzbDialog::~NzbDialog()
{
    configGroup->writeEntry( "size", size() );
    delete configGroup;
    delete model;
    delete modeltest;
}

void NzbDialog::okSlot()
{
    model->trimNzbFiles();
    configGroup->writeEntry( "size", size() );
    emit accept();
}

#include "nzbdialog.moc"
