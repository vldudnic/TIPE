/* 
FICHIER VOITURE: 

FONCTIONS GERANT LA VOITURE:
- DECISIONS
- DEPLACEMENTS
- ARRETS
- AFFICHAGE

FONCTION GERANT L'EVOLUTION D'UNE POPULATION DE VOITURES
*/

#pragma once
#include "Voiture_Prototypes.h"



/* FONCTIONS GERANT LA PARTIE PHYSIQUE DES VOITURES*/

/* Constructeurs et destrcuteurs par défaut */
Voiture::Voiture()
{}
Voiture::~Voiture()
{}

/* Constructeur aléatoire de la voiture */
Voiture::Voiture(unsigned int &seed) :
	m_longueur(LONGUEUR), m_largeur(LARGEUR), m_hauteur(HAUTEUR), m_angleMaxRoues(ANGLE_MAX_ROUES), m_angleRoues(0), m_angleDirection(0), m_distanceCapteurMax(DISTANCE_CAPTEURS_MAX), m_vitesseMax(VITESSE_MAX), m_acceleration(ACCELERATION), m_deceleration(DECELERATION), m_vitesse(6), m_crash(false) // On initialise quelques valeurs numériques
{
	/* Image de la voiture */
	m_imageVoiture.setSize(sf::Vector2f(m_largeur, m_longueur));
	m_imageVoiture.setFillColor(sf::Color::Black);
	m_imageVoiture.setOrigin(m_largeur / 2, m_longueur);
	m_imageVoiture.setPosition(X, Y); // Position arbitraire, correspondra à la position initiale sur le circuit

	/* Position du capteur (position de départ des mesures de distances) 
	Le capteur se trouve à l'avant de la voiture à égale distance des deux roues */
	m_positionCapteur.x = m_imageVoiture.getPosition().x;
	m_positionCapteur.y = m_imageVoiture.getPosition().y - m_longueur; 

	/* Initialisation des capteurs -angles, graphiques- */
	m_anglesCapteurs = ANGLES_CAPTEURS; 

	m_lignesCapteurs.resize(m_anglesCapteurs.size());
	for (int i = 0; i < m_lignesCapteurs.size(); i++)
	{
		m_lignesCapteurs[i].setPrimitiveType(sf::Lines);
		m_lignesCapteurs[i].resize(2);
		m_lignesCapteurs[i][0].color = sf::Color::Red;
		m_lignesCapteurs[i][1].color = sf::Color::Red;
	}

	/* Initialisation du Reseau de neurones */
	int n = m_anglesCapteurs.size();
	std::vector<int> taillesCouches({ n,5,5,4 }); // Il y a autant de neurones sur la 1ère couche du réseau qu'il y a de capteurs

	/* Création de l'ADN de la voiture: la taille de l'ADN est la somme des tailles des matrices (un gène pour chaque coéficient) */
	int taille(0);
	for (int i = 1; i < taillesCouches.size(); i++)
	{
		taille += taillesCouches[i - 1] * taillesCouches[i];
	}

	std::default_random_engine generator(seed);
	seed += 1000;
	std::uniform_real_distribution<float> distribution(-10.0f, 10.0f);

	for (int i = 0; i < taille; i++)
	{
		float nombre = distribution(generator);
		m_ADN.push_back(nombre); // Initialement les coeficients de l'ADN sont aléatoires
	}
	m_cerveauVoiture = ReseauNeurones(taillesCouches, m_ADN); // Le cerveau de la voiture est alors créé pour rendre compte de cet ADN 

	m_adaptation = 0.0f; // Initialisation de l'adaptation à 0 (obviously). L'adaptation sera recalculée plus tard
}

