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
#include <cstring>
#include <limits>

using namespace std;


struct Location {
	int id;
	int x;
	int y;
};

// PROTOTYPES
int calcDistance(Location c1, Location c2);
map<int, list<int> >  getMST( map<int, Location> cities);	
list<int> getOddVectors( map<int,list<int> > );
void printMST( map<int, list<int> > MST );
map<int, int> perfectMatching(list<int>, map<int, Location>);
list<int> euler_hamilton(map<int, list<int> > MST);
bool isNotBridge(map<int, list<int> > &MST, int listtemp);


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

		//get min spanning tree
		map<int, list<int> > MST;
		MST = getMST( cities );

		//get list of city ID's that have odd edges
		list<int> oddVectors = getOddVectors(MST);

		//get a map of city ID's to their perfect match city ID
		map<int, int> oddMatch = perfectMatching(oddVectors, cities);

		map<int, int>::iterator itr1;
		
		///////////////for(itr1 = oddMatch.begin(); itr1 != oddMatch.end(); ++itr1) {
			///////////////cout << itr1->first << ": " << itr1->second << endl;
		//////////////}


		//MST + perfect matching
		for(itr1 = oddMatch.begin(); itr1 != oddMatch.end(); itr1++) {
			MST[itr1->first].push_back(itr1->second);
		}
		printMST(MST); 

		//Final path
		list<int> fpath = euler_hamilton(MST); 
		//list<int>::iterator fpit;
		//for(fpit=fpath.begin(); fpit != fpath.end(); fpit++){
		//	cout << *fpit << endl;
		//}

		// printMST(MST);

/*
		//manually add matching pairs to MST for test.txt
		if( strcmp(argv[1], "test.txt") == 0 ){
			//add edge between vectors 6 and 3
			MST[6].push_back(3);
			MST[3].push_back(6);
			//add edge between vectors 2 and 7
			MST[2].push_back(7);
			MST[7].push_back(2);

			cout << endl;
			printMST(MST);
		} */

	}
	return 0;
}



//----------------------------------------- FUNCTIONS ----------------------------------------------------
bool isNotBridge(map<int, list<int> > MST, int listtemp, int maptemp){
	map<int, list<int> > tempMST;
	tempMST = MST;

	map<int, list<int> >::iterator m_itr;
	list<int>::iterator l_itr;

	//Delete the edge that we are considering taking
	tempMST.find(maptemp)->second.remove(listtemp);
	tempMST.find(listtemp)->second.remove(maptemp);
	//If deleting the edge leaves empty list, depete that key in the map
	if(tempMST.find(maptemp)->second.empty())	
		tempMST.erase(maptemp);	


	m_itr = tempMST.begin();

	while(1){
		if((m_itr->second.empty()) && (tempMST.size() == 1))
			return 1;

		else if((m_itr->second.empty()) && (tempMST.size() > 1))
			return 0;
		else{
			l_itr = m_itr->second.begin();

			int ltemp = *l_itr;
			int mtemp = m_itr->first;

			m_itr->second.remove(ltemp);			//Remove edge from city 1 in map
			tempMST.find(ltemp)->second.remove(mtemp);	//Remove edge from city two in map

			if(m_itr->second.empty())	//If the city doesn't share any more edges with any other city
			tempMST.erase(m_itr);		//Delete the city from the map

			m_itr = tempMST.find(ltemp);
		}
	}
}

list<int> euler_hamilton(map<int, list<int> > MST) {

	list<int> path;

	map<int, list<int> >::iterator m_itr;
	list<int>::iterator pathtemp;
	list<int>::iterator l_itr;

	m_itr = MST.begin();
	path.push_back(m_itr->first);

	while (!MST.empty()) { //While the map is not empty																				
		for( l_itr = m_itr->second.begin();  l_itr != m_itr->second.end();  l_itr++){ 
			int ltemp = *l_itr;
			int mtemp = m_itr->first;
			if(isNotBridge(MST, ltemp, mtemp) == 1){			//If this edge is not a bridge
				path.push_back(ltemp);					//Add the edge to the path
				m_itr->second.remove(ltemp);			//Remove edge from city 1 in map
				MST.find(ltemp)->second.remove(mtemp);	//Remove edge from city two in map

				if(m_itr->second.empty())	//If the city doesn't share any more edges with any other city
					MST.erase(m_itr);		//Delete the city from the map

				m_itr = MST.find(ltemp);


				pathtemp=path.begin();
				for(pathtemp; pathtemp != path.end();  pathtemp++){
					cout << *pathtemp << endl;
				}
				break;
			}
			
			else
				l_itr++;

		}
	}

	return path;
}







