# ESP32-C3 tabanlı HomeKit Akıllı Duvardan Anahtar (Smart Switch) 🚀

Bu proje; Apple HomeKit ekosistemiyle doğrudan (yerel ağ üzerinden, herhangi bir köprü/bridge cihazına ihtiyaç duymadan) haberleşen, yüksek güvenlikli ve izoleli bir akıllı ev anahtarı projesidir. Mevcut duvar içi anahtar kasalarına sığacak şekilde tasarlanmıştır.

## 🛠️ Donanım Mimarisi ve Bileşenler

Devre tasarımı, yüksek voltaj (220V AC) ile düşük voltaj (3.3V DC) lojik katmanlarının tamamen birbirinden izole edilmesi (Galvanik İzolasyon) prensibiyle geliştirilmiştir.

* **Mikrodenetleyici:** Wemos ESP32-C3 Mini (Apple Silicon M5 üzerinde tam performanslı derleme)
* **Güç Anahtarlama (Power Triac):** BTA16-600B ($16\text{A}$ yük kapasitesi, yüksek akım dayanımı)
* **Sürücü ve İzolasyon (Opto-Triac):** MOC3041
  * **Galvanik İzolasyon:** 220V şebeke gürültüsünün lojik katmana geçmesini engeller.
  * **Sıfır Geçiş Kontrolü (Zero-Crossing Detection):** AC sinüs dalgasının tam $0\text{V}$ olduğu anı yakalayarak anahtarlama yapar. Bu sayede Triac'ın ısınmasını azaltır, lamba ömrünü uzatır ve şebeke parazitlerini minimuma indirir.
* **Koruma Katmanları:**
  * **NTC Termistör:** Cihaz ilk açıldığında boş filtre kapasitörlerinin yarattığı ani akım sıçramasını (**Inrush Current**) dizginleyen "Soft Start" koruması.
  * **Varistör (MOV) & Sigorta:** Yüksek gerilim piklerine ve kısa devrelere karşı tam koruma.

## 💻 Yazılım Özellikleri ve Lojik Temelleri

* **Yerel HomeKit Entegrasyonu:** `arduino-homekit-esp8266` yerel C-SDK kütüphanesi kullanılarak Siri ve Apple Ev (Home) uygulaması entegrasyonu.
* **Non-blocking Debounce Lojiği:** Duvardaki fiziksel mekanik anahtarın (toggle) durum değişiklikleri, `delay()` gibi işlemciyi kilitleyen fonksiyonlar yerine `millis()` zamanlayıcısı kullanılarak okunmuştur. Böylece HomeKit arka plan trafiği kesintiye uğramaz.
* **Otomatik Yeniden Bağlanma (Fail-Safe):** `WiFi.setAutoReconnect(true)` lojiği sayesinde internet veya modem bağlantısı kopsa dahi cihaz arka planda sürekli ağı tarar; internet geri geldiğinde otomatik bağlanır. Bağlantı yokken bile fiziksel anahtar lambayı kontrol etmeye kesintisiz devam eder.
* **Gelişmiş İzleme Hazırlığı:** İleride eklenebilecek enerji izleme (HLW8012/BL0937) özellikleri için **Eve Home** uygulamasının özel UUID altyapısıyla uyumlu karakteristik mimarisi düşünülmüştür.

## 📂 Proje Yapısı

* `Smart_switch.ino`: Wi-Fi, donanım pin yönetimi ve debounce algoritmasını içeren ana C++ yürütücü kodu.
* `my_accessory.c`: Apple HomeKit protokolüne cihazın bir "Lightbulb" (Lamba) olduğunu bildiren saf C tabanlı kimlik ve karakteristik tanımlama dosyası.

## 🚀 Kurulum ve Derleme

1. Arduino IDE 2.0+ (Apple Silicon sürümü) kurun.
2. `arduino-homekit-esp8266` kütüphanesini IDE'ye dahil edin.
3. `Smart_switch.ino` içerisindeki `ssid` ve `password` alanlarına kendi Wi-Fi bilgilerinizi girin.
4. Kart olarak ESP32-C3 seçerek yüklemeyi tamamlayın.
5. Apple Ev uygulamasından `my_accessory.c` içinde tanımlı olan **`466-37-726`** kurulum koduyla cihazı eşleştirin.

---
*Geliştirici: Berkay - DEU Elektrik-Elektronik Mühendisliği*