/* Variante du constructeur (on connait déjà l'ADN) */
Voiture::Voiture(std::vector<float> ADN):
	m_longueur(LONGUEUR), m_largeur(LARGEUR), m_hauteur(HAUTEUR), m_angleMaxRoues(ANGLE_MAX_ROUES), m_angleRoues(0), m_angleDirection(0), m_distanceCapteurMax(DISTANCE_CAPTEURS_MAX), m_vitesseMax(VITESSE_MAX), m_acceleration(ACCELERATION), m_deceleration(DECELERATION), m_vitesse(6), m_crash(false)
{
	m_imageVoiture.setSize(sf::Vector2f(m_largeur, m_longueur));
	m_imageVoiture.setFillColor(sf::Color::Black);
	m_imageVoiture.setOrigin(m_largeur / 2, m_longueur);
	m_imageVoiture.setPosition(X, Y); 

	m_positionCapteur.x = m_imageVoiture.getPosition().x;
	m_positionCapteur.y = m_imageVoiture.getPosition().y - m_longueur;

	m_anglesCapteurs = ANGLES_CAPTEURS;
	m_lignesCapteurs.resize(m_anglesCapteurs.size());
	for (int i = 0; i < m_lignesCapteurs.size(); i++)
	{
		m_lignesCapteurs[i].setPrimitiveType(sf::Lines);
		m_lignesCapteurs[i].resize(2);
		m_lignesCapteurs[i][0].color = sf::Color::Red;
		m_lignesCapteurs[i][1].color = sf::Color::Red;
	}

	int n = m_anglesCapteurs.size();
	std::vector<int> taillesCouches({ n,5,5,4 }); 
				
	m_ADN = ADN;
	m_cerveauVoiture = ReseauNeurones(taillesCouches, m_ADN); 

	m_adaptation = 0.0f; 
}


void Voiture::cycleDecision(sf::Image & terrain) // Cycle principal de décisions d'une voiture
{	
	std::vector<float> donnees(acquerirDonnes(terrain)); // Acquisition des données
	priseDecision(donnees); // Traitement des données
	deplacer(); // Execution des décisions prises
}


void Voiture::afficher(sf::RenderWindow & window) // Fonction d'affichage basique
{
	window.draw(m_imageVoiture);
	for (int i = 0; i < m_lignesCapteurs.size(); i++)
	{
		window.draw(m_lignesCapteurs[i]);
	}
}


void Voiture::setCrash(sf::Image & terrain) // Determine si la voiture s'est crashée dans un mur
{
	sf::Vector2f position = m_positionCapteur; // On ne considère qu'un seul point à l'avant de la voiture
	if (terrain.getPixel(std::floor(position.x), std::floor(position.y)) != sf::Color::White) // Si ce point n'est pas sur la route: Crash
	{
		m_crash = true;
	}
}


void Voiture::demarer() // Démarage de la voiture: on réinitialise certaines constantes
{	
	m_vitesse = 6;
	m_imageVoiture.setPosition(X, Y);
	m_imageVoiture.setRotation(0);
	m_positionCapteur.x = m_imageVoiture.getPosition().x;
	m_positionCapteur.y = m_imageVoiture.getPosition().y - m_longueur;
	m_angleDirection = 0;
	m_angleRoues = 0;
	m_distanceParcourue = 0;
	m_adaptation = 0;
	m_horlogeInterne.restart();
}

bool Voiture::arreter(sf::FloatRect & zoneFin)
{	
	sf::FloatRect perimetreVoiture = m_imageVoiture.getGlobalBounds(); 
	bool zoneFinAtteinte = zoneFin.intersects(perimetreVoiture);
	m_dureeVie = m_horlogeInterne.getElapsedTime();

	return (m_crash || zoneFinAtteinte || m_dureeVie.asSeconds() > 60.0f || m_vitesse == 0); // La voiture s'arrete si elle s'est crashée, ou a atteinte la zone de fin , ou que le temps aloué est écoulé
}

bool Voiture::conditionsLimiteVerifie(float vitesse, float angle) // Permet de determiner si les composantes angle de rotation / vitesse de la voiture sont physiquement compatibles
{
	// cf calculs physiques/géométriques
	float rayon = m_longueur / std::abs(std::tan(angle));
	return ((vitesse*vitesse <= (m_largeur * g *rayon) / (50 * m_hauteur)) && (vitesse * vitesse <= mu * g * rayon));
}

/*
Fonctions pour gérer la vitesse et la direction de déplacement
On verifie à chaque fois que la voiture peut tourner/accellerer avec la fonction "conditionsLimitesVerifiees"
*/
void Voiture::freiner()
{
	m_vitesse -= m_deceleration / 10;
	if (m_vitesse < 0)
	{
		m_vitesse = 0;
	}
}

void Voiture::accelerer()
{
	if (conditionsLimiteVerifie(m_vitesse + m_acceleration / 10, m_angleRoues) && m_vitesse + m_acceleration / 10 <= m_vitesseMax)
	{
		m_vitesse += m_acceleration / 10;
	}
}

void Voiture::tournerDroite(float const angle)
{
	if (conditionsLimiteVerifie(m_vitesse, angle))
	{
		m_angleRoues = angle;
	}
}

void Voiture::tournerGauche(float const angle)
{
	if (conditionsLimiteVerifie(m_vitesse, -angle))
	{
		m_angleRoues = -angle;
	}
}


