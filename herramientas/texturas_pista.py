# Práctica 9: texturas de la pista de carreras (pista.png) y de los muros de la parte elevada (pista_muro.png).
# Se generan con ruido para que el asfalto y el concreto no se vean planos. Uso: python3 texturas_pista.py
# (se ejecuta desde la carpeta herramientas y guarda las imágenes en ../Textures)
import os, random
from PIL import Image, ImageDraw, ImageFilter

random.seed(9)
DESTINO = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "Textures")


def ruido(ancho, alto, base, variacion):
    im = Image.new("RGB", (ancho, alto))
    px = im.load()
    for y in range(alto):
        for x in range(ancho):
            r = random.randint(-variacion, variacion)
            px[x, y] = tuple(max(0, min(255, c + r)) for c in base)
    return im.filter(ImageFilter.GaussianBlur(0.6))


# Pista: u (horizontal) va de una orilla a la otra y v (vertical) avanza sobre el circuito.
# La imagen se repite en v, así que todo lo que hay en ella debe cerrar entre el borde de arriba y el de abajo.
A, H = 256, 512
pista = ruido(A, H, (58, 60, 64), 14)
d = ImageDraw.Draw(pista)
BORDILLO = 18                   # ancho del bordillo rojo y blanco de cada orilla
for k in range(8):              # 8 bloques alternados en el alto de la imagen
    color = (200, 30, 30) if k % 2 == 0 else (235, 235, 235)
    y0, y1 = k * H // 8, (k + 1) * H // 8 - 1
    d.rectangle([0, y0, BORDILLO - 1, y1], fill=color)
    d.rectangle([A - BORDILLO, y0, A - 1, y1], fill=color)
d.rectangle([BORDILLO + 6, 0, BORDILLO + 11, H - 1], fill=(230, 230, 230))        # línea blanca de la orilla
d.rectangle([A - BORDILLO - 12, 0, A - BORDILLO - 7, H - 1], fill=(230, 230, 230))
for k in range(2):              # línea central amarilla punteada, 2 segmentos por repetición
    y0 = k * H // 2 + H // 8
    d.rectangle([A // 2 - 4, y0, A // 2 + 3, y0 + H // 4 - 1], fill=(235, 190, 40))
pista.convert("RGBA").save(os.path.join(DESTINO, "pista.png"))

# Muro: concreto con juntas verticales; v = 1 es la parte de arriba del tramo elevado, donde va una franja
# roja y blanca como la de los bordillos
muro = ruido(256, 256, (150, 148, 142), 18)
d = ImageDraw.Draw(muro)
for x in (0, 128):
    d.line([x, 0, x, 255], fill=(105, 103, 98), width=3)
for k in range(4):
    color = (200, 30, 30) if k % 2 == 0 else (235, 235, 235)
    d.rectangle([k * 64, 0, (k + 1) * 64 - 1, 30], fill=color)   # stb_image invierte la imagen: arriba es v = 1
muro.convert("RGBA").save(os.path.join(DESTINO, "pista_muro.png"))
print("texturas de la pista listas")
