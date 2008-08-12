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
#ifndef KNEWZTITLEWIDGET_H
#define KNEWZTITLEWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QSizePolicy>

class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QStyleOptionFrameV2;
class TitleWidgetLabel;

/**
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class KNewzTitleWidget : public QFrame
{
    Q_OBJECT

    public:
        KNewzTitleWidget(QWidget *parent = 0);
        ~KNewzTitleWidget();
        QSize minimumSizeHint() const;
        Qt::Orientation orientation(){ return m_orientation; }
        void setOrientation( Qt::Orientation orientation ){ m_orientation = orientation; }
        QSize sizeHint() const;
        QSizePolicy sizePolicy() const;

    protected:
        virtual void paintEvent( QPaintEvent *event );
        virtual void resizeEvent( QResizeEvent *event );

    private:
        Qt::Orientation m_orientation;
        TitleWidgetLabel *text, *icon;
        QHBoxLayout *hlayout;
        QVBoxLayout *vlayout;
        QStyleOptionFrameV2 getStyleOption() const;

    friend class TitleWidgetLabel;

};

class TitleWidgetLabel : public QLabel{

    Q_OBJECT

    public:
        TitleWidgetLabel( KNewzTitleWidget *parent = 0, Qt::WindowFlags f = 0 );
        TitleWidgetLabel( const QString & text, KNewzTitleWidget * parent = 0, Qt::WindowFlags f = 0 );
        virtual ~TitleWidgetLabel(){};
        QSize minimumSizeHint() const;
        QSize sizeHint() const;
        QSizePolicy sizePolicy() const;

    protected:
        virtual void paintEvent( QPaintEvent *event );
        virtual void resizeEvent( QResizeEvent *event );

    private:
//         QStyleOption getStyleOption() const;
        KNewzTitleWidget *m_parent;
};

#endif
