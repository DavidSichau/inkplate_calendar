/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"
#include "uICAL/error.h"
#include "uICAL/datetime.h"
#include "uICAL/datestamp.h"
#include "uICAL/rrule.h"
#include "uICAL/rruleiter.h"
#include "uICAL/tz.h"
#include "uICAL/tziter.h"
#include "uICAL/vobject.h"
#include "uICAL/vline.h"

namespace uICAL {
    TZIter::TZIter(const VObject_ptr& timezone)
    {
        this->timezone = timezone;
        
    }

    seconds_t TZIter::toUTC(seconds_t timestamp) {
        if (this->timezone) { this->init(); }
        
        while (timestamp > this->transitions.back().local && this->next())
            {}
        
        auto i = this->transitions.begin();
        while (1) {
            if (i + 1 == this->transitions.end() || timestamp <= (i + 1)->local) {
                return timestamp - i->offset;
            }
            ++i;
        }
    }

    seconds_tz_t TZIter::fromUTC(seconds_t timestamp) {
        if (this->timezone) { this->init(); }
        
        while (timestamp > this->transitions.back().utc && this->next()) { }
        auto i = this->transitions.begin();
        while (1) {
            if (i + 1 == this->transitions.end() || timestamp <= (i + 1)->utc) {
                return seconds_tz_t(timestamp + i->offset, i->name);
            }
            ++i;
        }
    }

    void TZIter::init() {
        auto objs = this->timezone->listObjects("STANDARD");
        for (auto obj: objs) {
            components.push_back(parse_stuff(obj, true));
        }
        objs = this->timezone->listObjects("DAYLIGHT");
        for (auto obj: objs) {
            components.push_back(parse_stuff(obj, false));
        }
        this->timezone = NULL;
        
        this->next();
    }

    TZIter::tzcomp_t TZIter::parse_stuff(const VObject_ptr& obj, bool standard_time) {
        VLine_ptr dtStart = obj->getPropertyByName("DTSTART");
        VLine_ptr offsetFrom = obj->getPropertyByName("TZOFFSETFROM");
        VLine_ptr offsetTo = obj->getPropertyByName("TZOFFSETTO");
        VLine_ptr tzName = obj->getPropertyByName("TZNAME");
        RRule_ptr rrule;

        VLine_ptr rRule = obj->getPropertyByName("RRULE");
        DateStamp ds(dtStart->value);
        TZ_ptr tzFrom, tzTo;

        tzFrom = new_ptr<TZ>(offsetFrom->value);
        tzTo = new_ptr<TZ>(offsetTo->value);
        
        auto start = DateTime(ds, tzFrom);
        if (rRule != nullptr) {
            rrule = new_ptr<RRule>(rRule->value, start, nullptr, "");
        } else {
            rrule = new_ptr<RRule>("", start, nullptr, "");
        }
        
        auto rdates = obj->getPropertiesByName("RDATE");
        for (auto date : rdates) {
            DateStamp d(date->value);
            rrule->include(DateTime(d, tzFrom));
        }

        auto exdates = obj->getPropertiesByName("EXDATE");
        for (auto date : exdates) {
            DateStamp d(date->value);
            rrule->exclude(DateTime(d, tzFrom));
        }
        auto iter = new_ptr<RRuleIter>(rrule, DateTime(), DateTime());
        iter->next();
        if (!iter->now().valid()) {
            throw ImplementationError("Now is invalid");
        }
        
        TZIter::tzcomp_t ret;
        if (tzName != nullptr) {
            ret = {iter, tzFrom, tzTo, tzName->value};
        } else if (standard_time) {
            ret = {iter, tzFrom, tzTo, "STANDARD"};
        } else {
            ret = {iter, tzFrom, tzTo, "DAYLIGHT"};
        }

        return ret;
    }

    bool TZIter::next() {
        if (this->components.size() == 0) {
            return false;
        }
        
        auto i = std::min_element(this->components.begin(), this->components.end());
        TZIter::transitions_t t = {i->rr->now().seconds(), i->rr->now().seconds(), i->to->offsetSecs, i->name};
        this->transitions.push_back(t);
        if (!i->rr->next()) {
            this->components.erase(i);
        }
        return true;
    }

    bool operator < (const TZIter::tzcomp_t a, const TZIter::tzcomp_t b) {
        return a.rr->now() < b.rr->now();
    }
    

}
