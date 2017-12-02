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
	float x;
	float y;
};

// PROTOTYPES
int calcDistance(Location c1, Location c2);
map<int, list<int> >  getminTree( map<int, Location> cities);	
list<int> getOddVectors( map<int,list<int> > );
void printminTree( map<int, list<int> > minTree );
map<int, int> perfectMatching(list<int>, map<int, Location>);
list<int> euler_hamilton(map<int, list<int> > minTree);

list<int> pairwise(map<int, Location> cities, list<int> path);



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
		map<int, list<int> > minTree;
		minTree = getminTree( cities );

		//get list of city ID's that have odd edges
		list<int> oddVectors = getOddVectors(minTree);

		//get a map of city ID's to their perfect match city ID
		map<int, int> oddMatch = perfectMatching(oddVectors, cities);

		map<int, int>::iterator itr1;
		
		//minTree + perfect matching
		for(itr1 = oddMatch.begin(); itr1 != oddMatch.end(); itr1++) {
			minTree[itr1->first].push_back(itr1->second);
		}
	
	

		// list<int> fpath = euler_hamilton(minTree);		//Hamiltonian Cycle path
		list<int> newpath = euler_hamilton(minTree);		//Hamiltonian Cycle path
		list<int> fpath = pairwise(cities, newpath);


		int origin;
		int distance = 0;
		list<int>::iterator itr = fpath.begin();
		list<int>::iterator itr_end = fpath.end();
		--itr_end;



		while(itr != itr_end){
			origin = *itr;
			itr++;
			distance += calcDistance( cities[ origin ], cities[ *itr ] );
		}

		//connect the last city back to the first
		itr = fpath.begin();
		distance += calcDistance(cities[*itr], cities[*itr_end]);

		string filename = argv[1];
		filename += ".tour";
		
		ofstream oFile;

		oFile.open(filename);

		oFile << distance << endl;

		for(itr = fpath.begin(); itr != fpath.end(); itr++){
			oFile << *itr << endl;
		}

		cout << "Distance: " << distance << endl;

		oFile.close();

	}
	return 0;
}

/*
		while(itr != itr_end){
			origin = *itr;
			itr++;
			distance += calcDistance( cities[ origin ], cities[ *itr ] );
		}

		//connect the last city back to the first
		itr = fpath.begin();
		distance += calcDistance(cities[*itr], cities[*itr_end]);
*/

//----------------------------------------- FUNCTIONS ----------------------------------------------------
list<int> pairwise(map<int, Location> cities, list<int> path){
	list<int> newpath;
	int size = path.size();
	int improve = 0;

	while(improve < 2){

		int origin;
		int distance = 0;
		list<int>::iterator itr = path.begin();
		list<int>::iterator itr_end = path.end();
		--itr_end;

/*		string filename = argv[1];
		filename += ".tour";
		
		ofstream oFile;

		oFile.open(filename);

		oFile << distance << endl;

		for(itr = fpath.begin(); itr != fpath.end(); itr++){
			oFile << *itr << endl;
		}
*/

		while(itr != itr_end){
			origin = *itr;
			itr++;
			distance += calcDistance( cities[ origin ], cities[ *itr ] );
		}

		//connect the last city back to the first
		itr = path.begin();
		distance += calcDistance(cities[*itr], cities[*itr_end]);


		for(int i=0; i < size-1; i++){
			//cout << "loop" << i << endl;
			for(int k=i+1; k < size; k++){

				//Reorder path -- followed the wiki article psuedocode 
				list<int>::iterator l_itr, temp_itr;
			    l_itr=path.begin();
				// 1. take route[0] to route[i-1] and add them in order to new_route
			    for ( int c = 0; c <= i - 1; ++c )
			    {
			        newpath.push_back(*l_itr);
			        l_itr++;
			    }
			     
			    // 2. take route[i] to route[k] and add them in reverse order to new_route
			    list<int> temp;
			    for ( int c = i; c <= k; ++c )
			    {
			        temp.push_back(*l_itr);
			        l_itr++;
			    }
			    temp_itr=temp.end(); //get to the last element in temp list so we can add them in reverse order
			    temp_itr--;
			    for ( int c = i; c <= k; ++c )
			    {
			        newpath.push_back(*temp_itr);
			        temp_itr--;
			    }
			    temp.clear();

			    // 3. take route[k+1] to end and add them in order to new_route
			    for ( int c = k + 1; c < size; ++c )
			    {
			        newpath.push_back(*l_itr);
			        l_itr++;
			    }

			    //cout << "newpath" << endl;
			    //for(temp_itr=newpath.begin(); temp_itr != newpath.end(); temp_itr++){
			    	//cout << *temp_itr << endl;
			    //}

			    int origin2;
				int distance2 = 0;
				list<int>::iterator itr2 = newpath.begin();
				list<int>::iterator itr_end2 = newpath.end();
				--itr_end2;


				while(itr2 != itr_end2){
					origin2 = *itr2;
					itr2++;
					distance2 += calcDistance( cities[ origin2 ], cities[ *itr2 ] );
				}

				//connect the last city back to the first
				itr2 = newpath.begin();
				distance2 += calcDistance(cities[*itr2], cities[*itr_end2]);

				if (distance2 < distance) 
                {
  //              	cout << "made improvement" << endl;
                    improve = 0;
                    path = newpath;
                    distance = distance2;
                    newpath.clear(); //empty newpath container so we can repeat process
                }
                else{
    //            	cout << "did not improve" << endl;
                	newpath.clear();
                }
			}
		}

		improve ++;
	}

	return path;
}





