/**
 * @file main.ino
 * @author Masato Kobayashi (m.kobayashi.org@gmail.com)
 * @brief Sample rotary encoder library
 *        ロータリーエンコーダライブラリのサンプル
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

/**
 * Incremental rotary encoders can be thought of as a mechanism in which two switches are turned On and Off at different timings.
 * Therefore, the following conditions can be detected.
 * インクリメンタル型ロータリー・エンコーダでは、2つのスイッチがタイミングをずらして On Off する機構と考えることができます。
 * そのため、以下のような状態を検出することができます。
 * 
 * SW1: O O S S O O S S
 * SW2: S O O S S O O S
 * (O:open, S:Short)
 * 
 * If the connection at that time is correct, the following signals can be detected.
 * その際の結線が正しい場合は、以下のような信号を検出できます。
 * 
 * A:   H H L L H H L L
 * B:   L H H L L H H L
 *      2 3 1 0 2 3 1 0
 * 
 * A:   L H H L L H H L
 * B:   H H L L H H L L
 *      1 3 2 0 1 3 2 0
 * 
 * In this way, all values 0, 1, 2, and 3 can be detected.
 * このように、0,1,2,3 全ての値が検出できます。
 * 
 * 
 * On the other hand, if the connection is incorrect, the following signals are detected.
 * 一方、結線を間違えると、以下のような信号が検出されます。
 * 
 * A:   H H H L H H H L
 * B:   L H H L L H H L
 *      2 3 3 0 2 3 3 0
 * 
 * A:   L H H L L H H L
 * B:   H H H L H H H L
 *      1 3 3 0 1 3 3 0
 * 
 * A:   H H H L H H H L
 * B:   L H H L L H H L
 *      2 3 3 0 2 3 3 0
 * 
 * A:   L H H L L H H L
 * B:   H H H L H H H L
 *      1 3 3 0 1 3 3 0
 * 
 * As you can see, 0,2,3 or 0,1,3 cannot be detected.
 * By confirming this, it is possible to determine the wrong connection.
 * ここでわかるように 0,2,3 または、0,1,3 といずれかの値が検出できません。
 * このことを確認することで、結線の間違いを判定することができます。
 * 
 */
#include "Timer.h"
#include "RotaryEncoder.h"

#define RotaryEncoder_A   3
#define RotaryEncoder_B   2

uint32_t iCount = 0L;

Timer* timer;
RotaryEncoder* renc;

//! Interval timer interrupt flag (true: interrupt processing in progress)
//! インターバルタイマー割り込みフラグ（true:割り込み処理中） 
volatile bool bInInterval = false;

//! Rotary encoder connection check buffer
//! ロータリーエンコーダの結線確認用バッファ 
ConnectionCheck_t tCheck = {0,};

/**
 * @brief Interval timer interrupt handler
 *        インターバルタイマーによる割り込みのハンドラ
 * 
 * @details In interrupt processing, just set the flag to 
 *          minimize processing time.
 *          割り込み処理では処理時間が最小になるように
 *          フラグをセットするだけとする
 */
void Interval()
{
  bInInterval = true;
}

uint16_t iRight = 0, iLeft = 0;
uint16_t iRightOld = 0, iLeftOld = 0;
/**
 * @brief Function called after the direction of rotation is known
 *        回転方向が判明した後にコールされる関数
 * 
 * @param val A value indicating the direction of rotation
 * 　　　　　　回転方向を示す値
 * 　　　　　　 1 : Clockwise
 *                 時計回り
 *            -1 : Counterclockwise
 *                 反時計回り
 */
void Count(int16_t val)
{
  if (val > 0)
  {
    iLeft++;
  }
  else
  {
    iRight++;
  }
}

/**
 * @brief Arduino Setup routine
 *        Arduinoセットアップルーチン
 * 
 */
void setup() {
  // initialize serial communication at 115200 bits per second:
  // シリアル通信を115200ビット/秒で初期化
  Serial.begin(115200);

  Serial.println("Start Arduino Rotary Encoder.");

  // Instantiating a rotary encoder object
  //     Specify signal input pins
  //     When the rotation direction can be determined, 
  //     the Count function is called
  // ロータリーエンコーダオブジェクトのインスタンス化
  //     信号の入力ピンを指定する 
  //     回転方向が判定できたら Count 関数がコールされる 
  renc = new RotaryEncoder(RotaryEncoder_A, RotaryEncoder_B, Count);

  // Started checking the connection of the rotary encoder
  // ロータリーエンコーダの結線確認を開始 
  renc->BeginCheck(tCheck);

  // Instantiating an interval timer object
  // インターバルタイマオブジェクトのインスタンス化 
  timer = new Timer();
  // Start interval timer
  //     Interval function is called every 0.5ms
  // インターバルタイマ開始 
  //     0.5ms 間隔で Interval 関数がコールされる 
  timer->attach_us(500L, Interval);
}

/**
 * @brief Arduino loop function
 *        Arduinoループ機能
 * 
 */
void loop() {
  // When an interval timer interrupt occurs
  // インターバルタイマ割り込みが発生した時 
  if (bInInterval)
  {
    // Check the rotary encoder status and detect 
    // the direction of rotation
    // If the rotation direction is known, 
    // the Count function is called
    // ロータリーエンコーダの状態を確認して 
    // 回転方向を検出します 
    // 回転方向が判明した場合は Count 関数がコールされます 
    renc->CheckValue();
    bInInterval = false;
  }
  // Output status regularly
  // 定期的に状況を出力する 
  if (!(iCount++ & 0x0fffff))
  {
    int16_t iRightDelta = iRight - iRightOld;
    int16_t iLeftDelta = iLeft - iLeftOld;

    Serial.print("Right:");
    Serial.print(iRight);
    Serial.print(" (");
    if (iRightDelta)
      Serial.print("+");
    Serial.print(iRightDelta);
    Serial.print("), Left:");
    Serial.print(iLeft);
    Serial.print(" (");
    if (iLeftDelta)
      Serial.print("+");
    Serial.print(iLeftDelta);
    Serial.print(")   ");

    for (int i = 0; CONNECTION_CHECK_BUF_LENGTH > i; i++)
    {
      if (i)
      {
        Serial.print(", ");
      }
      Serial.print(i);
      Serial.print(":");
      Serial.print(tCheck[i]);
    }

    Serial.println();

    iRightOld = iRight;
    iLeftOld = iLeft;
  }
}
