/**
 * @file Timer.cpp
 * @author Masato Kobayashi (m.kobayashi.org@gmail.com)
 * @brief Timer Class
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
#include "Timer.h"

//! CPU frequency
#define CPU_FREQUENCY                       (16L * 1000L * 1000L)
//! Divided value at timer interrupt
#define DIVISION_NUMBER                     8
// CS value of Timer1
#define TCCR1B_CS1_1                        (bit(CS10))
#define TCCR1B_CS1_8                        (bit(CS11))
#define TCCR1B_CS1_64                       (bit(CS11) | bit(CS10))
#define TCCR1B_CS1_256                      (bit(CS12))
#define TCCR1B_CS1_1024                     (bit(CS12) | bit(CS10))
#define __TCCR1B_CS1__( n )                 TCCR1B_CS1_ ## n
#define _TCCR1B_CS1_( n )                   __TCCR1B_CS1__( n )
#define TCCR1B_CS1                          _TCCR1B_CS1_( DIVISION_NUMBER )
//! OCR value equivalent to 100 microseconds
#define OCR_COUNT                           ((uint16_t)((CPU_FREQUENCY / (uint32_t)DIVISION_NUMBER) / 1000L / 1000L * 100L))

//! Number of instances that can be used simultaneously
#define MAX_INSTANCE                        1
/**
 * @brief Instance operational status
 * 
 */
enum {
    //! Stopping
    STOP,
    //! Operating in multi-shot mode
    START,
    //! Operating in single shot mode
    ONCE,
};

//! Number of instances created
uint16_t iInstance = 0;
/**
 * @brief Variables referenced by each instance
 * 
 * @details In interrupt processing, member variables cannot be referenced.
 *          Prepare statically allocated variables for each instance.
 * 
 */
volatile struct {
    isr_f fCallBack;                        // Callback
    uint32_t lTimeMicroseconds;             // Set time
    uint32_t lRemaining;                    // Remaining time
    uint16_t iMode;                         // Operating state
} tAttaches[MAX_INSTANCE] = {
    {.fCallBack = NULL, .lTimeMicroseconds = 0L, .lRemaining = 0L, .iMode = STOP,},
};

/**
 * @brief Initialize timer interrupt
 * 
 */
void SetupTimerOne()
{
    if (iInstance)   return;

    // Stop interrupt
    noInterrupts();

    TCNT1 = 0;
    TCCR1A = 0;
    TCCR1B = bit(WGM12) | TCCR1B_CS1;                               // CTC mode
    OCR1A = OCR_COUNT;                                              // Set interrupt interval
    TIMSK1 = bit(OCIE1A);                                           // Set timer 1 interrupt

    // Resume interrupt
    interrupts();
}

/**
 * @brief Construct a new Timer:: Timer object
 * 
 */
Timer::Timer()
{
    SetupTimerOne();
    if (MAX_INSTANCE > iInstance)
    {
        tAttaches[iInstance].iMode = STOP;
        m_AttachIndex = iInstance;
        iInstance++;
    }
}

/**
 * @brief Register a multi-shot timer interrupt
 * 
 * @param millisecond 
 * @param callBack 
 */
void Timer::attach_ms(uint32_t millisecond, isr_f callBack)
{
    if (0 > m_AttachIndex) return;

    tAttaches[m_AttachIndex].fCallBack = callBack;
    tAttaches[m_AttachIndex].lRemaining =
    tAttaches[m_AttachIndex].lTimeMicroseconds = millisecond * 1000L / 100L;
    tAttaches[m_AttachIndex].iMode = START;
}

/**
 * @brief Register a multi-shot timer interrupt
 * 
 * @param microsecond 
 * @param callBack 
 */
void Timer::attach_us(uint32_t microsecond, isr_f callBack)
{
    if (0 > m_AttachIndex) return;

    tAttaches[m_AttachIndex].fCallBack = callBack;
    tAttaches[m_AttachIndex].lRemaining =
    tAttaches[m_AttachIndex].lTimeMicroseconds = microsecond / 100L;
    tAttaches[m_AttachIndex].iMode = START;
}

/**
 * @brief Register single shot timer interrupt
 * 
 * @param millisecond 
 * @param callBack 
 */
void Timer::once_ms(uint32_t millisecond, isr_f callBack)
{
    if (0 > m_AttachIndex) return;

    tAttaches[m_AttachIndex].fCallBack = callBack;
    tAttaches[m_AttachIndex].lRemaining =
    tAttaches[m_AttachIndex].lTimeMicroseconds = millisecond * 1000L / 100L;
    tAttaches[m_AttachIndex].iMode = ONCE;
}

/**
 * @brief Register single shot timer interrupt
 * 
 * @param microsecond 
 * @param callBack 
 */
void Timer::once_us(uint32_t microsecond, isr_f callBack)
{
    if (0 > m_AttachIndex) return;

    tAttaches[m_AttachIndex].fCallBack = callBack;
    tAttaches[m_AttachIndex].lRemaining =
    tAttaches[m_AttachIndex].lTimeMicroseconds = microsecond / 100L;
    tAttaches[m_AttachIndex].iMode = ONCE;
}

/**
 * @brief Discard timer interrupt
 * 
 */
void Timer::detach()
{
    if (0 > m_AttachIndex) return;

    tAttaches[m_AttachIndex].iMode = STOP;
    tAttaches[m_AttachIndex].fCallBack = NULL;
}

/**
 * @brief Destroy the Timer:: Timer object
 * 
 */
Timer::~Timer()
{
    if (0 > m_AttachIndex) return;

    tAttaches[m_AttachIndex].iMode = STOP;
    tAttaches[m_AttachIndex].fCallBack = NULL;
    m_AttachIndex -1;
}

/**
 * @brief Construct a new ISR object
 * 
 */
ISR (TIMER1_COMPA_vect)
{
    for (int i = 0; MAX_INSTANCE > i; i++)
    {
        if (tAttaches[i].fCallBack != NULL && tAttaches[i].iMode != STOP)
        {
            tAttaches[i].lRemaining--;
            if (!tAttaches[i].lRemaining)
            {
                tAttaches[i].fCallBack();
                if (tAttaches[i].iMode == ONCE)
                {
                    tAttaches[i].iMode = STOP;
                }
                tAttaches[i].lRemaining = tAttaches[i].lTimeMicroseconds;
            }
        }
    }
}
