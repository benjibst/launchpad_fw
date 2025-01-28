/*****************************************************************************
 * File Name: CY8CMBR3xxx_APIs.h
 *
 * Version 1.00
 *
 * Description:
 *   This file contains the declarations of all the high-level APIs.
 *
 * Note:
 *   N/A
 *
 * Owner:
 *   SRVS
 *
 * Related Document:
 *   MBR3 Design Guide
 *   MBR3 Device Register TRM
 *   MBR3 Device Datasheet
 *
 * Hardware Dependency:
 *   N/A
 *
 * Code Tested With:
 *   PSoC Creator 3.0 CP7
 *   CY3280-MBR3 Evaluation Kit
 *   CY8CKIT-042 Pioneer Kit
 *
 ******************************************************************************
 * Copyright (2014), Cypress Semiconductor Corporation.
 ******************************************************************************
 * This software is owned by Cypress Semiconductor Corporation (Cypress) and is
 * protected by and subject to worldwide patent protection (United States and
 * foreign), United States copyright laws and international treaty provisions.
 * Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
 * license to copy, use, modify, create derivative works of, and compile the
 * Cypress Source Code and derivative works for the sole purpose of creating
 * custom software in support of licensee product to be used only in conjunction
 * with a Cypress integrated circuit as specified in the applicable agreement.
 * Any reproduction, modification, translation, compilation, or representation of
 * this software except as specified above is prohibited without the express
 * written permission of Cypress.
 *
 * Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
 * REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * Cypress reserves the right to make changes without further notice to the
 * materials described herein. Cypress does not assume any liability arising out
 * of the application or use of any product or circuit described herein. Cypress
 * does not authorize its products for use as critical components in life-support
 * systems where a malfunction or failure may reasonably be expected to result in
 * significant injury to the user. The inclusion of Cypress' product in a life-
 * support systems application implies that the manufacturer assumes all risk of
 * such use and in doing so indemnifies Cypress against all charges. Use may be
 * limited by and subject to the applicable Cypress software license agreement.
 *****************************************************************************/

#if !defined(CY8CMBR3xxx_APIS_H)
#define CY8CMBR3xxx_APIS_H

/*******************************************************************************
 * Included headers
 *******************************************************************************/
#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

/*******************************************************************************
 * User defined Macros
 *******************************************************************************/
/* The APIs CY8CMBR3xxx_ReadDiffCounts() and CY8CMBR3xxx_ReadSensorDebugData()
 * try to read from the device for a maximum number of times defined by this
 * value, as long as the sync counters do not match. Please change as required.
 */
#define CY8CMBR3xxx_SYNC_COUNTER_MATCH_RETRY (10)

/*******************************************************************************
 * Data Type Definitions
 *******************************************************************************/
#if !defined(CY8CMBR3xxx_DATATYPE_GUARD)
#define CY8CMBR3xxx_DATATYPE_GUARD

#endif

/*******************************************************************************
 * Structure Definitions
 *******************************************************************************/
/*
 * This structure will hold the debug data read from the device by the
 * CY8CMBR3xxx_ReadSensorData() API.
 */
typedef struct
{
    uint8_t sensorCp;
    uint16_t sensorDiffCounts;
    uint16_t sensorBaseline;
    uint16_t sensorRawCounts;
    uint16_t sensorAverageCounts;
} CY8CMBR3xxx_SENSORDATA;

/*
 * This structure will hold the sensor status read from the device
 * by the CY8CMBR3xxx_ReadSensorStatus() API.
 * The slider positions and slider liftoff positions are valid for
 * CY8CMBR3106S device only.
 */
typedef struct
{
    uint16_t buttonStatus;
    uint16_t latchedButtonStatus;
    uint8_t proxStatus;
    uint8_t latchedProxStatus;
    uint8_t slider1Position;
    uint8_t liftoffSlider1Position;
    uint8_t slider2Position;
    uint8_t liftoffSlider2Position;
} CY8CMBR3xxx_SENSORSTATUS;

/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/
esp_err_t CY8CMBR3xxx_WriteData(i2c_master_dev_handle_t dev, uint8_t *writeBuffer, uint8_t numberOfBytes);
esp_err_t CY8CMBR3xxx_ReadData(i2c_master_dev_handle_t dev, uint8_t registerAddress, uint8_t *readBuffer, uint8_t numberOfBytes);
esp_err_t CY8CMBR3xxx_WriteDualByte(i2c_master_dev_handle_t dev, uint8_t registerAddress, uint16_t writeData);
esp_err_t CY8CMBR3xxx_ReadDualByte(i2c_master_dev_handle_t dev, uint8_t registerAddress, uint16_t *readData);
esp_err_t CY8CMBR3xxx_SendCommand(i2c_master_dev_handle_t dev, uint8_t command);
uint8_t CY8CMRB3xxx_CheckCommandStatus(i2c_master_dev_handle_t dev, uint8_t *errorCode);
esp_err_t CY8CMBR3xxx_Configure(i2c_master_dev_handle_t dev, const unsigned char *configuration);
uint16_t CY8CMBR3xxx_CalculateCrc(uint8_t *configuration);
esp_err_t CY8CMBR3xxx_VerifyDeviceOnBus(i2c_master_dev_handle_t dev);
esp_err_t CY8CMBR3xxx_SetDebugDataSensorId(i2c_master_dev_handle_t dev, uint8_t sensorId);
esp_err_t CY8CMBR3xxx_ReadSensorDebugData(i2c_master_dev_handle_t dev, CY8CMBR3xxx_SENSORDATA *debugData);
esp_err_t CY8CMBR3xxx_ReadDiffCounts(i2c_master_dev_handle_t dev, uint16_t *differenceCounts);
esp_err_t CY8CMBR3xxx_ReadSensorStatus(i2c_master_dev_handle_t dev, CY8CMBR3xxx_SENSORSTATUS *status);

#endif

/****************************End of File***************************************/
