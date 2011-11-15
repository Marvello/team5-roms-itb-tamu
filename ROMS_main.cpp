//============================================================================
// Name        : ROMS_main.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : C++ project Part II for your class (121, or 112) 
//============================================================================
//
#include "Simple_window.h"
#include "Graph.h"
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Display.H>

#include "ROMS_Menu.h"

using namespace std;
using namespace ROMS;
using namespace Graph_lib;

//types we need for a ROMS GUI interface-- a memu_bar and a place to display text and a means of attaching them to a Window.

namespace Graph_lib {
//------------------------------------------------------------------------------
	struct Window_text_display : Widget {
		Window_text_display(Point XY, int W, int H, string L, Callback cb): Widget(XY, W, H, L, cb)
		{}
		void set_buffer(string s);
		void attach(Window&);
	private:
		Fl_Text_Buffer data;
		Fl_Text_Display *ptd;

	};
	void Window_text_display::attach(Window& win)
	{
		
		ptd = new Fl_Text_Display(loc.x, loc.y, width, height, label.c_str());
		ptd->textfont(FL_COURIER);
		ptd->buffer(&data);	
		pw  = (Fl_Widget*) ptd;
		pw->callback(reinterpret_cast<Fl_Callback*>(do_it), &win); // pass the window
		own = &win;
	}
	void Window_text_display::set_buffer(string s) 
	{
		
		data.text(s.c_str());
	}

//------------------------------------------------------------------------------
	struct Menu_bar : Widget {
		Menu_bar(Point XY, int W, int H, Callback cb): Widget(XY, W, H, "", cb)
		{}
		void menu(Fl_Menu_Item *ma);
		void attach(Window&);
	private:
		Fl_Menu_Bar *pmb;
		Fl_Menu_Item *ma;

	};
	void Menu_bar::attach(Window& win)
	{
		pmb = new Fl_Menu_Bar(loc.x, loc.y, width, height);
		pmb->menu(ma);
		pw  = (Fl_Widget*) pmb;
		pw->callback(reinterpret_cast<Fl_Callback*>(do_it), &win); // pass the window
		own = &win;
	}
	void Menu_bar::menu(Fl_Menu_Item *m) 
	{
		ma = m;
	}
}

//header declarations
void do_about_box(Graph_lib::Window&);
void do_about_help(Graph_lib::Window&);
void do_read(Graph_lib::Window&, ROMS_Menu&, string, string, Msg_type);
void do_find_table_sales (Graph_lib::Window&, ROMS_Menu&); //Project II Part C.1 M.O.
void do_update_order_item (Graph_lib::Window&, ROMS_Menu&); //Project II Part C.1 M.O.
void Main_Window_CB(Fl_Widget*, void*);
void Menu_Bar_CB (Fl_Widget*, void*);


//global data
bool main_window_click = false;
bool menu_bar_click = false;
bool main_click_enabled = true;//used to force serialization on clicks
int window_userdata;
int button_selected;

