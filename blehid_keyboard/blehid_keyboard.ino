/*********************************************************************
  This is an example for our nRF52 based Bluefruit LE modules

  Pick one up today in the adafruit shop!

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  MIT license, check LICENSE for more information
  All text above, and the splash screen below must be included in
  any redistribution
*********************************************************************/
#include <bluefruit.h>
#include "Adafruit_TinyUSB.h"

// set to 3 for III hardware, or 5 for V hardware
#define PPK_VERSION 3

// Define to enable debug mode, which notes to the arduino console at 115200
// Comment out to disable debug mode.
// #define PPK_DEBUG

#if PPK_VERSION == 3
#define RTS_PIN       PIN_A1
#define DCD_PIN       PIN_A3
#define GND_PIN       PIN_A5
#endif

#if PPK_VERSION == 5
#define VCC_PIN       7
#define RX_PIN        8
#define RTS_PIN       5
#define DCD_PIN       4
#define GND_PIN       2
#endif

#define PAIR_BUTTON PIN_BUTTON1
#define BTN_ACTIVE LOW
#define BATTERY_ADC_PIN PIN_A6

#if (PPK_VERSION != 3) && (PPK_VERSION != 5)
#error
#error
#error    you did not set your ppk version!
#error    read the instructions or read the code!
#error
#error
#endif

BLEDis bledis;
BLEHidAdafruit blehid;
BLEBas blebas;    // BAS (Battery Service) helper class instance

// convenience masks
#define UPDOWN_MASK 0b10000000
#define MAP_MASK    0b01111111

// wait this many milliseconds before making sure keyboard is still awake
#define TIMEOUT 500000

// macro for testing if a char is printable ASCII
#define PRINTABLE_CHAR(x) ((x >= 32) && (x <= 126))

typedef enum LedBlinkPattern {
  LED_NO_BLINK,
  LED_BLINK_ONCE,
  LED_BLINK_TWICE,
  LED_BLINK_THREE_TIMES,
} LedBlinkPattern;

//SoftwareSerial keyboard_serial(RX_PIN, TX_PIN, true); // RX, TX, inverted

char key_map[128] = { 0 };
char fn_key_map[128] = { 0 };

char last_byte = 0;
char key_byte = 0;

uint8_t modifier = 0;
#define MAX_KEY_BUFFER 6
uint8_t key_report_buffer[MAX_KEY_BUFFER] = { HID_KEY_NONE };
int key_report_size = 0;
bool fn_key_down = false;

unsigned long last_comm = 0;

