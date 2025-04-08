import serial
import time
import os
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Configuración del directorio para guardar los datos
ruta_directorio = "C:/Users/Usuario/Documents/YO/Clase/BACH/STEM/CANSAT/A1 Programas/Python/datos"
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
def obtener_nombre_grafica():
    contador = 1
    while True:
        nombre_grafica = f"grafica_{contador}.png"
        grafica_completa = os.path.join(ruta_directorio, nombre_grafica)
        if not os.path.exists(grafica_completa):
            return grafica_completa
        contador += 1
ruta_grafica = obtener_nombre_grafica()

# Variables globales para graficar
num = []
temperatura = []
presion = []
altura = []

# Función para actualizar la gráfica en tiempo real
def actualizar_grafica(frame):
    if arduino.in_waiting > 0:
        try:
            # Leer y procesar datos
            line = arduino.readline().decode('utf-8').strip()
            print(f"Datos recibidos: {line}")
            
            # Escribir en el archivo
            file.write(line + '\n')
            file.flush()
            
            # Extraer valores de los datos recibidos
            datos = line.split(",")
            if len(datos) >= 4:
                num.append(int(datos[0]))
                temperatura.append(float(datos[1]))
                presion.append(float(datos[2]))
                altura.append(float(datos[3]))
                
                # Actualizar subgráficas
                ax1.clear()
                ax1.plot(num, temperatura, color='r', label='Temperatura (°C)')
                ax1.set_title("Temperatura")
                ax1.set_ylabel("°C")
                ax1.legend()
                ax1.grid()
                
                ax2.clear()
                ax2.plot(num, presion, color='b', label='Presión (Pa)')
                ax2.set_title("Presión")
                ax2.set_ylabel("Pa")
                ax2.legend()
                ax2.grid()
                
                ax3.clear()
                ax3.plot(num, altura, color='g', label='Altura (m)')
                ax3.set_title("Altura")
                ax3.set_ylabel("m")
                ax3.legend()
                ax3.grid()

                plt.savefig(ruta_grafica)
        except (ValueError, IndexError):
            print("Línea no válida, ignorada.")
            pass

# Configuración del puerto serie
try:
    arduino = serial.Serial('COM5', 9600, timeout=1)
    time.sleep(2)  # Espera a que el puerto se inicialice
    print("Conectado con éxito al puerto COM5")
    
    # Obtener el nombre del archivo para guardar los datos
    nombre_archivo = obtener_nombre_archivo()
    print(f"Guardando datos en: {nombre_archivo}")
    
    # Abrir el archivo para guardar los datos
    with open(nombre_archivo, 'a') as file:
        # Configuración de la figura de matplotlib
        fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 8))
        ani = FuncAnimation(fig, actualizar_grafica, interval=1000)  # Actualizar cada 1s
        
        plt.tight_layout()
        plt.show()

except serial.SerialException as e:
    print(f"No se puede conectar con el puerto: {e}")
finally:
    if 'arduino' in locals() and arduino.is_open:
        arduino.close()
        print("Puerto cerrado")
