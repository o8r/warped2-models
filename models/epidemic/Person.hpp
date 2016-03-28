#ifndef PERSON_HPP
#define PERSON_HPP

#include "serialization.hpp"
#include "cereal/types/map.hpp"

enum infection_state_t {

    UNINFECTED = 0,
    LATENT,
    INCUBATING,
    INFECTIOUS,
    ASYMPT,
    RECOVERED,
    MAX_INFECTION_STATE_NUM
};

class Person {
public:

    Person(unsigned long pid, double susceptibility, bool vacc_status, 
                infection_state_t infection_state, unsigned int arrival_timestamp, 
                unsigned int prev_state_change_timestamp)
        : pid_(pid), susceptibility_(susceptibility), vaccination_status_(vacc_status),
            infection_state_(infection_state), loc_arrival_timestamp_(arrival_timestamp),
            prev_state_change_timestamp_(prev_state_change_timestamp) {}

    unsigned long pid_;
    double susceptibility_;
    bool vaccination_status_;
    infection_state_t infection_state_;
    unsigned int loc_arrival_timestamp_;
    unsigned int prev_state_change_timestamp_;

    WARPED_REGISTER_SERIALIZABLE_MEMBERS(pid_, susceptibility_, vaccination_status_,
					 infection_state_, loc_arrival_timestamp_,
					 prev_state_change_timestamp_)

    template <typename Archive>
    static void load_and_construct(Archive& ar, cereal::construct<Person>& construct) {
      unsigned long pid;
      double susceptibility;
      bool vacc_status;
      infection_state_t infection_state;
      unsigned int arrival_timestamp;
      unsigned int prev_state_change_timestamp;

      ar(pid, susceptibility, vacc_status, infection_state, arrival_timestamp, prev_state_change_timestamp);
      construct(pid, susceptibility, vacc_status, infection_state, arrival_timestamp, prev_state_change_timestamp);
    }
};

#endif
