/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_tziter_h
#define uical_tziter_h

namespace uICAL {
    class VEvent;
    class VComponent;
    class VObjectStream;
    class DateTime;
    class RRuleIter;

    class TZIter {
        public:
            TZIter(const VObject_ptr& timezone);
            seconds_t toUTC(seconds_t timestamp);
            seconds_tz_t fromUTC(seconds_t timestamp);

            typedef struct {
                RRuleIter_ptr rr;
                TZ_ptr from;
                TZ_ptr to;
                string name;
            } tzcomp_t;

            typedef struct {
                seconds_t utc;
                seconds_t local;
                seconds_t offset;
                string name;
            } transitions_t;
            

        protected:
            VObject_ptr timezone;
            
            
            std::vector<tzcomp_t> components;
            std::vector<transitions_t> transitions;

            void init();
            bool next();
            tzcomp_t parse_stuff(const VObject_ptr& obj, bool standard_time);
            
    };

    bool operator < (const TZIter::tzcomp_t  a, const TZIter::tzcomp_t b);
}
#endif
