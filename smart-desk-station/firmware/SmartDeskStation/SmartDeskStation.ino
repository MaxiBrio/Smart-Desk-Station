#include <SPI.h>
#include <MFRC522.h>

// Pines exactos según tu esquema de conexión
const int PIN_LED_VERDE = 2;
const int PIN_LED_ROJO  = 3;
const int PIN_BUZZER    = 4;
const int PIN_ECHO      = 6;
const int PIN_TRIG      = 7;
#define SS_PIN  10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Parámetros de detección de presencia con histéresis (HC-SR04)
const int DIST_DETECCION   = 50;  // <= 50 cm detecta presencia y prende pantalla
const int DIST_SALIDA      = 60;  // > 60 cm evalúa retiro del usuario
const int LECTURAS_ENTRADA = 3;   // 3 muestras (0.6 s) para confirmar llegada
const int LECTURAS_SALIDA  = 20;  // 20 muestras x 200 ms = 4 segundos de ausencia para suspender

bool usuarioPresente = true;
int contadorEntrada  = 0;
int contadorSalida   = 0;

unsigned long ultimoMuestreoSonar = 0;
const unsigned long INTERVALO_SONAR = 200; // Muestreo cada 200 ms

// REEMPLAZA ESTE VALOR con el UID de tu tarjeta física (ver pasos abajo)
const String UID_AUTORIZADO = "ad e3 3e 1b";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  actualizarIndicadores();
  sonarBuzzer(100, 1);
  Serial.println("SYSTEM:READY");
}

void loop() {
  unsigned long tiempoActual = millis();

  // 1. CONTROL DE PANTALLA POR PRESENCIA (HC-SR04)
  if (tiempoActual - ultimoMuestreoSonar >= INTERVALO_SONAR) {
    ultimoMuestreoSonar = tiempoActual;
    int distancia = medirDistancia();

    if (!usuarioPresente) {
      // Usuario estaba ausente: verificar si regresó al escritorio
      if (distancia <= DIST_DETECCION) {
        contadorEntrada++;
        if (contadorEntrada >= LECTURAS_ENTRADA) {
          usuarioPresente = true;
          contadorEntrada = 0;
          contadorSalida  = 0;

          actualizarIndicadores();
          sonarBuzzer(100, 1);
          Serial.println("EVENT:PRENDER_PANTALLA");
        }
      } else {
        contadorEntrada = 0;
      }
    } else {
      // Usuario estaba presente: verificar si se retiró por más de 4 segundos
      if (distancia > DIST_SALIDA) {
        contadorSalida++;
        if (contadorSalida >= LECTURAS_SALIDA) {
          usuarioPresente = false;
          contadorSalida  = 0;
          contadorEntrada = 0;

          actualizarIndicadores();
          sonarBuzzer(80, 1);
          Serial.println("EVENT:SUSPENDER_PANTALLA");
        }
      } else {
        contadorSalida = 0;
      }
    }
  }

  // 2. MACRO DE APLICACIONES POR CREDENCIAL (RFID RC522)
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String uidLeido = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uidLeido += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      uidLeido += String(mfrc522.uid.uidByte[i], HEX);
      if (i < mfrc522.uid.size - 1) uidLeido += " ";
    }
    uidLeido.toLowerCase();

    // Imprime el UID leído para facilitar el registro de tu tarjeta física
    Serial.print("UID_DETECTADO:");
    Serial.println(uidLeido);

    if (uidLeido == UID_AUTORIZADO) {
      Serial.println("EVENT:DISPARAR_MACRO");
      digitalWrite(PIN_LED_ROJO, LOW);
      digitalWrite(PIN_LED_VERDE, HIGH);
      sonarBuzzer(150, 1);
      delay(800);
    } else {
      Serial.println("EVENT:ACCESO_DENEGADO");
      digitalWrite(PIN_LED_VERDE, LOW);
      digitalWrite(PIN_LED_ROJO, HIGH);
      sonarBuzzer(70, 2);
      delay(800);
    }

    // Restaura los LEDs según el estado actual de presencia
    actualizarIndicadores();

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}

int medirDistancia() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Timeout de 25 ms (~4.2 m) para evitar congelamientos y falsos ceros
  long duracion = pulseIn(PIN_ECHO, HIGH, 25000);
  if (duracion == 0) {
    return 999;
  }
  return duracion * 0.034 / 2;
}

void actualizarIndicadores() {
  if (usuarioPresente) {
    digitalWrite(PIN_LED_VERDE, HIGH);
    digitalWrite(PIN_LED_ROJO, LOW);
  } else {
    digitalWrite(PIN_LED_VERDE, LOW);
    digitalWrite(PIN_LED_ROJO, HIGH);
  }
}

void sonarBuzzer(int duracionMs, int repeticiones) {
  for (int i = 0; i < repeticiones; i++) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(duracionMs);
    digitalWrite(PIN_BUZZER, LOW);
    if (repeticiones > 1) {
      delay(duracionMs);
    }
  }
}