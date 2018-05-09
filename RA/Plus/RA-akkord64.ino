#include <ReefAngel_Features.h>
#include <Globals.h>
#include <RA_Wifi.h>
#include <Wire.h>
#include <OneWire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <InternalEEPROM.h>
#include <RA_NokiaLCD.h>
#include <RA_ATO.h>
#include <RA_Joystick.h>
#include <LED.h>
#include <RA_TempSensor.h>
#include <Relay.h>
#include <RA_PWM.h>
#include <Timer.h>
#include <Memory.h>
#include <InternalEEPROM.h>
#include <RA_Colors.h>
#include <RA_CustomColors.h>
#include <Salinity.h>
#include <RF.h>
#include <IO.h>
#include <ORP.h>
#include <AI.h>
#include <PH.h>
#include <WaterLevel.h>
#include <Humidity.h>
#include <DCPump.h>
#include <PAR.h>
#include <ReefAngel.h>


////// Place global variable code below here

////// Place global variable code above here


void setup()
{
    // This must be the first line
    ReefAngel.Init();  //Initialize controller
    ReefAngel.AddStandardMenu();  // Add Standard Menu
  //  ReefAngel.Use2014Screen();  // Let's use 2014 Screen
    // Ports toggled in Feeding Mode
    ReefAngel.FeedingModePorts = Port1Bit | Port4Bit;
    ReefAngel.FeedingModePortsE[0] = 0;
    ReefAngel.FeedingModePortsE[1] = 0;
    // Ports toggled in Water Change Mode
    ReefAngel.WaterChangePorts = Port1Bit | Port2Bit | Port3Bit | Port4Bit;
    ReefAngel.WaterChangePortsE[0] = 0;
    ReefAngel.WaterChangePortsE[1] = 0;
    // Ports turned off when Overheat temperature exceeded
    ReefAngel.OverheatShutoffPorts = Port3Bit;
    ReefAngel.OverheatShutoffPortsE[0] = 0;
    ReefAngel.OverheatShutoffPortsE[1] = 0;
    // Ports toggled when Lights On / Off menu entry selected
    ReefAngel.LightsOnPorts = 0;
    ReefAngel.LightsOnPortsE[0] = 0;
    ReefAngel.LightsOnPortsE[1] = 0;
    // Use T1 probe as temperature and overheat functions
    ReefAngel.TempProbe = T1_PROBE;
    ReefAngel.OverheatProbe = T1_PROBE;
    // Set the Overheat temperature setting
    InternalMemory.OverheatTemp_write( 800 );
    // Feeeding and Water Change mode speed
    ReefAngel.DCPump.FeedingSpeed=0;
    ReefAngel.DCPump.WaterChangeSpeed=0;

    // Ports that are always on
    ReefAngel.Relay.On( Port1 );
    ReefAngel.Relay.On( Port2 );
    ReefAngel.Relay.On( Port5 );
    ReefAngel.Relay.On( Port6 );
    ReefAngel.Relay.On( Port7 );
    ReefAngel.Relay.On( Port8 );
    ReefAngel.Relay.On( Box1_Port1 );
    ReefAngel.Relay.On( Box1_Port2 );
    ReefAngel.Relay.On( Box1_Port3 );
    ReefAngel.Relay.On( Box1_Port4 );
    ReefAngel.Relay.On( Box1_Port5 );
    ReefAngel.Relay.On( Box1_Port6 );
    ReefAngel.Relay.On( Box1_Port7 );
    ReefAngel.Relay.On( Box1_Port8 );

    ////// Place additional initialization code below here

   
    ////// Place additional initialization code above here
}

