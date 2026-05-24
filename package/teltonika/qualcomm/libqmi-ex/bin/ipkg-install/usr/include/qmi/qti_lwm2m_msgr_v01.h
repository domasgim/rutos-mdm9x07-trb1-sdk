#ifndef LWM2M_MSGR_SERVICE_01_H
#define LWM2M_MSGR_SERVICE_01_H
/**
  @file qti_lwm2m_msgr_v01.h

  @brief This is the public header file which defines the lwm2m_msgr service Data structures.

  This header file defines the types and structures that were defined in
  lwm2m_msgr. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  
  Copyright (c) 2012-2020 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //source/qcom/qct/interfaces/qmi/rel/deploy/qmi_lwm2m/api/qti_lwm2m_msgr_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Wed Apr  8 2020 (Spin 0)
   From IDL File: qti_lwm2m_msgr_v01.idl */

/** @defgroup lwm2m_msgr_qmi_consts Constant values defined in the IDL */
/** @defgroup lwm2m_msgr_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup lwm2m_msgr_qmi_enums Enumerated types used in QMI messages */
/** @defgroup lwm2m_msgr_qmi_messages Structures sent as QMI messages */
/** @defgroup lwm2m_msgr_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup lwm2m_msgr_qmi_accessor Accessor for QMI service object */
/** @defgroup lwm2m_msgr_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup lwm2m_msgr_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define LWM2M_MSGR_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define LWM2M_MSGR_V01_IDL_MINOR_VERS 0x00
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define LWM2M_MSGR_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define LWM2M_MSGR_V01_MAX_MESSAGE_ID 0x001A
/**
    @}
  */


/** @addtogroup lwm2m_msgr_qmi_consts
    @{
  */

/**  Max object info count.   */
#define LWM2M_MSGR_MAX_OBJ_CNT_V01 100

/**  Max attribute info count.    */
#define LWM2M_MSGR_MAX_ATTR_INFO_CNT_V01 10

/**  Max payload size.   */
#define LWM2M_MSGR_MAX_PAYLOAD_SIZE_V01 2000

/**  Max block data size.   */
#define LWM2M_MSGR_MAX_BLOCK_DATA_SIZE_V01 2000

/**  Max resource data size.   */
#define LWM2M_MSGR_MAX_RESOURCE_DATA_SIZE_V01 50

/**  Max message ID size.  */
#define LWM2M_MSGR_MAX_MSG_ID_SIZE_V01 50

/**  Max URL size.  */
#define LWM2M_MSGR_MAX_URL_SIZE_V01 512

