/*
FICHIER MATRICES

CLASSE MATRICE POUR FACILITER LE CALCUL MATRICIEL
*/


#pragma once
#include <vector>
#include <iostream>

class Matrice
{
public:
	Matrice(int n, int m, float x);
	void afficher() const;
	void setValeur(int i, int j, float val);
	float getValeur(int i, int j) const;
	int getColones() const;
	int getLignes() const;
	static Matrice additionne(Matrice A, Matrice B);
	static Matrice multiplie(Matrice A, Matrice B);
	static Matrice multiplie(float k, Matrice A);
	static bool sontEgales(Matrice A, Matrice B);
	~Matrice();

private:
	std::vector<std::vector<float>> m_valeurs;
	int m_lignes;
	int m_colones;
};