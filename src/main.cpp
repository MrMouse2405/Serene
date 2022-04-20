/*

	Serene

	A Light weight blazing fast framework built on top of pros
	that embed Lua programming language.

	For Users:
	Visit Website https://mrmouse2405.gitbook.io/serene/

	For Contributors, Code Reviewers, or Developers:

		Files to Ignore:
			These are Lua 5.4 files, little to no changes have been
			made in them, nor will be made. If any file names under
			src/Serene/ start with 'l' or 'L', ignore them, changing them
			will modify behaviour of Lua Programming Language, not 
			Serene. Any pull requests to these files, will be declined
			unless there is an important reason.

		Serene Files:
			These are files that are responsible for running Serene.

			Serene/Serene.h
				- This file initializes serene, runs Lua 5.4,
				  the serene event loop, and calls lua code that is 
				  supposed to run in each phase. For example during
				  init phase, autonomous phase, operator control, etc

			Serene/SereneInternals:
				- This folder contains all files essential for running
				  important serene internal processes. These are abstracted
				  from lua interface. Reason it's under "Internals"
				- If any updates or changes are to be made to modify
				  Serene, this and Serene.h is where you should look.
				- Notable mentions:
					- Event Loop

			Serene/SereneXPros:
				- This folder contains all files that allow lua to interact
				  with underlying pros api through an interface provided by
				  Serene.
				- If any updates or changes are to be made to the vex library
				  provided by serene, or other serene apis, that are accessed
				  by Lua, this where you should look.
				- Notable mentions:
				    - vex.cpp -> the vex library
					- other files -> all components created using vex.init()

		Lua files that are changed.
			For embedding Lua 5.4 onto Vex, these are the files
			that are changed either to communicate with underlying
			OS, or to add more functionalities.

			linit.c
			- File responsible for initializing all global lua libraries.
			- We have added vex lib (included from SereneXPros/vex.hpp) to 
			  the global lib registery in here to allow users to access 
			  vex library globally.
			- Some files are

			lbaselib.c
			- File responsible for initializing all global functions, such as
			  print(), tonumber(), tostring(), etc.
			- We have changed some of the functions in this file, to make it
			  easier to embed lua, and improve user experience.

		Other Lua Changes:
		    - Default Lua Integer is long long, but in serene it is int.
			  - This is because, Vex V5 runs ARM CORTEX which is a 32 bit processor,
			    a integer, whose size is 32 bit, is natural to the processor, and is therefore
				faster.

		Notes for people who are not familiar with Lua C API:
		  - lua_State, is the lua interperter instance.
		  - lua.hpp includes:
		     - lua.h -> includes basic functions, and macros for communicating with lua
			 - lauxlib.h -> Lua Auxilary Lib
		  - You can find plenty of resources online, as Lua is a quite well know language.
		  - We have also tried our best to document our code, you should be able to find it easy.

*/

#include "main.h"
#include "./Serene/Serene.hpp"


/*

	All the functions defined here

	Are called from Vex to run the robot


*/

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	serene_initialize();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
	serene_disabled();
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
	serene_competition_initialize();
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	serene_autonomous();
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	serene_opcontrol();
	// pros::Controller master(pros::E_CONTROLLER_MASTER);
	// pros::Motor left_mtr(1);
	// pros::Motor right_mtr(2);

	// while (true) {
	// 	pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
	// 	                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
	// 	                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
	// 	int left = master.get_analog(ANALOG_LEFT_Y);
	// 	int right = master.get_analog(ANALOG_RIGHT_Y);

	// 	left_mtr = left;
	// 	right_mtr = right;
	// 	pros::delay(20);
	// }
}