/**  Max config data size.     */
#define LWM2M_MSGR_MAX_CONFIG_DATA_SIZE_V01 2000
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t block1_data_len;  /**< Must be set to # of elements in block1_data */
  uint8_t block1_data[LWM2M_MSGR_MAX_BLOCK_DATA_SIZE_V01];
  /**<   Block-1 data. */

  uint8_t block1_more;
  /**<   More blocks to be received. */

  uint32_t block1_num;
  /**<   Block-1 number.                                             */

  uint16_t block1_size;
  /**<   Block-1 size.                         */

  uint32_t block1_offset;
  /**<   Block-1 offset. */

  uint32_t size1;
}lwm2m_block1_info_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t obj_mask;
  /**<   Object mask. */

  uint16_t object_ID;
  /**<   Object ID.   */

  uint16_t instance_ID;
  /**<   Instance ID. */

  uint16_t resource_ID;
  /**<   Resource ID. */

  uint16_t resource_instance_ID;
  /**<   Resource instance ID. */

  uint8_t resource_type;
  /**<   Resource data type.   */

  uint32_t resource_data_len;  /**< Must be set to # of elements in resource_data */
  uint8_t resource_data[LWM2M_MSGR_MAX_RESOURCE_DATA_SIZE_V01];
  /**<   Resource data.   */
}lwm2m_object_info_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_aggregates
    @{
  */
typedef struct {

  lwm2m_object_info_t_v01 obj_info;
  /**<   LWM2M object information associated with write attributes.  */

  uint32_t set_attr_mask;
  /**<   Bitmap indicating valid attribute fields to set. */

  uint32_t clr_attr_mask;
  /**<   Bitmap indicating attribute fields to clear. */

  uint8_t dim;
  /**<   Dimension. */

  uint32_t minPeriod;
  /**<   Minimum period. */

  uint32_t maxPeriod;
  /**<   Maximum period. */

  double greaterThan;
  /**<   Greater than. */

  double lessThan;
  /**<   Less than. */

  uint8_t step_valid;
  /**<   Step validity. */

  double step;
  /**<   Step.   */
}lwm2m_attributes_info_t_v01;  /* Type */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Registers an application with an LWM2M client. */
typedef struct {

  /* Mandatory */
  uint32_t user_cookie;
  /**<   Transparent user data payload (to be returned in the user callback).     */
}lwm2m_msgr_register_app_extended_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Registers an application with an LWM2M client. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code. */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.   */

  /* Optional */
  uint8_t handle_valid;  /**< Must be set to true if handle is being passed */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.   */
}lwm2m_msgr_register_app_extended_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Indication Message; Forwards any messages/events from the LwM2M client to the application.  */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration. */

  /* Mandatory */
  uint32_t user_cookie;
  /**<   Transparent user data payload (provided during the registration).   */

  /* Mandatory */
  uint32_t msg_type;
  /**<   DL message type (requests, acknowledgements, or internal). */

  /* Mandatory */
  lwm2m_object_info_t_v01 obj_info;
  /**<   Object ID information. */

  /* Mandatory */
  uint32_t msg_id_len;  /**< Must be set to # of elements in msg_id */
  uint8_t msg_id[LWM2M_MSGR_MAX_MSG_ID_SIZE_V01];
  /**<   Message ID. \n
   * The message ID is transparent to the application, but is passed to the application for every message
   * received from the server. The expectation is that the application stores the message ID associated
   * with the message and passes it to the LWM2M client when a response or notification must be sent to
   * the server. After the transaction pertaining to the message is complete, the message ID can be
   * discarded from the application. */

  /* Mandatory */
  uint16_t notification_id;
  /**<   Notification ID.\n
   * When a notification is sent using qapi_Net_LWM2M_Send_Message(), the notification ID associated
   * with the message is returned to the caller. It is the caller's responsibility to maintain the
   * notification ID for observation mapping. Later, when the network does a Cancel Observation for
   * a particular notification using RESET, it is indicated using the notification ID to the caller.
   * Using this notification ID, the caller can cancel the observation. If the cancel observation was not using
   * RESET, obj_info should have the information based on the observation that is to be cancelled.   */

  /* Mandatory */
  uint32_t content_type;
  /**<   Current encoded data payload content type. */

  /* Mandatory */
  uint32_t payload_len;  /**< Must be set to # of elements in payload */
  uint8_t payload[LWM2M_MSGR_MAX_PAYLOAD_SIZE_V01];
  /**<   Encoded data payload. */

  /* Mandatory */
  uint32_t lwm2m_attr_info_len;  /**< Must be set to # of elements in lwm2m_attr_info */
  lwm2m_attributes_info_t_v01 lwm2m_attr_info[LWM2M_MSGR_MAX_ATTR_INFO_CNT_V01];
  /**<   LwM2M write attributes list. */

  /* Mandatory */
  uint32_t event;
  /**<   Internal events. */

  /* Mandatory */
  uint8_t accept_is_valid;
  /**<   Flag to check accept field is set or not. */

  /* Mandatory */
  uint32_t accept;
  /**<   Intended data payload content type. */
}lwm2m_msgr_register_app_extended_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Deregisters an application with an LWM2M client. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.     */
}lwm2m_msgr_deregister_app_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Deregisters an application with an LWM2M client. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.   */
}lwm2m_msgr_deregister_app_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Observes object/instance/resource in the LWM2M client. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.   */

  /* Mandatory */
  uint32_t obj_info_len;  /**< Must be set to # of elements in obj_info */
  lwm2m_object_info_t_v01 obj_info[LWM2M_MSGR_MAX_OBJ_CNT_V01];
  /**<   LwM2M object observation list.   */
}lwm2m_msgr_observe_v2_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Observes object/instance/resource in the LWM2M client. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.    */
}lwm2m_msgr_observe_v2_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Indication Message; Indicates any resource changes in the LWM2M client to the application.  */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration. */

  /* Mandatory */
  uint32_t user_cookie;
  /**<   Transparent user data payload (provided during the registration).     */

  /* Mandatory */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */

  /* Mandatory */
  uint32_t obj_info_len;  /**< Must be set to # of elements in obj_info */
  lwm2m_object_info_t_v01 obj_info[LWM2M_MSGR_MAX_OBJ_CNT_V01];
  /**<   Object observaton Info. */
}lwm2m_msgr_observe_v2_ind_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Cancels observation on object/instance/resource in the LWM2M client. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.  */

  /* Mandatory */
  uint32_t obj_info_len;  /**< Must be set to # of elements in obj_info */
  lwm2m_object_info_t_v01 obj_info[LWM2M_MSGR_MAX_OBJ_CNT_V01];
  /**<   LwM2M object observation cancellation list.   */
}lwm2m_msgr_cancel_observe_v2_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Cancels observation on object/instance/resource in the LWM2M client. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */
}lwm2m_msgr_cancel_observe_v2_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Creates standard/custom LWM2M object instances in the LWM2M client. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.  */

  /* Mandatory */
  uint32_t obj_info_len;  /**< Must be set to # of elements in obj_info */
  lwm2m_object_info_t_v01 obj_info[LWM2M_MSGR_MAX_OBJ_CNT_V01];
  /**<   LwM2M object instance/resource info list. */
}lwm2m_msgr_create_obj_inst_v2_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Creates standard/custom LWM2M object instances in the LWM2M client. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.   */
}lwm2m_msgr_create_obj_inst_v2_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Deletes standard/custom LWM2M object instances from the LWM2M client. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration. */

  /* Mandatory */
  lwm2m_object_info_t_v01 obj_info;
  /**<   Object instance Info. */
}lwm2m_msgr_delete_obj_inst_v2_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Deletes standard/custom LWM2M object instances from the LWM2M client. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.    */
}lwm2m_msgr_delete_obj_inst_v2_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Gets value of the LWM2M object/instance/resource from the LwM2M client. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration. */

  /* Mandatory */
  lwm2m_object_info_t_v01 obj_info;
  /**<   LwM2M object instance/resource info list. */
}lwm2m_msgr_get_v2_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Gets value of the LWM2M object/instance/resource from the LwM2M client. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */

  /* Optional */
  uint8_t obj_info_valid;  /**< Must be set to true if obj_info is being passed */
  uint32_t obj_info_len;  /**< Must be set to # of elements in obj_info */
  lwm2m_object_info_t_v01 obj_info[LWM2M_MSGR_MAX_OBJ_CNT_V01];
  /**<   LwM2M object instance/resource info list.  */
}lwm2m_msgr_get_v2_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Sets the value of the LWM2M object/instance/resource from the LwM2M client */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.  */

  /* Mandatory */
  uint32_t obj_info_len;  /**< Must be set to # of elements in obj_info */
  lwm2m_object_info_t_v01 obj_info[LWM2M_MSGR_MAX_OBJ_CNT_V01];
  /**<   LwM2M object instance/resource info list. */
}lwm2m_msgr_set_v2_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Sets the value of the LWM2M object/instance/resource from the LwM2M client */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */
}lwm2m_msgr_set_v2_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Sends application data, which can be responses/notifications to the server. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.   */

  /* Mandatory */
  uint32_t msg_type;
  /**<   UL message type (response or notification). */

  /* Mandatory */
  lwm2m_object_info_t_v01 obj_info;
  /**<   Object Info. */

  /* Mandatory */
  uint32_t status_code;
  /**<   Application message status (applicable for responses only). */

  /* Mandatory */
  uint8_t conf_msg;
  /**<   Confirmable (ACK) or nonconfirmable application response/notifications.   */

  /* Mandatory */
  uint32_t msg_id_len;  /**< Must be set to # of elements in msg_id */
  uint8_t msg_id[LWM2M_MSGR_MAX_MSG_ID_SIZE_V01];
  /**<   Message ID length.  */

  /* Mandatory */
  uint32_t observation_seq_num;
  /**<   Observation sequence number.  */

  /* Mandatory */
  uint16_t notification_id;
  /**<   Notification ID.   */

  /* Mandatory */
  uint32_t content_type;
  /**<   Encoded data payload content type. */

  /* Mandatory */
  uint32_t payload_len;  /**< Must be set to # of elements in payload */
  uint8_t payload[LWM2M_MSGR_MAX_PAYLOAD_SIZE_V01];
  /**<   Encoded data payload. */
}lwm2m_msgr_send_message_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Sends application data, which can be responses/notifications to the server. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */
}lwm2m_msgr_send_message_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Wakes up the LWM2M client module to send notifications to the server. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.   */

  /* Mandatory */
  uint32_t msg_id_len;  /**< Must be set to # of elements in msg_id */
  uint8_t msg_id[LWM2M_MSGR_MAX_MSG_ID_SIZE_V01];
  /**<   Message ID.  */
}lwm2m_msgr_wakeup_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Wakes up the LWM2M client module to send notifications to the server. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */
}lwm2m_msgr_wakeup_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Sends initialization configuration parameters to the LWM2M client module. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration. */

  /* Mandatory */
  uint32_t config_option;
  /**<   Configuration type. */

  /* Mandatory */
  uint8_t config_type;
  /**<   Configuration data type.   */

  /* Optional */
  uint8_t config_data_valid;  /**< Must be set to true if config_data is being passed */
  uint32_t config_data_len;  /**< Must be set to # of elements in config_data */
  uint8_t config_data[LWM2M_MSGR_MAX_CONFIG_DATA_SIZE_V01];
  /**<   Configuration data. */

  /* Optional */
  uint8_t obj_info_valid;  /**< Must be set to true if obj_info is being passed */
  uint32_t obj_info_len;  /**< Must be set to # of elements in obj_info */
  lwm2m_object_info_t_v01 obj_info[LWM2M_MSGR_MAX_OBJ_CNT_V01];
  /**<   Configuration object instance/resource info list. */
}lwm2m_msgr_config_client_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Sends initialization configuration parameters to the LWM2M client module. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */
}lwm2m_msgr_config_client_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Gets the value of the default server specific Pmin and Pmax values. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.   */

  /* Mandatory */
  uint16_t server_id;
  /**<   Server ID.  */
}lwm2m_msgr_default_attr_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Gets the value of the default server specific Pmin and Pmax values. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */

  /* Optional */
  uint8_t p_min_valid;  /**< Must be set to true if p_min is being passed */
  uint32_t p_min;
  /**<   Default "p_min" server attribute value.   */

  /* Optional */
  uint8_t p_max_valid;  /**< Must be set to true if p_max is being passed */
  uint32_t p_max;
  /**<   Default "p_max" server attribute value. */
}lwm2m_msgr_default_attr_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Configures the server life time information in the LWM2M client. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.  */

  /* Mandatory */
  uint32_t life_time;
  /**<   Lifetime   */

  /* Optional */
  uint8_t url_info_valid;  /**< Must be set to true if url_info is being passed */
  uint32_t url_info_len;  /**< Must be set to # of elements in url_info */
  uint8_t url_info[LWM2M_MSGR_MAX_URL_SIZE_V01];
  /**<   URL Information  */
}lwm2m_msgr_set_server_lifetime_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Configures the server life time information in the LWM2M client. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */
}lwm2m_msgr_set_server_lifetime_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Gets the server life time information from the LWM2M client. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.  */

  /* Optional */
  uint8_t url_info_valid;  /**< Must be set to true if url_info is being passed */
  uint32_t url_info_len;  /**< Must be set to # of elements in url_info */
  uint8_t url_info[LWM2M_MSGR_MAX_URL_SIZE_V01];
  /**<   URL Information */
}lwm2m_msgr_get_server_lifetime_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Gets the server life time information from the LWM2M client. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */

  /* Optional */
  uint8_t life_time_valid;  /**< Must be set to true if life_time is being passed */
  uint32_t life_time;
  /**<   Lifetime   */
}lwm2m_msgr_get_server_lifetime_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Utility function to encode application response/notification data. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.  */

  /* Mandatory */
  lwm2m_object_info_t_v01 obj_info;
  /**<   LwM2M object info. */

  /* Mandatory */
  uint32_t res_info_len;  /**< Must be set to # of elements in res_info */
  lwm2m_object_info_t_v01 res_info[LWM2M_MSGR_MAX_OBJ_CNT_V01];
  /**<   LwM2M object resource info list (to encode).   */

  /* Mandatory */
  uint32_t enc_content_type;
  /**<   Encoding data format. */

  /* Optional */
  uint8_t attr_info_valid;  /**< Must be set to true if attr_info is being passed */
  lwm2m_attributes_info_t_v01 attr_info;
  /**<   LwM2M attributes info. */
}lwm2m_msgr_encode_data_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Utility function to encode application response/notification data. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */

  /* Optional */
  uint8_t out_enc_data_valid;  /**< Must be set to true if out_enc_data is being passed */
  uint32_t out_enc_data_len;  /**< Must be set to # of elements in out_enc_data */
  uint8_t out_enc_data[LWM2M_MSGR_MAX_PAYLOAD_SIZE_V01];
  /**<   Output encoded resource data.   */
}lwm2m_msgr_encode_data_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Utility function to decode server request data. */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.  */

  /* Mandatory */
  lwm2m_object_info_t_v01 obj_info;
  /**<   Object Info. */

  /* Mandatory */
  uint32_t in_enc_data_len;  /**< Must be set to # of elements in in_enc_data */
  uint8_t in_enc_data[LWM2M_MSGR_MAX_PAYLOAD_SIZE_V01];
  /**<   Input encoded data. */

  /* Mandatory */
  uint32_t dec_content_type;
  /**<   Decoding data format.   */
}lwm2m_msgr_decode_data_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Utility function to decode server request data. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code.   */

  /* Optional */
  uint8_t qapi_status_valid;  /**< Must be set to true if qapi_status is being passed */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */

  /* Optional */
  uint8_t res_info_valid;  /**< Must be set to true if res_info is being passed */
  uint32_t res_info_len;  /**< Must be set to # of elements in res_info */
  lwm2m_object_info_t_v01 res_info[LWM2M_MSGR_MAX_OBJ_CNT_V01];
  /**<   LwM2M object resource info list (decoded). */
}lwm2m_msgr_decode_data_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Request Message; Registers or deregisters the control point to receive indications */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration.  */

  /* Optional */
  uint8_t register_ind_valid;  /**< Must be set to true if register_ind is being passed */
  uint8_t register_ind;
  /**<   Specifies whether to register LwM2M messages/event information. 
       Values:
       - 0 -- Do not register for LwM2M messages/event information 
       - 1 -- Register for QMI_LWM2M_MSGR_REGISTER_APP_EXTENDED_IND message (default)
  */

  /* Optional */
  uint8_t observe_ind_valid;  /**< Must be set to true if observe_ind is being passed */
  uint8_t observe_ind;
  /**<   Specifies whether to register for observation information. 
       Values:
       - 0 -- Do not register for observation information. 
       - 1 -- Register for QMI_LWM2M_MSGR_OBSERVE_V2_IND message (default)
   */
}lwm2m_msgr_indication_register_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Response Message; Registers or deregisters the control point to receive indications */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 resp;
  /**<   QMI response result code. */
}lwm2m_msgr_indication_register_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup lwm2m_msgr_qmi_messages
    @{
  */
/** Indication Message; Indicates any resource changes in the LWM2M client to the application 
	       associated with block-1 data information (internally used - no QAPI)     */
typedef struct {

  /* Mandatory */
  uint32_t handle;
  /**<   Lwm2M client handle provided to the application on successful registration. */

  /* Mandatory */
  uint32_t user_cookie;
  /**<   Transparent user data payload (provided during the registration).     */

  /* Mandatory */
  uint32_t qapi_status;
  /**<   LwM2M client QAPI status.  */

  /* Mandatory */
  lwm2m_object_info_t_v01 obj_info;
  /**<   LwM2M object info associated with the block-1 information */

  /* Mandatory */
  lwm2m_block1_info_t_v01 block_info;
  /**<   LwM2M block-1 information   */
}lwm2m_msgr_block1_observe_v2_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_LWM2M_MSGR_BLOCK1_OBSERVE_V2_IND_V01
//#define REMOVE_QMI_LWM2M_MSGR_INDICATION_REGISTER_V01

/*Service Message Definition*/
/** @addtogroup lwm2m_msgr_qmi_msg_ids
    @{
  */
#define QMI_LWM2M_MSGR_INDICATION_REGISTER_REQ_V01 0x0001
#define QMI_LWM2M_MSGR_INDICATION_REGISTER_RESP_V01 0x0001
#define QMI_LWM2M_MSGR_REGISTER_APP_EXTENDED_IND_V01 0x0002
#define QMI_LWM2M_MSGR_OBSERVE_V2_IND_V01 0x0003
#define QMI_LWM2M_MSGR_BLOCK1_OBSERVE_V2_IND_V01 0x0004
#define QMI_LWM2M_MSGR_REGISTER_APP_EXTENDED_REQ_V01 0x000B
#define QMI_LWM2M_MSGR_REGISTER_APP_EXTENDED_RESP_V01 0x000B
#define QMI_LWM2M_MSGR_DEREGISTER_APP_REQ_V01 0x000C
#define QMI_LWM2M_MSGR_DEREGISTER_APP_RESP_V01 0x000C
#define QMI_LWM2M_MSGR_OBSERVE_V2_REQ_V01 0x000D
#define QMI_LWM2M_MSGR_OBSERVE_V2_RESP_V01 0x000D
#define QMI_LWM2M_MSGR_CANCEL_OBSERVE_V2_REQ_V01 0x000E
#define QMI_LWM2M_MSGR_CANCEL_OBSERVE_V2_RESP_V01 0x000E
#define QMI_LWM2M_MSGR_CREATE_OBJ_INST_V2_REQ_V01 0x000F
#define QMI_LWM2M_MSGR_CREATE_OBJ_INST_V2_RESP_V01 0x000F
#define QMI_LWM2M_MSGR_DELETE_OBJ_INST_V2_REQ_V01 0x0010
#define QMI_LWM2M_MSGR_DELETE_OBJ_INST_V2_RESP_V01 0x0010
#define QMI_LWM2M_MSGR_GET_V2_REQ_V01 0x0011
#define QMI_LWM2M_MSGR_GET_V2_RESP_V01 0x0011
#define QMI_LWM2M_MSGR_SET_V2_REQ_V01 0x0012
#define QMI_LWM2M_MSGR_SET_V2_RESP_V01 0x0012
#define QMI_LWM2M_MSGR_SEND_MESSAGE_REQ_V01 0x0013
#define QMI_LWM2M_MSGR_SEND_MESSAGE_RESP_V01 0x0013
#define QMI_LWM2M_MSGR_WAKEUP_REQ_V01 0x0014
#define QMI_LWM2M_MSGR_WAKEUP_RESP_V01 0x0014
#define QMI_LWM2M_MSGR_CONFIG_CLIENT_REQ_V01 0x0015
#define QMI_LWM2M_MSGR_CONFIG_CLIENT_RESP_V01 0x0015
#define QMI_LWM2M_MSGR_DEFAULT_ATTRIBUTE_REQ_V01 0x0016
#define QMI_LWM2M_MSGR_DEFAULT_ATTRIBUTE_RESP_V01 0x0016
#define QMI_LWM2M_MSGR_SET_SERVER_LIFETIME_REQ_V01 0x0017
#define QMI_LWM2M_MSGR_SET_SERVER_LIFETIME_RESP_V01 0x0017
#define QMI_LWM2M_MSGR_GET_SERVER_LIFETIME_REQ_V01 0x0018
#define QMI_LWM2M_MSGR_GET_SERVER_LIFETIME_RESP_V01 0x0018
#define QMI_LWM2M_MSGR_ENCODE_DATA_REQ_V01 0x0019
#define QMI_LWM2M_MSGR_ENCODE_DATA_RESP_V01 0x0019
#define QMI_LWM2M_MSGR_DECODE_DATA_REQ_V01 0x001A
#define QMI_LWM2M_MSGR_DECODE_DATA_RESP_V01 0x001A
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro lwm2m_msgr_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type lwm2m_msgr_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define lwm2m_msgr_get_service_object_v01( ) \
          lwm2m_msgr_get_service_object_internal_v01( \
            LWM2M_MSGR_V01_IDL_MAJOR_VERS, LWM2M_MSGR_V01_IDL_MINOR_VERS, \
            LWM2M_MSGR_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

