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
#include "output.h"
#include <string.h>

extern zfile output;


static const char *bacnet_bvlc_function_names[] = {
    "num_result",
    "num_write_broadcast_distribution_table",
    "num_read_broadcast_distribution_table",
    "num_read_broadcast_distribution_table_ack",
    "num_forwarded_npdu",
    "num_register_foreign_device",
    "num_read_foreign_device_table",
    "num_read_foreign_device_table_ack",
    "num_delete_foreign_device_table_entry",
    "num_distribute_broadcast_to_network",
    "num_original_unicast_npdu",
    "num_original_broadcast_npdu"
};

static const char *bacnet_nlm_msg_type_names[] = {
    "num_who_is_router_to_network",
    "num_i_am_router_to_network",
    "num_i_could_be_router_to_network",
    "num_reject_message_to_network",
    "num_router_busy_to_network",
    "num_router_available_to_network",
    "num_init_rt_table",
    "num_init_rt_table_ack",
    "num_establish_connection_to_network",
    "num_disconnect_connection_to_network",
    "num_ashrae_reserved",
    "num_vendor_specific"
};

static const char *bacnet_apdu_type_names[] = {
    "num_confirmed_service_req",
    "num_unconfirmed_service_req",
    "num_simple_ack",
    "num_complex_ack",
    "num_segment_ack",
    "num_error",
    "num_reject",
    "num_abort",
};


void bacnet_print_flow_record_bvlc(const flow_record_bacnet_t *record) {

    int i = 0;
    /* add bvlc header entry */
    zprintf(output,",\"bvlc_header\": {");

    /* loop through and add the bvlc data into the json */
    zprintf(output, "\"%s_valid\":%u", bacnet_bvlc_function_names[i], record->bvlc_stats[i].valid);
    zprintf(output, ",\"%s_invalid\":%u", bacnet_bvlc_function_names[i], record->bvlc_stats[i].invalid);

    for (i=1; i < MAX_BVLC_FUNCTION; i++) {
        zprintf(output, ",\"%s_valid\":%u", bacnet_bvlc_function_names[i], record->bvlc_stats[i].valid);
        zprintf(output, ",\"%s_invalid\":%u", bacnet_bvlc_function_names[i], record->bvlc_stats[i].invalid);
    }  

    zprintf(output,"}");    /* complete blvc header entry */
}

void bacnet_print_flow_record_npdu(const flow_record_bacnet_t *record) {

    /* add npdu header entry */
    zprintf(output,",\"npdu_header\": {");

    zprintf(output, "\"num_npdu_bad_version\":%u", record->npdu_header_counts.bad_version);
    zprintf(output, ",\"num_npci_network_layer_msg\":%u", record->npdu_header_counts.net_layer_msg);
    zprintf(output, ",\"num_npci_apdu_msg\":%u", record->npdu_header_counts.apdu_msg);
    zprintf(output, ",\"num_npci_dest_present\":%u", record->npdu_header_counts.dest_present);
    zprintf(output, ",\"num_npci_src_present\":%u", record->npdu_header_counts.src_present);
    zprintf(output, ",\"num_npci_expecting_reply\":%u", record->npdu_header_counts.expecting_reply);
    zprintf(output, ",\"num_npci_priority_life_safety\":%u", record->npdu_header_counts.priority_life_safety);
    zprintf(output, ",\"num_npci_critical_equip\":%u", record->npdu_header_counts.critical_equip);
    zprintf(output, ",\"num_npci_urgent\":%u", record->npdu_header_counts.urgent);
    zprintf(output, ",\"num_npci_normal\":%u", record->npdu_header_counts.normal);

    zprintf(output,"}");    /* complete npdu header entry */
}

