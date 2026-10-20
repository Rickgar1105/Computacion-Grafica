# Ejercicio de práctica 9: Animación básica

Parte del código de la práctica 8. El rover y el avión ya no se mueven con el teclado: se animan solos.

- Rover: recorre el piso en X en un ciclo infinito. Avanza hasta que su frente llega a la orilla del plano
  (x = 300), retrocede hasta que su parte trasera llega a la orilla contraria (x = -300) y vuelve a avanzar.
  Las llantas giran según la distancia recorrida, al revés cuando retrocede.
- Avión: vuela en un circuito cerrado (dos rectas y dos medias vueltas, a 10 de altura). Siempre apunta hacia
  donde avanza, nunca vuela de reversa, y se ladea hacia adentro en las vueltas. Su faro gira con él.

Ricardo Emmanuel Galicia Tequianes - 118001740

## Compilar (macOS, Homebrew: glfw, glew, glm, assimp)

    cd Ejercicio9
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    cd build && ./ejercicio9

## Controles

- W A S D + ratón: cámara
- Flecha izquierda / derecha: mueve el holocrón Jedi (además gira en dos ejes, flota y sus esquinas se abren y se cierran solas)
- C: siguiente color de la luz inferior del avión (rojo, amarillo, verde, cian, azul, magenta)
- K: enciende o apaga la luz azul claro del holocrón
- P: enciende o apaga la luz verde de la pinza del rover
- N: enciende o apaga las luces de la nave (luz puntual y spotlight al mismo tiempo)
- 1 / 2: enciende o apaga el motor 1 (izquierdo) / motor 2 (derecho): llama de plasma y luz azul
- L: linterna de la cámara
- Esc: cerrar

## Capturas

`./ejercicio9 --capturas ../Evidencias/lista_capturas.txt` recorre la lista dentro de la misma
ventana y guarda cada vista como .ppm en la carpeta build; mientras toma las capturas la animación se detiene
y el estado sale de la lista (opciones del ejercicio 9: --rover x, --sentido 1|-1, --recorrido s, donde s es la
distancia avanzada por el avión sobre su circuito). Con --salir el programa se cierra al terminar la lista.

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
