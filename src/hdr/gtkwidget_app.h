/*
	Program: FlowForm OSL (FOSL)
	Module: gtkwidget_app.h
	Author: Gary Conway <gconway@flowcorp.com>
	Created: 6-1-2013
	Updated: 
	Language: C,C++ (this module is C++ compilable and linkable with C libs)
	Version: 1.0
	Notice: All Rights Reserved


	The Global Widgettry
	--------------------

	this module is simply an include module for the application
	this module is included in main()
	the module places all the widgets defined in gtkwidgets.h (AppWidgets *app;)
	into the builder object using a series of MACROS in this file

	we then connect the builder object and the app object
	gtk_builder_connect_signals(builder,app);

	we then assign a global pointer, AppWidgets *app_ptr = app;
	subsequently, all widgets can then be addressed via the global pointer as app_ptr->widget


*/




// the following definition macros show how the GDK backend is configured
#ifdef GDK_WINDOWING_X11
#endif

#ifdef GDK_WINDOWING_WIN32
#endif

	// note that the following function call is unnecessary, because gtk_init() already calls this
	// function. I have a reference here simply as a note to self for later
	// this is virutally the same as calling  the C function setlocale(), but also takes care of locale specific items for GDK
	// this function returns a string in the form lang_COUNTRY, where lang is an ISO-639 language code
	// and COUNTRY is an ISO-3166 country code
	//	gtk_set_locale();

	// create appGET type macro that loads each widget into our local object (app)
	#define appGET(xx) 	app->xx=GTK_WIDGET(gtk_builder_get_object(builder,#xx))
	#define appGET_TABLE(xx) app->xx=GTK_TABLE(gtk_builder_get_object(builder,#xx))
	#define appGET_LISTSTORE(xx) app->xx=GTK_LIST_STORE(gtk_builder_get_object(builder,#xx))
	#define appGET_PIXBUF(xx) 	app->xx=GDK_PIXBUF(gtk_builder_get_object(builder,#xx))
	#define appGET_MENU(xx) 	app->xx=GTK_MENU(gtk_builder_get_object(builder,#xx))
	#define appGET_CELLRENDERER(xx) app->xx=GTK_CELL_RENDERER(gtk_builder_get_object(builder,#xx))
	#define appGET_LABEL(xx) app->xx=GTK_LABEL(gtk_builder_get_object(builder,#xx))
	#define appGET_WINDOW(xx) app->xx=GTK_WINDOW(gtk_builder_get_object(builder,#xx))
	#define appGET_MENUITEM(xx) app->xx=GTK_MENU_ITEM(gtk_builder_get_object(builder,#xx))
	#define appGET_TEXTBUFFER(xx) app->xx=GTK_TEXT_BUFFER(gtk_builder_get_object(builder,#xx))

	#define appGET_COMBOBOX(xx) app->xx=GTK_COMBO_BOX(gtk_builder_get_object(builder,#xx))
	#define appGET_TOGGLE(xx) app->xx=GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,#xx))

    #define appGET_COMBOBOXTEXT(xx) app->xx=GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,#xx))


//WIDGETS
// now load all of our objects into the local app object

// windows
	appGET(main_menu);
	appGET(lockconfig_window);
	appGET(load_window);
	appGET(maint_window);
	appGET(utd_maint_window);
	appGET(mei_maint_window);
	appGET(login_window);
	appGET(splash_window);
	appGET(status_window);
	appGET(admin_window);
	appGET(user_window);

	// user window
	appGET(user_close_btn);


	// admin
	appGET(user_btn);
    appGET(admin_close_btn);



	// status
	appGET_LABEL(status_lbl);
	appGET(status_ok_btn);

	// splash window
    appGET(login_exit_btn);
	appGET(verify_note_btn);

	// login window
	appGET(en_btn);
	appGET(es_btn);
	appGET(fr_btn);
	appGET(he_btn);
	appGET(login_btn);
	appGET(login_back_btn);
	appGET_LABEL(user_txt);
	appGET_LABEL(pw_txt);
	appGET(user_entry);
	appGET(pw_entry);

	appGET(one_btn);
   appGET(two_btn);
   appGET(three_btn);
   appGET(four_btn);
   appGET(five_btn);
   appGET(six_btn);
   appGET(seven_btn);
   appGET(eight_btn);
   appGET(nine_btn);
   appGET(zero_btn);
   appGET(tab_btn);
   appGET(bksp_btn);


// main window
	appGET(lock_config_btn);
	appGET(maint_btn);
	appGET(load_btn);
	appGET(unload_btn);
	appGET(lock_btn);
	appGET(logout_btn);
	appGET(admin_btn);

