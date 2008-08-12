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

#include "knewzhtml.h"

KNewzHTML::KNewzHTML(QWidget *parent)
 : KHTMLPart(parent)
{
    setJScriptEnabled(true);
    setJavaEnabled(true);
    setMetaRefreshEnabled(true);
    setPluginsEnabled(true);
    setOnlyLocalReferences(false);
    showProgressInfo(true);
}

KNewzHTML::~KNewzHTML()
{
}

bool KNewzHTML::urlSelected( const QString &url, int button, int state, const QString &_target,
                             const KParts::OpenUrlArguments &args, const KParts::BrowserArguments &browserArgs )
{
    return KHTMLPart::urlSelected( url, button, state, _target, args, browserArgs );
}

#include "knewzhtml.moc"
