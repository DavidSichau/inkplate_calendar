/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_vevent_h
#define uical_vevent_h

#include "Calendar/uical/uICAL/base.h"
#include "Calendar/uical/uICAL/datetime.h"

namespace uICAL {
    class VObject;

    class VEvent : public Base {
        public:
            VEvent(const VObject_ptr& event, const TZMap_ptr& tzmap, const string& default_tz);

            void str(ostream& out) const;

            string summary;
            DateTime start;
            DateTime end;
            RRule_ptr rrule;

            friend class VEventIter;
    };
}
#endif
