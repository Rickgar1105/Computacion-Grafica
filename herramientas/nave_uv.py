# Agrega coordenadas UV a craft_speederA.obj (Kenney Space Kit, CC0) con proyección por caja:
# cada cara toma las dos coordenadas del plano al que más se parece (según su normal),
# así las texturas repetibles de placas quedan sin estirarse. Cada material usa su propia textura.
import sys, numpy as np
src, dst_obj, dst_mtl = sys.argv[1:4]
V=[];VN=[];out=[];vts=[]
tex={"metal":"nave_metal.png","metalRed":"nave_pintura.png","metalDark":"nave_metal_oscuro.png","dark":"nave_cabina.png"}
for l in open(src):
    t=l.split()
    if not t: continue
    if t[0]=="v": V.append(np.array(list(map(float,t[1:4]))))
    elif t[0]=="vn": VN.append(l.strip())
for l in open(src):
    t=l.split()
    if not t or t[0] in ("v","vn","vt","mtllib","#"): continue
    if t[0]=="f":
        idx=[x.split("/") for x in t[1:]]
        P=[V[int(a[0])-1] for a in idx]
        n=np.cross(P[1]-P[0],P[2]-P[0]); ax=int(np.argmax(np.abs(n)))
        partes=[]
        for a,p in zip(idx,P):
            if ax==0: uv=(p[2],p[1])
            elif ax==1: uv=(p[0],p[2])
            else: uv=(p[0],p[1])
            vts.append(uv); vt=len(vts)
            vn=a[2] if len(a)>2 and a[2] else ""
            partes.append(f"{a[0]}/{vt}/{vn}" if vn else f"{a[0]}/{vt}")
        out.append("f "+" ".join(partes))
    else: out.append(l.strip())
with open(dst_obj,"w") as f:
    f.write("# Nave craft_speederA de Kenney (Space Kit, CC0) con coordenadas UV por proyeccion de caja\n")
    f.write("mtllib "+dst_mtl.split("/")[-1]+"\n")
    for v in V: f.write("v %.6f %.6f %.6f\n"%tuple(v))
    for uv in vts: f.write("vt %.4f %.4f\n"%uv)
    for vn in VN: f.write(vn+"\n")
    for l in out: f.write(l+"\n")
with open(dst_mtl,"w") as f:
    for m,t in tex.items(): f.write(f"newmtl {m}\nKd 1 1 1\nmap_Kd ../Textures/{t}\n\n")
print(len(V),"vertices",len(vts),"uv",len(out))