void loop()
{
    ReefAngel.StandardFan( T1_PROBE,Port4,770,785 ); // Enable Fan to come on at 78.5 and cool until 77.0
    ReefAngel.StandardHeater( T2_PROBE,Port3,750,770 ); // Enable Heater to kick on at 75 and off at 77.0
    // ReefAngel.DCPump.UseMemory = false;
    ReefAngel.DCPump.Threshold=20;          // Set lower limits for Jebao pumps
    ReefAngel.DCPump.DaylightChannel = None;
    ReefAngel.DCPump.ActinicChannel = None;
    ReefAngel.DCPump.FeedingSpeed=0;
    ReefAngel.DCPump.WaterChangeSpeed=0;
    ReefAngel.DCPump.ExpansionChannel[2] = None;
    ReefAngel.DCPump.ExpansionChannel[3] = None;
    ReefAngel.DCPump.ExpansionChannel[4] = None;
    ReefAngel.DCPump.ExpansionChannel[5] = None;

    boolean buzzer=false;
    if ( ReefAngel.isATOTimeOut() ) buzzer=true;
    if ( ReefAngel.isOverheat() ) buzzer=true;
    if ( ReefAngel.isBusLock() ) buzzer=true;
    ReefAngel.PWM.SetDaylight( buzzer?0:100 );

    ////// Place your custom code below here
// WavePump Cycle
 if (ReefAngel.DisplayedMenu!=FEEDING_MODE || ReefAngel.DisplayedMenu!=WATERCHANGE_MODE)
 {
if (hour()>=20 && hour()<9) // Slow everything down for sleepytime 9PM-9AM 5-25%
    {
//    ReefAngel.DCPump.SetMode( ReefCrest,25,10 );
    //ReefAngel.DCPump.UseMemory = false;
    ReefAngel.DCPump.SetMode(TidalSwell,40, 0 );
    ReefAngel.DCPump.ExpansionChannel[0] = Sync;
    ReefAngel.DCPump.ExpansionChannel[1] = AntiSync;  
    ReefAngel.PWM.SetChannel(2,ShortPulseMode(0,60,200,true));
    ReefAngel.PWM.SetChannel(3,ShortPulseMode(0,60,200,true));
    }
else if (hour()>=16 && hour()<17) // Nutrient Export mode for 1hr Stirring up the tank
    {
    //ReefAngel.DCPump.UseMemory = false;
    ReefAngel.DCPump.SetMode(NutrientTransport,40,90,3000);
    ReefAngel.DCPump.ExpansionChannel[0] = AntiSync;
    ReefAngel.DCPump.ExpansionChannel[1] = Sync;
    ReefAngel.PWM.SetChannel(2, NutrientTransportMode(40,90,3000, true));
    ReefAngel.PWM.SetChannel(3, NutrientTransportMode(40,90,3000, false));  
    }
else// Normal Daytime Reefcrest --Default for DCPump Set more is +-20% - 30-70% direction change 10seconds
    {
    //ReefAngel.DCPump.UseMemory = false;
    ReefAngel.DCPump.SetMode( ReefCrest,50,10 );
    ReefAngel.DCPump.ExpansionChannel[0] = Sync;
    ReefAngel.DCPump.ExpansionChannel[1] = AntiSync;
    ReefAngel.PWM.SetChannel(2,ShortPulseMode(0,60,200,true));
    ReefAngel.PWM.SetChannel(3,ShortPulseMode(0,60,200,true));
    } 
 }

if (ReefAngel.DisplayedMenu==FEEDING_MODE || ReefAngel.DisplayedMenu==WATERCHANGE_MODE) 
{
 ReefAngel.PWM.SetChannel(2,0); //Turn off rear pumps in Feed|Waterchange Mode
 ReefAngel.PWM.SetChannel(3,0); //Turn off rear pumps in Feed|Waterchange Mode
}
////// Place your custom code above here



    ////// Place your custom code above here

    ReefAngel.CloudPortal();
    // This should always be the last line
    ReefAngel.ShowInterface();
}

// graphics for the water level bar
const unsigned PROGMEM char BAR_CENTER[] = {0x92, 0x92, 0xFF, 0xFF, 0xFF, 0xFF, 0xDB, 0xDB, 0xB6, 0xB6, 0xB6, 0xB6};
const unsigned PROGMEM char BAR_WL_LEFT[] = {0x29, 0x29, 0x2A, 0x72, 0x72, 0x96};
const unsigned PROGMEM char BAR_WL_RIGHT[] = {0x6E, 0x92, 0x96, 0xDB, 0xDB, 0xDB};
const unsigned PROGMEM char BAR_DP_LEFT[] = {0xAB, 0xAB, 0xAF, 0xD3, 0xD7, 0xFB};
const unsigned PROGMEM char BAR_DP_RIGHT[] = {0xB7, 0xB7, 0xB7, 0xDB, 0xDB, 0xDB};
#define LEFT_EDGE 8
#define LINE1   19
#define LINE2   47
#define LINE3   70
#define WL_BAR_OFFSET 41
#define DC_BAR_OFFSET 4


// DC Pump mode text strings
String PUMP_MODES[] = {"Constant","Lagoon","ReefCrest","Short Pulse","Long Pulse","Nutrient",
                       "Tidal Swell","Feeding","","Night","","Custom","Else","Sine","Gyre"};