//finds a hamiltonian cycle of the modified minTree
list<int> euler_hamilton(map<int, list<int> > minTree) {

	list<int> path;

	map<int, list<int> >::iterator m_itr;

	m_itr = minTree.begin();

	path.push_back(m_itr->first);
	
	list<int>::iterator p_itr;
	p_itr = path.begin();

	int neighbor;

	//first build euler tour 
	//randomly walk through minTree. if we hit first element and minTree is not empty, find an element in minTree and add it
	while ( !minTree.empty()) { //While the map is not empty

		neighbor = minTree[ *p_itr ].front();
		
		//remove edge from both minTree lists
		minTree[ *p_itr ].remove( neighbor );
		minTree[ neighbor ].remove( *p_itr );

		if( minTree[ *p_itr ].empty() ) {
			minTree.erase( *p_itr );
		}
		if( minTree[ neighbor ].empty() ) {
			minTree.erase( neighbor );
		}
		
		if( p_itr == path.end() ) {
			path.insert(p_itr, neighbor);
			p_itr--;
		}
		else {
			p_itr++;
			path.insert(p_itr, neighbor);
			p_itr--;
		}


		//if neighbor has nowhere to go, remove it from minTree
		if( minTree[ *p_itr ].empty() ) {
			minTree.erase( *p_itr );
			//if we completed a cycle but minTree !empty, then create new path loop at remaining minTree element
			if( !minTree.empty() ){
				//iterate down path until we find a node that still has edges
				for (p_itr = path.begin(); p_itr != path.end(); p_itr++){
					if ( minTree.find( *p_itr ) != minTree.end() ){
						break;
					}
				}
			}
		}
	}
	
	//build hamiltonian cycle by ignoring repeat nodes on euler tour

	//to keep track of used nodes
	unordered_set<int> traveled;
	map<int, int> duplicates;

	for(p_itr = path.begin(); p_itr != path.end(); p_itr++ ){
		duplicates[*p_itr]++;
	}

	map<int,int>::iterator d_itr;
	vector<int> dupCities;

	for(d_itr = duplicates.begin(); d_itr != duplicates.end(); d_itr++){
		if( d_itr->second > 1 ){
			dupCities.push_back( d_itr->first);
		}
	}

	int num = dupCities.back();
	dupCities.pop_back();
	do {
		for(p_itr = path.begin(); p_itr != path.end(); p_itr++ ){
			if (*p_itr == num) {
				path.erase(p_itr++);
				if (!dupCities.empty()) {
					num = dupCities.back();
					dupCities.pop_back();
				} else {
					break;
				}
			}
		}
	} while (!dupCities.empty());


/*	for(p_itr = path.begin(); p_itr != path.end(); p_itr++ ){
		if( traveled.find( *p_itr ) == traveled.end() ){
			traveled.insert(*p_itr);
		}
		else {
			p_itr = path.erase(p_itr);
		}
	}
*/
// cout << endl;
// for (list<int>::iterator x = path.begin(); x != path.end(); x++)
// 	cout << *x << " ";
	
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




/*		printminTree(map<int, list<int>>)
	prints the minTree
*/
void printminTree( map<int, list<int> > minTree) {
		
		map<int, list<int> >::iterator m_itr;
		list<int>::iterator l_itr;

		for( m_itr = minTree.begin(); m_itr != minTree.end(); m_itr++) {
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
	float dx, dy;

	dx = c1.x - c2.x;
	dx *= dx;
	dy = c1.y - c2.y;
	dy *= dy;

	return static_cast<int>( round(sqrt( dx + dy )) );
}



/*     			 getminTree( map<int, Location> )
	Returns a minTree of the connected graph using Prim's algorithm
	The minTree is an adjacency list of location ids that store the list of child nodes of each city.
*/
map<int, list<int> >  getminTree( map<int, Location> cities) {	

	int numCities = cities.size();

	map<int, list<int> > minTree;
	
	unordered_set<int> outsideCities;

	//keep track of vectors
	vector<int> closestCityOutside( numCities );
	vector<int> closestCityDist( numCities );

	int minAdjCity;
	int tempDistance;
	
	//initialize MST with city 0 and its nearest neighbor
	tempDistance = INT_MAX;
	for (int i=1; i< numCities; i++){
		if ( calcDistance( cities[0], cities[i]) < tempDistance ){
			tempDistance = calcDistance( cities[0], cities[i]);
			minAdjCity = i;
		}
	}
	minTree[0].push_back(minAdjCity);
	minTree[minAdjCity].push_back(0);


	//keep track of cities outside MST
	for(int i=1; i < numCities; i++){
		outsideCities.insert(i);
	}
	outsideCities.erase(minAdjCity);


	//next calculate closest city outside of tree to each city inside tree 
	for (map<int, list<int> >::iterator m_itr = minTree.begin(); m_itr != minTree.end(); m_itr++) {
		closestCityDist[ m_itr->first ] = -1;
		for ( int j=1; j < numCities; j++ ){
			if ( j != minAdjCity ){
				tempDistance = calcDistance( cities[ m_itr->first ], cities[j] );
				if (closestCityDist[ m_itr->first ] < 0 || tempDistance < closestCityDist[ m_itr->first ] ){
					closestCityDist[ m_itr->first ] = tempDistance;
					closestCityOutside[ m_itr->first ] = j;
				}
			}
		}	
	}	

	int insideCity, outsideCity;
cout << "n = " << numCities << endl;
	//BUILD MST for rest of the cities
	for(int n=0; n<numCities - 2; n++) {
cout << n << " ";
		//find shortest distance between tree and outside tree
		tempDistance = INT_MAX;
		for (map<int, list<int> >::iterator m_itr = minTree.begin(); m_itr != minTree.end(); m_itr++) {
			if ( closestCityDist[ m_itr->first ] < tempDistance ) {
				insideCity = m_itr->first;
				tempDistance = closestCityDist[insideCity];
				outsideCity = closestCityOutside[insideCity];
			}
		}


		//add outsideCity to MST
		minTree[ insideCity ].push_back( outsideCity ); 
		minTree[ outsideCity ].push_back( insideCity );


		outsideCities.erase(outsideCity);

		
		//find the closest outside city to newly added city
		closestCityDist[outsideCity] = INT_MAX;
		for( unordered_set<int>::iterator o_itr = outsideCities.begin(); o_itr != outsideCities.end(); o_itr++){
			tempDistance = calcDistance( cities[outsideCity], cities[ *o_itr ] );
			if ( tempDistance < closestCityDist[outsideCity] ) {
				closestCityOutside[outsideCity] = *o_itr;
				closestCityDist[outsideCity] = tempDistance;
			}
		}
		
		//update any other cities that were closest to outside city
		for (map<int, list<int> >::iterator m_itr = minTree.begin(); m_itr != minTree.end(); m_itr++) {

			if ( closestCityOutside[ m_itr->first ] == outsideCity ) {
			
				//then we need to find a new city
				closestCityDist[ m_itr->first ] = INT_MAX;
				
				for( unordered_set<int>::iterator o_itr = outsideCities.begin(); o_itr != outsideCities.end(); o_itr++) {
					
					tempDistance = calcDistance( cities[ m_itr->first ], cities[*o_itr]);
					
					if ( tempDistance < closestCityDist[ m_itr->first ] ) {
						closestCityOutside[ m_itr->first ] = *o_itr;
						closestCityDist[ m_itr->first ] = tempDistance;
					}
				}
			}
		}
	}
	// cout << "MST complete!" << endl;
	// cout << "MST size = " << minTree.size() << endl;
	return minTree;
}




/*			getOddVectors( map< int, list<int> > )
	returns a list of integers representing the vectors with odd number of edges
*/
list<int> getOddVectors( map<int,list<int> > minTree ){

	list<int> oddList;

	map<int,list<int> >::iterator itr;

	for( itr = minTree.begin(); itr != minTree.end(); itr++) {
		if ( ( itr->second.size() % 2) == 1 ) {
			oddList.push_back(itr->first);
		}
	}

	return oddList;
}
