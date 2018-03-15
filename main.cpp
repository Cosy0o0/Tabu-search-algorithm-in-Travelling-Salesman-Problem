#include <iostream>
#include <queue>
#include <fstream>
#include <vector>
#include <ctime>

int N = 0; // liczba miast
using namespace std;

struct usuwane
{
	int x;
	int y;
};

struct matrix // struktura zawierająca macierz kosztów w postaci vectora oraz rozmiar macierzy
{
	vector <vector<int>> tab;
	int size = N;

};

class Stopwatch { // klasa umożliwiająca liczenie czasu algorytmu
private:
	clock_t tstart;
	clock_t tstop;
public:
	double time;
	Stopwatch::Stopwatch() {
		//ctor
	}
	void Stopwatch::start() {
		tstart = clock();
		time = 0;
	}
	void Stopwatch::stop() {
		tstop = clock();
		time = ((double)(tstop - tstart)) / CLOCKS_PER_SEC;
	}
	double Stopwatch::read() {
		return time;
	}
};

matrix generowanie(matrix start) // funkcja generująca losową macierz
{
	srand(time(NULL));
	for (int i = 0; i < N; i++)
	{
		vector <int> column;
		start.tab.push_back(column);
		for (int j = 0; j < N; j++)
		{
			int bufor;
			bufor = (rand() % 50) + 1;
			if (i == j) start.tab[i].push_back(INT_MAX);
			else start.tab[i].push_back(bufor);
		}
	}
	return start;
}

matrix wczytaj(matrix start) // funkcja wczytująca macierz z pliku symetrycznego
{
	ifstream plik;
	plik.open("gr48.tsp");
	for (int i = 0; i < N; i++)
	{
		vector <int> column;
		start.tab.push_back(column);
		for (int j = 0; j < N; j++)
		{
			start.tab[i].push_back(INT_MAX);
		}
	}
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j <= i; j++)
		{
			int bufor;
			plik >> bufor;
			start.tab[i][j] = bufor;
			start.tab[j][i] = start.tab[i][j];
			if (i == j)
			{
				start.tab[i][j] = INT_MAX;
			}
		}
	}
	plik.close();
	return start;
}

matrix wczytaj2(matrix start) // funkcja wczytująca macierz z pliku asymetrycznego
{
	ifstream plik;
	plik.open("60tabu.txt");
	for (int i = 0; i < N; i++)
	{
		vector <int> column;
		start.tab.push_back(column);
		for (int j = 0; j < N; j++)
		{
			int bufor;
			plik >> bufor;
			if (i == j) start.tab[i].push_back(INT_MAX);
			else start.tab[i].push_back(bufor);
		}
	}
	plik.close();
	return start;
}

int wyswietl(matrix start) // funkcja wyświetlająca macierz
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (start.tab[i][j] == INT_MAX) cout << "NN ";
			else cout << start.tab[i][j] << " ";
		}
		cout << endl;
	}
	return 0;
}

int tourCost(int * tour,matrix actual) // funkcja licząca koszt podróży
{
	int bestTourCost = 0;
	for (int i = 0; i < N - 1; i++)
	{
		bestTourCost += actual.tab[tour[i]][tour[i + 1]];
	}
	bestTourCost += actual.tab[tour[N - 1]][tour[0]];
	return bestTourCost;
}

int * beginRandom(int * startingTour) //funkcja podająca losową trasę przez wszystkie miasta
{
	int * startingRand = new int[N];
	for (int i = 0; i < N; i++)
	{
		startingRand[i] = -1;
	}
	int counter = 0, los = 0;
	while (counter < N)
	{
		do
		{
			los = rand() % N;
		} while (startingRand[los] != -1);
		startingRand[los] = -2;
		startingTour[counter] = los;
		counter++;
	}
	//delete[] startingRand;
	return startingTour;
}

int * swap(int i,int j,int * actualTour) // funkcja zamieniająca dwa miasta miejscami
{
	int bufor = actualTour[i];
	actualTour[i] = actualTour[j];
	actualTour[j] = bufor;
	return actualTour;
}

