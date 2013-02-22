/* PRACTICA DE DIBUJO DE UNA CURVA CUBICA DE BEZIER
R.Vivó, J.Lluch para GPC.etsinf.upv.es 2011      */

//Ficheros a incluir para el uso de la OpenGL
#include <GL/glut.h>
#include <GL/glui.h>
#include <stdio.h>

//Fichero de definicion de la Curva de Bezier
#include "CurvaBezier.h"
#include "Primitivas.h"

//Prototipo de las funciones usadas más abajo
void myinit(void);
void menu(const int value);
void crearMenu(void);
void botonRaton(int boton, int estado, int x, int y);
void movRaton(int x, int y);
void dibujarEjes(Transformacion const &t);
void dibujarCurva(Punto cb[]);
void dibujarTangentes(Punto cb[], Vector ct[]);
void dibujarPControl(Punto p[], Transformacion t, int nc);
void dibujarPoligCar(Punto [], Transformacion t);
void myReshape(GLsizei w, GLsizei h);
void display(void);

//Definiciones
#define PCONT  4	//Número de puntos de control dados para la polilínea
#define NVERT 20	//Número de vértices que se quiere tenga la polilínea

#define XYZ_X 0
#define XYZ_Y 1
#define XYZ_Z 2

#define EXIT	100
#define ON		200
#define OFF		201
#define VEL1	300
#define VEL2	301
#define VEL3	302
#define POLIG	400

//Movimiento del ratón
int   last_x, last_y;
float rotationX = 0.0, rotationY = 0.0;
float positionX = 0.0, positionY = 0.0;

//Variables globales
CurvaBezier Q1;									//La curva que vamos a ver
Punto G1[PCONT] =  {Punto(-1.0,-1.0, 1.0),		//Vector geométrico de puntos de control P0,P1,P2,P3
					Punto(-2.0, 0.0,-2.0),
					Punto( 2.0, 1.0,-2.0),
					Punto( 2.0, 0.0, 0.0) };	

CurvaBezier Q2;
Punto G2[PCONT] =  {Punto( 2.0, 0.0, 0.0),
					Punto( 2.0,-1.0, 2.0), 
					Punto(-2.0, 0.0, 2.0),
					Punto(-1.0, 1.0,-1.0)};

Punto  cb1[NVERT], cb2[NVERT];					//Vector de puntos que representa la curva
Vector tb1[NVERT], tb2[NVERT];					//Vector de tangentes en cada punto
Punto pcG1[PCONT], pcG2[PCONT];					//Vector de manejo de puntos de control

// Variables auxiliares para el movimiento de los puntos
bool suma_resta = true;
int xyz = XYZ_Y;
int movePoint = 1;
float paso = 0.2;
//GLUI_String strPunto[] = {"Punto (1-8): ", "1", "2", "3", "4", "5", "6", "7", "8"};
//GLUI_String strDirec[] = {"Direccion: ", "X", "Y", "Z"};

//Auxiliares
Flecha v;

//Animación
bool animar = false;
bool verPoligCar = true;
float alpha = 1.0;



void myinit(void){		//Inicialización de variables de la GL y globales

    glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);
	Q1 = CurvaBezier(G1);		//Define la curva Q
	Q2 = CurvaBezier(G2);		//Define la curva Q2

	for(int i=0; i<PCONT; i++){
		pcG1[i] = G1[i];
		pcG2[i] = G2[i];
	}

	crearMenu();
}

void menu(const int value){

	switch(value){
		case EXIT:	exit(1); break;
		case ON:	animar = true; break;
		case OFF:	animar = false; break;
		case VEL1:	alpha = 0.5; break;
		case VEL2:	alpha = 1.0; break;
		case VEL3:	alpha = 2.0; break;
		case POLIG: verPoligCar = !verPoligCar; break;
	}
	crearMenu();
	glutPostRedisplay();
}

void crearMenu(){

	// Crear los menus
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
		if(verPoligCar)
			glutAddMenuEntry("Ocultar polígonos", POLIG);
		else
			glutAddMenuEntry("Mostrar polígonos", POLIG);
		glutAddMenuEntry("Salir", EXIT);
}

void animacion(){
	
	if(animar) rotationY += alpha;
	glutPostRedisplay();
}

void moverPunto(int idPunto, int x, int y){
	
	float var = suma_resta ? paso: -paso;

	switch(idPunto){
		case 1:
			G1[0].setelem(xyz, G1[0].elem(xyz) + var);
		case 2:
			G1[1].setelem(xyz, G1[1].elem(xyz) + var); break;
		case 3:
			G1[2].setelem(xyz, G1[2].elem(xyz) + var);
			G2[1].setelem(xyz, G2[1].elem(xyz) - var); break;
		case 4: 
		case 5:
			G1[3].setelem(xyz, G1[3].elem(xyz) + var);
			G1[2].setelem(xyz, G1[2].elem(xyz) + var);
			G2[0].setelem(xyz, G2[0].elem(xyz) + var);
			G2[1].setelem(xyz, G2[1].elem(xyz) + var); break;
		case 6:
			G1[2].setelem(xyz, G1[2].elem(xyz) - var);
			G2[1].setelem(xyz, G2[1].elem(xyz) + var); break;
		case 7:
			G2[2].setelem(xyz, G2[2].elem(xyz) + var); break;
		case 8:
			G2[3].setelem(xyz, G2[3].elem(xyz) + var); break;
		default: break;
	}
}

