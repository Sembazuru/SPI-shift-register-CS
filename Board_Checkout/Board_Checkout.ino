/*
 * Board_Checkout - Board checkout shifting numbers out to light LEDs to know which bits enable which headers.
 * Where I use the term "Chip Select" you may use the term "Slave Select" if that is what you are used to.
 *
 * By Chris "Sembazuru" Elliott, SembazuruCDE (at) GMail.com
 * 2015-05-09
 *
 * Connect a 5V LED (or equivalent circuit) from CS (pin6 to cathode) to Vcc (pin 1 to anode) in each shift
 *  register board output header.
 *
 * Arduino connections (Tested on UNOr3)
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
 *  D7 =   XX (Not used)
 *  D8 =   XX (Not used)
 *  D9 = dCS (shift register board input header pin 3)
 *  D10 = rCS (shift register board input header pin 4)
 *  D11 = MOSI (shift register board input header pin 5)
 *  D12 = MISO (shift register board input header pin 6)
 *  D13 = SCK (shift register board input header pin 7)
 *  5V = VCC (shift register board input header pin 1)
 *  GND = GND (shift register board input header pin 2)
 *
 * Optionally, daisy chain several shift register boards in series by connecting the next board's input to the
 *  previous board's output (chain). This way you can see how the one byte is propigated down the daisy chain.
 *  Or you can play with the code before to practice writing multiple bytes to select any one of the outputs
 *  on your daisy chain.
 */

#include <SPI.h>

// The Chip Select signal on the SPI bus is a low active signal.
const byte dCS = 10; // dCD stands for "Device Chip Select", meaning enable the output of the shift register to assert the address stored in it.
const byte rCS = 9; // rCD stands for "Register Chip Select", meaning the CS pin to load a new address into the shift register.
const byte addresses[] // An array of addresses to select to make non-sequential device activation order easier.
{
  0b11111110, // This position of the 0 is for MSBFIRST = J1, and for LSBFIRST = J8
  0b11111101, // This position of the 0 is for MSBFIRST = J2, and for LSBFIRST = J7
  0b11111011, // This position of the 0 is for MSBFIRST = J3, and for LSBFIRST = J6
  0b11110111, // This position of the 0 is for MSBFIRST = J4, and for LSBFIRST = J5
  0b11101111, // This position of the 0 is for MSBFIRST = J5, and for LSBFIRST = J4
  0b11011111, // This position of the 0 is for MSBFIRST = J6, and for LSBFIRST = J3
  0b10111111, // This position of the 0 is for MSBFIRST = J7, and for LSBFIRST = J2
  0b01111111  // This position of the 0 is for MSBFIRST = J8, and for LSBFIRST = J1
};

void setup()
{
  Serial.begin(115200); // Change this to whatever your like running your Serial Monitor at.
  while (!Serial); // Wait for serial port to connect. Needed for Leonardo only.
  delay(1000); // 1 second chosen arbitrarily to allow time for the ERW versions of the IDE time to automagically open the Serial Monitor.
  Serial.println(F("Board_Checkout.ino"));  // so we know what sketch is running
  Serial.println();

  // set the Chip Select pins as outputs.
  pinMode (dCS, OUTPUT);
  pinMode (rCS, OUTPUT);
  // initialize SPI.
  SPI.begin();
  // Only have one of the two following lines un-commented at a time. See the comments in the addresses[] declaration above.
  SPI.setBitOrder(MSBFIRST); // Default on UNO.
  //  SPI.setBitOrder(LSBFIRST);
}

void loop()
{
  for (byte i = 0; i < (sizeof(addresses) / sizeof(addresses[0])); i++)
  {
    // Writing to the shift register as if it is a SPI device.
    digitalWrite(rCS, LOW); // Assert the shift register's Chip Select pin.
    SPI.transfer(addresses[i]); // Write the "address" of the currently desired end device into the shift register.
    digitalWrite(rCS, HIGH); // Release the shift register's Chip Select pin.

    Serial.println(addresses[i], BIN); // Show on the Serial Monitor the value that was sent out.

    // Now write to the desired end device on the shift register.
    digitalWrite(dCS, LOW); // Turn on shift buffer output to turn on LED (as if asserting chip select pin for a SPI write to the selected device).
    delay(1000); // Here we are just delaying for a second instead of using SPI.transfer();.
    digitalWrite(dCS, HIGH); // Turn off shift buffer output to turn off LED.
  }
}


