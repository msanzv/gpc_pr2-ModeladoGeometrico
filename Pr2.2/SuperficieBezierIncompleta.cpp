/* IMPLEMENTACIÓN DE UNA SUPERFICE BICÚBICA DE BEZIER
Prof: R.Vivó, J.Lluch para GPC.etsinf.upv.es 2011 
Alumno:													*/

#include "SuperficieBezier.h"
#include <stdio.h>

const Matriz SuperficieBezier::MBezier = Matriz(Real4(-1.0f , 3.0f , -3.0f , 1.0f),
 												Real4(3.0f , -6.0f , 3.0f , 0.0f),
 												Real4(-3.0f , 3.0f , 0.0f , 0.0f),
 												Real4(1.0f , 0.0f , 0.0f , 0.0f));


SuperficieBezier::SuperficieBezier()
//Constructor por defecto
{

};
	
SuperficieBezier::SuperficieBezier(Punto p[16])
//Constructor por filas
{
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++) 
			pControl.setdrawer(i,j ,p[i*4+j]);
	setC();
};

SuperficieBezier::SuperficieBezier(
						Punto p00, Punto p01, Punto p02, Punto p03,
						Punto p10, Punto p11, Punto p12, Punto p13,
						Punto p20, Punto p21, Punto p22, Punto p23,
						Punto p30, Punto p31, Punto p32, Punto p33 )
//Constructor con 16 puntos
{
	pControl.setdrawer(0,0,p00);
	pControl.setdrawer(0,1,p01);
	pControl.setdrawer(0,2,p02);
	pControl.setdrawer(0,3,p03);
	pControl.setdrawer(1,0,p10);
	pControl.setdrawer(1,1,p11);
	pControl.setdrawer(1,2,p12);
	pControl.setdrawer(1,3,p13);
	pControl.setdrawer(2,0,p20);
	pControl.setdrawer(2,1,p21);
	pControl.setdrawer(2,2,p22);
	pControl.setdrawer(2,3,p23);
	pControl.setdrawer(3,0,p30);
	pControl.setdrawer(3,1,p31);
	pControl.setdrawer(3,2,p32);
	pControl.setdrawer(3,3,p33);
	
	setC();
};

Real4 SuperficieBezier::controlPoint(int i,int j)const
{
	return pControl.drawer(i,j);
};

void SuperficieBezier::setC()
//Cálculo de la matriz de coeficientes C= Mb*G*Mbtras
{
	/* Introduce aquí tu código */
	C = MBezier * pControl * (!MBezier);
};


void SuperficieBezier::setPoint(int i,int j, Punto nuevo)
//Cambia el punto i,j [0..3][0..3] por el nuevo
{
	pControl.setdrawer(i,j,nuevo);
	setC();
};

Vector SuperficieBezier::uTangent (float u, float v)const
//Devuelve el vector tangente en dirección u en u,v
{
	Real4 aux = Real4(3*(u*u), 2*u, 1, 0) * C * Real4(v*v*v, v*v, v, 1);
	Vector vec = Vector(aux.x(), aux.y(), aux.z());
	return vec.unitary();
};

Vector SuperficieBezier::vTangent (float u, float v)const
//Devuelve el vector tangente en dirección v en u,v
{
	Real4 aux = Real4(u*u*u, u*u, u, 1) * C * Real4(3*(v*v), 2*v, 1, 0);
	Vector vec = Vector(aux.x(), aux.y(), aux.z());
	return vec.unitary();
};

Vector SuperficieBezier::normal (float u, float v)const
//Devuelve el vector normal en u,v
{
	return uTangent(u,v)^vTangent(u,v);
};

void SuperficieBezier::getPoints(int n, Punto *puntos)
/*Devuelve nxn puntos de la superficie en puntos[][]
mediante el método de las diferencias avanzadas*/ 
{
	setC();		//Calcula la matriz de coeficientes por conveniencia

	float d = 1.0/(n-1);
	float d2 = d*d, d3 = d*d*d;
	Matriz E = Matriz(Real4(0, 0, 0, 1),
					  Real4(d3, d2, d, 0),
					  Real4(6*d3, 2*d2, 0, 0),
					  Real4(6*d3, 0, 0, 0));
	Bloque D = E * C * (!E);

	int u=0, v=0;
	int k=0;
	Vector inc1, inc2, inc3;
		
	for(u=0; u<n; u++){
		puntos[k] = D.drawer(0,0);
		k+=1;
		inc1 = D.drawer(0,1);
		inc2 = D.drawer(0,2);
		inc3 = D.drawer(0,3);
		for(v=1; v<n; v++){
			puntos[k] = puntos[k-1] + inc1;
			k++;
			inc1 = inc1 + inc2;
			inc2 = inc2 + inc3;
		}
		D.addrowsup();
	}


};
void SuperficieBezier::getTangents(int n, Vector *tgU, Vector *tgV) const
//Devuelve nxn tangentes uniformemente distribuidas a u constante
{
	float d = 1.0/(float)(n-1);
	int pos = 0;
	for(float u=0.0; u<=1; u+=d){
		for(float v=0.0; v<=1; v+=d){
			tgU[pos] = uTangent(u,v);
			tgV[pos] = vTangent(u,v);
			pos++;
		}
	}
};

void SuperficieBezier::getNormals(int n, Vector *normales) const
//Devuelve nxn normales uniformemente distribuidas a u constante
{
	float d = 1.0/(float)(n-1);
	int pos = 0;
	for(float u=0.0; u<=1; u+=d){
		for(float v=0.0; v<=1; v+=d){
			normales[pos] = normal(u,v);
			pos++;
		}
	}
};