void bacnet_print_flow_record_nlm(const flow_record_bacnet_t *record) {

    int i = 0;
    /* add network message  entry */
    zprintf(output,",\"network_message\": {");

    /* loop through and add the nlm data into the json */
    zprintf(output, "\"%s\":%u", bacnet_nlm_msg_type_names[i], record->nlm_counts[i]);
    for (i=1; i <NETWORK_MESSAGE_ARRAY_MAX; i++) {
        zprintf(output, ",\"%s\":%u", bacnet_nlm_msg_type_names[i], record->nlm_counts[i]);
    }
    zprintf(output,"}");    /* complete network message entry */
}

void bacnet_print_flow_record_apdu(const flow_record_bacnet_t *record) {

    int i = 0;

    /* add apdu message  entry */
    zprintf(output,",\"apdu_message\": {");

    /* loop through and add the nlm data into the json */
    zprintf(output, "\"%s\":%u", bacnet_apdu_type_names[i], record->apdu_header_counts.type_counts[i]);
    for (i=1; i <PDU_TYPE_MAX; i++) {
        zprintf(output, ",\"%s\":%u", bacnet_apdu_type_names[i], record->apdu_header_counts.type_counts[i]);
    }
    zprintf(output, ",\"num_unconfirmed_broadcast_req\":%u", record->apdu_header_counts.num_unconfirmed_broadcast_req);
    zprintf(output, ",\"num_invalid_type\":%u", record->apdu_header_counts.num_invalid_type);

    zprintf(output,"}");    /* complete apdu  message entry */
}

