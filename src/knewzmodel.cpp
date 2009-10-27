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
				if(parent.parent().isValid())
				{
					row = parent.parent().row();
				}

				if(m_parent->dropIndicatorPosition() == QAbstractItemView::BelowItem)
				{
					row++;
				}
			}

			openNzbFiles(nzbFiles, row);
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

				if(files.size() > 0)
				{
					int beginrow = nzbFile->indexOf(droptarget);

					if(m_parent->dropIndicatorPosition() == QAbstractItemView::BelowItem)
					{
						beginrow++;
					}

					insertFiles(nzbFile, files, beginrow);
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
									beginRemoveRows(parent, filerow, filerow);
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
						row = downloadqueue->indexOf(droptarget);
						
						if(m_parent->dropIndicatorPosition() == QAbstractItemView::BelowItem)
						{
							row++;
						}
						
						insertNzbFiles(nzbFiles, row);
					}

					if(files.size() > 0)
					{
						insertFiles(droptarget, files);
					}
				}
			}
			else
			{
				//Drop occured outside the tree's contents
				//Only nzbfiles must be appended to the tree's contents
				QList<NzbFile*> nzbFiles;
				
				foreach(BaseType *base, nzbData)
				{
					if(base->type() == BaseType::NZBFILE)
					{
						NzbFile *nzbFile = dynamic_cast<NzbFile*>(base);

						if(!nzbFile)
						{
							continue;
						}

						int filerow = downloadqueue->indexOf(nzbFile);
						beginRemoveRows(QModelIndex(), filerow, filerow);
						nzbFiles.append(downloadqueue->takeAt(filerow));
						endRemoveRows();
					}
				}
				
				if(nzbFiles.count() > 0)
				{
					appendNzbFiles(nzbFiles);
				}
			}
		}
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

bool KNewzModel::insertFiles(NzbFile *parent, const QList<File*> &files, int row)
{
    if (!parent || row < 0 || files.count() < 1)
	{
        return false;
	}

	beginInsertRows(index(downloadqueue->indexOf(parent), 0), row, row + files.count() - 1);

	foreach(File *file, files)
	{
		parent->insert(row, file);
		row++;
	}

	endInsertRows();

	return true;
}

bool KNewzModel::openNzbFiles(const QList<NzbFile*> &nzbFiles, int row)
{
	if(nzbFiles.count() < 1)
	{
		return false;
	}

	//Append files to view if row is invalid
	if(row < 0 || row > downloadqueue->size())
	{
		row = downloadqueue->size();
	}
	
	if(insertRows(row, nzbFiles.size()))
	{
		QModelIndex start = index(row, 0);

		foreach(NzbFile *nzbFile, nzbFiles)
		{
			setData(index(row, 0), QVariant::fromValue(*nzbFile));
			row++;
		}

		emit dataChanged(start, index(row - 1, columnCount() - 1));
	}

	return true;
}

bool KNewzModel::insertNzbFiles(const QList<NzbFile*> &nzbFiles, int row)
{
	if(row < 0 || nzbFiles.count() < 1)
	{
		return false;
	}

	beginInsertRows(QModelIndex(), row, row + nzbFiles.count() - 1);

	foreach(NzbFile *nzbFile, nzbFiles)
	{
		downloadqueue->insert(row, nzbFile);
		row++;
	}

	endInsertRows();
	return true;
}

