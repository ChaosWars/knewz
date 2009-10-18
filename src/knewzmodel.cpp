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
#include <KDE/KLocalizedString>
#include <KDE/KUrl>
#include <QMutex>
#include <QMutexLocker>
#include "downloadqueue.h"
#include "file.h"
#include "knewzmodel.h"
#include "knewzview.h"
#include "nzbfile.h"
#include "nzbmimedata.h"
#include "nzbreader.h"

KNewzModel::KNewzModel(KNewzView *parent)
        : BaseModel(parent),
        m_parent(parent)
{
	downloadqueue = new DownloadQueue();
    rootItem << "" << i18n("Subject") << i18n("Size (MiB)") << i18n("Status") << i18n("ETA");
}

KNewzModel::~KNewzModel()
{
	delete downloadqueue;
}

int KNewzModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
    return 5;
}

QVariant KNewzModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
	{
        return QVariant();
	}

    if (role == Qt::CheckStateRole && index.column() == 0)
    {
        BaseType *base = static_cast< BaseType* >(index.internalPointer());
        return base->state();
    }

    if (role != Qt::DisplayRole)
	{
        return QVariant();
	}

    const QModelIndex parentIndex = index.parent();

    if (!parentIndex.isValid())
    {
        NzbFile *nzbFile = downloadqueue->at(index.row());

        switch (index.column())
        {
            case 0:
                break;
            case 1:
                return nzbFile->filename();
                break;
            case 2:
                return QString().setNum(nzbFile->bytes() / 1048576.00, 'f', 2);
                break;
            case 3:
                if (nzbFile->status() > 0)
                    return QString().setNum(nzbFile->status() / 1048576.00, 'f', 2);
                else
                    return "Queued";
                break;
            case 4:
                break;
            default:
                return QVariant();
                break;
        }
    }
    else
    {
        File *file = downloadqueue->at(parentIndex.row())->at(index.row());

        switch (index.column())
        {
            case 0:
                break;
            case 1:
                return file->subject();
                break;
            case 2:
                return QString().setNum(file->bytes() / 1048576.00, 'f', 2);
                break;
            case 3:
                if (file->status() > 0)
                    return QString().setNum(file->status() / 1048576.00, 'f', 2);
                else
                    return "Queued";
                break;
            case 4:
                break;
            default:
                return QVariant();
                break;
        }
    }

    return QVariant();
}

