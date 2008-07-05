/***************************************************************************
 *   Copyright (C) 2007 by Lawrence Lee   *
 *   valheru@facticius.net   *
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
#ifndef BASETYPE_H
#define BASETYPE_H

#include <QtCore>

/**
 * \class BaseType basetype.h
 *
 * \brief The base class for objects specific to the NZB file
 *
 * The objects that an NZB file is made up of, ie. the segments, files and
 * the NZB file itself all inherit from this class. It provides a mechanism
 * used by the models of the model/view architechture to determine at what
 * level the object they are displaying resides in the file. The models can
 * as use it to track the toggled state of an object for download purposes.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */
class BaseType{

    public:

        /**
         * Creates a BaseType object with a default checked state.
         *
         * @param state
         *      Optional state parameter. The default is checked.
         */
        BaseType( Qt::CheckState state = Qt::Checked ) : m_state( state ){};

        virtual ~BaseType(){};

        /**
         * Returns the checked state of the object.
         *
         * @return
         *      The checked state of the oject.
         */
        Qt::CheckState state() const{ return m_state; }

        /**
         * Update the checked state of the object.
         *
         * @param state
         *      The new checked state of the object.
         */
        void setState( Qt::CheckState state ){ m_state = state; }

        /**
         * Returns the type of the object. Classes that inherit from BaseType
         * must reimplement this. It is customary to return the name of the
         * class as a string from this method, eg:
         *
         * \code
         * const QString NewClass::type() const{ return QString( "NewClass" ); }
         * \endcode
         *
         * @return
         *      The object type.
         *
         * \see NzbFile, File, Segment
         */
        virtual const QString type() const = 0;

    private:
        Qt::CheckState m_state;

};

#endif
