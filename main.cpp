/**
 * @file main.cpp
 * @brief Programme principale
 *
 * Ce programme EST UN LOGICIEL LIBRE. Vous pouvez le redistribuer.
 */

/**
 * Au dessus Doxygen-style documentation, qui facilite
 * la generation des doc doxygen.
 */

#include <iostream>
#include <fstream>
#include <string>
#include "Graphe_or.h"

using namespace std;

void separer() {
	cout << "\n\n*********************************************************"
			<< "***********************************************************\n\n";
}

int main() {
	while (1) {
		ifstream monFlux;
		separer();
		cout << "\tVeuillez choisir un fichier" << endl << "[1]. Test1.txt"
				<< endl << "[2]. Test2.txt" << endl << "[3]. Test3.txt" << endl
				<< "[4]. Test4.txt" << endl << "[5]. Test5.txt" << endl
				<< "[6]. Test6.txt (l'exemple du cours magistral)" << endl
				<< "[7]. Terminer le programme" << endl;
		int choix { };
		cout << "Votre choix : ";
		cin >> choix;
		if (choix >= 1 && choix <= 6)
			monFlux.open("Test" + to_string(choix) + ".txt");
		else if (choix == 7) {
			cout << "Programme termine" << endl << "Bye Bye :)" << endl;
			monFlux.close();
			exit(EXIT_SUCCESS);
		} else {
			cout << "Choix incorrect, veuillez resaisir :" << endl;
			continue;
		}

		if (!monFlux) {
			cout << "Le fichier n'existe pas" << endl;
			exit(EXIT_FAILURE);
		}

		Graphe_or G;
		monFlux >> G.nb_taches;
		G.nb_sommets = G.nb_taches + 2;

		for (int i { G.nb_taches }; i--;) {
			/**
			 * Initialisation avec accolade pour des types built-in
			 * (depuis c++11)
			 */
			char c { }, nom { };
			int duree { };
			string pred;
			monFlux >> nom >> duree;
			monFlux.get(c);
			if (c == ' ')
				monFlux >> pred;
			G.taches.push_back( { nom, duree, pred });
		}

		/* Ranged-for (depuis c++11) */
		for (auto & p : G.taches)
			G.taches_map[p.nom] = &p;

		/**
		 * Calculer les successeurs de toutes les taches
		 * Trouver les etats initiaux.
		 */
		for (auto & p : G.taches) {
			G.etats += p.nom;
			if (!p.pred.empty()) {
				G.contraintes += p.pred;
				for (auto & q : p.pred)
					G.taches_map[q]->succ += p.nom;
			} else
				G.etats_initiaux += p.nom;
		}

		/* Trouver les etats finals. */
		for (auto & p : G.etats)
			if (G.contraintes.find(p) == string::npos)
				G.etats_finals += p;

		G.map_arc_init();

		separer();
		cout << "\nCreation du graphe\n" << "----------------------\n"
				<< "Proprietes du graphe :\n" << "- nombre de sommets : "
				<< G.nb_sommets << "\n" << "- graphe oriente\n"
				<< "- 1 valeur (numerique) pour chaque arc\n"
				<< "- maximum 1 arc d'un sommet X donne vers un sommet Y donne\n"
				<< "--> Ajout des sommets \"debut de projet\""
				<< "('a') et \"fin de projet\" ('z')\n"
				<< "--> Creation de la matrice d’adjacence :"
				<< "Madj[x][y] = vrai si arc de x vers y\n"
				<< "\t\t\tinitialise a faux\n"
				<< "\tde la matrice des valeurs : Mval[x][y] = valeur de l'arc s'il existe\n"
				<< "--> Creation des arcs associes aux contraintes"
				<< "de type \"X ne peut commencer que lorsque Y est terminee\""
				<< endl;

		for (auto & p : G.map_arc)
			cout << p.second << endl;

		cout << "--> Recherche des points d'entree et points de sortie"
				" - Ajout des arcs incidents au debut et a la fin de projet"
				<< endl;

		for (auto & p : G.etats_initiaux)
			cout << Arc { 'a', 0, p } << endl;
		for (auto & p : G.etats_finals)
			cout << Arc { p, G.taches_map[p]->duree, 'z' } << endl;

		separer();

		/**
		 * Detecter des circuits dans le graphe oriente, abandonner le programme
		 * s'il existe au moins un circuit.
		 */
		if (G.top_sort()) {
			cout << "Le programme ne peut plus continuer" << endl;
			continue;
		}

		/* Initialiser la matrice d'adjacence et la matrice des valeurs. */
		G.initialiser_matrice();
		/* Remplir la matrice d'adjacence et la matrice des valeurs. */
		G.remplir_matrice();

		separer();
//		G.afficher_matrice_adj();
		G.afficher_matrice(0);

		separer();
//		G.afficher_matrice_val();
		G.afficher_matrice(1);

		/* Calculer les rangs pour toutes les taches. */
		G.set_rang();
		/* Trouver le rang maximum de toutes les taches. */
		G.set_max_rang();
		/**
		 * Etablir une mini table de hashage en fonction des rangs
		 * anciennement calcules.
		 */
		G.set_hash_table();
		/* Calculer la date au plus tot pour toutes les taches. */
		G.calculer_date_tot();
		/* Calculer la date au plus tard pour toutes les taches. */
		G.calculer_date_tard();

		separer();
		/* Afficher toutes les taches. */
		G.afficher_taches();

		separer();
		/**
		 * Afficher le diagramme de gantt selon les dates au plus tot
		 * de toutes les taches.
		 */
		G.afficher_gantt(0);

		separer();
		/**
		 * Afficher le diagramme de gantt selon les dates au plus tard
		 * de toutes les taches.
		 */
		G.afficher_gantt(1);

		/* Liberer la memoire anciennement allouee a la matrice d'adjacence. */
		G.liberer_matrice();
		separer();
		cout << "Le programme a termine avec success." << endl;
	}
}