bool KNewzModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(column);
	
    if(action == Qt::IgnoreAction)
	{
        return true;
	}

	QMutexLocker lock(&downloadqueue->mutex());
	//External drag, ie. drag and drop from exploder/dolphin/konqueror/whatever Jobs is screwing you over with...
    if (data->hasFormat("text/uri-list"))
    {
        if(data->hasUrls())
        {
            QStringList files;

            foreach(const QUrl &url, data->urls())
            {
                files << url.path();
            }

            NzbReader reader;
            QList<NzbFile*> nzbFiles;

            foreach(const QString &file, files)
            {
                if(file.size() > 0)
                {
                    NzbFile *nzbFile = reader.parseLocalData(file);

                    if (nzbFile->size() > 0)
                    {
                        nzbFiles.append(nzbFile);
                    }
                }

            }

            if(nzbFiles.size() == 0)
			{
                return false;
			}

            if(parent.isValid())
            {
                /* We only want to add stuff to the top level if we are dropping NZB files */
				int beginrow = row;
				QModelIndex idx;

				if(parent.parent().isValid())
				{
					beginrow = parent.parent().row();
				}

				if(m_parent->dropIndicatorPosition() == QAbstractItemView::BelowItem)
				{
					beginrow++;
				}

				insertNzbFiles(nzbFiles, beginrow);

				/*if(insertRows(beginrow, nzbFiles.size()))
				{
					QModelIndex start = index(beginrow, 0);
					
					foreach(NzbFile *nzbFile, nzbFiles)
					{
						QModelIndex idx = index(beginrow, 0);
						setData(idx, QVariant::fromValue(*nzbFile));
						beginrow++;
					}

					emit dataChanged(start, index(beginrow - 1, columnCount()));
				}*/
            }
            else
            {
                /* Top level drop */
				appendNzbFiles(nzbFiles);
            }
		}
	}

	//Internal drag, ie. user dragging and dropping rows in the view
	if (data->hasFormat("text/x-nzb"))
	{
		const NzbMimeData *nzbMimeData = dynamic_cast<const NzbMimeData*>(data);

		if (!nzbMimeData)
		{
			return true;
		}

		QList<BaseType*> nzbData = nzbMimeData->getNzbData();

		if(parent.parent().isValid())
		{
			//Drop was performed on a File
			//Any nzbfiles must therefore be ignored
			//Any files must only be processed if they belong to the parent of the child the drop occured on
			NzbFile *nzbFile = static_cast<NzbFile*>(parent.parent().internalPointer());

			if(!nzbFile)
			{
				return true;
			}

			File *droptarget = static_cast<File*>(parent.internalPointer());

			if(!droptarget)
			{
				return true;
			}

			if(!nzbData.contains(droptarget))
			{
				QModelIndex nzbParent = parent.parent();
				QList<File*> files;

				foreach(BaseType *base, nzbData)
				{
					if(base->type() == BaseType::FILE)
					{
						File *file = dynamic_cast<File*>(base);

						if(!file)
						{
							continue;
						}

						if(file->parent() == nzbFile)
						{
							//We have to do this the slow, safe way since by taking a file we invalidate any known indexes
							int filerow = nzbFile->indexOf(file);
							beginRemoveRows(nzbParent, filerow, filerow);
							files.append(nzbFile->takeAt(filerow));
							endRemoveRows();
						}
					}
				}

				int numfiles = files.size();

				if(numfiles > 0)
				{
					int beginrow = nzbFile->indexOf(droptarget);

					if(m_parent->dropIndicatorPosition() == QAbstractItemView::BelowItem)
					{
						beginrow++;
					}

					insertFiles(nzbParent, files, beginrow);
				}
			}
		}
		else
		{
			if(parent.isValid())
			{
				//Drop occured on a top-level NzbFile
				//Any nzbfiles must be processed
				//Only files that belong to the nzbfile must be prepended to the nzbfile
				NzbFile *droptarget = static_cast<NzbFile*>(parent.internalPointer());
				
				if(!droptarget)
				{
					return true;
				}

				if(!nzbData.contains(droptarget))
				{
					//If we don't get the index at column 0, everything goes to shit
					QModelIndex nzbParent = index(row, 0);
					QList<NzbFile*> nzbFiles;
					QList<File*> files;
					NzbFile *nzbFile = NULL;
					File *file = NULL;
					int filerow = -1;

					foreach(BaseType *base, nzbData)
					{
						switch(base->type())
						{
							case BaseType::NZBFILE:
								nzbFile = dynamic_cast<NzbFile*>(base);

								if(!nzbFile)
								{
									continue;
								}

								filerow = downloadqueue->indexOf(nzbFile);
								beginRemoveRows(QModelIndex(), filerow, filerow);
								nzbFiles.append(downloadqueue->takeAt(filerow));
								endRemoveRows();
								break;
							case BaseType::FILE:
								file = dynamic_cast<File*>(base);

								if(!file)
								{
									continue;
								};

								if(file->parent() == droptarget)
								{
									filerow = droptarget->indexOf(file);
									beginRemoveRows(nzbParent, filerow, filerow);
									files.append(droptarget->takeAt(filerow));
									endRemoveRows();
								}
								break;
							default:
								break;
						}
					}

					if(nzbFiles.count() > 0)
					{
						insertNzbFiles(nzbFiles);
					}

					if(files.size() > 0)
					{
						insertFiles(nzbParent, files);
					}
				}
			}
			else
			{
				//Drop occured outside the tree's contents
				//Only nzbfiles must be appended to the tree's contents
				
			}
		}

        //Note: The list is garuanteed to contain only parents who's children are not in the list.
		//See mimeData()
        /*foreach(BaseType *base, nzbData)
        {
            if (base->type() == BaseType::NZBFILE)
            {
                NzbFile *nzbFile = dynamic_cast< NzbFile* >(base);

                if (!nzbFile)
				{
                    continue;
				}

                //We can do this without an operator== in NzbFile since although the Qt docs claim
                //you must have it, the compiler just compares the memory address for pointers.
                //Since a copy operation on a QList of pointers just copies the pointer, the memory
                //in all lists will point to the same object.
                int currentrow = downloadqueue->indexOf(nzbFile);
                //We can't use remove/insertRows() here, and we need to implement the
				//begin/endRemove/InsertRows to avoid corruption of the model/view
				QModelIndex idx = index(currentrow, 0);

                //Naughty user, no dropping items on themselves or their children
                if (idx == parent || idx == parent.parent())
				{
                    continue;
				}

                bool expanded = view->isExpanded(idx);
				beginRemoveRows(QModelIndex(), currentrow, currentrow);
                //Essentially take yourself...oh, the innuendo inherent in this operation :p
				nzbFile = downloadqueue->takeAt(currentrow);
                int rows = nzbFile->size();
                endRemoveRows();
                //Chaining ternary expressions...besides looking cool, it also deals with wankers who think dropping
                //top-level items on other peoples children is funny
				currentrow = parent.parent().isValid() ? parent.parent().row() : (parent.isValid() ? parent.row() : rowCount());
				beginInsertRows(QModelIndex(), currentrow, currentrow);
				downloadqueue->insert(currentrow, nzbFile);
                endInsertRows();
				QModelIndex newIdx = index(currentrow, 0);
                emit dataChanged(newIdx, index(rows, columnCount(), newIdx));

                if (expanded)
				{
                    view->setExpanded(newIdx, true);
				}
            }
            else
            {
                File *file = dynamic_cast<File*>(base);

				if(!file)
				{
					continue;
				}

                //Only process the drop if it is dropped on it's parent or on another sibling
                if (file->parent() == parent.internalPointer() || file->parent() == parent.parent().internalPointer())
                {
                    NzbFile *nzbFile = file->parent();
					int currentrow = nzbFile->indexOf(file);

					if (currentrow >= 0 && currentrow < nzbFile->size())
                    {
                        QModelIndex parentIdx = index(downloadqueue->indexOf(nzbFile), 0);
						beginRemoveRows(parentIdx, currentrow, currentrow);
						file = nzbFile->takeAt(currentrow);
                        endRemoveRows();

						if(filerow < 0)
						{
							if(file->parent() != parent.internalPointer())
							{
								File *child = static_cast<File*>(parent.internalPointer());
								filerow = nzbFile->indexOf(child) + 1;
							}
							else
							{
								filerow = 0;
							}
						}
						
						beginInsertRows(parentIdx, filerow, filerow);
						nzbFile->insert(filerow, file);
                        endInsertRows();
						emit dataChanged(index(filerow, 0, parentIdx), index(filerow, columnCount(), parentIdx));

						if(currentrow > filerow)
						{
							filerow++;
						}
                    }

                }
            }
        }*/
    }

	//Unselect rows, otherwise the wrong rows become selected, which is odd and confusing.
	//NOTE For some reason unkown to mankind, this line is absolutely fucking imperative, otherwise
	//the selection gets deleted if nothing is done with it. Go figure.
	view->selectionModel()->clearSelection();
    return true;
}

