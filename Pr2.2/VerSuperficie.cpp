/* PRACTICA DE DIBUJO DE UNA SUPERFICIE BICÚBICA DE BEZIER
R.Vivó, J.Lluch para GPC.etsinf.upv.es 2011				   */

#include <GL/glut.h>
#include <GL/glui.h>
#include <stdio.h>
#include <time.h>
#include "SuperficieBezier.h"
#include "Primitivas.h"

#define PCONT 4				//Número de puntos de control
#define NVERT 10			//Número de vértices

#define EXIT	100
#define ON		200
#define OFF		201
#define VEL1	301
#define VEL2	302
#define VEL3	303
#define MALLA	400

int   last_x, last_y;		//Movimiento del ratón
float rotationX = 0.0, rotationY = 0.0;

SuperficieBezier S;			//Los puntos de dan por filas manteniendo la U y variando la V
Punto pcS[16]= {
		 Punto(1.5, 0.0, 1.0), Punto(1.5, 1.5, 1.0), Punto(-1.5, 1.5, 1.0), Punto(-1.5, 0.0, 1.0),
		 Punto(0.5, 0.0, 0.5), Punto(0.5, 0.5, 0.5), Punto(-0.5, 0.5, 0.5), Punto(-0.5, 0.0, 0.5),
		 Punto(0.5, 0.0,-0.5), Punto(0.5, 0.5,-0.5), Punto(-0.5, 0.5,-0.5), Punto(-0.5, 0.0,-0.5),
		 Punto(1.5, 0.0,-1.0), Punto(1.5, 1.5,-1.0), Punto(-1.5, 1.5,-1.0), Punto(-1.5, 0.0,-1.0)};

SuperficieBezier T;
Punto pcT[16]= {
		 Punto(1.5, 0.0,-1.0), Punto(1.5,-1.5,-1.0), Punto(-1.5,-1.5,-1.0), Punto(-1.5, 0.0,-1.0),
		 Punto(0.5, 0.0,-0.5), Punto(0.5,-0.5,-0.5), Punto(-0.5,-0.5,-0.5), Punto(-0.5, 0.0,-0.5),
		 Punto(0.5, 0.0, 0.5), Punto(0.5,-0.5, 0.5), Punto(-0.5,-0.5, 0.5), Punto(-0.5, 0.0, 0.5),
		 Punto(1.5, 0.0, 1.0), Punto(1.5,-1.5, 1.0), Punto(-1.5,-1.5, 1.0), Punto(-1.5, 0.0, 1.0)};

Punto sb[NVERT*NVERT];		//Malla de puntos de la superficie
Vector tgU[NVERT*NVERT];	//Tangentes en u
Vector tgV[NVERT*NVERT];	//Tangentes en v
Vector n[NVERT*NVERT];		//Normales

// Animacion
//time_t ini = time(NULL), secs;
bool animar = false,
	 aniRev = false;
int numPasos = 1,
	maxPasos = 50;
int velocidad = 2;

// Colores
float colores[8][4] =  {{1,0,0,1},  // Rojo
						{0,1,0,1},  // Verde
						{0,0,1,1},  // Azul
						{1,1,0,1},  // Amarillo 
						{1,0,1,1},  // Magenta
						{0,1,1,1},  // Cyan
						{0,0,0,1},  // Negro						
						{1,1,1,1}}; // Blanco
int colorS = 7,
	colorT = 6;

// Opciones
bool verMalla = false;


void myInit(void);
void animacion(void);
void botonRaton(int boton, int estado, int x, int y );
void movRaton(int x, int y );
void teclaTeclado (unsigned char key, int x, int y);
void drawAxis(Transformacion const &t);
void drawMalla(Punto p[], Transformacion t);
void dibujarSuperficie(SuperficieBezier S, int color);
void color_s(const int color);
void color_t(const int color);
void menu(const int value);
void crearMenu();
void display(void);
void myReshape (GLsizei w, GLsizei h);
void onIdle(void);

void myinit(void){			//Función de inicialización
	
	// Inicialización de iluminación
	GLfloat global_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 20.0 };
	GLfloat light_position[] = { 2.0, 10.0, 2.0, 0.0 };
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	// Habilitar modelo de iluminación
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

    glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);

	S = SuperficieBezier(pcS);		//Crear superficies
	T = SuperficieBezier(pcT);

	crearMenu();
}

