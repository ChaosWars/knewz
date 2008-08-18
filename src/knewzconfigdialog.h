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
 * @class KNewzConfigDialog knewzconfigdialog.h
 */
#ifndef KNEWZCONFIGDIALOG_H
#define KNEWZCONFIGDIALOG_H

#include <KDE/KConfigDialog>

class QShowEvent;
class DisplayWidget;
class KNewzWallet;
class SearchWidget;
class ServerWidget;

/**
 * @brief KConfigDialog for KNewz
 *
 * Overridden to provide for saving the login information in KWallet.
 * See the KDE documentation for KConfigDialog for member documentation.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class KNewzConfigDialog : public KConfigDialog
{
    Q_OBJECT

    public:
        KNewzConfigDialog( QWidget *parent, const QString &name, KConfigSkeleton *config );
        ~KNewzConfigDialog();

    protected:
        void showEvent( QShowEvent *event );

    private:
        DisplayWidget *displayWidget;
        KNewzWallet *knewzwallet;
        SearchWidget *searchWidget;
        ServerWidget *serverWidget;
        void setupWallet();

    private Q_SLOTS:
        void saveWalletSettings();
        void walletClosed();

    Q_SIGNALS:
        void clearSearchHistory();
};

#endif
