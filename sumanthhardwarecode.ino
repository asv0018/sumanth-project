#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include<ArduinoJson.h>

#include <SoftwareSerial.h>

SoftwareSerial mySerial(D5,D6); // RX, TX

int people_count = 0;

#define ENTRY D0
#define EXIT D1
String code = "";


#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "iotree minds LLP"
#define WIFI_PASSWORD "87554321"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyC9MOLiQeQ5vACpjvkOoIEaTiWDPpgfQ4A"

/* 3. Define the RTDB URL */
#define DATABASE_URL "smart-library-26891-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

int icount = 0;

void setup()
{

  Serial.begin(115200);
  pinMode(ENTRY,INPUT);
  pinMode(EXIT,INPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  mySerial.begin(9600);
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = "shreevallabhas@gmail.com";
  auth.user.password = "1111111111";

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  
/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "smart-library-26891"


  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Or use legacy authenticate method
  // config.database_url = DATABASE_URL;
  // config.signer.tokens.legacy_token = "<database secret>";

  // To connect without auth in Test Mode, see Authentications/TestMode/TestMode.ino

  //////////////////////////////////////////////////////////////////////////////////////////////
  // Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  // otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

#if defined(ESP8266)
  // In ESP8266 required for BearSSL rx/tx buffer for large data handle, increase Rx size as needed.
  fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 2048 /* Tx buffer size in bytes from 512 - 16384 */);
#endif

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;
  
  if (Firebase.ready()){
    sendDataPrevMillis = millis();
    Serial.printf("Get int ref... %s\n", Firebase.RTDB.getInt(&fbdo, F("/libraryattendance/count"), &people_count) ? String(people_count).c_str() : fbdo.errorReason().c_str());
  }

}


void loop()
{
  if (mySerial.available()) {
    //Serial.write(mySerial.read());
    code = mySerial.readString();
    code.trim();
    Serial.println(code);
     if (Firebase.ready()){
        String documentPath = "books-data/"+code;
        String mask = "Singapore";
        // If the document path contains space e.g. "a b c/d e f"
        // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"
        Serial.print("Get a document... ");
        if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str())){
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
            FirebaseJson root = fbdo.payload();
            //StaticJsonBuffer<200> jsonBuffer;
            //JsonObject &root = jsonBuffer.parseObject(fbdo.payload().c_str());
             // Create a FirebaseJson object and set content with received payload
            FirebaseJson payload;
            payload.setJsonData(fbdo.payload().c_str());
        
            // Get the data from FirebaseJson object 
            FirebaseJsonData issuedTo;
            payload.get(issuedTo, "fields/issuedto/stringValue", true);
            Serial.println(issuedTo.stringValue);
            
            // Get the data from FirebaseJson object 
            FirebaseJsonData isAvailable;
            payload.get(isAvailable, "fields/isavailable/booleanValue", true);
            Serial.println(isAvailable.boolValue);
            Serial.println("Done");
            if(isAvailable.boolValue==false){
              FirebaseJson content;
              content.clear();
              content.set("fields/isavailable/booleanValue", true);
              content.set("fields/issuedto/stringValue", String("").c_str());
              Serial.println("The return process is going to start now");
              if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), "isavailable,issuedto" /* updateMask */))
                  Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
              else
                  Serial.println(fbdo.errorReason());
            }else{
              Serial.println("The book is not taken by anyone!!!"); 
            }
        }else{
            Serial.println(fbdo.errorReason());
        }
    }
  }

  // put your main code here, to run repeatedly:
  int entry_value = digitalRead(ENTRY);
  int exit_value = digitalRead(EXIT);
  if (entry_value==0){
    while(entry_value==0){
      entry_value = digitalRead(ENTRY);
    }
    people_count++;
    if (Firebase.ready()){
      Serial.printf("Set int... %s\n", Firebase.RTDB.setInt(&fbdo, F("/libraryattendance/count"), people_count) ? "ok" : fbdo.errorReason().c_str());
    }
  }
  
  if (exit_value==0){
    while(exit_value==0){
      exit_value = digitalRead(EXIT);
    }
    if(people_count>0){
      people_count--;
      if (Firebase.ready()){
        Serial.printf("Set int... %s\n", Firebase.RTDB.setInt(&fbdo, F("/libraryattendance/count"), people_count) ? "ok" : fbdo.errorReason().c_str());
      }
    }
  }
}