void animacion(){

	float var = (!aniRev) ? 1.0/maxPasos : -1.0/maxPasos;
	var *= velocidad;

	for(int i=0; i<PCONT*PCONT; i++){
		switch(i){								//Movimientos en X
			case 0: case 1: case 12: case 13:
			case 6: case 7: case 10: case 11:
				pcS[i].setx(pcS[i].x() - var);
				pcT[i].setx(pcT[i].x() - var); break;
			case 2: case 3: case 14: case 15:
			case 4: case 5: case  8: case  9:
				pcS[i].setx(pcS[i].x() + var);
				pcT[i].setx(pcT[i].x() + var); break;
			default: break;
		}
		switch(i){								//Movimientos en Y
			case 1: case 2: case 13: case 14:
				pcS[i].sety(pcS[i].y() - var);
				pcT[i].sety(pcT[i].y() + var); break;
			case 5: case 6: case  9: case 10:
				pcS[i].sety(pcS[i].y() + var);
				pcT[i].sety(pcT[i].y() - var); break;
			default: break;
		}
	}
	if(!aniRev)
		numPasos += velocidad;
	else
		numPasos -= velocidad;
	if(numPasos >= maxPasos || numPasos <= 1) 
		aniRev = !aniRev;

	glutPostRedisplay();
}

void botonRaton(int boton, int estado, int x, int y ){

	// Callback cuando pulsamos los botonos del ratón
	if (boton==GLUT_LEFT_BUTTON && estado==GLUT_DOWN){
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

void teclaTeclado (unsigned char key, int x, int y){

	switch(key){
		case 'a': case 'A': animar = !animar; break;
		case 'm': case 'M': verMalla = !verMalla; break;
		case 'q': case 'Q': exit(0); break;
		default: break;
	}
	crearMenu();
	glutPostRedisplay();
}

void drawAxis(Transformacion const &t){

	glLineWidth(1.0);
	Punto o = Punto().transform(t);
	Punto x = Punto(1,0,0).transform(t);
	Punto y = Punto(0,1,0).transform(t);
	Punto z = Punto(0,0,1).transform(t);

	float r[] = {1, 0, 0, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, r);
	Flecha(x-o, Color::ROJO ).drawAt(o);
	float g[] = {0, 1, 0, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, g);
	Flecha(y-o, Color::VERDE).drawAt(o);
	float b[] = {0, 0, 1, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, b);
	Flecha(z-o, Color::AZUL ).drawAt(o);
}

void drawMalla(Punto p[], Transformacion t){

	//Dibujo de la malla característica
	glColor3f(0.0, 0.0, 0.5);
	float mcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);
	glLineWidth(1.0);
	for(int i=0; i<PCONT; i++){
		glBegin(GL_LINE_STRIP);
		for(int j=0; j<PCONT*PCONT; j+=PCONT){
			glVertex3f(p[i+j].transform(t).x(), 
					   p[i+j].transform(t).y(), 
					   p[i+j].transform(t).z());
		}
		glEnd();
	}
	for(int i=0; i<PCONT*PCONT; i+=PCONT){
		glBegin(GL_LINE_STRIP);
		for(int j=0; j<PCONT; j++){
			glVertex3f(p[i+j].transform(t).x(), 
					   p[i+j].transform(t).y(), 
					   p[i+j].transform(t).z());
		}
		glEnd();
	}
}

void dibujarSuperficie(SuperficieBezier S, int color){

	//Calcula la superficie transformada y sus vectores 
	S.getPoints(NVERT, sb);
	S.getTangents(NVERT, tgU, tgV);
	S.getNormals(NVERT, n); 

	//Dibujo en sólido
	//float mcolor[] = {1.0f, 0.0f, 0.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colores[color]);
	for(int i=0; i<NVERT-1; i++){
		for(int j=0; j<NVERT-1; j++){
			int pos = i*NVERT+j;
			glBegin(GL_QUADS);
				glNormal3f( n[pos].x(),  n[pos].y(),  n[pos].z());
				glVertex3f(sb[pos].x(), sb[pos].y(), sb[pos].z());
				glNormal3f( n[pos+1].x(),  n[pos+1].y(),  n[pos+1].z());
				glVertex3f(sb[pos+1].x(), sb[pos+1].y(), sb[pos+1].z());
				glNormal3f( n[pos+NVERT+1].x(),  n[pos+NVERT+1].y(),  n[pos+NVERT+1].z());
				glVertex3f(sb[pos+NVERT+1].x(), sb[pos+NVERT+1].y(), sb[pos+NVERT+1].z());
				glNormal3f( n[pos+NVERT].x(),  n[pos+NVERT].y(),  n[pos+NVERT].z());
				glVertex3f(sb[pos+NVERT].x(), sb[pos+NVERT].y(), sb[pos+NVERT].z());
			glEnd();
		};
	};
	
	glLineWidth(1.0);					//Dibujo de las tangentes y normales	
	float rg[] = {1, 1, 0, 1};
	float gb[] = {0, 1, 1, 1};
	float rb[] = {1, 0, 1, 1};
	for (int i=0; i<NVERT; i++){
		for(int j=0; j<NVERT; j++){
			Flecha m;
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, rg);
				m = Flecha(tgU[i*NVERT+j]*0.1, Color(0.5,0.5,0.0));
				m.drawAt(sb[i*NVERT+j]);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gb);
				m = Flecha(tgV[i*NVERT+j]*0.1, Color(0.0,0.5,0.5));
				m.drawAt(sb[i*NVERT+j]);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, rb);
				m = Flecha(n[i*NVERT+j]*0.2, Color(1.0,0.0,0.0));
				m.drawAt(sb[i*NVERT+j]);
		};
	};
}

