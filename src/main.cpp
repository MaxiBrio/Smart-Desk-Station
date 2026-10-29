#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Pines para RC522 (Comunicación SPI)
#define RST_PIN   9
#define SS_PIN    10

// Pines para HC-SR04
#define TRIG_PIN  7
#define ECHO_PIN  6

// Parámetros de detección y filtrado
const int DISTANCIA_UMBRAL = 60;          // Distancia en cm para detectar presencia
const int HISTERESIS = 10;                // Margen para confirmar salida (70 cm)
const unsigned long COOLDOWN_RFID = 2500; // Tiempo mínimo entre lecturas del mismo tag (ms)

MFRC522 mfrc522(SS_PIN, RST_PIN);

bool personaDetectada = false;
unsigned long ultimaLecturaRFID = 0;
unsigned long ultimoMuestreoSensor = 0;

long medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Timeout de 30 ms para evitar bloqueos si no hay eco
  long duracion = pulseIn(ECHO_PIN, HIGH, 30000); 
  if (duracion == 0) return 999;
  return duracion * 0.0343 / 2;
}

void procesarRFID() {
  if (millis() - ultimaLecturaRFID < COOLDOWN_RFID) return;
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  // Construir el UID en formato hexadecimal continuo
  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();

  // Enviar evento estructurado a la PC
  Serial.print("RFID_TRIGGER:");
  Serial.println(uidStr);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  ultimaLecturaRFID = millis();
}

void procesarPresencia() {
  long distancia = medirDistancia();

  // Detección de llegada
  if (distancia > 0 && distancia <= DISTANCIA_UMBRAL && !personaDetectada) {
    personaDetectada = true;
    Serial.println("PRESENCIA_TRIGGER:ENTRADA");
  } 
  // Detección de salida con margen de histéresis
  else if (distancia > (DISTANCIA_UMBRAL + HISTERESIS) && personaDetectada) {
    personaDetectada = false;
    Serial.println("PRESENCIA_TRIGGER:SALIDA");
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("SYSTEM_READY");
}

void loop() {
  procesarRFID();

  // Muestreo del sensor ultrasónico cada 100 ms
  if (millis() - ultimoMuestreoSensor >= 100) {
    ultimoMuestreoSensor = millis();
    procesarPresencia();
  }
}