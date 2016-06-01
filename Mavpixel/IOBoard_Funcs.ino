/*
///////////////////////////////////////////////////////////////////////
//
// Please read licensing, redistribution, modifying, authors and 
// version numbering from main sketch file. This file contains only
// a minimal header.
//
// Copyright (c) 2013, Jani Hirvinen, jDrones & Co.
// All rights reserved.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
//  OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
//  POSSIBILITY OF SUCH DAMAGE.
//
/////////////////////////////////////////////////////////////////////////////
*/
/*

  jD-IOBoard general funcion calls


*/

void changeBaudRate(uint32_t newBaud) {
  Serial.flush();
  Serial.begin(newBaud);
  while(Serial.available()) Serial.read();
}

#ifdef HEARTBEAT
void HeartBeat() {
  c_hbMillis = millis();
  if(c_hbMillis - p_hbMillis > d_hbMillis) 
  {
    // save the last time you blinked the LED 
    p_hbMillis = c_hbMillis;   
    // if the LED is off turn it on and vice-versa:
      if (ledState == LOW)
        ledState = HIGH;
      else
        ledState = LOW;
    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState); 
    messageCounter++;   
   
   
  }
  
}
#endif 

 
// Our generic flight modes for ArduCopter & ArduPlane
void CheckFlightMode() {
  byte loopPos;
  
  
  if(apm_mav_type == 2) { // ArduCopter MultiRotor or ArduCopter Heli
    if(iob_mode == 0) {flMode = STAB;baseState=1;}   // Stabilize
    if(iob_mode == 1) {flMode = ACRO;baseState=2;}   // Acrobatic
    if(iob_mode == 2) {flMode = ALTH;baseState=3;}   // Alt Hold
    if(iob_mode == 3) {flMode = AUTO;baseState=4;}   // Auto
    if(iob_mode == 4) {flMode = GUID;baseState=5;}   // Guided
    if(iob_mode == 5) {flMode = LOIT;baseState=6;}   // Loiter
    if(iob_mode == 6) {flMode = RETL;baseState=1;}   // Return to Launch
    if(iob_mode == 7) {flMode = CIRC;baseState=2;}   // Circle
    if(iob_mode == 9) {flMode = LAND;baseState=3;}  // Land
    if(iob_mode == 11) {flMode = DRFT;baseState=4;}  // Drift
    if(iob_mode == 13) {flMode = SPRT;baseState=5;}  // Sport
    if(iob_mode == 14) {flMode = FLIP;baseState=6;}  // Flip
    if(iob_mode == 15) {flMode = ATUN;baseState=1;}  // Autotune
    if(iob_mode == 16) {flMode = POSH;baseState=2;}   // Poshold
    if(iob_mode == 17) {flMode = BRAK;baseState=3;}  // Brake
    if(iob_mode == 18) {flMode = THRO;baseState=4;}  // Throw
  }
  else if(apm_mav_type == 1) { // ArduPlane
    if(iob_mode == 2 ) {flMode = STAB;baseState=1;}  // Stabilize
    if(iob_mode == 0) {flMode = MANU;baseState=2;}   // Manual
    if(iob_mode == 12) {flMode = LOIT;baseState=3;}  // Loiter
    if(iob_mode == 11 ) {flMode = RETL;baseState=4;} // Return to Launch
    if(iob_mode == 5 ) {flMode = FBWA;baseState=5;}  // FLY_BY_WIRE_A
    if(iob_mode == 6 ) {flMode = FBWB;baseState=6;}  // FLY_BY_WIRE_B
    if(iob_mode == 15) {flMode = GUID;baseState=1;}  // GUIDED
    if(iob_mode == 10 ){flMode = AUTO;baseState=2;} // AUTO
    if(iob_mode == 1) {flMode = CIRC;baseState=3;}   // CIRCLE
    if(iob_mode == 3) {flMode = TRAN;baseState=4;}   // Training
    if(iob_mode == 4) {flMode = ACRO;baseState=5;}   // Acro
    if(iob_mode == 7) {flMode = CRUS;baseState=6;}   // Cruise
    if(iob_mode == 8) {flMode = ATUN;baseState=1;}   // Autotune
  }
#ifdef JD_IO
#ifndef NEWPAT  
  patt = flMode + 1;
#else 
  DPL(mbind01_ADDR + flMode);
  while(flMode != readEEPROM(mbind01_ADDR + (loopPos))) {
   DPL(flMode);
   loopPos = loopPos + 2;
  }    
  DPL(loopPos);

  pattByteA = readEEPROM(pat01_ADDR + loopPos);
  pattByteB = readEEPROM(pat01_ADDR + (loopPos + 1));
  DPL(pattByteA, BIN);
  DPL(pattByteB, BIN); 

/*
  tempvar = readEEPROM(mbind01_ADDR  + (flMode * 2) + 1);
  tempvar--; // - 1 to match correct pattern. As banks starts from 0 and modes from 1.
  pattByteA = readEEPROM(pat01_ADDR + (tempvar * 2));
  pattByteB = readEEPROM(pat01_ADDR + (tempvar * 2) + 1);
  
*/
//  DPL(pattByteA, BIN);
//  DPL(pattByteB, BIN);
#endif
#endif
}
  
// Checking if BIT is active in PARAM, return true if it is, false if not
byte isBit(byte param, byte bitfield) {
 if((param & bitfield) == bitfield) return 1;
  else return 0;  
}

