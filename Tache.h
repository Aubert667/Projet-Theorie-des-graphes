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

#ifndef TACHE_H_
#define TACHE_H_

#include <iostream>

using namespace std;

typedef struct Tache {
	char nom;
	int duree;
	int date_tot { -1 };
	int date_tard { -1 };
	int rang { -1 };
	/**
	 * Tous les predecessuers (contraintes) de la tache
	 * sous forme d'une chaine de caracteres.
	 */
	std::string pred;
	/**
	 * Tous les successeurs de la tache sous forme de chaine
	 * de caracteres.
	 */
	std::string succ;
	Tache(char, int, std::string);
} Tache;

Tache::Tache(char n, int d, string p) :
		nom { n }, duree { d }, pred { p } {
}

ostream& operator<<(ostream& strm, Tache const & obj) {
	return strm << obj.nom << "\t" << obj.duree << "\t"
			<< (obj.pred.size() ? obj.pred : "[NULL]") << "\t"
			<< (obj.succ.size() ? obj.succ : "[NULL]") << "\t" << obj.rang
			<< "\t" << obj.date_tot << "\t\t" << obj.date_tard << "";
}

#endif /* TACHE_H_ */
