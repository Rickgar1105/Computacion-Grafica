# Práctica 9: Animación básica

Parte del código del ejercicio de clase 9 (rover de orilla a orilla del piso y avión en un circuito cerrado).

- Nave (práctica 8): recorre una pista de carreras detrás de la escena, con dos rectas, dos curvas y, en la
  recta 1, una rampa de subida, un tramo elevado a 4 unidades y una rampa de bajada. Siempre apunta hacia donde
  avanza, levanta o baja la nariz en las rampas y se ladea hacia adentro en las curvas. Sus luces y las llamas
  de los motores van con ella. La malla de la pista y sus muros se generan con la misma función que mueve a la
  nave (PuntoPista).
- Satélite (sonda espacial de la práctica 4, misma jerarquía): vuela sobre la escena siguiendo un ocho y gira
  hacia donde avanza. Sus cuatro extremidades, que en la práctica 4 se movían con Q, Z, X y C, oscilan solas
  ±30° con 90° de desfase entre una y otra.
- El rover y el avión siguen animados como en el ejercicio 9.

Ricardo Emmanuel Galicia Tequianes - 118001740

## Compilar (macOS, Homebrew: glfw, glew, glm, assimp)

    cd Practica9
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    cd build && ./practica9

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

`./practica9 --capturas ../Evidencias/lista_capturas.txt` recorre la lista dentro de la misma
ventana y guarda cada vista como .ppm en la carpeta build; mientras toma las capturas la animación se detiene
y el estado sale de la lista (opciones del ejercicio 9: --rover x, --sentido 1|-1, --recorrido s, donde s es la
distancia avanzada por el avión sobre su circuito; de la práctica 9: --pista s para la nave, --satelite t para
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