Qt::ItemFlags KNewzModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
	{
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
	}

    return Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
}

QModelIndex KNewzModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
	{
        return QModelIndex();
	}

    if (!parent.isValid())
	{
        return createIndex(row, column, downloadqueue->at(row));
	}

    BaseType *base = static_cast< BaseType* >(parent.internalPointer());
    NzbFile *nzbFile = NULL;

    if (base->type() == BaseType::NZBFILE)
    {
        nzbFile = static_cast<NzbFile*>(parent.internalPointer());
    }
    else
    {
        return QModelIndex();
    }

	if(!nzbFile)
	{
		return QModelIndex();
	}

    File *file = nzbFile->value(row);

    if (!file)
	{
        return QModelIndex();
	}

    return createIndex(row, column, file);
}

bool KNewzModel::insertFiles(const QModelIndex &parent, const QList<File*> &files, int row)
{
    if (!parent.isValid() || row < 0 || files.count() < 1)
	{
        return false;
	}

    if(insertRows(row, files.size(), parent))
	{
		QModelIndex start = index(row, 0, parent);
		int currentRow = row;
		
		foreach(File *file, files)
		{
			setData(index(currentRow, 0, parent), QVariant::fromValue(*file));
			currentRow++;
		}

		emit dataChanged(start, index(currentRow - 1, columnCount(), parent));
		return true;
	}

    return false;
}

