/*
 * State machine: StopWatch
 * States: Active -> Running, Stopped
 * Events: EvStartStop, EvReset
 * State-local storage: Running and Stopped need elapsed_time_. Running needs
 *                      start_time_ to update the elapsed_time_.
 * 
 * To get state information from the state machine, we can do one of two things:
 *   1) Define a virtual base class from which every state state inherits and
 *      use state_cast.
 *   2) Define the act of retrieving information as an event. Each state now
 *      has more than one events; So we need mpl::list. We need to add a method
 *      sc::result react(const Event& )
 */

#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>
#include <ctime>

namespace sc = boost::statechart;

// Declaring events
class EvStartstop : public sc::event<EvStartstop> {};
class EvReset : public sc::event<EvReset> {};

// We need to forward-declare the first state since the 
// state machine declaration should specify the initial state
// and all states need the corresponding state machine.
class Active;
class StopWatch : public sc::state_machine<StopWatch, Active> {};

class Stopped;
// Third parameter is the inner initial state. We can have a 
// hierarchy of states. This hierarchy helps in minimizing code duplicity
// across different states.
class Active : public sc::simple_state<Active, StopWatch, Stopped> {
public:
  Active() : elapsed_time_(0.0) {}
  double ElapsedTime() const { return elapsed_time_; }
  double &ElapsedTime() { return elapsed_time_; }
  // EvReset now applies to both states, since Stopped is the initial state
  // that is where we end up when we get EvReset event.
  typedef sc::transition<EvReset, Active> reactions;
private:
  double elapsed_time_;
};

// Inner states specify Active as their context.
class Running : public sc::simple_state<Running, Active> {
public:
  typedef sc::transition<EvStartstop, Stopped> reactions;
  // This gets run when entering the state.
  // Initialize start_time_ with current time.
  Running() : start_time_(std::time(0)) {}
  // This gets run when exiting the state.
  ~Running() {
    // context() gives access to outer state or the state machine itself.
    context<Active>().ElapsedTime() += std::difftime(std::time(0), start_time_);
  }

private:
  std::time_t start_time_;
};

// Hence the forward declaration
class Stopped : public sc::simple_state<Stopped, Active> {
public:
  typedef sc::transition<EvStartstop, Running> reactions;
};

int main() {
  StopWatch my_watch;
  my_watch.initiate();
  my_watch.process_event(EvStartstop());
  my_watch.process_event(EvStartstop());
  my_watch.process_event(EvStartstop());
  my_watch.process_event(EvReset());
  return 0;
}