// Memory Locaitons
#define CURT_MEMORY              50
#define DCPUMP_NIGHT_MODE        CURT_MEMORY
#define DCPUMP_NIGHT_SPEED       DCPUMP_NIGHT_MODE + 1
#define DCPUMP_NIGHT_DURATION    DCPUMP_NIGHT_MODE + 2
#define DCPUMP_NIGHT_THRESHOLD   DCPUMP_NIGHT_MODE + 3

// Custom functions
boolean isNightTime();
void checkCustomMemoryValues();
uint8_t DCPumpNightMode_read();
uint8_t DCPumpNightSpeed_read();
uint8_t DCPumpNightDuration_read();
uint8_t DCPumpNightThreshold_read();
uint8_t CustomRead(int address);
void CustomWrite(int address, const uint8_t value);

void DrawCustomMain()
{
    // Fonts in use:   NUMBERS_16x16, FONT_8x16
    byte x = 6;
    byte y = 3;
    byte t;
    char text[11];
    boolean fNight = isNightTime();

#ifdef __TEST__
    ReefAngel.LCD.DrawText(COLOR_RED, DefaultBGColor, x, y, "_____TEST CODE_____");
#else
    // Date & time
    ReefAngel.LCD.DrawDate(x, y);
#endif  // __TEST__

    // Divider line
    ReefAngel.LCD.Clear(COLOR_BLACK, 1, 11, 128, 11);

    // Draw T1
    ConvertNumToString(text, ReefAngel.Params.Temp[T1_PROBE], 10);
    x = 8;
    y = LINE1;  // 19
    // NOTE huge numbers are 16x16 (width x height)
    ReefAngel.LCD.Clear(DefaultBGColor, x, y, x+64, y+16);
    ReefAngel.LCD.DrawHugeNumbers(T1TempColor, DefaultBGColor, x, y, text);

    // Draw T2
    // NOTE 4 digits for T1 (16 width, 16*4 pixels)
    x = 80;
#ifdef __TEST__
    ReefAngel.Params.Temp[T2_PROBE] = 810;
#endif  // __TEST__
    if(ReefAngel.Params.Temp[T2_PROBE] > 0) {
        ReefAngel.LCD.DrawText(T2TempColor, DefaultBGColor, x, y, "T2:");
        // NOTE "T2:" is 3 chars of 5 pixels each (15 pixels), plus 2 pixels space after, start on 3rd pixel
        ReefAngel.LCD.DrawSingleMonitor(ReefAngel.Params.Temp[T2_PROBE], T2TempColor, x+18, y, 10);
    } else {
        // No, T2, clear the entire space for it
        ReefAngel.LCD.Clear(DefaultBGColor, x, y, x+38, y+8);
    }

    // Draw T3 if present
    y = 29;
#ifdef __TEST__
    ReefAngel.Params.Temp[T3_PROBE] = 796;
#endif  // __TEST__
    if(ReefAngel.Params.Temp[T3_PROBE] > 0) {
        ReefAngel.LCD.DrawText(T3TempColor, DefaultBGColor, x, y, "T3:");
        // NOTE "T3:" is 3 chars of 5 pixels each (15 pixels), plus 2 pixels space after, start on 3rd pixel
        ReefAngel.LCD.DrawSingleMonitor(ReefAngel.Params.Temp[T3_PROBE], T3TempColor, x+18, y, 10);
    } else {
        // No, T3, clear the entire space for it
        ReefAngel.LCD.Clear(DefaultBGColor, x, y, x+38, y+8);
    }

    // Draw PH
    y = LINE2;
#ifdef __TEST__
    strcpy(text, "8.20");
#else
    ConvertNumToString(text, ReefAngel.Params.PH, 100);
#endif // __TEST__
    // NOTE ph max is X.XX, 4 digits
    ReefAngel.LCD.DrawLargeText(PHColor, DefaultBGColor, 3, y, "pH:", Font8x16);
    x = 29;
    // TODO confirm width to clear
    ReefAngel.LCD.Clear(DefaultBGColor, x, y, x+32, y+16);
    ReefAngel.LCD.DrawLargeText(PHColor, DefaultBGColor, x, y, text, Font8x16);

    // Draw Salinity
#ifdef __TEST__
    strcpy(text, "35.0");
#else
    ConvertNumToString(text, ReefAngel.Params.Salinity, 10);
#endif // __TEST__
    // NOTE Salinity max is XX.X, 4 digits
    x = 69;
    // TODO confirm width to clear
    ReefAngel.LCD.Clear(DefaultBGColor, x, y, x+32, y+16);
    ReefAngel.LCD.DrawLargeText(COLOR_CRIMSON, DefaultBGColor, x, y, text, Font8x16);
    ReefAngel.LCD.DrawLargeText(COLOR_CRIMSON, DefaultBGColor, 102, y, "ppt", Font8x16);

    pingSerial();

    // Draw Water Level Box (used code from libraries)
    y = LINE3;
    // TODO determine color to use for WATER level (COLOR_ROYALBLUE)
    ReefAngel.LCD.DrawText(COLOR_ROYALBLUE, DefaultBGColor, 8, y, "Water");
#ifdef __TEST__
    t = 79;
#else
    t = ReefAngel.WaterLevel.GetLevel(0);
#endif  // __TEST__
    // sanity check, ensure level isn't greater than 100
    if(t > 100) {
        t = 100;
    }
    x = 99;
    // draw value to the right of the bar
    ReefAngel.LCD.DrawText(COLOR_ROYALBLUE, DefaultBGColor, 99, y, "    ");
    sprintf(text, "%d%%", t);
    ReefAngel.LCD.DrawText(COLOR_ROYALBLUE, DefaultBGColor, 99, y, text);
    // Water level is truncated to 50 pixels instead of 100
    // Every 2% equals 1 pixel
    // divide water level by 2
    t = t / 2;
    byte b;
    // draw the image to the left of the center line, filled bar background
    for(b = 0; b < t; b++)
    {
        // swidth,sheight, X, Y, img
        ReefAngel.LCD.DrawImage(1, 6, b + WL_BAR_OFFSET, y + 2, BAR_WL_LEFT);
    }
    // draw the center image, indicator
    ReefAngel.LCD.DrawImage(2, 6, t + WL_BAR_OFFSET, y + 2, BAR_CENTER);
    // draw the image to the right of the center line, unfilled bar background
    for(b = t; b < 50; b++)
    {
        ReefAngel.LCD.DrawImage(1, 6, b + WL_BAR_OFFSET + 2, y + 2, BAR_WL_RIGHT);
    }

    // Draw DC Pump Mode (next line)
    y = 88;
    x = 4;
    if (fNight) {
      t = DCPumpNightMode_read();
    } else {
      t = InternalMemory.DCPumpMode_read();
    }
    // copy the string from the global array into a buffer of 11 chars for display on screen
    PUMP_MODES[t].toCharArray(text, 11);
    // NOTE text is max of 11 chars, 11*5=55 pixels
    ReefAngel.LCD.Clear(DefaultBGColor, x, y, x+55, y+8);
    ReefAngel.LCD.DrawText(COLOR_BLACK, DefaultBGColor, x, y, text);

    // Draw Day/Night schedule (same line as pump mode)
    // NOTE Center text in available space between column 72 and 128
    if (fNight) {
      strcpy(text, "Night");
      b = COLOR_BLACK;
      t = COLOR_WHITE;
      x = 82;
    } else {
      strcpy(text, "Day");
      b = DefaultBGColor;
      t = COLOR_BLACK;
      x = 90;
    }
    // clear entire space with the background color
    // clear bigger space than just for the text to line up
    // the bottom of the square with the dc pump speed bar
    ReefAngel.LCD.Clear(b, 74, y, 128, y+18);
    ReefAngel.LCD.DrawLargeText(t, b, x, y+1, text, Font8x16);

    // Draw DC Pump Speed bar
    y = 98;
    x = 58;
#ifdef __TEST__
    t = 65;
#else
    t = ReefAngel.PWM.GetDaylightValue();
#endif  // __TEST__
    ReefAngel.LCD.Clear(DefaultBGColor, x, y, x+15, y+8);
    ReefAngel.LCD.DrawText(COLOR_BLACK, DefaultBGColor, x, y, t);
    // scale bar to be half width, divide value by 2
    t = t / 2;
    for(b = 0; b < t; b++)
    {
        ReefAngel.LCD.DrawImage(1, 6, b + DC_BAR_OFFSET, y + 2, BAR_DP_LEFT);
    }
    ReefAngel.LCD.DrawImage(2, 6, t + DC_BAR_OFFSET, y + 2, BAR_CENTER);
    for(b = t; b < 50; b++)
    {
        ReefAngel.LCD.DrawImage(1, 6, b + DC_BAR_OFFSET + 2, y + 2, BAR_DP_RIGHT);
    }

    // Draw outlet bar
    t = ReefAngel.Relay.RelayData;
    t &= ReefAngel.Relay.RelayMaskOff;
    t |= ReefAngel.Relay.RelayMaskOn;
    ReefAngel.LCD.DrawOutletBox(12, 112, t);

    pingSerial();
}

