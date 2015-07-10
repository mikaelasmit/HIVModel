/////////////////////////////////////////////////////////////////
//    Created by Mikaela Smit on 07/11/2014.				   //
//    Copyright (c) 2014 Mikaela Smit. All rights reserved.    //
//    These event are added to the EventQ.					   //
/////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <math.h>                                                          // For 'log'
#include "eventfunctions.h"
#include "event.h"															// Need to add these to be able to	
#include "eventQ.h"															// [...]run Global Time and Recurrent events pointers
#include "person.h"
#include "errorcoutmacro.h"


using namespace std;


//// --- OUTSIDE INFORMATION --- ////
extern double *p_GT;														// Tell this .cpp that there is pointer to Global Time defined externally
extern int EndYear;															// Include endyear so that we can avoid putting unnecessary items into EventQ
extern priority_queue<event*, vector<event*>, timeComparison> *p_PQ;		// Tell this .cpp that there is a priorty_queue externall and define pointer to it
extern int total_population;												// Update total population for output and for next new entry
extern person** MyArrayOfPointersToPeople;									// Pointer to MyArrayOfPointersToPeople
extern int *p_PY;															// Pointer to show which year range we are on
extern vector<event*> Events;


//// --- Key parameters for HIV natural history, treatment and mortality --- ////
// The following arrays are for [Sex][Age], where Sex: 0-Men & 1=Women and Age: 0=0-24, 1=25-34, 2=35-44, and 3= over 45
float CD4_startarray[2][4][3]={												// Array for proportion with specific CD4 count ranges upon seroconversion (Men and women)
	{{0.57905, 0.83195, 1},		{0.53669, 0.79935, 1},		{0.49260, 0.76275, 1},		{0.42467, 0.70094, 1}},
	{{0.62750, 0.84933, 1},		{0.58635, 0.81863, 1},		{0.54277, 0.78372, 1},		{0.47411, 0.72378, 1}}
};
		
float ARTDeath_event[2][4][7]={												// Array to see if this event will be either ART start or death will orcurr in this CD4 cat - see Excel for calculation
	{{0.048, 0.066, 0.088, 0.100, 0.145, 0.089, 1},		{0.052, 0.072, 0.092, 0.105, 0.152, 0.098, 1},		{0.056, 0.079, 0.097, 0.111, 0.160, 0.108, 1},		{0.065, 0.095, 0.109, 0.124, 0.177, 0.130, 1}},
	{{0.052, 0.072, 0.095, 0.108, 0.156, 0.096, 1},		{0.056, 0.079, 0.100, 0.113, 0.164, 0.106, 1},		{0.061, 0.086, 0.106, 0.120, 0.172, 0.118, 1},		{0.072, 0.104, 0.119, 0.134, 0.191, 0.142, 1}}
};
	
float ART_start[2][4][7]={													// If number is smaller than that in array then patient starts ART if its bigger they will die
	{{0.678, 0.608, 0.773, 0.773, 0.778, 0.556, 0.042},		{0.627, 0.553, 0.730, 0.731, 0.736, 0.499, 0.034},		{0.576, 0.501, 0.687, 0.687, 0.693, 0.447, 0.027},		{0.487, 0.421, 0.605, 0.606, 0.612, 0.361, 0.019}},								
	{{0.670, 0.599, 0.766, 0.766, 0.771, 0.546, 0.040},		{0.618, 0.543, 0.723, 0.723, 0.729, 0.490, 0.032},		{0.567, 0.491, 0.679, 0.679, 0.685, 0.437, 0.026},		{0.478, 0.403, 0.596, 0.596, 0.603, 0.352, 0.019}}
};

