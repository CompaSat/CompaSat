import serial
import time
import os

ruta_directorio = "C:/Users/CMC10506/Documents/Antón/datos"

os.makedirs(ruta_directorio, exist_ok=True)

# Función para encontrar el próximo nombre de archivo disponible
def obtener_nombre_archivo():
    contador = 1
    while True:
        nombre_archivo = f"datos_recibidos_{contador}.txt"
        ruta_completa = os.path.join(ruta_directorio, nombre_archivo)
        if not os.path.exists(ruta_completa):
            return ruta_completa
        contador += 1

try:
    arduino = serial.Serial('COM8', 9600, timeout=1)
    time.sleep(2)  # Espera a que el puerto se inicialice
    print("Conectado con éxito al puerto COM3")
    
    # Obtiene el nombre del archivo disponible
    nombre_archivo = obtener_nombre_archivo()
    print(f"Guardando datos en: {nombre_archivo}")
    
    # Abre el archivo para guardar los datos
    with open(nombre_archivo, 'a') as file:
        while True:
            if arduino.in_waiting > 0:
                line = arduino.readline().decode('utf-8').strip()
                print(f"Datos recibidos: {line}")
                
                # Guarda la línea en el archivo
                file.write(line + '\n')
                file.flush()
                
except serial.SerialException as e:
    print(f"No se puede conectar con el puerto: {e}")
finally:
    if 'arduino' in locals() and arduino.is_open:
        arduino.close()
        print("Puerto cerrado")
