/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2007-2009  Joseph Artsimovich <joseph_a@mail.ru>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PROPERTY_SET_H_
#define PROPERTY_SET_H_

#include "ref_countable.h"
#include "intrusive_ptr.h"
#include "Property.h"
#include <vector>

class PropertyFactory;
class QDomDocument;
class QDomElement;
class QString;

class PropertySet : public ref_countable {
public:
    PropertySet() {
    }

    /**
     * \brief Makes a deep copy of another property set.
     */
    PropertySet(PropertySet const& other);

    PropertySet(QDomElement const& el, PropertyFactory const& factory);

    /**
     * \brief Makes a deep copy of another property set.
     */
    PropertySet& operator=(PropertySet const& other);

    void swap(PropertySet& other);

    QDomElement toXml(QDomDocument& doc, QString const& name) const;

    /**
     * Returns a property stored in this set, if one having a suitable
     * type is found, or returns a null smart pointer otherwise.
     */
    template<typename T>
    intrusive_ptr<T> locate();

    template<typename T>
    intrusive_ptr<T const> locate() const;

    /**
     * Returns a property stored in this set, if one having a suitable
     * type is found, or returns a default constructed object otherwise.
     */
    template<typename T>
    intrusive_ptr<T> locateOrDefault();

    template<typename T>
    intrusive_ptr<T const> locateOrDefault() const;

    /**
     * Returns a property stored in this set, if one having a suitable
     * type is found.  Otherwise, a default constructed object is put
     * to the set and then returned.
     */
    template<typename T>
    intrusive_ptr<T> locateOrCreate();

private:
    typedef std::vector<intrusive_ptr<Property>> PropList;
    PropList m_props;
};


template<typename T>
intrusive_ptr<T>
PropertySet::locate() {
    PropList::iterator it(m_props.begin());
    PropList::iterator const end(m_props.end());
    for (; it != end; ++it) {
        if (T* obj = dynamic_cast<T*>(it->get())) {
            return intrusive_ptr<T>(obj);
        }
    }

    return intrusive_ptr<T>();
}

template<typename T>
intrusive_ptr<T const>
PropertySet::locate() const {
    PropList::const_iterator it(m_props.begin());
    PropList::const_iterator const end(m_props.end());
    for (; it != end; ++it) {
        if (T const* obj = dynamic_cast<T const*>(it->get())) {
            return intrusive_ptr<T const>(obj);
        }
    }

    return intrusive_ptr<T const>();
}

template<typename T>
intrusive_ptr<T>
PropertySet::locateOrDefault() {
    intrusive_ptr<T> obj(locate<T>());
    if (!obj.get()) {
        obj.reset(new T);
    }

    return obj;
}

template<typename T>
intrusive_ptr<T const>
PropertySet::locateOrDefault() const {
    intrusive_ptr<T const> obj(locate<T>());
    if (!obj.get()) {
        obj.reset(new T);
    }

    return obj;
}

template<typename T>
intrusive_ptr<T>
PropertySet::locateOrCreate() {
    intrusive_ptr<T> obj(locate<T>());
    if (!obj.get()) {
        obj.reset(new T);
        m_props.push_back(obj);
    }

    return obj;
}

inline void swap(PropertySet& o1, PropertySet& o2) {
    o1.swap(o2);
}

#endif  // ifndef PROPERTY_SET_H_