/*int procesarClick(int x, int y){

		glReadBuffer(GL_BACK);
			
		int np = -1;
		GLint viewport[4];
		GLubyte px[3];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glReadPixels(x, viewport[3]-y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void *)px);

		if(px[0] == 255 && px[1] == 0   && px[2] == 0  ) np = 0;
		if(px[0] == 0   && px[1] == 255 && px[2] == 0  ) np = 1;
		if(px[0] == 0   && px[1] == 0   && px[2] == 255) np = 2;
		if(px[0] == 50  && px[1] == 50  && px[2] == 50 ) np = 3;
		if(px[0] == 255 && px[1] == 255 && px[2] == 0  ) np = 4;
		if(px[0] == 0   && px[1] == 255 && px[2] == 255) np = 5;
		if(px[0] == 255 && px[1] == 0   && px[2] == 255) np = 6;
		if(px[0] == 200 && px[1] == 200 && px[2] == 200) np = 7;

		glReadBuffer(GL_FRONT);
		printf("\nnp = %d", np);

		return np;
}*/

void botonRaton(int boton, int estado, int x, int y){

	// Callback cuando pulsamos los botonos del ratón
	if(boton==GLUT_LEFT_BUTTON && estado==GLUT_DOWN){

		//int idpunto = procesarClick(x, y);
		//if(idpunto != -1)
		//	moverPunto(idpunto, x, y);
		//else{
			last_x = x;
			last_y = y;
		//}
	}
}

void movRaton(int x, int y){

	// Callback del movimiento del raton, varía el ángulo desde donde presentamos el dibujo
	rotationX += (float)(last_y - y); //La y decrece hacia arriba
	rotationY += (float)(x - last_x);
	positionX += (float)(x - last_x);
	positionY += (float)(last_y - y);

	last_x = x;
	last_y = y;

	glutPostRedisplay(); 
}

void teclaTeclado(unsigned char key, int x, int y){

	switch(key){
		case 'x': case 'X': xyz = XYZ_X; break;
		case 'y': case 'Y': xyz = XYZ_Y; break;
		case 'z': case 'Z': xyz = XYZ_Z; break;

		case '1': movePoint = 1; break;
		case '2': movePoint = 2; break;
		case '3': movePoint = 3; break;
		case '4': movePoint = 4; break;
		case '5': movePoint = 5; break;
		case '6': movePoint = 6; break;
		case '7': movePoint = 7; break;
		case '8': movePoint = 8; break;

		case 'a': case 'A': animar = !animar; break;
		case 'p': case 'P': verPoligCar = !verPoligCar; break;
		case 'q': case 'Q': exit(0); break;
		default: break;
	}

	glutPostRedisplay();
}

void especialTeclado(int key, int x, int y){
	
	switch(key){
	  case GLUT_KEY_UP:	
	  case GLUT_KEY_RIGHT: 
		  suma_resta = true;
		  break;
	  case GLUT_KEY_DOWN: 
	  case GLUT_KEY_LEFT:  
		  suma_resta = false;
		  break;
	  default: break;
	}
	moverPunto(movePoint, x, y);

	glutPostRedisplay();
}

void dibujarEjes(Transformacion const &t){

	Punto o = Punto().transform(t);
	Punto x = Punto(1,0,0).transform(t);
	Punto y = Punto(0,1,0).transform(t);
	Punto z = Punto(0,0,1).transform(t);
	Flecha(x-o, Color::ROJO ).drawAt(o);
	Flecha(y-o, Color::VERDE).drawAt(o);
	Flecha(z-o, Color::AZUL ).drawAt(o);
}

void imprimirLeyenda(){

		// Leyenda
		glColor3f( 0.0, 0.0, 0.0 );

		GLUI_String puntoStr = strPunto[0];
		puntoStr += strPunto[movePoint];
		glRasterPos2i(20, 40);
		for(unsigned int c=0; c<puntoStr.length(); ++c)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, puntoStr[c]);

		GLUI_String direcStr = strDirec[0];
		direcStr += strDirec[xyz+1];
		glRasterPos2i(20, 20);
		for(unsigned int c=0; c<direcStr.length(); ++c)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, direcStr[c]);
}

