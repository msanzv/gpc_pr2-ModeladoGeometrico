/* PRACTICA DE DIBUJO DE UNA SUPERFICIE BICÚBICA DE BEZIER
R.Vivó, J.Lluch para GPC.etsinf.upv.es 2011				   */

#include <GL/glut.h>
#include <stdio.h>

#include "SuperficieBezier.h"
#include "Primitivas.h"

#define PCONT 4	//Número de vértices
#define NVERT 10	//Número de vértices

//Movimiento del ratón
int   last_x, last_y;
float rotationX = 0.0, rotationY = 0.0;

SuperficieBezier S;
//Los puntos de dan por filas manteniendo la U y variando la V
Punto pcontrol[16]= {
		 Punto(1.0,0.0,1.0),  Punto(0.5,1.0,1.0),  Punto(-0.5,1.0,1.0),  Punto(-1.0,0.0,1.0),
		 Punto(1.0,0.0,0.5),  Punto(0.5,0.0,0.5),  Punto(-0.5,0.0,0.5),  Punto(-1.0,0.0,0.5),
		 Punto(1.0,0.0,-0.5), Punto(0.5,0.0,-0.5), Punto(-0.5,0.0,-0.5), Punto(-1.0,0.0,-0.5),
		 Punto(1.0,0.0,-1.0), Punto(0.5,1.0,-1.0), Punto(-0.5,1.0,-1.0), Punto(-1.0,0.0,-1.0)};
Punto sb[NVERT*NVERT];	//Malla de puntos de la superficie
Vector tgU[NVERT*NVERT];	//Tangentes en u
Vector tgV[NVERT*NVERT];	//Tangentes en v
Vector n[NVERT*NVERT];	//Normales

void myinit(void){
//Función de inicialización

    glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0,0.0,0.0);

	S = SuperficieBezier(pcontrol);		//Nueva superficie
}
void botonRaton(int boton, int estado, int x, int y ){

	// Callback cuando pulsamos los botonos del ratón
	if ( boton == GLUT_LEFT_BUTTON && estado == GLUT_DOWN ) {
		last_x = x;
		last_y = y;
	}
}

void movRaton(int x, int y ){

	// Callback del movimiento del raton, varia el ángulo desde donde presentamos el dibujo
	rotationX += (float) (last_y - y); //La y decrece hacia arriba
	rotationY += (float) (x - last_x);

	last_x = x;
	last_y = y;

	glutPostRedisplay(); 
}

void drawAxis(Transformacion const &t){

	Punto o=Punto().transform(t);
	Punto x=Punto(1,0,0).transform(t);
	Punto y=Punto(0,1,0).transform(t);
	Punto z=Punto(0,0,1).transform(t);
	Flecha(x-o,Color::ROJO).drawAt(o);
	Flecha(y-o,Color::VERDE).drawAt(o);
	Flecha(z-o,Color::AZUL).drawAt(o);
}

void display(void){
//Función de dibujo

	int i,j;
	Transformacion t;
	t.reset();
	t.translation(Punto(0,0,-5.0));		//Desplaza la cámara hacia atrás
	t.rotX(40.0-rotationX);				//Gira la superficie para verla desde arriba
	t.rotY(-40.0+rotationY);			// atendiendo a la interacción con el ratón
	
	//Aplica la transformación a los puntos de control
	for(i=0; i<PCONT; i++){
		for(j=0; j<PCONT; j++){
			S.setPoint(i, j, pcontrol[i*PCONT+j].transform(t));
		};
	};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Dibujar ejes
	drawAxis(t);

	//Calcula la superficie transformada y sus vectores 
	S.getPoints(NVERT, sb);
	S.getTangents(NVERT, tgU, tgV);
	S.getNormals(NVERT, n); 

	//Dibujo en alámbrico
	glColor3f(0.0,0.0,0.0);
	for(i=0; i<NVERT; i++){	//u cte
		glBegin(GL_LINE_STRIP);
		for(j=0; j<NVERT; j++){
			glVertex3f( (GLfloat)((sb[i*NVERT+j]).x()),
						(GLfloat)((sb[i*NVERT+j]).y()),
						(GLfloat)((sb[i*NVERT+j]).z()));
		};
		glEnd();
	};
 	for(i=0; i<NVERT; i++){	//v cte
		glBegin(GL_LINE_STRIP);
		for(j=0; j<NVERT; j++){
			glVertex3f( (GLfloat)((sb[i+j*NVERT]).x()),
						(GLfloat)((sb[i+j*NVERT]).y()),
						(GLfloat)((sb[i+j*NVERT]).z()));
		};
		glEnd();
	};	
	
	//Dibujo de las tangentes y normales
	for (i=0; i<NVERT; i++){
		for(j=0; j<NVERT; j++){
			Flecha m;
			m = Flecha(tgU[i*NVERT+j]*0.1, Color(0.5,0.5,0.0));
			m.drawAt(sb[i*NVERT+j]);
			m = Flecha(tgV[i*NVERT+j]*0.1, Color(0.0,0.5,0.5));
			m.drawAt(sb[i*NVERT+j]);
			m = Flecha(n[i*NVERT+j]*0.2, Color(1.0,0.0,0.0));
			m.drawAt(sb[i*NVERT+j]);
		};
	};

	glFlush();
}

void myReshape(GLsizei w, GLsizei h)
//Función de redimensión del marco y selección de proyección
{
    h = (h == 0) ? 1 : h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	//Cámara perspectiva en el origen con fov 40º y cerca=1 lejos=200
	gluPerspective(40.0, (GLfloat) w/(GLfloat) h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
	//Incializaciones
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB );
	glutInitWindowSize (500, 500); 
	glutInitWindowPosition (100, 100);
	//Creación de la ventana
	glutCreateWindow ("Superficie de Bezier");
	myinit();
	//Registro CallBacks
	glutDisplayFunc(display); 
	glutReshapeFunc(myReshape);
	glutMotionFunc(movRaton);
	glutMouseFunc(botonRaton);
	//Puesta en marcha
	glutMainLoop();
	return(0);
}

