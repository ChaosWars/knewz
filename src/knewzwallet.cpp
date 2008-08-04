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
#include "knewzwallet.h"

using namespace KWallet;

QMutex KNewzWallet::m_mutex;
int KNewzWallet::m_ref = 0;
KNewzWallet* KNewzWallet::m_instance = NULL;
Wallet* KNewzWallet::m_wallet = NULL;

KNewzWallet::KNewzWallet() : QObject()
{
    m_wallet = Wallet::openWallet( Wallet::LocalWallet(), -1 );
    if( m_wallet ){
        connect( m_wallet, SIGNAL( walletClosed() ), this, SIGNAL( walletClosed() ) );
        connect( m_wallet, SIGNAL( folderUpdated(const QString& ) ), this, SIGNAL( folderUpdated(const QString& ) ) );
        connect( m_wallet, SIGNAL( folderListUpdated() ), this, SIGNAL(folderListUpdated() ) );
        connect( m_wallet, SIGNAL( folderRemoved( const QString& ) ), this, SIGNAL( folderRemoved( const QString& ) ) );
        connect( m_wallet, SIGNAL( walletOpened( bool ) ), this, SIGNAL( walletOpened( bool ) ) );
    }else{
        KMessageBox::error( 0, i18n( "You either canceled the request, or the KWallet sustem is disabled." ),
                            i18n( "Failed to open the KWallet system" ) );
    }

}

KNewzWallet* KNewzWallet::Instance()
{
    m_mutex.lock();
    m_ref++;
    kDebug() << "m_ref:" << m_ref;
    m_mutex.unlock();

    if( !m_instance ){
        QMutexLocker mutexLock( &m_mutex );
        /* Make sure that the instance wasn't created while we were
        waiting for the lock */
        if( !m_instance ){
            m_instance = new KNewzWallet();
        }
    }

    return m_instance;
}

void KNewzWallet::close()
{
    QMutexLocker lock( &m_mutex );
    m_ref--;
    kDebug() << "m_ref:" << m_ref;

    if( m_ref == 0 ){
        delete m_wallet;
        m_wallet = NULL;
    }
}

void KNewzWallet::open()
{
    if( !m_wallet->isOpen() ){
        m_wallet = Wallet::openWallet( Wallet::LocalWallet(), -1 );
    }
}

#include "knewzwallet.moc"
