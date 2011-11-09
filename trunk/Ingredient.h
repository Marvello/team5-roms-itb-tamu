#ifndef INGREDIENT_H_
#define INGREDIENT_H_

#include <string>
#include <sstream>			//B.1.a
using namespace std;


struct Ingredient {
public:
	// constructors
	Ingredient () {}
	Ingredient(int iid, int rid, float a, string u, string n):
	ingredient_id(iid), recipe_id(rid), amt(a), units(u), name(n) {}
   // utility functions
	string display() const {	//B.1.a
		ostringstream oss;
		oss << amt << " " << units << " " << name;
		return oss.str();
	}
	int get_rec_id() const {return recipe_id;} //B.1.a
	int get_iid() const {return ingredient_id;} //JZ B1
	float amount() const {return amt;}//JZ B1
	string get_units() const {return units;}//JZ B1
	string get_name() const {return name;}//JZ B1

private:
   //constants  

   // private data
	int ingredient_id;
	int recipe_id;
	float amt;
	string units;
	string name;
};


#endif //INGREDIENT_H_
