/**
 * @file RotaryEncoder.h
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
#ifndef _ROTARY_ENCODER_H_
#define _ROTARY_ENCODER_H_

#include <stdint.h>
#include <stdio.h>

// Caller function type declaration
typedef void (*recb_f)(int16_t);

// Data buffer for connection inspection 
#define CONNECTION_CHECK_BUF_LENGTH         4
typedef uint16_t ConnectionCheck_t[CONNECTION_CHECK_BUF_LENGTH];

/**
 * @brief RotaryEncoder Class
 * 
 */
class RotaryEncoder
{
private:
    // Callback function
    recb_f fCallback = NULL;
    // IO port number
    uint16_t iIoportA;
    uint16_t iIoportB;
    // Previous value
    uint16_t iOld;
    //  
    bool bDoConnectionCheck = false;
    uint16_t* tCheckBuf = NULL;

public:
    RotaryEncoder(uint16_t, uint16_t, recb_f);
    ~RotaryEncoder();

    void CheckValue(void);

    // Connection check
    void BeginCheck(uint16_t*);
    void RefreshCheck(void);

};

#endif
