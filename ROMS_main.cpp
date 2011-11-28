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

/*
 Class for displaying sample fonts.
 */
class FontDisplay : public Fl_Widget
{
  void draw(void);
  
public:
  int  font, size;
  
  int test_fixed_pitch(void);
  
  FontDisplay(Fl_Boxtype B, int X, int Y, int W, int H, const char *L = 0) 
  : Fl_Widget(X, Y, W, H, L)
  {
    box(B);
    font = 0;
    size = 16;
  }
};

/*
 Draw the sample text.
 */
void FontDisplay::draw(void)
{
  draw_box();
  fl_font((Fl_Font)font, size);
  fl_color(FL_BLACK);
  fl_draw(label(), x() + 3, y() + 3, w() - 6, h() - 6, align());
}

int FontDisplay::test_fixed_pitch(void)
{
  int w1, w2;
  int h1, h2;
  
  w1 = w2 = 0;
  h1 = h2 = 0;
  
  fl_font((Fl_Font)font, size);
  
  fl_measure("MHMHWWMHMHMHM###WWX__--HUW", w1, h1, 0);
  fl_measure("iiiiiiiiiiiiiiiiiiiiiiiiii", w2, h2, 0);
  
  if (w1 == w2) return 1; // exact match - fixed pitch
  
  // Is the font "nearly" fixed pitch? If it is within 5%, say it is...
  double f1 = (double)w1;
  double f2 = (double)w2;
  double delta = fabs(f1 - f2) * 5.0;
  if (delta <= f1) return 2; // nearly fixed pitch...
  
  return 0; // NOT fixed pitch
}

//header declarations
void do_about_box(Graph_lib::Window&);
void do_about_help(Graph_lib::Window&);
void do_grafTotSales(Graph_lib::Window&, ROMS_Menu&);
void do_read(Graph_lib::Window&, ROMS_Menu&, string, string, Msg_type);
void do_find_table_sales (Graph_lib::Window&, ROMS_Menu&); //Project II Part C.1 M.O.
void do_update_order_item (Graph_lib::Window&, ROMS_Menu&); //Project II Part C.1 M.O.
void Main_Window_CB(Fl_Widget*, void*);
void Menu_Bar_CB (Fl_Widget*, void*);
void display_total_sales_for_menu_item(Window& w, ROMS_Menu& M);//JZ C3
string call_function(int menu_item_id, ROMS_Menu& M);//JZ C3
void add_recipe(Window& w, ROMS_Menu& M); //JZ C3
string recipe_function(int id, string chef, string instr, ROMS_Menu& M);//JZ C3

// Find category
void find_table(Graph_lib::Window&, ROMS_Menu&);

