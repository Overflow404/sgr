/*
 * warcraft3.c
 * Copyright (C) 2009-2011 by ipoque GmbH
 * 
 * This file is part of OpenDPI, an open source deep packet inspection
 * library based on the PACE technology by ipoque GmbH
 * 
 * OpenDPI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * OpenDPI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenDPI.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */



/* include files */

#include "ndpi_protocols.h"
#ifdef NDPI_PROTOCOL_WARCRAFT3

static void ndpi_int_warcraft3_add_connection(struct ndpi_detection_module_struct
												*ndpi_struct, struct ndpi_flow_struct *flow)
{
	ndpi_int_add_connection(ndpi_struct, flow, NDPI_PROTOCOL_WARCRAFT3, NDPI_REAL_PROTOCOL);
}

void ndpi_search_warcraft3(struct ndpi_detection_module_struct
							 *ndpi_struct, struct ndpi_flow_struct *flow)
{
	struct ndpi_packet_struct *packet = &flow->packet;
	
//      struct ndpi_id_struct         *src=ndpi_struct->src;
//      struct ndpi_id_struct         *dst=ndpi_struct->dst;

	u_int16_t l;

	NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "search WARCRAFT3\n");


	if (flow->packet_counter == 1 && packet->payload_packet_len == 1 && packet->payload[0] == 0x01) {
		NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "maybe warcraft3: packet_len == 1\n");
		return;
	} else if (packet->payload_packet_len >= 4 && (packet->payload[0] == 0xf7 || packet->payload[0] == 0xff)) {

		NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "packet_payload begins with 0xf7 or 0xff\n");

		l = packet->payload[2] + (packet->payload[3] << 8);	// similar to ntohs

		NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "l = %u \n", l);

		while (l <= (packet->payload_packet_len - 4)) {
			if (packet->payload[l] == 0xf7) {
				u_int16_t temp = (packet->payload[l + 2 + 1] << 8) + packet->payload[l + 2];
				NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "another f7 visited.\n");
				if (temp <= 2) {
					NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "break\n");
					break;
				} else {
					l += temp;
					NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "l = %u \n", l);
				}
			} else {
				NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "break\n");
				break;
			}
		}


		if (l == packet->payload_packet_len) {
			NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "maybe WARCRAFT3\n");
			NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "flow->packet_counter = %u \n",
					flow->packet_counter);
			if (flow->packet_counter > 2) {
				NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "detected WARCRAFT3\n");
				ndpi_int_warcraft3_add_connection(ndpi_struct, flow);
				return;
			}
			return;
		}


	}


	NDPI_LOG(NDPI_PROTOCOL_WARCRAFT3, ndpi_struct, NDPI_LOG_DEBUG, "no warcraft3 detected.\n");
	NDPI_ADD_PROTOCOL_TO_BITMASK(flow->excluded_protocol_bitmask, NDPI_PROTOCOL_WARCRAFT3);
}

#endif
