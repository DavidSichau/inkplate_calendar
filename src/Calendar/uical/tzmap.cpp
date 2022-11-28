/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/tzmap.h"
#include "uICAL/tz.h"
#include "uICAL/vline.h"
#include "uICAL/vobject.h"
#include "uICAL/error.h"

namespace uICAL {
    TZMap::TZMap() {
    }

    void TZMap::add(const VObject_ptr& timezone) {
        string tzId = timezone->getPropertyByName("TZID")->value;

        auto standards = timezone->listObjects("STANDARD");
        for (auto standard : standards) {

            VLine_ptr tmp;
            string offset, name;

            tmp = standard->getPropertyByName("TZOFFSETFROM");
            if (tmp != nullptr) {
                offset = tmp->value;
            } else {
                throw ImplementationError("all timezones must have TZOFFSETFROM");
            }

            tmp = standard->getPropertyByName("TZNAME");
            if (tmp != nullptr) {
                name = tmp->value;
            }

            this->add(tzId, name, timezone);
        }
    }

    void TZMap::add(const string& id, const string& name, const VObject_ptr& timezone) {
        this->id_attrib_map[id].tziter = new_ptr<TZIter>(timezone);
        this->id_attrib_map[id].name = name;
    }

    string TZMap::findId(const string& nameOrId) const {
        for (auto i : this->id_attrib_map) {
            if (i.second.name == nameOrId || i.first == nameOrId) {
                return i.first;
            }
        }

        return string();
    }

    seconds_t TZMap::toUTC(const string& id, seconds_t timestamp) {
        return this->id_attrib_map[id].tziter->toUTC(timestamp);
    }

    seconds_tz_t TZMap::fromUTC(const string& id, seconds_t timestamp) {
        return this->id_attrib_map[id].tziter->fromUTC(timestamp);
    }

    string TZMap::getName(const string& tzId) {
        return tzId;
    }

    void TZMap::str(ostream& out) const {
        for (auto i : this->id_attrib_map) {
            out << i.first << " : " << i.second.name << " : ";
            TZ::offsetAsString(out, -i.second.tziter->toUTC(0));
            out << uICAL::endl;
        }
    }
}
