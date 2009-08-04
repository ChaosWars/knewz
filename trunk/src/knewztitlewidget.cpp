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
#include <KDE/KLocale>
#include <QDockWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QPaintEngine>
#include <QResizeEvent>
#include <QStylePainter>
#include <QStyleOptionFrameV2>
#include <QVBoxLayout>
#include "knewztitlewidget.h"
#include "knewzsettings.h"

TitleWidgetLabel::TitleWidgetLabel(KNewzTitleWidget *parent, Qt::WindowFlags f)
        : QLabel(parent, f), m_parent(parent)
{
}

TitleWidgetLabel::TitleWidgetLabel(const QString &text, KNewzTitleWidget *parent, Qt::WindowFlags f)
        : QLabel(text, parent, f), m_parent(parent)
{
}

// QStyleOption TitleWidgetLabel::getStyleOption() const
// {
//     QStyleOption opt;
//     opt.initFrom( this );
//
//     if( m_parent->m_orientation & Qt::Vertical ){
//         QSize size = opt.rect.size();
//         size.transpose();
//         opt.rect.setSize( size );
//     }
//
//     return opt;
//
// }

QSize TitleWidgetLabel::minimumSizeHint() const
{
    QSize size = pixmap() ? QSize(32, 32) : QSize(120, 20);

    if (m_parent->m_orientation & Qt::Vertical)
        size.transpose();

    return size;
}

void TitleWidgetLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStylePainter p(this);

    switch (m_parent->m_orientation)
    {
        case Qt::Vertical:
            p.rotate(-90);
            p.translate(0, -width());
            break;
        case Qt::Horizontal:
        default:
            break;
    }

    pixmap() ? p.drawItemPixmap(QRect(QPoint(0, 0), size()), Qt::AlignCenter, *pixmap()) :

    p.drawItemText(QRect(QPoint(0, 10), size()), Qt::AlignCenter, palette(), true, text());
}

void TitleWidgetLabel::resizeEvent(QResizeEvent *event)
{
    event->accept();
    ensurePolished();
    update();
}

QSize TitleWidgetLabel::sizeHint() const
{
    QSize size = pixmap() ? QSize(32, 32) : QSize(120, 20);

    if (m_parent->m_orientation & Qt::Vertical)
        size.transpose();

    return size;
}

QSizePolicy TitleWidgetLabel::sizePolicy() const
{
    QSizePolicy policy;
    policy.setHorizontalPolicy(QSizePolicy::Minimum);
    policy.setVerticalPolicy(QSizePolicy::Fixed);

    switch (m_parent->m_orientation)
    {
        case Qt::Vertical:
            policy.transpose();
        case Qt::Horizontal:
        default:
            break;
    }

    return policy;
}

KNewzTitleWidget::KNewzTitleWidget(QWidget *parent)
        : QFrame(parent)
{
    setAutoFillBackground(true);
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Plain);
    setBackgroundRole(QPalette::Base);
    text = new TitleWidgetLabel(i18n("Queue Manager"), this);
    text->setBackgroundRole(QPalette::Base);
    text->setFont(QFont("Sans", 10, QFont::Bold));
    icon = new TitleWidgetLabel(this);
    icon->setPixmap(KIcon("view-choose").pixmap(32, 32));
    m_orientation = KNewzSettings::headerOrientationHorizontal() ? Qt::Horizontal : Qt::Vertical;

    QSize size(170, 50);
    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    switch (m_orientation)
    {
        case Qt::Horizontal:
            vlayout = 0;
            setMinimumSize(size);
            setSizePolicy(policy);
            hlayout = new QHBoxLayout(this);
            hlayout->addWidget(text);
            hlayout->addStretch();
            hlayout->addWidget(icon);
            setLayout(hlayout);
            break;
        case Qt::Vertical:
            hlayout = 0;
            size.transpose();
            setMinimumSize(size);
            policy.transpose();
            setSizePolicy(policy);
            vlayout = new QVBoxLayout(this);
            vlayout->addWidget(icon);
            vlayout->addStretch();
            vlayout->addWidget(text);
            setLayout(vlayout);
            break;
        default:
            break;
    }

}

KNewzTitleWidget::~KNewzTitleWidget()
{
}

QStyleOptionFrameV2 KNewzTitleWidget::getStyleOption() const
{
    QStyleOptionFrameV2 opt;
    opt.initFrom(this);

    if (m_orientation == Qt::Vertical)
    {
        QSize size = opt.rect.size();
        size.transpose();
        opt.rect.setSize(size);
    }

    return opt;

}

QSize KNewzTitleWidget::minimumSizeHint() const
{
    QSize size(170, 50);

    if (m_orientation == Qt::Vertical)
        size.transpose();

    return size;
}

void KNewzTitleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStylePainter p(this);

    switch (m_orientation)
    {
        case Qt::Vertical:
            p.rotate(-90);
            p.translate(0, -width());
            break;
        case Qt::Horizontal:
        default:
            break;
    }

    p.drawControl(QStyle::CE_FocusFrame, getStyleOption());
}

void KNewzTitleWidget::resizeEvent(QResizeEvent *event)
{
    event->accept();
    QDockWidget *dockWidget = qobject_cast< QDockWidget* >(parentWidget());

    if (dockWidget->features() & QDockWidget::DockWidgetVerticalTitleBar)
    {
        m_orientation = Qt::Vertical;

        if (!vlayout)
        {
            setUpdatesEnabled(false);
            hlayout->removeWidget(text);
            hlayout->removeWidget(icon);
            delete hlayout;
            hlayout = 0;
            vlayout = new QVBoxLayout(this);
            vlayout->addWidget(icon);
            vlayout->addStretch();
            vlayout->addWidget(text);
            setMinimumSize(minimumSizeHint());
            setSizePolicy(sizePolicy());
            setLayout(vlayout);
            setUpdatesEnabled(true);
            ensurePolished();
            update();
        }

    }
    else
    {
        m_orientation = Qt::Horizontal;

        if (!hlayout)
        {
            setUpdatesEnabled(false);
            vlayout->removeWidget(text);
            vlayout->removeWidget(icon);
            delete vlayout;
            vlayout = 0;
            hlayout = new QHBoxLayout(this);
            hlayout->addWidget(text);
            hlayout->addStretch();
            hlayout->addWidget(icon);
            setMinimumSize(minimumSizeHint());
            setSizePolicy(sizePolicy());
            setLayout(hlayout);
            setUpdatesEnabled(true);
            ensurePolished();
            update();
        }

    }
}

QSize KNewzTitleWidget::sizeHint() const
{
    QSize size(170, 50);

    if (m_orientation == Qt::Vertical)
        size.transpose();

    return size;
}

QSizePolicy KNewzTitleWidget::sizePolicy() const
{
    QSizePolicy policy;
    policy.setHorizontalPolicy(QSizePolicy::Preferred);
    policy.setVerticalPolicy(QSizePolicy::Fixed);

    switch (m_orientation)
    {
        case Qt::Vertical:
            policy.transpose();
        case Qt::Horizontal:
        default:
            break;
    }

    return policy;
}

#include "knewztitlewidget.moc"