int tabu(matrix actual) // główna funkcja algorytmu
{
	Stopwatch clock;
	clock.start(); // rozpoczęcie liczenia czasu
	int tabuSize = 0.4 * N; // podanie rozmiaru listy tabu
	int * tabuList = new int[tabuSize];
	for (int i = 0; i < tabuSize; i++)
	{
		tabuList[i] = -1;
	}
	int * bestTour = new int[N];
	bestTour = beginRandom(bestTour); // najlepsza trasa jako początkowo wylosowana
	int bestTourCost = tourCost(bestTour, actual); //najlepszy koszt przejścia
	int * actualTour = new int[N];
	int repeatCounter = 0;
	int lastBestTourCost = 0;
	int bestTourCostInTheWorld = INT_MAX; // najlepszy koszt w całym algorytmie
	for (int k = 0; k < 1200; k++) // główna pętla, arbitralna ilość iteracji, podana na podstawie prób
	{								// i zależna od ilości miast
		if (repeatCounter == 50) // jeśli przez 50 przejść pętli głównej najlepsza wartość się nie zmieni,
		{						// wylosuj od nowa drogę
			bestTour = beginRandom(bestTour);
			bestTourCost = tourCost(bestTour, actual);
			for (int i = 0; i < tabuSize; i++)
			{
				tabuList[i] = -1;
			}
		}
		actualTour = bestTour;
		int bestSwapValue = INT_MAX;
		int bestI = 0;
		int bestJ = 0;
		int actualTourCost = 0;
		for (int i = 0; i < N - 1; i++)
		{
			for (int j = i + 1; j < N; j++) //zamiana miast każdy z każdym i znalezienie najlepszej zamiany
			{
				actualTour = swap(i, j, actualTour);
				actualTourCost = tourCost(actualTour, actual);
				int actualBestSwap = actualTourCost - bestTourCost;
				if (actualBestSwap < bestSwapValue) //jeśli zamiana jest lepsza od najlepszej dotychczasowej
				{
					int p = 0;
					if (actualTourCost + actualBestSwap < bestTourCost)
					{								// sprawdzenie czy zamiana spełnia kryterium aspiracji
						bestSwapValue = actualBestSwap;
						bestI = i;
						bestJ = j;
					}
					else					// jeśli nie, sprawdzenie czy zamiana znajduje się na liście tabu
					{
						for (; p < tabuSize; p++)
						{
							if (actualTour[j] == tabuList[p]) break;
						}
						if (p == tabuSize)
						{
							bestSwapValue = actualBestSwap;
							bestI = i;
							bestJ = j;
						}
					}
				}
				actualTour = swap(i, j, actualTour); // powrót do drogi przed zamianą
			}
		}
		//actualTourCost = tourCost(actualTour, actual);
		//actualTourCost += bestSwapValue;
		actualTour = swap(bestI, bestJ, actualTour); // zamiana drogi na najlepszą z przeszukanych
		actualTourCost = tourCost(actualTour, actual);
		tabuList[k % tabuSize] = actualTour[bestJ]; // aktualizacja listy tabu
		if (actualTourCost < bestTourCost) // zamiana drogi na lepszą
		{
			bestTourCost = actualTourCost;
			bestTour = swap(bestI, bestJ, bestTour);
		}
		if (lastBestTourCost == bestTourCost) repeatCounter++; // liczenie powtórzeń bez polepszenia wyniku
		else repeatCounter = 0;
		lastBestTourCost = bestTourCost; // aktualizacja najlepszego wyniku w algorytmie v
		if (bestTourCost < bestTourCostInTheWorld) bestTourCostInTheWorld = bestTourCost;
		//cout << bestTourCost << endl; ****************************
	}
	clock.stop(); // zatrzymanie liczenia czasu
	cout <<"Czas obliczen: "<< clock.read() << endl;
	cout <<"Najkrotsza droga: "<< bestTourCostInTheWorld << endl;
	//delete[] actualTour;
	//delete[] bestTour;
	return 0;
}

int main()
{
	srand(time(NULL));
	matrix start;


	for (char option = '0'; option != '5';)
	{
		cout << "Program obliczania problemu komiwojazera przy użyciu tabu search" << endl;
		cout << "1: Wygeneruj losowa tablice" << endl;
		cout << "2: Wczytaj z pliku" << endl;
		cout << "3: Wyswietl" << endl;
		cout << "4: Uruchom algorytm" << endl;
		cout << "5: Opusc program" << endl;
		cin >> option;

		switch (option)
		{
		case '1':
			cout << "Podaj liczbe wierzcholkow: ";
			cin >> N;
			start = generowanie(start);
			break;
		case '2':
			cout << "Podaj liczbe wierzcholkow: ";
			cin >> N;
			start = wczytaj(start);
			break;
		case '3':
			wyswietl(start);
			break;
		case '4':
			tabu(start);
			break;
		case '5':
			break;
		default:
			cout << "Nie ma takiej opcji!" << endl;
			break;
		}
	}

	return 0;
}
