/**
   This code converts the Palm Portable Keyboard's output into bluetooth.
   See https://github.com/pymo/ppk_bluetooth for details.

*/

#include <driver/uart.h>
#include <esp_sleep.h>

#include "BleKeyboard.h"

// Define to enable debug output through the arduino console at 115200
// Comment out to disable debug output.
// #define PPK_DEBUG

// Define to compile firmware for Handspring keyboard
// Comment out to compile firmware for Palm III or V keyboard
// #define HANDSPRING

// Uncomment to put the firmware in battery test mode. It prints
// the raw battery reading to the bluetooth host. It also disables the
// sleep function.
// #define BATTERY_TEST_MODE

#ifdef HANDSPRING
// Handspring pinout
#define VCC_PIN 4
#define VCC_PIN_NUM GPIO_NUM_4
#define GND_PIN 6
#define GND_PIN_NUM GPIO_NUM_6
#define RX_PIN 10
#define RX_PIN_NUM GPIO_NUM_10
#define TX_PIN 19
#define HEX_ID0 0xF9
#define HEX_ID1 0xFB
#define INVERT_TTL false
#else
// Palm III or V pinout
#define VCC_PIN 4
#define VCC_PIN_NUM GPIO_NUM_4
#define RTS_PIN 6
#define RTS_PIN_NUM GPIO_NUM_6
#define DCD_PIN 7
#define GND_PIN 10
#define GND_PIN_NUM GPIO_NUM_10
#define RX_PIN 5
#define RX_PIN_NUM GPIO_NUM_5
#define TX_PIN 19
#define HEX_ID0 0xFA
#define HEX_ID1 0xFD
#define INVERT_TTL true
#endif

#define BATTERY_ADC_PIN 2
#define BATTERY_LED 9
#define FUNC_LED 3

