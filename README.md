# Smart Desk Station

Prototipo de control y automatización de puesto de trabajo desarrollado para la asignatura **Especialidad III** (Ingeniería Civil Informática — Universidad Autónoma de Chile).

## Funcionalidades
* **Control de presencia (HC-SR04):** Suspende el monitor de Windows tras detectar ausencia continua y lo reactiva al regresar al escritorio[cite: 1, 4].
* **Automatización de entorno (RFID RC522):** Valida credenciales por radiofrecuencia para desplegar automáticamente Visual Studio Code, servidor local, pestañas web y Spotify[cite: 1, 4].

## Conexiones de Hardware (Arduino UNO)
| Componente | Pin Módulo | Pin Arduino UNO |
| :--- | :--- | :--- |
| LED Verde (+ R 220Ω) | Ánodo (+) | D2[cite: 1, 4] |
| LED Rojo (+ R 220Ω) | Ánodo (+) | D3[cite: 1, 4] |
| Buzzer Activo 5V | Señal (+) | D4[cite: 1, 4] |
| Sensor HC-SR04 | ECHO / TRIG | D6 / D7[cite: 1, 4] |
| Lector RFID RC522 | RST / SDA | D9 / D10[cite: 1, 4] |
| Lector RFID RC522 | MOSI / MISO / SCK | D11 / D12 / D13[cite: 1, 4] |

## Integrantes
1. Román Acevedo   
2. gnacio Alcalde   
3. Maximiliano Briones   
4. Benjamín González

## Instalación y Uso
1. **Firmware:** Abrir `firmware/SmartDeskStation/SmartDeskStation.ino` en Arduino IDE, instalar la librería `MFRC522` y cargar en la placa[cite: 1, 4].
2. **Software PC:**
   ```bash
   cd software
   pip install -r requirements.txt
   python smart_desk.py

## 3. Pasos para crear y subir el repositorio a GitHub

1. **Crear el repositorio en la web de GitHub:**
   * Inicia sesión en [github.com](https://github.com) y haz clic en **New** (Nuevo repositorio).
   * Asígnale el nombre `smart-desk-station`, déjalo en **Public** (o **Private** si prefieres invitar solo a tus compañeros y al docente) y **no marques** ninguna casilla de inicialización (*Add a README* o *.gitignore*), ya que los creaste localmente.
   * Haz clic en **Create repository** y copia la URL HTTPS que aparece (ej. `[https://github.com/tu-usuario/smart-desk-station.git](https://github.com/tu-usuario/smart-desk-station.git)`).

2. **Subir los archivos desde la terminal de VS Code:**
   * Abre la carpeta raíz `smart-desk-station` en Visual Studio Code.
   * Abre la terminal integrada (`Ctrl + ñ` o `Ctrl + J`) y ejecuta en orden:

   ```bash
   # Inicializar Git en la carpeta del proyecto
   git init

   # Agregar todos los archivos y carpetas
   git add .

   # Crear el primer commit
   git commit -m "feat: estructura inicial de firmware Arduino y listener Python"

   # Renombrar la rama principal a main
   git branch -M main

   # Vincular con tu repositorio remoto (reemplaza con tu URL)
   git remote add origin https://github.com/tu-usuario/smart-desk-station.git

   # Subir el código a GitHub
   git push -u origin main