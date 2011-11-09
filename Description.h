
#ifndef DESCRIPTION_H_
#define DESCRIPTION_H_

#include <string>
#include <sstream>			//B.1.a
using namespace std;

struct Description {
public:
	// constructors
	Description () {}
	Description (string s) : description(s) {}
    // utility functions
	string display() const {	//B.1.a
		ostringstream oss;
		oss << description ;
		return oss.str();
	}	

private:
   //constants
     
   // private data
	string description;
};


#endif //DESCRIPTION_H_
