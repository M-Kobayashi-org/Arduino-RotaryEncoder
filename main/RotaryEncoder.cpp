/**
 * @file RotaryEncoder.cpp
 * @author Masato Kobayashi (m.kobayashi.org@gmail.com)
 * @brief RotaryEncoder Class
 * @version 0.1
 * @date 2019-11-20
 * 
 * @copyright Copyright (c) 2019
 * 
 */
/**
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
#include <Arduino.h>
#include "RotaryEncoder.h"

//! Table for determining the rotation direction
const int16_t iMutations[] = {
  /* 0 -> 0 : 0 */  0,
  /* 0 -> 1 : + */  1,
  /* 0 -> 2 : - */ -1,
  /* 0 -> 3 : E */  0,
  /* 1 -> 0 : - */ -1,
  /* 1 -> 1 : 0 */  0,
  /* 1 -> 2 : E */  0,
  /* 1 -> 3 : + */  1,
  /* 2 -> 0 : + */  1,
  /* 2 -> 1 : E */  0,
  /* 2 -> 2 : 0 */  0,
  /* 2 -> 3 : - */ -1,
  /* 3 -> 0 : E */  0,
  /* 3 -> 1 : - */ -1,
  /* 3 -> 2 : + */  1,
  /* 3 -> 3 : 0 */  0,
};

/**
 * @brief Construct a new Rotary Encoder:: Rotary Encoder object
 * 
 * @param ioA 'A' port number
 * @param ioB 'B' port number
 * @param f Callback function
 */
RotaryEncoder::RotaryEncoder(uint16_t ioA, uint16_t ioB, recb_f f)
{
    // Store parameters
    iIoportA = ioA;
    iIoportB = ioB;
    fCallback = f;

    // IO port setting
    pinMode(iIoportA, INPUT_PULLUP);
    pinMode(iIoportB, INPUT_PULLUP);

    // Record previous value
    iOld = ((digitalRead(iIoportA) & 1) << 1) | (digitalRead(iIoportB) & 1);
}

/**
 * @brief Destroy the Rotary Encoder:: Rotary Encoder object
 * 
 */
RotaryEncoder::~RotaryEncoder()
{
}

/**
 * @brief Check the rotary encoder status and detect the direction of rotation
 * 
 * @details If the rotation direction is known, the Count function is called
 * 
 */
void RotaryEncoder::CheckValue()
{
    // Get current value
    uint16_t iNow = ((digitalRead(iIoportA) & 1) << 1) | (digitalRead(iIoportB) & 1);

    // Determine rotation direction from current location and previous value
    int16_t iMutation = iMutations[(iOld << 2) | iNow];
    // Save value this time
    iOld = iNow;

    // Call back direction of rotation
    if (fCallback && iMutation)
    {
        fCallback(iMutation);
    }

    // Connection check 
    if (tCheckBuf)
    {
        tCheckBuf[iNow]++;
    }
}

/////// Connection check
/**
 * @brief Clear connection confirmation buffer
 * 
 */
void RotaryEncoder::RefreshCheck()
{
    if (tCheckBuf) {
        for (int i = 0; CONNECTION_CHECK_BUF_LENGTH > i; i++)
        {
            tCheckBuf[i] = 0;
        }
    }
}

/**
 * @brief Connection confirmation start
 * 
 * @param buf Connection check data storage buffer pointer
 */
void RotaryEncoder::BeginCheck(uint16_t* buf)
{
    tCheckBuf = buf;
    RefreshCheck();
}
