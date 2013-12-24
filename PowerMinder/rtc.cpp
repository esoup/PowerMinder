//
//  rtc.cpp
//  
//
// http://playground.arduino.cc//Main/DS1302?action=sourceblock&num=1
//
//

// DS1302 RTC
// ----------
//
// Open Source / Public Domain
//
// Version 1
//     By arduino.cc user "Krodal".
//     June 2012
//     Using Arduino 1.0.1
// Version 2
//     By arduino.cc user "Krodal"
//     March 2013
//     Using Arduino 1.0.3, 1.5.2
//     The code is no longer compatible with older versions.
//     Added bcd2bin, bin2bcd_h, bin2bcd_l
//     A few minor changes.
//

// eSoup Version 1
//  by Ian McCutcheon
//  October 2013
//  split out a .h file
//  
 
//
// Documentation: datasheet
// 
// The DS1302 uses a 3-wire interface: 
//    - bidirectional data.
//    - clock
//    - chip select
// It is not I2C, not OneWire, and not SPI.
// So the standard libraries can not be used.
// Even the shiftOut() function is not used, since it
// could be too fast (it might be slow enough, 
// but that's not certain).
//
// I wrote my own interface code according to the datasheet.
// Any three pins of the Arduino can be used.
//   See the first defines below this comment, 
//   to set your own pins.
//
// The "Chip Enable" pin was called "/Reset" before.
//
// The chip has internal pull-down registers.
// This keeps the chip disabled, even if the pins of 
// the Arduino are floating.
//
//
// Range
// -----
//      seconds : 00-59
//      minutes : 00-59
//      hour    : 1-12 or 0-23
//      date    : 1-31
//      month   : 1-12
//      day     : 1-7
//      year    : 00-99
//
//
// Burst mode
// ----------
// In burst mode, all the clock data is read at once.
// This is to prevent a rollover of a digit during reading.
// The read data is from an internal buffer.
//
// The burst registers are commands, rather than addresses.
// Clock Data Read in Burst Mode
//    Start by writing 0xBF (as the address), 
//    after that: read clock data
// Clock Data Write in Burst Mode
//    Start by writing 0xBE (as the address), 
//    after that: write clock data
// Ram Data Read in Burst Mode
//    Start by writing 0xFF (as the address), 
//    after that: read ram data
// Ram Data Write in Burst Mode
//    Start by writing 0xFE (as the address), 
//    after that: write ram data
//
//
// Ram
// ---
// The DS1302 has 31 of ram, which can be used to store data.
// The contents will be lost if the Arduino is off, 
// and the backup battery gets empty.
// It is better to store data in the EEPROM of the Arduino.
// The burst read or burst write for ram is not implemented 
// in this code.
//
//
// Trickle charge
// --------------
// The DS1302 has a build-in trickle charger.
// That can be used for example with a lithium battery 
// or a supercap.
// Using the trickle charger has not been implemented 
// in this code.
//

#include <Arduino.h>
#include "rtc.h"

// --------------------------------------------------------
// DS1302_clock_burst_read
//
// This function reads 8 bytes clock data in burst mode
// from the DS1302.
//
// This function may be called as the first function,
// also the pinMode is set.
//
void DS1302_clock_burst_read( uint8_t *p)
{
    int i;
    
    _DS1302_start();
    
    // Instead of the address,
    // the CLOCK_BURST_READ command is issued
    // the I/O-line is released for the data
    _DS1302_togglewrite( DS1302_CLOCK_BURST_READ, true);
    
    for( i=0; i<8; i++)
    {
        *p++ = _DS1302_toggleread();
    }
    _DS1302_stop();
}


// --------------------------------------------------------
// DS1302_clock_burst_write
//
// This function writes 8 bytes clock data in burst mode
// to the DS1302.
//
// This function may be called as the first function,
// also the pinMode is set.
//
void DS1302_clock_burst_write( uint8_t *p)
{
    int i;
    
    _DS1302_start();
    
    // Instead of the address,
    // the CLOCK_BURST_WRITE command is issued.
    // the I/O-line is not released
    _DS1302_togglewrite( DS1302_CLOCK_BURST_WRITE, false);
    
    for( i=0; i<8; i++)
    {
        // the I/O-line is not released
        _DS1302_togglewrite( *p++, false);
    }
    _DS1302_stop();
}