void config_keymap()
{
  // y0 row
  key_map[0b00000000] = HID_KEY_1;
  key_map[0b00000001] = HID_KEY_2;
  key_map[0b00000010] = HID_KEY_3;
  key_map[0b00000011] = HID_KEY_Z;
  key_map[0b00000100] = HID_KEY_4;
  key_map[0b00000101] = HID_KEY_5;
  key_map[0b00000110] = HID_KEY_6;
  key_map[0b00000111] = HID_KEY_7;

  // y1 row
  key_map[0b00001000] = HID_KEY_GUI_LEFT; // "CMMD" or "Cmd"
  key_map[0b00001001] = HID_KEY_Q;
  key_map[0b00001010] = HID_KEY_W;
  key_map[0b00001011] = HID_KEY_E;
  key_map[0b00001100] = HID_KEY_R;
  key_map[0b00001101] = HID_KEY_T;
  key_map[0b00001110] = HID_KEY_Y;
  key_map[0b00001111] = HID_KEY_GRAVE;

  // y2 row
  key_map[0b00010000] = HID_KEY_X;
  key_map[0b00010001] = HID_KEY_A;
  key_map[0b00010010] = HID_KEY_S;
  key_map[0b00010011] = HID_KEY_D;
  key_map[0b00010100] = HID_KEY_F;
  key_map[0b00010101] = HID_KEY_G;
  key_map[0b00010110] = HID_KEY_H;
  key_map[0b00010111] = HID_KEY_SPACE; // "Space 1"

  // y3 row
  key_map[0b00011000] = HID_KEY_CAPS_LOCK;
  key_map[0b00011001] = HID_KEY_TAB;
  key_map[0b00011010] = HID_KEY_CONTROL_LEFT;
  key_map[0b00011011] = 0;
  key_map[0b00011100] = 0;
  key_map[0b00011101] = 0;
  key_map[0b00011110] = 0;
  key_map[0b00011111] = 0;

  // y4 row
  key_map[0b00100000] = 0;
  key_map[0b00100001] = 0;
  key_map[0b00100010] = 0; // Fn key
  key_map[0b00100011] = HID_KEY_ALT_LEFT;
  key_map[0b00100100] = 0;
  key_map[0b00100101] = 0;
  key_map[0b00100110] = 0;
  key_map[0b00100111] = 0;

  // y5 row
  key_map[0b00101000] = 0;
  key_map[0b00101001] = 0;
  key_map[0b00101010] = 0;
  key_map[0b00101011] = 0;
  key_map[0b00101100] = HID_KEY_C;
  key_map[0b00101101] = HID_KEY_V;
  key_map[0b00101110] = HID_KEY_B;
  key_map[0b00101111] = HID_KEY_N;

  // y6 row
  key_map[0b00110000] = HID_KEY_MINUS;
  key_map[0b00110001] = HID_KEY_EQUAL;
  key_map[0b00110010] = HID_KEY_BACKSPACE;
  key_map[0b00110011] = HID_KEY_HOME; // "Special Function One"
  key_map[0b00110100] = HID_KEY_8;
  key_map[0b00110101] = HID_KEY_9;
  key_map[0b00110110] = HID_KEY_0;
  key_map[0b00110111] = HID_KEY_SPACE; // "Space 2" as Space

  // y7 row
  key_map[0b00111000] = HID_KEY_BRACKET_LEFT;
  key_map[0b00111001] = HID_KEY_BRACKET_RIGHT;
  key_map[0b00111010] = HID_KEY_BACKSLASH;
  key_map[0b00111011] = HID_KEY_END; // "Special Function Two"
  key_map[0b00111100] = HID_KEY_U;
  key_map[0b00111101] = HID_KEY_I;
  key_map[0b00111110] = HID_KEY_O;
  key_map[0b00111111] = HID_KEY_P;

  // y8 row
  key_map[0b01000000] = HID_KEY_APOSTROPHE;
  key_map[0b01000001] = 0x28;  //Return
  key_map[0b01000010] = HID_KEY_PAGE_UP; // "Special Function Three"
  key_map[0b01000011] = 0;
  key_map[0b01000100] = HID_KEY_J;
  key_map[0b01000101] = HID_KEY_K;
  key_map[0b01000110] = HID_KEY_L;
  key_map[0b01000111] = HID_KEY_SEMICOLON;

  // y9 row
  key_map[0b01001000] = HID_KEY_SLASH;
  key_map[0b01001001] = HID_KEY_ARROW_UP;
  key_map[0b01001010] = HID_KEY_PAGE_DOWN; // "Special Function Four"
  key_map[0b01001011] = 0;
  key_map[0b01001100] = HID_KEY_M;
  key_map[0b01001101] = HID_KEY_COMMA;
  key_map[0b01001110] = HID_KEY_PERIOD;
  key_map[0b01001111] = HID_KEY_INSERT; // "DONE" as Insert

  // y10 row
  key_map[0b01010000] = HID_KEY_DELETE;
  key_map[0b01010001] = HID_KEY_ARROW_LEFT;
  key_map[0b01010010] = HID_KEY_ARROW_DOWN;
  key_map[0b01010011] = HID_KEY_ARROW_RIGHT;
  key_map[0b01010100] = 0;
  key_map[0b01010101] = 0;
  key_map[0b01010110] = 0;
  key_map[0b01010111] = 0;

  // y11 row
  key_map[0b01011000] = HID_KEY_SHIFT_LEFT;
  key_map[0b01011001] = HID_KEY_SHIFT_RIGHT;
  key_map[0b01011010] = 0;
  key_map[0b01011011] = 0;
  key_map[0b01011100] = 0;
  key_map[0b01011101] = 0;
  key_map[0b01011110] = 0;
  key_map[0b01011111] = 0;
}