void display(void){
//Función de respuesta a la acción de mostrar el contenido de la ventana

	Transformacion t;						//Acumula las transformaciones aplicadas
	float current;

	//Para dibujar la curva aplicaremos la transformacion a los puntos
	//de control, calcularemos los puntos sobre la curva y la dibujaremos como polilínea

	t.reset();
	t.translation(Vector(0, 0, -7));		//Mueve la camara hacia atrás
	t.rotX( 40.0 - rotationX);				//Gira la superficie para verla desde arriba
	t.rotY(-40.0 + rotationY);				//atendiendo a la interacción con el ratón

	for(int j=0; j<PCONT; j++){ 
		Q1.setPoint(j, G1[j].transform(t));	//Transforma los puntos de control
		Q2.setPoint(j, G2[j].transform(t));
	}
	
	Q1.getPoints(NVERT, cb1);				//Cálculo de vértices Q1
	Q2.getPoints(NVERT, cb2);				//Cálculo de vértices Q2
	Q1.getTangents(NVERT, tb1);				//Cálculo de tangentes Q1
	Q2.getTangents(NVERT, tb2);				//Cálculo de tangentes Q2

	glClear(GL_COLOR_BUFFER_BIT);			//Borra la ventana
	glDrawBuffer(GL_FRONT);
	
	dibujarEjes(t);
	//imprimirLeyenda();

	//glGetFloatv(GL_LINE_WIDTH, &current);

	glColor3f(0.0,0.0,0.0);
		dibujarCurva(cb1); 
		if(verPoligCar) 
			dibujarPoligCar(G1, t); 
		dibujarPControl(G1, t, 1); 
		dibujarTangentes(cb1, tb1);
	glColor3f(0.0,0.0,1.0);
		dibujarCurva(cb2); 
		if(verPoligCar) 
			dibujarPoligCar(G2, t); 
		dibujarPControl(G2, t, 2); 
		dibujarTangentes(cb2, tb2);

	//glLineWidth(current);

	glFlush();								//Descarga los buffers
}

void dibujarCurva(Punto cb[]){

	//Dibujo de la curva Q
	glLineWidth(2.0);
	glBegin(GL_LINE_STRIP);					
	for(int j=0; j<NVERT; j++){
		glVertex3f( (GLfloat)(cb[j].x()),
					(GLfloat)(cb[j].y()),
					(GLfloat)(cb[j].z()));
	};
	glEnd();
}

void dibujarTangentes(Punto cb[], Vector tb[]){

	//Dibujo de las tangentes
	glLineWidth(1.0);
	for(int j=0; j<NVERT; j++){
		v = Flecha(tb[j],Color(1.0,0.0,0.0));
		v.drawAt(cb[j]);
	};
}

void dibujarPControl(Punto p[], Transformacion t, int nc){

	//Dibujo de los puntos de control
	//glColor3f(0.0,0.0,0.0);
	glPointSize(5.0);
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	for(int i=0; i<PCONT; i++){
		glVertex3f(p[i].transform(t).x(), 
				   p[i].transform(t).y(), 
				   p[i].transform(t).z());
	}
	glEnd();
	/*
	//Dibujo de los mismos puntos en background para el color picking
	glDrawBuffer(GL_BACK);
	glDisable(GL_DITHER);
	glBegin(GL_POINTS);
	for(int i=0; i<PCONT; i++){
		if(nc == 1){
			switch (i) {	
				case 0: glColor3ub(255,0,0); break;		// Rojo
				case 1: glColor3ub(0,255,0); break;		// Verde
				case 2: glColor3ub(0,0,255); break;		// Azul
				case 3: glColor3ub(50,50,50); break;	// Gris osc.
			}
		}else{
			switch (i) {	
				case 0: glColor3ub(255,255,0); break;	// Amarillo	
				case 1: glColor3ub(0,255,255); break;	// Celeste
				case 2: glColor3ub(255,0,255); break;	// Rosa
				case 3: glColor3ub(200,200,200); break;	// Gris
			}
		}
		glVertex3f(p[i].transform(t).x(), 
				   p[i].transform(t).y(), 
				   p[i].transform(t).z());
	}
	glEnd();
	glDrawBuffer(GL_FRONT); */
}


void dibujarPoligCar(Punto p[], Transformacion t){

	//Dibujo del poligono caracteristico
	glLineWidth(1.0);
	glBegin(GL_LINE_STRIP);
	for(int i=0; i<PCONT; i++){
		glVertex3f(p[i].transform(t).x(), 
				   p[i].transform(t).y(), 
				   p[i].transform(t).z());
	}
	glEnd();
}

void onIdle(void){

	if(animar) 
		animacion();
}

//Función de respuesta a la acción de cambio de tamaño de la ventana
void myReshape(GLsizei w, GLsizei h){

	//Inicialización de la cámara de GL
    h = (h==0) ? 1 : h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	//Cámara perspectiva en el origen con fov 40º y cerca=1 lejos=200
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv){
	//Programa principal

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500); 
	glutInitWindowPosition(100, 100);
	//Creación de la ventana
	glutCreateWindow("Curva de Bezier TEST");
	myinit();
	//Registro CallBacks
	glutDisplayFunc(display); 
	glutReshapeFunc(myReshape);
	glutMotionFunc(movRaton);
	glutMouseFunc(botonRaton);
	glutKeyboardFunc(teclaTeclado);
	glutSpecialFunc(especialTeclado);
	glutIdleFunc(onIdle);

	//Puesta en marcha
	glutMainLoop();
	return(0);
}