// --------------------------------------------------------
// DS1302_read
//
// This function reads a byte from the DS1302
// (clock or ram).
//
// The address could be like "0x80" or "0x81",
// the lowest bit is set anyway.
//
// This function may be called as the first function,
// also the pinMode is set.
//
uint8_t DS1302_read(int address)
{
    uint8_t data;
    
    // set lowest bit (read bit) in address
    bitSet( address, DS1302_READBIT);
    
    _DS1302_start();
    // the I/O-line is released for the data
    _DS1302_togglewrite( address, true);
    data = _DS1302_toggleread();
    _DS1302_stop();
    
    return (data);
}


// --------------------------------------------------------
// DS1302_write
//
// This function writes a byte to the DS1302 (clock or ram).
//
// The address could be like "0x80" or "0x81",
// the lowest bit is cleared anyway.
//
// This function may be called as the first function,
// also the pinMode is set.
//
void DS1302_write( int address, uint8_t data)
{
    // clear lowest bit (read bit) in address
    bitClear( address, DS1302_READBIT);
    
    _DS1302_start();
    // don't release the I/O-line
    _DS1302_togglewrite( address, false);
    // don't release the I/O-line
    _DS1302_togglewrite( data, false);
    _DS1302_stop();
}


// --------------------------------------------------------
// _DS1302_start
//
// A helper function to setup the start condition.
//
// An 'init' function is not used.
// But now the pinMode is set every time.
// That's not a big deal, and it's valid.
// At startup, the pins of the Arduino are high impedance.
// Since the DS1302 has pull-down resistors,
// the signals are low (inactive) until the DS1302 is used.
void _DS1302_start( void)
{
    digitalWrite( DS1302_CE_PIN, LOW); // default, not enabled
    pinMode( DS1302_CE_PIN, OUTPUT);
    
    digitalWrite( DS1302_SCLK_PIN, LOW); // default, clock low
    pinMode( DS1302_SCLK_PIN, OUTPUT);
    
    pinMode( DS1302_IO_PIN, OUTPUT);
    
    digitalWrite( DS1302_CE_PIN, HIGH); // start the session
    delayMicroseconds( 4);           // tCC = 4us
}


// --------------------------------------------------------
// _DS1302_stop
//
// A helper function to finish the communication.
//
void _DS1302_stop(void)
{
    // Set CE low
    digitalWrite( DS1302_CE_PIN, LOW);
    
    delayMicroseconds( 4);           // tCWH = 4us
}


// --------------------------------------------------------
// _DS1302_toggleread
//
// A helper function for reading a byte with bit toggle
//
// This function assumes that the SCLK is still high.
//
uint8_t _DS1302_toggleread( void)
{
    uint8_t i, data;
    
    data = 0;
    for( i = 0; i <= 7; i++)
    {
        // Issue a clock pulse for the next databit.
        // If the 'togglewrite' function was used before
        // this function, the SCLK is already high.
        digitalWrite( DS1302_SCLK_PIN, HIGH);
        delayMicroseconds( 1);
        
        // Clock down, data is ready after some time.
        digitalWrite( DS1302_SCLK_PIN, LOW);
        delayMicroseconds( 1);        // tCL=1000ns, tCDD=800ns
        
        // read bit, and set it in place in 'data' variable
        bitWrite( data, i, digitalRead( DS1302_IO_PIN));
    }
    return( data);
}


// --------------------------------------------------------
// _DS1302_togglewrite
//
// A helper function for writing a byte with bit toggle
//
// The 'release' parameter is for a read after this write.
// It will release the I/O-line and will keep the SCLK high.
//
void _DS1302_togglewrite( uint8_t data, uint8_t release)
{
    int i;
    
    for( i = 0; i <= 7; i++)
    {
        // set a bit of the data on the I/O-line
        digitalWrite( DS1302_IO_PIN, bitRead(data, i));
        delayMicroseconds( 1);     // tDC = 200ns
        
        // clock up, data is read by DS1302
        digitalWrite( DS1302_SCLK_PIN, HIGH);
        delayMicroseconds( 1);     // tCH = 1000ns, tCDH = 800ns
        
        if( release && i == 7)
        {
            // If this write is followed by a read,
            // the I/O-line should be released after
            // the last bit, before the clock line is made low.
            // This is according the datasheet.
            // I have seen other programs that don't release
            // the I/O-line at this moment,
            // and that could cause a shortcut spike
            // on the I/O-line.
            pinMode( DS1302_IO_PIN, INPUT);
            
            // For Arduino 1.0.3, removing the pull-up is no longer needed.
            // Setting the pin as 'INPUT' will already remove the pull-up.
            // digitalWrite (DS1302_IO, LOW); // remove any pull-up  
        }
        else
        {
            digitalWrite( DS1302_SCLK_PIN, LOW);
            delayMicroseconds( 1);       // tCL=1000ns, tCDD=800ns
        }
    }
}