void color_s(const int color){

	colorS = color;
	glutPostRedisplay();
}

void color_t(const int color){

	colorT = color;
	glutPostRedisplay();
}

void menu(const int value){

	switch(value){
		case EXIT:	exit(1); break;
		case ON:	animar = true; break;
		case OFF:	animar = false; break;
		case VEL1:	velocidad = 1; break;
		case VEL2:	velocidad = 2; break;
		case VEL3:	velocidad = 3; break;
		case MALLA: verMalla = !verMalla; break;
	}
	crearMenu();
	glutPostRedisplay();
}

void crearMenu(){

	// Crear los menus
	int subm_color_s = glutCreateMenu(color_s);
		glutAddMenuEntry("Rojo", 0);
		glutAddMenuEntry("Verde", 1);
		glutAddMenuEntry("Azul", 2);
		glutAddMenuEntry("Amarillo", 3);
		glutAddMenuEntry("Magenta", 4);
		glutAddMenuEntry("Cyan", 5);
		glutAddMenuEntry("Negro", 6);
		glutAddMenuEntry("Blanco", 7);
	int subm_color_t = glutCreateMenu(color_t);
		glutAddMenuEntry("Rojo", 0);
		glutAddMenuEntry("Verde", 1);
		glutAddMenuEntry("Azul", 2);
		glutAddMenuEntry("Amarillo", 3);
		glutAddMenuEntry("Magenta", 4);
		glutAddMenuEntry("Cyan", 5);
		glutAddMenuEntry("Negro", 6);
		glutAddMenuEntry("Blanco", 7);
	int subm_color = glutCreateMenu(menu);
		glutAddSubMenu("Superficie 1", subm_color_s);
		glutAddSubMenu("Superficie 2", subm_color_t);
	int subm_veloc = glutCreateMenu(menu);
		glutAddMenuEntry("Lenta", VEL1);
		glutAddMenuEntry("Media", VEL2);
		glutAddMenuEntry("Rápida",VEL3);
	int mainmenu = glutCreateMenu(menu);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
		if(animar)
			glutAddMenuEntry("Animación OFF", OFF);
		else
			glutAddMenuEntry("Animación ON", ON);
		glutAddSubMenu("Velocidad de animación", subm_veloc);
		glutAddSubMenu("Cambiar colores", subm_color);
		if(verMalla)
			glutAddMenuEntry("Ocultar malla", MALLA);
		else
			glutAddMenuEntry("Mostrar malla", MALLA);
		glutAddMenuEntry("Salir", EXIT);
}

void display(void){
//Función de dibujo

	Transformacion t;
	t.reset();
	t.translation(Punto(0,0,-5.0));			//Desplaza la cámara hacia atrás
	t.rotX( 40.0 - rotationX);				//Gira la superficie para verla desde arriba
	t.rotY(-40.0 + rotationY);				// atendiendo a la interacción con el ratón
	
	//Aplica la transformación a los puntos de control
	for(int i=0; i<PCONT; i++){
		for(int j=0; j<PCONT; j++){
			S.setPoint(i, j, pcS[i*PCONT+j].transform(t));
			T.setPoint(i, j, pcT[i*PCONT+j].transform(t));
		};
	};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Dibujar ejes
	drawAxis(t);

	//Dibujar curvas
	dibujarSuperficie(S, colorS);
	dibujarSuperficie(T, colorT);

	if(verMalla){
		drawMalla(pcS, t);
		drawMalla(pcT, t);
	}

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
	gluPerspective(50.0, (GLfloat) w/(GLfloat) h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void onIdle(void){

	if(animar) 
		animacion();
}

int main(int argc, char** argv){

	//Incializaciones
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500); 
	glutInitWindowPosition(100, 100);
	//Creación de la ventana
	glutCreateWindow("Superficie de Bezier");
	myinit();
	//Registro CallBacks
	glutDisplayFunc(display); 
	glutReshapeFunc(myReshape);
	glutMotionFunc(movRaton);
	glutMouseFunc(botonRaton);
	glutKeyboardFunc(teclaTeclado);
	glutIdleFunc(onIdle);
	//Puesta en marcha
	glutMainLoop();

	return(0);
}

