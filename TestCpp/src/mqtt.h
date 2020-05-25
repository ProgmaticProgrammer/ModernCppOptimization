/*
 * mqtt.cpp
 *
 *  Created on: 23/05/2020
 *      Author: Owner
 */
#include <string>
#include <vector>

namespace mqtt {

template <typename T>
using my_allocator = std::allocator<T>;

using mqtt_string_t = std::basic_string<char, std::char_traits<char>, my_allocator<char>>;

template <typename T>
using mqtt_vector_t = std::vector<T, my_allocator<T>>;
using mqtt_publish_handler_t = void*;

struct client_interface {
  virtual ~client_interface() = default;

  virtual void send_to_broker(uint8_t const* data, uint16_t length) = 0;
  virtual void receive_from_broker(/*...*/) = 0;
  virtual void queue_task(/*...*/) = 0;
  virtual void update_connection_status(/*...*/) = 0;
  template <typename Packet>
  void send(Packet const& packet) {
    // auto buffer = serialize(packet);
    // send_to_broker(pointer(buffer), size(buffer));
  }
};

template <typename Connection, typename Executor,
          typename PublishHandler = mqtt_publish_handler_t>
class client {
 public:
  client(mqtt_string_t identifier, Connection& connection,
         Executor& task_executor);

 private:
 //namespace detail = mqtt::detail;
  //detail::client_interface_wrapper<Connection, Executor> client_interface_;
  PublishHandler publish_handler_;
  mqtt_string_t mqtt_identifier_;
  //detail::client_machine client_machine_;
};

}  // namespace mqtt
