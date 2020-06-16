/**
 * ArduinoSampling.ino
 * Lou Nigra 6/2020
 * 
 * Audio sampling and playback with the arduino
 * 
 * CURRENT STATUS: Test program to characterize the recording process
 * sampling delays including the periodic lengthy SD card management
 * delays.
 * 
 * PROGRAM FUNCTION:
 * Measure the raw sampling interval while recording without using 
 * interrupts.
 * 
 * The 3.3V output is connected to A0 only to have something known to
 * sample.
 * 
 * The AD is sampled, the 10-bit result is converted to two bytes and
 * stored little-ended (per WAV file standard) to the file.
 * 
 * Time between samples is measured as deltaT and is printed out
 * as each sample is stored, but its maximum value is maintained to
 * be reported at the end of the program as two bytes little-ended 
 * prior to the audio data.
 * 
 * After the recording loop, the file is read and sample
 * byte pairs are converted back to original int values and printed
 * out. maxDeltaT is found during this process and printed out at 
 * the end.
 * 
 */

/**
 * This program is modified from the Arduino SD example "ReadWrite"
 * per header below.
 * 
 * NOTE:SD CS pin changed for the SD card sheild being used.
 */
 
/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10 (Arduino) BUILTIN_SDCARD constant (Teensy 3.5-6)

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
#include <SD.h>

const int csPin = 10;
const int analogInPin = A0;
const int bytesPerValue = 2;
unsigned int val;
unsigned long timer, deltaT, maxDeltaT, numSamp;
byte data[2];
File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  delay( 1000 ); // while ( !Serial ); for teensy. 
  Serial.print("Initializing SD card...");
  if ( !SD.begin( csPin ) ) {
    Serial.println("initialization failed!");
    Serial.println("Ending program");
    Serial.flush();
    exit(0);
  }
  Serial.println("initialization done.");
  SD.remove( "test.dat" );
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.dat", FILE_WRITE ); // O_WRITE for seek to work
  //myFile.seek( 0 );
  // if the file opened okay, write to it:
  if (!myFile) {
    // if the file didn't open, print an error:
    Serial.println("error opening test.dat");
    Serial.println("Ending program");
    Serial.flush();
    exit(0);
  }
  deltaT = 0;
  val = 0;
  data[0] = 0;
  data[1] = 0;
  maxDeltaT = 0;
  numSamp = 200000;
}

void loop() {
  for ( unsigned long i = 0 ; i < numSamp ; i++ ) {
    timer = micros();
    if ( i < 2 ) {
      analogRead( A0 );
      myFile.write( data, 2 );
      myFile.seek( 0 );
    } else {
      val = analogRead( A0 );
      data[0] = lowByte( val );
      data[1] = highByte( val );
      myFile.write( data, bytesPerValue );
      deltaT = micros() - timer;
      Serial.println( deltaT );
      if ( deltaT > maxDeltaT ) maxDeltaT = deltaT;
    }
  }
  myFile.close();
  Serial.println( "Done sampling. Reading file." );
  myFile = SD.open( "test.dat" );
  if( myFile ) {
    Serial.println( "test.dat opened" );
    while( myFile.available() ) {
      Serial.println( (unsigned int)myFile.read() + (unsigned int)myFile.read() * 256, DEC );
    } 
  } else {
    Serial.println( "Error opening file" );
  }
  Serial.print( "Max delta T:" );Serial.println( maxDeltaT );
  myFile.close();
  while( true );
}
