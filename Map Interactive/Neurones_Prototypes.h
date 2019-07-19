/*
FICHIER NEURONES

CLASSE RESEAUNEURONES GERANT LE FONCTIONNEMENT D'UN RESEAU DE NEURONES
*/


#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include "Matrices_Prototypes.h"

class ReseauNeurones
{
public:
	ReseauNeurones();
	ReseauNeurones(std::vector<int> taillesCouches, std::vector<float> ADN); 
	Matrice reponseStimuli(std::vector<float> stimuli);
	~ReseauNeurones();

private:
	std::vector<Matrice> m_couches;
	std::vector<Matrice> m_matricesPoids;

};


