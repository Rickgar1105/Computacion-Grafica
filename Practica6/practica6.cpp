/* Practica 6: Texturizado. Basada en Practica6271 del curso. */
#define STB_IMAGE_IMPLEMENTATION
#include "CommonValues.h"
#include <cstdio>
#include <cmath>
#include <vector>
#include <fstream>
#include <filesystem>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
std::vector<MeshModel*> meshListModel;
void CrearHolocron()
{
    // Cada cara tiene cuatro vertices independientes: una arista puede tener dos UV.
    const glm::vec3 caras[6][4] = {
        {{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}},
        {{1,-1,-1},{-1,-1,-1},{-1,1,-1},{1,1,-1}},
        {{1,-1,1},{1,-1,-1},{1,1,-1},{1,1,1}},
        {{-1,-1,-1},{-1,-1,1},{-1,1,1},{-1,1,-1}},
        {{-1,1,1},{1,1,1},{1,1,-1},{-1,1,-1}},
        {{-1,-1,-1},{1,-1,-1},{1,-1,1},{-1,-1,1}}
    };
    std::vector<GLfloat> vertices;
    std::vector<unsigned int> indices;
    for (unsigned int c=0;c<6;++c) {
        float u0=0, u1=1, v0=0, v1=1;
        glm::vec2 uv[4]={{u0,v0},{u1,v0},{u1,v1},{u0,v1}};
        glm::vec3 n=glm::normalize(glm::cross(caras[c][1]-caras[c][0],caras[c][2]-caras[c][0]));
        for(int j=0;j<4;++j) vertices.insert(vertices.end(),{caras[c][j].x,caras[c][j].y,caras[c][j].z,uv[j].x,uv[j].y,n.x,n.y,n.z});
        for(unsigned int k: {0u,1u,2u,2u,3u,0u}) indices.push_back(k);
        MeshModel* cara = new MeshModel();
        cara->CreateMeshModel(vertices.data(),indices.data(),vertices.size(),indices.size());
        meshListModel.push_back(cara); vertices.clear(); indices.clear();
    }

}


