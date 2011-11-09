
#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include <string>
#include <sstream>			//B.1.a
#include <iomanip>
using namespace std;

struct Instructions {
public:
	// constructors
	Instructions () {}
	Instructions (string s): instructions(s) {}
    // utility functions
	string display() const {	//B.1.a
		ostringstream oss;

		/* DTC B.2 */
		int i = 0, j = 0;

		while(j < instructions.length()) {
			if(instructions[i] != '\n') {
				oss << instructions[i];
				if(instructions[i] == '.') {
					if(instructions[i-1] < 47  || instructions[i-1] > 58) {
						oss << "\n" << right << setw(22);
					}
				}
			}
			else if(instructions[i] == '\n') { 
				oss << instructions[i] << right << setw(20);
			}	
			i++;
			j++;
		}
		/* DTC B.2 */

		return oss.str();
	}	//B.1.a

private:
   //constants
     
   // private data
	string instructions;
};


#endif //INSTRUCTIONS_H_