// Add menu item
void update_menu_item(Graph_lib::Window&, ROMS_Menu&);

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
 {"Graf", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Total sales by month", 0,  (Fl_Callback*)Main_Window_CB, Address (Graf_totsales), 0, FL_NORMAL_LABEL, 0, 14, 0},
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
 void find_menu_item_cb(Address addr, Address)//JZ C3
{
     Main_Window_CB((Fl_Widget*) addr, Address(Find_menu_item_button));
}

void add_recipe_cb(Address addr, Address)//JZ C3
{
     Main_Window_CB((Fl_Widget*) addr, Address(Add_recipe_button));
}


// DTC C.2 Start
// Callback find total sales by server 
 void find_total_table(Address addr, Address)
 {
	Menu_Bar_CB((Fl_Widget*) addr, Address (Find_category_sales));	 
 }

// Callback for update menu item
 void update_menu_item_cb(Address addr, Address)
 {
	Menu_Bar_CB((Fl_Widget*) addr, Address (Update_add_menu_item));
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
				case About_info:
					do_about_box(sw);
					break;
				case About_help:
					do_about_help(sw);
					break;
				case Tables_button: case Orders_button: case Categories_button: case Recipes_button: case Menu_items_button:
					t.set_buffer(m.show_button((Msg_type) window_userdata).c_str());
					break;
				case Find_menu_item_sales://JZ C3
					display_total_sales_for_menu_item(sw, m);
					break;
				case Update_add_recipe://JZ C3
					add_recipe(sw, m);
					break;           
				case Exit_program:
					cout << "save files and exit\n";
					m.save_and_exit(); //JZ B1
					exit(0); //exit program
					break;
				case Show_order: case Show_recipe: case Show_menu:
					t.set_buffer(m.show_button((Msg_type) window_userdata).c_str());
					break;		
				case Find_table_sales: //Case for Find Total Sales By Table  Project II Part C.1 M.O.
					do_find_table_sales(sw,m);
					break;
				case Update_add_order_item: //Case for Update Order Items Project II Part C.1 M.O.
					do_update_order_item(sw,m);
					break;
				// DTC C2 START //
				// case if Find_category_sales
				case Find_category_sales:
					find_table(sw, m);
					break;
				case Update_add_menu_item:
					update_menu_item(sw, m);
					break;
				// DTC C2 END //
				case Graf_totsales: //Case if the Total Sales Graf Being called
					do_grafTotSales(sw,m);
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
	Graph_lib::Window ab(Point(w.x()+100, w.y()+100), 1060, 670, "About Box");
	ab.color(Color::white);
	ab.callback((Fl_Callback*)Menu_Bar_CB, Address (Close_about_box));
	Text msg(Point(15,50), "Ward Project Part II: Restaurant Order Management System");
	msg.set_color(Color::black);
	ab.attach(msg);
	Text msg2(Point(15,70), "Team 5");
	msg2.set_color(Color::black);
	ab.attach(msg2);
	Text msg3(Point(15,85), "Team Members: ");
	msg3.set_color(Color::black);
	ab.attach(msg3);
	Text msg4(Point(15,100), "Marvello Oni");
	msg4.set_color(Color::black);
	ab.attach(msg4);
	Image marvello(Point(15,110), "Marvello.jpeg");
	ab.attach(marvello);
	Text msg5(Point(394,100), "Dimas Tri Ciputra");
	msg5.set_color(Color::black);
	ab.attach(msg5);
	Image dimas(Point(394,110), "Dimas.jpg");
	ab.attach(dimas);
	Text msg6(Point(789,100), "Joanna Zhu");
	msg6.set_color(Color::black);
	ab.attach(msg6);
	Image joanna(Point(789,110), "Joanna.jpg");
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
void do_find_table_sales(Graph_lib::Window& w, ROMS_Menu& menu) {
        Graph_lib::Window ab(Point(w.x()+100, w.y()+100), 400, 120, "Find Total Sales by Table"); //Create the Window
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
void do_update_order_item (Graph_lib::Window& w, ROMS_Menu& menu) {
        Graph_lib::Window ab(Point(w.x()+100, w.y()+100), 400, 200, "Find Table Box"); // Create the window
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

// DTC C2 START

// Show find category sales window
void find_table(Graph_lib::Window& w, ROMS_Menu& m) {

	Graph_lib::Window nw(Point(w.x()+100, w.y()+100), 400, 150, "Find Table");
	nw.callback((Fl_Callback*)Menu_Bar_CB, Address (Close_about_box));

	// Create the input box
	In_box input(Point(120, 30), 90, 20, "Category ID : ");
	nw.attach(input);

	// Create button
	Button total(Point(240, 30), 120, 30, "Total",find_total_table);
	nw.attach(total);

	// Create the output box
	Text msg2(Point(40, 100), "Total :");
	nw.attach(msg2);

	Text out(Point(100, 100), "");
	nw.attach(out);

	wait_for_menu_bar_click();

	bool exit = false;
	while(!exit) {
		if(button_selected == Find_category_sales) {
			double total = m.find(2, input.get_int());

			if(total > 0) {
				ostringstream oss;
				oss << "$" << total;
				out.set_label(oss.str());
				wait_for_menu_bar_click();
			}
			else if(total < 0) {
				out.set_label("ID invalid");
				wait_for_menu_bar_click();
			}
			else {
				out.set_label("Empty");
				wait_for_menu_bar_click();
			}	
		}
		else {
			exit = true;
		}
	}
	return;
}

// Add Menu item
void update_menu_item(Graph_lib::Window& w, ROMS_Menu &m) {
	Graph_lib::Window ab(Point(w.x()+100, w.y()+100), 600, 200, "Update Menu Item"); // Create the window
	ab.callback((Fl_Callback*)Menu_Bar_CB, Address (Close_about_box));

	In_box menu_item_id(Point(100,10), 80, 20, "Menu Item ID:"); // Inbox for the menu item id
    In_box category_id(Point(100,40), 80, 20, "Category ID:"); // Inbox for the category id 
    In_box recipe_id(Point(100,70), 80, 20, "Recipe ID:"); //Inbox for recipe id
    In_box menu_item_name(Point(100,100), 80, 20, "Menu Item Name:"); // Inbox for menu item name
	In_box menu_item_price(Point(100, 130), 80, 20, "Price:"); // Inbox for price 
	In_box description(Point(100,160), 150, 20, "Description:"); // Inbox for description
	Text results(Point(100, 190), ""); //Text to show result
	Button search(Point(220, 10), 150, 20, "Insert Menu Item", update_menu_item_cb);  // 

	ab.attach(menu_item_id);
	ab.attach(category_id);
	ab.attach(recipe_id);
	ab.attach(menu_item_name);
	ab.attach(menu_item_price);
	ab.attach(description);
	ab.attach(results);
	ab.attach(search);

	wait_for_menu_bar_click();
	bool exit = false;

	while(!exit) {
		if(button_selected == Update_add_menu_item) {
			string price = "" + menu_item_price.get_string();
			double temp = atof(price.c_str());
			int result = m.updateMenuItem(menu_item_id.get_int(), category_id.get_int(), recipe_id.get_int(), menu_item_name.get_string(), temp, description.get_string());
			if (result > -1)
			{
				ostringstream oss; 
				oss << "Update Success";
				results.set_label(oss.str());
				wait_for_menu_bar_click();
			}
			else //If one the inbox is empty
			{
				results.set_label("Update Fail"); //Request for user entry
				wait_for_menu_bar_click();
			}
		}
		else {
			exit = true;
		}
	}

	ab.detach(menu_item_id);
	ab.detach(category_id);
	ab.detach(recipe_id);
	ab.detach(menu_item_name);
	ab.detach(menu_item_price);
	ab.detach(description);
	ab.detach(results);
	ab.detach(search);
	return;
}

// DTC C2 END


void display_total_sales_for_menu_item(Window& w, ROMS_Menu& M) //JZ C3
{
    Window win(Point(100, 100), 300, 300, "Total sales for menu item");
    win.color(Color::white);
	Text msg1(Point(15,50), "Display total sales for menu item");
	msg1.set_color(Color::black);
	win.attach(msg1);
	In_box in1(Point(100,60), 50, 30, "Menu Item ID: ");
	win.attach(in1);
	Text total(Point(50, 100), "Total Sales: ");
	total.set_color(Color::black);
	win.attach(total);
	Button mi_button(Point(150,150), 50, 20, "Find", find_menu_item_cb);
	win.attach(mi_button);
	try
    {
		bool exit = false;
        while(!exit)
    	{
    		main_click_enabled=true;
    		wait_for_main_window_click();
    		cout<<"Act on userdata="<<window_userdata<<endl;
    		switch(window_userdata)
    		{
				case Find_menu_item_button:
				total.set_label(call_function(in1.get_int(), M));
                break;
				default:
				cout<<"Case not implemented"<<endl;
				exit = true;
            }
    	}                      
    }
	
	catch(exception& e)
	{
        cerr<<"exception: "<<e.what()<<endl;
        return;
    }
    catch (...)
    {
          cerr<<"some exception: "<<endl;
          return;
    }
    
    win.detach(msg1);
    win.detach(in1);
    win.detach(mi_button);
	return;
}

string call_function(int menu_item_id, ROMS_Menu& M)//JZ C3
{
    stringstream sstr;
    double total=M.display_total_sales_for_menuitem(menu_item_id);
    if(total==0)
        return "No sales found for given menu item";
    sstr<<"Total Sales: $"<<total;
    return sstr.str();
}

void add_recipe(Window& w, ROMS_Menu& M) //JZ C3 Update
{
    Window win(Point(100, 100), 300, 300, "Add Recipe");
    win.color(Color::white);
    win.callback((Fl_Callback*)Menu_Bar_CB, Address (Close_about_box));
	In_box in1(Point(50,60), 50, 30, "Recipe ID: ");
	win.attach(in1);
	In_box in2(Point(50,90), 50, 30, "Chef Name: ");
	win.attach(in2);
	In_box in3(Point(50,120), 150, 20, "Instructions: ");
	win.attach(in3);
	Text results(Point(50, 150), ""); 
	results.set_color(Color::black);
	win.attach(results);
	Button recipe_button(Point(175,250), 50, 20, "Add", add_recipe_cb);
	win.attach(recipe_button);

	try
    {                
		bool exit = false;
         while(!exit) 
         {
            main_click_enabled=true;    
            wait_for_main_window_click();     
            cout<<"Act on userdata="<<window_userdata<<endl;
    		switch(window_userdata)
    		{
                 case Add_recipe_button:  
                        results.set_label(recipe_function(in1.get_int(),in2.get_string(), in3.get_string(), M));       
                        break;
                 default:
                         cout<<"Case not implemented"<<endl;
						 exit = true;
            }
        }             
    }
	
	catch(exception& e)
	{
        cerr<<"exception: "<<e.what()<<endl;
        return;
    }
    catch (...)
    {
          cerr<<"some exception: "<<endl;
          return;
    }
    
    win.detach(in1);
    win.detach(in2);
    win.detach(in3);
    win.detach(results);
    win.detach(recipe_button);
	return;
}

string recipe_function(int id, string chef, string instr, ROMS_Menu& M)//JZ C3
{
    int answer=M.add_recipe(id, chef,instr);
    stringstream oss;

    if (answer > -1)
    { 
            oss << "Update Success";
    }
    else //If one the inbox is empty
    {
            oss<<"Update Fail"; //Request for user entry
    }
    return oss.str();
}

void do_grafTotSales(Graph_lib::Window& w, ROMS_Menu& M) //Project Part D.1 M.O
{
	
	class Scale {                // data value to coordinate conversion
		int cbase;               // coordinate base
		int vbase;               // base of values
		double scale;
		public:
		Scale(int b, int vb, double s) :cbase(b), vbase(vb), scale(s) { }
		int operator()(int v) const { return cbase + (v-vbase)*scale; }
	};
	
	map<Point,double> TotSales = M.grafTotalSales(); // Get the total Sales sort by month
	map<Point,double>::iterator iter; // Iterator For map

	const int xmax = 850;
	const int ymax = 600;
	const int xoffset = 100;
	const int yoffset = 60;
	const int xspace = 40;
	const int yspace = 40;
	
	const int xlength = (xmax-50)-xoffset-xspace;   
    const int ylength = ymax-yoffset-yspace;
	
	// Create window
	Graph_lib::Window win(Point(w.x()+100, w.y()+100), xmax, ymax, "Total Sales Graph By Month");
	win.callback((Fl_Callback*)Menu_Bar_CB, Address (Close_about_box));
	
	//Get Total Sales First Month
	iter = TotSales.begin();
	const int first_point = iter->first.x;				

	//Get Total Sales last Month
	iter = TotSales.end();
	--iter;
	const int last_point = iter->first.x;

	// Scale of x values
	const double xscale = double(xlength)/(last_point - first_point);
	// Scale of y values 
	const double yscale = double(ylength)/100; 
	//Create Scale for X and Y axis
	Scale ys(ymax-yoffset,0,-yscale);
	Scale xs(xoffset,first_point,xscale);
	
	// Create polyline for point
	Open_polyline object1;
	
	//Create Axis
	Axis smbx(Axis::x, Point(xoffset,ymax-yoffset), xlength, (last_point - first_point), "Date");
	smbx.label.move(570, -15);
	Axis smby(Axis::y, Point(xoffset,ymax-yoffset), ylength, 5,"Total Sales");
	//Attach the axis
	win.attach(smbx);
	win.attach(smby);

	//Text For labeling the axis
	Text range1(Point(xoffset-35, (ymax-yoffset)), "$0"); 
	Text range2(Point(xoffset-35, (ymax-yoffset) - 100), "$100");
	Text range3(Point(xoffset-35, (ymax-yoffset) - 200), "$200");
	Text range4(Point(xoffset-35, (ymax-yoffset) - 300), "$300");
	Text range5(Point(xoffset-35, (ymax-yoffset) - 400), "$400");
	Text range6(Point(xoffset-35, (ymax-yoffset) - 500), "$500");
	win.attach(range1);
	win.attach(range2);
	win.attach(range3);
	win.attach(range4);
	win.attach(range5);
	win.attach(range6);

	Text month_1(Point(xs(2)-5,(xoffset, ymax-yoffset)+ 20),"Feb");
	Text month_2(Point(xs(3)-5,(xoffset, ymax-yoffset)+ 20),"Mar");
	Text month_3(Point(xs(4)-5,(xoffset, ymax-yoffset)+ 20),"Apr");
	Text month_4(Point(xs(5)-5,(xoffset, ymax-yoffset)+ 20),"May");
	Text month_5(Point(xs(6)-5,(xoffset, ymax-yoffset)+ 20),"Jun");
	Text month_6(Point(xs(7)-5,(xoffset, ymax-yoffset)+ 20),"Jul");
	Text month_7(Point(xs(8)-5,(xoffset, ymax-yoffset)+ 20),"Aug");
	Text month_8(Point(xs(9)-5,(xoffset, ymax-yoffset)+ 20),"Sep");
	Text month_9(Point(xs(10)-5,(xoffset, ymax-yoffset)+ 20),"Okt");
	win.attach(month_1);
	win.attach(month_2);
	win.attach(month_3);
	win.attach(month_4);
	win.attach(month_5);
	win.attach(month_6);
	win.attach(month_7);
	win.attach(month_8);
	win.attach(month_9);

	//Insert The total sales into a open polyline
	for (iter=TotSales.begin(); iter!=TotSales.end();iter++)
	{
		cout << iter->first.x << " = " << iter->second << endl;
		object1.add(Point(xs(iter->first.x), ys(iter->second/10)));
	}
	
	// Customize the polyline
	object1.set_color(Color::blue);

	// Attach the object
	win.attach(object1);

	wait_for_menu_bar_click();

	//Detach All Variable
	win.detach(smbx);
	win.detach(smby);
	win.detach(range1);
	win.detach(range2);
	win.detach(range3);
	win.detach(range4);
	win.detach(range5);
	win.detach(range6);
	win.detach(month_1);
	win.detach(month_2);
	win.detach(month_3);
	win.detach(month_4);
	win.detach(month_5);
	win.detach(month_6);
	win.detach(month_7);
	win.detach(month_8);
	win.detach(month_9);
	win.detach(object1);
}
