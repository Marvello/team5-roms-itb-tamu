/*
	The directives starting with # below ensures that this file is read by the compiler only once
	even if it is #included several times. It is call an "include guard"
*/
#ifndef CATEGORY_H_
#define CATEGORY_H_

#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

struct Category {
public:
	// constructors
	Category () {}
	Category (int id, string n)
		:cat_id(id), cat_name(n) {}
    // utility functions
	string display() const {	
		ostringstream oss;

		/* DTC B.2 */
		oss << left << setw(10) << cat_id;
		oss << cat_name ;
		/* DTC B.2 */

		return oss.str();
	}

    int get_cat_id() const {return cat_id;}  // Part II B.3 M.O.
    string get_catname() const {return cat_name;}//JZ B1
private:
   //constants
     
   // private data
	int cat_id;
	string cat_name;
};


#endif //CATEGORY_H_
