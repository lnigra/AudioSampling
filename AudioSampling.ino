/**
 * ArduinoSampling.ino
 * Lou Nigra 6/2020
 * 
 * Audio sampling and playback with the Arduino UNO or Teensy 3.5-6
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
 
// Include pre-written library code.
#include <SPI.h>
#include <SD.h>

// Define unchanging constant parameters using special 
// "const" modifier. This is equivalent to #DEFINE, but prettier,
// for one thing.
const int csPin = 10; // For various Arduino SD shields
//const int csPin = BUILTIN_SDCARD; //for Teensy 3.5-6
const int analogInPin = A0;
const int bytesPerValue = 2;

// Variables
unsigned int val;
unsigned long timer, deltaT, maxDeltaT, numSamp;
byte data[2];
File myFile;

void setup() {
  
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  //delay( 1000 ); // For Arduino to set the serial port up.
  while ( !Serial ); // Teensy.
   
  Serial.print("Initializing SD card...");
  
  if ( !SD.begin( csPin ) ) {
    
    Serial.println("initialization failed!");
    Serial.println("Ending program");
    Serial.flush();
    exit(0);
    
  }
  
  Serial.println("initialization done.");

  // Delete the old file so we create a blank new file when
  // we open it because we don't want this new data
  // to add (append) to the end of the old data
  SD.remove( "test.dat" );
  myFile = SD.open("test.dat", O_WRITE | O_CREAT );
  
  // If the file doesn't open, something's wrong - abort the
  // program.
  if (!myFile) {
    // if the file didn't open, print an error:
    Serial.println("error opening test.dat");
    Serial.println("Ending program");
    Serial.flush();
    exit(0); // Aborts the program.
  }

  // Initialize some variables that need pre-defined values.
  data[0] = 0; // See below, dummy writes will be zero.
  data[1] = 0;
  maxDeltaT = 0; // Start with the lowest possible value
  numSamp = 50000; // Sets the number of samples in the file.

  // First two samples are dummy read/writes without measuring
  // elapsed time. First sample takes a very long time to read
  // from the AD converter and to write to the SD. Second
  // dummy sample is just for good measure.
   
  for ( int i = 0 ; i < 2 ; i++ ) {
    
    analogRead( A0 ); // Dummy read AD converter
    myFile.write( data, 2 ); // Dummy write zeroes
    
  } 

}

void loop() {
  
  for ( unsigned long i = 0 ; i < numSamp ; i++ ) {
    
      timer = micros(); // Establish time reference for sample     
      val = analogRead( A0 ); // Read sample value (two byte number)
      data[0] = lowByte( val ); // Can only store one byte at a time
      data[1] = highByte( val );
      myFile.write( data, bytesPerValue ); // Store the two bytes.
      deltaT = micros() - timer; // Measure elapsed time.
      Serial.println( deltaT );
      if ( deltaT > maxDeltaT ) maxDeltaT = deltaT; // Track maximum
          
  }
  
  myFile.close(); // Required to be sure the data is in the file.
  Serial.println( "Done sampling. Reading file." );

  // Open the file for reading back the results
  myFile = SD.open( "test.dat" );
  
  if( myFile ) { // If opened, go ahead and read it until end-of-file.
    
    Serial.println( "test.dat opened" );
    
    while( myFile.available() ) {
      
      Serial.println( (unsigned int)myFile.read() 
                    + (unsigned int)myFile.read() * 256, DEC );
                    
    }
     
  } else { // Failed to open, somethings wrong, but finish the program
    
    Serial.println( "Error opening file" );
    
  }

  // Finish up the program
  Serial.print( "Max delta T:" );Serial.println( maxDeltaT );
  myFile.close();
  
  // This is a trap, repeats forever. Keeps loop() from repeating
  // You can also use exit(0) to force the microcontroller to
  // stop running your program.
  while( true );
}
