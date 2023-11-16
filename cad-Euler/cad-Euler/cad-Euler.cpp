#include <GL/glut.h>
#include "Body_Product.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
using namespace std;
#ifndef _WIN32
#define __stdcall
#endif

#ifndef CALLBACK
#define CALLBACK
#endif

void CALLBACK tessBeginCB(GLenum which);
void CALLBACK tessEndCB();
void CALLBACK tessErrorCB(GLenum errorCode);
void CALLBACK tessVertexCB(const GLvoid* data);
void CALLBACK tessVertexCB2(const GLvoid* data);

void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void idleCB();
void mouseCB(int button, int stat, int x, int y);
void mouseMotionCB(int x, int y);
void initGL();
int  initGLUT(int argc, char** argv);
bool initSharedMem();
void clearSharedMem();
void initLights();
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
GLuint draw_Solid(Solid* s);

bool mouseLeftDown;
bool mouseRightDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
GLuint list;       

int main(int argc, char** argv)
{
    Solid* s = nullptr;
    //构建实体的B-rep和扫掠过程
    s = product();
    initSharedMem();
    initGLUT(argc, argv);
    initGL();
    //渲染
    list = draw_Solid(s);
    glutMainLoop(); 
    return 0;
}

GLuint draw_Solid(Solid* s) {
    GLuint id = glGenLists(1); 
    if (!id) return id;          
    if (s == nullptr)
        return id;
    GLUtesselator* tess = gluNewTess(); 
    if (!tess) return 0;  
    gluTessCallback(tess, GLU_TESS_BEGIN, (void(__stdcall*)(void))tessBeginCB);
    gluTessCallback(tess, GLU_TESS_END, (void(__stdcall*)(void))tessEndCB);
    gluTessCallback(tess, GLU_TESS_ERROR, (void(__stdcall*)(void))tessErrorCB);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void(__stdcall*)(void))tessVertexCB2);
    glNewList(id, GL_COMPILE);
    for (s; s->prevs != nullptr; s = s->prevs);
    while (s) {
        Face* f = s->sface;
        while (f) {
            gluTessBeginPolygon(tess, 0);
            Loop* lp = f->floop;
            while (lp) {
                gluTessBeginContour(tess);
                HalfEdge* he = lp->ledg;
                Vertex* first_v = he->startv;
                int p_id = m[he->startv->x][he->startv->y][he->startv->z];
                gluTessVertex(tess, mem_point[p_id], mem_point[p_id]);
                he = he->next;
                while (he->startv != first_v) {
                    p_id = m[he->startv->x][he->startv->y][he->startv->z];
                    gluTessVertex(tess, mem_point[p_id], mem_point[p_id]);
                    he = he->next;
                }
                gluTessEndContour(tess);
                lp = lp->nextl;
            }
            f = f->nextf;
            gluTessEndPolygon(tess);
        }
        s = s->nexts;
    }
    glEndList();
    gluDeleteTess(tess);       

    return id;
}





int initGLUT(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);  
    glutInitWindowSize(600, 400);               
    glutInitWindowPosition(100, 100);           
    int handle = glutCreateWindow(argv[0]);     
    glutDisplayFunc(displayCB);
    glutTimerFunc(33, timerCB, 33);            
    glutReshapeFunc(reshapeCB);
    glutMouseFunc(mouseCB);
    glutMotionFunc(mouseMotionCB);
    return handle;
}

void initGL()
{
    glShadeModel(GL_SMOOTH);                   
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0, 0, 0, 0);                   
    glClearStencil(0);                         
    glClearDepth(1.0f);                         
    glDepthFunc(GL_LEQUAL);
    initLights();
    setCamera(0, 0, 5, 0, 0, 0);
}

bool initSharedMem()
{
    mouseLeftDown = mouseRightDown = false;
    return true;
}

void clearSharedMem()
{
}


void initLights()
{
    GLfloat lightKa[] = { .2f, .2f, .2f, 1.0f };  
    GLfloat lightKd[] = { .7f, .7f, .7f, 1.0f };  
    GLfloat lightKs[] = { 1, 1, 1, 1 };           
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);
    float lightPos[4] = { 0, 0, 20, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);                        
}

void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); 
}

void displayCB()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(0, 0, cameraDistance);
    glRotatef(cameraAngleX, 1, 0, 0);  
    glRotatef(cameraAngleY, 0, 1, 0);   
    glCallList(list);
    glPopMatrix();
    glutSwapBuffers();
}

void reshapeCB(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    float aspectRatio = (float)w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)(w) / h, 1.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void timerCB(int millisec)
{
    glutTimerFunc(millisec, timerCB, millisec);
    glutPostRedisplay();
}

void idleCB()
{
    glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;

    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if (state == GLUT_UP)
            mouseLeftDown = false;
    }

    else if (button == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if (state == GLUT_UP)
            mouseRightDown = false;
    }
}

void mouseMotionCB(int x, int y)
{
    if (mouseLeftDown)
    {
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    if (mouseRightDown)
    {
        cameraDistance += (y - mouseY) * 0.2f;
        mouseY = y;
    }


}

void CALLBACK tessBeginCB(GLenum which)
{
    glBegin(which);

}

void CALLBACK tessEndCB()
{
    glEnd();
}

void CALLBACK tessVertexCB(const GLvoid* data)
{
    const GLdouble* ptr = (const GLdouble*)data;

    glVertex3dv(ptr);

}

void CALLBACK tessVertexCB2(const GLvoid* data)
{
    const GLdouble* ptr = (const GLdouble*)data;
    glColor3dv(ptr + 3);
    glVertex3dv(ptr);
}

void CALLBACK tessErrorCB(GLenum errorCode)
{
    const GLubyte* errorStr;
    errorStr = gluErrorString(errorCode);
}

