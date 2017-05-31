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

/**************************************************
 * @file bacnet.h
 *
 * @brief Interface to BACnet code which is used
 *        to collect or export using the protocol.
 **************************************************/

#ifndef BACNET_H
#define BACNET_H

#include "p2f.h"
#include "bacenum.h"
#include "err.h"


#define BVLL_TYPE_BACNET_IP (0x81)
#define BACNET_PORT_1 47808
#define BACNET_PORT_2 59511

#define CONFIRMED_REQ 0
#define UNCONFIRMED_REQ 1
#define SIMPLE_ACK 2
#define COMPLEX_ACK 3
#define SEGMENT_ACK 4

#define APDU_TYPE_MASK 0xF0
#define APDU_TYPE_SHIFT 4

typedef struct bvlc_header_ {
    uint8_t type;
    uint8_t function;
    uint16_t length;
} bvlc_header;

typedef struct npdu_header_ {
    uint8_t version;
    uint8_t control;
} npdu_header;

typedef struct adr_header_ {
    uint16_t net;
    uint8_t len;
} adr_header;

#define NPCI_CONTROL_APDU_MASK 0x80
#define NPCI_CONTROL_DEST_SPECIFIER_MASK 0x20
#define NPCI_CONTROL_SRC_SPECIFIER_MASK 0x08
#define NPCI_CONTROL_EXPECTING_REPLY_MASK 0x40
#define NPCI_CONTROL_PRIORITY_MASK 0x03


typedef struct apdu_header_ {
    uint8_t type;

    // THERE IS MORE HERE BUT I ONLY CARE ABOUT THE APDU TYPE
} apdu_header;

void bacnet_init(struct flow_record *flow_record);
void bacnet_print_flow_record_bacnet(const flow_record_bacnet_t *bacnet_flow);
enum status process_bacnet(const void *payload_start,
                           int len,
                           struct flow_record *flow_record);

#endif /* BACNET_H */