void ledOff(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}
void ledOn(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

// wait this many milliseconds before making sure keyboard is still awake
#define KEEPALIVE_TIMEOUT 500000

// if the keyboard has been idled for 30 min, enter light sleep.
#define IDLE_TIMEOUT 1800000

// if the keyboard can not initialize after 3 seconds, reboot the board.
#define KEYBOARD_INIT_TIMEOUT 3000

// Conversion factor for micro seconds to mili seconds
#define uS_TO_mS_FACTOR 1000

BleKeyboard bleKeyboard(/*deviceName=*/"Palm Keyboard",
                        /*deviceManufacturer=*/"Xinming Chen");

char key_map[128] = {0};
char fn_key_map[128] = {0};

char last_byte = 0;

// The timestamp of the last time the keyboard has communicated with the board.
unsigned long last_comm = 0;
// The timestamp of the last time there is a key been pressed.
unsigned long last_pressed = 0;

void config_keymap() {
  // y0 row
  key_map[0b00000000] = '1';
  key_map[0b00000001] = '2';
  key_map[0b00000010] = '3';
  key_map[0b00000011] = 'z';
  key_map[0b00000100] = '4';
  key_map[0b00000101] = '5';
  key_map[0b00000110] = '6';
  key_map[0b00000111] = '7';

  // y1 row
  key_map[0b00001000] = KEY_LEFT_GUI;  // "CMMD" or "Cmd"
  key_map[0b00001001] = 'q';
  key_map[0b00001010] = 'w';
  key_map[0b00001011] = 'e';
  key_map[0b00001100] = 'r';
  key_map[0b00001101] = 't';
  key_map[0b00001110] = 'y';
  key_map[0b00001111] = '`';

  // y2 row
  key_map[0b00010000] = 'x';
  key_map[0b00010001] = 'a';
  key_map[0b00010010] = 's';
  key_map[0b00010011] = 'd';
  key_map[0b00010100] = 'f';
  key_map[0b00010101] = 'g';
  key_map[0b00010110] = 'h';
  key_map[0b00010111] = ' ';  // "Space 1"

  // y3 row
  key_map[0b00011000] = KEY_CAPS_LOCK;
  key_map[0b00011001] = KEY_TAB;
  key_map[0b00011010] = KEY_LEFT_CTRL;
  key_map[0b00011011] = 0;
  key_map[0b00011100] = 0;
  key_map[0b00011101] = 0;
  key_map[0b00011110] = 0;
  key_map[0b00011111] = 0;

  // y4 row
  key_map[0b00100000] = 0;
  key_map[0b00100001] = 0;
  key_map[0b00100010] = 0;  // Fn key
  key_map[0b00100011] = KEY_LEFT_ALT;
  key_map[0b00100100] = 0;
  key_map[0b00100101] = 0;
  key_map[0b00100110] = 0;
  key_map[0b00100111] = 0;

  // y5 row
  key_map[0b00101000] = 0;
  key_map[0b00101001] = 0;
  key_map[0b00101010] = 0;
  key_map[0b00101011] = 0;
  key_map[0b00101100] = 'c';
  key_map[0b00101101] = 'v';
  key_map[0b00101110] = 'b';
  key_map[0b00101111] = 'n';

  // y6 row
  key_map[0b00110000] = '-';
  key_map[0b00110001] = '=';
  key_map[0b00110010] = KEY_BACKSPACE;
  key_map[0b00110011] = KEY_HOME;  // "Special Function One"
  key_map[0b00110100] = '8';
  key_map[0b00110101] = '9';
  key_map[0b00110110] = '0';
  key_map[0b00110111] = ' ';  // "Space 2"

  // y7 row
  key_map[0b00111000] = '[';
  key_map[0b00111001] = ']';
  key_map[0b00111010] = '\\';
  key_map[0b00111011] = KEY_END;  // "Special Function Two"
  key_map[0b00111100] = 'u';
  key_map[0b00111101] = 'i';
  key_map[0b00111110] = 'o';
  key_map[0b00111111] = 'p';

  // y8 row
  key_map[0b01000000] = '\'';
  key_map[0b01000001] = KEY_RETURN;
  key_map[0b01000010] = KEY_PAGE_UP;  // "Special Function Three"
  key_map[0b01000011] = 0;
  key_map[0b01000100] = 'j';
  key_map[0b01000101] = 'k';
  key_map[0b01000110] = 'l';
  key_map[0b01000111] = ';';

  // y9 row
  key_map[0b01001000] = '/';
  key_map[0b01001001] = KEY_UP_ARROW;
  key_map[0b01001010] = KEY_PAGE_DOWN;  // "Special Function Four"
  key_map[0b01001011] = 0;
  key_map[0b01001100] = 'm';
  key_map[0b01001101] = ',';
  key_map[0b01001110] = '.';
  key_map[0b01001111] = KEY_INSERT;  // "DONE" or "Done"

  // y10 row
  key_map[0b01010000] = KEY_DELETE;
  key_map[0b01010001] = KEY_LEFT_ARROW;
  key_map[0b01010010] = KEY_DOWN_ARROW;
  key_map[0b01010011] = KEY_RIGHT_ARROW;
  key_map[0b01010100] = 0;
  key_map[0b01010101] = 0;
  key_map[0b01010110] = 0;
  key_map[0b01010111] = 0;

  // y11 row
  key_map[0b01011000] = KEY_LEFT_SHIFT;
  key_map[0b01011001] = KEY_RIGHT_SHIFT;
  key_map[0b01011010] = 0;
  key_map[0b01011011] = 0;
  key_map[0b01011100] = 0;
  key_map[0b01011101] = 0;
  key_map[0b01011110] = 0;
  key_map[0b01011111] = 0;
}

void config_fnkeymap() {
  fn_key_map[0b00011001] = KEY_ESC;  // tab key
  fn_key_map[0b00000000] = KEY_F1;   // 1
  fn_key_map[0b00000001] = KEY_F2;   // 2
  fn_key_map[0b00000010] = KEY_F3;   // 3
  fn_key_map[0b00000100] = KEY_F4;   // 4
  fn_key_map[0b00000101] = KEY_F5;   // 5
  fn_key_map[0b00000110] = KEY_F6;   // 6
  fn_key_map[0b00000111] = KEY_F7;   // 7
  fn_key_map[0b00110100] = KEY_F8;   // 8
  fn_key_map[0b00110101] = KEY_F9;   // 9
  fn_key_map[0b00110110] = KEY_F10;  // 0
  fn_key_map[0b00110000] = KEY_F11;  // -
  fn_key_map[0b00110001] = KEY_F12;  // =
}

void boot_keyboard() {
#ifdef PPK_DEBUG
  Serial.println("beginning keyboard boot sequence");
#endif

#ifdef HANDSPRING
  pinMode(RX_PIN, INPUT_PULLUP);
#else
  pinMode(DCD_PIN, INPUT);
  pinMode(RTS_PIN, INPUT);
  pinMode(RX_PIN, INPUT_PULLDOWN);
#endif

  pinMode(VCC_PIN, OUTPUT);
  digitalWrite(VCC_PIN, LOW);
  pinMode(GND_PIN, OUTPUT);
  digitalWrite(GND_PIN, LOW);
  delay(100);
  digitalWrite(VCC_PIN, HIGH);

#ifndef HANDSPRING
  while (digitalRead(DCD_PIN) != HIGH) {
    delay(1);
  };

#ifdef PPK_DEBUG
  Serial.println("DCD_PIN response done");
#endif

  if (digitalRead(RTS_PIN) == LOW) {
    delay(10);
    pinMode(RTS_PIN, OUTPUT);
    digitalWrite(RTS_PIN, HIGH);
  } else {
    pinMode(RTS_PIN, OUTPUT);
    digitalWrite(RTS_PIN, HIGH);
    digitalWrite(RTS_PIN, LOW);
    delay(10);
    digitalWrite(RTS_PIN, HIGH);
  }
#endif  // #ifndef HANDSPRING

#ifdef PPK_DEBUG
  Serial.print("waiting for keyboard serial ID... ");
#endif

  // Read the Hardware Serial1 until we get the expected hex ID. Due to the
  // timing of plugging in the adapter, there could be garbage chars before the
  // ID, the following code will ignore them.
  int byte1, byte2 = 0;
  int start_time = millis();
  while (true) {
    delay(10);
    if (Serial1.available()) {
      byte1 = byte2;
      byte2 = Serial1.read();
#ifdef PPK_DEBUG
      Serial.print(byte2);
      Serial.print(" ");
#endif
    }
    if ((byte1 == HEX_ID0) && (byte2 == HEX_ID1)) break;
    if (millis() - start_time > KEYBOARD_INIT_TIMEOUT) {
#ifdef PPK_DEBUG
      Serial.print(
          "No keyboard ID received, something is wrong, rebooting the board.");
#endif
      ESP.restart();
    }
  }

#ifdef PPK_DEBUG
  Serial.println(" done");
#endif
}

void setup() {
  setCpuFrequencyMhz(80);
#ifdef PPK_DEBUG
  Serial.begin(115200);
#endif
  ledOn(BATTERY_LED);
  ledOn(FUNC_LED);
  CheckBatteryWithInterval();
  // The adapter may have just been plugged in and is not fully contacting
  // with the keyboard's golden finger yet. Wait for 1s before doing anything.
  delay(500);

  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN, /*invert=*/INVERT_TTL);

  config_keymap();
  config_fnkeymap();
  boot_keyboard();
  ledOff(BATTERY_LED);
  bleKeyboard.begin();
#ifdef PPK_DEBUG
  Serial.println("setup completed");
#endif
}

