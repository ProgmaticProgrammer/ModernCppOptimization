/*
 * message.h
 *
 *  Created on: 23/05/2020
 *      Author: Owner
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <tuple>

#include "mqtt.h"

enum class qos_t : uint8_t {
  QOS0 = 0x00,
  QOS1 = 0x01,
  QOS2 = 0x02,
  FAILURE = 0x80,
};

// Client to Server message types
struct connect {
  mqtt::string client_id;
};
struct subscribe {
  uint16_t packet_id;
  using topic_filter_t = std::tuple<mqtt::string, qos_t>;
  using filters_t = mqtt::vector<topic_filter_t>;
  filters_t filters;
};
struct unsubscribe {
  uint16_t packet_id;
  mqtt::vector<mqtt::string> filters;
};
struct pingreq {};
struct disconnect {
  bool force;
};

#endif /* MESSAGE_H_ */
