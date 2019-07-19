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

/* Constructeurs et destrcuteurs par d�faut */
Voiture::Voiture()
{}
Voiture::~Voiture()
{}

/* Constructeur al�atoire de la voiture */
Voiture::Voiture(unsigned int &seed) :
	m_longueur(LONGUEUR), m_largeur(LARGEUR), m_hauteur(HAUTEUR), m_angleMaxRoues(ANGLE_MAX_ROUES), m_angleRoues(0), m_angleDirection(0), m_distanceCapteurMax(DISTANCE_CAPTEURS_MAX), m_vitesseMax(VITESSE_MAX), m_acceleration(ACCELERATION), m_deceleration(DECELERATION), m_vitesse(6), m_crash(false) // On initialise quelques valeurs num�riques
{
	/* Image de la voiture */
	m_imageVoiture.setSize(sf::Vector2f(m_largeur, m_longueur));
	m_imageVoiture.setFillColor(sf::Color::Black);
	m_imageVoiture.setOrigin(m_largeur / 2, m_longueur);
	m_imageVoiture.setPosition(X, Y); // Position arbitraire, correspondra � la position initiale sur le circuit

	/* Position du capteur (position de d�part des mesures de distances) 
	Le capteur se trouve � l'avant de la voiture � �gale distance des deux roues */
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
	std::vector<int> taillesCouches({ n,5,5,4 }); // Il y a autant de neurones sur la 1�re couche du r�seau qu'il y a de capteurs

	/* Cr�ation de l'ADN de la voiture: la taille de l'ADN est la somme des tailles des matrices (un g�ne pour chaque co�ficient) */
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
		m_ADN.push_back(nombre); // Initialement les coeficients de l'ADN sont al�atoires
	}
	m_cerveauVoiture = ReseauNeurones(taillesCouches, m_ADN); // Le cerveau de la voiture est alors cr�� pour rendre compte de cet ADN 

	m_adaptation = 0.0f; // Initialisation de l'adaptation � 0 (obviously). L'adaptation sera recalcul�e plus tard
}

/* Variante du constructeur (on connait d�j� l'ADN) */
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


void Voiture::cycleDecision(sf::Image & terrain) // Cycle principal de d�cisions d'une voiture
{	
	std::vector<float> donnees(acquerirDonnes(terrain)); // Acquisition des donn�es
	priseDecision(donnees); // Traitement des donn�es
	deplacer(); // Execution des d�cisions prises
}


void Voiture::afficher(sf::RenderWindow & window) // Fonction d'affichage basique
{
	window.draw(m_imageVoiture);
	for (int i = 0; i < m_lignesCapteurs.size(); i++)
	{
		window.draw(m_lignesCapteurs[i]);
	}
}


void Voiture::setCrash(sf::Image & terrain) // Determine si la voiture s'est crash�e dans un mur
{
	sf::Vector2f position = m_positionCapteur; // On ne consid�re qu'un seul point � l'avant de la voiture
	if (terrain.getPixel(std::floor(position.x), std::floor(position.y)) != sf::Color::White) // Si ce point n'est pas sur la route: Crash
	{
		m_crash = true;
	}
}


void Voiture::demarer() // D�marage de la voiture: on r�initialise certaines constantes
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

	return (m_crash || zoneFinAtteinte || m_dureeVie.asSeconds() > 60.0f || m_vitesse == 0); // La voiture s'arrete si elle s'est crash�e, ou a atteinte la zone de fin , ou que le temps alou� est �coul�
}

bool Voiture::conditionsLimiteVerifie(float vitesse, float angle) // Permet de determiner si les composantes angle de rotation / vitesse de la voiture sont physiquement compatibles
{
	// cf calculs physiques/g�om�triques
	float rayon = m_longueur / std::abs(std::tan(angle));
	return ((vitesse*vitesse <= (m_largeur * g *rayon) / (50 * m_hauteur)) && (vitesse * vitesse <= mu * g * rayon));
}

