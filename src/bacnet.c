/*
 *
 * Copyright (c) 2016 Cisco Systems, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 *
 *   Neither the name of the Cisco Systems, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**********************************************************
 * @file bacnet.c
 *
 * @brief Source code to perform IPFIX protocol operations.
 **********************************************************/

/*
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <netdb.h>
#include <openssl/rand.h>
#include "ipfix.h"
#include "pkt.h"
#include "http.h"
#include "tls.h"
#include "pkt_proc.h"
#include "p2f.h"
#include "config.h"
*/

#include "bacnet.h"
#include <string.h>


void bacnet_print_flow_record_bacnet(flow_record_bacnet *bacnet_flow_record);

enum status process_bacnet_bvlc_header(uint8_t *head,
				       int len,
				       uint16_t *header_size,
				       uint16_t *bacnet_length,
				       uint8_t *function) {

    bvlc_header *header = (bvlc_header *)head;

    *function = header->function;
    *bacnet_length = ntohs(header->length);
    *header_size = sizeof(bvlc_header);

    switch (*function) {
      case REGISTER_FOREIGN_DEVICE:
      case BVLC_RESULT:
	  *header_size = *header_size + 2;
	  break;
      case FORWARD_NPDU:
	  *header_size = *header_size + 6;
	  break;
      default:
	break;
    }

    return ok;

}

enum status process_bacnet_npdu_header(uint8_t *head,
				       int len,
				       uint8_t function,
				       uint16_t *header_size) {

    *header_size = sizeof(npdu_header);

    if (function == DISTRIBUTE_BROADCAST_TO_NETWORK)
	*header_size = *header_size + 4;

    return ok;
}

enum status process_bacnet_apdu_header(uint8_t *head,
				       int len,
				       uint8_t *apdu_type) {

    apdu_header *header = (apdu_header *)head;

    *apdu_type = (APDU_TYPE_MASK & header->type) >> APDU_TYPE_SHIFT;

    return ok;

}

int is_address_broadcast(struct in_addr address) {

    /// There has to be a better way to do this.

    char *address_string = inet_ntoa(address);

    int address_string_length = strlen(address_string);
    char *broadcast = ".255";
    int broadcast_length = strlen(broadcast);

    if (address_string_length > broadcast_length) {
	if (strcmp(address_string + address_string_length - broadcast_length, broadcast) == 0) {
	    return 1;
	}
    }

    return 0;
}


enum status process_bacnet(const void *payload_start,
                           int len,
                           struct flow_record *flow_record) {

    uint8_t *packet = (uint8_t *)payload_start;

    uint8_t function = 0;
    uint16_t bacnet_length = 0;
    uint16_t header_size = 0;
    uint8_t apdu_type = 0;
    enum status result = ok;

    result = process_bacnet_bvlc_header(packet, len, &header_size, &bacnet_length, &function);

    if (result != ok)
	return result;

    if (header_size == len) {

	if (function == REGISTER_FOREIGN_DEVICE)
	    flow_record->bacnet_source.num_bvlc_foreign_request++;
	else if (function == BVLC_RESULT)
	    flow_record->bacnet_source.num_bvlc_result++;

	return result;
    }

    packet = packet + header_size;
    len -= header_size;

    result = process_bacnet_npdu_header(packet, len, function, &header_size);

    if (result != ok)
	return result;

    len -= header_size;

    packet = packet + header_size;

    result = process_bacnet_apdu_header(packet, len, &apdu_type);

    if (result != ok)
	return result;

    switch (apdu_type) {
      case CONFIRMED_REQ:
	flow_record->bacnet_source.num_confirmed_req++;
        break;
      case UNCONFIRMED_REQ:
	flow_record->bacnet_source.num_unconfirmed_req++;

	if (is_address_broadcast(flow_record->key.da))
	    flow_record->bacnet_source.num_unconfirmed_req_broadcast++;
        break;
      case SIMPLE_ACK:
	flow_record->bacnet_source.num_simple_ack++;
        break;
      case COMPLEX_ACK:
	flow_record->bacnet_source.num_complex_ack++;
        break;
      case SEGMENT_ACK:
	flow_record->bacnet_source.num_segment_ack++;
        break;
      default:
	break;
    }

    return result;
}

void bacnet_print_flow_record_bacnet(flow_record_bacnet *bacnet_flow_record) {
    printf("Num Confirmed Req: %d \n"
	   "Num Unconfirmed Req Broadcast: %d\n"
	   "Num Total Unconfirmed Req: %d\n"
	   "Num Simple ACK: %d\n"
	   "Num Complex ACK: %d\n"
	   "Num Segment ACK: %d\n"
	   "Num BVLC Only Foreign Request: %d\n"
	   "Num BVLC Only Result: %d\n",
	   bacnet_flow_record->num_confirmed_req,
	   bacnet_flow_record->num_unconfirmed_req_broadcast,
	   bacnet_flow_record->num_unconfirmed_req,
	   bacnet_flow_record->num_simple_ack,
	   bacnet_flow_record->num_complex_ack,
	   bacnet_flow_record->num_segment_ack,
	   bacnet_flow_record->num_bvlc_foreign_request,
	   bacnet_flow_record->num_bvlc_result);
}

void bacnet_init_flow_record_bacnet(flow_record_bacnet *bacnet_flow_record) {
    bacnet_flow_record->num_confirmed_req = 0;
    bacnet_flow_record->num_unconfirmed_req_broadcast = 0;
    bacnet_flow_record->num_unconfirmed_req = 0;
    bacnet_flow_record->num_simple_ack = 0;
    bacnet_flow_record->num_complex_ack = 0;
    bacnet_flow_record->num_segment_ack = 0;
    bacnet_flow_record->num_bvlc_foreign_request = 0;
    bacnet_flow_record->num_bvlc_result = 0;
}


void bacnet_init(struct flow_record *flow_record) {
    bacnet_init_flow_record_bacnet(&flow_record->bacnet_source);
}
