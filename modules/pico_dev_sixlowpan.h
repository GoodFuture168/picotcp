/*********************************************************************
 PicoTCP. Copyright (c) 2012-2015 Altran Intelligent Systems. Some rights reserved.
 See LICENSE and COPYING for usage.
 
 Authors: Jelle De Vleeschouwer
 *********************************************************************/
#ifndef INCLUDE_PICO_SIXLOWPAN
#define INCLUDE_PICO_SIXLOWPAN

/* picoTCP includes */
#include "pico_device.h"
#include "pico_config.h"

#define IEEE802154_PHY_MTU (128u)
#define IEEE802154_MAC_MTU (125u)
#define IEEE802154_PHY_OVERHEAD (3u)
#define IEEE802154_MAC_OVERHEAD (2u)

/**
 *  FRAME TYPE DEFINITIONS (IEEE802.15.4)
 */
typedef enum
{
    IEEE802154_FRAME_TYPE_BEACON,
    IEEE802154_FRAME_TYPE_DATA,
    IEEE802154_FRAME_TYPE_ACKNOWLEDGEMENT,
    IEEE802154_FRAME_TYPE_COMMAND
} PACKED IEEE802154_frame_type_t;

/**
 *  FCF FLAG VALUE DEFINITIONS (IEEE802.15.4)
 */
typedef enum
{
    IEEE802154_FALSE,
    IEEE802154_TRUE
} PACKED IEEE802154_flag_t;

/**
 *  FRAME VERSION DEFINITIONS (IEEE802.15.4)
 */
typedef enum
{
    IEEE802154_FRAME_VERSION_2003,
    IEEE802154_FRAME_VERSION_2006
} PACKED IEEE802154_frame_version_t;

/**
 *  SECURITY LEVEL DEFINITIONS (IEEE802.15.4)
 */
typedef enum
{
    IEEE802154_SECURITY_LVL_NONE,
    IEEE802154_SECURITY_LVL_MIC_32,
    IEEE802154_SECURITY_LVL_MIC_64,
    IEEE802154_SECURITY_LVL_MIC_128,
    IEEE802154_SECURITY_LVL_ENC,
    IEEE802154_SECURITY_LVL_ENC_MIC_32,
    IEEE802154_SECURITY_LVL_ENC_MIC_64,
    IEEE802154_SECURITY_LVL_ENC_MIC_128
} PACKED IEEE802154_security_level_t;

/**
 *  SECURITY CONTROL FIELD (IEEE802154)
 */
typedef PACKED_STRUCT_DEF
{
    uint8_t reserved: 3;
    uint8_t key_identifier_mode: 2;
    uint8_t security_level: 3;
}
IEEE802154_scf_t;

/**
 *  AUXILIARY SECURITY HEADER (IEEE802.15.4)
 */
typedef struct
{
    IEEE802154_scf_t scf;
    uint8_t frame_counter[4];
    uint8_t key_identifier[9];
}
IEEE802154_security_header_t;

/**
 *  FRAME CONTROL FIELD (IEEE802.15.4)
 */
PACKED_STRUCT_DEF IEEE802154_fcf
{
    uint8_t frame_type: 3;          /* Type of frame, see PICO_FRAME_TYPE_x */
    uint8_t security_enabled: 1;    /* '1' When frame is secured */
    uint8_t frame_pending: 1;       /* '1' When the sending host has more data */
    uint8_t ack_required: 1;        /* Request for an acknowledgement */
    uint8_t intra_pan: 1;           /* PAN ID's are equal, src-PAN is elided */
    uint8_t res0: 1;                /* 1 reserved bit */
    uint8_t res1: 2;                /* 2 reserved bits */
    uint8_t dam: 2;                 /* Destination AM, see PICO_ADDR_MODE_x */
    uint8_t frame_version: 2;       /* Version, see PICO_FRAME_VERSION_x */
    uint8_t sam: 2;                 /* Source AM, see PICO_ADDR_MODE_x */
};

typedef union
{
    struct IEEE802154_fcf fcf;
    uint8_t components[2];
} IEEE802154_fcf_t;

/**
 *  FRAME HEADER (IEEE802.15.4)
 */
typedef PACKED_STRUCT_DEF
{
    struct IEEE802154_fcf fcf;
    uint8_t seq;
    uint16_t pan;
    uint8_t addresses[0];
} IEEE802154_hdr_t;

/**
 *  RADIO DRIVER RETURN CODES (DRIVER)
 */
typedef enum
{
    /* PLATFORM ERRORS */
	RADIO_ERR_NOERR = 0,
	RADIO_ERR_EINVAL,
    RADIO_ERR_ENOMEM,
    /* RADIO ERRORS */
	RADIO_ERR_ENOCONN,
    RADIO_ERR_ERX,
    RADIO_ERR_ETX
}
radio_rcode_t;

/**
 *  Generic radio-structure to provide an interface between the 
 *	IEEE802.15.4-radio specific device driver and the 6LoWPAN-
 *	adaption layer.
 */
typedef struct RADIO
{
	/**
	 *
	 */
	radio_rcode_t (*transmit)(struct RADIO *radio, void *buf, int len);
	
	/**
	 *
	 */
	radio_rcode_t (*receive)(struct RADIO *radio, uint8_t buf[IEEE802154_PHY_MTU]);
	
	/**
	 *
	 */
	radio_rcode_t (*get_addr_ext)(struct RADIO *radio, uint8_t buf[8]);
	
	/**
	 *
	 */
	uint16_t (*get_pan_id)(struct RADIO *radio);
	
	/**
	 *
	 */
	uint16_t (*get_addr_short)(struct RADIO *radio);
	
	/**
	 *
	 */
	radio_rcode_t (*set_addr_short)(struct RADIO *radio, uint16_t short_16);
}
radio_t;

void pico_sixlowpan_set_prefix(struct pico_device *dev, struct pico_ip6 prefix);

/**
 *  The radio may or may not already have had a short 16-bit address
 *  configured. If it didn't, this function allows the radio to notify the
 *  6LoWPAN layer when it did configured a short 16-bit address after the
 *  initialisation-procedure. This can be possible due to an association
 *  event while comminissioning the IEEE802.15.4 PAN.
 *  This function will call radio_t->get_addr_short in it's turn.
 *
 *  @param dev pico_device *, the 6LoWPAN pico_device-instance.
 */
void pico_sixlowpan_short_addr_configured(struct pico_device *dev);

/**
 *  Creates a picoTCP-compatible pico_device. Creates the
 *  interface between picoTCP and the device driver (radio_t).
 *
 *  @param radio Radio-instance for the interface between 802.15.4 and
 *				 picoTCP.
 *
 *  @return pico_device-instance, initialised and everything.
 */
struct pico_device *pico_sixlowpan_create(radio_t *radio);

#endif /* INCLUDE_PICO_SIXLOWPAN */