bool KNewzModel::appendFiles(const QModelIndex &parent, const QList<File*> &files)
{
	qDebug() << rowCount(parent);
	return insertFiles(parent, files, rowCount(parent));
}

bool KNewzModel::insertNzbFiles(const QList<NzbFile*> &nzbFiles, int row)
{
	if(row < 0 || nzbFiles.count() < 1)
	{
		return false;
	}

	if(insertRows(row, nzbFiles.size()))
	{
		QModelIndex start = index(row, 0);
		int currentRow = row;
		
		foreach(NzbFile *nzbFile, nzbFiles)
		{
			setData(index(currentRow, 0), QVariant::fromValue(*nzbFile));
			currentRow++;
		}

		emit dataChanged(start, index(currentRow - 1, columnCount()));
		return true;
	}

	return false;
}

bool KNewzModel::appendNzbFiles(const QList<NzbFile*> &nzbFiles)
{
	return insertNzbFiles(nzbFiles, rowCount());
}

bool KNewzModel::removeRows(const QList<File*> &files)
{
	bool result = true;
	
	foreach(File *file, files)
	{
		result = result && removeRow(file);
	}
	
	return result;
}

bool KNewzModel::removeRow(File *file)
{
	NzbFile *nzbFile = file->parent();
	return removeRows(nzbFile->indexOf(file), 1, index(downloadqueue->indexOf(nzbFile), 0));
}

bool KNewzModel::removeRows(const QList<NzbFile*> &nzbFiles)
{
	bool result = true;

	foreach(NzbFile *nzbFile, nzbFiles)
	{
		result = result && removeRow(nzbFile);
	}

	return result;
}

bool KNewzModel::removeRow(NzbFile *nzbFile)
{
	return removeRows(downloadqueue->indexOf(nzbFile), 1);
}

bool KNewzModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || count < 1 || parent.parent().isValid())
	{
        return false;
	}

    beginInsertRows(parent, row, row + count - 1);

    if (parent.isValid())
    {
        for (int i = 0; i < count; i++)
        {
            NzbFile *nzbFile = downloadqueue->at(parent.row());
            File *file = new File();
            file->setParent(nzbFile);

            if (row >= nzbFile->size())
			{
                nzbFile->append(file);
			}
            else
			{
                nzbFile->insert(row, file);
			}
        }
    }
    else
    {

        bool append = (row >= downloadqueue->size());

        for (int i = 0; i < count; i++)
        {
            if (!append)
			{
                downloadqueue->insert(row, new NzbFile());
			}
            else
			{
                downloadqueue->append(new NzbFile());
			}
        }
    }

    endInsertRows();
    return true;
}

QMimeData* KNewzModel::mimeData(const QModelIndexList &indexes) const
{
    QList< BaseType*> data;
    NzbMimeData *nzbMimeData = new NzbMimeData();
    QModelIndexList list = indexes;
    sanitizeSelection(list);

    foreach(const QModelIndex &index, list)
    {
        BaseType *base = static_cast< BaseType* >(index.internalPointer());
        data.append(base);
    }

    nzbMimeData->setNzbData(data);
    return nzbMimeData;
}

QStringList KNewzModel::mimeTypes() const
{
    QStringList mimetypes;
    mimetypes << "text/x-nzb";
    return mimetypes;
}

