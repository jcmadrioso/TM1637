/***************************************
* @file Clock_ESP32.ino
* @ingroup examples
* @brief Clock example for ESP32 with timers
*
* With this example, the ESP32 displays
* a clock that counts from 00:00.
*
* It uses ESP32 Timer0 with a prescaler of 80,
* for a clock of 80MHz. Adjust the prescaler
* value acording to the board clock frequency.
****************************************/

/** API:
 *
 * Visit https://github.com/AKJ7/TM1637 for more information
 *
 * @code{.cpp}
 * class TM1637
 * {
 *      TM1637(uint8_t clkPin, uint8_t dataPin, uint8_t bufferSize = TOTAL_DIGITS);
 *      void init();
 *      void begin();
 *      const Animator& refresh();
 *      void update();
 *      display(T value, bool overflow = true, bool pad = true, uint8_t offset = 0);
 *      template <typename T>
 *      DEPRECATED void dispNumber(T value)
 *      void sendToDisplay(DisplayControl_e displayControl) const noexcept;
 *      template <DataCommand_e dataCommand, AddressCommand_e addressCommand>
 *      void sendToDisplay(uint8_t* values, uint8_t size) const noexcept;
 *      template <DataCommand_e dataCommand, AddressCommand_e addressCommand, typename ... Ts>
 *      void sendToDisplay(Ts ... values);
 *      void setBrightness(uint8_t value);
 *      void changeBrightness(uint8_t value);
 *      void setDp(uint8_t value);
 *      uint8_t getBrightness();
 *      void offMode();
 *      void onMode();
 *      void colonOn();
 *      void colonOff();
 *      void switchColon();
 *      void clearScreen();
 * }
 * @endcode
 */

#include <TM1637.h>


volatile uint8_t minutes = 0, seconds = 0;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// Timer ISR
void IRAM_ATTR timer_isr() {
   portENTER_CRITICAL_ISR(&timerMux);
   seconds++;
   if (seconds == 60) {
    minutes++;
    seconds = 0;
   }
   portEXIT_CRITICAL_ISR(&timerMux);
}

// Instantiation and pins configurations
// Pin 3 - > DIO
// Pin 2 - > CLK
TM1637 tm1637(22, 23);

void setup()
{
    tm1637.init();
    tm1637.setBrightness(5);

   // Set the prescaler to 80. Quartz of the ESP32 is at 80Mhz, so:
   // 80000000 / 80 = 1000000 tics/sec
   timer = timerBegin(0, 80, true);  // Timer0, prescaler=80, rising edge
   timerAttachInterrupt(timer, &timer_isr, true);  // Trigger=true -> trigger by edge
    
   // Set alarm to trigger every 1e6 tics ( 1 second)
   timerAlarmWrite(timer, 1000000, true);   // autoreload=true   
   timerAlarmEnable(timer);
}

void loop()
{
            tm1637.display(seconds + minutes * 100);
            delay(500);            // Waits for 0.5 second = 500 millisecond.
            tm1637.switchColon();
}