void config_fnkeymap()
{
  fn_key_map[0b00011001] = HID_KEY_ESCAPE; // tab key
  fn_key_map[0b00000000] = HID_KEY_F1; // 1
  fn_key_map[0b00000001] = HID_KEY_F2; // 2
  fn_key_map[0b00000010] = HID_KEY_F3; // 3
  fn_key_map[0b00000100] = HID_KEY_F4; // 4
  fn_key_map[0b00000101] = HID_KEY_F5; // 5
  fn_key_map[0b00000110] = HID_KEY_F6; // 6
  fn_key_map[0b00000111] = HID_KEY_F7; // 7
  fn_key_map[0b00110100] = HID_KEY_F8; // 8
  fn_key_map[0b00110101] = HID_KEY_F9; // 9
  fn_key_map[0b00110110] = HID_KEY_F10; // 0
  fn_key_map[0b00110000] = HID_KEY_F11; // -
  fn_key_map[0b00110001] = HID_KEY_F12; // =
}

void print_byte_bin(char bin_byte)
{
  Serial.print("0b");

  for (int i = 7; i > -1; i--) Serial.print(int((bin_byte & (1 << i)) >> i));
}

void print_keychange(char key_byte, char key_code, int key_up)
{
  if (key_up) Serial.print("released: "); else Serial.print("pressed:  ");
  print_byte_bin(key_byte);

  Serial.print(" mapped to ");

  if (key_code)
  {
    print_byte_bin(key_code);

    if (PRINTABLE_CHAR(key_code))
    {
      Serial.print(" (");
      Serial.print(key_code);
      Serial.print(")");
    }
    else
    {
      Serial.print(" (unprintable)");
    }
  }
  // Fn has no keycode, special case it
  else if (key_byte == 34)
  {
    Serial.print("Fn");
  }
  else
  {
    Serial.print("nothing");
  }

  Serial.println("");
}

void boot_keyboard()
{
#ifdef PPK_DEBUG
  Serial.println("beginning keyboard boot sequence");
#endif

  pinMode(GND_PIN, OUTPUT);
  pinMode(DCD_PIN, INPUT);
  pinMode(RTS_PIN, INPUT);

  digitalWrite(GND_PIN, LOW);

#ifdef PPK_DEBUG
  Serial.print("waiting for keyboard response...");
#endif

  while (digitalRead(DCD_PIN) != HIGH) {
    delay(1);
  };

#ifdef PPK_DEBUG
  Serial.println(" done");
#endif

  Serial1.begin(9600);

#ifdef PPK_DEBUG
  Serial.print("finishing handshake...");
#endif

  if (digitalRead(RTS_PIN) == LOW)
  {
    delay(10);
    pinMode(RTS_PIN, OUTPUT);
    digitalWrite(RTS_PIN, HIGH);
  }
  else
  {
    pinMode(RTS_PIN, OUTPUT);
    digitalWrite(RTS_PIN, HIGH);
    digitalWrite(RTS_PIN, LOW);
    delay(10);
    digitalWrite(RTS_PIN, HIGH);
  }

#ifdef PPK_DEBUG
  Serial.print("waiting for keyboard serial ID...");
#endif

  delay(10);
  // Read the Hardware Serial1 until we get the expected hex ID. Due to the timing of plugging in the adapter, there could be garbage chars before the ID, the following code will ignore them.
  int byte1,  byte2 = 0;
  while (true) {
    if (Serial1.available()) {
      byte1 = byte2;
      byte2 = Serial1.read();
#ifdef PPK_DEBUG
      Serial.println(byte2);
#endif
    }
    if ((byte1 == 0xFA) && (byte2 == 0xFD)) break;
  }

#ifdef PPK_DEBUG
  Serial.println(" done");
#endif

  last_comm = millis();
}

#define INVALID_CONN_HANDLE 65535
#define MAX_CONCURRENT_CONN 10
uint16_t conn_handles[MAX_CONCURRENT_CONN] = {INVALID_CONN_HANDLE};

