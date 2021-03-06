#include <cstdio>
#include <cmath>
#include <string>
#include <map>
#include <cstring>
#include <unordered_set>
#include <utility>
#include <time.h>
#include "utils.h"
#include "aresta.h"
#include "map_tests.h"
#include "string_find.h"
#include "file_reading.h"

#define PARAGENS_APROX 10
#define REPETE_ALGORITS 100
#define REPETE_PARTIAL 200
/** @file */

template<>
struct vertex_greater_than<long double> {
	bool operator()(Vertex<NoInfo> * a, Vertex<NoInfo> * b) const {
		return a->getDist() > b->getDist();
	}
};

template<>  //for A*
struct vertex_greater_than_A<long double> {
	bool operator()(Vertex<NoInfo> * a, Vertex<NoInfo> * b) const {
		return ((a->getDist() + a->getDestinyDistance())
				> (b->getDist() + b->getDestinyDistance()));
	}
};

template<>
struct heuristicFunc<NoInfo> {
	NoInfo destino;
	bool operator()(Vertex<NoInfo> * a, Vertex<NoInfo> * b) {
		long double distOri = haversine_km(a->getInfo().latitude,
				a->getInfo().longitude, destino.latitude, destino.longitude)
				+ a->getDist();
		long double distDest = haversine_km(b->getInfo().latitude,
				b->getInfo().longitude, destino.latitude, destino.longitude)
				+ b->getDist();
		return (distOri > distDest ? true : false);
	}
	//no longer needed
	long double operator()(Vertex<NoInfo> * a) const {
		return haversine_km(a->getInfo().latitude, a->getInfo().longitude,
				destino.latitude, destino.longitude);
	}
};

/**
 * @brief Opens the data files and populates a graph.
 * @param A the "xxx_a.txt" file
 * @param B the "xxx_b.txt" file
 * @param C the "xxx_c.txt" file
 * @param grafo the graph being populated
 * @param gv the GraphViewer instance being used
 */
void abrirFicheiros(string A, string B, string C, Graph<NoInfo> & grafo,
		GraphViewer * gv) {

	ifstream inFile;
	//Ler o ficheiro A2.txt
	inFile.open(A);

	if (!inFile) {
		cerr << "Unable to open file datafile.txt";
		exit(1);   // call system to stop
	}

	std::string line;

	BigAssInteger idNo = 0;
	long double X = 0;
	long double Y = 0;

	while (std::getline(inFile, line)) {
		std::stringstream linestream(line);
		std::string data;

		linestream >> idNo;

		std::getline(linestream, data, ';'); // read up-to the first ; (discard ;).
		linestream >> X;
		std::getline(linestream, data, ';'); // read up-to the first ; (discard ;).
		linestream >> Y;    //X and Y are in degrees

		std::getline(linestream, data, ';'); // read up-to the first ; (discard ;).
		linestream >> X;
		std::getline(linestream, data, ';'); // read up-to the first ; (discard ;).
		linestream >> Y;    //X and Y are in radians
		//cout << "idNo: " << idNo << " long: " << X << " lat: " << Y << endl;
		NoInfo temp(idNo % 100000000, X, Y);
		gv->addNode(idNo % 100000000);
		grafo.addVertex(temp);

	}

	inFile.close();

	//abrir C2.txt sao as arestas
	inFile.open(C);

	if (!inFile) {
		cerr << "Unable to open file datafile.txt";
		exit(1);   // call system to stop
	}

	BigAssInteger idAresta;
	BigAssInteger idNo1;
	BigAssInteger idNo2;

	BigAssInteger i = 0;

	while (std::getline(inFile, line)) {
		std::stringstream linestream(line);
		std::string data;

		linestream >> idAresta;

		std::getline(linestream, data, ';'); // read up-to the first ; (discard ;).
		linestream >> idNo1;
		std::getline(linestream, data, ';'); // read up-to the first ; (discard ;).
		linestream >> idNo2;    //X and Y are in degrees

		NoInfo origem(idNo1 % 100000000, 0, 0);  //so para efeitos de pesquisa
		Vertex<NoInfo>* source = grafo.getVertex(origem);
		NoInfo destino(idNo2 % 100000000, 0, 0);
		Vertex<NoInfo>* destiny = grafo.getVertex(destino);

		//pre processamento do grafico pelo parser ja garante informacao sem erros //i think

		if (grafo.removeEdge(origem, destino)) //conseguiu remover
				{
			grafo.addEdge(origem, destino,
					haversine_km(source->getInfo().latitude,
							source->getInfo().longitude,
							destiny->getInfo().latitude,
							destiny->getInfo().longitude), i);
		}
		grafo.addEdge(origem, destino,
				haversine_km(source->getInfo().latitude,
						source->getInfo().longitude,
						destiny->getInfo().latitude,
						destiny->getInfo().longitude), i);
		gv->addEdge(i, idNo1 % 100000000, idNo2 % 100000000,
				EdgeType::DIRECTED);
		gv->setVertexColor(idNo1 % 100000000, GREEN);
		i++;

	}
	gv->rearrange();
	inFile.close();
}