int main(int argc,char** argv) {
#ifdef __APPLE__
    // Permite abrir desde Finder sin depender del directorio de trabajo.
    char executable[4096]; uint32_t size=sizeof(executable);
    if(_NSGetExecutablePath(executable,&size)==0 && !std::filesystem::exists("shaders/shader_texture.vert")) {
        auto base=std::filesystem::weakly_canonical(executable).parent_path();
        if(std::filesystem::exists(base/"shaders")) std::filesystem::current_path(base);
        else if(std::filesystem::exists(base/"../Resources/shaders")) std::filesystem::current_path(base/"../Resources");
    }
#endif
    freopen("registro.txt","w",stdout);setbuf(stdout,nullptr);
    Window window(1366,768);
    if(window.Initialise()!=0) return 1;
    printf("OpenGL %s / GLSL %s\n",glGetString(GL_VERSION),glGetString(GL_SHADING_LANGUAGE_VERSION));
    // GLEW puede dejar un error de enumeracion en perfiles Core; limpiar antes de verificar la escena.
    while(glGetError()!=GL_NO_ERROR) {}
    Shader shader;
    shader.CreateFromFiles("shaders/shader_texture.vert","shaders/shader_texture.frag");
    CrearHolocron();
    Texture floor("Textures/piso.tga");
    std::vector<Texture*> caras;
    for(int i=1;i<=6;++i) {
        auto t=new Texture(("Textures/cara_"+std::to_string(i)+".png").c_str());
        if(!t->LoadTextureA(GL_CLAMP_TO_EDGE)) return 2; caras.push_back(t);
    }
    if(!floor.LoadTextureA()) return 2;
    Model imported,plane;
    imported.LoadModel("Models/holocron_modelado.obj");plane.LoadModel("Models/avion_verde.obj");
    GLfloat floorV[]={-25,0,-25,0,0,0,1,0, 25,0,-25,12,0,0,1,0, 25,0,25,12,12,0,1,0, -25,0,25,0,12,0,1,0};
    unsigned int floorI[]={0,2,1,0,3,2};MeshModel ground;ground.CreateMeshModel(floorV,floorI,32,6);
    Camera camera(glm::vec3(0,7,18),glm::vec3(0,1,0),-90,-16,6,.1);
    float last=glfwGetTime(),angle=25;bool rotate=false;int view=4;
    while(!window.getShouldClose()) {
        float now=glfwGetTime(),dt=std::min(now-last,.05f);last=now;
        glfwPollEvents();auto keys=window.getsKeys();
        int selected=keys[GLFW_KEY_1]?1:keys[GLFW_KEY_2]?2:keys[GLFW_KEY_3]?3:keys[GLFW_KEY_4]?4:0;
        if(selected) {
            view=selected; keys[GLFW_KEY_1]=keys[GLFW_KEY_2]=keys[GLFW_KEY_3]=keys[GLFW_KEY_4]=false;
            if(view==1)camera=Camera(glm::vec3(-7,3.6,7),glm::vec3(0,1,0),-90,-15,6,.1);
            if(view==2)camera=Camera(glm::vec3(-3.5,3.6,7),glm::vec3(0,1,0),-90,-15,6,.1);
            if(view==3)camera=Camera(glm::vec3(7,4.2,9),glm::vec3(0,1,0),-109,-13,6,.1);
            if(view==4)camera=Camera(glm::vec3(0,7,18),glm::vec3(0,1,0),-90,-16,6,.1);
        }
        if(keys[GLFW_KEY_R]) { rotate=!rotate; keys[GLFW_KEY_R]=false; }if(rotate)angle+=dt*18;
        camera.keyControl(keys,dt);camera.mouseControl(window.getXChange(),window.getYChange());
        glClearColor(.10f,.15f,.20f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        shader.UseShader();auto m=shader.GetModelLocation(),c=shader.getColorLocation();
        auto proj=glm::perspective(glm::radians(45.f),float(window.getBufferWidth())/window.getBufferHeight(),.1f,200.f);
        auto look=camera.calculateViewMatrix();
        glUniformMatrix4fv(shader.GetProjectionLocation(),1,GL_FALSE,glm::value_ptr(proj));
        glUniformMatrix4fv(shader.GetViewLocation(),1,GL_FALSE,glm::value_ptr(look));
        auto setModel=[&](glm::mat4 model){glUniformMatrix4fv(m,1,GL_FALSE,glm::value_ptr(model));};
        setModel(glm::mat4(1));glUniform3f(c,.6,.65,.7);floor.UseTexture();ground.RenderMeshModel();
        auto cube=glm::translate(glm::mat4(1),glm::vec3(-7,1.3,0));
        cube=glm::rotate(cube,glm::radians(angle),glm::vec3(0,1,0));
        setModel(cube);glUniform3f(c,1,1,1);for(int i=0;i<6;++i){caras[i]->UseTexture();meshListModel[i]->RenderMeshModel();}
        auto second=glm::translate(glm::mat4(1),glm::vec3(-3.5,1.3,0));
        second=glm::rotate(second,glm::radians(-angle),glm::vec3(0,1,0));
        setModel(second);imported.RenderModel(c);
        auto air=glm::translate(glm::mat4(1),glm::vec3(4,1.8,0));
        air=glm::rotate(air,glm::radians(-20.f),glm::vec3(0,1,0));
        air=glm::scale(air,glm::vec3(.75));setModel(air);plane.RenderModel(c);
        if((argc>1&&std::string(argv[1])=="--captura")||keys[GLFW_KEY_F12]) {
            int w=window.getBufferWidth(),h=window.getBufferHeight();std::vector<unsigned char> pix(w*h*4);
            glReadPixels(0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,pix.data());
            std::string name=argc>2?argv[2]:"captura_"+std::to_string(view)+".ppm";
            std::ofstream out(name,std::ios::binary);out<<"P6\n"<<w<<" "<<h<<"\n255\n";
            for(int y=h-1;y>=0;--y)for(int x=0;x<w;++x)out.write(reinterpret_cast<char*>(&pix[(y*w+x)*4]),3);
            printf("Captura %s %dx%d; error OpenGL: %u\n",name.c_str(),w,h,glGetError());
            keys[GLFW_KEY_F12]=false;if(argc>1)break;
        }
        window.swapBuffers();
    }
    imported.ClearModel();plane.ClearModel();
    for(auto mesh:meshListModel)delete mesh;
    for(auto texture:caras)delete texture;
    return 0;
}
