/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"
#include "uICAL/dateperiod.h"
#include "uICAL/datetime.h"
#include "uICAL/rrule.h"
#include "uICAL/vevent.h"
#include "uICAL/vline.h"
#include "uICAL/vobject.h"
#include "uICAL/error.h"

namespace uICAL {
    VEvent::VEvent(const VObject_ptr& obj, const TZMap_ptr& tzmap, const string& default_tz) {

        VLine_ptr dtStart = obj->getPropertyByName("DTSTART");
        VLine_ptr dtEnd = obj->getPropertyByName("DTEND");
        VLine_ptr rRule = obj->getPropertyByName("RRULE");
        VLine_ptr summary = obj->getPropertyByName("SUMMARY");

        if (dtStart == nullptr) { throw ImplementationError("all events must have a DTSTART"); }
        if (summary == nullptr) { throw ImplementationError("all events must have a SUMMARY"); }

        if (dtStart->getParam("VALUE") == "DATE") {
            this->start = DateTime(dtStart->value + "T000000" +  dtStart->getParam("TZID"), tzmap, default_tz);
            if (dtEnd == nullptr) {
                DatePeriod oneday(86400);
                this->end = this->start + oneday;
            } else {
                this->end = DateTime(dtEnd->value + "T000000" + dtStart->getParam("TZID"), tzmap, default_tz);
            }
        } else {
            this->start = DateTime(dtStart->value + dtStart->getParam("TZID"), tzmap, default_tz);
            if (dtEnd == nullptr) {
                DatePeriod oneday(86400);
                this->end = this->start + oneday;
            } else {
                this->end = DateTime(dtEnd->value + dtStart->getParam("TZID"), tzmap, default_tz);
            }
        }

        this->summary = summary->value;

        if (rRule != nullptr) {
            this->rrule = new_ptr<RRule>(rRule->value, this->start, tzmap, default_tz);
        } else {
            this->rrule = new_ptr<RRule>("", this->start, tzmap, default_tz);
        }

        auto rdates = obj->getPropertiesByName("RDATE");
        for (auto date : rdates) {
            this->rrule->include(DateTime(date->value + date->getParam("TZID"), tzmap, default_tz));
        }

        auto exdates = obj->getPropertiesByName("EXDATE");
        for (auto date : exdates) {
            this->rrule->exclude(DateTime(date->value + date->getParam("TZID"), tzmap, default_tz));
        }
    }

    void VEvent::str(ostream& out) const {
        out << "VEVENT: " << this->summary << uICAL::endl;
        out << " - start: " << this->start << uICAL::endl;
        out << " - end: " << this->end << uICAL::endl;
        out << " - rrule: " << this->rrule << uICAL::endl;
    }
}
