/*
	Program: FlowForm OSL (FOSL)
	Module: gtkwidgets.h
	Author: Gary Conway <gconway@flowcorp.com>
	Created: 6-1-2013
	Updated: 
	Language: C,C++ (this module is C++ compilable and linkable with C libs)
	Version: 1.0
	Notice: Copyright 2019 FireKing Security, All Rights Reserved

	this module is simply an include module for the application
	it defines the app_ptr structure that we will use throughout
	this module is included with the other global declarations

	see gtkwidget_app.h for a more complte explanation

*/


/*
	we define a structure of widgets so that we can assign a pointer (app_ptr) to the
	struc and subsequently globally access all widget objects via a single pointer. (That's way cool!)
*/


/*
	Possible constructs:

	GtkWidget *ptr;
	GtkTable *ptr;
	GtkPixbuf *ptr;
	GtkListStore *ptr;
	GtkCellRenderer *ptr;
	GtkLabel *ptr;
	GtkComboBox *ptr;
	GtkToggleButton *ptr;
*/


//WIDGETS
typedef struct
{

	// windows
	GtkWidget *main_menu;
	GtkWidget *lockconfig_window;
	GtkWidget *load_window;
	GtkWidget *maint_window;
	GtkWidget *utd_maint_window;
	GtkWidget *mei_maint_window;
	GtkWidget *login_window;
	GtkWidget *splash_window;
	GtkWidget *status_window;
	GtkWidget *admin_window;
	GtkWidget *user_window;


	// user
	GtkWidget *user_close_btn;
	GtkWidget *user_cancel_btn;
	GtkLabel *username_lbl;
	GtkLabel *firstname_lbl;
	GtkLabel *lastname_lbl;
	GtkLabel *userlevel_lbl;
	GtkLabel *dept_lbl;
	GtkLabel *useractive_lbl;
	GtkLabel *user_id_lbl;
	GtkLabel *lang_lbl;
	GtkLabel *lastmodified_lbl;

	GtkWidget *username_txt;
	GtkWidget *firstname_txt;
	GtkWidget *lastname_txt;
	GtkComboBox *userlevel_combo;
	GtkComboBox *user_dept_combo;
	GtkWidget *user_active_switch;
	GtkLabel *user_id_txt;
	GtkLabel *lastmodified_txt;
	GtkComboBox *lang_combo;



	// admin
	GtkWidget *user_btn;
	GtkWidget *admin_close_btn;

	// status
	GtkLabel *status_lbl;
	GtkWidget *status_ok_btn;

	// splash window
    GtkWidget *login_exit_btn;
	GtkWidget *verify_note_btn;


	// login window
	GtkWidget *en_btn;
	GtkWidget *es_btn;
	GtkWidget *fr_btn;
	GtkWidget *he_btn;
	GtkWidget *login_btn;
	GtkWidget *login_back_btn;
	GtkLabel *pw_txt;
	GtkLabel *user_txt;
	GtkWidget *user_entry;
	GtkWidget *pw_entry;

	GtkWidget *one_btn;
    GtkWidget *two_btn;
    GtkWidget *three_btn;
    GtkWidget *four_btn;
    GtkWidget *five_btn;
    GtkWidget *six_btn;
    GtkWidget *seven_btn;
    GtkWidget *eight_btn;
    GtkWidget *nine_btn;
    GtkWidget *zero_btn;
    GtkWidget *tab_btn;
    GtkWidget *bksp_btn;



// load_window
//	GtkWidget *close_load_button;
//	GtkWidget *stop_load_button;
//	GtkWidget *load_button;
//	GtkWidget *unload_all_button;


	GtkToggleButton *dayssame;

//	GtkLabel *dist_title_label;
//	GtkTable *screw_comp_table;
//	GtkTextBuffer *global_home_offset_textbuffer;
//	GdkPixbuf *img;
//	GtkListStore *actuator_liststore;
//	GtkCellRenderer *pl_active_cellrenderer;
//	GtkMenuItem *file_menuitem;


// inventory grid on ShowLoad window
    GtkLabel *inv0;
    GtkLabel *inv1;
    GtkLabel *inv2;
    GtkLabel *inv3;
    GtkLabel *inv4;
    GtkLabel *inv5;
    GtkLabel *inv6;
    GtkLabel *inv7;


// main window
	GtkWidget *lock_config_btn;
	GtkWidget *maint_btn;
	GtkWidget *load_btn;
	GtkWidget *unload_btn;
	GtkWidget *lock_btn;
	GtkWidget *logout_btn;
	GtkWidget *admin_btn;


// maint window
	GtkWidget *mei_button;
	GtkWidget *utd_button;
	GtkWidget *maint_lock_button;
	GtkWidget *maint_close_btn;
	GtkLabel *utd_maint_title;
	GtkLabel *maint_title;

// utd maint window
	GtkWidget *zero_utd_btn;
	GtkWidget *utd_close_btn;
	GtkWidget *utd_set_addr_btn;
	GtkWidget *utd_get_addr_btn;
	GtkWidget *utd_get_inventory_btn;
	GtkWidget *utd_unloadall_btn;
	GtkWidget *utd_reset_btn;

// mei maint window
	GtkWidget *mei_reset_btn;
	GtkWidget *mei_close_btn;
	GtkWidget *mei_maint_title;

	GtkTextBuffer *textbuffer1;

	GtkComboBox *lock_hr;
	GtkComboBox *unlock_hr;
	GtkComboBox *lock_min;
	GtkComboBox *unlock_min;
	GtkComboBoxText *LockSelector;
//    GtkComboBox *LockSelector;

	GtkComboBox *DaySelector;

	// active buttons
	GtkLabel *config_active_lbl;
	GtkToggleButton *sun_active;
	GtkToggleButton *mon_active;
	GtkToggleButton *tue_active;
	GtkToggleButton *wed_active;
	GtkToggleButton *thu_active;
	GtkToggleButton *fri_active;
	GtkToggleButton *sat_active;


	// lock configuration labels
	GtkLabel *sun_lock_lbl;
	GtkLabel *sun_unlock_lbl;
	GtkLabel *mon_lock_lbl;
	GtkLabel *mon_unlock_lbl;
    GtkLabel *tue_lock_lbl;
    GtkLabel *tue_unlock_lbl;
    GtkLabel *wed_lock_lbl;
    GtkLabel *wed_unlock_lbl;
    GtkLabel *thu_lock_lbl;
    GtkLabel *thu_unlock_lbl;
    GtkLabel *fri_lock_lbl;
    GtkLabel *fri_unlock_lbl;
    GtkLabel *sat_lock_lbl;
    GtkLabel *sat_unlock_lbl;

	GtkLabel *config_timelock;
	GtkLabel *config_day_word;
	GtkLabel *config_lock_word;
	GtkLabel *config_unlock_word;
	GtkLabel *config_lock_word_2;
	GtkLabel *config_unlock_word_2;


	// buttons
/*
	GtkWidget *one_btn;
	GtkWidget *two_btn;
	GtkWidget *three_btn;
	GtkWidget *four_btn;
	GtkWidget *five_btn;
	GtkWidget *six_btn;
	GtkWidget *seven_btn;
	GtkWidget *eight_btn;
	GtkWidget *nine_btn;
	GtkWidget *zero_btn;
*/
	GtkWidget *enter_btn;
	GtkWidget *clear_btn;
	GtkWidget *escape_btn;
	GtkWidget *help_btn;

/*
	GtkWidget *load_btn1;
    GtkWidget *drop_btn;
    GtkWidget *doors_btn;
    GtkWidget *display_btn;
    GtkWidget *report_btn;
    GtkWidget *insta1_btn;
    GtkWidget *insta2_btn;
    GtkWidget *insta3_btn;
    GtkWidget *insta4_btn;
    GtkWidget *vend_btn;
    GtkWidget *unload_btn1;
    GtkWidget *buy_change_btn;
    GtkWidget *pgrm_btn;
    GtkWidget *drop_set_btn;
*/
	GtkWidget *left1_btn;
    GtkWidget *left2_btn;
    GtkWidget *left3_btn;
    GtkWidget *left4_btn;

	GtkWidget *up_right_btn;
	GtkWidget *dn_left_btn;

	GtkWidget *lockconfig_close_btn;


} AppWidgets;

AppWidgets *app_ptr;