bool fn_key_down = false;

void ReportKeyUpDownEvent(uint8_t key_code, bool key_up) {
  if (key_up) {
    bleKeyboard.release(key_code);
  } else {
    bleKeyboard.press(key_code);
  }
}

void PrintStringToKeyboard(char *str) {
  for (int i = 0; i < strlen(str); ++i) {
    char to_be_sent = str[i];
    if (to_be_sent < 32 || to_be_sent > 126) to_be_sent = ' ';
    ReportKeyUpDownEvent(to_be_sent, /*key_up=*/false);
    delay(10);
    ReportKeyUpDownEvent(to_be_sent, /*key_up=*/true);
    delay(5);
  }
}
void ResetKeyUpDownEvent() { bleKeyboard.releaseAll(); }

// convenience masks
#define UPDOWN_MASK 0b10000000
#define MAP_MASK 0b01111111

void HandleKeyEvent(uint8_t key_byte) {
  bool key_up = ((key_byte & UPDOWN_MASK) != 0);
  uint8_t masked_key_byte = key_byte & MAP_MASK;
  uint8_t key_code = 0;

  if (fn_key_down) {
    if (fn_key_map[masked_key_byte]) key_code = fn_key_map[masked_key_byte];
  } else {
    key_code = key_map[masked_key_byte];
  }

  // keyboard duplicates the final key-up byte
  if (key_byte == last_byte) {
    ResetKeyUpDownEvent();
  } else {
    if (key_code != 0) {
      ReportKeyUpDownEvent(key_code, key_up);
    } else {
      // special case the Fn key
      if ((masked_key_byte) == 34) {
        fn_key_down = !key_up;
      }
    }
  }

  // Handles the Volume and Brightness keys in the end, they are using consumer
  // report.
  if (fn_key_down) {
    MediaKeyReport media_key_report = {0, 0};
    if (masked_key_byte == 0b01010001)
      memcpy(media_key_report, KEY_MEDIA_BRIGHTNESS_DOWN,
             sizeof(media_key_report));  // left arrow
    else if (masked_key_byte == 0b01010010)
      memcpy(media_key_report, KEY_MEDIA_VOLUME_DOWN,
             sizeof(media_key_report));  // down arrow
    else if (masked_key_byte == 0b01010011)
      memcpy(media_key_report, KEY_MEDIA_BRIGHTNESS_UP,
             sizeof(media_key_report));  // right arrow
    else if (masked_key_byte == 0b01001001)
      memcpy(media_key_report, KEY_MEDIA_VOLUME_UP,
             sizeof(media_key_report));  // up arrow
    else if (masked_key_byte == 0b00001000)
      memcpy(media_key_report, KEY_MEDIA_WWW_HOME,
             sizeof(media_key_report));  // Fn+CMD=iOS Home button
    if (media_key_report[0] != 0 || media_key_report[1] != 0) {
      if (key_up)
        bleKeyboard.release(media_key_report);
      else
        bleKeyboard.press(media_key_report);
    }
  }
  last_byte = key_byte;
#ifdef PPK_DEBUG
  Serial.printf("%s %u\n", key_up ? "released" : "pressed",
                (unsigned int)masked_key_byte);
#endif
  last_comm = millis();
  last_pressed = millis();
}