void prph_connect_callback(uint16_t conn_handle)
{
  // Pushes the conn_handle into the list
  for (int i = 0; i < MAX_CONCURRENT_CONN; ++i) {
    if (conn_handles[i] == conn_handle) {
      break;
    } else if (conn_handles[i] == INVALID_CONN_HANDLE) {
      conn_handles[i] = conn_handle;
      break;
    }
  }
#ifdef PPK_DEBUG
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char peer_name[64] = { 0 };
  connection->getPeerName(peer_name, sizeof(peer_name));

  Serial.print("[Prph] Connected to ");
  Serial.print(peer_name);
  Serial.print(", conn_handle=");
  Serial.println(conn_handle);
#endif
}

void prph_disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  // Removes the conn_handle from the list
  for (int i = 0; i < MAX_CONCURRENT_CONN; ++i) {
    if (conn_handles[i] == conn_handle) {
      conn_handles[i] = INVALID_CONN_HANDLE;
    }
  }
#ifdef PPK_DEBUG
  Serial.println();
  Serial.print("[Prph] Disconnected, conn_handle=");
  Serial.println(conn_handle);
#endif
}

void DisconnectAll() {
  for (int i = 0; i < MAX_CONCURRENT_CONN; ++i) {
    if (conn_handles[i] != INVALID_CONN_HANDLE) {
      if (Bluefruit.connected(conn_handles[i]))Bluefruit.disconnect(conn_handles[i]) ;
    }
  }
}

void setup()
{
  pinMode(PAIR_BUTTON, INPUT_PULLUP);

#ifdef PPK_DEBUG
  Serial.begin(115200);
  // delay for a bit to allow for opening serial monitor etc.
  for (int i = 0; i < 3; delay(1000 + i++)) Serial.print(".");
  Serial.println("Bluefruit52 HID Keyboard Example");
  Serial.println("--------------------------------\n");

  Serial.println();
  Serial.println("Go to your phone's Bluetooth settings to pair your device");
  Serial.println("then open an application that accepts keyboard input");

  Serial.println();
  Serial.println("Enter the character(s) to send:");
  Serial.println();
#endif

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Palm Portable Keyboard");

  Bluefruit.Periph.setConnectCallback(prph_connect_callback);
  Bluefruit.Periph.setDisconnectCallback(prph_disconnect_callback);

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Palm Portable Keyboard");
  bledis.begin();

  blebas.begin();
  blebas.write(100);

  blehid.begin();

  // Set callback for set LED from central
  blehid.setKeyboardLedCallback(set_keyboard_led);

  // Set up and start advertising
  startAdv();

#ifdef PPK_DEBUG
  Serial.print("compiled in debug mode with PPK_VERSION ");
  Serial.println(PPK_VERSION);
#endif

  config_keymap();
  config_fnkeymap();
  boot_keyboard();

#ifdef PPK_DEBUG
  Serial.println("setup completed");
#endif
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);

  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for the dev name in the advertising packet
  Bluefruit.Advertising.addName();


  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void ReportKeyUpDownEvent(uint8_t key_code, bool key_up) {
  if (key_up)
  {
    // Removes the keycode from key_report_buffer if found.
    for (int i = 0; i < key_report_size; ++i) {
      if (key_report_buffer[i] == key_code) {
        // Shifts the remaining reports left to fill in the blank
        int j = i + 1;
        while (j < key_report_size) {
          key_report_buffer[j - 1] = key_report_buffer[j];
          ++j;
        }
        key_report_buffer[j - 1] = HID_KEY_NONE;
        --key_report_size;
      }
    }
  }
  else
  {
    // Adds the keycode to the end of key_report_buffer.
    if (key_report_size < MAX_KEY_BUFFER) {
      key_report_buffer[key_report_size] = key_code;
      ++key_report_size;
    }
  }
  if (key_report_size > 0) blehid.keyboardReport(modifier, key_report_buffer);
  else ResetKeyUpDownEvent();
  //if(TinyUSBDevice.mounted()){}
}

void ResetKeyUpDownEvent() {
  blehid.keyRelease();
  // reset report
  key_report_size = 0;
  memset(key_report_buffer, HID_KEY_NONE, MAX_KEY_BUFFER);
}


unsigned long last_pair_pushed = 0;
bool pair_triggered = false;

