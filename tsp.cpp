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
#include <cmath>
#include <climits>

using namespace std;

struct Location {
	int id;
	int x;
	int y;
};

int calcDistance(Location c1, Location c2);

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

		//duplicate map to create tree (need to remove elements)
		map<int, Location> temp_cities = cities;
		map<int, Location>::iterator temp_itr;

		//create min spanning tree using Primm's algorithm
		map<int, list<int> > MST;
		map<int, list<int> >::iterator mst_itr;
		
		list<int>::iterator list_itr;

		//MST "root node" will be stored in MST[-1], which should be unused city identifier
		temp_itr = temp_cities.begin();
		MST[-1].push_back( temp_itr->second.id );

		temp_cities.erase(temp_itr); //remove root node from temp

		//vars to keep track of shortest distance between MST and temp_cities
		int distance;
		int parentId, childId;

		//loop cities - 1 times
		for( int i=1; i < cities.size(); i++){
			
			distance = INT_MAX;

			//now get the next shortest distance between MST and temp_cities...
			//iterate through the map of lists
			for( mst_itr = MST.begin();  mst_itr != MST.end();  mst_itr++) {
	
				//iterate through each list
				for( list_itr = mst_itr->second.begin();  list_itr != mst_itr->second.end();  list_itr++) {
					
					city = cities[ *list_itr ];

					//check against each city in temp_cities
					for( temp_itr = temp_cities.begin(); temp_itr != temp_cities.end(); temp_itr++) {
					
						if ( calcDistance( city, cities[ temp_itr->second.id ] ) < distance ) {
							distance = calcDistance( city, cities[ temp_itr->second.id ] );
							parentId = *list_itr;
							childId = temp_itr->second.id;
						}
					}
				}
			}
			//add edge to MST (ie add child node to parent list)
			MST[parentId].push_back( childId );
			
			temp_cities.erase(childId);
		}


		//test MST size and temp size
		int mstsize = 0;
		for( mst_itr = MST.begin(); mst_itr != MST.end(); mst_itr++) {
			mstsize += mst_itr->second.size();
		}
		cout << "MST size: " << mstsize << endl;
		cout << "temp_cities: " << temp_cities.size() << endl;

	}
	return 0;
}


//returns the integer distance of two cities 
int calcDistance(Location c1, Location c2) {
	long dx, dy;

	dx = c1.x - c2.x;
	dx *= dx;
	dy = c1.y - c2.y;
	dy *= dy;

	return static_cast<int>( sqrt( dx + dy ) );
}