float CD4_rate_change[2][4][7]={											// Time to progression to next CD4 stage
	{{4.96, 2.96, 2.91, 1.82, 1.68, 1.11, 1.18},	{4.94, 2.94, 2.89, 1.90, 1.67, 1.09, 0.94},		{4.92, 2.91, 2.88, 1.89, 1.65, 1.08, 0.76},		{4.87, 2.86, 2.84, 1.86, 1.61, 1.05, 0.53}},
	{{5.35, 3.18, 3.14, 2.06, 1.81, 1.19, 1.14},	{5.33, 3.16, 3.12, 2.05, 1.79, 1.17, 0.91},		{5.30, 3.13, 3.10, 2.03, 1.77, 1.16, 0.74},		{5.24, 3.07, 3.05, 2.00, 1.72, 1.13, 0.51}}
};

float CD4_rates[2][6]={
    {0.19835, 0.32913, 0.33628, 0.50936, 0.57271, 0.86702},
    {0.18356, 0.30458, 0.31120, 0.47137, 0.52999, 0.80236},
};

float Death_CD4_rates[2][4][7]={
    {   {0.00321, 0.00912, 0.00733, 0.01281, 0.02161, 0.03741, 0.84544},
        {0.00403, 0.01145, 0.00921, 0.01608, 0.02713, 0.04696, 1.06123},
        {0.00497, 0.01412, 0.01136, 0.01984, 0.03347, 0.05794, 1.30933},
        {0.00712, 0.02022, 0.01626, 0.02841, 0.04792, 0.08296, 1.87488},
    },
    
    {   {0.00334, 0.00948, 0.00762, 0.01331, 0.02245, 0.03887, 0.87847},
        {0.00419, 0.01189, 0.00957, 0.01671, 0.02819, 0.04879, 1.10269},
        {0.00517, 0.01467, 0.01180, 0.02061, 0.03478, 0.06020, 1.36048},
        {0.00740, 0.02101, 0.01690, 0.02952, 0.04980, 0.08620, 1.94813},
    }
};




float ART_CD4_rates[2][4][7]={
    {   {0.00677, 0.01416, 0.02495, 0.04362, 0.07563, 0.04681, 0.03685},
        {0.00677, 0.01416, 0.02495, 0.04362, 0.07563, 0.04681, 0.03685},
        {0.00677, 0.01416, 0.02495, 0.04362, 0.07563, 0.04681, 0.03685},
        {0.00677, 0.01416, 0.02495, 0.04362, 0.07563, 0.04681, 0.03685},
    },
    
    {   {0.00677, 0.01416, 0.02495, 0.04362, 0.07563, 0.04681, 0.03685},
        {0.00677, 0.01416, 0.02495, 0.04362, 0.07563, 0.04681, 0.03685},
        {0.00677, 0.01416, 0.02495, 0.04362, 0.07563, 0.04681, 0.03685},
        {0.00677, 0.01416, 0.02495, 0.04362, 0.07563, 0.04681, 0.03685},
    }
};


//////////////////////////////////////
//// --- FUNCTIONS FOR EVENTS --- ////
//////////////////////////////////////

//// --- NEW YEAR FUNCTION --- ////
void EventTellNewYear(person *MyPointerToPerson){				
																		
	cout << "A new year has started, it is now " << *p_GT << endl;				// This whole function to output every time a new year starts - to make sure the model is actually running
	
	// Lets get the pointer to the right year range
	if (*p_GT<1955){*p_PY = 0; };
	if (*p_GT >= 1955 && *p_GT<1960){*p_PY = 1; };
	if (*p_GT >= 1960 && *p_GT<1965){*p_PY = 2; };
	if (*p_GT >= 1965 && *p_GT<1970){*p_PY = 3; };
	if (*p_GT >= 1970 && *p_GT<1975){*p_PY = 4; };
	if (*p_GT >= 1975 && *p_GT<1980){*p_PY = 5; };
	if (*p_GT >= 1980 && *p_GT<1985){*p_PY = 6; };
	if (*p_GT >= 1985 && *p_GT<1990){*p_PY = 7; };
	if (*p_GT >= 1990 && *p_GT<1995){*p_PY = 8; };
	if (*p_GT >= 1995 && *p_GT<2000){*p_PY = 9; };
	if (*p_GT >= 2000 && *p_GT<2005){*p_PY = 10;};
	if (*p_GT >= 2005){*p_PY = 11; };
	E(cout << "Let's check relevant things have been updated... *p_PY: " << *p_PY << " and Global Time: " << *p_GT << endl;)

	// Schedule event for next year
	event * RecurrentTellNewYear = new event;
	Events.push_back(RecurrentTellNewYear);
	RecurrentTellNewYear->time = *p_GT + 1;										
	RecurrentTellNewYear->p_fun = &EventTellNewYear;
	p_PQ->push(RecurrentTellNewYear);

	E(cout << "We have finished telling you the new year and setting fertility variables for the year." << endl;)	// Error message - can be switched on/off
}


