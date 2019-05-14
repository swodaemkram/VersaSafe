/*
	Module: fire.h
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 11-01-2018
    Revised:
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2018 FireKing Security
    Version: 1.0


	Production Suite  topology:

	Most all of the modules are written in a linear coding fashion, however, a number of OOP
	technologies are also utilized for things such as controllers, actuators and screen widgetry.
	We use the GTK/GDK gui libraries for onscreen content and development. The screens are laid out
	using GLADE to build what ultimately becomes the XML file describing all widgets. We link
	to the GTK/GDK C libraries in spite of the fact that most programmers would turn their noses up
	at this. The fact is that the C libraries make much more efficient use of resources and
	the resulting run time footprint is much smaller (and faster) than creating everything as objects and using
	the GTK/GDK C++ libraries. We use objects where program dictates demand, nowhere else.

	Truthfully, (to those who are not knowledgeable in the foundations of software), FlowForm is very much an OOP
	application, in that, all of the data is stored in dymanic structures and in C++, a class and
	a structure are one and the same. The difference being that structures have been around since
	the White Book and those of us who have been using them forever, have been doing OOP long before
	it became THE thing to do. Also, newer OOP programming uses a set of new operators that perform
	functions on structures/classes, however, I'd sooner not use these and use older more reliable
	methods because there are no hidden functions when writing in vanilla C code. I have found very little
	difference in development time and RAD is the primary argument for using the newer OOP functionality.
	My structure approach vs current generation C++ OOP, realizes improved portability and stability.
	Thus the improvements, portability, stability, footprint, CPU cognizance are resultant of
	the programmer taking on more of the PROGRAMMING job and leaving none to IDE's and libraries
	that are often huge and cumbersome. The proof of the pudding is in the <300K disk footprint 
	FlowForm OSL requires and its lightning execution speed.

	Additionally, such things as string searches, strstr() is MUCH faster than string.find. Likewise,
	many other discretes are faster also. This is because specifically purposed code is always smaller
	and faster than general purpose code.


*/

#ifndef FIRE_INCLUDE

	#define FIRE_INCLUDE
	#define product_name "VSafe"
	#define OK 0
	#define ERROR 1
	#define PENDING 2
	#define VAREMPTY -1
	#define MAJOR_VERSION 1
	#define MINOR_VERSION 0
	#define MICRO_VERSION 0
	#define MAX_LOCKS  8

	// the following are used with the compile time var PGM_FLAVOR, set in the Makefiles
	#define PRODUCTION_PGM 100
	#define FLOWFORM_PGM 200
	#define HEADER_PGM 300
	#define RECIPE_PGM 400

	#ifndef TRUE
		#define TRUE 1
	#endif

	#ifndef FALSE
		#define FALSE 0
	#endif

	#define OFF 0
	#define ON 1




typedef enum
{
	Manual_mode,
	Load_mode,
	Unload_mode,
	CMC_mode,
	Prod_mode

} OperationMode;


// for CountTotalActuators() in actuators.cpp
enum
{
	ENABLED_ACTUATORS=0,
	TOTAL_ACTUATORS,
	PRESENT_ACTUATORS
};


// used by pid.cpp file
enum
{
	VERSASAFE_PID=0,
};




// endif FIRE_INCLUDE
#endif



