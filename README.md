# 🍃 Deneyap Kart ile Akıllı Oda Havası Kalite Monitörü

<p align="center">
  <a href="#donanim-ve-baglantilar"><img src="https://img.shields.io/badge/Mikrodenetleyici-Deneyap%20Kart%20ESP32-1e293b?style=flat-square&logo=espressif&logoColor=white" alt="Donanım" /></a>
  <a href="#donanim-ve-baglantilar"><img src="https://img.shields.io/badge/Sensör-SHT4x%20Sıcaklık%20%26%20Nem-334155?style=flat-square" alt="SHT4x" /></a>
  <a href="#donanim-ve-baglantilar"><img src="https://img.shields.io/badge/Sensör-Analog%20Duman%20%26%20Gaz-334155?style=flat-square" alt="Gaz Sensörü" /></a>
  <a href="#donanim-ve-baglantilar"><img src="https://img.shields.io/badge/Ekran-0.96%22%20OLED%20%2B%205x7%20LED%20Matris-334155?style=flat-square" alt="Ekranlar" /></a>
  <a href="#iletisim-protokolu"><img src="https://img.shields.io/badge/Haberleşme-I2C%20Zincirleme%20(Daisy--Chain)-475569?style=flat-square" alt="I2C" /></a>
  <a href="#lisans"><img src="https://img.shields.io/badge/Lisans-MIT-10b981?style=flat-square" alt="Lisans" /></a>
</p>

> **İç Ortam Hava Güvenliği ve Konfor Takip Sistemi:** Deneyap Kart (ESP32) üzerinde çalışan; ortam sıcaklığını (°C), bağıl nemi (%) ve havadaki yanıcı gaz / duman yoğunluğunu (ppm) eşzamanlı ölçerek 0.96" OLED ekranda anlık raporlayan, kritik eşik aşımında 5x7 LED matris üzerinden çarpı (X) uyarısı, normal koşullarda ise gülen yüz (😊) ifadesi sunan akıllı IoT çevre izleme istasyonu.

---

<a id="icindekiler"></a>
## 📑 İçindekiler

