//
//  LoadParams.cpp
//  
//
//  Created by Mikaela Smit on 29/10/2015.
//
//

#include "LoadParams.h"
#include <stdio.h>	 									// Include some essential libraries and files
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "person.h"
#include "event.h"
#include "eventfunctions.h"
#include "errorcoutmacro.h"
#include "eventQ.h"

#include <fstream>										// some important libraries for reading in the arrays
#include <string>
#include <sstream>
#include <stdlib.h>
#include <cmath>

using namespace std;


//// --- Pointers to external arrays --- ////
double** BirthArray;									// This  is a pointer to an array!! i.e pointer to pointer :)
double** DeathArray_Women;
double** DeathArray_Men;
double** HIVArray_Women;
double** HIVArray_Men;


//// --- Load key arrays --- ////
void loadBirthArray(){									// Let's read in the fertility array
    E(cout << "Lets load the fertility array. " << endl;)
    
    ifstream myfile("/Users/Mikaela/Documents/HIVModel/HIVModel/fertility.csv");
    
    BirthArray = new double *[301];
    for (int row = 0; row<301; row++){				// This loop will read in every number to the right place
        string line;
        getline(myfile, line);
        stringstream iss(line);
        
        BirthArray[row]=new double[121];
        for (int col = 0; col<121; col++){
            string val;
            getline (iss, val, ',');
            stringstream convertor(val);
            convertor >>  BirthArray[row][col];
        }
    }
    
    E(cout << "Fertility array has been read in successfully! " << endl;)
    cout << "BirthArray: " << BirthArray[0][0] << " and " << BirthArray[121][121] << endl;
}

void loadDeathArray_Women(){							// Let's read in the death array for women
    E(cout << "Lets load the death array for women. " << endl;)
    
    ifstream myfile1("/Users/Mikaela/Documents/HIVModel/HIVModel/mortality_array_women.csv");
    
    DeathArray_Women = new double *[301];
    for (int row = 0; row<301; row++){				// This loop will read in every number to the right place
        string line;
        getline(myfile1, line);
        stringstream iss(line);
        
        DeathArray_Women[row]=new double[121];
        for (int col = 0; col<121; col++){
            string val;
            getline (iss, val, ',');
            stringstream convertor(val);
            convertor >>  DeathArray_Women[row][col];
        }
    }
    E(cout << "Mortality array for women has been read in successfully! " << endl;)
}

void loadDeathArray_Men(){								// Let's read in the death array for men
    E(cout << "Lets load the death array for men. " << endl;)
    
    ifstream myfile2("/Users/Mikaela/Documents/HIVModel/HIVModel/mortality_array_men.csv");
    
    DeathArray_Men = new double *[301];
    for (int row = 0; row<301; row++){				// This loop will read in every number to the right place
        string line;
        getline(myfile2, line);
        stringstream iss(line);
        
        DeathArray_Men[row]=new double[121];
        for (int col = 0; col<121; col++){
            string val;
            getline (iss, val, ',');
            stringstream convertor(val);
            convertor >>  DeathArray_Men[row][col];
        }
    }
    E(cout << "Mortality array for men has been read in successfully! " << endl;)
}

void loadHIVArray_Women(){
    E(cout << "Lets load the HIV array for women.  " << endl;)
    
    ifstream myfile3("/Users/Mikaela/Documents/HIVModel/HIVModel/HIV_incidence_women.csv");			// Let's read in the HIV array for women (incidence)
    
    HIVArray_Women = new double *[201];
    for (int row = 0; row<201; row++){					// This loop will read in every number to the right place
        string line;
        getline(myfile3, line);
        stringstream iss(line);
        
        HIVArray_Women[row]=new double[121];
        for (int col = 0; col<121; col++){
            string val;
            getline (iss, val, ',');
            stringstream convertor(val);
            convertor >>  HIVArray_Women[row][col];
        }
    }
    E(cout << "HIV array for women has been read in successfully! " << endl;)
}

void loadHIVArray_Men(){
    E(cout << "Lets load the HIV array for men.  " << endl;)
    
    ifstream myfile4("/Users/Mikaela/Documents/HIVModel/HIVModel/HIV_incidence_men.csv");				// Let's read in the HIV array for men (incidence)
    
    HIVArray_Men = new double *[201];
    for (int row = 0; row<201; row++){					// This loop will read in every number to the right place
        string line;
        getline(myfile4, line);
        stringstream iss(line);
        
        
        HIVArray_Men[row]=new double[121];
        for (int col = 0; col<121; col++){
            string val;
            getline (iss, val, ',');
            stringstream convertor(val);
            convertor >>  HIVArray_Men[row][col];
        }
    }
    E(cout << "HIV array for men has been read in successfully! " << endl;)
}
