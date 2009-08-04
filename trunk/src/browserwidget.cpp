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
#include <KDE/KIcon>
#include <KDE/KIconLoader>
#include <KDE/KStandardDirs>
#include <QFile>
#include <QDataStream>
#include <QMovie>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QUrl>
#include "browserwidget.h"
#include "downloadqueue.h"
#include "nzbdialog.h"
#include "nzbreader.h"
#include "knewz.h"
#include "knewzmodel.h"
#include "knewzsettings.h"

KNewzCookieJar::KNewzCookieJar(QObject *parent)
        : QNetworkCookieJar(parent)
{
    QFile file(KStandardDirs::locateLocal("appdata", "browser/") + "cookies.dat");
    QByteArray buffer;
    QList<QByteArray> cookies;

    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream stream(&file);

        while (stream.status() == QDataStream::Ok)
        {
            stream >> buffer;
            cookies << buffer;
        }
    }

    file.close();

    QList< QNetworkCookie > cookieList;

    foreach(const QByteArray &cookie, cookies)
    {
        cookieList << QNetworkCookie::parseCookies(cookie);
    }

    setAllCookies(cookieList);
}

KNewzCookieJar::~KNewzCookieJar()
{
    QFile file(KStandardDirs::locateLocal("appdata", "browser/") + "cookies.dat");

    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream stream(&file);
        QList< QNetworkCookie > cookies = allCookies();

        foreach(const QNetworkCookie &cookie, cookies)
        {
            stream << cookie.toRawForm();
        }

    }

    file.close();
}

BrowserWidget::BrowserWidget(KNewz *mainWindow, QWidget *parent)
        : QWidget(parent), m_main(mainWindow)
{
    setupUi(this);
    progressBar->setVisible(false);
//     KIconLoader *iconLoader = KIconLoader::global();
//     idleMovie = iconLoader->loadMovie( "process-idle-kde", KIconLoader::Small );
//     loadingMovie = iconLoader->loadMovie( "process-working-kde", KIconLoader::Toolbar );
//     progressIcon->setMovie( idleMovie );
    QNetworkAccessManager *nam = webView->page()->networkAccessManager();
    nam->setCookieJar(new KNewzCookieJar(nam));
    webView->page()->setForwardUnsupportedContent(true);
    back->setIcon(KIcon("go-previous"));
    forward->setIcon(KIcon("go-next"));
    reload->setIcon(KIcon("view-refresh"));
    stop->setIcon(KIcon("process-stop"));
    backAction = webView->page()->action(QWebPage::Back);
    forwardAction = webView->page()->action(QWebPage::Forward);
    reloadAction = webView->page()->action(QWebPage::Reload);
    stopAction = webView->page()->action(QWebPage::Stop);
    connect(back, SIGNAL(clicked()), backAction, SIGNAL(triggered()));
    connect(forward, SIGNAL(clicked()), forwardAction, SIGNAL(triggered()));
    connect(reload, SIGNAL(clicked()), reloadAction, SIGNAL(triggered()));
    connect(stop, SIGNAL(clicked()), webView, SLOT(stop()));
    connect(webView, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    connect(webView, SIGNAL(loadProgress(int)), progressBar, SLOT(setValue(int)));
    connect(webView->page(), SIGNAL(unsupportedContent(QNetworkReply*)), this, SLOT(unsupportedContent(QNetworkReply*)));
}

BrowserWidget::~BrowserWidget()
{
}

void BrowserWidget::finishedDownload()
{
    QString header = reply->rawHeader("Content-Disposition");
    QRegExp regexp("filename=\".*\"");
    int index = regexp.indexIn(header) + 10;   //compensate for filename="
    int len = regexp.matchedLength() - 15; //strip filename=" and trailing .nzb"
    QString filename = header.mid(index, len);
    NzbReader reader;
    QList< NzbFile* > nzbFiles;
    QByteArray data(reply->readAll());
    nzbFiles.append(reader.parseNetworkData(data, filename));

    if (nzbFiles.size() < 1)
        return;

    KNewzModel *model = m_main->model();

    if (!KNewzSettings::openFilesSilently())
    {
        NzbDialog nzbDialog(this, nzbFiles);
        nzbDialog.exec();

        if (nzbDialog.result() == QDialog::Accepted)
        {
            int count = nzbDialog.files().size();

            if (count < 1)
                return;

            downloadqueue->mutex().lock();

            int row = m_main->model()->rowCount();

            model->insertRows(row, count);

            foreach(NzbFile *nzbFile, nzbDialog.files())
            {
                QModelIndex idx = m_main->model()->index(row, 0);
                model->setData(idx, QVariant::fromValue(*nzbFile));
                row++;
            }

            downloadqueue->mutex().unlock();
        }
    }
    else
    {
        downloadqueue->mutex().lock();
        int row = model->rowCount();
        model->insertRows(row, nzbFiles.size());

        foreach(NzbFile *nzbFile, nzbFiles)
        {
            QModelIndex idx = model->index(row, 0);
            model->setData(idx, QVariant::fromValue(*nzbFile));
            row++;
        }

        downloadqueue->mutex().unlock();
    }

}

void BrowserWidget::load(const QString &string)
{
    QString url = string;

    if (url.left(7) != "http://")
        url.prepend("http://");

    webView->load(QUrl(url));
}

void BrowserWidget::loadStarted()
{
    progressBar->setVisible(true);
    stop->setEnabled(true);
//     progressIcon->setMovie( loadingMovie );
}

void BrowserWidget::loadFinished(bool /*ok*/)
{
    progressBar->setVisible(false);
    stop->setEnabled(false);
//     progressIcon->setMovie( idleMovie );
}

void BrowserWidget::unsupportedContent(QNetworkReply *reply)
{
    this->reply = reply;
    connect(reply, SIGNAL(finished()), this, SLOT(finishedDownload()));
}

#include "browserwidget.moc"
