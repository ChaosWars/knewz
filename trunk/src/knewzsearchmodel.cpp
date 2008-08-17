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
#include <KDE/KConfig>
#include <KDE/KStandardDirs>
#include <QFile>
#include <QMutexLocker>
#include <QTextStream>
#include "knewzsearchmodel.h"

KNewzSearchModel* KNewzSearchModel::m_instance = 0;
QMutex KNewzSearchModel::m_mutex;

KNewzSearchModel::KNewzSearchModel( QObject *parent )
    : QStandardItemModel( 0, 2, parent )
{
}

KNewzSearchModel::~KNewzSearchModel()
{
}

void KNewzSearchModel::loadEngines()
{
    QMap<QString, QString> searchEngines;
    QFile file( KStandardDirs::locate( "appdata", "search/" ) + "searchengines.txt" );

    if( file.exists() ){
        if( file.open( QIODevice::ReadOnly ) ){
            QTextStream stream( &file );

            while( !stream.atEnd() ){
                QString entry( stream.readLine() );
                QStringList engine( entry.split( "," ) );
                searchEngines.insert( engine.value( 0 ), engine.value( 1 ) );
            }
        }
    }

    file.close();
    KConfigGroup configGroup( KGlobal::config(), "SearchEngines" );

    if( configGroup.exists() ){
        searchEngines = configGroup.entryMap();
    }

    m_instance->clear();
    m_instance->setHorizontalHeaderLabels( QStringList() << "Site" << "URL" );
    QMapIterator<QString, QString> it( searchEngines );
    QStandardItem *rootItem = m_instance->invisibleRootItem();

    while( it.hasNext() ){
        it.next();
        QList< QStandardItem* > row;
        QStandardItem *site = new QStandardItem( it.key() );
        row.append( site );
        QStandardItem *url = new QStandardItem( it.value() );
        row.append( url );
        rootItem->appendRow( row );
    }
}

KNewzSearchModel* KNewzSearchModel::self()
{
    if( !m_instance ){
        QMutexLocker lock( &m_mutex );

        if( !m_instance )
            m_instance = new KNewzSearchModel();

    }

    return m_instance;
}

void KNewzSearchModel::saveEngines()
{
    KConfigGroup configGroup( KGlobal::config(), "SearchEngines" );

    for( int i = 0, size = m_instance->rowCount(); i < size; i++ ){
        QStandardItem *item = m_instance->item( i, 0 );
        QString engine = item ? item->text() : "";
        item = m_instance->item( i, 1 );
        QString path = item ? item->text() : "";
        configGroup.writeEntry( engine, path );
    }
}

#include "knewzsearchmodel.moc"
