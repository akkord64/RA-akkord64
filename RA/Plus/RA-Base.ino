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
    ReefAngel.Use2014Screen();  // Let's use 2014 Screen 
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
    ReefAngel.TempProbe = T2_PROBE;
    ReefAngel.OverheatProbe = T2_PROBE;
    // Set the Overheat temperature setting
    InternalMemory.OverheatTemp_write( 800 );

    // Feeeding and Water Change mode speed
    ReefAngel.DCPump.FeedingSpeed=0;
    ReefAngel.DCPump.WaterChangeSpeed=0;


    // Ports that are always on
    ReefAngel.Relay.On( Port1 );
    ReefAngel.Relay.On( Port2 );
    //ReefAngel.Relay.On( Port3 );
    //ReefAngel.Relay.On( Port4 );
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
    ReefAngel.StandardFan( T2_PROBE,Port4,770,785 ); // Enable Fan to come on at 78.5 and cool until 77.0
    ReefAngel.StandardHeater( T2_PROBE,Port3,750,770 ); // Enable Heater to kick on at 75 and off at 77.0
    ReefAngel.DCPump.UseMemory = false;
    ReefAngel.DCPump.SetMode( ShortPulse,50,1000 );
    ReefAngel.DCPump.DaylightChannel = None;
    ReefAngel.DCPump.ActinicChannel = None;
    ReefAngel.DCPump.ExpansionChannel[0] = AntiSync;
    ReefAngel.DCPump.ExpansionChannel[1] = Sync;
    ReefAngel.DCPump.ExpansionChannel[2] = AntiSync;
    ReefAngel.DCPump.ExpansionChannel[3] = Sync;
    ReefAngel.DCPump.ExpansionChannel[4] = None;
    ReefAngel.DCPump.ExpansionChannel[5] = None;
    ////// Place your custom code below here
    

    ////// Place your custom code above here

    // This should always be the last line
    ReefAngel.CloudPortal();
    ReefAngel.ShowInterface();
}

