/*
FICHIER VOITURE

DEFINITION DES CONSTANTES PHYSIQUES NECESSAIRES AU FONCTIONNEMENT DE LA VOITURE

CLASSE VOITURE :
GERE LE FONCTIONEMENT PHYSIQUE DE LA VOITURE
GERE L'EVOLUTION D'UNE POPULATION DE VOITURES (partie génétique)
*/


#pragma once
#include <ctime> 
#include <cstdlib> 
#include <SFML/Graphics.hpp>
#include "Neurones_Prototypes.h"

/* Definition des constantes physiques */
#define g 9.81f
#define mu 0.8f
#define PI 3.14f

/* Definition des constantes liées aux voitures */
#define X 210.0f
#define Y 950.0f
#define LONGUEUR 50.0f
#define LARGEUR 20.0f
#define HAUTEUR 15.0f
#define ANGLE_MAX_ROUES PI/6
#define DISTANCE_CAPTEURS_MAX 250.0f
#define VITESSE_MAX 30.0f
#define ACCELERATION 3.0f
#define DECELERATION 3.0f
#define ANGLES_CAPTEURS std::vector<float>({-PI/3,-PI/6,0,PI/6,PI/3})


class Voiture
{

/* PARTIE PHYSIQUE */
public:
	Voiture(); 
	Voiture(unsigned int &seed); // Constructeur aléatoire
	Voiture(std::vector<float> ADN); // Constructeur à partir d'un ADN connu
	void cycleDecision(sf::Image &terrain);
	void afficher(sf::RenderWindow &window);
	void setCrash(sf::Image &terrain);
	void demarer();
	bool arreter(sf::FloatRect &zoneFin);
	~Voiture();

private:
	sf::RectangleShape m_imageVoiture; // Representation graphique de la voiture

	/* Composantes constantes de la voiture */
	float m_longueur;
	float m_largeur;
	float m_hauteur;
	float m_angleMaxRoues;
	float m_distanceCapteurMax;
	float m_vitesseMax;
	float m_acceleration; // Considérée constante (lorsque la voiture accelère) égale à l'acelleration maximale
	float m_deceleration; // Idem
	std::vector<float> m_anglesCapteurs;

	/* Composantes variables de la voiture */
	float m_angleRoues;
	float m_vitesse;
	float m_angleDirection;
	sf::Vector2f m_positionCapteur;
	std::vector<sf::VertexArray> m_lignesCapteurs;

	/* Autres variables */
	float m_distanceParcourue;
	sf::Clock m_horlogeInterne;
	sf::Time m_dureeVie;
	bool m_crash;
	
	ReseauNeurones m_cerveauVoiture; // Centre de décision (IA)

	/* Fonctions auxiliaires */
	bool conditionsLimiteVerifie(float vitesse, float angle);
	void freiner();
	void accelerer();
	void tournerDroite(float const angle);
	void tournerGauche(float const angle);
	std::vector<float> acquerirDonnes(sf::Image &terrain);
	void priseDecision(std::vector<float> donnes);
	void deplacer();



/* PARTIE GENETIQUE */
public:
	void setAdaptation(); 
	float getAdaptation();
	std::vector<float> getADN();

	static std::vector<Voiture> genererPopulation(int taillePop, unsigned int &seed);
	static std::vector<Voiture> selectionNaturelle(std::vector<Voiture> population, float p, unsigned int &seed);
	static std::vector<Voiture> mutationPopulation(std::vector<Voiture> population, float p, unsigned int &seed);
	static std::vector<Voiture> reproduction(std::vector<Voiture> population, unsigned int & seed);
	
private:
	std::vector<float> m_ADN;
	float m_adaptation;



};
