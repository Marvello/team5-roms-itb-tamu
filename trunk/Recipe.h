#ifndef RECIPE_H_
#define RECIPE_H_

#include <string>
#include <sstream>			//B.1.a
#include <iomanip>

// other types
#include "Instructions.h"				//uncomment once you have added the type definition
using namespace std;
 
struct Recipe {
public:
	// constructors
	Recipe (int id, string n, Instructions a)	//uncomment once you have added the type definition
		: recipe_id(id), chef_name(n), instr(a) {}

   // utility functions
	string display() const {	//B.1.a
		
		/* DTC B.2 */
		ostringstream oss;
		oss << setw(19) << left << chef_name;
		oss << instr.display() ;
		oss << '\n' ;
		return oss.str();
		/* DTC */

	}	//B.1.a
	int get_rec_id() const {return recipe_id;}	//B.1.a
	string get_chef()const {return chef_name;}	//B.1.b
	string get_instructions() const {return instr.display();} //JZ B1
private:
   //constants
     
   // private data
	int recipe_id;
	string chef_name;
	Instructions instr;			//uncomment once you have added the type definition
};

struct SortRecipesByChef		//B.1.b
{
   bool operator()(Recipe& a, Recipe& b) const
   {
	   return a.get_chef() < b.get_chef();
   }
};
#endif //RECIPE_H_