void HandlePairButton() {
  if ( BTN_ACTIVE == digitalRead(PAIR_BUTTON) ) {
    if (last_pair_pushed == 0) last_pair_pushed = millis();
    if (millis() - last_pair_pushed > 1000 && !pair_triggered) {
#ifdef PPK_DEBUG
      Serial.println("Clearing bonds and restart pairing...");
#endif
      pair_triggered = true;

      DisconnectAll();
      Bluefruit.Advertising.stop();
      Bluefruit.Advertising.clearData();
      Bluefruit.ScanResponse.clearData(); // add this

#ifdef PPK_DEBUG
      Serial.println("----- Before -----\n");
      bond_print_list(BLE_GAP_ROLE_PERIPH);
      bond_print_list(BLE_GAP_ROLE_CENTRAL);
#endif

      Bluefruit.Periph.clearBonds();
      Bluefruit.Central.clearBonds();

#ifdef PPK_DEBUG
      Serial.println("----- After  -----\n");
      bond_print_list(BLE_GAP_ROLE_PERIPH);
      bond_print_list(BLE_GAP_ROLE_CENTRAL);
#endif
      startAdv();
    }
  } else {
    pair_triggered = false;
    last_pair_pushed = 0;
  }
}

void HandleKeyEvent(char key_byte) {

  bool key_up = ((key_byte & UPDOWN_MASK) != 0);
  char masked_key_byte = key_byte & MAP_MASK;
  char key_code = 0;

  if (fn_key_down) {
    if (fn_key_map[masked_key_byte]) key_code = fn_key_map[masked_key_byte];
  } else {
    key_code = key_map[masked_key_byte];
  }

  // keyboard duplicates the final key-up byte
  if (key_byte == last_byte) {
    ResetKeyUpDownEvent();
  }
  else
  {
#ifdef PPK_DEBUG
    print_keychange(masked_key_byte, key_code, key_up);
#endif
    if (key_code != 0)
    {
      ReportKeyUpDownEvent(key_code, key_up);
    }
    else
    {
      // special case the Fn key
      if ((masked_key_byte) == 34)
      {
        fn_key_down = !key_up;
      }
    }
  }

  // Handles the Volume and Brightness keys in the end, they are using consumer report.
  if (fn_key_down) {
    uint16_t usage_code = 0;
    if (masked_key_byte == 0b01010001)usage_code = HID_USAGE_CONSUMER_BRIGHTNESS_DECREMENT; // left arrow
    else if (masked_key_byte == 0b01010010)usage_code = HID_USAGE_CONSUMER_VOLUME_DECREMENT; // down arrow
    else if (masked_key_byte == 0b01010011)usage_code = HID_USAGE_CONSUMER_BRIGHTNESS_INCREMENT; // right arrow
    else if (masked_key_byte == 0b01001001)usage_code = HID_USAGE_CONSUMER_VOLUME_INCREMENT; // up arrow
    if (usage_code) {
      if (key_up) blehid.consumerKeyRelease();
      else blehid.consumerKeyPress(usage_code);
    }
  }
  last_byte = key_byte;
  last_comm = millis();
}


#define BAT_CHECK_INTERVAL 300000 //5 minutes
unsigned long last_battery_check_time = 0;
float volt_per_lsb = 2 * 3.6F / 1024.0F; // 10-bit ADC with 3.6V input range, with 50% divider
//batVolt has voltage levels every 5% of charge level. Adjust as necessary. Slightly better than a straight linear interp of voltage
float batVolt_lookup[] = {3.27, 3.61, 3.69, 3.71, 3.73, 3.75, 3.77, 3.79, 3.80, 3.82, 3.84, 3.85, 3.87, 3.91, 3.95, 3.98, 4.02, 4.08, 4.11, 4.15, 4.20};
LedBlinkPattern led_pattern = LED_NO_BLINK;