/*
Fonctions pour g�rer la vitesse et la direction de d�placement
On verifie � chaque fois que la voiture peut tourner/accellerer avec la fonction "conditionsLimitesVerifiees"
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


std::vector<float> Voiture::acquerirDonnes(sf::Image &terrain) // Fonction qui permet � la voiture de "lire" la route
{
	std::vector<float> donnees; // Le vecteur des donn�es acquisitionn�es

	for (int i = 0; i < m_anglesCapteurs.size(); i++) // On rempli le vecteur donnees avec un signal qui rend compte de la distance du mur le plus proche de la voiture dans une certaine direction
	{
		float angle(m_angleDirection + m_anglesCapteurs[i]);
		float distanceCapteur(0.0f);
		float XPointEtudie(m_positionCapteur.x), YPointEtudie(m_positionCapteur.y);
		m_lignesCapteurs[i][0].position = m_positionCapteur;

		while (distanceCapteur < m_distanceCapteurMax && terrain.getPixel(std::floor(XPointEtudie), std::floor(YPointEtudie)) == sf::Color::White)
		{
			distanceCapteur += 5.0f;
			XPointEtudie += 5.0f * std::sin(angle); YPointEtudie -= 5.0f * std::cos(angle); // On verifie que chaque point est sur la route jusqu'� une limite de distance
		}

		m_lignesCapteurs[i][1].position = sf::Vector2f(XPointEtudie, YPointEtudie);

		donnees.push_back(1 - distanceCapteur / m_distanceCapteurMax); // Plus le capteur est proche d'un mur, plus le signal est fort (entre 0 et 1)
	}

	return donnees; // On renvoie le vecteur compos� des donn�es acquisitionn�es
}

void Voiture::priseDecision(std::vector<float> donnees) // Fonction d�cisionnelle de la voiture
{
	Matrice Neurones(m_cerveauVoiture.reponseStimuli(donnees)); // Le cerveau analyse les donn�es reccueillies

	float NeuroneAcceleration(Neurones.getValeur(1, 1));
	float NeuroneFrein(Neurones.getValeur(2, 1));
	float NeuroneTournerDroite(Neurones.getValeur(3, 1));
	float NeuroneTournerGauche(Neurones.getValeur(4, 1));

	if (NeuroneAcceleration > 0.5f)
	{
		accelerer(); // Si le Neurone d'acceleration est pr�dominant : La voiture d�cide d'accellerer
	}
	if (NeuroneFrein > 0.5f)
	{
		freiner(); // Si le Neurone de freinage est pr�dominant: ... freiner
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
	float R(m_longueur / std::abs(std::tan(m_angleRoues)));  // Quelques calculs g�om�triques
	float theta(m_vitesse / 10 / R);

	if (m_angleRoues > 0)
	{
		m_angleDirection += theta;
		m_imageVoiture.rotate(theta * 180 / PI); // Rotation de la voiture cf calculs g�om�triques
	}
	else
	{
		m_angleDirection -= theta;
		m_imageVoiture.rotate(-theta * 180 / PI); // Rotation de la voiture cf calculs g�om�triques
	}

	m_distanceParcourue += m_vitesse / 10;

	m_imageVoiture.move(m_vitesse / 10 * std::sin(m_angleDirection), -m_vitesse / 10 * std::cos(m_angleDirection)); // D�placement de la voiture cf calculs g�om�triques
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
		m_adaptation = m_distanceParcourue + m_distanceParcourue / m_dureeVie.asSeconds(); // Si elle a fini le parcours, son adaptation est �gale a la distance + la distance/temps de parcours pour favoriser les voitures rapides
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


std::vector<Voiture> Voiture::genererPopulation(int taillePop,unsigned int &seed) // Fonction qui permet de cr�er une population de voitures g�n�r�es al�atoirement (ADN al�atoire)
{	
	std::vector<Voiture> population;
	for (int i = 0; i < taillePop; i++)
	{
		Voiture v(seed);
		population.push_back(v);
	}
	return population;
}

std::vector<Voiture> Voiture::selectionNaturelle(std::vector<Voiture> population, float p, unsigned int &seed) // Fonction de s�l�ction des voitures
{
	std::vector<Voiture> survivants;
	int n = population.size();

	/* Initialisations des g�n�rateurs al�atoires */
	std::uniform_real_distribution<float> distribution(0.0, 1.0);
	
	std::default_random_engine generator(seed);
	seed += 1000;
	
	for(int i = 0; i < n - 1 ; i+=2) // Tant qu'il reste des individus � comparer
	{	
		Voiture gagnant;
		Voiture challenger1 = population[i];
		Voiture challenger2 = population[i + 1];
		/* Fight ! */
		float nombre = distribution(generator);
		if (challenger1.m_adaptation > challenger2.m_adaptation) 
		{
			if (nombre > p) // On laisse une chance au candidat d�favoris� de voler la victoire > permet de maintenir la DIVERSITY
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

std::vector<Voiture> Voiture::mutationPopulation(std::vector<Voiture> population, float p, unsigned int &seed) // On recr�e une population mut�e a partir de la population des survivants
{	
	/* Initialisations des g�n�rateurs al�atoires */
	std::default_random_engine generator(seed);
	seed += 1000;
	
	std::uniform_real_distribution<float> distrib(0, 1);
	std::uniform_real_distribution<float> distribution(-10, 10);

	std::vector<Voiture> mutants;
	for (int i = 0; i < population.size(); i++)
	{
		for (int j = 0; j < population[i].m_ADN.size(); j++) // Chaque g�ne de chaque individu a une chance d'�tre modifi� al�atoirement
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
	/* Initialisations des g�n�rateurs al�atoires */
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
			if (nombre > 0.5) // Selon la parit� de j, l'enfant recoit un g�ne du parent 1 ou du parent 2
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
	> On obitient une liste d'enfants de m�me taille que la liste des parents 
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


