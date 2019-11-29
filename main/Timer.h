/**
 * @file Timer.h
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
#ifndef _TIMER_H_
#define _TIMER_H_

// Caller function type declaration
typedef void (*isr_f)(void);

/**
 * @brief Timer Class
 * 
 */
class Timer
{
private:
    //! The number of the created instance
    int16_t m_AttachIndex = -1;

public:
    Timer();
    ~Timer();

    void attach_ms(uint32_t, isr_f);
    void attach_us(uint32_t, isr_f);
    void once_ms(uint32_t, isr_f);
    void once_us(uint32_t, isr_f);
    void detach();
};

#endif