void CheckBatteryWithInterval() {
  int current_time = millis();
  if ((current_time - last_battery_check_time)  < BAT_CHECK_INTERVAL) return;

  float battery_volt = ((float)analogRead(BATTERY_ADC_PIN)) * volt_per_lsb;
  int battery_level = -1;  // percentage of the battery
  for (int n = 0; n <= 21; n++)
  {
    if (battery_volt < batVolt_lookup[n])
    {
      battery_level = 5 * n;
      break;
    }
  }
  if (battery_level < 0)
  {
    //You shouldn't be here, batVolt was not found in range of lookup values.
    battery_level = 100;
  }
#ifdef PPK_DEBUG
  Serial.print("Battery level ");
  Serial.print(battery_level);
  Serial.print("%, voltage = ");
  Serial.print(battery_volt);
  Serial.println(" V");
#endif
  blebas.write(battery_level);
  if (battery_level >= 60)led_pattern = LED_NO_BLINK;
  else if (battery_level >= 40 && battery_level < 60)led_pattern = LED_BLINK_ONCE;
  else if (battery_level >= 20 && battery_level < 40)led_pattern = LED_BLINK_TWICE;
  else if (battery_level < 20)led_pattern = LED_BLINK_THREE_TIMES;
  last_battery_check_time = current_time;
}

#define BLINK_CYCLE 3000 //3 seconds
#define BLINK_LENGTH 50 //milliseconds
unsigned long last_cycle_start_timestamp = 0;
bool led_is_on = false;

void HandleLedBlink() {
  int current_time = millis();
  //  | On | Off | On | Off |
  if (led_pattern >= LED_BLINK_ONCE && (current_time - last_cycle_start_timestamp) > BLINK_CYCLE) { // Overdue for the next cycle, turn on the LED
    if (!led_is_on) {
      ledOn( LED_RED );
      led_is_on = true;
    }
    last_cycle_start_timestamp = current_time;
  } else if ((current_time - last_cycle_start_timestamp) > BLINK_LENGTH && (current_time - last_cycle_start_timestamp) <= BLINK_LENGTH * 4) {
    if (led_is_on) {
      ledOff( LED_RED );
      led_is_on = false;
    }
  } else if (led_pattern >= LED_BLINK_TWICE && (current_time - last_cycle_start_timestamp) > BLINK_LENGTH * 4 && (current_time - last_cycle_start_timestamp) <= BLINK_LENGTH * 5) {
    if (!led_is_on) {
      ledOn( LED_RED );
      led_is_on = true;
    }
  } else if ((current_time - last_cycle_start_timestamp) > BLINK_LENGTH * 5 && (current_time - last_cycle_start_timestamp) <= BLINK_LENGTH * 8) {
    if (led_is_on) {
      ledOff( LED_RED );
      led_is_on = false;
    }
  } else if (led_pattern >= LED_BLINK_THREE_TIMES && (current_time - last_cycle_start_timestamp) > BLINK_LENGTH * 8 && (current_time - last_cycle_start_timestamp) <= BLINK_LENGTH * 9) {
    if (!led_is_on) {
      ledOn( LED_RED );
      led_is_on = true;
    }
  } else if ((current_time - last_cycle_start_timestamp) > BLINK_LENGTH * 9) {
    if (led_is_on) {
      ledOff( LED_RED );
      led_is_on = false;
    }
  }
}

void loop()
{
  HandlePairButton();
  if (Serial1.available()) {
    for (int i = Serial1.available(); i > 0; i--) {
      HandleKeyEvent(Serial1.read());
    }
  }
  else
  {
    // reboot if no recent comms, otherwise keyboard falls asleep
    if ((millis() - last_comm) > TIMEOUT) {
#ifdef PPK_DEBUG
      Serial.println("rebooting keyboard for timeout");
#endif
      digitalWrite(GND_PIN, HIGH);
      boot_keyboard();
    }
  }
  CheckBatteryWithInterval();
  HandleLedBlink();
  // Delay a bit after a loop to save power. We don't need too frequent key detection.
  delay(5);
}

/**
   Callback invoked when received Set LED from central.
   Must be set previously with setKeyboardLedCallback()

   The LED bit map is as follows: (also defined by KEYBOARD_LED_* )
      Kana (4) | Compose (3) | ScrollLock (2) | CapsLock (1) | Numlock (0)
*/
void set_keyboard_led(uint16_t conn_handle, uint8_t led_bitmap)
{
  (void) conn_handle;

#ifdef PPK_DEBUG
  Serial.print("led_bitmap = ");
  Serial.println(led_bitmap);
#endif
  // light up Red Led if any bits is set
  if ( led_bitmap )
  {
    ledOn( LED_RED );
  }
  else
  {
    ledOff( LED_RED );
  }
}
