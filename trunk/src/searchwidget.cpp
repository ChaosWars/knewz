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
#include "searchwidget.h"

SearchWidget::SearchWidget( QWidget *parent )
 : QWidget( parent )
{
    setupUi( parent );
    add->setIcon( KIcon( "list-add" ) );
    remove->setIcon( KIcon( "list-remove" ) );
    moveToTop->setIcon( KIcon( "go-top" ) );
    moveUp->setIcon( KIcon( "go-up" ) );
    moveDown->setIcon( KIcon( "go-down" ) );
    moveToBottom->setIcon( KIcon( "go-bottom" ) );
}

SearchWidget::~SearchWidget()
{
}

void SearchWidget::addSearchEngine()
{
    
}

#include "searchwidget.moc"
