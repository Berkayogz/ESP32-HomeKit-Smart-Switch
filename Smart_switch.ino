#include <Arduino.h>
#include <arduino_homekit_server.h>
#include <ESP8266WiFi.h> // Not: ESP32-C3 HomeKit kütüphanen hangisini içeriyorsa

// Donanım Pin Tanımlamaları
#define LAMP_PIN 4    // Triac'ı tetikleyen pin (D4)
#define SWITCH_PIN 5  // Duvardaki fiziksel anahtar pini (D5)

// Debounce (Kararlılık) Değişkenleri
int lastSwitchState = -1;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // 50 milisaniye güvenlik barajı

// my_accessory.c dosyasından gelen harici HomeKit karakteristiği
extern "C" homekit_characteristic_t cha_on;

// Wi-Fi Bilgileri
const char* ssid = "WIFI_AGINIZIN_ADI";
const char* password = "WIFI_SIFRENIZ";

// Lamba durumunu değiştiren fonksiyon (Hem Siri hem fiziksel anahtar çağırır)
void set_lamp(bool on) {
  digitalWrite(LAMP_PIN, on ? HIGH : LOW);
  Serial.print(">>> LAMBA DURUMU DEGISYOR: ");
  Serial.println(on ? "ACIK" : "KAPALI");
}

// HomeKit'ten (Siri/iPhone) gelen komutları yakalayan fonksiyon
void my_accessory_identify(homekit_value_t _value) {
  Serial.println("Aksesuar Kimliği Doğrulandı.");
}

void setter_cha_on(const homekit_value_t value) {
  bool on = value.bool_value;
  cha_on.value.bool_value = on; // Hafızadaki durumu güncelle
  set_lamp(on);                 // Röleyi/Triac'ı tetikle
}

void wifi_connect() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true); // İnternet gelince otomatik bağlanma emri
  WiFi.begin(ssid, password);
  Serial.print("Wi-Fi Baglantisi Kuruluyor...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Baglandi!");
  Serial.print("IP Adresi: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LAMP_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP); // Dahili ve harici Pull-Up aktif

  wifi_connect();

  // Açılışta anahtarın mevcut konumunu kaydet (Kendi kendine tetiklenmeyi önler)
  lastSwitchState = digitalRead(SWITCH_PIN);

  // HomeKit Kurulumları
  cha_on.setter = setter_cha_on; 
  arduino_homekit_setup_server();
  
  Serial.println("--- SIKINTISIZ DONANIM VE HOMEKIT AKTIF ---");
}

void loop() {
  ESP.wdtFeed(); // Watchdog resetini engelle
  arduino_homekit_loop();

  // 1. Pini anlık olarak oku
  int reading = digitalRead(SWITCH_PIN);

  // 2. Hareket Algılama (Edge Detection)
  static int lastReading = -1;
  if (reading != lastReading) {
    lastDebounceTime = millis(); // Sayacı SADECE değişim anında başlat
    lastReading = reading;
  }

  // 3. Kararlılık Kontrolü (Debounce Zamanlaması)
  if ((millis() - lastDebounceTime) > debounceDelay) {
    
    // Eğer gürültü değilse ve durum gerçekten kalıcı olarak değiştiyse
    if (reading != lastSwitchState) {
      lastSwitchState = reading; // Yeni kararlı durumu onayla
      
      // HomeKit durumunu TERSİNE çevir (Toggle mantığı)
      bool new_state = !cha_on.value.bool_value;
      cha_on.value.bool_value = new_state;
      
      // Fiziksel lambayı sür
      set_lamp(new_state);
      
      // iPhone / Apple Ev uygulamasına yeni durumu fırlat (Ekran anında güncellenir)
      homekit_characteristic_notify(&cha_on, cha_on.value);
      
      Serial.println(">>> FIZIKSEL ANAHTAR TETIKLENDI! <<<");
    }
  }
  yield();
}