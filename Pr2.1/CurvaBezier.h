/* DEFINICIÓN DE UNA CURVA CUBICA DE BEZIER
R.Vivó, J.Lluch para GPC.etsinf.upv.es 2011 */

#ifndef __CURVABEZIER
#define __CURVABEZIER

#include "Algebra.h"

class CurvaBezier
{
private:
	static const Matriz MBezier;	//Matriz caracteristica de Bezier
	Punto pControl[4];				//Puntos de control
	Matriz C;						//Matriz de Coeficientes

	void setC();					//Calcula la matriz de coeficientes

public:
	CurvaBezier();					//Constructores
	CurvaBezier(Punto p[4]);	
	CurvaBezier(Punto p0, Punto p1, Punto p2, Punto p3);

	void getPoints(int n, Punto *puntos)const;		//Muestrea la curva y devuelve n puntos
	void getTangents(int n, Vector *tangentes)const;//Muestrea la curva y devuelve n tangentes
	void setPoint(int cual, Punto nuevo);			//Cambia el punto cual [0..3] por el nuevo
	Punto controlPoint(int i)const;					//Devuelve el punto de control [0..3]
	Vector tangent(float u)const;					//Devuelve el vector unitario tangente en u
};


#endif
