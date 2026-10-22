# Ejercicio de práctica 9-2: Animación compleja

Parte del código de la práctica 9 (nave en la pista de carreras, satélite en ocho, rover y holocrón).

- Avión: ya no da vueltas en el circuito del ejercicio 9. Ejecuta un programa de acrobacias que se repite sin
  saltos: recta, rizo, recta, tonel, recta, Immelmann (medio rizo y medio tonel), recta, tonel volado, recta y
  Split-S (medio tonel y medio rizo). El programa es el arreglo PROGRAMA_ACROBACIAS; cada tipo de maniobra es un
  caso de AvanzarManiobra y PoseAcrobacia recorre el arreglo para obtener la posición, el frente y el techo del
  avión a una distancia s del inicio. Todo va en el plano z = 16, de y = 5 a y = 28.
- Estela de humo: 70 esferas en las posiciones anteriores de la cola del avión, calculadas con la misma función.
- El punto 2 del ejercicio (textura animada) se explicará la siguiente clase.

Ricardo Emmanuel Galicia Tequianes - 118001740

## Compilar (macOS, Homebrew: glfw, glew, glm, assimp)

    cd Ejercicio9_2
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    cd build && ./ejercicio9_2

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

`./ejercicio9_2 --capturas ../Evidencias/lista_capturas.txt` recorre la lista dentro de la misma
ventana y guarda cada vista como .ppm en la carpeta build; mientras toma las capturas la animación se detiene
y el estado sale de la lista (opciones del ejercicio 9: --rover x, --sentido 1|-1, --recorrido s, donde s es la
distancia avanzada por el avión sobre su programa de acrobacias; de la práctica 9: --pista s para la nave, --satelite t para
el parámetro del ocho y --fase f para las extremidades, en radianes). Con --salir el programa se cierra al terminar la lista.

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
- Textures/pista.png y Textures/pista_muro.png: texturas de la pista y de sus muros, generadas con
  herramientas/texturas_pista.py.
- Evidencias: capturas usadas en el reporte.
