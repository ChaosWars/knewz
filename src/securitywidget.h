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

/**
 * @class SecurityWidget securitywidget.h
 */
#ifndef SECURITYWIDGET_H
#define SECURITYWIDGET_H

#include <QtGui/QWidget>
#include <ui_securitysettings.h>

/**
 * @brief General settings widget
 *
 * Widget providing an interface to the general settings for KNewz.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class SecurityWidget : public QWidget, public Ui::SecuritySettings
{
    public:

        /**
         * Constructor
         * @param parent
         *      Parent widget.
         */
        SecurityWidget( QWidget *parent );
        ~SecurityWidget();
};

#endif