/**
 * @brief This function tests the graph route colouring.
 * @param data the graph being used
 * @param gv the GraphViewer instance being used
 * @param numCaminhos the number of routes being calculated
 * @param sizeCaminhos the size of the routes being calculated
 * @param source the start node for the routes.
 */
void teste_colorir(Graph<NoInfo>& data, GraphViewer*& gv, int numCaminhos,
		unsigned int sizeCaminhos, Vertex<NoInfo>* source,
		Vertex<NoInfo>* destiny, string cor) {

	if (source == NULL || destiny == NULL) {
		int i = 0;
		while (i < numCaminhos) {
			int ind0 = rand() % data.getVertexSet().size();
			int ind1 = rand() % data.getVertexSet().size();
			Vertex<NoInfo> * ori = data.getVertex(
					NoInfo(data.getVertexSet()[ind0]->getInfo().idNo, 0, 0));
			Vertex<NoInfo> * des = data.getVertex(
					NoInfo(data.getVertexSet()[ind1]->getInfo().idNo, 0, 0));
			if (ori == NULL || des == NULL || ori == des)
				continue;

			vector<NoInfo> path = data.getDijkstraPath(ori->getInfo(),
					des->getInfo());

			if (path.size() < sizeCaminhos)
				continue;

			string color = "BLACK";
			switch (i) {
			case 0:
				color = YELLOW;
				break;
			case 1:
				color = ORANGE;
				break;
			case 2:
				color = RED;
				break;
			case 3:
				color = PINK;
				break;
			case 4:
				color = GRAY;
				break;
			case 5:
				color = BLACK;
				break;
			}

			cout << "novo caminho: " << i << endl;
			for (unsigned int i = 0; i < path.size(); i++) {
				Sleep(100);
				cout << path[i] << endl;

				gv->setVertexColor(path[i].idNo, color);
				gv->setVertexSize(path[i].idNo, 40);
				if (i < (path.size() - 1)) {
					cout << data.getVertex(path[i])->getIdEdge(path[i + 1])
							<< endl;
					gv->setEdgeDashed(
							data.getVertex(path[i])->getIdEdge(path[i + 1]),
							false);
					gv->setEdgeColor(
							data.getVertex(path[i])->getIdEdge(path[i + 1]),
							color);
					gv->setEdgeThickness(
							data.getVertex(path[i])->getIdEdge(path[i + 1]), 5);
				}
				gv->rearrange();
			}

			i++;
		}
	}

	else {
		vector<NoInfo> path = data.getDijkstraPath(source->getInfo(),
				destiny->getInfo());

		for (unsigned int i = 0; i < path.size(); i++) {
			Sleep(100);
			cout << path[i] << endl;

			gv->setVertexColor(path[i].idNo, cor);
			gv->setVertexSize(path[i].idNo, 40);
			if (i < (path.size() - 1)) {
				cout << data.getVertex(path[i])->getIdEdge(path[i + 1]) << endl;
				gv->setEdgeDashed(
						data.getVertex(path[i])->getIdEdge(path[i + 1]), false);
				gv->setEdgeColor(
						data.getVertex(path[i])->getIdEdge(path[i + 1]), cor);
				gv->setEdgeThickness(
						data.getVertex(path[i])->getIdEdge(path[i + 1]), 5);
			}
			gv->rearrange();
		}

	}
}

/**
 * The main function.
 *
 * It receives up to 14 arguments:
 *  - argv[1]  Start node (node number), assess connectivity ("Connectivity"), complexity ("comp") or auto ("auto")
 *             or exact string search ("StringSearchExact") or approximate string search ("StringSearchAprox");
 *  - argv[2]  End node (argv[1] = node number, end node number) or number of paths (argv[1] = "auto", 1-5)
 *	- argv[3]  Algorithm to use (if there's a start node or auto, "A*", "Dijkstra" or "Floyd-Warshall")
 *	- argv[4]  Number of subway lines (if there's a start node or auto, 1-10)
 *	- argv[5]  Length of subway lines (if there's a start node or auto, {10, 20, 30, 40, 50})
 *	- argv[6]  Number of bus lines (if there's a start node or auto, 1-20)
 *	- argv[7]  Length of bus lines (if there's a start node or auto, {10, 20, 30, 40, 50})
 *	- argv[8]  Filename radical (e.g. xxx.png, xxxA.txt, xxxB.txt, xxxC.txt)
 *	- argv[9]  Map image width
 *	- argv[10] Map image height
 *	- argv[11] Lower longitude value
 *	- argv[12] Lower latitude value
 *	- argv[13] Highest longitude value
 *	- argv[14] Highest latitude value
 */
