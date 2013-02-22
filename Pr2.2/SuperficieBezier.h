/* DEFINICIÓN DE UNA SUPERFICIE BICÚBICA DE BEZIER
R.Vivó, J.Lluch para GPC.etsinf.upv.es 2011 */

#ifndef __SUPERFICIEBEZIER
#define __SUPERFICIEBEZIER

#include "Algebra.h"

class SuperficieBezier
{

private:

	static const Matriz MBezier;	//Matriz caracteristica de Bezier
	Bloque pControl;				//Puntos de control 
	Bloque C;						//Matrices de coeficientes

	void setC();					//Calcula la matriz de coeficientes

public:

	SuperficieBezier();				//Constructores
	SuperficieBezier(Punto p[16]);	//Ordenación igual que constructor siguiente
	SuperficieBezier(	Punto p00, Punto p01, Punto p02, Punto p03, //u cte
						Punto p10, Punto p11, Punto p12, Punto p13, //u cte
						Punto p20, Punto p21, Punto p22, Punto p23,
						Punto p30, Punto p31, Punto p32, Punto p33);
	

	void getPoints(int n, Punto *puntos);//Devuelve nxn puntos en una malla
	void getNormals(int n, Vector *normales)const;//Devuelve nxn normales unif. distribuidas
	void getTangents(int n, Vector *tgU, Vector *tgV)const;//Devuelve nxn tangentes en u y v unif. distribuidas
	Real4 controlPoint(int i, int j)const;//Devuelve el punto de control Pij
	void setPoint(int i, int j, Punto nuevo);//Cambia el punto i,j [0..3][0..3] por el nuevo
	Vector uTangent(float u, float v)const;//Devuelve el vector tangente en dirección u en u,v
	Vector vTangent(float u, float v)const;//Devuelve el vector tangente en dirección v en u,v
	Vector normal(float u, float v)const;	//Devuelve el vector normal en u,v
};


#endif
