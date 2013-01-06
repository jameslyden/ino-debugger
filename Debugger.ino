/* A drop-in debugging routine to add to Arduino projects.
 * Setup requires configuring the serial port as needed, adding the Debug function to the sketch,
 * adding the global variables, and calling Debug() from somewhere in the body of loop()
 *
 * This sketch can also be used as-is, in which case it provides near-realtime I/O status,
 * with no additional functionality.
 */

//int currDebugTime;
//int lastDebugTime;
 
void setup() {
  Serial.begin(115200);
}
 
void loop() {
  // do work
  delay(100);
  // collect pin states and send to serial port 
  Debug();  
}

void Debug() {
  // setup analog and digital pin ranges
  int aPinStart = A0;
  int aPinEnd = A2;
  int dPinStart = 2;
  int dPinEnd = 6;
  // all debug data is assembled into a char array before writing
  // structure is 5B header, 1B per analog pin, 1B per digital pin, an 1B CRC value
  int debugPayloadSize = (aPinEnd - aPinStart + 1) + (dPinEnd - dPinStart + 1);
  int debugDataSize = 4 + debugPayloadSize + 1;
  int debugDataCount = 0;
  char debugData[debugDataSize];

  // Start with a header, should prevent accidental interpretation of other program data as debug data
  debugData[debugDataCount++] = 255;
  debugData[debugDataCount++] = 254;
  debugData[debugDataCount++] = 253;
  debugData[debugDataCount++] = debugDataSize;
  
  // Compute ms since last run through this function
//  currDebugTime = millis();
//  int interval = currDebugTime - lastDebugTime;
//  if (interval > 255) {
//    interval = 255;
//  }
//  debugData[debugDataCount++] = interval;
//  lastDebugTime = currDebugTime;
  
  // Iterate through analog inputs, mapping to a 250-point scale
  for (int i = aPinStart; i <= aPinEnd; i++) {
    debugData[debugDataCount++] = map(analogRead(i), 0, 1023, 0, 249);
  }
  
  // Iterate through digital inputs, mapping LOW to 250 and HIGH to 251
  for (int i = dPinStart; i <= dPinEnd; i++) {
    debugData[debugDataCount++] = digitalRead(i) + 250; 
  }
  
  // Build 8-bit CRC from payload
  int CRC = 0;  
  for (int i = 4; i < debugDataSize - 1; i++) {
    CRC = CRC + debugData[i];
    CRC = CRC % 256;
  }
  debugData[debugDataSize - 1] = CRC;
  
  // Send debug data over serial port
  Serial.print(debugData);
}
  
  
