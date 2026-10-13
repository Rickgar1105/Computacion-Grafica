# Práctica 8: Iluminación 2

Parte del código del ejercicio de clase 8 (que a su vez parte de la práctica 7).

Ricardo Emmanuel Galicia Tequianes - 118001740

## Compilar (macOS, Homebrew: glfw, glew, glm, assimp)

    cd Practica8
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    cd build && ./practica8

## Controles

- W A S D + ratón: cámara
- Y / U: rover hacia adelante / hacia atrás (faro azul y giro de llantas)
- Flecha arriba / abajo: el avión sube / baja con su luz inferior
- H / J: el avión avanza / retrocede con su faro naranja
- Flecha izquierda / derecha: mueve el holocrón Jedi (además gira en dos ejes, flota y sus esquinas se abren y se cierran solas)
- C: siguiente color de la luz inferior del avión (rojo, amarillo, verde, cian, azul, magenta)
- K: enciende o apaga la luz azul claro del holocrón
- P: enciende o apaga la luz verde de la pinza del rover
- N: enciende o apaga las luces de la nave (luz puntual y spotlight al mismo tiempo)
- 1 / 2: enciende o apaga el motor 1 (izquierdo) / motor 2 (derecho): llama de plasma y luz azul
- L: linterna de la cámara
- Esc: cerrar

## Capturas

`./practica8 --capturas ../Evidencias/lista_capturas.txt` recorre la lista dentro de la misma
ventana, guarda cada vista como .ppm en la carpeta build (opciones nuevas: --color 0..5, --holocron 0|1, --pinza 0|1, --holocronx x, --holocronang grados, --apertura 0..1, --nave 0|1, --motor1 0|1, --motor2 0|1) y después deja el programa abierto.

## Archivos

- Models/Pata_*.obj y Models/Llanta_*.obj: patas y llantas del rover separadas en Blender
  (script Models/separar_llantas_rover.py; archivo editable en ../Blender/rover_llantas_separadas.blend).
- Models/holocron_esquina.obj y Models/holocron_nucleo.obj: holocrón Jedi armado con 8 esquinas (la misma malla reflejada)
  y un núcleo, 24 vértices cada malla y una sola textura (Textures/holocron_jedi.png, hecha de una foto con
  ../_trabajo/textura_holocron.py).
- Models/nave_speeder.obj: craft_speederA de Kenney (Space Kit 2.0, CC0) con coordenadas UV agregadas por
  proyección de caja (herramientas/nave_uv.py). Texturas nave_*.png y fuego_plasma.png generadas con
  herramientas/texturas_nave.py.
- Models/lampara.obj: light-curved de Kenney, City Kit (Roads), licencia CC0.
- Evidencias: capturas usadas en los documentos.