void KNewzModel::moveToTop()
{
	QMutexLocker(&downloadqueue->mutex());
	QModelIndexList selection = view->selectionModel()->selectedIndexes();
	
	if (selection.size() < 1)
	{
		return;
	}
	
	QList<File*> files = sanitizeSelection(selection);
	qStableSort(files.begin(), files.end(), qGreater<File*>());
	qStableSort(selection.begin(), selection.end(), qGreater<QModelIndex>());
	
	foreach(File *file, files)
	{
		NzbFile *nzbFile = file->parent();
		int row = nzbFile->indexOf(file);
		
		if (row > 0 && row < nzbFile->size())
		{
			QModelIndex parent = index(downloadqueue->indexOf(nzbFile), 0);
			beginRemoveRows(parent, row, row);
			File *curFile = nzbFile->takeAt(row);
			endRemoveRows();
			beginInsertRows(parent, 0, 0);
			nzbFile->insert(0, curFile);
			endInsertRows();
			emit dataChanged(index(0, 0, parent), index(0, columnCount(), parent));
		}
	}
	
	foreach(const QModelIndex &idx, selection)
	{
		BaseType *base = static_cast< BaseType* >(idx.internalPointer());
		
		if (base->type() == BaseType::NZBFILE)
		{
			if(downloadqueue->size() < 2)
			{
				continue;
			}
			
			NzbFile *nzbFile = dynamic_cast<NzbFile*>(base);
			
			if (!nzbFile || nzbFile == downloadqueue->first())
			{
				continue;
			}
			
			int row = downloadqueue->indexOf(nzbFile);
			
			if (row > 0 && row < downloadqueue->size())
			{
				QModelIndex idx = index(row, 0);
				bool expanded = view->isExpanded(idx);
				beginRemoveRows(QModelIndex(), row, row);
				nzbFile = downloadqueue->takeAt(row);
				endRemoveRows();
				beginInsertRows(QModelIndex(), 0, 0);
				downloadqueue->prepend(nzbFile);
				endInsertRows();
				QModelIndex newIdx = index(0, 0);
				emit dataChanged(newIdx, index(nzbFile->count() - 1, columnCount(), newIdx));
				
				if (expanded)
				{
					view->setExpanded(newIdx, true);
				}
			}
		}
		else
		{
			File *file = dynamic_cast<File*>(base);
			
			if(!file)
			{
				continue;
			}
			
			NzbFile *nzbFile = file->parent();
			
			if(nzbFile->count() < 2 || file == nzbFile->first())
			{
				continue;
			}
			
			int row = nzbFile->indexOf(file);
			
			if(row > 0 && row < nzbFile->size())
			{
				QModelIndex parent = index(downloadqueue->indexOf(nzbFile), 0);
				beginRemoveRows(parent, row, row);
				File *curFile = nzbFile->takeAt(row);
				endRemoveRows();
				beginInsertRows(parent, 0, 0);
				nzbFile->prepend(curFile);
				endInsertRows();
				emit dataChanged(index(0, 0, parent), index(0, columnCount(), parent));
			}
		}
	}
	
	view->selectionModel()->clearSelection();
}

void KNewzModel::moveUp()
{
	QMutexLocker(&downloadqueue->mutex());
    //QModelIndexList selection = view->selectionModel()->selectedIndexes();
    //QList< File* > files = cleanSelection(selection);
}

void KNewzModel::moveDown()
{
	QMutexLocker(&downloadqueue->mutex());
    //QModelIndexList selection = view->selectionModel()->selectedIndexes();
    //QList< File* > files = cleanSelection(selection);
}

void KNewzModel::moveToBottom()
{
	QMutexLocker(&downloadqueue->mutex());
    QModelIndexList selection = view->selectionModel()->selectedIndexes();

    if (selection.size() < 1)
	{
        return;
	}

    QList<File*> files = sanitizeSelection(selection);

	foreach(File *file, files)
	{
		NzbFile *nzbFile = file->parent();
		int row = nzbFile->indexOf(file);

		if (row >= 0 && row < nzbFile->size())
		{
			QModelIndex parent = index(downloadqueue->indexOf(nzbFile), 0);
			beginRemoveRows(parent, row, row);
			File *curFile = nzbFile->takeAt(row);
			endRemoveRows();
			int newrow = nzbFile->size();
			beginInsertRows(parent, newrow, newrow);
			nzbFile->append(curFile);
			endInsertRows();
			emit dataChanged(index(newrow, 0, parent), index(newrow, columnCount(), parent));
		}
	}

    foreach(const QModelIndex &idx, selection)
    {
        BaseType *base = static_cast< BaseType* >(idx.internalPointer());

        if (base->type() == BaseType::NZBFILE)
        {
			if(downloadqueue->size() < 2)
			{
				continue;
			}
			
            NzbFile *nzbFile = dynamic_cast<NzbFile*>(base);

            if (!nzbFile || nzbFile == downloadqueue->last())
			{
                continue;
			}

            int row = downloadqueue->indexOf(nzbFile);

            if (row >= 0 && row < downloadqueue->size())
            {
                QModelIndex idx = index(row, 0);
                bool expanded = view->isExpanded(idx);
                beginRemoveRows(QModelIndex(), row, row);
                nzbFile = downloadqueue->takeAt(row);
                endRemoveRows();
				int newrow = downloadqueue->size();
				beginInsertRows(QModelIndex(), newrow, newrow);
                downloadqueue->append(nzbFile);
                endInsertRows();
				QModelIndex newIdx = index(newrow, 0);
				emit dataChanged(newIdx, index(newrow, columnCount(), newIdx));

                if (expanded)
				{
                    view->setExpanded(newIdx, true);
				}
            }
        }
        else
        {
			File *file = dynamic_cast<File*>(base);

			if(!file)
			{
				continue;
			}
			
			NzbFile *nzbFile = file->parent();

			if(nzbFile->count() < 2 || file == nzbFile->last())
			{
				continue;
			}
			
			int row = nzbFile->indexOf(file);
			
			if (row >= 0 && row < nzbFile->size())
			{
				QModelIndex parent = index(downloadqueue->indexOf(nzbFile), 0);
				beginRemoveRows(parent, row, row);
				File *curFile = nzbFile->takeAt(row);
				endRemoveRows();
				int newrow = nzbFile->size();
				beginInsertRows(parent, newrow, newrow);
				nzbFile->insert(newrow, curFile);
				endInsertRows();
				emit dataChanged(index(newrow, 0, parent), index(newrow, columnCount(), parent));
			}
        }
    }

    view->selectionModel()->clearSelection();
}

