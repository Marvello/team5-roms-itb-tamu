#ifndef MENU_ITEM_H_
#define MENU_ITEM_H_

#include <string>
#include <iomanip>

// other types
#include "Description.h"				//uncomment once you have added the type definition
using namespace std;

struct Menu_Item
{
public:
	// constructors
	Menu_Item (int id, int cat, int rec, string n, double amt, Description s)	//uncomment once you have added the type definition
		: menu_item_id(id), cat_id(cat), recipe_id(rec), menu_item_name(n), price(amt), descr(s) {}

	 // utility functions
	string display() const {	//B.1.b

		/* DTC B.2 */
		string temp = "";
		ostringstream oss;

		oss << left << setw(20);

		if(menu_item_name.length() > 19)  {
			temp = menu_item_name.substr(0 , 18);
			oss << temp + "-";
		}
		else { 
			oss << menu_item_name ; 
		}

		oss << setw(11) << left << menu_item_id;
		oss << setw(7) << left << price;
		oss << descr.display();

		if( temp != "" ) {
			temp = menu_item_name.substr(19, menu_item_name.length() - 19);
			oss << "\n" << temp ;
		}
		return oss.str();
		/* DTC B.2 */
	}	
	string get_name() const {return menu_item_name;} //B.1.a
	int get_recipe_id() const {return recipe_id;} //B.1.a
	string get_description() const {return descr.display();}	//B.1.a
	int get_menu_item_id() const {return menu_item_id;}	//B.2.a
	double get_price() const {return price;}	//B.2.a
	int get_cat_id() const {return cat_id;}  // Part II B.3 M.O.
private:
   //constants
     
   // private data
	int menu_item_id;
	int cat_id;
	int recipe_id;
	string menu_item_name;
	double price;
	Description descr;			//uncomment once you have added the type definition
   
};


#endif //MENU_ITEM_H_
