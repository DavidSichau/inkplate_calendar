/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/tz.h"
#include "uICAL/datestamp.h"
#include "uICAL/epochtime.h"
#include "uICAL/tzmap.h"

namespace uICAL {
    static TZ_ptr UNDEF = new_ptr<TZ>();
    static TZ_ptr UNAWARE = new_ptr<TZ>(false);

    TZ_ptr TZ::undef() {
        return UNDEF;
    }

    TZ_ptr TZ::unaware() {
        return UNAWARE;
    }

    TZ::TZ() {
        this->offsetSecs = -1;
        this->aware = false;
    }

    TZ::TZ(bool aware) {
        this->offsetSecs = 0;
        this->aware = aware;
    }

    TZ::TZ(const string& tz) {
        this->offsetSecs = TZ::parseOffsetSecs(tz);
        this->aware = true;
    }

    TZ::TZ(const string& tz, const TZMap_ptr& tzmap)
    : idmap(tzmap)
    {
        string id = "";
        if (tzmap != NULL) {
            id = this->idmap->findId(tz);
        }
        if (!id.empty()) {
            this->id = id;
            this->aware = true;
        }
        else {
            this->offsetSecs = TZ::parseOffsetSecs(tz);
            this->aware = true;
        }
    }

    bool TZ::is_aware() const {
        return this->aware;
    }

    int TZ::parseOffsetSecs(const string& tz) {
        if (tz == "Z") {
            return 0;
        }
        try {
            if (tz.length() == 5 || tz.length() == 7) {
                char sign;
                unsigned tzH, tzM, tzS;

                // e.g.: +0200
                sign = tz.at(0);
                tzH = tz.substr(1, 2).as_int();
                tzM = tz.substr(3, 2).as_int();
                if (tz.length() == 7) {
                    tzS = tz.substr(5, 2).as_int();
                } else {
                    tzS = 0;
                }

                int offset = ((tzH * 60) + tzM) * 60 + tzS;
                if (sign == '-') {
                    offset *= -1;
                }
                return offset;
            }
        }
        catch (std::invalid_argument const &e)
        {}
        catch (std::out_of_range const &e)
        {}
        throw ValueError("Bad timezone: \"" + tz + "\"");
    }

    void TZ::offsetAsString(ostream& out, int offsetSecs) {
        if (offsetSecs != -1) {
            if (offsetSecs == 0) {
                out << "Z";
            }
            else
            {
                if (offsetSecs < 0) {
                    out << "-";
                    offsetSecs *= -1;
                }
                else
                {
                    out << "+";
                }
                out << string::fmt(fmt_02d, offsetSecs / 60 / 60);
                out << string::fmt(fmt_02d, (offsetSecs / 60)% 60);
                if (offsetSecs  % 60) {
                    out << string::fmt(fmt_02d, offsetSecs % 60);
                }
            }
        }
    }

    seconds_t TZ::toUTC(seconds_t timestamp) const {
        if (!this->id.empty()) {
            return this->idmap->toUTC(this->id, timestamp);
        }
        if (this->offsetSecs == -1)
            throw ImplementationError("Timezone not defined");
        return timestamp - this->offsetSecs;
    }

    seconds_tz_t TZ::fromUTC(seconds_t timestamp) const {
        if (!this->id.empty()) {
            return this->idmap->fromUTC(this->id, timestamp);
        }
        if (this->offsetSecs == -1)
            throw ImplementationError("Timezone not defined");
        return seconds_tz_t(timestamp + this->offsetSecs, this->as_str());
    }

    void TZ::str(ostream& out) const {
        if (!this->id.empty()) {
            out << this->idmap->getName(this->id);
            return;
        }

        if (this->offsetSecs == -1)
            throw ImplementationError("Timezone not defined");
        if (!this->aware)
            return;
        TZ::offsetAsString(out, this->offsetSecs);
    }
}