/*		perfectMatching(list<int>, map<int, Location>)
	Creates a map of perfect matching pairs where each vertex is matched with its closest neighbor.
	Only one edge per vertex.
*/
map<int, int> perfectMatching(list<int> oddVectors, map<int, Location> cities) {
	
	// struct for tracking which city has the shortest distance from the starting position
	struct DL {
		int distance;
		int location;
		list<int>::iterator cityItr;
	};

	map<int, int> oddMatch; //cityId, cityID of matching pairs
	DL dl;
	int currDistance;
	list<int>::iterator cityA_Itr, cityB_Itr;

	while (oddVectors.size() != 0) {
		dl.distance = numeric_limits<int>::max();
		dl.location = numeric_limits<int>::max();
		cityA_Itr = oddVectors.begin();
		cityB_Itr = oddVectors.begin();
		cityB_Itr++;

		for (; cityB_Itr != oddVectors.end(); ++cityB_Itr) {
			currDistance = calcDistance(cities[*cityA_Itr], cities[*cityB_Itr]);

			// cout << *cityA_Itr << " to " << *cityB_Itr << ": " << currDistance << endl;

			if (currDistance < dl.distance) {
				dl.distance = currDistance;
				dl.location = *cityB_Itr;
				dl.cityItr = cityB_Itr;
			}
		}

		oddMatch[*cityA_Itr] = dl.location;
		oddMatch[dl.location] = *cityA_Itr;

		oddVectors.erase(cityA_Itr);
		oddVectors.erase(dl.cityItr);
	}

	return oddMatch;
}



/*		printMST(map<int, list<int>>)
	prints the MST
*/
void printMST( map<int, list<int> > MST) {
		
		map<int, list<int> >::iterator m_itr;
		list<int>::iterator l_itr;

		for( m_itr = MST.begin(); m_itr != MST.end(); m_itr++) {
			cout << m_itr->first << ": ";
			l_itr = m_itr->second.begin();
			while( l_itr !=  m_itr->second.end() ){
				cout << *l_itr << " ";
				l_itr++;
			}
			cout << endl;
		}
}




/*			calcDistance(Location c1, Location c2)
	Returns the integer distance of two cities 
*/
int calcDistance(Location c1, Location c2) {
	long dx, dy;

	dx = c1.x - c2.x;
	dx *= dx;
	dy = c1.y - c2.y;
	dy *= dy;

	return static_cast<int>( sqrt( dx + dy ) );
}



/*     			 getMST( map<int, Location> )
	Returns a MST of the connected graph using Prim's algorithm
	The MST is an adjacency list of location ids that store the list of child nodes of each city.
*/
map<int, list<int> >  getMST( map<int, Location> cities) {	
		
		map<int, list<int> > MST;
		
		//temp variable
		Location city;

		//duplicate map to create tree (need to remove elements)
		map<int, Location> temp_cities = cities;
		map<int, Location>::iterator temp_itr;

		map<int, list<int> >::iterator mst_itr;
		
		list<int>::iterator list_itr;

		//vars to keep track of shortest distance between MST and temp_cities
		int distance;
		int edge;
		int id1, id2;

	//add first relationship to MST
		//get first city and remove from temp cities
		temp_itr = temp_cities.begin();
		city = temp_itr->second;
		id1 = city.id;
		temp_cities.erase( id1 );

		distance = INT_MAX;
		
		//find city closest to city1 to initialize MST
		for( temp_itr = temp_cities.begin(); temp_itr != temp_cities.end(); temp_itr++){				
			edge = calcDistance( city, cities[ temp_itr->second.id ] );
			if ( edge < distance ) {
				distance = edge;
				id2 = temp_itr->second.id;
			}
		}	
		MST[id1].push_back(id2);
		MST[id2].push_back(id1);

		temp_cities.erase(id2);

		//add rest of cities
		for( int i=0; i < cities.size() - 2; i++){
			
			distance = INT_MAX;

			//to get the next shortest distance between MST and temp_cities we iterate through the MST using mst_tr and list_itr
			for( mst_itr = MST.begin();  mst_itr != MST.end();  mst_itr++) {	
				for( list_itr = mst_itr->second.begin();  list_itr != mst_itr->second.end();  list_itr++) {
					
					city = cities[ *list_itr ];

					//check against each city in temp_cities
					for( temp_itr = temp_cities.begin(); temp_itr != temp_cities.end(); temp_itr++) {
	
						edge = calcDistance( city, cities[ temp_itr->second.id ] );
						if ( edge < distance ) {
							distance = edge;
							id1 = *list_itr;
							id2 = temp_itr->second.id;
						}
					}
				}
			}
			MST[id1].push_back(id2);
			MST[id2].push_back(id1);
			
			temp_cities.erase(id2);
		}

		//test MST size and temp size
		int mstsize = 0;
		for( mst_itr = MST.begin(); mst_itr != MST.end(); mst_itr++) {
			mstsize += mst_itr->second.size();
		}
		
//		cout << "MST vectors: " << MST.size() << "  MST edges: " << mstsize << endl;
//		cout << "temp_cities: " << temp_cities.size() << endl;

		return MST;
}




/*			getOddVectors( map< int, list<int> > )
	returns a list of integers representing the vectors with odd number of edges
*/
list<int> getOddVectors( map<int,list<int> > MST ){

	list<int> oddList;

	map<int,list<int> >::iterator itr;

	for( itr = MST.begin(); itr != MST.end(); itr++) {
		if ( ( itr->second.size() % 2) == 1 ) {
			oddList.push_back(itr->first);
		}
	}

	return oddList;
}