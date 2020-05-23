/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "diag/Trace.h"
#include "Timer.h"

// ----------------------------------------------------------------------------
//
// Print a greeting message on the trace device and enter a loop
// to count seconds.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//
// ----------------------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main (int argc, char* argv[])
{
  // Normally at this stage most of the microcontroller subsystems, including
  // the clock, were initialised by the CMSIS SystemInit() function invoked
  // from the startup file, before calling main().
  // (see system/src/cortexm/_initialize_hardware.c)
  // If further initialisations are required, customise __initialize_hardware()
  // or add the additional initialisation here, for example:
  //
  // HAL_Init();

  // In this sample the SystemInit() function is just a placeholder,
  // if you do not add the real one, the clock will remain configured with
  // the reset value, usually a relatively low speed RC clock (8-12MHz).

  // Show the program parameters (passed via semihosting).
  // Output is via the semihosting output channel.
  trace_dump_args(argc, argv);

  // Send a greeting to the trace device (skipped on Release).
  trace_puts("Hello ARM World!");

  // Send a message to the standard output.
  puts("Standard output message.");

  // Send a message to the standard error.
  fprintf(stderr, "Standard error message.\n");

  // At this stage the system clock should have already been configured
  // at high speed.
  trace_printf("System clock: %u Hz\n", SystemCoreClock);

  Timer timer;
  timer.start();

  int seconds = 0;

#define LOOP_COUNT (5)
  int loops = LOOP_COUNT;
  if (argc > 1)
    {
      // If defined, get the number of loops from the command line,
      // configurable via semihosting.
      loops = atoi (argv[1]);
    }

  // Short loop.
  for (int i = 0; i < loops; i++)
    {
      timer.sleep(Timer::FREQUENCY_HZ);

      ++seconds;

      // Count seconds on the trace device.
      trace_printf ("Second %d\n", seconds);
    }
  return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
*/


#include <iostream>

#define BOOST_NO_EXCEPTIONS
#include <boost/throw_exception.hpp>
void boost::throw_exception(std::exception const & e){
while(1);
}

// back-end
#include <boost/msm/back/state_machine.hpp>
// front-end
#include <boost/msm/front/state_machine_def.hpp>

namespace msm = boost::msm;
namespace mpl = boost::mpl;

namespace fsm {

///////////////////////////////////////////////////
namespace events {

struct play {};
struct end_pause {};
struct stop {};
struct pause {};
struct open_close {};

// A "complicated" event type that carries some data.
enum class DiskType { DISK_CD = 0, DISK_DVD = 1 };
struct cd_detected {
  cd_detected(std::string name, DiskType diskType)
      : name(name), disc_type(diskType) {}

  std::string name;
  DiskType disc_type;
};

}  // namespace events

///////////////////////////////////////////////////
namespace front_end {

using DiskType = events::DiskType;
// define the FSM structure
struct player_ : public msm::front::state_machine_def<player_> {
  template <class Event, class FSM>
  void on_entry(Event const&, FSM&) {
    std::cout << "entering: Player" << std::endl;
  }
  template <class Event, class FSM>
  void on_exit(Event const&, FSM&) {
    std::cout << "leaving: Player" << std::endl;
  }

  // The list of FSM states
  struct Empty : public msm::front::state<> {
    // every (optional) entry/exit methods get the event passed.
    template <class Event, class FSM>
    void on_entry(Event const&, FSM&) {
      std::cout << "entering: Empty" << std::endl;
    }
    template <class Event, class FSM>
    void on_exit(Event const&, FSM&) {
      std::cout << "leaving: Empty" << std::endl;
    }
  };
  struct Open : public msm::front::state<> {
    template <class Event, class FSM>
    void on_entry(Event const&, FSM&) {
      std::cout << "entering: Open" << std::endl;
    }
    template <class Event, class FSM>
    void on_exit(Event const&, FSM&) {
      std::cout << "leaving: Open" << std::endl;
    }
  };

  // sm_ptr still supported but deprecated as functors are a much better way to
  // do the same thing
  struct Stopped : public msm::front::state<msm::front::default_base_state,
                                            msm::front::sm_ptr> {
    template <class Event, class FSM>
    void on_entry(Event const&, FSM&) {
      std::cout << "entering: Stopped" << std::endl;
    }
    template <class Event, class FSM>
    void on_exit(Event const&, FSM&) {
      std::cout << "leaving: Stopped" << std::endl;
    }
    void set_sm_ptr(player_* pl) { m_player = pl; }
    player_* m_player;
  };

  struct Playing : public msm::front::state<> {
    template <class Event, class FSM>
    void on_entry(Event const&, FSM&) {
      std::cout << "entering: Playing" << std::endl;
    }
    template <class Event, class FSM>
    void on_exit(Event const&, FSM&) {
      std::cout << "leaving: Playing" << std::endl;
    }
  };

  // state not defining any entry or exit
  struct Paused : public msm::front::state<> {};

  // the initial state of the player SM. Must be defined
  typedef Empty initial_state;

  // transition actions
  void start_playback(events::play const&) {
    std::cout << "player::start_playback\n";
  }
  void open_drawer(events::open_close const&) {
    std::cout << "player::open_drawer\n";
  }
  void close_drawer(events::open_close const&) {
    std::cout << "player::close_drawer\n";
  }
  void store_cd_info(events::cd_detected const&) {
    std::cout << "player::store_cd_info\n";
  }
  void stop_playback(events::stop const&) {
    std::cout << "player::stop_playback\n";
  }
  void pause_playback(events::pause const&) {
    std::cout << "player::pause_playback\n";
  }
  void resume_playback(events::end_pause const&) {
    std::cout << "player::resume_playback\n";
  }
  void stop_and_open(events::open_close const&) {
    std::cout << "player::stop_and_open\n";
  }
  void stopped_again(events::stop const&) {
    std::cout << "player::stopped_again\n";
  }
  // guard conditions
  bool good_disk_format(events::cd_detected const& evt) {
    // to test a guard condition, let's say we understand only CDs, not DVD
    if (evt.disc_type != DiskType::DISK_CD) {
      std::cout << "wrong disk, sorry" << std::endl;
      return false;
    }
    return true;
  }
  // used to show a transition conflict. This guard will simply deactivate one
  // transition and thus solve the conflict
  bool auto_start(events::cd_detected const&) { return false; }

  typedef player_ p;  // makes transition table cleaner

  // Transition table for player
  struct transition_table
      : mpl::vector<
            //    Start     Event        Target      Action Guard
            //   +---------+------------+-----------+---------------------------+----------------------------+
            a_row<Stopped, events::play, Playing, &player_::start_playback>,
            a_row<Stopped, events::open_close, Open, &player_::open_drawer>,
            _row<Stopped, events::stop, Stopped>,
            //   +---------+------------+-----------+---------------------------+----------------------------+
            a_row<Open, events::open_close, Empty, &player_::close_drawer>,
            //   +---------+------------+-----------+---------------------------+----------------------------+
            a_row<Empty, events::open_close, Open, &player_::open_drawer>,
            row<Empty, events::cd_detected, Stopped, &player_::store_cd_info,
                &player_::good_disk_format>,
            row<Empty, events::cd_detected, Playing, &player_::store_cd_info,
                &player_::auto_start>,
            //   +---------+------------+-----------+---------------------------+----------------------------+
            a_row<Playing, events::stop, Stopped, &player_::stop_playback>,
            a_row<Playing, events::pause, Paused, &player_::pause_playback>,
            a_row<Playing, events::open_close, Open, &player_::stop_and_open>,
            //   +---------+------------+-----------+---------------------------+----------------------------+
            a_row<Paused, events::end_pause, Playing,
                  &player_::resume_playback>,
            a_row<Paused, events::stop, Stopped, &player_::stop_playback>,
            a_row<Paused, events::open_close, Open, &player_::stop_and_open>
            //   +---------+------------+-----------+---------------------------+----------------------------+
            > {};
  // Replaces the default no-transition response.
  template <class FSM, class Event>
  void no_transition(Event const& e, FSM&, int state) {
    std::cout << "no transition from state " << state << " on event " << std::endl;
              /*<< typeid(e).name()*/
  }
};

}  // namespace front_end
///////////////////////////////////////////////////
// back-end
using player_ = fsm::front_end::player_;
typedef msm::back::state_machine<player_> player;
}  // namespace fsm

///////////////////////////////////////////////////
//
// Testing utilities.
//
namespace events = fsm::events;
using player = fsm::player;
using DiskType = fsm::events::DiskType;

static char const* const state_names[] = {"Stopped", "Open", "Empty", "Playing",
                                          "Paused"};
void pstate(player const& p) {
  std::cout << " -> " << state_names[p.current_state()[0]] << std::endl;
}

void test() {
  player p;
  // needed to start the highest-level SM. This will call on_entry and mark the
  // start of the SM
  p.start();
  // go to Open, call on_exit on Empty, then action, then on_entry on Open
  p.process_event(events::open_close{});
  pstate(p);
  p.process_event(events::open_close{});
  pstate(p);
  // will be rejected, wrong disk type
  p.process_event(events::cd_detected{"louie, louie", DiskType::DISK_DVD});
  pstate(p);
  p.process_event(events::cd_detected{"louie, louie", DiskType::DISK_CD});
  pstate(p);
  p.process_event(events::play{});

  // at this point, Play is active
  p.process_event(events::pause{});
  pstate(p);
  // go back to Playing
  p.process_event(events::end_pause{});
  pstate(p);
  p.process_event(events::pause{});
  pstate(p);
  p.process_event(events::stop{});
  pstate(p);
  // event leading to the same state
  // no action method called as it is not present in the transition table
  p.process_event(events::stop{});
  pstate(p);
  std::cout << "stop fsm" << std::endl;
  p.stop();
}
#ifdef __TM4C123GH6PM__
#include <Arduino.h>

void setup() {
  // initialize LED digital pin as an output.
  pinMode(RED_LED, OUTPUT);
}

void loop() {
  // turn the LED on (HIGH is the voltage level)
  digitalWrite(RED_LED, HIGH);
  // wait for a second
  delay(1000);
  // turn the LED off by making the voltage LOW
  digitalWrite(RED_LED, LOW);
  // wait for a second
  delay(1000);
}
#else
int main() {
  test();
  return 0;
}
#endif  // PART_TM4C123GH6PM