// load window
//	appGET(load_button);
//	appGET(stop_load_button);
//	appGET(close_load_button);
//	appGET(unload_all_button);

	appGET_LABEL(inv0);
    appGET_LABEL(inv1);
    appGET_LABEL(inv2);
    appGET_LABEL(inv3);
    appGET_LABEL(inv4);
    appGET_LABEL(inv5);
    appGET_LABEL(inv6);
    appGET_LABEL(inv7);


	appGET_TOGGLE(dayssame);

	// cpi adjustment window
//	appGET_LABEL(scomp_title_label);
//	appGET_TEXTBUFFER(global_home_offset_textbuffer);
//	appGET_CELLRENDERER(help_contents_cellrenderer);
//	appGET_PIXBUF(img);
//	appGET_TABLE(airstand_io_table);
//	appGET_LISTSTORE(actuator_liststore);
//	appGET_MENUITEM(file_menuitem);

	// lock confi screen
	// comboboxtext
	appGET_COMBOBOX(lock_hr);
	appGET_COMBOBOX(lock_min);
    appGET_COMBOBOX(unlock_hr);
    appGET_COMBOBOX(unlock_min);

	appGET_COMBOBOXTEXT(LockSelector);
	appGET_COMBOBOX(DaySelector);

	appGET_LABEL(sun_lock_lbl);
    appGET_LABEL(sun_unlock_lbl);
    appGET_LABEL(mon_lock_lbl);
    appGET_LABEL(mon_unlock_lbl);
    appGET_LABEL(tue_lock_lbl);
    appGET_LABEL(tue_unlock_lbl);
    appGET_LABEL(wed_lock_lbl);
    appGET_LABEL(wed_unlock_lbl);
    appGET_LABEL(thu_lock_lbl);
    appGET_LABEL(thu_unlock_lbl);
    appGET_LABEL(fri_lock_lbl);
    appGET_LABEL(fri_unlock_lbl);
    appGET_LABEL(sat_lock_lbl);
    appGET_LABEL(sat_unlock_lbl);


	// maint window
	appGET(mei_button);
	appGET(utd_button);
	appGET(maint_lock_button);
	appGET(maint_close_btn);
	appGET_LABEL(utd_maint_title);
	appGET_LABEL(maint_title);

	// utd maint window
	appGET(zero_utd_btn);
	appGET(utd_close_btn);
	appGET(utd_set_addr_btn);
	appGET(utd_get_addr_btn);
	appGET(utd_get_inventory_btn);
	appGET(utd_unloadall_btn);
	appGET(utd_reset_btn);

	// mei maint window
	appGET(mei_reset_btn);
	appGET(mei_close_btn);
	appGET(mei_maint_title);
	// buttons
/*
	appGET(one_btn);
	appGET(two_btn);
    appGET(three_btn);
    appGET(four_btn);
    appGET(five_btn);
    appGET(six_btn);
    appGET(seven_btn);
    appGET(eight_btn);
    appGET(nine_btn);
    appGET(zero_btn);
*/
    appGET(enter_btn);
    appGET(clear_btn);
    appGET(escape_btn);
    appGET(help_btn);

/*
    appGET(load_btn1);
    appGET(drop_btn);
    appGET(doors_btn);
    appGET(display_btn);
    appGET(report_btn);
    appGET(vend_btn);
    appGET(unload_btn1);
    appGET(buy_change_btn);
    appGET(pgrm_btn);
    appGET(drop_set_btn);
    appGET(insta1_btn);
    appGET(insta2_btn);
    appGET(insta3_btn);
    appGET(insta4_btn);

	appGET(left1_btn);
    appGET(left2_btn);
    appGET(left3_btn);
    appGET(left4_btn);

    appGET(up_right_btn);
    appGET(dn_left_btn);
*/

// labels on lockconfig_window
	appGET(lockconfig_close_btn);
	appGET_LABEL(config_timelock);
	appGET_LABEL(config_day_word);
	appGET_LABEL(config_lock_word);
	appGET_LABEL(config_unlock_word);
	appGET_LABEL(config_lock_word_2);
	appGET_LABEL(config_unlock_word_2);



	appGET_TEXTBUFFER(textbuffer1);


    appGET_LABEL(config_active_lbl);
    appGET_TOGGLE(sun_active);
	appGET_TOGGLE(mon_active);
    appGET_TOGGLE(tue_active);
    appGET_TOGGLE(wed_active);
    appGET_TOGGLE(thu_active);
    appGET_TOGGLE(fri_active);
    appGET_TOGGLE(sat_active);










