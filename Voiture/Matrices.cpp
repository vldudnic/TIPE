/*
FICHIER MATRICES

FONCTIONS POUR FACILITER LE CALCUL MATRICIEL
*/


#pragma once
#include "Matrices_Prototypes.h"

Matrice::Matrice(int n, int m, float x) : m_lignes(n), m_colones(m)
{
	std::vector<float> ligne(m, x);
	for (int i = 0; i < n; ++i)
	{
		m_valeurs.push_back(ligne);
	}
}

void Matrice::afficher() const
{
	for (int i = 0; i < m_lignes; ++i)
	{
		for (int j = 0; j < m_colones; ++j)
		{
			std::cout << m_valeurs[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void Matrice::setValeur(int i, int j, float val)
{
	m_valeurs[i-1][j-1] = val;
}

float Matrice::getValeur(int i, int j) const
{
	return m_valeurs[i-1][j-1];
}

int Matrice::getColones() const
{
	return m_colones;
}

int Matrice::getLignes() const
{
	return m_lignes;
}

Matrice Matrice::additionne(Matrice A, Matrice B)
{
	if (A.m_lignes == B.m_lignes && A.m_colones == B.m_colones)
	{
		Matrice M(A.m_lignes, A.m_colones, 0);
		for (int i = 0; i < A.m_lignes; i++)
		{
			for (int j = 0; j < A.m_colones; j++)
			{
				M.m_valeurs[i][j] = A.m_valeurs[i][j] + B.m_valeurs[i][j];
			}
		}
		return M;
	}
	else
	{
		std::cout << "Matrices incompatibles\n";
	}
}

Matrice Matrice::multiplie(Matrice A, Matrice B)
{
	if (A.m_colones == B.m_lignes)
	{
		Matrice M(A.m_lignes, B.m_colones, 0);
		for (int i = 0; i < M.m_lignes; i++)
		{
			for (int j = 0; j < M.m_colones; j++)
			{
				for (int k = 0; k < A.m_colones; k++)
				{
					M.m_valeurs[i][j] += (A.m_valeurs[i][k] * B.m_valeurs[k][j]);
				}
			}
		}
		return M;
	}
	else
	{
		std::cout << "Matrices incompatibles\n";
	}
}

Matrice Matrice::multiplie(float k, Matrice A)
{	
	Matrice M(A.getLignes(), A.getColones(), 0);
	for (int i = 0; i < A.m_lignes; i++)
	{
		for (int j = 0; j < A.m_colones; j++)
		{
			M.m_valeurs[i][j] = k*A.m_valeurs[i][j];
		}
	}
	return M;
}

bool Matrice::sontEgales(Matrice A, Matrice B)
{
	if (A.m_lignes != B.m_lignes || A.m_colones != B.m_colones)
	{
		return false;
	}
	else
	{
		for (int i = 0; i < A.m_lignes; i++)
		{
			for (int j = 0; j < A.m_colones; j++)
			{
				if (A.m_valeurs[i][j] != B.m_valeurs[i][j])
				{
					return false;
				}
			}
		}
	}
	return true;
}


Matrice::~Matrice()
{}