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

#include <KDE/KDebug>
#include <KDE/KLocale>
#include <KDE/KMessageBox>
#include <QWidget>
#include "knewzwallet.h"

using namespace KWallet;

QMutex KNewzWallet::m_mutex;
int KNewzWallet::m_ref = 0;
KNewzWallet* KNewzWallet::m_instance = 0;
Wallet* KNewzWallet::m_wallet = 0;

KNewzWallet::KNewzWallet( QWidget *parent )
    : QObject( parent )
{
    m_parent = parent;
    initializeWallet();
}

KNewzWallet* KNewzWallet::Instance( QWidget *parent )
{
    m_mutex.lock();
    m_ref++;
    m_mutex.unlock();

    if( !m_instance ){
        QMutexLocker mutexLock( &m_mutex );
        /* Make sure that the instance wasn't created while we were
        waiting for the lock */
        if( !m_instance ){
            m_instance = new KNewzWallet( parent );
        }
    }

    return m_instance;
}

void KNewzWallet::close()
{
    QMutexLocker lock( &m_mutex );
    m_ref--;

    if( m_ref < 1 ){
        delete m_wallet;
        m_wallet = 0;
    }
}

void KNewzWallet::open()
{
    initializeWallet();
}

void KNewzWallet::initializeWallet()
{
    m_wallet = Wallet::openWallet( Wallet::LocalWallet(), m_parent->winId() );
    if( m_wallet ){

        if( !m_wallet->hasFolder( "KNewz" ) ){

            if( !m_wallet->createFolder( "KNewz" ) ){
                KMessageBox::error( 0, i18n( "There was an error creating the folder for KNewz in KWallet.\n\nYour information was not saved. Resolve this problem first before trying to store your data using KWallet.\n\nIn the meanwhile, you can store your login data in the programs configuration file\n. Be warned though that anyone with access to your home directory can read this information." ), i18n( "Error creating KWallet folder" ) );
            }else{
                m_wallet->setFolder( "KNewz" );
            }

        }else{
            m_wallet->setFolder( "KNewz" );
        }

        connect( m_wallet, SIGNAL( walletClosed() ), this, SIGNAL( walletClosed() ) );
        connect( m_wallet, SIGNAL( folderUpdated(const QString& ) ), this, SIGNAL( folderUpdated(const QString& ) ) );
        connect( m_wallet, SIGNAL( folderListUpdated() ), this, SIGNAL(folderListUpdated() ) );
        connect( m_wallet, SIGNAL( folderRemoved( const QString& ) ), this, SIGNAL( folderRemoved( const QString& ) ) );
        connect( m_wallet, SIGNAL( walletOpened( bool ) ), this, SIGNAL( walletOpened( bool ) ) );
    }else{
        KMessageBox::error( 0, i18n( "You either canceled the request,\nor the KWallet sustem is disabled." ),
                            i18n( "Failed to open the KWallet system" ) );
    }
}

#include "knewzwallet.moc"
