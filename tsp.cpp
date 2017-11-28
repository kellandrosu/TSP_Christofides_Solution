/**
Travelling Salesman Project
Authors: Andrius Kelly, Sandhya Raman, Kevin Woods
**/

#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <map>

using namespace std;

struct Location {
	int id;
	int x;
	int y;
};

int main(int argc, char* argv[]) {
    
	if(argc != 2) {
		cout << "Need file name\n" ;
	}
	
	else {
		
		ifstream iFile;	
		iFile.open(argv[1]);

		//use a map because city id might not be sorted order
		map<int, Location> cities;
		Location city;

		//iterates through input file, add to city dict
		while ( iFile >> city.id) {	
			iFile >> city.x;
			iFile >> city.y;
			cities[city.id] = city;
		}

		map< int, Location>::iterator itr;

		for( itr = cities.begin(); itr != cities.end(); itr++){
			city = itr->second;
			cout << city.id << " " << city.x << " " << city.y << endl; 
		}

	}
	return 0;
}
