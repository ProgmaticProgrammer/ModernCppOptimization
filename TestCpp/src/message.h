/*
 * message.h
 *
 *  Created on: 23/05/2020
 *      Author: Owner
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <cstddef>
#include <tuple>

#include "mqtt.h"
namespace mqtt {

enum class qos_t : uint8_t {
  QOS0 = 0x00,
  QOS1 = 0x01,
  QOS2 = 0x02,
  FAILURE = 0x80,
};
enum class connection_status_t : uint8_t {
  DISCONNECTED = 0x00,
  CONNECTED = 0x01,
  DISCONNECTING = 0x02,
  FAILURE = 0x80,
};
struct connect {
  mqtt_string_t client_id;
};
struct subscribe {
  uint16_t packet_id;
  using topic_filter_t = std::tuple<mqtt_string_t, qos_t>;
  using filters_t = mqtt_vector_t<topic_filter_t>;
  filters_t filters;
};
struct unsubscribe {
  uint16_t packet_id;
  mqtt_vector_t<mqtt_string_t> filters;
};
struct pingreq {};
struct disconnect {
  bool force;
};
struct publish_out {};
struct shutdown_timeout {};


}  // namespace mqtt
#endif /* MESSAGE_H_ */
