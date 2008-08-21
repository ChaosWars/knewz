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
#include <KDE/KLocale>
#include <KDE/KMessageBox>
#include <KDE/KStandardDirs>
#include "displaywidget.h"
#include "knewzconfigdialog.h"
#include "knewzsettings.h"
#include "knewzwallet.h"
#include "searchwidget.h"
#include "serverwidget.h"


using namespace KWallet;

KNewzConfigDialog::KNewzConfigDialog( QWidget *parent, const QString &name, KConfigSkeleton *config )
    : KConfigDialog( parent, name, config ), knewzwallet( NULL )
{
    setAttribute( Qt::WA_DeleteOnClose );
    QWidget *displaySettings = new QWidget();
    displayWidget = new DisplayWidget( displaySettings );
    addPage( displaySettings, i18n( "Application" ), "preferences-desktop" );
    QWidget *serverSettings = new QWidget();
    serverWidget = new ServerWidget( serverSettings );
    addPage( serverSettings, i18n( "Server" ), "preferences-system-network" );
    QWidget *searchSettings = new QWidget();
    searchWidget = new SearchWidget( searchSettings );
    addPage( searchSettings, i18n( "Search" ), "edit-find" );
    connect( this, SIGNAL( applyClicked() ), SLOT( saveWalletSettings() ) );
    connect( searchWidget, SIGNAL( clearSearchHistory() ), this, SIGNAL( clearSearchHistory() ) );

    if( KNewzSettings::saveEncrypted() && KNewzSettings::authentication()){
        setupWallet();
    }

}


KNewzConfigDialog::~KNewzConfigDialog()
{
    knewzwallet->close();
}

void KNewzConfigDialog::saveWalletSettings()
{
    if( KNewzSettings::saveEncrypted() && KNewzSettings::authentication() ){

        if( !knewzwallet )
            setupWallet();

        if( knewzwallet->isOpen() ){
            knewzwallet->writeEntry( "username", QByteArray( KNewzSettings::username().toUtf8() ) );
            knewzwallet->writePassword( "password", KNewzSettings::password() );
            KConfigGroup configGroup( KGlobal::config(), "ServerSettings" );
            configGroup.deleteEntry( "username" );
            configGroup.deleteEntry( "password" );
        }else{
            int result = KMessageBox::messageBox( this, KMessageBox::WarningYesNoCancel,
                                                  i18n( "The application was unable to open KWallet.\nThis was probably because you canceled the transaction with the wallet.\n\nThe application can save the login information in it's configuration file, but this is inadviseable from a security standpoint since the information will be saved as plain text, and anyone with access to your home directory can access this information.\n\nClick Yes to save the information, No to reenter the KWallet password or Cancel to return to the settings dialog." ) );

                                         switch( result ){
                                             case KMessageBox::Yes:
                                                 break;
                                             case KMessageBox::No:
                                                 setupWallet();
                                                 saveWalletSettings();
                                                 break;
                                             case KMessageBox::Cancel:
                                                 enableButtonApply( true );
                                                 show();
                                                 break;
                                             default:
                                                 break;
                                         }
        }

    }else{
    }
}

void KNewzConfigDialog::setupWallet()
{
    knewzwallet = KNewzWallet::Instance( this );
    connect( knewzwallet, SIGNAL( walletClosed() ), SLOT( walletClosed() ) );

    if( !knewzwallet->isOpen() ){
        knewzwallet->open();
    }

    QByteArray username;
    knewzwallet->readEntry( "username", username );
    serverWidget->kcfg_username->setText( username );
    QString password;
    knewzwallet->readPassword( "password", password );
    serverWidget->kcfg_password->setText( password );
    enableButtonApply( false );
}

void KNewzConfigDialog::showEvent( QShowEvent *event )
{
	Q_UNUSED( event )
}

void KNewzConfigDialog::walletClosed()
{
}

#include "knewzconfigdialog.moc"
