/* PRACTICA DE DIBUJO DE UNA CURVA CUBICA DE BEZIER
R.Vivó, J.Lluch para GPC.etsinf.upv.es 2011      */

//Ficheros a incluir para el uso de la OpenGL
#include <GL/glut.h>

//Fichero de definicion de la Curva de Bezier
#include "CurvaBezier.h"
#include "Primitivas.h"

//Prototipo de las funciones usadas más abajo
void myinit(void);
void myReshape(GLsizei w, GLsizei h);
void display(void);

//Definiciones
#define NVERT 20	//Número de vértices que se quiere tenga la polilínea

//Variables globales
CurvaBezier Q;						//La curva que vamos a ver
Punto G[4]={Punto( -3.0, 0.0, 0.0), //Vector geométrico de puntos de control P0,P1,P2,P3
			Punto( -2.0,-3.0,-4.0),
			Punto(	2.0, 3.0, 4.0),
			Punto(	3.0, 0.0, 0.0) };			

Punto cb[NVERT];			//El vector de puntos que representa la curva
Vector tb[NVERT];			//El vector de tangentes en cada punto

//Auxiliares
Flecha v;

void myinit(void)
//Inicialización de variables de la GL y globales
{
    glClearColor (1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0,0.0,0.0);
	Q= CurvaBezier(	G );	//Define la curva 
}

void display(void)
//Función de respuesta a la acción de mostrar el contenido de la ventana
{
	Transformacion t;						//Acumula las transformaciones aplicadas
	int j;
	float current;

	glClear(GL_COLOR_BUFFER_BIT);			//Borra la ventana

	//Para dibujar la curva aplicaremos la transformacion a los puntos
	//de control, calcularemos los puntos sobre la curva y la dibujaremos como polilínea

	t.reset();
	t.translation(Vector(0,0,-7));						//Mueve la camara hacia atrás
	t.rotY(20);											//Gira un poco la curva en Y

	for(j=0;j<4;j++) 
		Q.setPoint(j,G[j].transform(t));	//Transforma los puntos de control
	
	Q.getPoints(NVERT, cb);					//Cálculo de vértices
	Q.getTangents(NVERT, tb);				//Cálculo de tangentes

	glGetFloatv(GL_LINE_WIDTH, &current);
	glLineWidth(2.0);
	glColor3f(0.0,0.0,0.0);

	//Dibujo de la curva
	glBegin(GL_LINE_STRIP);					//Uso de la polilinea de GL
		for(j=0;j<NVERT;j++){
			glVertex3f( (GLfloat)(cb[j].x()),
						(GLfloat)(cb[j].y()),
						(GLfloat)(cb[j].z()));
		};
	glEnd();
	glLineWidth(current);

	//Dibujo de las tangentes
	for(j=0;j<NVERT;j++){
			v= Flecha(tb[j],Color(1.0,0.0,0.0));
			v.drawAt(cb[j]);
	};

	glFlush();								//Descarga los buffers
}

void myReshape(GLsizei w, GLsizei h)
//Función de respuesta a la acción de cambio de tamaño de la ventana
{
	//Inicialización de la cámara de GL
    h = (h == 0) ? 1 : h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
//Programa principal
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB );
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Curva de Bezier TEST");
   myinit();
   glutDisplayFunc(display); 
   glutReshapeFunc(myReshape);
   glutMainLoop();
   return(0);
}