//main window menu bar
//Note: for expediancy sake, to define the menu bar below we used an FLTK type Fl_Menu_Item. 
//This makes our program  directly tied to FLTK instead of Dr. Stroustrup's interface Graph_lib.
//We don't like this, but we did it to save time, which often happens in the real world.
//A better solution is to write code to hide this in the implementation of the Graph_lib::Menu_bar, but that means adding
//changes to it and getting general agreement with Dr. Stroustrup. Subsequent testing and distribution would take time we don't have.
//An extra credit exercise is to see if you can eliminate direct references in your code to FLTK types like we have here
Fl_Menu_Item menu_bar[] = {
 {"Read", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Recipes", 0,  (Fl_Callback*)Main_Window_CB, Address (Read_recipes_ingredients), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Orders", 0,  (Fl_Callback*)Main_Window_CB, Address (Read_orders), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Menu Items", 0,  (Fl_Callback*)Main_Window_CB, Address (Read_category_menu_items), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Show", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Orders", 0,  (Fl_Callback*)Main_Window_CB, Address (Show_order), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Recipes", 0,  (Fl_Callback*)Main_Window_CB, Address (Show_recipe), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Menu Items", 0,  (Fl_Callback*)Main_Window_CB, Address (Show_menu), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Categories", 0,  (Fl_Callback*)Main_Window_CB, Address (Show_catagories), 0, FL_NORMAL_LABEL, 0, 14, 0}, //Project II Part B.3 M.O.
 {"Order Items", 0,  (Fl_Callback*)Main_Window_CB, Address (Show_tables), 0, FL_NORMAL_LABEL, 0, 14, 0}, //Project II Part B.3 M.O.
 {0,0,0,0,0,0,0,0,0},
 {"Find", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Category Sales", 0,  (Fl_Callback*)Main_Window_CB, Address (Find_category_sales), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Table Sales", 0,  (Fl_Callback*)Main_Window_CB, Address (Find_table_sales), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Menu Item Sales", 0,  (Fl_Callback*)Main_Window_CB, Address (Find_menu_item_sales), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Update", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Add Order Item", 0,  (Fl_Callback*)Main_Window_CB, Address (Update_add_order_item), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Add Menu Item", 0,  (Fl_Callback*)Main_Window_CB, Address (Update_add_menu_item), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Add Recipe", 0,  (Fl_Callback*)Main_Window_CB, Address (Update_add_recipe), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"About", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Info", 0,  (Fl_Callback*)Main_Window_CB, Address (About_info), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Help", 0,  (Fl_Callback*)Main_Window_CB, Address (About_help), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Exit", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Save and exit", 0,  (Fl_Callback*)Main_Window_CB, Address (Exit_program), 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};


//call back functions
void recps_cb(Address addr, Address) // callback for recipes button
{
	Main_Window_CB((Fl_Widget*) addr, Address (Tables_button));
}
void order_cb(Address addr, Address) // callback for orders button
{
	Main_Window_CB((Fl_Widget*) addr, Address (Orders_button));
}
void cat_cb(Address addr, Address) // callback for category button
{
	Main_Window_CB((Fl_Widget*) addr, Address (Show_catagories)); //Project II Part B.3 M.O.
}
void mi_cb(Address addr, Address) // callback for menu item button
{
	Main_Window_CB((Fl_Widget*) addr, Address (Menu_items_button)); //Project II Part B.3 M.O.
}
void oi_cb(Address addr, Address) // callback for order item button
{
	Main_Window_CB((Fl_Widget*) addr, Address (Recipes_button));
}
 void display_cb(Address addr, Address) // callback for display window
{
	Main_Window_CB((Fl_Widget*) addr, Address (Display_window));
}
 void menu_bar_cb(Address addr, Address)//callback for menu bar
{
	Menu_Bar_CB((Fl_Widget*) addr, Address (Menu_bar_cb));
}

//callback for find sales by table Project II Part C.1 M.O.
void find_table_cb(Address addr, Address)
{
	Menu_Bar_CB((Fl_Widget*) addr, Address (Find_table_sales));
}
//callback for update order item  Project II Part C.1 M.O.
void update_order_item_cb(Address addr, Address)
{
	Menu_Bar_CB((Fl_Widget*) addr, Address (Update_add_order_item));
}

void Menu_Bar_CB (Fl_Widget*, void* ud) {
		cout << "Menu Bar Callback" << " Userdata=" << (int) ud << "\n";
		button_selected = (int) ud;
		menu_bar_click = true;
}

void Main_Window_CB(Fl_Widget*, void* ud) {
	if(main_click_enabled)//don't signal the window click if main loop is active
	{
		cout << "Main Window Callback" << " Userdata=" << (int) ud << "\n";
		window_userdata = (int) ud;
		main_window_click = true;
		main_click_enabled = false; //main loop active on current click
	}
}
//wait functions for callbacks to be executed
void wait_for_main_window_click() {
	while (!main_window_click) 
		Fl::wait();
	main_window_click = false;
	Fl::redraw();
}

void wait_for_menu_bar_click() {
	while (!menu_bar_click)
		Fl::wait();
	menu_bar_click = false;
	Fl::redraw();
}

int main()
{
	//create the main window and display it
	
	ROMS_Menu m;
    Graph_lib::Window sw(600, 500, "Restaurant Order Management System");
    Button recps(Point(40, 50), 100, 30, "Recipes", recps_cb);
	sw.attach(recps);
	Button order(Point(40, 100), 100, 30, "Orders", order_cb);
	sw.attach(order);
	Button cat(Point(40, 150), 100, 30, "Categories", cat_cb);
	sw.attach(cat);
	Button mi(Point(40, 200), 100, 30, "Menu Items", mi_cb);
	sw.attach(mi);
	Button ci(Point(40, 250), 100, 30, "Order Items", oi_cb);
	sw.attach(ci);
    sw.callback((Fl_Callback*)Main_Window_CB, Address (Exit_program));
	Menu_bar mb(Point(0, 0), 600, 30, menu_bar_cb);
    mb.menu(menu_bar);
	sw.attach(mb);

	Window_text_display t(Point(150,50),400, 400, "Data Display", display_cb );
	t.set_buffer("No Data Loaded\n");
	sw.attach(t);
 	sw.resizable(&sw);
	

	//main loop to execute user clicks on the main window
	//do you understand how this loop works compared to the way the main loop worked in Part I of the project?
	try {
		while(true) {
			main_click_enabled = true;
			wait_for_main_window_click();
			cout << "Act on userdata = " << window_userdata << endl;
			switch(window_userdata) 
			{
				//need to complete all the menu cases
				case Read_recipes_ingredients:
					do_read(sw, m, "recipes.dat", "Open Recipes/Ingredients File", Read_recipes_ingredients);
					break;
				case Read_orders:
					do_read(sw, m, "orders.dat", "Open Order/Order Items File", Read_orders);
					break;
				case Read_category_menu_items:
					do_read(sw, m, "catmenu.dat", "Open Category/Menu Item File", Read_category_menu_items);
					break;
				case Show_catagories: //Project II Part B.3 M.O.
					t.set_buffer(m.showCatagories());
					break; 
				case Show_tables: //Project II Part B.3 M.O.
					t.set_buffer(m.showOrderItem());
					break;
				case Find_table_sales: //Case for Find Total Sales By Table  Project II Part C.1 M.O.
					do_find_table_sales(sw,m);
					break;
				case Update_add_order_item: //Case for Update Order Items Project II Part C.1 M.O.
					do_update_order_item(sw,m);
					break;
				case About_info:
					do_about_box(sw);
					break;
				case About_help:
					do_about_help(sw);
					break;
				case Tables_button: case Orders_button: case Categories_button: case Recipes_button: case Menu_items_button:
					t.set_buffer(m.show_button((Msg_type) window_userdata).c_str());
					break;
				case Exit_program:
					cout << "save files and exit\n";
					m.save_and_exit(); //JZ B1
					exit(0); //exit program
					break;
				case Show_order: case Show_recipe: case Show_menu:
					t.set_buffer(m.show_button((Msg_type) window_userdata).c_str());
					break;			
				default:
					cout << "case not implemented\n";
			}
		}
		return 0;
	}
	catch(exception& e) {
		cerr << "exception: " << e.what() << '\n';
		return 1;
	}
	catch (...) {
		cerr << "Some exception\n";
		return 2;
	}
}
//show about box info window
void do_about_box(Graph_lib::Window& w) //JZ B1
{
	Window ab(Point(w.x()+100, w.y()+100), 400, 120, "About Box");
	ab.color(Color::white);
	ab.callback((Fl_Callback*)Menu_Bar_CB, Address (Close_about_box));
	Text msg(Point(15,50), "Ward Project Part II: Restaurant Order Management System");
	msg.set_color(Color::black);
	ab.attach(msg);
	Text msg2(Point(15,60), "Team 5");
	msg2.set_color(Color::black);
	ab.attach(msg2);
	Text msg3(Point(15,70), "Team Members: ");
	msg3.set_color(Color::black);
	ab.attach(msg3);
	Text msg4(Point(15,80), "Marvello Oni");
	msg4.set_color(Color::black);
	ab.attach(msg4);
	Image marvello(Point(15,90), "Marvello.jpeg");
	ab.attach(marvello);
	Text msg5(Point(394,80), "Dimas T Cuputa");
	msg5.set_color(Color::black);
	ab.attach(msg5);
	Image dimas(Point(394,90), "Dimas.jpg");
	ab.attach(dimas);
	Text msg6(Point(789,80), "Joanna Zhu");
	msg6.set_color(Color::black);
	ab.attach(msg6);
	Image joanna(Point(789,90), "Joanna.jpg");
	ab.attach(joanna);
	wait_for_menu_bar_click();
	ab.detach(msg);//clean up window
	return;
}

//show Help window
void do_about_help(Graph_lib::Window& w) //Project II Part B.3 M.O.
{
	Graph_lib::Window ab(Point(w.x()+100, w.y()+100), 440, 150, "Help Box");
	ab.color(Color::white);
	ab.callback((Fl_Callback*)Menu_Bar_CB, Address (Close_about_box));
	Text msg(Point(15,50), "This program is used for manage restaurant order system.");
	Text msg2(Point(15,75), "You MUST read the external file first before using the program.");
	Text msg3(Point(15,100), "Other function can be used after read has been called.");
	msg.set_color(Color::black);
	ab.attach(msg);
	ab.attach(msg2);
	ab.attach(msg3);
	wait_for_menu_bar_click();
	ab.detach(msg);//clean up window
	return;
}

void do_read(Graph_lib::Window& w, ROMS_Menu& m, string dfn, string msg, Msg_type type)
{
	//Here again we linked our code directly to the FLTK library instead of creating an interface in our Graph_lib interface to FLTK
	//We did it for lack of time, which reflects the real word. Unfortunately if we wanted to move our code to a different graphics
	//library, we have to change our code and not just our Graphic_lib implementation. we are using the FLTK file chooser directly.
	Fl_File_Chooser fc(dfn.c_str(), "*.dat", Fl_File_Chooser::CREATE, msg.c_str());
	fc.show();
	while (fc.shown())
		Fl::wait();	//wait for OK or Cancel button
	if(0!=fc.value())//make sure there is a string to use as a filename
	{
		string fname = fc.value();
		switch(type) {
			case Read_recipes_ingredients:
				m.read_recipes_ingredients(fname);
				break;
			case Read_orders:
				m.Read_orders(fname);
				break;
			case Read_category_menu_items:
				m.read_catprds(fname);
				break;
			default:
				throw InvalidType();
		}
	}
	return;
}

//Procedure to find sales by table Project II Part C.1 M.O.
void do_find_table_sales(Window& w, ROMS_Menu& menu) {
        Window ab(Point(w.x()+100, w.y()+100), 400, 120, "Find Total Sales by Table"); //Create the Window
        ab.color(Color::white);
        ab.callback((Fl_Callback*)Menu_Bar_CB, Address (Close_about_box));

        //text box
		In_box TabInBox(Point(130,30), 110, 20,"Insert Table ID:"); // Inbox for the Table ID
		ab.attach(TabInBox); // Attach Inbox to Windows
        
		//text
        Text sum_txt(Point(60, 80), ""); //Text to show result
        ab.attach(sum_txt); //Attach Text to Windows

		//Button
		Button Submit(Point(250,30),110,20,"Submit",find_table_cb); //Submit Button for find total sales
		ab.attach(Submit);//Attach button to Windows
		wait_for_menu_bar_click();

		bool exit = false;
        while(!exit) {
				if(button_selected == Find_table_sales) { //If the button is pressed
						double temp = menu.find(1,TabInBox.get_int()); //Get total sales for table ID in inbox
						if (temp > 0)
						{
							cout <<"Total Sales For Table ID " << TabInBox.get_int() << " are $" << temp << endl; //For Debugging purpose
							ostringstream oss; 
							oss << "Total Sales For Table ID " << TabInBox.get_int() << " are $" << temp;
							sum_txt.set_label(oss.str()); //Set the string to result Text
							wait_for_menu_bar_click();
						}
						else
						{
							sum_txt.set_label("ID is invalid or inbox is empty"); //Set the string to result Text
							wait_for_menu_bar_click();
						}
                }
                //anything else
                else {
                        exit = true;
                }
        }
		ab.detach(TabInBox); //Detach the inbox
		ab.detach(sum_txt); //Detach the text
		ab.detach(Submit); //Detach the button
}

//Procedure to update order item Project II Part C.1 M.O.
void do_update_order_item (Window& w, ROMS_Menu& menu) {
        Window ab(Point(w.x()+100, w.y()+100), 400, 200, "Find Table Box"); // Create the window
        ab.color(Color::white);
        ab.callback((Fl_Callback*)Menu_Bar_CB, Address (Close_about_box));

        In_box input_order_item_id(Point(100,10), 80, 20, "Order Item ID:"); // Inbox for the order item id
        In_box input_menu_item_id(Point(100,40), 80, 20, "Menu Item ID:"); // Inbox for the menu item id 
        In_box input_seat_id(Point(100,70), 80, 20, "Seat ID:"); //Inbox for the seat id
        In_box input_qty(Point(100,100), 80, 20, "Quantity:"); // Inbox for the quantity id
		Text result(Point(100, 140), ""); //Text to show result
        Button search_table(Point(220, 10), 150, 20, "Insert Order Item", update_order_item_cb);  // Create the button for search purpose - DS C update order item

        ab.attach(input_order_item_id); // Attach the order item id inbox
        ab.attach(input_menu_item_id); // Attach the menu item id inbox
        ab.attach(input_seat_id); // Attach the seat id inbox
        ab.attach(input_qty); // Attach the quantity inbox
		ab.attach(result); //Attach the result text
        ab.attach(search_table); // Attach the update button
		wait_for_menu_bar_click();

		bool exit = false;
        while(!exit) {
				if(button_selected == Update_add_order_item) { //If the button is pressed			
						int temp = menu.updateOrderItem(input_order_item_id.get_int(),input_menu_item_id.get_int(),input_qty.get_int(),input_seat_id.get_string()); //Update the order item
						cout << temp << endl; //for debugging purpose
						if (temp > -1)
						{
							ostringstream oss; 
							oss << "Order Item Successfully add. Number of order items is now " << temp;
							result.set_label(oss.str());
							wait_for_menu_bar_click();
						}
						else //If one the inbox is empty
						{
							result.set_label("Update Order Item Fail"); //Request for user entry
							wait_for_menu_bar_click();
						}

				}
                //anything else
                else {
                        exit = true;
                }
        }

		ab.detach(input_order_item_id); // Detach the order item id inbox
        ab.detach(input_menu_item_id); // Detach the menu item id inbox
        ab.detach(input_seat_id); // Detach the seat id inbox
        ab.detach(input_qty); // Detach the quantity inbox
		ab.detach(result); // Detach the result text
        ab.detach(search_table); // Detach the update button
}