int main(int argc, char * argv[]) {
	srand(time(NULL));
	vector<vector<NoInfo>> linhas_geradas;

	if (argc != 15) {
		cout << "There was an input error, please contact the developers."
				<< endl;
		return 1;
	}

	string ficheiro = argv[8];
	Graph<NoInfo> data;
	int xMaxW = atoi(argv[9]);
	int yMaxW = atoi(argv[10]);

	GraphViewer * gv = new GraphViewer(xMaxW, yMaxW, false); //not dynamic
	gv->setBackground(ficheiro + ".png");
	gv->createWindow(xMaxW, yMaxW);
	gv->defineEdgeCurved(false);
	gv->defineEdgeDashed(true);
	gv->defineVertexColor(GREEN);
	gv->defineVertexSize(4);
	gv->defineEdgeColor(BLACK);
	struct cantos corners;
	corners.minLong = atof(argv[11]);
	corners.minLat = atof(argv[12]);
	corners.maxLong = atof(argv[13]);
	corners.maxLat = atof(argv[14]);

	abrirFicheiroXY(ficheiro + "_a.txt", ficheiro + "_b.txt",
			ficheiro + "_c.txt", data, gv, corners, xMaxW, yMaxW);

	int linhas_metro = atoi(argv[4]);
	int linhas_autocarro = atoi(argv[6]);
	int comp_metro = atoi(argv[5]);
	int comp_autocarro = atoi(argv[7]);

	int number_of_paths = -1;
	if (strcmp(argv[1], "auto") == 0) {
		linhas_geradas = gera_linhas(data, linhas_metro, linhas_autocarro,
				comp_metro, comp_autocarro);
		number_of_paths = atoi(argv[2]);
		testFloidWarshal_big(data, gv, number_of_paths);
		cout << "END" << endl;
		getchar();
		return 0;

	} else if (strcmp(argv[1], "comp") == 0) {
		testExecutionTimes(data, gv);
		cout << "END" << endl;
		getchar();
		return 0;
	} else if (strcmp(argv[1], "Connectivity") == 0) {
		vector<int> soma;
		vector<Vertex<NoInfo>*> vertices = data.getVertexSet();
		for (unsigned int i = 0; i < vertices.size(); i++) {
			vector<NoInfo> tentativa = data.bfs(vertices[i]);
			soma.push_back(vertices.size() - tentativa.size());
		}
		int max_falha = 0;
		int min_falha = vertices.size();
		for (int ind : soma) {
			if (ind < min_falha)
				min_falha = ind;
			if (ind > max_falha)
				max_falha = ind;
		}
		cout << "Em " << vertices.size() << " nos, o minimo de falhas foi "
				<< min_falha << endl;
		cout << "O maximo foi " << max_falha << endl;
		cout << "END" << endl;
		getchar();
		return 0;

	} //string search
	else if ((strcmp(argv[1], "SearchExact") == 0)
			|| (strcmp(argv[1], "SearchApprox") == 0)) {
		vector<par> dados_metro;
		vector<par> dados_autocarro;
		try {
			dados_metro = load_names("_metros.txt");
			dados_autocarro = load_names("_autocarros.txt");
		} catch (exception & e) {
			cout << "There was an error opening the files..." << endl;
			return 1;
		}
		linhas_geradas = gera_linhas_nomes(data, linhas_metro, linhas_autocarro,
				comp_metro, comp_autocarro, dados_metro, dados_autocarro);

		for (unsigned int i = 0; i < linhas_geradas.size(); i++)
			for (unsigned int j = 0; j < linhas_geradas[i].size(); j++) {
				//cout << linhas_geradas[i][j].nome_paragem << endl;
				gv->setVertexLabel(linhas_geradas[i][j].idNo,
						linhas_geradas[i][j].nome_paragem);
			}

		for (unsigned int i = 0; i < linhas_geradas.size(); i++) {
			Sleep(1000);
			printPathColored(data, gv, linhas_geradas[i], linhas_geradas);
		}

		string pesquisa;
		cout << "DIGITE A SUA PESQUISA:" << endl;
		while (getline(cin, pesquisa)) {
			//for(int i = 0; i < 34;i++) cout << endl;
			//cout << pesquisa << endl;
			if (pesquisa == "END")
				break;

			vector<NoInfo> occorrencias_palavra;
			vector<string> info_adicional;

			//cout << "kmp:" << endl;
			auto start_kmp = std::chrono::high_resolution_clock::now();
			vector<unsigned int> pi = computePrefix(pesquisa); //todo check this out, plus, increment no. words
			for (int numero = 0; numero < REPETE_ALGORITS; numero++) {
				occorrencias_palavra.clear();
				info_adicional.clear();

				for (unsigned int i = 0; i < linhas_geradas.size(); i++) {
					for (unsigned int j = 0; j < linhas_geradas[i].size();
							j++) {
						if (kmpStringMatchGivenPi(
								linhas_geradas[i][j].nome_paragem, pesquisa, pi)
								> 0) {
							occorrencias_palavra.push_back(
									linhas_geradas[i][j]);
							if (j == 0)
								info_adicional.push_back("INI");
							else if (j == (linhas_geradas[i].size() - 1))
								info_adicional.push_back("FIM");
							else
								info_adicional.push_back("NORM");
						}

					}
				}
			}
			auto end_kmp = std::chrono::high_resolution_clock::now();
			//cout << "end kmp:" << endl;
			//cout << "naive:" << endl;
			auto start_naive = std::chrono::high_resolution_clock::now();
			for (int numero = 0; numero < REPETE_ALGORITS; numero++) {
				occorrencias_palavra.clear();
				info_adicional.clear();
				for (unsigned int i = 0; i < linhas_geradas.size(); i++) {
					for (unsigned int j = 0; j < linhas_geradas[i].size();
							j++) {
						if (naiveStringMatch(linhas_geradas[i][j].nome_paragem,
								pesquisa) > 0) {
							occorrencias_palavra.push_back(
									linhas_geradas[i][j]);
							if (j == 0)
								info_adicional.push_back("INI");
							else if (j == (linhas_geradas[i].size() - 1))
								info_adicional.push_back("FIM");
							else
								info_adicional.push_back("NORM");
						}

					}
				}
			}
			auto end_naive = std::chrono::high_resolution_clock::now();
			//cout << "end naive:" << endl;

			if (occorrencias_palavra.size() == 0) {
				cout << "Paragem Desconhecida!" << endl;
			} else {
				cout << "Encontrei:" << endl;
				for (unsigned int i = 0; i < occorrencias_palavra.size(); i++)
					cout << i + 1 << "-> "
							<< occorrencias_palavra[i].nome_paragem << endl;
			}

			for (unsigned int i = 0; i < occorrencias_palavra.size(); i++) {
				//cout << "changed" << endl;
				for (unsigned int index = 0; index < 15; index++) {
					gv->setVertexColor(occorrencias_palavra[i].idNo, "GRAY");
					gv->setVertexSize(occorrencias_palavra[i].idNo, 30);
					gv->rearrange();
					Sleep(100);
					gv->setVertexColor(occorrencias_palavra[i].idNo, "WHITE");
					gv->setVertexSize(occorrencias_palavra[i].idNo, 40);
					gv->rearrange();
					Sleep(100);
				}
				if (info_adicional[i] == "NORM") {
					gv->setVertexColor(occorrencias_palavra[i].idNo,
					COR_NO_NORMAL);
					gv->setVertexSize(occorrencias_palavra[i].idNo, 25);
				} else if (info_adicional[i] == "INI") {
					gv->setVertexColor(occorrencias_palavra[i].idNo,
					COR_NO_INICIO);
					gv->setVertexSize(occorrencias_palavra[i].idNo, 30);
				} else {
					gv->setVertexColor(occorrencias_palavra[i].idNo,
					COR_NO_FIM);
					gv->setVertexSize(occorrencias_palavra[i].idNo, 30);
				}
				gv->rearrange();

			}

			auto start_partial = std::chrono::high_resolution_clock::now();
			auto end_partial = std::chrono::high_resolution_clock::now();

			if (occorrencias_palavra.size() == 0
					|| (strcmp(argv[1], "SearchApprox") == 0)) {
				cout << "INICIAR PESQUISA APROXIMADA" << endl;
				start_partial = std::chrono::high_resolution_clock::now();
				//pesquisa aprox
				vector<vector<int>> distancias;
				for (unsigned int linha = 0; linha < linhas_geradas.size();
						linha++) {
					vector<int> distancia_linha_atual;
					for (unsigned int no = 0; no < linhas_geradas[linha].size();
							no++) {
						distancia_linha_atual.push_back(
								distancia_entre_palavras(
										linhas_geradas[linha][no].nome_paragem,
										pesquisa));
					}
					distancias.push_back(distancia_linha_atual);
				}
				end_partial = std::chrono::high_resolution_clock::now();

				cout << PARAGENS_APROX << " com maior proximidade em relacao a "
						<< pesquisa << ":" << endl;
				printMoreProximate(linhas_geradas, distancias, PARAGENS_APROX);

			}

			cout << "TEMPO QUE DEMOROU A PROCURAR" << endl;

			//complexidade palavra
			cout << "MILISECONDS:" << endl;
			cout << "1-KMP repetido " << REPETE_ALGORITS << " vezes demorou:"
					<< (double) std::chrono::duration_cast<
							std::chrono::milliseconds>(end_kmp - start_kmp).count()
					<< "ms." << endl;

			cout << "2-Naive repetido " << REPETE_ALGORITS << " vezes demorou:"
					<< (double) std::chrono::duration_cast<
							std::chrono::milliseconds>(end_naive - start_naive).count()
					<< "ms." << endl;

			cout << "3-Partial matching repetido " << REPETE_PARTIAL << " vezes:"
					<< (double) std::chrono::duration_cast<
							std::chrono::milliseconds>(
							end_partial - start_partial).count() << "ms."
					<< endl;
			cout << "MICROSECONDS:" << endl;
			cout << "1-KMP repetido " << REPETE_ALGORITS << " vezes demorou:"
					<< (double) std::chrono::duration_cast<
							std::chrono::microseconds>(end_kmp - start_kmp).count()
					<< "us." << endl;

			cout << "2-Naive repetido " << REPETE_ALGORITS << " vezes demorou:"
					<< (double) std::chrono::duration_cast<
							std::chrono::microseconds>(end_naive - start_naive).count()
					<< "us." << endl;

			cout << "3-Partial matching repetido " << REPETE_PARTIAL << " vezes:"
					<< (double) std::chrono::duration_cast<
							std::chrono::microseconds>(
							end_partial - start_partial).count() << "us."
					<< endl;
			cout << "NANOSECONDS:" << endl;
			cout << "1-KMP repetido " << REPETE_ALGORITS << " vezes demorou:"
					<< (double) std::chrono::duration_cast<
							std::chrono::nanoseconds>(end_kmp - start_kmp).count()
					<< "ns." << endl;

			cout << "2-Naive repetido " << REPETE_ALGORITS << " vezes demorou:"
					<< (double) std::chrono::duration_cast<
							std::chrono::nanoseconds>(end_naive - start_naive).count()
					<< "ns." << endl;

			cout << "3-Partial matching repetido " << REPETE_PARTIAL << " vezes:"
					<< (double) std::chrono::duration_cast<
							std::chrono::nanoseconds>(
							end_partial - start_partial).count() << "ns."
					<< endl;

			cout << endl << "DIGITE A SUA PESQUISA:" << endl;
		}

		cout << "END" << endl;
		getchar();
		return 0;

	} else { //no inicio para o fim
		linhas_geradas = gera_linhas(data, linhas_metro, linhas_autocarro,
				comp_metro, comp_autocarro);
		Vertex<NoInfo> * origem = data.getVertex(
				NoInfo(atoi(argv[1]), 0, 0, ' '));
		Vertex<NoInfo> * destino = data.getVertex(
				NoInfo(atoi(argv[2]), 0, 0, ' '));
		if (origem == NULL || destino == NULL) {
			cout << "There is no such path" << endl;
			return 2;
		}
		vector<NoInfo> caminho;
		if (strcmp(argv[3], "A*") == 0) {
			preparaA_star(data, destino->getInfo());
			caminho = data.getA_starPath(origem->getInfo(), destino->getInfo());

		} else if (strcmp(argv[3], "Dijkstra") == 0) {
			caminho = data.getDijkstraPath(origem->getInfo(),
					destino->getInfo());

		} else if (strcmp(argv[3], "Floyd-Warshall") == 0) {
			caminho = data.getfloydWarshallPath(origem->getInfo(),
					destino->getInfo());
		} else {
			cout << "There is no such Algorithm." << endl;
			return 3;
		}

		printPathColored(data, gv, caminho, linhas_geradas);
		cout << "END" << endl;
		getchar();
		return 0;

	}
	cout << "END" << endl;
	getchar();
	return 0;
}
