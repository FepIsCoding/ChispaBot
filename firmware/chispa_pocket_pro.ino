// Chispa Pocket Pro - firmware minimo (sin probar)
// Placa: XIAO_ESP32S3. USB CDC On Boot: Enabled. Libreria: GxEPD2
// Igual que Pocket, mas: Bluetooth (recibe el mismo protocolo) y un segundo pulsador en D9.
#include <GxEPD2_BW.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#define NUS "6e400001-b5a3-f393-e0a9-e50e2dcc3d3e"
#define RXC "6e400002-b5a3-f393-e0a9-e50e2dcc3d3e"
GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(D1, D3, D4, D5));
String linea = "";
bool foco = true, corre = false, cambio = true;
long resta = 1500;
unsigned long t0 = 0;

void procesar(String l) {
  int i = l.indexOf("left");
  if (i >= 0) {
    resta = l.substring(i + 6).toInt();
    foco = l.indexOf("break") < 0;
    cambio = true;
  }
}
void entrada(char c) {
  if (c == 10) { procesar(linea); linea = ""; }
  else linea += c;
}

class RxCb : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* c) {
    auto v = c->getValue();
    for (size_t k = 0; k < v.length(); k++) entrada((char)v[k]);
  }
};

void dibujar() {
  char b[8];
  sprintf(b, "%02ld:%02ld", resta / 60, resta % 60);
  display.setPartialWindow(0, 0, 200, 200);
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeSans12pt7b);
    display.setCursor(12, 34);
    display.print(foco ? "Foco" : "Pausa: mate");
    display.setFont(&FreeSansBold24pt7b);
    display.setCursor(18, 120);
    display.print(b);
    display.setFont(&FreeSans12pt7b);
    display.setCursor(12, 180);
    display.print(corre ? "Chispa Pocket Pro" : "Toca para empezar");
  } while (display.nextPage());
}

void setup() {
  Serial.begin(115200);
  pinMode(D2, INPUT_PULLUP);
  pinMode(D9, INPUT_PULLUP);
  SPI.begin(D8, -1, D10, D1);
  display.init(115200);
  BLEDevice::init("Chispa Pocket Pro");
  BLEServer* s = BLEDevice::createServer();
  BLEService* sv = s->createService(NUS);
  BLECharacteristic* rx = sv->createCharacteristic(RXC, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  rx->setCallbacks(new RxCb());
  sv->start();
  BLEAdvertising* a = BLEDevice::getAdvertising();
  a->addServiceUUID(NUS);
  a->start();
}

void loop() {
  static bool p2 = true, p9 = true;
  bool a = digitalRead(D2), b = digitalRead(D9);
  if (p2 && !a) { corre = !corre; t0 = millis(); cambio = true; }
  if (p9 && !b) { foco = !foco; resta = foco ? 1500 : 300; cambio = true; }
  p2 = a; p9 = b;
  while (Serial.available()) entrada((char)Serial.read());
  if (corre && millis() - t0 >= 60000) {
    t0 = millis();
    resta -= 60;
    if (resta <= 0) { foco = !foco; resta = foco ? 1500 : 300; }
    cambio = true;
  }
  if (cambio) { cambio = false; dibujar(); }
  delay(20);
}
