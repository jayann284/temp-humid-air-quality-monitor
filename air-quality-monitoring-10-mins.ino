#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6CcY1Ae0v"
#define BLYNK_TEMPLATE_NAME "Temperature"
#define BLYNK_AUTH_TOKEN "V8rXQZ-s3K7WSNyGVVJ43N_NNnKXRqYb"

#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"
#include <DHT.h>

#define DHTPIN D4                                                           // Connect to Pin D4 in ESP8266
#define DHTTYPE DHT11                                                       // select dht type as DHT 11 or DHT22
DHT dht(DHTPIN, DHTTYPE);

int air_quality = A0;

const long duration = 600000; // 10 mins in millisecond
unsigned long previousMillis = 0;
float t = 0;
float h = 0;
float m = 0;
float t_total = 0;
float h_total = 0;
float m_total = 0;
float t_ave = 0;
float h_ave = 0;
float m_ave = 0;
int n_count = 0;

String sheetHumid = "";
String sheetTemp = "";
String sheetairquality="";

const char* ssid = "what the fuck (2.4)";                //replace with our wifi ssid
const char* password = "Alorro123!";         //replace with your wifi password

const char* host = "script.google.com";
const char *GScriptId = "AKfycbwEMPVFXTKP5mm0KbQCVMYJfDGpnRtqcEE_WyNv2Jz4R3hA-0u8w9WgIKr7OqRc6ABDyg"; // Replace with your own google script id
const int httpsPort = 443; //the https port is same

// echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout
const char* fingerprint = "";

//const uint8_t fingerprint[20] = {};

String url = String("/macros/s/") + GScriptId + "/exec?value=Temperature";  // Write Teperature to Google Spreadsheet at cell A1
// Fetch Google Calendar events for 1 week ahead
String url2 = String("/macros/s/") + GScriptId + "/exec?cal";  // Write to Cell A continuosly

//replace with sheet name not with spreadsheet file name taken from google
String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"air_quality_data_sheet\", \
                       \"values\": ";
String payload = "";

HTTPSRedirect* client = nullptr;

// used to store the values of free stack and heap before the HTTPSRedirect object is instantiated
// so that they can be written to Google sheets upon instantiation

void setup() {
  // start blynk connection
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  delay(1000);
  Serial.begin(115200);
  dht.begin();     //initialise DHT11

  // wifi status: not connected
  Blynk.virtualWrite(V3, "Not Connected");

  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // wifi status: connected
  Blynk.virtualWrite(V3, "Connected");
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  Serial.print("Connecting to ");
  Serial.println(host);          //try to connect with "script.google.com"

  // Try to connect for a maximum of 5 times then exit
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
  
  // Finish setup() function in 1s since it will fire watchdog timer and will reset the chip.
  //So avoid too many requests in setup()

  Serial.println("\nWrite into cell 'A1'");
  Serial.println("------>");
  // fetch spreadsheet data
  client->GET(url, host);
  
  Serial.println("\nGET: Fetch Google Calendar Data:");
  Serial.println("------>");
  // fetch spreadsheet data
  client->GET(url2, host);

  Serial.println("\nStart Collecting Sensor Data");

  // delete HTTPSRedirect object
  delete client;
  client = nullptr;
}

void loop() {
  Blynk.run();
  Blynk.virtualWrite(V4, "----------");

  unsigned long currentMillis = millis();
  h = dht.readHumidity();                                              // Reading temperature or humidity takes about 250 milliseconds!
  t = dht.readTemperature();  
  m = analogRead(air_quality);

  t_total += t;
  h_total += h;
  m_total += m;

  // Read temperature as Celsius (the default)
  if (isnan(h) || isnan(t) || isnan(m)) {                                                // Check if any reads failed and exit early (to try again).
    Serial.println(F("Failed to read from DHT and MQ135 sensor!"));
    return;
  }

  // passing data to blynk
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, m);
 
  Serial.println("");
  Serial.print("Humidity: ");  Serial.print(h);
  sheetHumid = String(h) + String(" %");                                         //convert integer humidity to string humidity
  Serial.print("%  Temperature: ");  Serial.print(t);  Serial.println("°C ");
  sheetTemp = String(t) + String(" °C");
  Serial.print("Air Quality: "); Serial.println(m);
  sheetairquality = String(m) + String(" PPM");
  Serial.println(""); 
  
  n_count++; // gets number of recordings
  delay(60000); // records every 1 min
  Serial.print("Current Runtime "); Serial.println( currentMillis);
  static int error_count = 0;
  static int connect_count = 0;
  const unsigned int MAX_CONNECT = 20;
  static bool flag = false;

  if (currentMillis - previousMillis >= duration) {
    Blynk.run();
    Serial.println("");
    Serial.println("10 minutes DONE");
    Serial.println("Getting the AVERAGE");

    t_ave = (t_total / n_count);
    h_ave = (h_total / n_count);
    m_ave = (m_total / n_count);

    Serial.println("");
    Serial.println("Air Quality (Average)");
    Serial.println(m_ave);
    Serial.println("Temperature (Average)");
    Serial.println(t_ave);
    Serial.println("Humidity (Average)");
    Serial.println(h_ave);
    Serial.println("");

    payload = payload_base + "\"" + t_ave + "," + h_ave + "," + m_ave + "\"}";

    if (!flag) {
        client = new HTTPSRedirect(httpsPort);
        client->setInsecure();
        flag = true;
        client->setPrintResponseBody(true);
        client->setContentTypeHeader("application/json");
    }

    if (client != nullptr) {
        if (!client->connected()) {
            client->connect(host, httpsPort);
            client->POST(url2, host, payload, false);
            Serial.print("Sent : ");  Serial.println("Temp, Humid and Air Quality");
        }
    } else {
        DPRINTLN("Error creating client object!");
        error_count = 5;
    }

    if (connect_count > MAX_CONNECT) {
        connect_count = 0;
        flag = false;
        delete client;
        return;
    }

    Serial.println("POST or SEND Sensor data to Google Spreadsheet:");
    if (client->POST(url2, host, payload)) {
        Blynk.virtualWrite(V4, "Success");
    } else {
        ++error_count;
        DPRINT("Error-count while connecting: ");
        DPRINTLN(error_count);
        Blynk.virtualWrite(V4, "Unsuccessful");
    }

    if (error_count > 3) {
        Serial.println("Halting processor...");
        delete client;
        client = nullptr;
        Serial.printf("Final free heap: %u\n", ESP.getFreeHeap());
        Serial.printf("Final stack: %u\n", ESP.getFreeContStack());
        Serial.flush();
        ESP.deepSleep(0);
        Blynk.virtualWrite(V4, "Unsuccessful");
    }
    delay(3000); // keep delay of minimum 2 seconds

    // reset values for the next recording
    t_total = 0;
    h_total = 0;
    m_total = 0;
    n_count = 0;

    // Update previousMillis to the current time to start a new 10-minute interval
    previousMillis = currentMillis; // This line is added
  }
}
