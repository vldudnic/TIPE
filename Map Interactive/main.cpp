/* 
FICHIER MAIN 

BOUCLE PRINCIPALE DE L'ALGORITHME
AFFICHAGE DES VOITURES ET DE LA ROUTE
*/


#pragma once
#include <algorithm>
#include "Voiture_Prototypes.h"
#define ps 0.10f
#define pm 0.01f
#define TAILLE_POP 10
#define GENERATION_MAX 50

/* Deux fonctions pour la gestion des listes */
std::vector<Voiture> concatener(std::vector<Voiture> vector1, std::vector<Voiture> vector2);
float moyenne(std::vector<float> vector1);

int main()
{	
	unsigned int seed = (unsigned int)time(NULL);

	/* Initialisation des fichiers */
	const std::string fichierPolice("monofonto.ttf");
	const std::string fichierTerrain("Route2.png");
	const std::string fichierTerrain2("Route3.png");

	/* Initialisation de la police */
	sf::Font font;
	if (!font.loadFromFile(fichierPolice))
	{
		std::cout << "ERREUR POLICE" << std::endl;
		std::system("PAUSE");
		return 0;
	}


	/* PREMIERE ROUTE */

	/* Initialisation de l'image terrain, et de la texture */
	sf::Image imageTerrain;
	if (!imageTerrain.loadFromFile(fichierTerrain))
	{
		std::cout << "Impossible de charge l'image\n";
		std::system("PAUSE");
		return 0;
	}
	sf::Texture textureTerrain;
	textureTerrain.loadFromFile(fichierTerrain);
	textureTerrain.setSmooth(true);
	sf::Sprite spriteTerrain;
	spriteTerrain.setTexture(textureTerrain);
	spriteTerrain.setPosition(0, 0);
	
	/* Initialisation de la zone d'arrivée du parcours */
	sf::RectangleShape zoneFin;
	zoneFin.setFillColor(sf::Color::Transparent);
	zoneFin.setOutlineThickness(3);
	zoneFin.setOutlineColor(sf::Color::Red);
	zoneFin.setSize(sf::Vector2f(80, 270));
	zoneFin.setPosition(1700, 155);
	sf::FloatRect zoneArrivee = zoneFin.getGlobalBounds(); // On utilisera uniquement la boite englobante zoneArrivee

	float adaptationMoyenne = 0;
	float meilleureAdaptation = 0;

	sf::Text texteAdaptation;
	texteAdaptation.setFont(font);
	texteAdaptation.setFillColor(sf::Color::Black);
	texteAdaptation.setCharacterSize(20);
	texteAdaptation.setPosition(1050, 30);

	sf::RenderWindow mainWindow(sf::VideoMode(1920, 1080, 32), "Fenetre"); // Création de la fenêtre en plein écran
	//mainWindow.setFramerateLimit(200);

	
	std::vector<Voiture> population(Voiture::genererPopulation(TAILLE_POP, seed)); // Création de la population

	for (int i = 0; i < GENERATION_MAX; i++) // On boucle sur toutes les générations
	{	

		texteAdaptation.setString("Génération: " + std::to_string(i) + "  | Adaptation Moyenne: " + std::to_string(adaptationMoyenne) + " | Adaptation Maximale: " + std::to_string(meilleureAdaptation));

		std::vector<float> listeAdapt;

		for (int j = 0; j < population.size(); j++) // On boucle pour chaque voiture de la population
		{	
			population[j].demarer();
			while (!population[j].arreter(zoneArrivee)) // Tant que la voiture ne s'est pas arretée 
			{
				/* Fonctions de commande de la voiture */
				population[j].cycleDecision(imageTerrain);
				population[j].setCrash(imageTerrain);

				/* Fonctions d'affichage */
				if (!(i%10))
				{
					mainWindow.clear(sf::Color::White);
					mainWindow.draw(spriteTerrain);
					mainWindow.draw(zoneFin);
					mainWindow.draw(texteAdaptation);
					population[j].afficher(mainWindow);
					mainWindow.display();
				}
			}

			/* Gestion de l'adaptation de la voiture */
			population[j].setAdaptation();
			listeAdapt.push_back(population[j].getAdaptation());
		}

		mainWindow.clear(sf::Color::White);
		mainWindow.draw(spriteTerrain);
		mainWindow.draw(texteAdaptation);
		mainWindow.display();

		/* Afficahge de l'adaptation maximale et moyenne de la génération i */
		std::sort(listeAdapt.begin(), listeAdapt.end());
		adaptationMoyenne = moyenne(listeAdapt);
		meilleureAdaptation = listeAdapt[listeAdapt.size() - 1];

		/* Fonctions génétiques */
		std::vector<Voiture> survivants = Voiture::selectionNaturelle(population, ps, seed);
		std::vector<Voiture> mutants = Voiture::mutationPopulation(survivants, pm, seed);
		std::vector<Voiture> enfants = Voiture::reproduction(mutants, seed);
		population = concatener(mutants, enfants);
	}



	/* DEUXIEME ROUTE */
	/* On reprend les mêmes fonctions, mais on charge une autre route */

	mainWindow.clear();

	sf::Image imageTerrain2;
	if (!imageTerrain2.loadFromFile(fichierTerrain2))
	{
		std::cout << "Impossible de charge l'image\n";
		std::system("PAUSE");
		return 0;
	}
	sf::Texture textureTerrain2;
	textureTerrain2.loadFromFile(fichierTerrain2);
	textureTerrain2.setSmooth(true);
	sf::Sprite spriteTerrain2;
	spriteTerrain2.setTexture(textureTerrain2);
	spriteTerrain2.setPosition(0, 0);

	sf::RectangleShape zoneFin2;
	zoneFin2.setFillColor(sf::Color::Transparent);
	zoneFin2.setOutlineThickness(3);
	zoneFin2.setOutlineColor(sf::Color::Red);
	zoneFin2.setSize(sf::Vector2f(180,60));
	zoneFin2.setPosition(650, 50);
	sf::FloatRect zoneArrivee2 = zoneFin2.getGlobalBounds(); 

	for (int i = 0; i < GENERATION_MAX; i++) // On boucle sur toutes les générations
	{

		texteAdaptation.setString("Génération: " + std::to_string(i) + "  | Adaptation Moyenne: " + std::to_string(adaptationMoyenne) + " | Adaptation Maximale: " + std::to_string(meilleureAdaptation));

		std::vector<float> listeAdapt;

		for (int j = 0; j < population.size(); j++) // On boucle pour chaque voiture de la population
		{
			population[j].demarer();
			while (!population[j].arreter(zoneArrivee2)) // Tant que la voiture ne s'est pas arretée 
			{
				/* Fonctions de commande de la voiture */
				population[j].cycleDecision(imageTerrain2);
				population[j].setCrash(imageTerrain2);

				/* Fonctions d'affichage */
				if (!(i % 10))
				{
					mainWindow.clear(sf::Color::White);
					mainWindow.draw(spriteTerrain2);
					mainWindow.draw(zoneFin2);
					mainWindow.draw(texteAdaptation);
					population[j].afficher(mainWindow);
					mainWindow.display();
				}
			}

			/* Gestion de l'adaptation de la voiture */
			population[j].setAdaptation();
			listeAdapt.push_back(population[j].getAdaptation());
		}

		mainWindow.clear(sf::Color::White);
		mainWindow.draw(spriteTerrain2);
		mainWindow.draw(texteAdaptation);
		mainWindow.display();

		/* Afficahge de l'adaptation maximale et moyenne de la génération i */
		std::sort(listeAdapt.begin(), listeAdapt.end());
		adaptationMoyenne = moyenne(listeAdapt);
		meilleureAdaptation = listeAdapt[listeAdapt.size() - 1];

		/* Fonctions génétiques */
		std::vector<Voiture> survivants = Voiture::selectionNaturelle(population, ps, seed);
		std::vector<Voiture> mutants = Voiture::mutationPopulation(survivants, pm, seed);
		std::vector<Voiture> enfants = Voiture::reproduction(mutants, seed);
		population = concatener(mutants, enfants);
	}

	std::system("PAUSE");
	return 0;
}



std::vector<Voiture> concatener(std::vector<Voiture> vector1, std::vector<Voiture> vector2) // Concatener deux listes de voitures
{
	std::vector<Voiture> temp;
	for (int i = 0; i < vector1.size(); i++)
	{
		temp.push_back(vector1[i]);
	}
	for (int i = 0; i < vector2.size(); i++)
	{
		temp.push_back(vector1[i]);
	}
	return temp;
}

float moyenne(std::vector<float> vector1) // Calculer la valeure moyenne d'une liste de valeures
{
	float somme(0);
	for (int i = 0; i < vector1.size(); i++)
	{
		somme += vector1[i];
	}
	return somme / vector1.size();
}
		