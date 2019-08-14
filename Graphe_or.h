/**
 * @file main.cpp
 * @brief Programme principale
 * @date 4 Jan 2017
 *
 * Copyright 2017
 * Ce programme EST UN LOGICIEL LIBRE. Vous pouvez le redistribuer.
 */

/**
 * Au dessus Doxygen-style documentation, qui facilite
 * la generation des doc doxygen.
 */

#ifndef GRAPHE_OR_H_
#define GRAPHE_OR_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include "Tache.h"
#include "Arc.h"

using namespace std;

/* Graphe oriente pour un ordonnancement. */
typedef struct Graphe_ordonnancement {
	int nb_taches { };
	/**
	 * Nombre de sommets du graphe, egal a nombre de taches plus 2,
	 * car il y deux sommets fictifs (un est libelle "a" et l'autre
	 * "z", represantant respectivement l'etat au debut et l'etat a la fin).
	 */
	int nb_sommets { };
	/**
	 * Map prenant le nom du tache comme cle, et un pointeur vers cette
	 * tache comme valeur. Ce map nous permet de faire reference a une
	 * tache facilement en utilisant sont nom.
	 */
	map<char, Tache*> taches_map;
	/* Vecteur contenant toutes les instances des taches. */
	vector<Tache> taches;
	/**
	 * Chaine de caracteres qui est la concatenation de toutes les noms
	 * des taches n'ayant pas de predecesseurs (contraintes).
	 */
	string etats_initiaux;
	/**
	 * Chaine de caracteres qui est la concatenation de toutes les noms
	 * des taches n'ayant pas de successeurs.
	 */
	string etats_finals;
	/**
	 * Chaine de caracteres qui est la concatenation de toutes les noms
	 * des taches.
	 */
	string etats;
	/**
	 * Chaine de caracteres qui est la concatenation de toutes les noms
	 * des predecesseurs (contraintes) des taches.
	 */
	string contraintes;
	/**
	 * La matrice d'adjacence. Initialisation des variables membres en
	 * utilisant des accolades (depuis c++11).
	 */
	int **Madj { };
	/**
	 * La matrice des valeurs. Initialisation des variables membres en
	 * utilisant des accolades (depuis c++11).
	 */
	int **Mval { };
	/**
	 * Le rang maximum de toutes les taches.
	 * Attention:
	 * le rang du sommet "a" --> 0
	 * le rang des etats initiaux --> 1
	 * le rang du sommet "z" --> max_rang + 1
	 * le rang des etas finals --> pas forcement max_rang !
	 * (mais il y a au moins un etat final qui possede le rang
	 * valant max_rang).
	 *
	 * Initialisation des variables membres en utilisant
	 * des accolades (depuis c++11).
	 */
	int max_rang { -1 };
	/**
	 *  Mini table de hashage contenant les taches de rangs differents
	 *  Ne prend pas en compte les sommets "a" et "z".
	 */
	vector<std::vector<Tache*> > vv;

	/**
	 *  Les trois variables membres suivantes seront utilisees par la fonction
	 *  top_sort() pour detecter des circuits.
	 */

	/* Map temporaire pour detercter des circuits dans le graphe courant. */
	map<char, Tache*> taches_map_t;
	/* Vecteur temporaire pour detecter des circuits dans le graphe courant. */
	vector<Tache> taches_t;
	/* Map contenant tous les Arcs du graphe courant. */
	map<pair<char, char>, Arc> map_arc;

	/* Afficher toutes les taches de l'ordonnancement. */
	void afficher_taches() const;

	/* Initaliser la variable membre map_arc. */
	void map_arc_init();
	/**
	 * Dans theorie des graphes, un tri topolotique d'un graphe oriente
	 * sans circuit est un ordre lineaire dans lequel chaque sommet
	 * se trouve avant tous ses successeurs (si le somment possede
	 * des successeurs).
	 *
	 * Chaque graphe oriente sans circuit possede un ou plusieurs tries
	 * topologiques.
	 *
	 * Si on n'arrive pas a faire un tri topologique, alors qu'il
	 * y a forcement au moins un circuit dans le graphe. C'est comme cela qu'on verifie
	 * s'il existe des circuits dans un graphe oriente.
	 */
	bool top_sort();

	void initialiser_matrice();
	void remplir_matrice();
	void afficher_matrice(int) const;
	void liberer_matrice();

	/* Calculer le rang d'une tache donnee. */
	int rang(Tache *t);
	/* Configurer les rangs pour toutes les taches. */
	void set_rang();
	/**
	 * Calculer la valeur maximum des rangs des taches,
	 * et allouer cette valeur a la variable membre max_rang.
	 */
	void set_max_rang();
	/**
	 * Mettre des taches dans une table de hashage selon les rangs
	 * des taches, configurer la variable membre vv.
	 */
	void set_hash_table();

	/* Calculer la date au plus tot pour toutes les taches. */
	void calculer_date_tot();
	/* Calculer la date au plus tard pour toutes les taches. */
	void calculer_date_tard();

	/* Afficher le diagramme de gantt pour l'ordonnancement. */
	void afficher_gantt(int);
} Graphe_or;

