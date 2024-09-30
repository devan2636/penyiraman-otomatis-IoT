#include <string.h>
// Device libraries (Arduino ESP32/ESP8266 Cores)
#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
//#else // jika menggunakan ESP32
#include <WiFi.h>
#endif
// Custom libraries
#include <utlgbotlib.h>
#define WIFI_SSID "Astronaut"
#define WIFI_PASS "Astronaut26"
#define MAX_CONN_FAIL 50
#define MAX_LENGTH_WIFI_SSID 31
#define MAX_LENGTH_WIFI_PASS 63
#define TLG_TOKEN "1158974916:AAEJYd2niUHUF-KSvR97Vmy5ZKzim_beQ4M" // token latihan_bot
#define DEBUG_LEVEL_UTLGBOT 0
#define PIN_Lampu 5 
#define PIN_Pompa 6
#define pintriger 4
#define pinecho 2

const char TEXT_START[] =
  " PENYIRAMAN OTOMATIS DENGAN SITEM IoT \n"
  "\n"
  "Check /help command to see how to use me."
  "Salam Dede Yusup";

char buff[100];
//boolean lampu_status;
//boolean pompa_status;

const char TEXT_HELP[] =
  "Available Commands:\n"
  "\n"
  "/start - Show start text.\n"
  "/help - Show actual text.\n"
  "/lampu_on - Turn on the Lampu.\n"
  "/lampu_off - Turn off the Lampu.\n"
  "/pompa_on - Turn on the Pompa.\n"
  "/pompa_off - Turn off the Pompa.\n"
  "/lampustatus - Show actual Lampu status.\n"
  "/pompastatus - Show actual Pompa status.\n"
  "/status_air - Show actual Ketinggian Air status.\n"
  "/status_kelembaban - Show actual Kelembaban Tanah status.\n";

void wifi_init_stat(void);
bool wifi_handle_connection(void);


// Create Bot object
uTLGBot Bot(TLG_TOKEN);

//Pompa dan Lampu status
uint8_t lampu_status;
uint8_t pompa_status;


void setup()
{
  Serial.begin(115200);
  Bot.set_debug(DEBUG_LEVEL_UTLGBOT);
  digitalWrite(PIN_Lampu,HIGH);
  pinMode(PIN_Lampu,OUTPUT);
  lampu_status = 1;
  digitalWrite(PIN_Pompa,HIGH);  
  pinMode(PIN_Pompa,OUTPUT); 
  pompa_status = 1;
  pinMode (pintriger,OUTPUT);
  pinMode (pinecho,INPUT);  
  wifi_init_stat();
  Serial.println("Waiting for WiFi connection.");
  while (!wifi_handle_connection())
  {
    Serial.print(".");
    delay(500);
  }
  Bot.getMe();
}