//// --- DEATH EVENT --- ////	
void EventMyDeathDate(person *MyPointerToPerson){

	if (MyPointerToPerson->Alive==1){MyPointerToPerson->Alive=0;}
	E(cout << "Person " << MyPointerToPerson->PersonID << " just died. Their life status now is: " << MyPointerToPerson->Alive << endl;)
}


//// --- BIRTH EVENT AND MAKING NEW PERSON --- ////
void EventBirth(person *MyPointerToPerson){								
		
	E(cout << "A birth is about to happen and my life status: " << endl;)		// Error message - can be switched on/off

	if(MyPointerToPerson->Alive == 1) {											// Only let woman give birth if she is still alive 
		
		total_population=total_population+1;									// Update total population for output and for next new entry
		MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);				// Update age to get age at birth for output
		

		// Creating a new person 
		MyArrayOfPointersToPeople[total_population-1]=new person();
		(MyArrayOfPointersToPeople[total_population-1])->PersonIDAssign(total_population-1);
		(MyArrayOfPointersToPeople[total_population-1])->Alive=1;
		(MyArrayOfPointersToPeople[total_population-1])->GenderDistribution();
		(MyArrayOfPointersToPeople[total_population-1])->GetMyDoBNewEntry();
		(MyArrayOfPointersToPeople[total_population-1])->GetDateOfDeath();
		(MyArrayOfPointersToPeople[total_population-1])->GetDateOfBaby();
		(MyArrayOfPointersToPeople[total_population-1])->GetMyDateOfHIVInfection();

	
		// Link Mother and Child
		(MyArrayOfPointersToPeople[total_population-1])->MotherID=MyPointerToPerson->PersonID;			// Give child their mothers ID
		MyPointerToPerson->ChildIDVector.push_back((MyArrayOfPointersToPeople[total_population-1]));	// Give mothers their child's ID
			
		E(cout << "We have finished giving birth " << endl;)					// Error message - can be switched on/off
	
	}
}


