// This model was ported from the ROSS pcs model. See https://github.com/carothersc/ROSS
// Also see "Distributed Simulation of Large-Scale PCS Networks" by Carothers et al.

#ifndef PCS_SIM_HPP_DEFINED
#define PCS_SIM_HPP_DEFINED

#include <string>
#include <vector>
#include <memory>

#include "warped.hpp"

#include "MLCG.h"
#include "NegExp.h"

// These may be moved to config file or command line later
#define NUM_CELLS_X 1024     //256
#define NUM_CELLS_Y 1024     //256

#define NUM_VP_X 512 
#define NUM_VP_Y 512

#define MAX_NORMAL_CHANNELS 10
#define MAX_RESERVE_CHANNELS 0

/*
 * This is in Mins 
 */
#define MOVE_CALL_MEAN 4500.0
#define NEXT_CALL_MEAN 360.0
#define CALL_TIME_MEAN 180.0

/*
 * When Normal_Channels == 0, then all have been used 
 */
#define NORM_CH_BUSY ( !( SV->Normal_Channels & 0xffffffff ) )

/*
 * When Reserve_Channels == 0, then all have been used 
 */
#define RESERVE_CH_BUSY ( !( SV->Reserve_Channels & 0xffffffff ) )

WARPED_DEFINE_OBJECT_STATE_STRUCT(PcsState) {
  double          const_state_1;
  int             const_state_2;
  // int             normal_channels;
  // int             reserve_channels;
  int             portables_in;
  int             portables_out;
  int             call_attempts;
  int             channel_blocks;
  int             busy_lines;
  int             handoff_blocks;
  int             cell_location_x;
  int             cell_location_y;
};

enum method_name_t {
  NEXTCALL_METHOD,
  COMPLETIONCALL_METHOD,
  MOVECALLIN_METHOD,
  MOVECALLOUT_METHOD
};

enum channel_t {
  NONE,
  NORMAL,
  RESERVE
};

enum min_t {
  COMPLETECALL,
  NEXTCALL,
  MOVECALL
};

enum direction_t {
  LEFT,
  RIGHT,
  DOWN,
  UP
};

class PcsEvent : public warped::Event {
public:
  PcsEvent();
  PcsEvent(const std::string& receiver_name, const unsigned int completion_timestamp,
           const unsigned int next_timestamp, const unsigned int move_timestamp,
           const std::string& creator_name, channel_t channel, method_name_t method_name)
    : receiver_name(receiver_name), next_timestamp(next_timestamp),
      move_timestamp(move_timestamp), completion_timestamp(completion_timestamp),
      creator_name(creator_name), channel(channel), method_name(method_name) {}
  PcsEvent(const PcsEvent& other)
    : Event((const warped::Event&)other),
      receiver_name(other.receiver_name), next_timestamp(other.next_timestamp),
      move_timestamp(other.move_timestamp), completion_timestamp(other.completion_timestamp),
      creator_name(other.creator_name), channel(other.channel),
      method_name(other.method_name) {}

protected:
  std::string receiver_name;
  std::string creator_name;
  unsigned int completion_timestamp;
  unsigned int next_timestamp;
  unsigned int move_timestamp;
  channel_t channel;
  method_name_t method_name;

  WARPED_REGISTER_SERIALIZABLE_MEMBERS(creator_name, receiever_name, timestamp, channel,
                                       method_name)
};

class PcsCell : public warped::SimulationObject {
public:
  PcsCell(const std::string& name, const unsigned int num_cells, const unsigned int num_portables,
          const unsigned int normal_channels, const unsigned int reserve_channels,
          const bool reserve_cell = false)
    : SimulationObject(name), num_cells(num_cells), num_portables(num_portables),
      normal_channels(normal_channels), reserve_channels(reserve_channels),
      // portables_in(0), portables_out(0), call_attempts(0), channel_blocks(0), busy_lines(0),
      // handoff_blocks(0), call_attempts(0)
      rng(new MLCG), reserve_cell(reserve_cell), state()
  {
    move_expo(MOVE_CALL_MEAN, this->rng.get());
    next_expo(NEXT_CALL_MEAN, this->rng.get());
    time_expo(CALL_TIME_MEAN, this->rng.get());
  }

  double blocking_probability() {
    return ((double)(state.channel_blocks + statehandoff_blocks)) / ((double) (state.call_attempts - state.busy_lines));
  }

  std::vector<std::unique_ptr<warped::Event> > createInitialEvents();
  std::vector<std::unique_ptr<warped::Event> > receieveEvent(const warped::Event&);

  warped::ObjectState& getState() { return this->state; }

protected:
  std::string name;
  unsigned int num_cells;
  unsigned int num_portables;
  unsigned int normal_channels;
  unsigned int reserve_channels;
  std::unique_ptr<MLCG> rng;
  NegativeExpntl move_expo;
  NegativeExpntl next_expo;
  NegativeExpntl time_expo;
  bool reserve_cell;

private:
  PcsState state;
};

#endif