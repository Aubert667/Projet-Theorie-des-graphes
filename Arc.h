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

#ifndef ARC_H_
#define ARC_H_

using namespace std;

typedef struct Arc {
	char debut { };
	int longueur { };
	char fin { };
	Arc();
	Arc(char, int, char);
} Arc;

Arc::Arc() {
}

Arc::Arc(char d, int l, char f) :
		debut(d), longueur(l), fin(f) {
}

ostream& operator<<(ostream& strm, Arc const& obj) {
	return strm << obj.debut << " --[" << obj.longueur << "]--> " << obj.fin;
}

/**
 * Definir une structure pour determiner si un arc est plus grad
 * qu'un autre arc. Cette structure serve du troisieme argument
 * pour le map des arc (veuillez voir dans "Graph_or.h").
 */
struct ArcComparer {
	bool operator()(const Arc& first, const Arc& second) const {
		if (first.debut == second.debut)
			return first.fin < second.fin;
		return first.debut < second.debut;
	}
};

#endif /* ARC_H_ */
