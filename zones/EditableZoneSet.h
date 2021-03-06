/*

    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

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

#ifndef EDITABLE_ZONE_SET_H_
#define EDITABLE_ZONE_SET_H_

#include "EditableSpline.h"
#include "PropertySet.h"
#include "intrusive_ptr.h"
#include <QObject>
#include <boost/mpl/bool.hpp>
#include <boost/foreach.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <map>

class EditableZoneSet : public QObject {
Q_OBJECT
private:
    typedef std::map<EditableSpline::Ptr, intrusive_ptr<PropertySet>> Map;
public:
    class const_iterator;

    class Zone {
        friend class EditableZoneSet::const_iterator;
    public:
        Zone() {
        }

        EditableSpline::Ptr const& spline() const {
            return m_iter->first;
        }

        intrusive_ptr<PropertySet> const& properties() const {
            return m_iter->second;
        }

    private:
        explicit Zone(Map::const_iterator it)
                : m_iter(it) {
        }

        Map::const_iterator m_iter;
    };


    class const_iterator : public boost::iterator_facade<
            const_iterator, Zone const, boost::forward_traversal_tag
    > {
        friend class EditableZoneSet;

        friend class boost::iterator_core_access;
    public:
        const_iterator()
                : m_zone() {
        }

        void increment() {
            ++m_iter;
            m_zone.m_iter = m_ptrSplineMap->find(*m_iter);
        }

        bool equal(const_iterator const& other) const {
            return m_iter == other.m_iter;
        }

        Zone const& dereference() const {
            return m_zone;
        }

    private:
        explicit const_iterator(std::list<EditableSpline::Ptr>::const_iterator it,
                                const Map* splineMap)
                : m_iter(it),
                  m_ptrSplineMap(splineMap),
                  m_zone(splineMap->find(*it)) {
        }

        Zone m_zone;
        std::list<EditableSpline::Ptr>::const_iterator m_iter;
        const Map* m_ptrSplineMap;
    };

    typedef const_iterator iterator;

    EditableZoneSet();

    const_iterator begin() const {
        return iterator(m_splineList.begin(), const_cast<const Map*>(&m_splineMap));
    }

    const_iterator end() const {
        return iterator(m_splineList.end(), const_cast<const Map*>(&m_splineMap));
    }

    PropertySet const& defaultProperties() const {
        return m_defaultProps;
    }

    void setDefaultProperties(PropertySet const& props);

    void addZone(EditableSpline::Ptr const& spline);

    void addZone(EditableSpline::Ptr const& spline, PropertySet const& props);

    void removeZone(EditableSpline::Ptr const& spline);

    void commit();

    intrusive_ptr<PropertySet> propertiesFor(EditableSpline::Ptr const& spline);

    intrusive_ptr<PropertySet const> propertiesFor(EditableSpline::Ptr const& spline) const;

signals:

    void committed();

private:
    Map m_splineMap;
    std::list<EditableSpline::Ptr> m_splineList;
    PropertySet m_defaultProps;
};


namespace boost {
    namespace foreach {
// Make BOOST_FOREACH work with the above class (necessary for boost >= 1.46 with gcc >= 4.6)
        template<>
        struct is_noncopyable<EditableZoneSet> : public boost::mpl::true_ {
        };
    }  // namespace foreach
}  // namespace boost
#endif // ifndef EDITABLE_ZONE_SET_H_