std::vector<float> Voiture::acquerirDonnes(sf::Image &terrain) // Fonction qui permet à la voiture de "lire" la route
{
	std::vector<float> donnees; // Le vecteur des données acquisitionnées

	for (int i = 0; i < m_anglesCapteurs.size(); i++) // On rempli le vecteur donnees avec un signal qui rend compte de la distance du mur le plus proche de la voiture dans une certaine direction
	{
		float angle(m_angleDirection + m_anglesCapteurs[i]);
		float distanceCapteur(0.0f);
		float XPointEtudie(m_positionCapteur.x), YPointEtudie(m_positionCapteur.y);
		m_lignesCapteurs[i][0].position = m_positionCapteur;

		while (distanceCapteur < m_distanceCapteurMax && terrain.getPixel(std::floor(XPointEtudie), std::floor(YPointEtudie)) == sf::Color::White)
		{
			distanceCapteur += 5.0f;
			XPointEtudie += 5.0f * std::sin(angle); YPointEtudie -= 5.0f * std::cos(angle); // On verifie que chaque point est sur la route jusqu'à une limite de distance
		}

		m_lignesCapteurs[i][1].position = sf::Vector2f(XPointEtudie, YPointEtudie);

		donnees.push_back(1 - distanceCapteur / m_distanceCapteurMax); // Plus le capteur est proche d'un mur, plus le signal est fort (entre 0 et 1)
	}

	return donnees; // On renvoie le vecteur composé des données acquisitionnées
}

void Voiture::priseDecision(std::vector<float> donnees) // Fonction décisionnelle de la voiture
{
	Matrice Neurones(m_cerveauVoiture.reponseStimuli(donnees)); // Le cerveau analyse les données reccueillies

	float NeuroneAcceleration(Neurones.getValeur(1, 1));
	float NeuroneFrein(Neurones.getValeur(2, 1));
	float NeuroneTournerDroite(Neurones.getValeur(3, 1));
	float NeuroneTournerGauche(Neurones.getValeur(4, 1));

	if (NeuroneAcceleration > 0.5f)
	{
		accelerer(); // Si le Neurone d'acceleration est prédominant : La voiture décide d'accellerer
	}
	if (NeuroneFrein > 0.5f)
	{
		freiner(); // Si le Neurone de freinage est prédominant: ... freiner
	}
	if (NeuroneTournerDroite > 0.5f)
	{
		tournerDroite((NeuroneTournerDroite - 0.5f)*m_angleMaxRoues * 2); // ... etc
	}
	if (NeuroneTournerGauche > 0.5f)
	{
		tournerGauche((NeuroneTournerGauche - 0.5f)*m_angleMaxRoues * 2);
	}

}


void Voiture::deplacer()
{
	float R(m_longueur / std::abs(std::tan(m_angleRoues)));  // Quelques calculs géométriques
	float theta(m_vitesse / 10 / R);

	if (m_angleRoues > 0)
	{
		m_angleDirection += theta;
		m_imageVoiture.rotate(theta * 180 / PI); // Rotation de la voiture cf calculs géométriques
	}
	else
	{
		m_angleDirection -= theta;
		m_imageVoiture.rotate(-theta * 180 / PI); // Rotation de la voiture cf calculs géométriques
	}

	m_distanceParcourue += m_vitesse / 10;

	m_imageVoiture.move(m_vitesse / 10 * std::sin(m_angleDirection), -m_vitesse / 10 * std::cos(m_angleDirection)); // Déplacement de la voiture cf calculs géométriques
	m_positionCapteur.x = m_imageVoiture.getPosition().x + m_longueur * std::sin(m_angleDirection);
	m_positionCapteur.y = m_imageVoiture.getPosition().y - m_longueur*std::cos(m_angleDirection);

}



/* FONCTIONS GERANT LA PARTIE GENETIQUE */

void Voiture::setAdaptation()
{
	if (m_crash || m_dureeVie.asSeconds() > 60.0f || m_vitesse == 0)
	{
		m_adaptation = m_distanceParcourue; // Si la voiture n'a pas fini le parcours, son adaptation est la distance parcourue
	}
	else
	{
		m_adaptation = m_distanceParcourue + m_distanceParcourue / m_dureeVie.asSeconds(); // Si elle a fini le parcours, son adaptation est égale a la distance + la distance/temps de parcours pour favoriser les voitures rapides
	}
}

