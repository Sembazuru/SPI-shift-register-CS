/*
 * SPI_Register_Arduino_Sample - Sample sketch for the "SPI Chip Select Shift Register" board using 4
 *  each of Adafruit SSD1306 based 128x32 OLED display and Adafruit MAX31855 based thermocouple amplifier.
 *  Demonstrating one technique of "random" (well, non-sequential) SPI access by shifting full bytes out
 *  to the shift register.
 *
 * By Chris "Sembazuru" Elliott, SembazuruCDE (at) GMail.com
 * 2015-05-10
 *
 * Sample program to test SPI register using 4 128x32 SPI OLED monochrome display using Adafruit's SSD1306 library,
 * and 4 thermocouple amplifiers using Adafruit's MAX31855 library.
 * Once a second, query all 4 thermocouples for temperature in Celcius and display the reading on 4 displays. (One display
 * for each thermocouple.)
 *
 * Arduino connections
 *  A0 =   XX (Not used)
 *  A1 =   XX (Not used)
 *  A2 =   XX (Not used)
 *  A3 =   XX (Not used)
 *  A4 =   XX (Not used)
 *  A5 =   XX (Not used)
 *  D0 =   XX (Not used)
 *  D1 =   XX (Not used)
 *  D2 =   XX (Not used)
 *  D3 =   XX (Not used)
 *  D4 =   XX (Not used)
 *  D5 =   XX (Not used)
 *  D6 =   XX (Not used)
 *  D7 = OLED_DC (all OLEDs)
 *  D8 = OLED_Reset (all OLEDs)
 *  D9 = rCS (shift register board input header pin 3)
 *  D10 = dCS (shift register board input header pin 4)
 *  D11 = MOSI (shift register board input header pin 5)
 *  D12 = MISO (shift register board input header pin 6)
 *  D13 = SCK (shift register board input header pin 7)
 *  5V = VCC (shift register board input header pin 1)
 *  GND = GND (shift register board input header pin 2)
 *
 */

// Add any requred #include lines here for external libraries.
#include <SPI.h>
const byte register_CS = 9;
const byte device_CS = 10;

// OLED display using Adafruit's library
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
const byte OLED_DC = 7;
const byte OLED_Reset = 8;
const byte OLED_CS = device_CS;
const byte displayDevices[] //
{
  0b01111111, // MSBFIRST = J8
  0b10111111, // MSBFIRST = J7
  0b11011111, // MSBFIRST = J6
  0b11101111  // MSBFIRST = J5
};
const byte qtyDisp = sizeof(displayDevices) / sizeof(displayDevices[0]);
Adafruit_SSD1306 display(OLED_DC, OLED_Reset, OLED_CS); // Use the same object (and thus same buffer) to write to all displays.
// Check to make sure we remembered to reconfigure the library for a display height of 32 pixels.
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// MAX31885 t/c amplifier using Adafruit's library
#include <Adafruit_MAX31855.h>
const byte tc_CS = device_CS; // Same CS pin as far as the Arduino is concerned.
const byte tcDevices[]
{
  0b11110111, // MSBFIRST = J4
  0b11111011, // MSBFIRST = J3
  0b11111101, // MSBFIRST = J2
  0b11111110  // MSBFIRST = J1
};
const byte qtyTc = sizeof(tcDevices) / sizeof(tcDevices[0]);
Adafruit_MAX31855 thermocouple(tc_CS);

// Timing constants and variables
unsigned long currentMillis = 0;    // stores the value of millis() in each iteration of loop()
unsigned long previousScan = 0;   // will store last time the LED was updated
const unsigned int scanInterval = 1000; // How many milliseconds between thermocouple scan and display updates

void setup()
{
  // Put your setup code here, to run once:
  pinMode(register_CS, OUTPUT);
  pinMode(device_CS, OUTPUT);

  // Display initialize
  setRegisterCS(initDisplayCS()); // Select all the displays to initializing all of them with the shared reset pin.
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  for (byte i = 0; i < qtyDisp; i++)
  {
    setRegisterCS(displayDevices[i]); // Select the next OLED display
    display.display();
  }
  display.clearDisplay(); // Flush the display buffer that is shared for all the displays.
  delay(1000);

  // HW Serial port setup.
  Serial.begin(115200); // Change this to whatever your like running your Serial Monitor at.
  while (!Serial); // Wait for serial port to connect. Needed for Leonardo only.
  delay(1000); // Simply to allow time for the ERW versions of the IDE time to automagically open the Serial Monitor. 1 second chosen arbitrarily.
  Serial.println(F("Starting SPI_Register_Arduino_Sample.ino"));  // so we know what sketch is running
  Serial.println();
}

void loop()
{
  // Put your main code here, to run repeatedly:
  currentMillis = millis();   // capture the latest value of millis()

  if (currentMillis - previousScan >= scanInterval)
  {
    for (byte i = 0; i < qtyTc; i++)
    {
      previousScan = currentMillis;
      setRegisterCS(tcDevices[i]); // Select the next thermocouple amplifier
      double TemperatureC = thermocouple.readCelsius(); // Library returns a double. On ATMEGA based Arduinos this is the same as float, but other boards (like Due) this might be double the width of a float.

      // Show on the Serial Monitor
      Serial.print(F("Device "));
      Serial.print(i);
      Serial.print(' ');
      Serial.print(TemperatureC, 2);
      Serial.println('C');

      // Show on a display.
      setRegisterCS(displayDevices[i]); // Select the next OLED display
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print(TemperatureC, 2);
      display.print('C');
      display.display(); // Send the buffer to the currently selected display.

      display.clearDisplay(); // Flush the display buffer
    }
    Serial.println();
  }
}

void setRegisterCS(byte rCSbitfield)
{
  digitalWrite(register_CS, LOW); // Assert rCS
  SPI.transfer(rCSbitfield); // Shift the intended device CS into the register
  digitalWrite(register_CS, HIGH); // Release rCS
}

byte initDisplayCS()
{
  byte initDispCS = 0b11111111;
  for (byte i = 0; i < qtyDisp; i++)
  {
    initDispCS = initDispCS & displayDevices[i];
  }
  return initDispCS;
}