/*  Battery function
    The current consumption of the entire board measured from the battery
   connector:
   79mA 160MHz normal operation
   73.6mA 80MHz normal operation
   15.6mA Bluetooth off
   1.78mA Light sleep
   19.6uA power switch off
*/
#define BAT_CHECK_INTERVAL 60000  // 60 seconds
unsigned long last_battery_check_time = 0;

/*
float batVolt_lookup[] =
 {3, 3.35, 3.45, 3.49, 3.53, 3.57, 3.58, 3.59, 3.61, 3.63, 3.65, 3.68, 3.73,
  3.78, 3.82, 3.87, 3.91, 3.96, 4.02, 4.09, 4.17};
 */

// The ADC of ESP32-C3 is not linear to the voltage... so we don't bother
// converting the raw value to volts. We just measure the raw value across the
// entire run time, then we can calculate the raw reading at 0%, 5%, ..., 100%
// battery life. It is a rough estimation anyway.
//
// If you are using a different ESP32 board, you may need to change this table.
// This lookup table must have 21 entries. Each entry must be different,
// otherwise we will have divide-by-zero error below.
int bat_percent_lookup[] = {2000, 2340, 2390, 2420, 2450, 2475, 2490,
                            2505, 2520, 2530, 2550, 2570, 2600, 2645,
                            2675, 2705, 2740, 2770, 2805, 2845, 2905};
LedBlinkPattern battery_led_pattern = LED_NO_BLINK;

