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
 * @class KNewz knewz.h
 *
 * @mainpage KNewz
 *      A program for downloading the contents of NZB files for KDE4.
 */
#ifndef KNEWZ_H
#define KNEWZ_H

#include <KDE/KSharedConfig>
#include <KDE/KXmlGuiWindow>
#include <KDE/KUrl>

class KAction;
class KComboBox;
class KConfigGroup;
class KHistoryComboBox;
class KRecentFilesAction;
class KSystemTrayIcon;
class KTabWidget;
class QAction;
class QDockWidget;
class QNetworkReply;
class QNetworkRequest;
class BrowserWidget;
class DockButtonWidget;
//class DownloadQueue;
class KNewzModel;
//class KNewzTitleWidget;
class KNewzView;
class KNewzWallet;
class Connection;

/**
 * @brief The main window of the program.
 *
 * Provides a main window for the program, using KXmlGuiWindow to set up and
 * track changes to the menubar and toolbar.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com
 */

class KNewz : public KXmlGuiWindow
{
        Q_OBJECT

    public:
        /**
         * Constructor.
         * @param parent
         *      Parent widget. Since this is the main window, this should always be 0.
         */
        KNewz(QWidget *parent = 0);
        virtual ~KNewz();
//         static KNewz* self();
        void parseCommandLineArgs();
        KNewzView* view() { return m_view; }
        KNewzModel* model() { return m_model; }

    public Q_SLOTS:

        /**
         * Load the settings for the program
         */
        void loadSettings();

        /**
         * Display the contents of @p files for selection and adding to the download queue.
         * @param files
         *      The list of files to open.
         */
        void openFiles(const QStringList &files, bool silently = false);

    protected:
        virtual bool queryClose();
        virtual bool queryExit();

    private:
        //Variables
        Connection *connection;
        //Widgets
        KTabWidget *mainWidget;
        BrowserWidget *browserWidget;
        KNewzView *m_view;
        KNewzModel *m_model;
        //Download queue instance
        //DownloadQueue *downloadqueue;
        //Actions
        KAction *openFilesAction, *preferences, *toggleDock;
        KConfigGroup *configGroup;
        KRecentFilesAction *recentFiles;
        KSharedConfigPtr config;
        KSystemTrayIcon *trayIcon;
        KNewzWallet *knewzwallet;
        //Dock Widget
        QDockWidget *dock;
        DockButtonWidget *dockButtonWidget;
        //KNewzTitleWidget *titleWidget;
        //Search toolbar
        KComboBox *searchBox;
        KHistoryComboBox *searchLine;
        KAction *searchAction, *searchLineAction, *searchBoxAction;
        //Queue Actions
        KAction *resumeQueueAction, *stopQueueAction, *clearQueueAction;
        //static KNewz *m_instance;
        //static QMutex m_mutex;
        //Functions
        bool ok_to_close;
        void checkDirectories();
        void createDockWidget();
        void setupActions();
        void setupToolbars();
        void setupWallet();

    private Q_SLOTS:
        void exit();
        void openRecentFile(const KUrl &url);
        void optionsConfigure();
        void resumeQueue();
        void stopQueue();
        void search();
        void searchTextChanged(const QString &text);
        void urlOpen();
        void walletClosed();
		void connectionClosed();
};

#endif

