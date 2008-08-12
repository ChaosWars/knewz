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
#include <QVBoxLayout>
#include "knewztitlewidget.h"
#include "knewzsettings.h"

TitleWidgetLabel::TitleWidgetLabel( QWidget *parent, Qt::WindowFlags f )
    : QLabel( parent, f )
{
}

TitleWidgetLabel::TitleWidgetLabel( const QString &text, QWidget *parent, Qt::WindowFlags f )
    : QLabel( text, parent, f )
{
}

QStyleOption TitleWidgetLabel::getStyleOption() const
{
    QStyleOption opt;
    opt.initFrom( this );

    if( m_parent->m_orientation & Qt::Vertical ){
        QSize size = opt.rect.size();
        kDebug() << size;
        size.transpose();
        kDebug() << size;
        opt.rect.setSize( size );
    }

    return opt;

}

QSize TitleWidgetLabel::minimumSizeHint() const
{
    QSize size = pixmap() ? QSize( 35, 35 ) : QSize( 120, 20 );

    if( m_parent->m_orientation & Qt::Vertical )
        size.transpose();

    return size;
}

void TitleWidgetLabel::paintEvent( QPaintEvent *event )
{
    Q_UNUSED(event);
    QStylePainter p(this);

    switch( m_parent->m_orientation ){
        case Qt::Vertical:
            p.rotate( -90 );
//             p.translate( 0, -width() );
            break;
        case Qt::Horizontal:
        default:
            break;
    }

    pixmap() ? p.drawItemPixmap( getStyleOption().rect, Qt::AlignCenter, *pixmap() ) :
               p.drawItemText( getStyleOption().rect, Qt::AlignCenter, palette(), true, text() );
}

void TitleWidgetLabel::resizeEvent( QResizeEvent *event )
{
    event->accept();
    ensurePolished();
    update();
}

void TitleWidgetLabel::setParent( KNewzTitleWidget *parent )
{
    m_parent = parent;
}

QSize TitleWidgetLabel::sizeHint() const
{
    QSize size = pixmap() ? QSize( 35, 35 ) : QSize( 120, 20 );

    if( m_parent->m_orientation & Qt::Vertical )
        size.transpose();

    return size;
}

QSizePolicy TitleWidgetLabel::sizePolicy() const
{
    QSizePolicy policy;
    policy.setHorizontalPolicy( QSizePolicy::Preferred );
    policy.setVerticalPolicy( QSizePolicy::Fixed );

    switch( m_parent->m_orientation ){
        case Qt::Vertical:
            policy.transpose();
        case Qt::Horizontal:
        default:
            break;
    }

    return policy;
}

KNewzTitleWidget::KNewzTitleWidget(QWidget *parent)
    : KTitleWidget(parent)
{
    frame = new QFrame( this );
    frame->setFrameShape( QFrame::StyledPanel );
    frame->setFrameShadow( QFrame::Raised );
    text = new TitleWidgetLabel( i18n( "Queue Manager" ), frame );
    text->setParent( this );
    text->setFont( QFont( "Sans", 10, QFont::Bold ) );
    icon = new TitleWidgetLabel( frame );
    icon->setParent( this );
    icon->setPixmap( KIcon( "view-choose" ).pixmap( 32, 32 ) );
    bool horizontal = KNewzSettings::headerOrientationHorizontal();
    m_orientation = horizontal ? Qt::Horizontal : Qt::Vertical;

    switch( m_orientation ){
        case Qt::Horizontal:
            vlayout = 0;
            hlayout = new QHBoxLayout( frame );
            hlayout->addWidget( text );
            hlayout->addStretch();
            hlayout->addWidget( icon );
            frame->setLayout( hlayout );
            break;
        case Qt::Vertical:
            hlayout = 0;
            vlayout = new QVBoxLayout( frame );
            vlayout->addWidget( icon );
            vlayout->addStretch();
            vlayout->addWidget( text );
            frame->setLayout( vlayout );
            break;
        default:
            break;
    }

    setWidget( frame );
}

KNewzTitleWidget::~KNewzTitleWidget()
{
}

QStyleOptionFrameV2 KNewzTitleWidget::getStyleOption() const
{
    QStyleOptionFrameV2 opt;
    opt.initFrom( this );
    opt.palette.setColor( QPalette::Window, QColor( Qt::white ) );

    if( m_orientation == Qt::Vertical ){
        QSize size = opt.rect.size();
        size.transpose();
        opt.rect.setSize( size );
    }

    return opt;

}

QSize KNewzTitleWidget::minimumSizeHint() const
{
    QSize size( 200, 70 );

    if( m_orientation == Qt::Vertical )
        size.transpose();

    return size;
}

void KNewzTitleWidget::paintEvent( QPaintEvent *event )
{
    Q_UNUSED(event);
    QStylePainter p(this);

    switch( m_orientation ){
        case Qt::Vertical:
            p.rotate( -90 );
            p.translate( 0, -width() );
            break;
        case Qt::Horizontal:
        default:
            break;
    }

    p.drawControl( QStyle::CE_FocusFrame, getStyleOption() );
}

void KNewzTitleWidget::resizeEvent( QResizeEvent *event )
{
    event->accept();
    QDockWidget *dockWidget = qobject_cast< QDockWidget* >( parentWidget() );
    if ( dockWidget->features() & QDockWidget::DockWidgetVerticalTitleBar ) {
        m_orientation = Qt::Vertical;

        if( !vlayout ){
            setUpdatesEnabled( false );
            hlayout->removeWidget( text );
            hlayout->removeWidget( icon );
            delete hlayout;
            hlayout = 0;
            vlayout = new QVBoxLayout( frame );
            vlayout->addWidget( icon );
            vlayout->addStretch();
            vlayout->addWidget( text );
            frame->setLayout( vlayout );
            setUpdatesEnabled( true );
            ensurePolished();
            update();
        }

    }else{
        m_orientation = Qt::Horizontal;

        if( !hlayout ){
            setUpdatesEnabled( false );
            vlayout->removeWidget( text );
            vlayout->removeWidget( icon );
            delete vlayout;
            vlayout = 0;
            hlayout = new QHBoxLayout( frame );
            hlayout->addWidget( text );
            hlayout->addStretch();
            hlayout->addWidget( icon );
            frame->setLayout( hlayout );
            setUpdatesEnabled( true );
            ensurePolished();
            update();
        }

    }
}

QSize KNewzTitleWidget::sizeHint() const
{
    QSize size( 200, 70 );

    if( m_orientation == Qt::Vertical )
        size.transpose();

    return size;
}

QSizePolicy KNewzTitleWidget::sizePolicy() const
{
    QSizePolicy policy;
    policy.setHorizontalPolicy( QSizePolicy::Preferred );
    policy.setVerticalPolicy( QSizePolicy::Fixed );

    switch( m_orientation ){
        case Qt::Vertical:
            policy.transpose();
        case Qt::Horizontal:
        default:
            break;
    }

    return policy;
}

#include "knewztitlewidget.moc"
