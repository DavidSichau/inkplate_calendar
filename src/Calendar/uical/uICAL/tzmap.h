/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_tzinfo_h
#define uical_tzinfo_h

#include "Calendar/uical/uICAL/base.h"
#include "Calendar/uical/uICAL/tziter.h"

namespace uICAL {
    class VComponent;

    class TZMap : public Base {
        public:
            TZMap();

            void add(const VObject_ptr& timezone);

            string findId(const string& nameOrId) const;

            seconds_t toUTC(const string& tzId, seconds_t timestamp);
            seconds_tz_t fromUTC(const string& tzId, seconds_t timestamp);

            string getName(const string& tzId);

            void str(ostream& out) const;

        protected:
            void add(const string& id, const string& name, const VObject_ptr& tz);
            int parseOffset(const string& offset) const;

            typedef struct {
                TZIter_ptr tziter;
                string name;
            } attribs_t;

            std::map<string, attribs_t> id_attrib_map;
    };
}
#endif
