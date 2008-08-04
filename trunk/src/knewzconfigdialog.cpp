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
#include "knewzconfigdialog.h"
#include "generalwidget.h"
#include "serverwidget.h"
#include "knewzsettings.h"
#include "knewzwallet.h"

using namespace KWallet;

KNewzConfigDialog::KNewzConfigDialog( QWidget *parent, const QString &name, KConfigSkeleton *config )
    : KConfigDialog( parent, name, config ), knewzwallet( NULL )
{
    QWidget *generalSettings = new QWidget();
    generalWidget = new GeneralWidget( generalSettings );
    addPage( generalSettings, i18n( "General" ), "preferences-system-general" );
    QWidget *serverSettings = new QWidget();
    serverWidget = new ServerWidget( serverSettings );
    addPage( serverSettings, i18n( "Server" ), "preferences-system-network" );
    setAttribute( Qt::WA_DeleteOnClose );
    settings = KNewzSettings::self();
    connect( this, SIGNAL( applyClicked() ), SLOT( saveWalletSettings() ) );

    if( settings->saveEncrypted() ){
        setupWallet();
    }
}


KNewzConfigDialog::~KNewzConfigDialog()
{
    knewzwallet->close();
}

void KNewzConfigDialog::saveWalletSettings()
{
    if( settings->saveEncrypted() ){

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
                                                  i18n( "The application was unable to open KWallet.\nThis was probably because you canceled the transaction with the wallet. The application can save the login information in it's configuration file, but this is inadviseable from a security standpoint since the information will be saved as plain text, and anyone with access to your home directory can access this information.\nClick Yes to save the information, No to reenter the KWallet password or Cancel to return to the settings dialog." ) );

                                         switch( result ){
                                             case KMessageBox::Yes:
                                                 break;
                                             case KMessageBox::No:
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
    knewzwallet = KNewzWallet::Instance();
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

void KNewzConfigDialog::showEvent( QShowEvent */*event*/ )
{
}

void KNewzConfigDialog::walletClosed()
{
}

#include "knewzconfigdialog.moc"
