# Práctica 8: Iluminación 2

Parte del código de la práctica 7.

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
- C: siguiente color de la luz inferior del avión (rojo, amarillo, verde, cian, azul, magenta)
- K: enciende o apaga la luz azul claro del holocrón
- P: enciende o apaga la luz verde de la pinza del rover
- L: linterna de la cámara
- Esc: cerrar

## Capturas

`./practica8 --capturas ../Evidencias/lista_capturas.txt` recorre la lista dentro de la misma
ventana, guarda cada vista como .ppm en la carpeta build (opciones nuevas: --color 0..5, --holocron 0|1, --pinza 0|1) y después deja el programa abierto.

## Archivos

- Models/Pata_*.obj y Models/Llanta_*.obj: patas y llantas del rover separadas en Blender
  (script Models/separar_llantas_rover.py; archivo editable en ../Blender/rover_llantas_separadas.blend).
- Models/lampara.obj: light-curved de Kenney, City Kit (Roads), licencia CC0.
- Evidencias: capturas usadas en los documentos.
