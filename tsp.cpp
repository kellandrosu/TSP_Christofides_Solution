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
#include <unordered_set>

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

		iFile.close();

		//get min spanning tree
		map<int, list<int> > MST;
		MST = getMST( cities );

		//get list of city ID's that have odd edges
		list<int> oddVectors = getOddVectors(MST);

		//get a map of city ID's to their perfect match city ID
		map<int, int> oddMatch = perfectMatching(oddVectors, cities);

		map<int, int>::iterator itr1;
		
		//MST + perfect matching
		for(itr1 = oddMatch.begin(); itr1 != oddMatch.end(); itr1++) {
			MST[itr1->first].push_back(itr1->second);
		}
	
	
		list<int> fpath = euler_hamilton(MST); 	//Final path

		int origin;
		int distance = 0;
		list<int>::iterator itr = fpath.begin();

		while(itr != fpath.end()){
			origin = *itr;
			itr++;
			distance += calcDistance( cities[ origin ], cities[ *itr ] );	
		}

		string filename = argv[1];
		filename += ".tour";
		
		ofstream oFile;

		oFile.open(filename);

		oFile << distance << endl;

		for(itr = fpath.begin(); itr != fpath.end(); itr++){
			oFile << *itr << endl;
		}

		oFile.close();
		
	}
	return 0;
}



//----------------------------------------- FUNCTIONS ----------------------------------------------------




//finds a hamiltonian cycle of the modified MST
list<int> euler_hamilton(map<int, list<int> > MST) {

	list<int> path;

	map<int, list<int> >::iterator m_itr;
	list<int>::iterator p_itr;

	m_itr = MST.begin();

	path.push_back(m_itr->first);
	p_itr = path.begin();

	int neighbor;
	
	//first build euler tour 
	//randomly walk through MST. if we hit first element and MST is not empty, find an element in MST and add it
	while ( !MST.empty()) { //While the map is not empty

		neighbor = MST[ *p_itr ].front();
		
		//remove edge from both MST lists
		MST[ *p_itr ].remove( neighbor );
		MST[ neighbor ].remove( *p_itr );

		if( MST[ *p_itr ].empty() ) 
			MST.erase( *p_itr );
		
		p_itr++;
		path.insert(p_itr, neighbor);
		p_itr--;
		
		if( MST[ neighbor ].empty() ) {
			MST.erase( neighbor );
		}

		//if neighbor has nowhere to go, remove it from MST
		if( MST[ *p_itr ].empty() ) {
			MST.erase( *p_itr );
			//if we completed a cycle but MST !empty, then create new path loop at remaining MST element
			if( !MST.empty() ){
				//iterate down path until we find a node that still has edges
				p_itr = path.begin();
				while( MST.find( *p_itr ) == MST.end() ){
					p_itr++;
				}
			}
		}
	}
	
	//build hamiltonian cycle by ignoring repeat nodes on euler tour

	//to keep track of used nodes
	unordered_set<int> traveled;

	for(p_itr = path.begin(); p_itr != path.end(); p_itr++ ){
		if( traveled.find( *p_itr ) == traveled.end() ){
			traveled.insert(*p_itr);
		}
		else {
			p_itr = path.erase(p_itr);
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
		for( int i=0; i < (int)(cities.size()) - 2; i++){
			
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
