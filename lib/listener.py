import serial
import subprocess
import time
import os

# Ajusta al puerto COM correspondiente (ej. 'COM3', 'COM4' o '/dev/ttyUSB0')
PUERTO_SERIAL = 'COM3' 
BAUD_RATE = 115200

# Diccionario de UIDs y sus ejecutables o comandos asociados
ACCIONES_RFID = {
    "A3B4C5D6": r"C:\Program Files\Microsoft VS Code\Code.exe",
    "E1F2A3B4": "spotify.exe",
    "44A26129": "calc.exe"
}

def ejecutar_macro(ruta_o_comando):
    try:
        # Si es un ejecutable local o binario de sistema directo
        if os.path.exists(ruta_o_comando) or ruta_o_comando.endswith(".exe"):
            subprocess.Popen([ruta_o_comando])
        else:
            # Comandos de terminal, scripts o URLs
            os.system(ruta_o_comando)
    except Exception as e:
        print(f"Error al ejecutar macro: {e}")

def main():
    try:
        ser = serial.Serial(PUERTO_SERIAL, BAUD_RATE, timeout=1)
        print(f"Conectado exitosamente al puerto {PUERTO_SERIAL}")
    except serial.SerialException as e:
        print(f"Error al conectar con el puerto serial: {e}")
        return

    time.sleep(2)  # Pausa requerida por el auto-reset del microcontrolador al abrir el puerto

    while True:
        try:
            linea = ser.readline().decode('utf-8', errors='ignore').strip()
            if not linea:
                continue

            print(f"[EVENTO SERIAL]: {linea}")

            # Procesamiento de tarjeta o llavero RFID
            if linea.startswith("RFID_TRIGGER:"):
                uid = linea.split(":")[1]
                if uid in ACCIONES_RFID:
                    print(f"UID detectado: {uid} -> Lanzando aplicacion...")
                    ejecutar_macro(ACCIONES_RFID[uid])
                else:
                    print(f"UID no registrado en la lista: {uid}")

            # Procesamiento de proximidad por ultrasonido
            elif linea == "PRESENCIA_TRIGGER:ENTRADA":
                print("Persona detectada frente al sensor -> Ejecutando macro de entrada")
                # Ejemplo: abrir el navegador o un dashboard predeterminado
                subprocess.Popen(["cmd", "/c", "start", "https://github.com"])

            elif linea == "PRESENCIA_TRIGGER:SALIDA":
                print("El sensor ya no detecta presencia")

        except KeyboardInterrupt:
            print("\nFinalizando listener...")
            break
        except Exception as ex:
            print(f"Excepción en la lectura serial: {ex}")

    ser.close()

if __name__ == "__main__":
    main()