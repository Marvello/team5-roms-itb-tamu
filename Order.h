/*
	The directives starting with # below ensures that this file is read by the compiler only once
	even if it is #included several times. It is call an "include guard"
*/
#ifndef ORDER_H_
#define ORDER_H_

//other types
#include "Chrono.h"					
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace Chrono;

struct Order {
public:
	//data

	// constructors
	Order () {}
	Order(int oid, int sid, int tid, Date od, Time ot):
	order_id(oid), server_id(sid), table_id(tid), order_date(od), order_time(ot) {}
   // utility functions
	string display() const {	
		ostringstream oss;

		/*DTC B.2 */
		oss << left << setw(9) << order_id << " ";
		oss << left << setw(10) << server_id << " ";
		oss << left << setw(9) << table_id << " ";
		oss << right << order_date << left << "\t";
		oss << order_time;
		/*DTC B.2 */

		return oss.str();
	}
	int get_table_id() const {return table_id;}	//B.2.a
	int get_order_id() const {return order_id;}	//B.2.a
	int get_server_id() const {return server_id;} //JZ B1
	Date get_date() const {return order_date;}//JZ B1
	Time get_time() const {return order_time;}//JZ B1
private:
   //constants

   // private data
	int order_id;
	int server_id;
	int table_id;
	Date order_date;
	Time order_time;
};


#endif //ORDER_H_