/**
 * @brief Configurer les variables membres map_arc, taches_t et
 *        taches_map_t.
 */
void Graphe_or::map_arc_init() {
	/* Configurer map_arc. */
	for (auto & p : taches)
		if (!p.succ.empty())
			for (auto & q : p.succ)
				map_arc[ { p.nom, q }] = Arc { p.nom, p.duree, q };

	/* Configurer taches_t. */
	for (auto & p : taches)
		taches_t.push_back(p);

	/* Configurer taches_map_t. */
	for (auto & p : taches_t)
		taches_map_t[p.nom] = &p;
}

/**
 * @brief faire un tri topologique de toutes les taches.
 * @return true s'il y a des circuits, sinon retourne false.
 */
bool Graphe_or::top_sort() {
	/* Vecteur qui contiendra les taches dans l'ordre topologique. */
	vector<Tache*> L;
	/**
	 * Vecteur contenant tous les etats initiaux (les taches n'ayant
	 * pas de predecesseurs).
	 */
	vector<Tache*> Q;
	for (auto & p : etats_initiaux)
		Q.push_back(taches_map_t[p]);

	while (!Q.empty()) {
		Tache *n = Q.back();
		Q.pop_back();
		L.push_back(n);
		for (auto & m : n->succ) {
			auto it = map_arc.find( { n->nom, m });
			map_arc.erase(it);

			string &str = taches_map_t[m]->pred;
			str.erase(remove(str.begin(), str.end(), n->nom), str.end());

			string &str2 = n->succ;
			str2.erase(remove(str.begin(), str.end(), m), str.end());

			if (taches_map_t[m]->pred.empty())
				Q.push_back(taches_map_t[m]);
		}
	}
	if (!map_arc.empty()) {
		cout << "Des circuits existent !" << endl;
		return true;
	} else {
		cout << "\nOrdre apres tri topologique : ";
		for (auto & p : L)
			cout << p->nom << " ";
		cout << endl;
	}
	return false;

}

/**
 * @brief Initialiser la matrice d'adjacence et la matrice des valeurs.
 *        Allouer la memoire dynamiquement pour ces deux matrices.
 *        Initialiser toutes les valeurs dans la matrice a -1.
 */
void Graphe_or::initialiser_matrice() {
	Madj = new int*[nb_sommets];
	Mval = new int*[nb_sommets];
	for (int i { 0 }; i < nb_sommets; i++) {
		Madj[i] = new int[nb_sommets];
		Mval[i] = new int[nb_sommets];
	}

	for (int i { 0 }; i < nb_sommets; i++)
		for (int j { 0 }; j < nb_sommets; j++) {
			Madj[i][j] = 0;
			Mval[i][j] = -1;
		}
}

