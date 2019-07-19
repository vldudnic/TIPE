/*
FICHIER NEURONES

FONCTIONS GERANT LE RESEAU DE NEURONES
*/


#pragma once
#include "Neurones_Prototypes.h"


ReseauNeurones::ReseauNeurones() // Constructeur vide pour pas faire planter le programme
{}

/* Vrai constructeur */
ReseauNeurones:: ReseauNeurones(std::vector<int> taillesCouches, std::vector<float> ADN) // On construit le réseau à partir d'une liste de valeurs (l'ADN)
{	
	// Construction des couches du réseau 
	for (int i = 0; i < taillesCouches.size(); i++)
	{
		m_couches.push_back(Matrice(taillesCouches[i], 1, 0));
	}
	
	// Remplissage des matrices de passage avec les valeurs de la liste ADN
	int compteur(0);
	for (int k = 0; k < taillesCouches.size() - 1; k++)
	{
		Matrice M(taillesCouches[k + 1], taillesCouches[k], 0); // On crée une matrice de la bonne taille
		for (int i = 1; i <= M.getLignes(); i++) // On rempli la matrice
		{
			for (int j = 1; j <= M.getColones(); j++)
			{
				M.setValeur(i, j, ADN[compteur]);
				compteur++;
			}
		}
		m_matricesPoids.push_back(M);
	}
}


/* Fonction qui permet la transmission d'un signal dans le réseau de neurones */
Matrice ReseauNeurones::reponseStimuli(std::vector<float> stimuli)
{
	if (stimuli.size() != m_couches[0].getLignes()) // Gestion des erreurs de taille
	{
		std::cout << "Impossible de traiter les informations \n";
	}
	else
	{	
		// On initialise la premiere couche du reseau avec les données du vecteur stimuli
		for (int i = 1; i <= stimuli.size(); i++)
		{
			m_couches[0].setValeur(i, 1, stimuli[i-1]);
		}

		// On transmet ensuite le signal par produit matriciel d'une couche à l'autre du réseau
		for (int i = 1; i < m_couches.size(); i++)
		{
			m_couches[i] = Matrice::multiplie(m_matricesPoids[i - 1], m_couches[i - 1]);
			for (int j = 1; j <= m_couches[i].getLignes(); j++)
			{
				float x(m_couches[i].getValeur(j, 1));
				x = 1 / (1 + std::exp(-x)); // On modifie la valeur des Neurones avec la fonction sigmoïde
				m_couches[i].setValeur(j, 1, x);
			}
		}
		return m_couches[m_couches.size() - 1]; // On renvoie la derniere couche
	}
}

ReseauNeurones::~ReseauNeurones() // Destructeur par défaut
{}


