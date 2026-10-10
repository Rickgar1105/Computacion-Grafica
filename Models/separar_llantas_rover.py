# Separa la llanta de cada pata del rover de la practica 5 (por material) para
# poder girarla en OpenGL. Cada llanta queda con su origen en el centro de su caja
# envolvente; la pata conserva sus coordenadas originales (su bisagra no cambia).
# Ademas guarda un .blend con la jerarquia Cuerpo -> Pata -> Llanta.
import bpy, os, sys
argv = sys.argv[sys.argv.index("--") + 1:]
carpeta_p5, carpeta_modelos, blend_salida = argv

piezas = ["Delantera_Derecha", "Delantera_Izquierda", "Media_Derecha",
          "Media_Izquierda", "Trasera_Derecha", "Trasera_Izquierda"]

bpy.ops.wm.read_factory_settings(use_empty=True)

def importar(nombre):
    antes = set(bpy.data.objects)
    bpy.ops.wm.obj_import(filepath=os.path.join(carpeta_p5, nombre + ".obj"),
                          forward_axis='NEGATIVE_Z', up_axis='Y')
    nuevos = [o for o in bpy.data.objects if o not in antes]
    return nuevos[0]

def seleccionar(o):
    bpy.ops.object.select_all(action='DESELECT')
    o.select_set(True)
    bpy.context.view_layer.objects.active = o

def exportar(o, nombre):
    seleccionar(o)
    bpy.ops.wm.obj_export(filepath=os.path.join(carpeta_modelos, nombre + ".obj"),
                          export_selected_objects=True, export_materials=True,
                          forward_axis='NEGATIVE_Z', up_axis='Y',
                          export_uv=True, export_normals=True, path_mode='STRIP')

cuerpo = importar("Cuerpo"); cuerpo.name = "Cuerpo"
brazo = importar("Brazo"); brazo.name = "Brazo"
mundo = brazo.matrix_world.copy(); brazo.parent = cuerpo; brazo.matrix_world = mundo

with open(os.path.join(carpeta_modelos, "rover_centros_llantas.txt"), "w") as info:
    for p in piezas:
        pata = importar("PataRueda_" + p)
        seleccionar(pata)
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.mesh.select_all(action='SELECT')
        bpy.ops.mesh.separate(type='MATERIAL')
        bpy.ops.object.mode_set(mode='OBJECT')
        partes = [o for o in bpy.context.selected_objects]
        llanta = next(o for o in partes if any("wheels" in m.name for m in o.data.materials))
        resto = [o for o in partes if o is not llanta]
        seleccionar(resto[0])
        for o in resto: o.select_set(True)
        bpy.ops.object.join()
        pata = bpy.context.view_layer.objects.active
        pata.name = "Pata_" + p
        llanta.name = "Llanta_" + p
        seleccionar(llanta)
        bpy.ops.object.origin_set(type='ORIGIN_GEOMETRY', center='BOUNDS')
        c = llanta.location.copy()
        # mismo punto en ejes del OBJ (Y arriba): (x, z, -y)
        info.write(f"{p} {c.x:.4f} {c.z:.4f} {-c.y:.4f}\n")
        exportar(pata, "Pata_" + p)
        llanta.location = (0, 0, 0)
        exportar(llanta, "Llanta_" + p)
        llanta.location = c
        # jerarquia en el .blend (despues de exportar y sin mover nada): Cuerpo -> Pata -> Llanta
        for hijo, padre in ((pata, cuerpo), (llanta, pata)):
            mundo = hijo.matrix_world.copy()
            hijo.parent = padre
            hijo.matrix_world = mundo
bpy.ops.wm.save_as_mainfile(filepath=blend_salida)
print("listo")
