/*
 * mqtt_detail.hpp
 *
 *  Created on: 23/05/2020
 *      Author: Owner
 */

#ifndef MQTT_DETAIL_HPP_
#define MQTT_DETAIL_HPP_

#include <iostream>
#include <meta/meta.hpp>

#include "message.h"
// back-end
#include <boost/msm/back/state_machine.hpp>
// front-end
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#define BOOST_NO_EXCEPTIONS
#include <boost/throw_exception.hpp>

using namespace meta;

namespace msm = boost::msm;
namespace mpl = boost::mpl;
namespace front = boost::msm::front;

namespace mqtt {
namespace detail {

template <typename Connection, typename Executor>
struct client_interface_wrapper : client_interface {
  client_interface_wrapper(Connection& broker, Executor& executor)
      : broker_(broker), executor_(executor) {}
  virtual void send_to_broker(uint8_t const* data, uint16_t length) override {
    broker_.send(data, length);
  }

 private:
  Connection& broker_;
  Executor& executor_;
};

template <typename... T>
using TransitionTable = mpl::vector<T...>;

using State = msm::front::state<>;
using DefaultState =
    msm::front::state<msm::front::default_base_state, msm::front::sm_ptr>;


// state machine
template <typename Derived>
struct machine_base : public msm::front::state_machine_def<Derived> {
  client_interface* client_ = nullptr;
};
// front end
struct client_machine_ : public machine_base<client_machine_> {
  template <class Event, class FSM>
  void on_entry(Event const&, FSM&) {
    std::cout << "entering: client_machine" << std::endl;
  }
  template <class Event, class FSM>
  void on_exit(Event const&, FSM&) {
    std::cout << "leaving: client_machine" << std::endl;
  }

  struct NotConnected : public State {
    template <class Event, class FSM>
    void on_entry(Event const&, FSM& fsm) {
      //fsm.client_->update_connection_status(connection_status_t::DISCONNECTED);
    }
  };

  struct Connected : public State {
    template <class Event, class FSM>
    void on_entry(Event const&, FSM& fsm) {
      //fsm.client_->update_connection_status(connection_status_t::CONNECTED);
    }
    template <class Event, class FSM>
    void on_exit(Event const&, FSM& fsm) {
      //fsm.client_->update_connection_status(connection_status_t::DISCONNECTING);
    }
  };
  struct ConnectBroker : public State {};

  struct ShuttingDown : public State {
    // defer connect events until the next state
    using deferred_events = mpl::vector<connect>;
  };
void send_packet(publish_out const& evt){}
void send_packet(subscribe const& evt){}
void send_packet(unsubscribe const& evt){}
  // struct send_packet {
  //   template <class Fsm, class SourceState, class TargetState>
  //   void operator()(publish_out const& evt, Fsm& fsm, SourceState&,
  //                   TargetState&) {
  //     //fsm.client_->send(evt.publish);
  //   }
  //   template <class Fsm, class SourceState, class TargetState>
  //   void operator()(subscribe const& evt, Fsm& fsm, SourceState&,
  //                   TargetState&) {
  //     //fsm.client_->send(evt.subscribe);
  //   }
  //   template <class Fsm, class SourceState, class TargetState>
  //   void operator()(unsubscribe const& evt, Fsm& fsm, SourceState&,
  //                   TargetState&) {
  //     //fsm.client_->send(evt.unsubscribe);
  //   }
  // };
  using none = msm::front::none;
  struct transition_table
      : mpl::vector<
            // Start Event Next Action Guard
            // +------------------+---------------------------+------------------+----------------+----------
            _row<NotConnected, connect, ConnectBroker>,
            _row<ConnectBroker, none, Connected>,
            a_row<Connected, publish_out, Connected, &client_machine_::send_packet>,
            a_row<Connected, subscribe, ConnectBroker, &client_machine_::send_packet>,
            a_row<ConnectBroker, unsubscribe, Connected, &client_machine_::send_packet>,
            _row<Connected, connect, Connected>,
            _row<Connected, disconnect, ShuttingDown>,
            _row<ShuttingDown, shutdown_timeout, NotConnected>
            // +------------------+---------------------------+------------------+----------------+----------
            > {};
  template <typename... T>
  using Submachines = meta::list<T...>;

  using submachines = Submachines<ConnectBroker>;
  using initial_state = NotConnected;
};  // namespace detail
using client_machine = msm::back::state_machine<client_machine_>;

}  // namespace detail
}  // namespace mqtt

#endif /* MQTT_DETAIL_HPP_ */
