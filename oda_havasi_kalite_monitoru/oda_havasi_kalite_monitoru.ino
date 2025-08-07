#include <Deneyap_DumanDedektoru.h>              
#include <Deneyap_SicaklikNemBasincOlcer.h>      
#include <Deneyap_OLED.h>                       
#include <Deneyap_5x7LedEkran.h>                

SmokeDetector gazSensor;
SHT4x thpSensor;
OLED oled;
DotMatrix ledMatrix;

const uint16_t gazEsigi = 200;
const float sicaklikEsigi = 30.0;
const float nemEsigi = 80.0;

void setup() {
  Serial.begin(115200);

  if (!gazSensor.begin(0x20)) {
    Serial.println("Gaz sensörü I2C bağlantısı başarısız");
    while (1);
  }
  
  if (!thpSensor.begin(0x44)) {
    Serial.println("Sıcaklık/Nem sensörü I2C bağlantısı başarısız");
    while (1);
  }

  if (!oled.begin(0x7A)) {
    Serial.println("OLED ekran I2C bağlantısı başarısız");
  }
  oled.init();

  if (!ledMatrix.begin(0x0A)) {
    Serial.println("LED matris I2C bağlantısı başarısız");
    while (1);
  }

  oled.clearDisplay();
  oled.setTextXY(0, 0);
  oled.putString("Hava Kalitesi");
  oled.setTextXY(1, 0);
  oled.putString("Analiz ediliyor...");
  delay(2000);
}

void loop() {
  if (thpSensor.measure() != SHT4X_STATUS_OK) {
    Serial.println("Sicaklik/Nem ölçüm hatası");
    return;
  }

  float sicaklik = thpSensor.TtoDegC();
  float nem = thpSensor.RHtoPercent();
  uint16_t gazDegeri = gazSensor.ReadSmokeAnalog();

  Serial.print("Sicaklik: "); Serial.print(sicaklik); Serial.print(" °C, ");
  Serial.print("Nem: "); Serial.print(nem); Serial.print(" %, ");
  Serial.print("Gaz: "); Serial.println(gazDegeri);

  oled.clearDisplay();
  oled.setTextXY(0, 0);
  oled.putString("Sicaklik: ");
  oled.putFloat(sicaklik, 1);
  oled.putString(" C");

  oled.setTextXY(1, 0);
  oled.putString("Nem: ");
  oled.putFloat(nem, 1);
  oled.putString(" %");

  oled.setTextXY(2, 0);
  oled.putString("Gaz: ");
  oled.putInt(gazDegeri);
  oled.putString(" ppm");

  bool havaKotu = (gazDegeri > gazEsigi || sicaklik > sicaklikEsigi || nem > nemEsigi);

  ledMatrix.resetDotRows();

  if (havaKotu) {
    const byte uyari[35] = {
      1,0,0,0,1,
      0,1,0,1,0,
      0,0,1,0,0,
      0,1,0,1,0,
      1,0,0,0,1,
      0,0,0,0,0,
      0,0,0,0,0
    };

    ledMatrix.drawLedMatrix(uyari, 500);

    oled.setTextXY(4, 0);
    oled.putString("HAVA KALITESI KOTU!");
  } else {
    const byte gulumseme[35] = {
      0,0,0,0,0, 
      0,0,0,0,0,  
      0,1,0,1,0,  
      0,0,0,0,0,  
      1,0,0,0,1,  
      0,1,1,1,0,  
      0,0,0,0,0   
    };

    ledMatrix.drawLedMatrix(gulumseme, 500);  
    oled.setTextXY(4, 0);
    oled.putString("Hava Durumu: Iyi");
  }

  delay(3000); 
}