/**
 * @brief Configurer la matrice d'adjacence et la matrice des valeurs.
 */
void Graphe_or::remplir_matrice() {
	/**
	 * Remplir la matrice (Etape 1).
	 * En utilisant tous les contraintes de toutes les taches.
	 */
	for (int i { 0 }; i < nb_taches; i++)
		if (!taches[i].succ.empty())
			for (auto & p : taches[i].succ) {
				Madj[taches[i].nom - 'A'][p - 'A'] = 1;
				Mval[taches[i].nom - 'A'][p - 'A'] = taches[i].duree;
			}
	/**
	 * Remplir la matrice (Etape 2).
	 * Mettre les relatoins entre "a" et les etats initiaux dans le tableau.
	 */
	for (auto & p : etats_initiaux) {
		Madj[nb_sommets - 2][p - 'A'] = 1;
		Mval[nb_sommets - 2][p - 'A'] = 0;
	}
	/**
	 * Remplir la matrice (Etape 3).
	 * Mettre les relations entre "z" et les etats finals dans le tableau.
	 */
	for (auto & p : etats_finals) {
		Madj[p - 'A'][nb_sommets - 1] = 1;
		Mval[p - 'A'][nb_sommets - 1] = taches_map[p]->duree;
	}
}

/**
 * @brief Afficher la matrice d'adjacence ou la matrice des valeurs.
 * @param choix si egal a 0, alors afficher la matrice d'adjacence.
 *              si egal a 1, alors afficher la matrice des valeurs.
 */
void Graphe_or::afficher_matrice(int choix) const {

	if (choix == 0)
		cout << "Afficher la matrice d'adjacence :\n\n" << endl;
	else if (choix == 1)
		cout << "Afficher la matrice des valeurs :\n\n" << endl;
	else {
		cout << "Choix incorrecte !" << endl;
		return;
	}

	for (auto & p : taches)
		cout << p.nom << " ";
	cout << "a Z\n";

	for (int i { 0 }; i < nb_sommets; i++) {
		if (i == nb_sommets - 2)
			cout << "a ";
		else if (i == nb_sommets - 1)
			cout << "z ";
		else
			cout << taches[i].nom << " ";

		if (choix == 0) {
			for (int j { 0 }; j < nb_sommets; j++)
				if (Madj[i][j] == 0)
					cout << "  ";
				else
					cout << Madj[i][j] << " ";
		} else if (choix == 1) {
			for (int j { 0 }; j < nb_sommets; j++)
				if (Mval[i][j] == -1)
					cout << "  ";
				else
					cout << Mval[i][j] << " ";
		} else {
			cout << "Choix incorrecte !" << endl;
			return;
		}
		cout << endl;
	}
}

/**
 * @brief Liberer les memoires anciennement allouees a la matrice d'adjacence.
 */
void Graphe_or::liberer_matrice() {
	for (int i { nb_sommets }; i--;) {
		delete[] Madj[i];
		delete[] Mval[i];
	}
	delete[] Madj;
	delete[] Mval;
}

/**
 * @brief Afficher toutes les taches de l'ordonnancement.
 */
void Graphe_or::afficher_taches() const {
	cout << "Nom\t" << "Duree\t" << "Pred\t" << "Succ\t" << "Rang\t"
			<< "Date tot\t" << "Date tard" << endl;
	for (auto & p : taches)
		cout << p << endl;
}

/**
 * @brief trouver la valeur maximum d'un vector<int>.
 */
int max(vector<int> v) {
	return *max_element(begin(v), end(v));
}

/**
 * @brief Calculer le rang pour une tache (fonction recursive).
 * @param t pointeur vers une tache quelconque.
 * @return rang de la tache courante.
 */
