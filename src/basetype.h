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
* @class BaseType basetype.h
 */
#ifndef BASETYPE_H
#define BASETYPE_H

#include <KDE/KDebug>
#include <QtCore>

/**
 * @brief The base class for objects specific to the NZB file
 *
 * The objects that an NZB file is made up of, ie. the segments, files and
 * the NZB file itself all inherit from this class. It provides a mechanism
 * used by the models of the model/view architechture to determine at what
 * level the object they are displaying resides in the file. The models can
 * as use it to track the toggled state of an object for download purposes.
 *
 * Classes inheriting from this class must reimplement the function type at
 * the very least. Convention is that the function returns the name of the
 * inheriting class as a QString.
 *
 * @author Lawrence Lee <valheru.ashen.shugar@gmail.com>
 */

class BaseType
{
	public:
		enum Type{NZBFILE = 0, FILE};

		/**
			* Creates a BaseType object with a default checked state.
			*
			* @param state
			*      Optional state parameter. The default is checked.
			*/
		BaseType(Type type, Qt::CheckState state = Qt::Checked)
		: m_type(type), m_state(state)
		{
		}

		virtual ~BaseType()
		{
		}

		virtual void print() = 0;
		virtual void dumpQueue() = 0;

		/**
			* Returns the checked state of the object.
			*
			* @return
			*      The checked state of the oject.
			*/
		Qt::CheckState state() const { return m_state; }

		/**
			* Update the checked state of the object.
			*
			* @param state
			*      The new checked state of the object.
			*/
		void setState(Qt::CheckState state) { m_state = state; }

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
		BaseType::Type type() const { return m_type; }

		/**
			* Assigns @p other to this object and returns a reference.
			*/
		BaseType& operator=(const BaseType &other)
		{
			if(this != &other)
			{
				m_type = other.m_type;
				m_state = other.m_state;
			}

			return *this;
		}

		friend class NzbFile;
		friend class File;

	private:
		BaseType::Type m_type;
		Qt::CheckState m_state;

};

#endif