bool KNewzModel::appendNzbFiles(const QList<NzbFile*> &nzbFiles)
{
	return insertNzbFiles(nzbFiles, rowCount());
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

void KNewzModel::moveTo(MoveTo position)
{
	QMutexLocker(&downloadqueue->mutex());
	QModelIndexList selection = view->selectionModel()->selectedIndexes();
	cleanSelection(selection);

	if (selection.size() < 1)
	{
		return;
	}

	QItemSelection newSelection;
	int newnzbrow = 0;
	int newfilerow = 0;
	QModelIndex parent;
	QModelIndex newcurrent;
	
	foreach(const QModelIndex &idx, selection)
	{
		BaseType *base = static_cast<BaseType*>(idx.internalPointer());
		
		if(!base)
		{
			continue;
		}

		if(base->type() == BaseType::NZBFILE)
		{
			NzbFile *nzbFile = dynamic_cast<NzbFile*>(base);
			
			if(!nzbFile || downloadqueue->size() < 2 ||
				((position ==  TOP || position == UP) && nzbFile == downloadqueue->first()) ||
				((position ==  BOTTOM || position == DOWN) && nzbFile == downloadqueue->last())
			)
			{
				continue;
			}
			
			int row = downloadqueue->indexOf(nzbFile);
			
			if(row >= 0 && row < downloadqueue->size())
			{
				QModelIndex idx = index(row, 0);
				bool expanded = view->isExpanded(idx);
				beginRemoveRows(QModelIndex(), row, row);
				NzbFile *currentNzbFile = downloadqueue->takeAt(row);
				endRemoveRows();

				switch(position)
				{
					case TOP:
						break;
					case BOTTOM:
						newnzbrow = downloadqueue->count();
						break;
					case UP:
						newnzbrow = row - 1;
						break;
					case DOWN:
						newnzbrow = row + 1;
						break;
					default:
						newnzbrow = row;
						break;
				}

				beginInsertRows(QModelIndex(), newnzbrow, newnzbrow);
				downloadqueue->insert(newnzbrow, currentNzbFile);
				endInsertRows();
				QModelIndex newIdxStart = index(newnzbrow, 0);
				QModelIndex newIdxEnd = index(newnzbrow, columnCount() - 1);
				newSelection.append(QItemSelectionRange(newIdxStart, newIdxEnd));

				if(!newcurrent.isValid())
				{
					newcurrent = newIdxStart;
				}
				
				if(expanded)
				{
					view->setExpanded(newIdxStart, true);
				}

				if(position == TOP)
				{
 					newnzbrow++;
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

			if(static_cast<NzbFile*>(parent.internalPointer()) != nzbFile)
			{
				parent = index(downloadqueue->indexOf(nzbFile), 0);

				if(position == TOP)
				{
					newfilerow = 0;
				}
			}
			
			if(nzbFile->count() < 2 ||
				((position == TOP || position == UP) && file == nzbFile->first()) ||
				((position == BOTTOM || position == DOWN) && file == nzbFile->last())
			)
			{
				continue;
			}
			
			int row = nzbFile->indexOf(file);
			
			if(row >= 0 && row < nzbFile->size())
			{
				beginRemoveRows(parent, row, row);
				File *curFile = nzbFile->takeAt(row);
				endRemoveRows();

				switch(position)
				{
					case TOP:
						break;
					case BOTTOM:
						newfilerow = nzbFile->count();
						break;
					case UP:
						newfilerow = row - 1;
						break;
					case DOWN:
						newfilerow = row + 1;
						break;
					default:
						newfilerow = row;
						break;
				}

				beginInsertRows(parent, newfilerow, newfilerow);
				nzbFile->insert(newfilerow, curFile);
				endInsertRows();
				QModelIndex newIdxStart = index(newfilerow, 0, parent);
				QModelIndex newIdxEnd = index(newfilerow, columnCount() - 1, parent);
				newSelection.append(QItemSelectionRange(newIdxStart, newIdxEnd));

				if(!newcurrent.isValid())
				{
					newcurrent = newIdxStart;
				}

				if(position == TOP)
				{
					newfilerow++;
				}
			}
		}
	}

	view->selectionModel()->select(newSelection, QItemSelectionModel::Select);
	view->selectionModel()->setCurrentIndex(newcurrent,  QItemSelectionModel::Current);
}

void KNewzModel::moveToTop()
{
	moveTo(TOP);
}

void KNewzModel::moveUp()
{
	moveTo(UP);
}

void KNewzModel::moveDown()
{
	moveTo(DOWN);
}

void KNewzModel::moveToBottom()
{
	moveTo(BOTTOM);
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
		NzbFile *nzbFile = downloadqueue->at(parent.row());
		*(*nzbFile)[index.row()] = f_data;
		(*(*nzbFile)[index.row()]).setParent(nzbFile);
        return true;
    }

    return false;
}

Qt::DropActions KNewzModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

#include "knewzmodel.moc"