void CheckBatteryWithInterval() {
  int current_time = millis();
  if ((last_battery_check_time != 0) &&
      (current_time - last_battery_check_time < BAT_CHECK_INTERVAL))
    return;

  int raw_bat_data = analogRead(BATTERY_ADC_PIN);
#ifdef BATTERY_TEST_MODE
  char volt_str[50];
  sprintf(volt_str, "%d ", raw_bat_data);
  PrintStringToKeyboard(volt_str);
#endif
  int battery_level = -1;  // percentage of the battery
  int n;
  for (n = 0; n <= 20; n++) {
    if (raw_bat_data < bat_percent_lookup[n]) {
      if (n == 0) {
        battery_level = 0;
        break;
      } else {
        // raw_bat_data falls between bat_percent_lookup[n-1] and
        // bat_percent_lookup[n]. Do interpolation.
        battery_level = 5 * (n - 1) +
                        5 * (raw_bat_data - bat_percent_lookup[n - 1]) /
                            (bat_percent_lookup[n] - bat_percent_lookup[n - 1]);
        break;
      }
    }
  }
  if (n > 20) {
    battery_level = 100;
  }
#ifdef PPK_DEBUG
  Serial.printf("Battery percentage %d, raw_data = %d\n", battery_level,
                raw_bat_data);
#endif
  bleKeyboard.setBatteryLevel(battery_level);
  if (battery_level >= 20)
    battery_led_pattern = LED_NO_BLINK;
  else if (battery_level >= 15 && battery_level < 20)
    battery_led_pattern = LED_BLINK_ONCE;
  else if (battery_level >= 10 && battery_level < 15)
    battery_led_pattern = LED_BLINK_TWICE;
  else if (battery_level < 10)
    battery_led_pattern = LED_BLINK_THREE_TIMES;
  last_battery_check_time = current_time;
}

LedBlinkPattern func_led_pattern = LED_ALWAYS_ON;
unsigned long current_time = 0;

void CheckConnectionStatus() {
  if (bleKeyboard.isConnected())
    func_led_pattern = LED_BLINK_ONCE;
  else
    func_led_pattern = LED_BLINK_QUICK;
}

void HandleFuncLedBlink(uint8_t pin) {
  current_time = millis();

  switch (func_led_pattern) {
    case LED_NO_BLINK:
      ledOff(pin);
      break;
    case LED_ALWAYS_ON:
      ledOn(pin);
      break;
    case LED_BLINK_QUICK:
      if (current_time % 140 < 50)
        ledOn(pin);
      else
        ledOff(pin);
      break;
    case LED_BLINK_ONCE:
      if (current_time % 6000 < 50)
        ledOn(pin);
      else
        ledOff(pin);
      break;
    default:
      break;
  }
}

#define BLINK_CYCLE 3000  // 3 seconds
#define BLINK_LENGTH 50   // milliseconds
unsigned long last_cycle_start_timestamp = 0;

void HandleBatteryLedBlink(uint8_t pin) {
  current_time = millis();
  //  | On | Off | On | Off | On | Off |
  if (battery_led_pattern >= LED_BLINK_ONCE &&
      (current_time - last_cycle_start_timestamp) >
          BLINK_CYCLE) {  // Overdue for the next cycle, turn on the LED
    ledOn(pin);
    last_cycle_start_timestamp = current_time;
  } else if ((current_time - last_cycle_start_timestamp) > BLINK_LENGTH &&
             (current_time - last_cycle_start_timestamp) <= BLINK_LENGTH * 4) {
    ledOff(pin);
  } else if (battery_led_pattern >= LED_BLINK_TWICE &&
             (current_time - last_cycle_start_timestamp) > BLINK_LENGTH * 4 &&
             (current_time - last_cycle_start_timestamp) <= BLINK_LENGTH * 5) {
    ledOn(pin);
  } else if ((current_time - last_cycle_start_timestamp) > BLINK_LENGTH * 5 &&
             (current_time - last_cycle_start_timestamp) <= BLINK_LENGTH * 8) {
    ledOff(pin);
  } else if (battery_led_pattern >= LED_BLINK_THREE_TIMES &&
             (current_time - last_cycle_start_timestamp) > BLINK_LENGTH * 8 &&
             (current_time - last_cycle_start_timestamp) <= BLINK_LENGTH * 9) {
    ledOn(pin);
  } else if ((current_time - last_cycle_start_timestamp) > BLINK_LENGTH * 9) {
    ledOff(pin);
  }
}

