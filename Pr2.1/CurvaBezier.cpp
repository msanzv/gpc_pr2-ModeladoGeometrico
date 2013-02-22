/* IMPLEMENTACIÓN DE UNA CURVA CUBICA DE BEZIER
Prof: R.Vivó, J.Lluch para GPC.etsinf.upv.es 2011
Alumno:                                          */


#include <iostream>
#include <stdio.h>

#include "CurvaBezier.h"

using namespace std;

const Matriz CurvaBezier::MBezier=Matriz(Real4(-1.0f , 3.0f , -3.0f , 1.0f),
 										 Real4(3.0f , -6.0f , 3.0f , 0.0f),
 									     Real4(-3.0f , 3.0f , 0.0f , 0.0f),
 										 Real4(1.0f , 0.0f , 0.0f , 0.0f));

CurvaBezier::CurvaBezier()
//Constructor por defecto
{
	pControl[0]= Punto();
	pControl[1]= Punto();
	pControl[2]= Punto();
	pControl[3]= Punto();
};
CurvaBezier::CurvaBezier(Punto p[4])
//Constructor
{
	int i;
	for(i=0;i<4;i++) pControl[i]=p[i];
	setC();
};

CurvaBezier::CurvaBezier(Punto p0, Punto p1, Punto p2, Punto p3)
//Constructor
{
	pControl[0]= p0;
	pControl[1]= p1;
	pControl[2]= p2;
	pControl[3]= p3;
	setC();
};

void CurvaBezier::setC()
//Calculo de la matriz de coeficientes C=Mb*G
{
	/*Introduce aquí tu código*/
	Matriz G = Matriz(pControl[0], pControl[1], pControl[2], pControl[3]);
	C = MBezier * G;
	
};

void CurvaBezier::setPoint(int cual, Punto nuevo)
//Cambia el punto cual [0..3] por el nuevo
{
	pControl[cual]= nuevo;
	setC();
};

Punto CurvaBezier::controlPoint(int i)const
//Devuelve el punto de control
{
	if(i<0 || i>3) cerr << "CB: Indice fuera de rango" << endl;
	return pControl[i];
};


void CurvaBezier::getPoints(int n, Punto *puntos)const
//Devuelve n puntos sobre la curva en puntos[]
//mediante el método de las diferencias avanzadas
{

	float L = 1.0/(n-1);
	float L2 = L*L, L3 = L*L*L;
	Matriz E = Matriz(Real4(0, 0, 0, 1),
					  Real4(L3, L2, L, 0),
					  Real4(6*L3, 2*L2, 0, 0),
					  Real4(6*L3, 0, 0, 0));
	Matriz D = E*C;

	puntos[0] = D.row(0);
	Punto P = puntos[0];
	Vector inc1, inc2, inc3;
	inc1 = D.row(1);
	inc2 = D.row(2);
	inc3 = D.row(3);
	for(int i=1; i<n; ++i){
		P = P + inc1;
		puntos[i] = P;
		inc1 = inc1 + inc2;
		inc2 = inc2 + inc3;
	}
};

void CurvaBezier::getTangents(int n, Vector *tangentes) const
//Devuelve n tangentes en puntos equidistantes en u
{
	float u = 1.0/(n-1);
	float p = 0.0;
	for(int i=0; i<n; i++){
		tangentes[i] = tangent(p);
		p += u;
	}
};

Vector CurvaBezier::tangent(float u) const
//Devuelve la tangente en Q(u)
{
	Real4 aux = Real4(3*(u*u), 2*u, 1, 0);
	aux = aux * C;
	Vector v = Vector(aux.x(), aux.y(), aux.z());
	return v.unitary();
};

