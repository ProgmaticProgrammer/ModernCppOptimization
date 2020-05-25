/*
 * mqtt_detail.hpp
 *
 *  Created on: 23/05/2020
 *      Author: Owner
 */

#ifndef MQTT_DETAIL_HPP_
#define MQTT_DETAIL_HPP_

#include <iostream>
// back-end
#include <boost/msm/back/state_machine.hpp>
// front-end
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#define BOOST_NO_EXCEPTIONS
#include <boost/throw_exception.hpp>
namespace boost {
void throw_exception(std::exception const&) {
  while (1)
    ;
}
}  // namespace boost

namespace msm = boost::msm;
namespace mpl = boost::mpl;
namespace front = boost::msm::front;

namespace mqtt {
namespace detail {

struct client_interface {
  virtual ~client_interface() = default;

  virtual void send_to_broker(uint8_t const* data, uint16_t length) = 0;
  virtual void receive_from_broker(/*...*/) = 0;
  virtual void queue_task(/*...*/) = 0;
  virtual void update_connection_status(/*...*/) = 0;
  template <typename Packet>
  void send(Packet const& packet) {
    auto buffer = serialize(packet);
    send_to_broker(pointer(buffer), size(buffer));
  }
};

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

enum class connection_status_t : uint8_t {
  DISCONNECTED = 0x00,
  CONNECTED = 0x01,
  DISCONNECTING = 0x02,
  FAILURE = 0x80,
};
//serializable
struct Packet {};

struct connect {};
struct publish_out {
  Packet publish;
};
struct subscribe {
  Packet subscribe;
};
struct unsubscribe {
  Packet unsubscribe;
};
struct disconnect {};
struct shutdown_timeout {};
struct none {};

struct client_ : public msm::front::state_machine_def<client_> {};

template <typename Derived>
struct machine_base : public msm::front::state_machine_def<Derived> {
  client_interface* client_ = nullptr;
};

struct client_machine_ : public machine_base<client_machine_> {
  struct NotConnected : public State {
    template <class Event, class FSM>
    void on_entry(Event const&, FSM& fsm) {
      fsm.client_->update_connection_status(connection_status_t::DISCONNECTED);
    }
  };

  struct Connected : public State {
    template <class Event, class FSM>
    void on_entry(Event const&, FSM& fsm) {
      fsm.client_->update_connection_status(connection_status_t::CONNECTED);
    }
    template <class Event, class FSM>
    void on_exit(Event const&, FSM& fsm) {
      fsm.client_->update_connection_status(connection_status_t::DISCONNECTING);
    }
  };
  struct ConnectBroker : public State {};

  struct ShuttingDown : public State {
    // defer connect events until the next state
    using deferred_events = mpl::vector<connect>;
  };
  struct None : public State {};

  struct send_packet {
    template <class Fsm, class SourceState, class TargetState>
    void operator()(publish_out const& evt, Fsm& fsm, SourceState&,
                    TargetState&) {
      fsm.client_->send(evt.publish);
    }
    template <class Fsm, class SourceState, class TargetState>
    void operator()(subscribe const& evt, Fsm& fsm, SourceState&,
                    TargetState&) {
      fsm.client_->send(evt.subscribe);
    }
    template <class Fsm, class SourceState, class TargetState>
    void operator()(unsubscribe const& evt, Fsm& fsm, SourceState&,
                    TargetState&) {
      fsm.client_->send(evt.unsubscribe);
    }
  };
  using none = msm::front::none;
  struct transition_table
      : TransitionTable<
            // Start Event Next Action Guard
            // +------------------+---------------------------+------------------+----------------+----------
            front::Row<NotConnected, connect, ConnectBroker, none, none>,
            front::Row<ConnectBroker, none, Connected, none, none>,
            front::Row<Connected, publish_out, none, send_packet, none>,
            front::Row<Connected, subscribe, none, send_packet, none>,
            front::Row<Connected, unsubscribe, none, send_packet, none>,
            front::Row<Connected, connect, none, none, none>,
            front::Row<Connected, disconnect, ShuttingDown, none, none>,
            front::Row<ShuttingDown, shutdown_timeout, NotConnected, none, none>
            // +------------------+---------------------------+------------------+----------------+----------
            > {};
  template <typename... T>
  using Submachines = meta::meta_list<T...>;

  using submachines = Submachines<ConnectBroker>;
  using initial_state = NotConnected;
};
using client_machine = msm::back::state_machine<client_machine_>;

}  // namespace detail
}  // namespace mqtt

#endif /* MQTT_DETAIL_HPP_ */
