/*
 * mqtt.cpp
 *
 *  Created on: 23/05/2020
 *      Author: Owner
 */
//#include <string>
#include <vector>

#include "mqtt_detail.hpp"

namespace meta {
template <typename... T>
struct meta_list {};
}  // namespace meta

namespace mqtt {

template <typename T>
using my_allocator = std::allocator<T>;

template <typename Char, typename Traits = std::char_traits<Char>>
using basic_string = std::basic_string<Char, Traits, my_allocator<Char>>;
using string = basic_string<char>;
using mqtt_string_t = string;

template <typename T>
using vector = std::vector<T, my_allocator<T>>;

using mqtt_publish_handler_t = void*;

template <typename Connection, typename Executor,
          typename PublishHandler = mqtt_publish_handler_t>
class client {
 public:
  client(mqtt_string_t identifier, Connection& connection,
         Executor& task_executor);

 private:
  detail::client_interface_wrapper<Connection, Executor> client_interface_;
  PublishHandler publish_handler_;
  mqtt_string_t mqtt_identifier_;
  detail::client_machine client_machine_;
};

}  // namespace mqtt
