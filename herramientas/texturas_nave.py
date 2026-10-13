# Genera las texturas de la nave (placas de metal, pintura naranja, metal oscuro, cabina)
# y la textura del fuego de plasma azul de los motores. Todas son repetibles (tileables).
import numpy as np, os, sys
from PIL import Image, ImageDraw, ImageFilter
rng=np.random.default_rng(8)
OUT=sys.argv[1]
S=512
def ruido(escala, s=S):
    r=rng.random((s//escala+2, s//escala+2))
    im=Image.fromarray((r*255).astype(np.uint8)).resize((s+2*escala, s+2*escala), Image.BICUBIC)
    return np.asarray(im.crop((escala,escala,escala+s,escala+s)),float)/255
def placas(base, var, linea, remaches=True, rayones=0, celdas=4):
    a=np.zeros((S,S,3))
    n=0.6*ruido(64)+0.3*ruido(16)+0.1*ruido(4)
    cep=np.asarray(Image.fromarray((rng.random((S,S))*255).astype(np.uint8)).filter(ImageFilter.BoxBlur(0)).resize((S,S)),float)/255
    cep=np.asarray(Image.fromarray((cep*255).astype(np.uint8)).resize((S//64,S),Image.BILINEAR).resize((S,S),Image.BILINEAR),float)/255
    for c in range(3): a[...,c]=base[c]+var*(n-0.5)+0.04*(cep-0.5)
    im=Image.fromarray(np.clip(a*255,0,255).astype(np.uint8)); d=ImageDraw.Draw(im)
    p=S//celdas
    for i in range(celdas):
        for j in range(celdas):
            # algunas placas un poco más claras u oscuras
            k=rng.uniform(-0.05,0.05)
            x0,y0=i*p,j*p
            sub=np.asarray(im.crop((x0,y0,x0+p,y0+p)),float)*(1+k)
            im.paste(Image.fromarray(np.clip(sub,0,255).astype(np.uint8)),(x0,y0))
    d=ImageDraw.Draw(im)
    for i in range(celdas+1):
        d.line([(i*p,0),(i*p,S)],fill=linea,width=3); d.line([(0,i*p),(S,i*p)],fill=linea,width=3)
        # división extra en algunas placas
    for i in range(celdas):
        for j in range(celdas):
            if rng.random()<0.35: d.line([(i*p+p//2,j*p),(i*p+p//2,j*p+p)],fill=linea,width=2)
    if remaches:
        for i in range(celdas):
            for j in range(celdas):
                for (u,v) in [(10,10),(p-10,10),(10,p-10),(p-10,p-10)]:
                    x,y=i*p+u,j*p+v
                    d.ellipse([x-3,y-3,x+3,y+3],fill=tuple(min(255,int(c*255*1.15)) for c in base),outline=linea)
    for _ in range(rayones):
        x,y=rng.integers(0,S,2); L=rng.integers(10,60); ang=rng.uniform(0,np.pi)
        d.line([(x,y),(x+L*np.cos(ang),y+L*np.sin(ang))],fill=(200,200,205),width=1)
    return im.filter(ImageFilter.SMOOTH)
placas((0.80,0.83,0.87),0.10,(110,115,125),rayones=40).save(os.path.join(OUT,"nave_metal.png"))
placas((0.93,0.55,0.16),0.12,(120,60,20),rayones=25,celdas=2).save(os.path.join(OUT,"nave_pintura.png"))
placas((0.55,0.58,0.64),0.12,(60,65,75),rayones=20).save(os.path.join(OUT,"nave_metal_oscuro.png"))
# cabina y toberas: vidrio oscuro con un reflejo diagonal
y,x=np.mgrid[0:S,0:S]/S
g=0.15+0.10*(1-y)+0.05*ruido(32)
refl=np.exp(-((x-y-0.1)**2)/0.004)*0.35
a=np.stack([g*0.8+refl,g*0.9+refl,g*1.25+refl],-1)
Image.fromarray(np.clip(a*255,0,255).astype(np.uint8)).save(os.path.join(OUT,"nave_cabina.png"))
# fuego de plasma: u a lo ancho de la llama, v a lo largo (v=0 en la tobera, v=1 en la punta)
W,H=256,512
v,u=np.mgrid[0:H,0:W]/np.array([H,W])[:,None,None] if False else (np.mgrid[0:H,0:W][0]/H, np.mgrid[0:H,0:W][1]/W)
largo=np.clip(1-v,0,1)
ancho=0.5*(0.35+0.65*largo)                 # la llama se angosta hacia la punta
dist=np.abs(u-0.5)/np.maximum(ancho,1e-3)
estr=np.asarray(Image.fromarray((rng.random((8,W))*255).astype(np.uint8)).resize((W,H),Image.BICUBIC),float)/255
nucleo=np.exp(-(dist/0.35)**2)
halo=np.exp(-(dist/0.9)**2)
intens=(0.75*halo+0.6*nucleo)*(largo**0.8)*(0.8+0.4*estr)
r=0.08*halo+0.55*nucleo; gch=0.3*halo+0.8*nucleo; b=np.ones_like(u)
borde=np.clip(1-2*np.abs(u-0.5),0,1)**1.5   # el borde del plano queda transparente
alpha=np.clip(intens*borde,0,1)
rgba=np.stack([np.clip(r,0,1),np.clip(gch,0,1),b,alpha],-1)
Image.fromarray((rgba*255).astype(np.uint8),"RGBA").filter(ImageFilter.GaussianBlur(1.5)).save(os.path.join(OUT,"fuego_plasma.png"))
print("ok")
