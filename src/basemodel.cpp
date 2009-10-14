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

#include <QTreeView>
#include "basemodel.h"
#include "basetype.h"
#include "file.h"
#include "nzbfile.h"

BaseModel::BaseModel(QTreeView *parent)
        : QAbstractItemModel(parent)
{
    view = parent;
    connect(view, SIGNAL(clicked(const QModelIndex&)), SLOT(clicked(const QModelIndex&)));
}

BaseModel::~BaseModel()
{
}

QVariant BaseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem.at(section);

    return QVariant();
}

void BaseModel::clicked(const QModelIndex& index)
{
    if (!(index.column() == 0))
        return;

    BaseType *base = static_cast< BaseType* >(index.internalPointer());

    Qt::CheckState checkstate = base->state() == Qt::Checked ? Qt::Unchecked : Qt::Checked;

    changeCheckState(index, checkstate, base);
}

void BaseModel::changeCheckState(const QModelIndex &idx, Qt::CheckState state, BaseType *base)
{
    if (!base)
        base = static_cast< BaseType* >(idx.internalPointer());

    base->setState(state);

    view->update(idx);

    if (base->type() == BaseType::NZBFILE)
    {
        NzbFile *nzbFile = static_cast< NzbFile* >(idx.internalPointer());

        for (int i = 0, size = nzbFile->size(); i < size; i++)
        {
            nzbFile->at(i)->setState(state);
            view->update(index(i, 0, idx));
        }
    }
    else
    {
        NzbFile *nzbFile = static_cast< NzbFile* >(idx.parent().internalPointer());
        Qt::CheckState m_state = nzbFile->first()->state();
        int counter = 0;

        for (int i = 0, size = nzbFile->size(); i < size; i++)
        {

            if (nzbFile->at(i)->state() == m_state)
            {
                counter++;
            }
        }

        counter == nzbFile->size() ? nzbFile->setState(m_state) : nzbFile->setState(Qt::PartiallyChecked);

        view->update(idx.parent());
    }
}

QList<File*> BaseModel::sanitizeSelection(QModelIndexList &selection) const
{
    /* We want to filter the index list here, since we cannot allow a selection to contain both
    root items and children of those root items. Here we traverse the list, filtering out any
    children who's parents are also in the list */

    QMutableListIterator< QModelIndex > it(selection);

    while (it.hasNext())
    {
        QModelIndex idx = it.next();

        if (idx.column() > 0)
            it.remove();
    }

    //List is now 4x shorter
    it.toFront();
    QList<File*> files;

    while (it.hasNext())
    {
        QModelIndex idx = it.next();
        BaseType *base = static_cast< BaseType* >(idx.internalPointer());

        if (base->type() == BaseType::FILE)
        {
            File *file = dynamic_cast< File* >(base);

            if (file)
            {
                //If the current files parent is also in the list, then we don't want to process it
                if(selection.indexOf(idx.parent(), 0) != -1)
                {
                    files.append(file);
                    it.remove();
                }
            }
        }
    }

    return files;
}

#include "basemodel.moc"
