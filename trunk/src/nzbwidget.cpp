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
#include <QTreeView>
#include <QVBoxLayout>
#include "nzbwidget.h"
#include "nzbfile.h"
#include "nzbmodel.h"

NzbWidget::NzbWidget( const QList<NzbFile*> &nzbfiles )
{
    layout = new QVBoxLayout();
    view = new QTreeView( this );
    model = new NzbModel( this, nzbfiles );
    view->setModel( model );
    layout->addWidget( view );
    setLayout( layout );
}


NzbWidget::~NzbWidget()
{
    delete view;
    delete model;
    delete layout;
}

#include "nzbwidget.moc"
