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
#include <KDE/KIcon>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>
#include "nzbdialog.h"
#include "nzbfile.h"
#include "nzbmodel.h"

NzbDialog::NzbDialog( QWidget *parent, const QList<NzbFile*> &nzbfiles )
    : QDialog( parent )
{
    // Set up the model/view
    view = new QTreeView( this );
    model = new NzbModel( view, nzbfiles );
    view->setModel( model );
    //Set up the selection button group
    selectButtonGroup = new QButtonGroup( this );
    checkSelected = new QPushButton( "Check &Selected", this );
    selectButtonGroup->addButton( checkSelected );
    uncheckSelected = new QPushButton( "&Uncheck Selected", this );
    selectButtonGroup->addButton( uncheckSelected );
    checkAll = new QPushButton( "Check &All", this );
    selectButtonGroup->addButton( checkAll );
    checkNone = new QPushButton( "Check &None", this );
    selectButtonGroup->addButton( checkNone );
    invertSelection = new QPushButton( "&Invert Selection", this );
    selectButtonGroup->addButton( invertSelection );
    //Set up the default button group
    defaultButtonGroup = new QButtonGroup( this );
    ok = new QPushButton( KIcon( "dialog-ok" ), "&OK", this );
    defaultButtonGroup->addButton( ok );
    cancel = new QPushButton( KIcon( "dialog-cancel" ), "&Cancel", this );
    defaultButtonGroup->addButton( cancel );
    //Set up the button layout
    buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget( uncheckSelected );
    buttonLayout->addWidget( checkSelected );
    buttonLayout->addWidget( invertSelection );
    buttonLayout->addWidget( checkAll );
    buttonLayout->addWidget( checkNone );
    buttonLayout->addStretch();
    buttonLayout->addWidget( ok );
    buttonLayout->addWidget( cancel );
    //Set up the layout
    layout = new QVBoxLayout();
    layout->addWidget( view );
    layout->addLayout( buttonLayout );
    setLayout( layout );
    //Set up the signals
    connect( ok, SIGNAL( clicked() ), SLOT( accept() ) );
    connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );
    connect( checkAll, SIGNAL( clicked() ), SLOT( checkAllSlot() ) );
    connect( checkNone, SIGNAL( clicked() ), SLOT( checkNoneSlot() ) );
    connect( uncheckSelected, SIGNAL( clicked() ), SLOT( uncheckSelectedSlot() ) );
    connect( invertSelection, SIGNAL( clicked() ), SLOT( invertSelectionSlot() ) );
}


NzbDialog::~NzbDialog()
{
    delete model;
    delete view;
    delete selectButtonGroup;
    delete defaultButtonGroup;
    delete buttonLayout;
    delete layout;
}

void NzbDialog::checkAllSlot()
{
}

void NzbDialog::checkNoneSlot()
{
}

void NzbDialog::checkSelectedSlot()
{
}

void NzbDialog::uncheckSelectedSlot()
{
}

void NzbDialog::invertSelectionSlot()
{
}

#include "nzbdialog.moc"