1. [Proje Hakkında ve Problem Tanımı](#proje-hakkinda)
2. [Donanım Mimarisi ve I2C Bağlantı Tablosu](#donanim-ve-baglantilar)
3. [Hava Kalitesi Eşik Analizi ve Değerlendirme Kriterleri](#esik-analizi)
4. [LED Matris Görsel Geri Bildirim Haritası (5x7)](#led-matris-haritasi)
5. [Sistem Çalışma Mantığı ve Algoritma Akışı](#calisma-mantigi)
6. [Örnek Seri Port (UART) Terminal Çıktısı](#seri-port-ciktisi)
7. [Proje Dizin Yapısı](#dizin-yapisi)
8. [Kurulum ve Çalıştırma Rehberi](#kurulum-ve-calistirma)
9. [Geliştirici](#gelistirici)
10. [Lisans](#lisans)

---

<a id="proje-hakkinda"></a>
## 🎯 1. Proje Hakkında ve Problem Tanımı

Kapalı çalışma odalarında, sınıflarda veya laboratuvarlarda yetersiz havalandırma; yüksek nem, aşırı sıcaklık ve fark edilmeyen zararlı gaz birikimine yol açarak baş ağrısına, odak kaybına ve güvenlik zaafiyetlerine neden olur.

Bu proje:
- **SHT4x sensörü** ile yüksek hassasiyetli sıcaklık ve bağıl nem ölçümü yapar,
- **Analog duman dedektörü** ile havadaki yabancı gaz parçacıklarını anlık tarar,
- Tüm bu metrikleri **her 3 saniyede bir** değerlendirerek kullanıcıya hem sayısal (OLED) hem de hızlı algılanabilir piktografik (LED Matris) görsel ikaz üretir.

---

<a id="donanim-ve-baglantilar"></a>
## 🔌 2. Donanım Mimarisi ve I2C Bağlantı Tablosu

Tüm sensör ve ekran modülleri, Deneyap Kart'ın standart I2C haberleşme veri yoluna (**SDA / SCL**) tek bir hat üzerinden zincirleme (daisy-chain) olarak bağlanmıştır:

| Donanım Modülü | Entegre / Çip | I2C Adresi | Görev ve Ölçüm Aralığı |
| :--- | :--- | :---: | :--- |
| **Deneyap Kart** | ESP32-WROOM-32 | *Master* | Veri toplama, eşik kıyaslama ve ekran kontrolü |
| **Deneyap Sıcaklık, Nem ve Basınç** | Sensirion SHT4x | `0x44` | Sıcaklık (°C) ve Bağıl Nem (%) ölçümü |
| **Deneyap Duman Dedektörü** | Analog Gaz Sensörü | `0x20` | Duman ve yanıcı gaz konsantrasyonu (ppm) |
| **Deneyap OLED Ekran** | 0.96" SSD1306 (128x64) | `0x7A` | Ayrıntılı sayısal değerler ve durum metni |
| **Deneyap 5x7 LED Ekran** | Dot Matrix Sürücü | `0x0A` | Piktogram animasyonu (Gülen yüz / Tehlike 'X') |

---

<a id="esik-analizi"></a>
## ⚖️ 3. Hava Kalitesi Eşik Analizi ve Değerlendirme Kriterleri

Yazılım içerisindeki mantıksal karar motoru (`havaKotu`) aşağıdaki güvenlik eşiklerini referans alır:

| Parametre | Kritik Eşik Değeri | Durum | Sistem Tepkisi |
| :--- | :---: | :---: | :--- |
| **Gaz Yoğunluğu** | `> 200 ppm` | ⚠️ Tehlike | Alarm durumu tetiklenir |
| **Sıcaklık** | `> 30.0 °C` | ⚠️ Aşırı Sıcak | Alarm durumu tetiklenir |
| **Bağıl Nem** | `> 80.0 %` | ⚠️ Boğucu Nem | Alarm durumu tetiklenir |
| **Normal Aralık** | Tüm değerler eşik altı | ✅ İdeal Hava | İdeal yaşam konforu modu |

```cpp
bool havaKotu = (gazDegeri > gazEsigi || sicaklik > sicaklikEsigi || nem > nemEsigi);
```

---

<a id="led-matris-haritasi"></a>
## 💡 4. LED Matris Görsel Geri Bildirim Haritası (5x7)

5 sütun x 7 satırdan oluşan (35 LED) matris ekran üzerinde kullanıcıya iki temel durum ikonu çizdirilir:

### A. Normal / İdeal Hava (Gülen Yüz 😊)
```text
[0,0,0,0,0]  . . . . .
[0,0,0,0,0]  . . . . .
[0,1,0,1,0]  . # . # .  <- Gözler
[0,0,0,0,0]  . . . . .
[1,0,0,0,1]  # . . . #  <- Ağız kenarları
[0,1,1,1,0]  . # # # .  <- Gülümseme tabanı
[0,0,0,0,0]  . . . . .
OLED Ekran Mesajı: "Hava Durumu: Iyi"
```

### B. Kötü / Tehlikeli Hava (Uyarı Çarpısı ❌)
```text
[1,0,0,0,1]  # . . . #
[0,1,0,1,0]  . # . # .
[0,0,1,0,0]  . . # . .  <- Kesişim noktası
[0,1,0,1,0]  . # . # .
[1,0,0,0,1]  # . . . #
[0,0,0,0,0]  . . . . .
[0,0,0,0,0]  . . . . .
OLED Ekran Mesajı: "HAVA KALITESI KOTU!"
```

---

<a id="calisma-mantigi"></a>
## 🔄 5. Sistem Çalışma Mantığı ve Algoritma Akışı

```
                 +-----------------------------------+
                 |       setup(): I2C Başlatma       |
                 |  Gaz(0x20), SHT4x(0x44),          |
                 |  OLED(0x7A), LED Matris(0x0A)     |
                 +-----------------------------------+
                                   |
                                   v
                 +-----------------------------------+
                 |      OLED Başlangıç Ekranı:       |
                 |    "Hava Kalitesi Analiz..."      |
                 +-----------------------------------+
                                   |
                                   v
                 +-----------------------------------+
          +----->|      loop(): Sensörleri Oku       |<-----+
          |      |  - SHT4x: TtoDegC() & RHtoPercent |      |
          |      |  - Gaz: ReadSmokeAnalog()         |      |
          |      +-----------------------------------+      |
          |                        |                        |
          |                        v                        |
          |      +-----------------------------------+      |
          |      |       OLED Sayısal Güncelleme     |      |
          |      |   Sicaklik (C), Nem (%), Gaz(ppm) |      |
          |      +-----------------------------------+      |
          |                        |                        |
          |                        v                        |
          |      +-----------------------------------+      |
          |      |      Eşik Aşımı Var mı? (OR)      |      |
          |      |   Gaz>200 || Sicaklik>30 || Nem>80|      |
          |      +-----------------------------------+      |
          |               /                     \           |
          |            EVET                    HAYIR        |
          |             /                         \         |
          |   [LED Matris: 'X' İkonu]    [LED Matris: Gülen Yüz]|
          |   [OLED: "HAVA KÖTÜ!"]       [OLED: "Hava İyi"]    |
          |             \                         /         |
          |              +-----------+-----------+          |
          |                          |                      |
          |                          v                      |
          |                 [delay(3000): 3 Saniye]         |
          |                          |                      |
          +--------------------------+----------------------+
```

---

<a id="seri-port-ciktisi"></a>
## 🖥️ 6. Örnek Seri Port (UART) Terminal Çıktısı

Deneyap Kart bilgisayara bağlıyken `115200 baud` hızında seri monitör üzerinden anlık ölçümleri gözlemleyebilirsiniz:

```text
Sicaklik: 24.3 °C, Nem: 42.1 %, Gaz: 118
Sicaklik: 24.5 °C, Nem: 42.8 %, Gaz: 122
Sicaklik: 25.1 °C, Nem: 44.0 %, Gaz: 130
Sicaklik: 31.2 °C, Nem: 83.5 %, Gaz: 245  <-- Eşik Aşımı (Alarm Devrede)
```

---

<a id="dizin-yapisi"></a>
## 📁 7. Proje Dizin Yapısı

```
oda_havasi_kalite_monitoru/
├── README.md                          # Proje teknik dokümantasyonu
└── oda_havasi_kalite_monitoru/        # Arduino / Deneyap kaynak kod klasörü
    └── oda_havasi_kalite_monitoru.ino # Ana mikrodenetleyici C++ yazılımı
```

---

<a id="kurulum-ve-calistirma"></a>
## 🚀 8. Kurulum ve Çalıştırma Rehberi

### 1. Gereksinimler
- [Arduino IDE](https://www.arduino.cc/en/software) (v1.8.19 veya v2.x)
- **Deneyap Kart Kart Paketi:** `Araçlar > Kart > Kart Yöneticisi` kısmından *Deneyap Geliştirme Kartları* paketini yükleyin.

### 2. Gerekli Kütüphaneler
Arduino IDE'de **Taslak > Kütüphane Ekle > Kütüphaneleri Yönet...** bölümünden aşağıdaki Deneyap kütüphanelerini kurun:
- `Deneyap_DumanDedektoru`
- `Deneyap_SicaklikNemBasincOlcer`
- `Deneyap_OLED`
- `Deneyap_5x7LedEkran`

### 3. Karta Yükleme
1. Depoyu klonlayın:
   ```bash
   git clone https://github.com/kvsrustun/oda_havasi_kalite_monitoru.git
   ```
2. `oda_havasi_kalite_monitoru/oda_havasi_kalite_monitoru.ino` dosyasını Arduino IDE ile açın.
3. **Araçlar > Kart > Deneyap Kart** seçimini yapın.
4. Modülleri I2C bağlantı tablosuna göre Deneyap Kart'a takın.
5. Kodu derleyip karta yükleyin.

---

<a id="gelistirici"></a>
## 👩‍💻 9. Geliştirici

**Kevser ÜSTÜN**  
🎓 Giresun Üniversitesi — Bilgisayar Mühendisliği  
🔗 [GitHub Profilim](https://github.com/kvsrustun)

---

<a id="lisans"></a>
## 📄 10. Lisans

Bu proje [MIT Lisansı](LICENSE) kapsamında açık kaynak olarak sunulmuştur.