#ifdef JD_IO
// Roll all outputs one time one by one and have delay XX ms between status changes
void SlowRoll(int FlashDly) {
 for(int pos = 0; pos <= 5; pos ++ ) {
   digitalWrite(Out[pos], EN);
   delay(FlashDly);
   digitalWrite(Out[pos], DI);
   delay(FlashDly);   
 }
}

// Switch all outputs ON
void AllOn() {
  if(debug) Serial.println("AllOn");
   for(int looper = 0; looper <= 5; looper++) {
    digitalWrite(Out[looper], EN);
  }  
}


// Switch all outputs OFF
void AllOff() {
  if(debug) Serial.println("AllOff");
   for(int looper = 0; looper <= 5; looper++) {
    digitalWrite(Out[looper], DI);
  }  
}


// Update main pattern
void RunPattern() {
#ifndef NEWPAT  
   digitalWrite(Out[REAR], flight_patt[patt][patt_pos]);
#else
      if(patt_pos >= 0 && patt_pos <= 7) {
        if(getLBit(pattByteA, patt_pos)) 
          digitalWrite(Out[REAR], EN);
        else
          digitalWrite(Out[REAR], DI);
      } else {
        if(getLBit(pattByteB, patt_pos - 8)) 
          digitalWrite(Out[REAR], EN);
        else
          digitalWrite(Out[REAR], DI);
      }
#endif
}

// Clear main pattern
void ClearPattern() {
   digitalWrite(Out[REAR], 0);
}

// Updating base leds state
void updateBase() {
   digitalWrite(Out[LEFT], le_patt[LeRiPatt][patt_pos]);
   digitalWrite(Out[RIGHT], ri_patt[LeRiPatt][patt_pos]);
}


// Reads current state of high power output and save them to parameter
void GetIO() {
  for(int pos = 0; pos <= 5; pos++) {
   IOState[pos] = digitalRead(Out[pos]);
  }
}

// Recalls save value for highpower IO states and outputs them back
void PutIO() {
  for(int pos = 0; pos <= 5; pos++) {
   digitalWrite(Out[pos], IOState[pos]);
  }
}


void updatePWM() {
    curPwm = millis();
    if(curPwm - prePwm > 5) {
      // save the last time you blinked the LED 
      prePwm = curPwm;
    if (pwm1dir) {
      pwm1++;
    } 
    else 
    {
      pwm1--;
    }
    if(pwm1 >= 255 && pwm1dir == 1) pwm1dir = 0;
    if(pwm1 <= 20 && pwm1dir == 0) pwm1dir = 1;
    analogWrite(Out[FRONT], pwm1);
    
//    if(iob_pitch>0)
//    {
//      analogWrite(RIGHT,(iob_roll*2));
//    }
//    else if(iob_pitch<0)
//    {
//      analogWrite(LEFT,abs(iob_roll)*2);
//    }
//    else
//    {
//      analogWrite(RIGHT,0);
//      analogWrite(LEFT,0);
//    }
    
    }
}

boolean getLBit(byte Reg, byte whichBit) {
  boolean State;
  switch(whichBit) {
    case 0:
     State = Reg & (1 << 7);
     return State;
     break;
    case 1:
     State = Reg & (1 << 6);
     return State;
     break;
    case 2:
     State = Reg & (1 << 5);
     return State;
     break;
    case 3:
     State = Reg & (1 << 4);
     return State;
     break;
    case 4:
     State = Reg & (1 << 3);
     return State;
     break;
    case 5:
     State = Reg & (1 << 2);
     return State;
     break;
    case 6:
     State = Reg & (1 << 1);
     return State;
     break;
    case 7:
     State = Reg & (1 << 0);
     return State;
     break;
  }
}
#endif
#ifdef FRSKY
void Batt_Alarm_LED(void)
{
     //-------Operat Battery Display Alarm LED------//
   switch(Batt_Cell_Detect)
   {
     case 0x06:{
                  if(iob_vbat_A < (Batt_Volt_Cell6_Config + ((Batt_Volt_Cell6_Config/100) * BattAlarmPercentage)) )  // 15% of battery 6cell  low->18  
                  {
                    LeRiPatt=3;
                  }
                  else
                  {
                    LeRiPatt=0;
                  }
               };break;
     case 0x05:{
                  if(iob_vbat_A < (Batt_Volt_Cell5_Config + ((Batt_Volt_Cell5_Config/100) * BattAlarmPercentage)) )   // 15% of battery low  5cell  low->15
                  {
                     LeRiPatt=3;
                  }
                  else
                  {
                     LeRiPatt=0;
                  }
               };break;
     case 0x04:{
                  if(iob_vbat_A < (Batt_Volt_Cell4_Config + ((Batt_Volt_Cell4_Config/100) * BattAlarmPercentage)) )  // 15% of battery low  4cell  low->12
                  {
                     LeRiPatt=3;
                  }
                  else 
                  {
                     LeRiPatt=0;
                  }
               };break;
     case 0x03:{
                  if(iob_vbat_A < (Batt_Volt_Cell3_Config + ((Batt_Volt_Cell3_Config/100) * BattAlarmPercentage)) )  // 15% of battery low  3cell  low->9
                  {
                     LeRiPatt=3;
                  }
                  else
                  {
                     LeRiPatt=0;
                  }
               };break;
     // default: ;break;
   }
}
#endif