void DrawCustomGraph()
{
}

boolean isNightTime()
{
    boolean fResult = false;
    byte bOffHour = InternalMemory.StdLightsOffHour_read();
    byte bOffMinute = InternalMemory.StdLightsOffMinute_read();
    byte bOnHour = InternalMemory.StdLightsOnHour_read();
    byte bOnMinute = InternalMemory.StdLightsOnMinute_read();

    /*
    If current hour is off hour and current minute is equal or after off minute, night time is true
    if current hour is after off hour, night time is true
    (beyond midnight, aka early hours in morning, overnight wrap)
    if current hour is before on hour, night time is true
    if current hour is on hour and current minute is before off minute, night time is true
    */
    if ( ((hour() == bOffHour) && (minute() >= bOffMinute)) ||
         (hour() > bOffHour) ||
         (hour() < bOnHour) ||
         ((hour() == bOnHour) && (minute() < bOnMinute)) )
    {
        fResult = true;
    }

    return fResult;
}

void checkCustomMemoryValues()
{
    if ( DCPumpNightMode_read() > 14 )
    {
        // Default to Constant Mode
        CustomWrite(DCPUMP_NIGHT_MODE, 0);
    }
    if ( DCPumpNightSpeed_read() > 100 )
    {
        // Default to 0 speed
        CustomWrite(DCPUMP_NIGHT_SPEED, 0);
    }
    if ( DCPumpNightDuration_read() > 100 )
    {
        // Default to 0 duration
        CustomWrite(DCPUMP_NIGHT_DURATION, 0);
    }
    if ( DCPumpNightThreshold_read() > 100 )
    {
        // Default to 30 threshold
        CustomWrite(DCPUMP_NIGHT_THRESHOLD, 30);
    }
}

