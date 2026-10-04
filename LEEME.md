# Práctica 6: Texturizado

Abre Practica6Texturizado.app con doble clic en este Mac. La aplicación fue compilada para ARM64 y utiliza las bibliotecas Homebrew instaladas en este equipo; para otro equipo puede ser necesario recompilar.

## Controles

1: holocrón por código. 2: holocrón importado. 3: avión. 4: vista general.
W/A/S/D: mover cámara. Botón derecho y ratón: orientar cámara.
R: activar o detener el giro. F12: guardar captura PPM en Resources. Esc: salir.

## Archivos

Practica6 contiene C++, CMakeLists.txt, shaders, texturas, modelos OBJ/MTL y evidencias reales de ejecución. Los editables finales son Models/holocron_final.blend y Models/avion_final.blend. Mantén la carpeta Textures junto a Models al trabajar con los archivos. Las seis caras corresponden a las imágenes proporcionadas; Textures/ORIGEN_CARAS.txt conserva sus nombres originales.

El reporte DOCX utiliza la plantilla de formatos; el PDF es su versión de lectura. El avión es una recreación simplificada de la referencia, con ojos, nariz, sonrisa y llamas. La hélice es estática.

La geometría se modeló en la interfaz de Blender. El ajuste posterior de UV y materiales del OBJ se hizo sobre esa geometría exportada. Los shaders de texturizado proceden del material del profesor. GL_REPEAT continúa disponible por defecto; las caras usan GL_CLAMP_TO_EDGE. La carga RGBA evita modificar GL_UNPACK_ALIGNMENT.

## Recompilación

Dependencias: CMake, pkg-config, GLFW, GLEW, GLM y Assimp (Homebrew).
Desde Practica6:

```sh
cmake -S . -B build
cmake --build build
cd build
./practica6
```

La ejecución comprobada corresponde a macOS, OpenGL 4.1 y GLSL 4.10. Consulta Evidencias/registro_ejecucion.txt.
