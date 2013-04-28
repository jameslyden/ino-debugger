Arduino Debugger Messaging Protocol
===================================
The Arduino Debugger Messaging Protocol (ADMP) is used to send the values of the
Arduino's digital and analog inputs over the serial port for further processing.


Version 1: Periodic updating
----------------------------

Version 1 of ADMP sends every input's current value on a periodic basis. There
are no messages sent from the receiver to the Arduino. This allows for:
* simple decoding, and
* easily inserting debugging into a program that is unaware of it.

However, it also has some weaknesses, which include:
* requiring the inputs of interest to be configured at compile time,
* requiring the inputs of interest to be sequential (they are only specified by start and end values), and
* input numbers are relative to lowest pin used, rather than actual pin numbers.

### Packet structure

The structure of a V1 packet is defined in the following table:
| Section			| Bytes	| Notes															|
|-----------------|--------|--------------------------------------------------|
| Header				| 3		| Unique sequence (0xFF, 0xFE, 0xFD) that identifies the start of an ADMPv1 packet. Helps to prevent collisions between debug data and other program data, and to sync the receiver (these three values will never occur in the rest of the packet). |
| Size				| 1		| Total size of this packet in bytes, all-inclusive. The total number of inputs contained in the packet is equal to this size value minus 5. |
| Analog Inputs	| X		| One byte per analog input transmitted. The value of X is equal to (ending analog pin - starting analog pin + 1). The minimum is 0 (if the starting analog pin is greater than the ending one), and the maximum is equal to the number of analog inputs available on the Arduino. |
| Digital Inputs	| Y		| One byte per digital input transmitted. The value and limits of Y are equivalent to those for X, but considering digital inputs rather than analog ones. |
| CRC					| 1		| An 8-bit simple summing CRC of the *payload values* only. This serves as a final protection against accidentally interpreting program data (or unsynced debug data) as ADMP packets. |

### Interpretation of data

Analog inputs are mapped from the 10-bit ADC output to a value between 0 and 249.
Digital inputs are mapped to 250 (LOW) or 251 (high). This non-overlapping map
allows the receiving end to discriminate digital and analog values without any
additional data. It also means 2 bits of resolution are lost on analog values,
but with a 5V reference, it still provides a resolution of 20mV.

### Maximum polling rate

Because ADMPv1 updates all analog values for each packet, the maximum polling
rate is equal to the lesser of data rate or conversion rate.
* data rate = Serial baud rate / Packet size
* conversion rate = ADC rate / Analog channels sampled

With default settings, the ADC rate is approximately 9000 samples/sec. However,
this can be improved to 62500 samples/sec with some register changes.


Version 2: On-demand polling
----------------------------

Version 2 is under development. The key changes from version 1 are:
* The receiver requests each input needed, and requests updates one at a time.
* Each packet will only contain a single input's value.