void bacnet_print_flow_record_bacnet(const flow_record_bacnet_t *bacnet_flow_record) {
    
    if (bacnet_flow_record->total_pkts) {
        zprintf(output,",\"bacnet\": {");
        zprintf(output, "\"total_pkts\":%u", bacnet_flow_record->total_pkts);
        bacnet_print_flow_record_bvlc(bacnet_flow_record);
        bacnet_print_flow_record_npdu(bacnet_flow_record);
        bacnet_print_flow_record_nlm(bacnet_flow_record);
        bacnet_print_flow_record_apdu(bacnet_flow_record);
        zprintf(output,"}");  /* complete the bacnet record */
    }

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

enum status process_bacnet_bvlc_header(uint8_t **head,
      	                               uint16_t *next_length,
                                       struct flow_record *flow_record) {

    int  err = ok;
    bvlc_header *header = (bvlc_header *)*head;
    stat_counts_t *bvlc_stats = flow_record->bacnet_source.bvlc_stats;
    
    uint16_t bacnet_length = ntohs(header->length);
    uint16_t header_size = sizeof(bvlc_header);
    int bvlc_valid = 0;

    /* make sure this is really Bacnet */
    if (header->type != BVLL_TYPE_BACNET_IP) {
     fprintf(stderr, "Header type failure  is 0x%02x", header->type);
       /* this is not a BVLL packet */
       return failure;
    }
    flow_record->bacnet_source.total_pkts++;

    switch (header->function) { 
      case BVLC_RESULT:
          /* 
           * add for TTL field to the header 
           * The function doesn't have a NPDU
           */
          header_size+=2;
          if (bacnet_length == 6) {
              bvlc_valid = 1;
          }
	  break;
      case BVLC_WRITE_BROADCAST_DISTRIBUTION_TABLE:
      case BVLC_READ_BROADCAST_DIST_TABLE:
      case BVLC_READ_BROADCAST_DIST_TABLE_ACK:
          break;
      case BVLC_FORWARDED_NPDU:
	  header_size += 6;
	  break;
      case BVLC_REGISTER_FOREIGN_DEVICE: 
          if (bacnet_length == 6) { 
              bvlc_valid = 1;
          }
          break;
      case BVLC_READ_FOREIGN_DEVICE_TABLE:
      case BVLC_READ_FOREIGN_DEVICE_TABLE_ACK:
      case BVLC_DELETE_FOREIGN_DEVICE_TABLE_ENTRY:
      case BVLC_DISTRIBUTE_BROADCAST_TO_NETWORK:
      case BVLC_ORIGINAL_UNICAST_NPDU:
      case BVLC_ORIGINAL_BROADCAST_NPDU:
          bvlc_valid = 1;
      default:
	break;
    }
    if (bvlc_valid) {
        bvlc_stats[header->function].valid++;
    } else {
        bvlc_stats[header->function].invalid++;
        err = failure;
    }
    *head += header_size;
    *next_length = bacnet_length-header_size;
    return err;

}

enum status process_bacnet_npdu_header(uint8_t **head,
                                       uint16_t *next_length,
                                       struct flow_record *flow_record,
                                       uint8_t *nlm) {

    /*
     *  header size is variable depending on NPCI (control) 
     * #define NPCI_CONTROL_APDU_MASK 0x80
     * #define NPCI_CONTROL_DEST_SPECIFIER_MASK 0x20
     * #define NPCI_CONTROL_SRC_SPECIFIER_MASK 0x08
     * #define NPCI_CONTROL_EXPECTING_REPLY_MASK 0x40
     * #define NPCI_CONTROL_PRIORITY_MASK 0x03
     */
    npdu_header *header = (npdu_header *)*head;
    npdu_header_counts_t *npdu_stats = &flow_record->bacnet_source.npdu_header_counts;

    uint16_t header_size = sizeof(npdu_header);
    uint8_t src_dest_offset =  sizeof(npdu_header);
    adr_header *adr = NULL;
     
    if (header->version != 0x01) {
        npdu_stats->bad_version++;
        return failure;
    }

    /* process NPCI */
    if (header->control & NPCI_CONTROL_APDU_MASK) {
       /* Message is a Network Layer Message Type */
       npdu_stats->net_layer_msg++;
       *nlm = 1;
    } else {
       /* message is an APDU */
       npdu_stats->apdu_msg++;
       *nlm = 0;
    }
 
    if (header->control & NPCI_CONTROL_DEST_SPECIFIER_MASK) {
       /* dest specifier present */
       npdu_stats->dest_present++;

       adr = (adr_header *)(header+src_dest_offset);
       src_dest_offset += sizeof(adr_header) + adr->len;
       header_size += src_dest_offset + 1;  /* need to account for the hop count */         
   }
    
    if (header->control & NPCI_CONTROL_SRC_SPECIFIER_MASK) {
       /* src specifier present */
       npdu_stats->src_present++;
       adr = (adr_header *)(header+src_dest_offset);
       header_size += sizeof(adr_header) + adr->len;
    }
    
    if (header->control & NPCI_CONTROL_EXPECTING_REPLY_MASK) {
       npdu_stats->expecting_reply++;
    }
   
    switch (header->control & NPCI_CONTROL_PRIORITY_MASK) {
    case MESSAGE_PRIORITY_LIFE_SAFETY:
        npdu_stats->priority_life_safety++;
        break;
    case MESSAGE_PRIORITY_CRITICAL_EQUIPMENT:
        npdu_stats->critical_equip++;
        break;
    case MESSAGE_PRIORITY_URGENT:
        npdu_stats->urgent++;
        break;
    default:
        /*  MESSAGE_PRIORITY_NORMAL */
        npdu_stats->normal++;
        break;
    }
    
    /* 
     * we need to determine where the Network layer Service Data unit(NSDU) start 
     * which is either an network Layer message or APDU
     */
    *head += header_size;
    *next_length -= header_size;


     

    return ok;
}


enum status process_bacnet_nlm_header(uint8_t **head,
                                      uint16_t *next_length,
                                      struct flow_record *flow_record) {

    uint8_t nlm_msg_type = (uint8_t) **head;

    switch (nlm_msg_type) {
    case NETWORK_MESSAGE_WHO_IS_ROUTER_TO_NETWORK:
    case NETWORK_MESSAGE_I_AM_ROUTER_TO_NETWORK:
    case NETWORK_MESSAGE_I_COULD_BE_ROUTER_TO_NETWORK:
    case NETWORK_MESSAGE_REJECT_MESSAGE_TO_NETWORK:
    case NETWORK_MESSAGE_ROUTER_BUSY_TO_NETWORK:
    case NETWORK_MESSAGE_ROUTER_AVAILABLE_TO_NETWORK:
    case NETWORK_MESSAGE_INIT_RT_TABLE:
    case NETWORK_MESSAGE_INIT_RT_TABLE_ACK:
    case NETWORK_MESSAGE_ESTABLISH_CONNECTION_TO_NETWORK:
    case NETWORK_MESSAGE_DISCONNECT_CONNECTION_TO_NETWORK:
        flow_record->bacnet_source.nlm_counts[nlm_msg_type]++;
        break;
    default:
        if (nlm_msg_type >= 0xa && nlm_msg_type <= 0x7f) {
            /* reserved for ASHRAE */
            flow_record->bacnet_source.nlm_counts[NETWORK_MESSAGE_ASHRAE_RESERVED]++;
        } else {
           /* this is a vendor specific message (0x80-0xff) */
            flow_record->bacnet_source.nlm_counts[NETWORK_MESSAGE_VENDOR_SPECIFIC]++;
        }
        break;
    }

    return ok;

}

enum status process_bacnet_apdu_header(uint8_t **head,
                                       uint16_t *next_length,
                                       struct flow_record *flow_record) {

    apdu_header *header = (apdu_header *)*head;
    apdu_header_counts_t *apdu_stats = &flow_record->bacnet_source.apdu_header_counts;

    uint8_t apdu_type = (APDU_TYPE_MASK & header->type) >> APDU_TYPE_SHIFT;

    /* currently we are only counting types */
    switch (apdu_type) {
    case PDU_TYPE_UNCONFIRMED_SERVICE_REQUEST:
        if (is_address_broadcast(flow_record->key.da))
            apdu_stats->num_unconfirmed_broadcast_req++;
           /* FALLTHRU to count type */
    case PDU_TYPE_CONFIRMED_SERVICE_REQUEST:
    case PDU_TYPE_SIMPLE_ACK:
    case PDU_TYPE_COMPLEX_ACK:
    case PDU_TYPE_SEGMENT_ACK:
    case PDU_TYPE_ERROR:
    case PDU_TYPE_REJECT:
    case PDU_TYPE_ABORT:
        apdu_stats->type_counts[apdu_type]++;
        break;
    default:
        apdu_stats->num_invalid_type++;
	break;
    }

    return ok;

}


enum status process_bacnet(const void *payload_start,
                           int len,
                           struct flow_record *flow_record) {

    uint8_t *packet = (uint8_t *)payload_start;
    uint16_t next_length = 0;
    enum status result = ok;
    uint8_t nlm = 0;

    result = process_bacnet_bvlc_header(&packet, &next_length, flow_record);

    if (result != ok)
	return result;
 
    if (next_length == 0) {
       /* we have don't have a npdu to process */
       return result;
    }

    result = process_bacnet_npdu_header(&packet, &next_length, flow_record, &nlm);

    if (result != ok)
	return result;


    if (next_length == 0) {
        /* this should not occur */
       return result;
    }
    if (nlm) { 
        result = process_bacnet_nlm_header(&packet, &next_length, flow_record);
    } else { 
        result = process_bacnet_apdu_header(&packet, &next_length, flow_record);
    }
    if (result != ok)
	return result;

    return result;
}


void bacnet_init_flow_record_bacnet(flow_record_bacnet_t *bacnet_flow_record) {
    memset(bacnet_flow_record, 0, sizeof(flow_record_bacnet_t));
}


void bacnet_init(struct flow_record *flow_record) {
    bacnet_init_flow_record_bacnet(&flow_record->bacnet_source);
}