uint8_t DCPumpNightMode_read()
{
    return CustomRead(DCPUMP_NIGHT_MODE);
}

uint8_t DCPumpNightSpeed_read()
{
    return CustomRead(DCPUMP_NIGHT_SPEED);
}

uint8_t DCPumpNightDuration_read()
{
    return CustomRead(DCPUMP_NIGHT_DURATION);
}

uint8_t DCPumpNightThreshold_read()
{
    return CustomRead(DCPUMP_NIGHT_THRESHOLD);
}

uint8_t CustomRead(int address)
{
#if not defined __SAM3X8E__
    return eeprom_read_byte((unsigned char *) address);
#else
    return SPIEEPROM.Read(address);
#endif
}

void CustomWrite(int address, const uint8_t value)
{
    if (CustomRead(address) != value )
    {
#if not defined __SAM3X8E__
        eeprom_write_byte((unsigned char *) address, value);
#else
        SPIEEPROM.Write(address, value);
#endif
    }
}

// RA_STRING1=null
// RA_STRING2=null
// RA_STRING3=null
// RA_LABEL LABEL_ACTINIC=Actinic
// RA_LABEL LABEL_DAYLIGHT=Daylight
// RA_LABEL LABEL_PORT1=Port 1
// RA_LABEL LABEL_PORT2=Port 2
// RA_LABEL LABEL_PORT21=Port 21
// RA_LABEL LABEL_PORT22=Port 22
// RA_LABEL LABEL_PORT23=Port 23
// RA_LABEL LABEL_PORT24=Port 24
// RA_LABEL LABEL_PORT25=Port 25
// RA_LABEL LABEL_PORT26=Port 26
// RA_LABEL LABEL_PORT27=Port 27
// RA_LABEL LABEL_PORT28=Port 28
// RA_LABEL LABEL_PORT3=Port 3
// RA_LABEL LABEL_PORT4=Port 4
// RA_LABEL LABEL_PORT5=Light1
// RA_LABEL LABEL_PORT6=Light2
// RA_LABEL LABEL_PORT7=Light3
// RA_LABEL LABEL_PORT8=Light4
