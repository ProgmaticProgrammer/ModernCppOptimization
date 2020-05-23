/*
 * mqtt_detail.hpp
 *
 *  Created on: 23/05/2020
 *      Author: Owner
 */

#ifndef MQTT_DETAIL_HPP_
#define MQTT_DETAIL_HPP_

//#include <vector>
#include <boost\mpl\vector.hpp>
#include <boost\msm\back\state_machine.hpp>
#include <boost\msm\front\state_machine_def.hpp>

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
using TransitionTable = boost::mpl::vector<T...>;

// struct State {};
using State = boost::msm::front::state<>;

enum class connection_status_t : uint8_t {
  DISCONNECTED = 0x00,
  CONNECTED = 0x01,
  DISCONNECTING = 0x02,
  FAILURE = 0x80,
};

enum class event : uint8_t {
  connect = 0x00,
  publish_out = 0x01,
  subscribe = 0x02,
  unsubscribe = 0x04,
  disconnect = 0x10,
  shutdown_timeout = 0x20,
};

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
  using deferred_events = boost::mpl::vector<event::connect>;
};

struct transition_table
    : TransitionTable<
          // Start              Event                       Next               Action           Guard
          // +------------------+---------------------------+------------------+----------------+----------
          Row<NotConnected,     event::connect,             ConnectBroker,     none,            none>,
          Row<ConnectBroker,    none,                       Connected,         none,            none>,
          Row<Connected,        event::publish_out,         none,              send_packet,     none>,
          Row<Connected,        event::subscribe,           none,              send_packet,     none>,
          Row<Connected,        event::unsubscribe,         none,              send_packet,     none>,
          Row<Connected,        event::connect,             none,              none,            none>,
          Row<Connected,        event::disconnect,          ShuttingDown,      none,            none>,
          Row<ShuttingDown,     event::shutdown_timeout,    NotConnected,      none,            none>
          // +------------------+---------------------------+------------------+----------------+----------
          > {};

template <typename... T>
using Submachines = meta::meta_list<T...>;

template <typename Derived>
struct machine_base : public boost::msm::front::state_machine_def<Derived> {
  client_interface* client_ = nullptr;
};

struct client_machine_ : public machine_base<client_machine_> {
  using submachines = Submachines<ConnectBroker>;
  using initial_state = NotConnected;
};
using client_machine = boost::msm::back::state_machine<client_machine_>;

}  // namespace detail
}  // namespace mqtt

#endif /* MQTT_DETAIL_HPP_ */