void loop()
{
  if (!wifi_handle_connection())
  {
    // Wait 100ms and check again
    delay(100);
    return;
  }

  // Check for Bot received messages
  while (Bot.getUpdates())
  {
    Serial.println("Received message:");
    Serial.println(Bot.received_msg.text);
    Serial.println(Bot.received_msg.chat.id);

    if (strncmp(Bot.received_msg.text, "/start", strlen("/start")) == 0)
    {
      Bot.sendMessage(Bot.received_msg.chat.id, TEXT_START);
    }

    else if (strncmp(Bot.received_msg.text, "/help", strlen("/help")) == 0)
    {
      Bot.sendMessage(Bot.received_msg.chat.id, TEXT_HELP);
    }
    else if (strncmp(Bot.received_msg.text, "/lampu_on", strlen("/lampu_on")) == 0)
    {
      lampu_status = 0;
      Serial.println("Command /lampu_on received.");
      Serial.println("Turning on the Lampu.");

      Bot.sendMessage(Bot.received_msg.chat.id, "Lampu turned on.");
    }

    else if (strncmp(Bot.received_msg.text, "/lampu_off", strlen("/lampu_off")) == 0)
    {
      lampu_status = 1;
      // Show command reception through Serial
      Serial.println("Command /lampu_off received.");
      Serial.println("Turning off the Lampu.");

      // Send a Telegram message to notify that the Lampu has been turned off
      Bot.sendMessage(Bot.received_msg.chat.id, "Lampu turned off.");
    }
    else if (strncmp(Bot.received_msg.text, "/pompa_on", strlen("/pompa_on")) == 0)
    {
      lampu_status = 0;
      Serial.println("Command /pompa_on received.");
      Serial.println("Turning on the Lampu.");

      Bot.sendMessage(Bot.received_msg.chat.id, "Pompa turned on.");
    }

    else if (strncmp(Bot.received_msg.text, "/pompa_off", strlen("/pompa_off")) == 0)
    {
      pompa_status = 1;
      // Show command reception through Serial
      Serial.println("Command /pompa_off received.");
      Serial.println("Turning off the Pompa.");

      // Send a Telegram message to notify that the Pompa has been turned off
      Bot.sendMessage(Bot.received_msg.chat.id, "Pompa turned off.");
    }

    // If /lampustatus command was received
    else if (strncmp(Bot.received_msg.text, "/lampustatus", strlen("/lampustatus")) == 0)
    {
      // Send a Telegram message to notify actual Lampu status
      if (lampu_status)
        Bot.sendMessage(Bot.received_msg.chat.id, "The Lampu is off.");
      else
        Bot.sendMessage(Bot.received_msg.chat.id, "The Lampu is on.");
    }
    // If /pompastatus command was received
    else if (strncmp(Bot.received_msg.text, "/pompastatus", strlen("/pompastatus")) == 0)
    {
      // Send a Telegram message to notify actual Pompa status
      if (pompa_status)
        Bot.sendMessage(Bot.received_msg.chat.id, "The Pompa is off.");
      else
        Bot.sendMessage(Bot.received_msg.chat.id, "The Pompa is on.");
    }
      // Send a Telegram message to notify actual Status Air
    else if (strncmp(Bot.received_msg.text, "/status_air", strlen("/status_air")) == 0)
    {
      long sisaair;
      long pembacaan;
      long durasi;
      digitalWrite(pintriger, LOW);
      delayMicroseconds(2);
      digitalWrite(pintriger, HIGH);
      delayMicroseconds(2);    
      digitalWrite(pintriger, LOW);  
      durasi=pulseIn( pinecho,HIGH );
      pembacaan = durasi/58.2; //rumus mencari jarak
      sisaair = pembacaan - 30; // mengecek sisa air 30 cm untuk ukuran wadah 30 cm
      Serial.println(sisaair);
      String msg = "Sisa Air :";
      msg += sisaair;
      msg += " cm\n";
      msg.toCharArray(buff, 100);
      Bot.sendMessage(Bot.received_msg.chat.id, buff);
        if (sisaair<4 )// ketika ketinngian air kurang dari 3 cm
          {
            Bot.sendMessage(Bot.received_msg.chat.id, " Segera Isi Cadangan Air ! ") ;
          }
        else
          {
            Bot.sendMessage(Bot.received_msg.chat.id, " Cadangan Air Masih Banyak ") ;
          }
    }
    else if (strncmp(Bot.received_msg.text, "/statuskelembaban", strlen("/status_kelembaban")) == 0)
    {
      int kelembaban = analogRead(A0);
      Serial.println(kelembaban);
      String msg = "Status Kelembaban Tanah :";
      msg += kelembaban;
      msg += "Skala Maksimal 587 \n";
      msg.toCharArray(buff, 100);
      Bot.sendMessage(Bot.received_msg.chat.id, buff);
    }

//    digitalWrite(PIN_RELAY, relay_status);
    // Feed the Watchdog
    yield();
}
}
// Init WiFi interface
void wifi_init_stat(void)
{
  Serial.begin(115200);
  Serial.println("Initializing TCP-IP adapter...");
  Serial.print("Wifi connecting to SSID: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println("TCP-IP adapter successfuly initialized.");
}
bool wifi_handle_connection(void)
{
  static bool wifi_connected = false;

  // Device is not connected
  if (WiFi.status() != WL_CONNECTED)
  {
    // Was connected
    if (wifi_connected)
    {
      Serial.println("WiFi disconnected.");
      wifi_connected = false;
    }

    return false;
  }
  // Device connected
  else
  {
    // Wasn't connected
    if (!wifi_connected)
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());

      wifi_connected = true;
    }
    return true;
  }
}
