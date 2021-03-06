/*
 * string_find.h
 *
 *  Created on: 11/05/2017
 *      Author: Xavier Fontes
 */

#ifndef SRC_STRING_FIND_H_
#define SRC_STRING_FIND_H_

#include<vector>
#include"utils.h"
using namespace std;

/** @file */


/**
 * @brief Loads the station names from their text files into the program.
 * @param filename the file with the station names
 * @return a vector with pairs corresponding a name to a boolean value (if it already exists or not)
 */
vector<par> load_names(string filename) {
	vector<par> res;

	ifstream infile(filename.c_str());
	if (!infile.is_open())
		throw(exception());
	string estacao;
	while (getline(infile, estacao)) {
		par par_teste = par(estacao, false);
		res.push_back(par_teste);
	}
	return res;
}

/**
 * @brief Chooses a random station.
 * @param vec a vector with pairs of names and boolean values indicating if they already exist
 * @return a random unnused station
 */
string choose_random(vector<par> & vec) {
	while (true) {
		unsigned int i = rand() % vec.size();
		if (!vec[i].usado) {
			vec[i].usado = true;
			return vec[i].nome_estacao;
		}
	}
}

/**
 * @brief Computes the prefix for the KMP algorithm.
 * @param toSearch the pattern that will be searched
 * @return a vector with the KMP prefix for the pattern.
 * @see kmpStringMatchGivenPi
 */
vector<unsigned int> computePrefix(string toSearch) {
	unsigned int m = toSearch.size();
	vector<unsigned int> pi(m + 1, 0);

	unsigned int k = 0;

	for (size_t q = 2; q <= m; q++) {
		while (k > 0 && toSearch.at(k) != toSearch.at(q - 1)) {
			k = pi.at(k);
		}
		if (toSearch.at(k) == toSearch.at(q - 1)) {
			k++;
		}

		pi.at(q) = k;
	}

	return pi;
}

/**
 * @brief The main KMP algorithm.
 * @param input the text being searched
 * @param toSearch the pattern being matched
 * @param pi the KMP algorithm prefix for toSearch
 * @see computePrefix
 * @return the number of occurrences of toSearch in input
 */
unsigned int kmpStringMatchGivenPi(string input, string toSearch,
		const vector<unsigned int>& pi) {
	if(toSearch.length() > input.length())
		return 0;
	unsigned int occ = 0;
	unsigned int n = input.size();
	unsigned int m = toSearch.size();
	unsigned int q = 0;
	for (size_t i = 1; i <= n; i++) {
		while (q > 0 && toSearch.at(q) != input.at(i - 1)) {
			q = pi.at(q);
		}
		if (toSearch.at(q) == input.at(i - 1)) {
			q++;
		}
		if (q == m) {
			occ++;
			q = pi.at(q);
		}
	}
	return occ;
}

/**
 * @brief Calculates the edit distance between a pattern and the text.
 * @param pattern the pattern being matched
 * @param texto the text being searched
 * @return the edit distance between pattern and texto
 */
int distancia_entre_palavras(string pattern, string texto) {
	vector<vector<int>> D(pattern.size() + 1, vector<int>(texto.size() + 1, 0));

	for (unsigned int i = 0; i <= pattern.size(); i++)
		D[i][0] = i;
	for (unsigned int j = 0; j <= texto.size(); j++)
		D[0][j] = j;

	for (unsigned int i = 1; i <= pattern.size(); i++) {
		for (unsigned int j = 1; j <= texto.size(); j++) {
			if (pattern[i - 1] == texto[j - 1]) {
				D[i][j] = D[i - 1][j - 1];
			} else {

				D[i][j] = 1
						+ min(min(D[i - 1][j - 1], D[i - 1][j]), D[i][j - 1]);
			}
		}
	}

	return D[pattern.size()][texto.size()];

}

/**
 * @brief Searches for occurrences of a pattern with a naive algorithm.
 * @param pattern the pattern being matched
 * @param text the text being searched
 * @return the occurrences of pattern in text
 */
int naiveStringMatch(string text, string pattern) {
	int sum = 0;
	if(pattern.length() > text.length())
		return sum;
	for (unsigned int i = 0; i <= (text.size() - pattern.size()); i++) {
		unsigned int inicioTexto = i;
		unsigned int inicioPattern = 0;
		while (true) {
			if (pattern[inicioPattern] == text[inicioTexto]) {
				if (inicioPattern == (pattern.size() - 1)) {
					sum++;
					break;
				} else {
					inicioPattern++;
					inicioTexto++;
				}
			} else {
				break;
			}

		}
	}
	return sum;
}

/**
 * @brief Prints the n-th closest matches for the pattern.
 * @param linhas_geradas a vector with all the bus/subway lines generated
 * @param distancias the edit distances between the pattern and each stop
 * @param numero_lista the number of matches being shown
 */
void printMoreProximate(const vector<vector<NoInfo>> &linhas_geradas,
		const vector<vector<int>> &distancias,unsigned  int numero_lista) {
	vector<string> lista;
	vector<int> diferencas;
	int min = 10000;
	int max = 0;
	for(unsigned int i = 0; i < distancias.size(); i++){
		for(unsigned int j = 0; j < distancias[i].size(); j++){
			if(distancias[i][j] < min)
				min = distancias[i][j];
			if(distancias[i][j] > max)
				max = distancias[i][j];
		}
	}

	while(lista.size() < numero_lista || min > max){
		for(unsigned int i = 0; i < distancias.size(); i++){
				for(unsigned int j = 0; j < distancias[i].size(); j++){
					if(distancias[i][j] == min){
						lista.push_back(linhas_geradas[i][j].nome_paragem);
						diferencas.push_back(min);
						if(lista.size() >= numero_lista)
							break;
					}
				}
				if(lista.size() >= numero_lista)
					break;
			}
		if(lista.size() >= numero_lista)
			break;
		min++;
	}

	for(unsigned int ind = 0; ind < lista.size(); ind++){
		cout << ind+1 << ": " << lista[ind] << " -> " << diferencas[ind] << endl;
	}

	return;
}
#endif /* SRC_STRING_FIND_H_ */
