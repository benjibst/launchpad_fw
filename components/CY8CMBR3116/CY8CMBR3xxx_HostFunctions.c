/*****************************************************************************
 * File Name: CY8CMBR3xxx_HostFunctions.c
 *
 * Version 1.00
 *
 * Description:
 *   This file contains the definitions of the low-level APIs. You may need to
 *   modify the content of these APIs to suit your host processor’s I2C
 *   implementation.
 *
 * Note:
 *   These host-dependent Low Level APIs are provided as an example of
 *   low level I2C read and write functions. This set of low level APIs are
 *   written for PSoC 4200/4100 devices and hence should be re-written
 *   with equivalent host-dependent APIs from the respective IDEs, if the
 *   host design does not include PSoC 4200/4100 device.
 *
 *   To use these APIs, the host should implement a working I2C communication
 *   interface. This interface will be used by these APIs to communicate to the
 *   CY8CMBR3xxx device.
 *
 *   For PSoC 4200/4100 devices, please ensure that you have created an instance
 *   of SCB component with I2C Master configuration. The component should be
 *   named "SCB".
 *
 * Owner:
 *   SRVS
 *
 * Related Document:
 *   MBR3 Design Guide
 *   MBR3 Device Datasheet
 *
 * Hardware Dependency:
 *   PSoC 4200 (Update this as per the host used)
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

/*******************************************************************************
 * Included headers
 *******************************************************************************/
#include "CY8CMBR3xxx_HostFunctions.h"
#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"

/*******************************************************************************
 * API Constants
 *******************************************************************************/
#define CY8CMBR3xxx_READ (1)
#define CY8CMBR3xxx_WRITE (0)
#define CY8CMBR3xxx_ACK (0)
#define CY8CMBR3xxx_NACK (1)
#define CY8CMBR3xxx_READ_BYTE_ERROR (0x80000000)

/* The following macro defines the maximum number of times the low-level read
 * and write functions try to communicate with the CY8CMBR3xxx device, as
 * long as the I2C communication is unsuccessful.
 */
#define CY8CMBR3xxx_RETRY_TIMES (10)

/*******************************************************************************
 *   Function Code
 *******************************************************************************/

/*******************************************************************************
 * Function Name: Host_LowLevelWrite
 ********************************************************************************
 *
 * Summary:
 *  This API writes to the register map of the CY8CMBR3xxx device using the I2C
 *  communication protocol. The implementation is host processor dependent and
 *  you may need to update the API code to suit your host.
 *
 * Parameters:
 *  i2c_master_dev_handle_t dev:
 *   The I2C address of the CY8CMBR3xxx device. Valid range: 8 - 119
 *
 *  uint8_t *writeBuffer:
 *   The buffer from which data is written to the device.
 *
 *   The first element should always contain the location of the register
 *   of the device to write to. This value can be within 0 – 251.
 *
 *   Each successive element should contain the data to be written to that
 *   register and the successive registers. These elements can have a value
 *   between 0 – 255. The number of data bytes can be between 0 and 128.
 *
 *  uint8_t numberOfBytes:
 *   Number of bytes to be written, equal to the number of elements in the
 *   buffer (i.e. number of data bytes + 1)
 *
 * Pre:
 *  The I2C interface should be enabled and working before this Low Level
 *  API can be called. Also make sure that the Global Interrupts are also
 *  enabled (if required)
 *
 * Post:
 *  N/A
 *
 * Return:
 *  status
 *    Value                Description
 *    TRUE                 Write process was successful
 *    FALSE                Write process was not successful
 *
 *******************************************************************************/
esp_err_t Host_LowLevelWrite(i2c_master_dev_handle_t dev, uint8_t *writeBuffer, uint8_t numberOfBytes)
{
    uint8_t retryCount = CY8CMBR3xxx_RETRY_TIMES; /* Retry count, in case of any issue */
    esp_err_t err;
    for (int i = 0; i < retryCount; i++)
    {
        err = i2c_master_transmit(dev, writeBuffer, numberOfBytes, 100);
        if (err == ESP_OK)
            break;
    }
    return err;
}

/*******************************************************************************
 * Function Name: Host_LowLevelRead
 ********************************************************************************
 *
 * Summary:
 *  This API reads from the register map of the CY8CMBR3xxx device using the
 *  I2C communication protocol. The implementation is host processor dependent
 *  and you may need to update the API code to suit your host.
 *
 * Parameters:
 *  i2c_master_dev_handle_t dev:
 *   The I2C address of the CY8CMBR3xxx device. Valid range: 8 - 119
 *
 *  uint8_t *readBuffer:
 *   The buffer to be updated with the data read from the device.
 *
 *   The register location to read from should be set prior to calling
 *   this API. Each successive element to contain the data read from that
 *   register and the successive registers. These elements can have a value
 *   between 0 – 255.
 *
 *  uint8_t numberOfBytes:
 *   Number of data bytes to be read, equal to the number of elements in
 *   the buffer. Valid range: 1 – 252
 *
 * Pre:
 *  The I2C interface should be enabled and working before this Low Level
 *  API can be called. Also make sure that the Global Interrupts are also
 *  enabled (if required)
 *
 * Post:
 *  N/A
 *
 * Return:
 *  status
 *    Value                Description
 *    TRUE                 Read process was successful
 *    FALSE                Read process was not successful
 *
 *******************************************************************************/
esp_err_t Host_LowLevelRead(i2c_master_dev_handle_t dev, uint8_t *readBuffer, uint8_t numberOfBytes)
{
    esp_err_t err;
    for (int i = 0; i < CY8CMBR3xxx_RETRY_TIMES; i++)
    {
        err = i2c_master_receive(dev, readBuffer, numberOfBytes, 100);
        if (err == ESP_OK)
            break;
    }
    return err;
}

/*******************************************************************************
 * Function Name: Host_LowLevelDelay
 ********************************************************************************
 *
 * Summary:
 *  This API implements a time-delay function to be used by the High-level APIs.
 *  The delay period is in milliseconds. This delay is achieved by a
 *  code-execution block for the required amount of time.
 *
 *  The implementation is host processor dependent and you need to update the
 *  API code to suit your host.
 *
 * Parameters:
 *  uint16_t milliseconds:
 *   The amount of time in milliseconds for which a wait is required.
 *   Valid range: 0 – 65535
 *
 * Return:
 *  None
 *
 *******************************************************************************/
void Host_LowLevelDelay(uint16_t milliseconds)
{
    vTaskDelay(milliseconds / portTICK_PERIOD_MS);
}

/****************************End of File***************************************/
