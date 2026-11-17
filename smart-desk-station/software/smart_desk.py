import ctypes
import os
import subprocess
import time
import webbrowser
import serial

# Cambia 'COM3' por el puerto asignado a tu Arduino en el Administrador de dispositivos / Arduino IDE
PUERTO_COM = "COM3"
BAUD_RATE = 9600


def suspender_pantalla():
  """Apaga la señal de video del monitor."""
  print("[ULTRASONIDO] Ausencia detectada -> Suspendiendo pantalla...")
  # 0x0112: WM_SYSCOMMAND | 0xF170: SC_MONITORPOWER | 2: Apagar monitor
  ctypes.windll.user32.SendMessageW(0xFFFF, 0x0112, 0xF170, 2)


def prender_pantalla():
  """Reactiva el monitor y envía un evento de entrada para despertar la imagen."""
  print("[ULTRASONIDO] Presencia detectada -> Encendiendo pantalla...")
  # -1: Encender monitor
  ctypes.windll.user32.SendMessageW(0xFFFF, 0x0112, 0xF170, -1)
  # Movimiento relativo de 1 píxel ida y vuelta para que Windows reactive el video
  ctypes.windll.user32.mouse_event(0x0001, 1, 0, 0, 0)
  ctypes.windll.user32.mouse_event(0x0001, -1, 0, 0, 0)


def ejecutar_macro():
  """Abre aplicaciones de trabajo y páginas web configuradas."""
  print("[RFID] Credencial autorizada -> Ejecutando macro de entorno...")

  # 1. Abrir Visual Studio Code
  try:
    subprocess.Popen(["code"], shell=True)
  except Exception as e:
    print(f"  -> No se pudo abrir VS Code: {e}")

  # 2. Abrir Spotify
  try:
    os.startfile("spotify:")
  except Exception:
    subprocess.Popen(["cmd", "/c", "start", "spotify:"], shell=True)

  # 3. Abrir páginas web (académicas, repositorios o servidor local)
  paginas = [
      "https://github.com",
      "https://www.uautonoma.cl",

  ]
  for url in paginas:
    webbrowser.open(url)

  # 4. Opcional: Si quieres levantar un servidor local automáticamente, descomenta y ajusta la ruta:
  # subprocess.Popen(["npm", "run", "dev"], cwd=r"C:\ruta\a\tu\proyecto", shell=True)


def main():
  print(f"Conectando con Arduino en {PUERTO_COM}...")
  try:
    arduino = serial.Serial(PUERTO_COM, BAUD_RATE, timeout=1)
    time.sleep(2)  # Espera a que el Arduino complete su reinicio serial
    print("Smart Desk Station en línea. Esperando eventos...")

    while True:
      if arduino.in_waiting > 0:
        mensaje = arduino.readline().decode("utf-8", errors="ignore").strip()

        if mensaje == "EVENT:SUSPENDER_PANTALLA":
          suspender_pantalla()
        elif mensaje == "EVENT:PRENDER_PANTALLA":
          prender_pantalla()
        elif mensaje == "EVENT:DISPARAR_MACRO":
          ejecutar_macro()
        elif mensaje == "EVENT:ACCESO_DENEGADO":
          print("[RFID] Alerta: Tarjeta no autorizada.")
        elif mensaje.startswith("UID_DETECTADO:"):
          print(f"[INFO] {mensaje}")

  except serial.SerialException:
    print(
        f"Error: No se pudo abrir {PUERTO_COM}. Verifica el número de puerto y"
        " cierra el Monitor Serie del Arduino IDE."
    )
  except KeyboardInterrupt:
    print("\nPrograma detenido por el usuario.")


if __name__ == "__main__":
  main()