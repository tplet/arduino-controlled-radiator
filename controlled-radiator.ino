/**
 * Radiator control remote
 * 
 * Used from Noirot DCN series
 * 
 * Commands:
 * 10 - Confort: 0 V (IC+ and IC- off)
 * 20 - Eco: 230 V (IC+ and IC- on)
 * 30 - Hors-gel: 115 V négatif (IC+ off, IC- on)
 * 40 - Stop: 115 V positif (IC+ on, IC- off)
 * 50 - Conf. -1°C: 230 V during 3s (not tested)
 * 60 - Conf. -2°C: 230 V during 7s (not tested)
 *
 *******************************
 */

// Enable debug prints
//#define MY_DEBUG

// Extend network (only for no-battery power)
#define MY_REPEATER_FEATURE

// Enable and select radio type attached 
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69
//#define MY_RS485

// Pins CE, CSN for ARDUINO
#define MY_RF24_CE_PIN    9
#define MY_RF24_CS_PIN   10

// Pin for radiator control
#define IC_P_PIN 2 // IC+
#define IC_M_PIN 3 // IC-

// IC pin state (on/off)
#define IC_ON  1
#define IC_OFF 0

// State values
#define STATE_COMFORT 10
#define STATE_ECO 20
#define STATE_FROST_PROTECT 30
#define STATE_STOP 40
#define STATE_COMFORT_M1 50
#define STATE_COMFORT_M2 60

// EEPROM address to store state
#define ADDRESS_STATE 0

// Interval to send heart beat (ms)
#define INTERVAL_PING 30000 // 30s

#include <Wire.h>
#include <SPI.h>
#include <MySensors.h>  
#include "SparkFunMAX17043.h"
#include "DataBuffer.h"

#define CHILD_ID 0

unsigned int state = STATE_STOP; // Default value
DataBuffer heartbeatBuffer(INTERVAL_PING);
// Message which contain probe value
MyMessage messageState(CHILD_ID, V_PERCENTAGE);

void before()
{
  // Set IC pins in output mode
  pinMode(IC_P_PIN, OUTPUT);
  pinMode(IC_M_PIN, OUTPUT);
}

void presentation()  
{ 
  // Send the sketch version information to the gateway
  sendSketchInfo("ControlledRadiator", "1.0.0");

  // Register all sensors to gw (they will be created as child devices)
  present(CHILD_ID, S_DIMMER, "RadiatorState", true);
}


void setup()
{
  // Restore last know state
  unsigned int s = loadState(ADDRESS_STATE);
    
  // Set relay to last known state (using eeprom storage). If no value stored (=255), ignore restored state
  if (s != 255) {
    setState(s);
  }
  // Initial state on first boot
  else {
    setState(STATE_STOP);
  }

  // Send initial state to declare device or confirm value
  send(messageState.set(getState()));  
}


void loop()      
{
  // Send heart beat
  if (heartbeatBuffer.isOutdated()) {
    sendHeartbeat();
    heartbeatBuffer.reset();
  }

  delay(500);
}

/**
 * When receive message
 */
void receive(const MyMessage &message)
{
    // We only expect one type of message from controller. But we better check anyway.
    if (message.type == V_PERCENTAGE) {

        // Read new state
        unsigned int s = message.getUInt();

        // Check if value allowed
        if (isStateValueAllowed(s)) {
          #ifdef MY_DEBUG
          Serial.print("Incoming change for sensor:");
          Serial.println(s);
          #endif

          // Change state value
          setState(s);
        }        
    }
}

/**
 * Check if state value is allowed
 */
bool isStateValueAllowed(unsigned int s)
{
  return s == STATE_COMFORT || s == STATE_ECO || s == STATE_FROST_PROTECT || s == STATE_STOP || s == STATE_COMFORT_M1 || s == STATE_COMFORT_M2;
}

/**
 * Get state
 * 
 * @return unsigned int
 */
unsigned int getState()
{
  return state;
}

/**
 * Set state and apply command on IC pins
 */
void setState(unsigned int s)
{
  state = s;

  // Save state to eeprom
  saveState(ADDRESS_STATE, state);
  
  switch (state) {
    // 10 - Confort: 0 V (IC+ and IC- off)
    case STATE_COMFORT :
      digitalWrite(IC_P_PIN, IC_OFF);
      digitalWrite(IC_M_PIN, IC_OFF);
      break;
    // 20 - Eco: 230 V (IC+ and IC- on)
    case STATE_ECO :
      digitalWrite(IC_P_PIN, IC_ON);
      digitalWrite(IC_M_PIN, IC_ON);
      break;
    // 30 - Hors-gel: 115 V négatif (IC+ off, IC- on)
    case STATE_FROST_PROTECT :
      digitalWrite(IC_P_PIN, IC_OFF);
      digitalWrite(IC_M_PIN, IC_ON);
      break;
    // 40 - Stop: 115 V positif (IC+ on, IC- off)
    case STATE_STOP :
      digitalWrite(IC_P_PIN, IC_ON);
      digitalWrite(IC_M_PIN, IC_OFF);
      break;    
    // 50 - Conf. -1°C: 230 V during 3s (not tested)
    case STATE_COMFORT_M1 :
      digitalWrite(IC_P_PIN, IC_ON);
      digitalWrite(IC_M_PIN, IC_ON);
      wait(3000);
      digitalWrite(IC_P_PIN, IC_OFF);
      digitalWrite(IC_M_PIN, IC_OFF);
      break;    
    // 60 - Conf. -2°C: 230 V during 7s (not tested)
    case STATE_COMFORT_M2 :
      digitalWrite(IC_P_PIN, IC_ON);
      digitalWrite(IC_M_PIN, IC_ON);
      wait(7000);
      digitalWrite(IC_P_PIN, IC_OFF);
      digitalWrite(IC_M_PIN, IC_OFF);
      break;    
  }
}

