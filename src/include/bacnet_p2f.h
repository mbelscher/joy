/*
 *	
 * Copyright (c) 2017 Cisco Systems, Inc.
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

/**
 * \file bacnet_p2f.h
 *
 * \brief header file for joy
 * 
 */

#ifndef BACNET_P2F_H
#define BACNET_P2F_H

#include "bacenum.h"

typedef struct stat_counts_ {
   unsigned int valid;
   unsigned int invalid;
} stat_counts_t;

typedef struct npdu_header_counts_ {
    unsigned int bad_version;
    unsigned int net_layer_msg;
    unsigned int apdu_msg;
    unsigned int dest_present;
    unsigned int src_present;
    unsigned int expecting_reply;
    unsigned int priority_life_safety;
    unsigned int critical_equip;
    unsigned int urgent;
    unsigned int normal;
} npdu_header_counts_t;

typedef struct apdu_header_counts_ {
    unsigned int type_counts[PDU_TYPE_MAX];
    unsigned int num_invalid_type;
    unsigned int num_unconfirmed_broadcast_req;
} apdu_header_counts_t;



typedef struct flow_record_bacnet_ {
    unsigned int total_pkts;
    stat_counts_t bvlc_stats[MAX_BVLC_FUNCTION];
    npdu_header_counts_t npdu_header_counts;
    unsigned int nlm_counts[NETWORK_MESSAGE_ARRAY_MAX];
    apdu_header_counts_t apdu_header_counts;
} flow_record_bacnet_t;


#endif /* BACNET_P2F_H */