QModelIndex KNewzModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
	{
        return QModelIndex();
	}

    BaseType *base = static_cast< BaseType* >(index.internalPointer());
    File *file;

    if (base->type() == BaseType::FILE)
    {
        file = static_cast< File* >(index.internalPointer());
    }
    else
    {
        return QModelIndex();
    }

    NzbFile *nzbFile = file->parent();
    return createIndex(downloadqueue->indexOf(nzbFile), 0, nzbFile);
}

bool KNewzModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || count < 1)
	{
        return false;
	}

    int rows = row + count - 1;
    int beginRow = row;
    beginRemoveRows(parent, row, rows);

    if (parent.isValid())
    {
        NzbFile *nzbFile = static_cast< NzbFile* >(parent.internalPointer());

        while (beginRow <= rows)
        {
            File *file = nzbFile->at(row);
            nzbFile->removeAt(row);
            delete file;
            beginRow++;
        }

		if(nzbFile->size() == 0)
		{
			downloadqueue->removeAt(parent.row());
			delete nzbFile;
		}
    }
    else
    {
        while (beginRow <= rows)
        {
            QMutableListIterator< File* > it(*(downloadqueue->at(row)));

            while (it.hasNext())
            {
                File *file = it.next();
                it.remove();
                delete file;
            }

            NzbFile *nzbFile = downloadqueue->at(row);
            downloadqueue->removeAt(row);
            delete nzbFile;
            beginRow++;
        }
    }

    endRemoveRows();
    return true;
}

int KNewzModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
	{
        return 0;
	}

    if (!parent.isValid())
	{
        return downloadqueue->size();
	}

    if (!parent.parent().isValid())
	{
        return downloadqueue->at(parent.row())->size();
	}

    return 0;
}

bool KNewzModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
	{
        return false;
	}

    BaseType *base = static_cast<BaseType*>(index.internalPointer());

    if (value.canConvert<NzbFile>() && (base->type() == BaseType::NZBFILE))
    {
        NzbFile n_data = value.value<NzbFile>();
        //Wacky syntax is due to downloadqueue being a pointer to a list
		//of pointers to objects.
		//QList<NzbFile*>::operator[](int) returns NzbFile* in this case.
		//So (*downloadqueue) gives us an actual QList, then the []
		//operator gives us a NzbFile*, from which we obtain the NzbFile
		//which we need by using the * operand.
        *(*downloadqueue)[index.row()] = n_data;
        /* Note that since File is also a registered data type, all child rows have
           now been inserted and had their data set as well */
        return true;
    }
    else if (value.canConvert<File>() && (base->type() == BaseType::FILE))
    {
        File f_data = value.value<File>();
        QModelIndex parent = index.parent();
        //Same story here as above, except we need to go one step further and
		//get a File. I'm sure you can follow it by now :)
		*(*(*downloadqueue)[parent.row()])[index.row()] = f_data;
        return true;
    }

    return false;
}

Qt::DropActions KNewzModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

#include "knewzmodel.moc"
