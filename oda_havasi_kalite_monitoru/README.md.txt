# Hava Kalitesi İzleme Sistemi 
Bu proje, Deneyap platformundaki sensör modüllerini kullanarak ortamın hava kalitesini ölçer ve OLED ekranda kullanıcıya bilgi verir. Ayrıca LED matris ekran ile görsel geri bildirim sunar.

## 🚀 Proje Amacı

Bu sistem ile aşağıdaki veriler izlenmektedir:
- Ortam sıcaklığı
- Nem oranı
- Gaz (duman) yoğunluğu

Eşik değerler aşıldığında sistem kullanıcıyı hem yazılı hem görsel olarak uyarır.

## 🧰 Kullanılan Donanımlar

- 🔹 Deneyap Duman Dedektörü 
- 🔹 Deneyap Sıcaklık/Nem/Basınç Ölçer 
- 🔹 Deneyap OLED Ekran 
- 🔹 Deneyap 5x7 LED Matris Ekran 
- 🔹 Deneyap Geliştirme Kartı 

## ⚙️ Bağlantılar

Tüm modüller I2C üzerinden bağlanır. Aşağıdaki I2C adresleri kullanılmaktadır:

| Modül                | I2C Adresi |
|----------------------|------------|
| Duman Dedektörü      |   0x20     |
| Sıcaklık/Nem Sensörü |   0x44     |
| OLED Ekran           |   0x7A     |
| LED Matris           |   0x0A     |

## 📏 Eşik Değerler

| Parametre  | Eşik Değeri |
|------------|-------------|
| Gaz (ppm)  | > 200       |
| Sıcaklık   | > 30°C      |
| Nem        | > 80%       |

Bu değerlerin herhangi biri aşıldığında:
- OLED ekranda “HAVA KALİTESİ KÖTÜ!” uyarısı gösterilir.
- LED matris üzerinde uyarı şekli gösterilir.

Aksi durumda:
- LED matris gülen yüz gösterir.
- OLED üzerinde olumlu mesaj yer alır.

## 🖥️ Seri Monitör Çıkışı

Program her ölçümde aşağıdaki gibi bir çıktı verir:

Sicaklik: 28.4 °C, Nem: 45.2 %, Gaz: 134



## 📦 Kütüphaneler

- `Deneyap_DumanDedektoru.h`
- `Deneyap_SicaklikNemBasincOlcer.h`
- `Deneyap_OLED.h`
- `Deneyap_5x7LedEkran.h`

Kütüphaneler Deneyap KArt IDE veya Arduino IDE üzerinden eklenmelidir.

## 🔄 Döngü Süresi

Ölçüm ve güncelleme her **3 saniyede bir** yapılmaktadır.

## 📸 LED Matris Görselleri

## 👩‍💻 Geliştirici

**Kevser ÜSTÜN**  
Giresun Üniversitesi – Bilgisayar Mühendisliği  
