# AudioSampling
Basic audio recording and playback on Arduino UNO

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
 * Time between samples is measured as deltaT and is also stored
 * as two bytes little-ended prior to the audio data.
 * 
 * After the recording loop, the file is read and deltaT and Sample
 * bytes are converted to int and printed out.maxDeltaT is found
 * during this process and printed out at the end.
 * 