esp_sleep_wakeup_cause_t SleepAndWaitForWakeUp() {
#ifdef PPK_DEBUG
  Serial.println("Entering light sleep.");
  /* To make sure the complete line is printed before entering sleep mode,
     need to wait until UART TX FIFO is empty:
  */
  uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
#endif
  ledOff(BATTERY_LED);
  ledOff(FUNC_LED);
  // Holds the state of VCC, GND and RTS line so the PPK is still alive.
  // Otherwise we won't be able to wake up by key press.
  gpio_hold_en(VCC_PIN_NUM);
  gpio_hold_en(GND_PIN_NUM);
#ifndef HANDSPRING
  gpio_hold_en(RTS_PIN_NUM);
#endif
  gpio_wakeup_enable(RX_PIN_NUM,
                     INVERT_TTL ? GPIO_INTR_HIGH_LEVEL : GPIO_INTR_LOW_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  // We need to wake up periodically to keep the PPK alive.
  // Otherwise the PPK goes to sleep and we won't be able to wake up by key
  // press.
  esp_sleep_enable_timer_wakeup(KEEPALIVE_TIMEOUT * uS_TO_mS_FACTOR);
  // Sleep starts, execution will continue once RX line is active, or the timer
  // is up.
  esp_light_sleep_start();
  gpio_hold_dis(VCC_PIN_NUM);
  gpio_hold_dis(GND_PIN_NUM);
#ifndef HANDSPRING
  gpio_hold_dis(RTS_PIN_NUM);
#endif
  return esp_sleep_get_wakeup_cause();
}

void loop() {
  if (Serial1.available()) {
    for (int i = Serial1.available(); i > 0; i--) {
      HandleKeyEvent(Serial1.read());
    }
  }
  // Sleep the board if the keyboard has been idling for a long time
  bool keyboard_rebooted_in_this_cycle;
  keyboard_rebooted_in_this_cycle = false;

#ifndef BATTERY_TEST_MODE
  if ((millis() - last_comm) > IDLE_TIMEOUT) {
    esp_sleep_wakeup_cause_t wakeup_reason = SleepAndWaitForWakeUp();
    switch (wakeup_reason) {
      case ESP_SLEEP_WAKEUP_TIMER:
#ifdef PPK_DEBUG
        Serial.println(
            "Wakeup caused by timer, toogle the keyboard and sleep again.");
#endif
        boot_keyboard();
        keyboard_rebooted_in_this_cycle = true;
        // We don't update last_comm here, so in the next loop, the board will
        // enter sleep again.
        break;
      case ESP_SLEEP_WAKEUP_GPIO:
#ifdef PPK_DEBUG
        Serial.println("Wakeup caused by key press, don't go to sleep again.");
#endif
        boot_keyboard();
        last_comm = millis();
        keyboard_rebooted_in_this_cycle = true;
        break;
      default:
#ifdef PPK_DEBUG
        Serial.printf(
            "It should not happen. Wakeup was not caused by timer nor "
            "keypress: %d\n",
            wakeup_reason);
#endif
        break;
    }
  }
#endif

  // reboot if no recent keypress, otherwise keyboard falls asleep
  if (!keyboard_rebooted_in_this_cycle &&
      (millis() - last_pressed) > KEEPALIVE_TIMEOUT) {
#ifdef PPK_DEBUG
    Serial.println("rebooting keyboard for timeout");
#endif
    last_pressed = millis();
    boot_keyboard();
  }
  CheckBatteryWithInterval();
  HandleBatteryLedBlink(BATTERY_LED);
  CheckConnectionStatus();
  HandleFuncLedBlink(FUNC_LED);
  // Delay a bit after a loop to save power. We don't need too frequent key
  // detection. The hardware reference of PPK says "Filters Key Bounce: 15 - 25
  // milliseconds".
  delay(25);
}
