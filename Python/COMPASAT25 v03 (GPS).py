import serial
import time
import os
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import folium
from flask import Flask, render_template_string, jsonify
import logging

# Configuración del directorio para guardar los datos
ruta_directorio = "C:/Users/Usuario/Documents/YO/Clase/BACH/STEM/CANSAT/A1 Programas/Python/datos"
os.makedirs(ruta_directorio, exist_ok=True)

# Función para encontrar el próximo nombre de archivo disponible
def obtener_nombre_base():
    contador = 1
    while True:
        nombre_base = f"datos_recibidos_{contador}"
        ruta_archivo = os.path.join(ruta_directorio, f"{nombre_base}.txt")
        ruta_grafica = os.path.join(ruta_directorio, f"{nombre_base}.png")
        if not os.path.exists(ruta_archivo) and not os.path.exists(ruta_grafica):
            return nombre_base
        contador += 1

# Obtener nombre base para los archivos
nombre_base = obtener_nombre_base()
nombre_archivo = os.path.join(ruta_directorio, f"{nombre_base}.txt")
ruta_grafica = os.path.join(ruta_directorio, f"{nombre_base}.png")

# Variables globales para graficar
num = []
temperatura = []
presion = []
altura = []
latitud = []
longitud = []
satelites = []
compas = []

# Configuración del mapa en Flask
app = Flask(__name__)
current_coords = {'lat': 0, 'lon': 0}

# Configuración para suprimir logs en la terminal
log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)

@app.route('/')
def index():
    return render_template_string('''
    <!DOCTYPE html>
    <html>
    <head>
        <title>COMPASAT25 - Recorrido</title>
        <style>
            #map {
                height: 100vh;
                width: 100%;
            }
        </style>
        <script src="https://cdn.jsdelivr.net/npm/leaflet@1.9.3/dist/leaflet.js"></script>
        <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/leaflet@1.9.3/dist/leaflet.css" />
    </head>
    <body>
        <div id="map"></div>
        <script>
            var map = L.map('map').setView([{{ coords.lat }}, {{ coords.lon }}], 15);
            var marker = L.marker([{{ coords.lat }}, {{ coords.lon }}]).addTo(map);
            L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
                maxZoom: 19
            }).addTo(map);

            // Lista para almacenar el historial de coordenadas
            var route = [];
            var polyline = L.polyline(route, { color: 'blue' }).addTo(map);

            setInterval(() => {
                fetch('/update').then(response => response.json()).then(data => {
                    // Agregar nueva coordenada al historial
                    var newLatLng = [data.lat, data.lon];
                    route.push(newLatLng);

                    // Actualizar la línea en el mapa
                    polyline.setLatLngs(route);

                    // Centrar el mapa y mover el marcador
                    map.setView(newLatLng, map.getZoom());
                    marker.setLatLng(newLatLng);
                });
            }, 1000);
        </script>
    </body>
    </html>
    ''', coords=current_coords)

@app.route('/update')
def update():
    return jsonify(current_coords)

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
            if len(datos) == 8:
                num.append(int(datos[0]))
                temperatura.append(float(datos[1]))
                presion.append(float(datos[2]))
                altura.append(float(datos[3]))
                lat_actual = float(datos[4])
                lon_actual = float(datos[5])
                latitud.append(lat_actual)
                longitud.append(lon_actual)
                satelites.append(int(datos[6]))
                compas.append(datos[7])

                # Actualizar coordenadas en el mapa
                current_coords['lat'] = lat_actual
                current_coords['lon'] = lon_actual

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
                ax3.plot(num, altura, color='g', label='Altitud (m)')
                ax3.set_title("Altitud")
                ax3.set_ylabel("m")
                ax3.legend()
                ax3.grid()

                # Guardar la gráfica en un archivo PNG
                plt.savefig(ruta_grafica)

        except (ValueError, IndexError):
            print("Línea no válida, ignorada.")
            pass

# Configuración del puerto serie
try:
    arduino = serial.Serial('COM7', 9600, timeout=1)
    time.sleep(2)  # Espera a que el puerto se inicialice
    print("Conectado con éxito al puerto COM7")
    
    print(f"Guardando datos en: {nombre_archivo}")
    
    # Abrir el archivo para guardar los datos
    with open(nombre_archivo, 'a') as file:
        # Configuración de la figura de matplotlib
        fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 8))
        ani = FuncAnimation(fig, actualizar_grafica, interval=1000, cache_frame_data=False)  # Actualizar cada 1s
        
        # Iniciar el servidor Flask en un hilo separado
        import threading
        threading.Thread(target=app.run, kwargs={'debug': False, 'use_reloader': False}).start()
        print("Conecta a http://127.0.0.1:5000 para ver el mapa")

        plt.tight_layout()
        plt.show()

except serial.SerialException as e:
    print(f"No se puede conectar con el puerto: {e}")
finally:
    if 'arduino' in locals() and arduino.is_open:
        arduino.close()
        print("Puerto cerrado")