int Graphe_or::rang(Tache *t) {
	/* Le rang des etats initiaux est 1 */
	if (t->pred.empty())
		return 1;
	vector<int> tmp;
	for (auto & p : t->pred)
		tmp.push_back(rang(taches_map[p]));
	return max(tmp) + 1;
}

/**
 * @brief Calculer les rangs pour toutes les taches.
 */
void Graphe_or::set_rang() {
	for (auto & p : taches)
		p.rang = rang(&p);
}

/**
 * @brief Calculer la valeur maximum des rangs de toutes les taches,
 *        et alloue cette valeur a la variable membre max_rang.
 */
void Graphe_or::set_max_rang() {
	for (auto & p : taches)
		max_rang = (p.rang > max_rang) ? p.rang : max_rang;
}

/**
 * @brief Configurer la mini table de hashage vv contenant les pointeurs
 *        vers des taches de rangs differents.
 */
void Graphe_or::set_hash_table() {
	vv.resize(max_rang);
	for (auto & p : taches)
		vv[p.rang - 1].push_back(&p);
}

/**
 * @brief Calculer la date au plus tot pour toutes les taches.
 */
void Graphe_or::calculer_date_tot() {
	/* Allouer les date au plus tot aux etats initiaux. */
	for (auto & p : etats_initiaux)
		taches_map[p]->date_tot = 0;

	for (int i { 1 }; i < (int) vv.size(); i++)
		for (auto & p : vv[i]) {
			int max { -1 };
			for (auto & q : p->pred) {
				Tache *tmp = taches_map[q];
				if (tmp->date_tot + tmp->duree > max)
					max = tmp->date_tot + tmp->duree;
			}
			p->date_tot = max;
		}
}

/**
 * @brief Calculer la date au plus tard pour toutes les taches.
 */
void Graphe_or::calculer_date_tard() {
	/* Calculer la duree minimum du projet. */
	int max { -1 };
	for (auto & p : etats_finals) {
		Tache *tmp = taches_map[p];
		if (tmp->date_tot + tmp->duree > max)
			max = tmp->date_tot + tmp->duree;
	}

	for (auto & p : etats_finals) {
		Tache *tmp = taches_map[p];
		tmp->date_tard = max - tmp->duree;
	}

	for (int i { max_rang - 1 }; i--;)
		for (auto & p : vv[i]) {
			if (p->succ.empty())
				continue;
			int min { 9999999 };
			for (auto & q : p->succ) {
				Tache *tmp = taches_map[q];
				if (tmp->date_tard < min)
					min = tmp->date_tard;
			}
			p->date_tard = min - p->duree;
		}
}

/**
 * @brief Afficher le diagramme de gantt.
 *        Si choix == 0, alors afficher selon la date au plus tot.
 *        Si choix == 1, alors afficher selon la date au plus tard.
 *        Sinon, afficher une erreur.
 * @param choix entier a passer en parametre pour cette fonction.
 */
void Graphe_or::afficher_gantt(int choix) {
	int max { -1 };
	for (auto & p : etats_finals) {
		Tache *tmp = taches_map[p];
		if (tmp->date_tot + tmp->duree > max)
			max = tmp->date_tot + tmp->duree;
	}
	cout << "  ";
	for (int i { 0 }; i < max + 1; i++)
		cout << i << (i > 9 ? "  " : "   ");
	cout << "\n";
	for (auto & p : vv)
		for (auto & q : p) {
			int a { }, b { }, c { };
			if (choix == 0)
				a = q->date_tot;
			else if (choix == 1)
				a = q->date_tard;
			else {
				cout << "Choix incorrect!" << endl;
				return;
			}
			b = q->duree;
			c = max + 1 - a - b;
			cout << q->nom << " ";
			for (int i { a }; i--;)
				cout << "|   ";
			for (int i { b }; i--;)
				cout << "|===";
			for (int i { c }; i--;)
				cout << "|   ";
			cout << "\n";
		}
}

#endif /* GRAPHE_OR_H_ */