float Voiture::getAdaptation()
{
	return m_adaptation;
}

std::vector<float> Voiture::getADN()
{
	return m_ADN;
}


std::vector<Voiture> Voiture::genererPopulation(int taillePop,unsigned int &seed) // Fonction qui permet de créer une population de voitures générées aléatoirement (ADN aléatoire)
{	
	std::vector<Voiture> population;
	for (int i = 0; i < taillePop; i++)
	{
		Voiture v(seed);
		population.push_back(v);
	}
	return population;
}

std::vector<Voiture> Voiture::selectionNaturelle(std::vector<Voiture> population, float p, unsigned int &seed) // Fonction de séléction des voitures
{
	std::vector<Voiture> survivants;
	int n = population.size();

	/* Initialisations des générateurs aléatoires */
	std::uniform_real_distribution<float> distribution(0.0, 1.0);
	
	std::default_random_engine generator(seed);
	seed += 1000;
	
	for(int i = 0; i < n - 1 ; i+=2) // Tant qu'il reste des individus à comparer
	{	
		Voiture gagnant;
		Voiture challenger1 = population[i];
		Voiture challenger2 = population[i + 1];
		/* Fight ! */
		float nombre = distribution(generator);
		if (challenger1.m_adaptation > challenger2.m_adaptation) 
		{
			if (nombre > p) // On laisse une chance au candidat défavorisé de voler la victoire > permet de maintenir la DIVERSITY
			{
				gagnant = Voiture(challenger1.m_ADN);
			}
			else
			{
				gagnant = Voiture(challenger2.m_ADN);
			}
		}
		else
		{
			if (nombre > p)
			{
				gagnant = Voiture(challenger2.m_ADN);
			}
			else
			{
				gagnant = Voiture(challenger1.m_ADN);
			}
		}

		survivants.push_back(gagnant);
	}
	return survivants; // On renvoie la liste des survivants
}

std::vector<Voiture> Voiture::mutationPopulation(std::vector<Voiture> population, float p, unsigned int &seed) // On recrée une population mutée a partir de la population des survivants
{	
	/* Initialisations des générateurs aléatoires */
	std::default_random_engine generator(seed);
	seed += 1000;
	
	std::uniform_real_distribution<float> distrib(0, 1);
	std::uniform_real_distribution<float> distribution(-10, 10);

	std::vector<Voiture> mutants;
	for (int i = 0; i < population.size(); i++)
	{
		for (int j = 0; j < population[i].m_ADN.size(); j++) // Chaque gène de chaque individu a une chance d'être modifié aléatoirement
		{
			float nombre = distrib(generator);
			if (nombre < p)
			{
				population[i].m_ADN[j] = distribution(generator);
			}
		}
		mutants.push_back(Voiture(population[i].m_ADN));
	}
	return mutants; // On renvoie la population des mutants
}

std::vector<Voiture> Voiture::reproduction(std::vector<Voiture> population, unsigned int & seed) // Fonction de reproduction des survivants
{	
	/* Initialisations des générateurs aléatoires */
	std::default_random_engine generator(seed);
	seed += 1000;

	std::uniform_real_distribution<float> distribution(0, 1);
	int n = population[0].m_ADN.size();
	std::vector<Voiture> enfants;

	/* On croise chaque survivant de la liste avec le suivant */
	for (int i = 0; i < population.size() - 1 ; i++)
	{	
		std::vector<float> ADNEnfant;
		for (int j = 0; j < n; j++)
		{	
			float nombre = distribution(generator);
			if (nombre > 0.5) // Selon la parité de j, l'enfant recoit un gène du parent 1 ou du parent 2
			{
				ADNEnfant.push_back(population[i].m_ADN[j]); 
			}
			else
			{
				ADNEnfant.push_back(population[i+1].m_ADN[j]);
			}
		}
		enfants.push_back(Voiture(ADNEnfant));
	}

	/* On croise le premier et le dernier des survivants de la liste 
	> On obitient une liste d'enfants de même taille que la liste des parents 
	> Permet de garder un nombre pair d'individus dans la population (facilite les calculs)
	*/
	std::vector<float> ADNEnfant;
	for (int i = 0; i < n; i++)
	{	
		if (i % 2)
		{
			ADNEnfant.push_back(population[population.size() - 1].m_ADN[i]);
		}
		else
		{
			ADNEnfant.push_back(population[0].m_ADN[i]);
		}
	}
	enfants.push_back(Voiture(ADNEnfant));
	
	return enfants; // On renvoie la liste des enfants
}