//// --- HIV EVENT --- ////
void EventMyHIVInfection(person *MyPointerToPerson){
	
	E(cout << "Somebody is about to get infected with HIV: " << endl;)			// Error message - can be switched on/off

	if(MyPointerToPerson->Alive == 1) {											// Only execute this is patient is still alove
	
		MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);				// Update age to get correct parameter below
	

		//// --- Get my CD4 count at start --- ////
		double	h = ((double)rand() / (RAND_MAX));								// Gets a random number between 0 and 1.
		int i=0;
		int j=0;
		int a=25;
	
		while (MyPointerToPerson->Age>=a && a<46){a=a+10; i++;};				// To get the right age-specific row in the above sex-specific arrays
		while (h>CD4_startarray[MyPointerToPerson->Sex-1][i][j] && j<3){j++;}	// To get the corresponding CD4 count for correct age and sex from random 'h' generated above
	
		MyPointerToPerson->CD4_cat=0+j;											// CD4 count cat (variable over time)
		MyPointerToPerson->CD4_cat_start=0+j;									// CD4 count cat at start (to keep in records)

	
		//// --- Let's see what will happen next (Death, CD4 count progression or ART initiation) ---- ////
        double FindART_CD4_rate = ART_CD4_rates[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
        double FindCD4_rate = CD4_rates[MyPointerToPerson->Sex-1][MyPointerToPerson->CD4_cat];
        double FindDeath_CD4_rate = Death_CD4_rates[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
        
        
        // Lets see when ART would start
        double ART_test = 0;
        double art = ((double)rand() / (RAND_MAX));
        ART_test = (-1/FindART_CD4_rate) * log(art);
        //cout <<  ART_test << endl;
        
        
        // Lets see when CD4 count progression would start
        double CD4_test = 0;
        double cd4 = ((double)rand() / (RAND_MAX));
        CD4_test = (-1/FindCD4_rate) * log(cd4);
        //cout <<  CD4_test << endl;
        
        
        // Lets see when death would happen
        double death_test = 0;
        double dd = ((double)rand() / (RAND_MAX));
        death_test = (-1/FindDeath_CD4_rate) * log(dd);
        //cout <<  death_test << endl;
        
        
        //cout << "Sex: " << MyPointerToPerson->Sex << " Age: " << MyPointerToPerson->Age << " CD4: " << MyPointerToPerson->CD4_cat << endl;
//        cout << "ART rate: " << ART_test << endl;
//        cout << "CD4 rate: " << CD4_test << endl;
//        cout << "Death rate: " << death_test << endl;
        
        
        //Before the introduction of ART
        if (*p_GT<2011){
            
            if (CD4_test<death_test){
                
                //cout << "We are scheduling a CD4 progression " << endl;
                
                event * CD4change = new event;
                Events.push_back(CD4change);
                CD4change->time = *p_GT+CD4_test;
                CD4change->p_fun = &EventCD4change;
                CD4change->person_ID = MyPointerToPerson;
                p_PQ->push(CD4change);
                
            }
            
            if (death_test<=CD4_test){
                
                //cout << "We are scheduling an HIV death " << endl;
                
                double death_test_date = *p_GT +death_test;                     // Get the actual date, not just time until death
                
                if (death_test_date<MyPointerToPerson->DateOfDeath){            // Check HIV deaths happens before natural death
                
                    MyPointerToPerson->DateOfDeath=death_test_date;
                    
                    event * DeathEvent = new event;								// In that case we need to add the new death date to the EventQ
                    Events.push_back(DeathEvent);
                    DeathEvent->time = MyPointerToPerson->DateOfDeath;
                    DeathEvent->p_fun = &EventMyDeathDate;
                    DeathEvent->person_ID = MyPointerToPerson;
                    p_PQ->push(DeathEvent);
                }
                
            }
            
        }
        
        // After the introduction of ART
        
      // TO INSERT!!!
        
     

    }

	E(cout << "Somebody has just been infected with HIV!" << endl;)				// Error message - can be switched on/off
}



void EventCD4change(person *MyPointerToPerson){
	
	E(cout << "Somebody is about to experience a drop in CD4 count: " << endl;)	// Error message - can be switched on/off

	if(MyPointerToPerson->Alive == 1) {											// Add any additional things to progression of CD4 count
	
		
        
        //// --- Let's get the right index for all relevant arrays used here and update important info ---- ////
        int i=0;
        int a=25;
        
        while (MyPointerToPerson->Age>=a && a<46){a=a+10; i++;};				// To get the right age-specific row in the above sex-specific arrays
        MyPointerToPerson->Age= (*p_GT - MyPointerToPerson->DoB);				// Update age to get correct parameter below
        MyPointerToPerson->CD4_cat=MyPointerToPerson->CD4_cat+1;                // Update CD4 count
        
        
        if (MyPointerToPerson->CD4_cat==6){
            
            double FindDeath_CD4_rate = Death_CD4_rates[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
            
            // Lets see when death would happen
            double death_test = 0;
            double dd = ((double)rand() / (RAND_MAX));
            death_test = (-1/FindDeath_CD4_rate) * log(dd);
            //cout <<  death_test << endl;
            
            double death_test_date = *p_GT +death_test;                      // Get the actual date, not just time until death
            
            if (death_test_date<MyPointerToPerson->DateOfDeath){            // Check HIV deaths happens before natural death
                
                MyPointerToPerson->DateOfDeath=death_test_date;
                
                event * DeathEvent = new event;								// In that case we need to add the new death date to the EventQ
                Events.push_back(DeathEvent);
                DeathEvent->time = MyPointerToPerson->DateOfDeath;
                DeathEvent->p_fun = &EventMyDeathDate;
                DeathEvent->person_ID = MyPointerToPerson;
                p_PQ->push(DeathEvent);
            };
        };

    

        if (MyPointerToPerson->CD4_cat<6){
        
            //// --- Let's see what will happen next (Death, CD4 count progression or ART initiation) ---- ////
            double FindART_CD4_rate = ART_CD4_rates[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
            double FindCD4_rate = CD4_rates[MyPointerToPerson->Sex-1][MyPointerToPerson->CD4_cat];
            double FindDeath_CD4_rate = Death_CD4_rates[MyPointerToPerson->Sex-1][i][MyPointerToPerson->CD4_cat];
            
            
            // Lets see when ART would start
            double ART_test = 0;
            double art = ((double)rand() / (RAND_MAX));
            ART_test = (-1/FindART_CD4_rate) * log(art);
            //cout <<  ART_test << endl;
            
            
            // Lets see when CD4 count progression would start
            double CD4_test = 0;
            double cd4 = ((double)rand() / (RAND_MAX));
            CD4_test = (-1/FindCD4_rate) * log(cd4);
            //cout <<  CD4_test << endl;
            
            
            // Lets see when death would happen
            double death_test = 0;
            double dd = ((double)rand() / (RAND_MAX));
            death_test = (-1/FindDeath_CD4_rate) * log(dd);
            //cout <<  death_test << endl;
            
            
            //cout << "Sex: " << MyPointerToPerson->Sex << " Age: " << MyPointerToPerson->Age << " CD4: " << MyPointerToPerson->CD4_cat << endl;
            //        cout << "ART rate: " << ART_test << endl;
            //        cout << "CD4 rate: " << CD4_test << endl;
            //        cout << "Death rate: " << death_test << endl;
            
            
            //Before the introduction of ART
            if (*p_GT<2011){
                
                if (CD4_test<death_test){
                    //cout << "We are scheduling a CD4 progression " << endl;
                    event * CD4change = new event;
                    Events.push_back(CD4change);
                    CD4change->time = *p_GT+CD4_test;
                    CD4change->p_fun = &EventCD4change;
                    CD4change->person_ID = MyPointerToPerson;
                    p_PQ->push(CD4change);
                    
                }
                
                if (death_test<=CD4_test){
                    
                    //cout << "We are scheduling an HIV death " << endl;
                    
                    double death_test_date = *p_GT +death_test;                      // Get the actual date, not just time until death
                    
                    if (death_test_date<MyPointerToPerson->DateOfDeath){            // Check HIV deaths happens before natural death
                        
                        MyPointerToPerson->DateOfDeath=death_test_date;
                        
                        event * DeathEvent = new event;								// In that case we need to add the new death date to the EventQ
                        Events.push_back(DeathEvent);
                        DeathEvent->time = MyPointerToPerson->DateOfDeath;
                        DeathEvent->p_fun = &EventMyDeathDate;
                        DeathEvent->person_ID = MyPointerToPerson;
                        p_PQ->push(DeathEvent);
                    }
                    
                }
                
            }
        }
        
        // After the introduction of ART
        
        // TO INSERT!!!

	}
	
	E(cout << "Somebody has just experiences a drop in CD4 count!" << endl;)	// Error message - can be switched on/off
}
	








