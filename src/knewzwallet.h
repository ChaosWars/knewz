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
 * @class KNewzWallet knewzwallet.h
 */
#ifndef KNEWZWALLET_H
#define KNEWZWALLET_H

#include <KDE/KWallet/Wallet>
#include <QMutex>

class QWidget;

/**
 * @brief A static interface to the KWallet system
 *
 * This provides a static interface to the KWallet system, so that you don't
 * have to pass references around when initializing classes. Simply use
 * Instance() to obtain a pointer to this class from whichever class you need
 * to access the KWallet system from. You can then obtain a pointer to a
 * KWallet::Wallet using the wallet() function.
 * This class is reference counted, so it is imperative to call close() on it
 * once your class has finished using it.
 * Failure to call close() on this class when you are finished using it will leave
 * the KWallet system in an invalid state, forcing the user to manually close all
 * open connection via KWalletManager. The safest method to accomplish this would
 * be to obtain a reference to this class and the KWallet system in your classes
 * constructor, and then calling close() it in your classes destructor:
 *
 * @code
 * MyClass::MyClass
 * {
 *     ...
 *     knewzwallet = KNewzWallet::Instance();
 *     ...
 * }
 *
 * MyClass::~MyClass()
 * {
 *     knewzwallet->close();
 * }
 * @endcode
 *
 * After using close() you should reobtain a reference to KNewzWallet using Instance().
 * Failure to do so will fuck up the reference counter.
 *
 * Since the user can manually force the KWallet system to close an open
 * wallet, it is strongly suggested that you connect to this class's walletClosed()
 * signal to track if the wallet has been closed. You can then use isOpen() to check
 * if the wallet is open before executing an operation. If isOpen() returns false, or
 * the walletClosed() signals is emitted, then you can reobtain a connection to KWallet
 * with open().
 *
 * This class is thread safe.
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */

class KNewzWallet : public QObject
{

        Q_OBJECT

    public:
        static KNewzWallet* Instance(QWidget *parent);
        void close();
        void open();
        virtual int sync() { if (m_wallet) return m_wallet->sync(); else return -1; }
        virtual int lockWallet() { if (m_wallet) return m_wallet->lockWallet(); else return -1; }
        virtual const QString& walletName() const { if (m_wallet) return m_wallet->walletName(); else { QString *s = new QString(); return *s; } }
        virtual bool isOpen() const { if (m_wallet) return m_wallet->isOpen(); else return false; }

        virtual void requestChangePassword(WId w) { if (m_wallet) m_wallet->requestChangePassword(w); }
        virtual QStringList folderList() { if (m_wallet) return m_wallet->folderList(); else return QStringList(); }
        virtual bool hasFolder(const QString &f) { if (m_wallet) return m_wallet->hasFolder(f); else return false; }
        virtual bool setFolder(const QString &f) {  if (m_wallet) return m_wallet->setFolder(f); else return false; }
        virtual bool removeFolder(const QString &f) { if (m_wallet) return m_wallet->removeFolder(f); else return false; }
        virtual bool createFolder(const QString &f) { if (m_wallet) return m_wallet->createFolder(f); else return false; }
        virtual const QString& currentFolder() const {  if (m_wallet) return m_wallet->currentFolder(); else return *(new QString()); }
        virtual QStringList entryList() { if (m_wallet) return m_wallet->entryList(); else return QStringList(); }
        virtual int renameEntry(const QString &oldName, const QString &newName) { if (m_wallet) return m_wallet->renameEntry(oldName, newName); else return -1; }
        virtual int readEntry(const QString &key, QByteArray &value) { if (m_wallet) return m_wallet->readEntry(key, value); else return -1; }
        virtual int readMap(const QString &key, QMap< QString, QString > &value) { if (m_wallet) return m_wallet->readMap(key, value); else return -1; }
        virtual int readPassword(const QString &key, QString &value) { if (m_wallet) return m_wallet->readPassword(key, value); else return -1; }
        int readEntryList(const QString &key, QMap< QString, QByteArray > &value) { if (m_wallet) return m_wallet->readEntryList(key, value); else return -1; }
        int readMapList(const QString &key, QMap< QString, QMap< QString, QString > > &value) { if (m_wallet) return m_wallet->readMapList(key, value); else return -1; }
        int readPasswordList(const QString &key, QMap< QString, QString > &value) { if (m_wallet) return m_wallet->readPasswordList(key, value); else return -1; }
        virtual int writeEntry(const QString &key, const QByteArray &value, KWallet::Wallet::EntryType entryType) { if (m_wallet) return m_wallet->writeEntry(key, value, entryType); else return -1; }
        virtual int writeEntry(const QString &key, const QByteArray &value) { if (m_wallet) return m_wallet->writeEntry(key, value); else return -1; }
        virtual int writeMap(const QString &key, const QMap< QString, QString > &value) { if (m_wallet) return m_wallet->writeMap(key, value); else return -1; }
        virtual int writePassword(const QString &key, const QString &value) { if (m_wallet) return m_wallet->writePassword(key, value); else return -1; }
        virtual bool hasEntry(const QString &key) { if (m_wallet) return m_wallet->hasEntry(key); else return false; }
        virtual int removeEntry(const QString &key) { if (m_wallet) return m_wallet->removeEntry(key); else return -1; }
        virtual KWallet::Wallet::EntryType entryType(const QString &key) { if (m_wallet) return m_wallet->entryType(key); else return KWallet::Wallet::Unknown; }

    Q_SIGNALS:
        void walletClosed();
        void folderUpdated(const QString &folder);
        void folderListUpdated();
        void folderRemoved(const QString &folder);
        void walletOpened(bool success);

    protected:
        KNewzWallet(QWidget *parent);
        ~KNewzWallet() { delete m_wallet; };

    private:
        static QMutex m_mutex;
        static int m_ref;
        QWidget *m_parent;
        static KNewzWallet *m_instance;
        static KWallet::Wallet* m_wallet;
        void initializeWallet();
};

#endif
