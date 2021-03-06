# SPI-shift-register-CS
The concept is to have a way of selecting many SPI devices without eating up an I/O pin for each device's chip select.

Use a shift register chip to "point to" one of many SPI devices, and then use the SPI ~CS signal on the shift register's output enable pin for the SPI data communication.

One technique might be to shift a 1 the number of times equal to the position number of the desired device, then shift the one's complement out. Then procede with the normal SPI transfer as normal using the shift register network's /OE pin as the SPI transfer ~CS pin.

For revision 1 try using MOSI and SCK to shift to the register. So only need 1 extra pin for the latch. Because the latch of 74HCT595 is rising edge triggered, add circuitry to change the MOSI line from pass-through with latch high to serial cascade with latch low. Essentially will act like a standard SPI ~CS pin for using SPI to shift the "address" of the target SPI device into the shift register.


Possible parts:
<ul>
<li>74HC595 - Shift Register Single 8-Bit Serial to Serial/Parallel
<ul><li>https://octopart.com/search?q=74HC595&filter[fields][avg_avail][]=[1%20TO%20*]&view=matrix&filter[fields][specs.mounting_style.value][]=Through%20Hole
<li>Or any equivalent. Must provide tri-state outputs. 74HC family for compatibility with both 3.3V and 5V SPI networks.
</ul><li>74HC257 - Quadruple 2-Line To 1-Line Data Selectors/Multiplexers
<ul><li>https://octopart.com/search?q=74HC257&filter[fields][avg_avail][]=[1%20TO%20*]&view=matrix&filter[fields][specs.mounting_style.value][]=Through%20Hole
<li>Or any equivalent. Tri-state outputs not necessary. 74HC family for compatibility with both 3.3V and 5V SPI networks.
</ul></ul>
