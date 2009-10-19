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
#include <QApplication>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QKeyEvent>
#include <QModelIndexList>
#include <QMutexLocker>
#include "basetype.h"
#include "file.h"
#include "knewzmodel.h"
#include "knewzview.h"
#include "nzbfile.h"
#include "nzbmimedata.h"

KNewzViewEventFilter::KNewzViewEventFilter(KNewzView *parent) : m_parent(parent)
{
}

bool KNewzViewEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = dynamic_cast< QKeyEvent* >(event);

        if (keyEvent->key() == Qt::Key_Delete)
        {
            QModelIndexList list = m_parent->selectionModel()->selectedRows();
            m_parent->model()->sanitizeSelection(list);

            foreach(const QModelIndex &index, list)
            {
                BaseType *base = static_cast< BaseType* >(index.internalPointer());

                if (base->type() == BaseType::NZBFILE)
                {
                    NzbFile *nzbFile = dynamic_cast< NzbFile* >(base);

                    if (nzbFile)
                    {
                        m_parent->model()->removeRow(nzbFile);
                    }

                }
                else
                {
                    File *file = dynamic_cast< File* >(base);
                    NzbFile *parent = file->parent();

                    if (file)
					{
                        m_parent->model()->removeRows(parent->indexOf(file), 1, index.parent());
					}
                }
            }
        }
        else if (keyEvent->key() == Qt::Key_Space)
        {
            QModelIndexList list = m_parent->selectionModel()->selectedRows();
            m_parent->model()->sanitizeSelection(list);

            foreach(const QModelIndex &index, list)
            {
                BaseType *base = static_cast< BaseType* >(index.internalPointer());
                m_parent->model()->changeCheckState(index, base->state() == Qt::Checked ? Qt::Unchecked : Qt::Checked, base);
            }
        }

        return QObject::eventFilter(obj, event);
    }

    return QObject::eventFilter(obj, event);
}

KNewzView::KNewzView(QWidget *parent)
        : QTreeView(parent)
{
    setUniformRowHeights(true);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    eventFilterObject = new KNewzViewEventFilter(this);
    installEventFilter(eventFilterObject);
}

KNewzView::~KNewzView()
{
    delete eventFilterObject;
}

void KNewzView::dragEnterEvent(QDragEnterEvent *event)
{
	QTreeView::dragEnterEvent(event);
	
	if(event->mimeData()->hasFormat("text/uri-list") || event->mimeData()->hasFormat("text/x-nzb"))
    {
        event->acceptProposedAction();
    }
}

void KNewzView::dragMoveEvent(QDragMoveEvent *event)
{
	QTreeView::dragMoveEvent(event);
	
    if(event->mimeData()->hasFormat("text/uri-list") || event->mimeData()->hasFormat("text/x-nzb"))
    {
        event->acceptProposedAction();
    }
}

void KNewzView::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list"))
    {
		QModelIndex idx(indexAt(event->pos()));
		QModelIndex parent = model()->index(idx.row(), 0, idx.parent());
        model()->dropMimeData(event->mimeData(), event->proposedAction(), idx.row(), idx.column(), parent);
        event->acceptProposedAction();
    }
    else if(event->mimeData()->hasFormat("text/x-nzb"))
    {
        event->setDropAction(Qt::MoveAction);
        const NzbMimeData *mimeData = qobject_cast<const NzbMimeData*>(event->mimeData());

        if(mimeData)
        {
			QModelIndex idx(indexAt(event->pos()));
			QModelIndex parent = model()->index(idx.row(), 0, idx.parent());
			model()->dropMimeData(mimeData, event->dropAction(), idx.row(), idx.column(), parent);
            event->accept();
        }
    }
	else
	{
		event->ignore();
	}
}

#include "knewzview.moc"
