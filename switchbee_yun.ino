#include <Bridge.h>
#include <Process.h>

const int switchPins[] = {2, 3, 4, 5};

//my home bridge IP...change this to work wherever you are
String hueBridgeIP = "192.168.1.202";
String apiUser = "coreymenscher";
String apiPath = "/api/" + apiUser + "/lights/";


// Hue API id's for the lights to be controlled
const int tylerLightIds[] = {2, 3};
const int laylalampId = 1;
const int floorLampId = 5; //there is no 4 (?)

bool pinStates[] = {false, false, false, false};
bool lastPinStates[] = {false, false, false, false};

void setup() {
  Serial.begin(9600);
  Bridge.begin();
//  Console.begin();
}

void readSwitch(int readPinIdx) {
  pinStates[readPinIdx] = digitalRead(switchPins[readPinIdx]);
//  Serial.print(switchPins[readPinIdx]);
//  Serial.print(": ");
//  Serial.print(pinStates[readPinIdx]);
//  Serial.println(" ");

  // if a switch was hit, and the last time it was read it was not on, send
  // the command to the Hue bridge to set the appropriate lights
  if(pinStates[readPinIdx] == 1 && lastPinStates[readPinIdx] != pinStates[readPinIdx]) {
    Serial.print("Switch ");
    Serial.print(readPinIdx+1);
    Serial.println(" was hit!");
    
    handleSwitch(readPinIdx);
  }
}

void handleSwitch(int readPinIdx) {
  Process proc;
  proc.begin("curl");  

  /*****************************************************************************************
  NOTE: IT IS EXTREMELY IMPORTANT TO INCLUDE THE "Content-Type: application/json" HEADER!
  Also, you must use the -d option and **single quote it** so the JSON has double quotes when
  transmitted to the Bridge.  (Notice that I am escaping the quotes in the "payload"
  variable below
  *****************************************************************************************/

  // This is an example of a valid curl command... 
  //curl -H "Content-Type: application/json" -X PUT -d '{"on": false}' http://192.168.1.202/api/coreymenscher/lights/5/state

  String apiUrl = "http://" + hueBridgeIP + apiPath + floorLampId + "/state";
  String payload = "";

  switch(readPinIdx) {
    //Off
    case 0:
      payload = "{\"on\": false}";
      break;
    
    //White
    case 1:
      payload = "{\"on\": true, \"hue\": 0, \"sat\": 0}";
      break;
    
    //Random color
    case 3:
      break;
    
    //Bedtime
    case 4:
      payload = "{\"on\": true, \"hue\": 46920, \"sat\": 254, \"bri\": 1}";
      break;
  }
  
//  Serial.print("API URL: ");
//  Serial.println(apiUrl);
//  Serial.print("Sending payload: ");
//  Serial.println(payload);
//  proc.addParameter("-H");
//  proc.addParameter("Content-Type: application/json");
//  proc.addParameter("-X");
//  proc.addParameter("PUT");
//  proc.addParameter("-d");
//  proc.addParameter(payload);
//  proc.addParameter(apiUrl);
//  proc.run();

  String cmd="curl -H \"Content-Type: application/json\" -X PUT -d '"+payload+"' "+apiUrl;
  Serial.println(cmd);
  proc.runShellCommandAsynchronously(cmd);
}

void loop() {
    
  // Get the state of each switch pin and set it to pinStates array
  for (int i = 0; i < 4; i++) {
    //save the last set of pin states
    lastPinStates[i] = pinStates[i];
    
    //read the switch
    readSwitch(i);
    delay(1);
  }
}

