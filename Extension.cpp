/* Extension.cpp
 * This file contains the definitions for
 * your extension's general runtime functions,
 * such as the constructor and destructor,
 * handling routines, etc.
 * Functions defined here:
 * Extension::Extension <constructor>
 * Extension::~Extension <destructor>
 * Extension::Handle
 * Extension::Display
 * Extension::Pause
 * Extension::Continue
 * Extension::Save
 * Extension::Load
 * Extension::Action		<--|
 * Extension::Condition		<--|- not what you think!
 * Extension::Expression	<--|
 */

#include "Common.h"

/* <constructor>
 * This is your extension's constructor, which
 * is the replacement for the old CreateRunObject
 * function. You don't need to manually call
 * constructors or pointlessly initialize
 * pointers with dynamic memory. Just link
 * your A/C/Es, perform initialization steps, and
 * you're good to go.
 */
Extension::Extension(RD *rd, SerializedED *SED, createObjectInfo *COB) : rd(rd), rh(rd->rHo.hoAdRunHeader), Runtime(rd)
{
	//Link all your action/condition/expression functions
	//to their IDs to match the IDs in the JSON here.
	LinkAction(0, LoadJSON);
	LinkAction(1, EnterObject);
	LinkAction(2, EnterArray);
	LinkAction(3, GoUp);
	LinkAction(4, GotoRoot);
	LinkAction(5, DebugWindow);
	LinkAction(6, SetBookmark);
	LinkAction(7, GotoBookmark);

	LinkCondition(0, OnError);
	LinkCondition(1, IsString);
	LinkCondition(2, IsInteger);
	LinkCondition(3, IsDouble);
	LinkCondition(4, IsObject);
	LinkCondition(5, IsArray);
	LinkCondition(6, IsBoolean);
	LinkCondition(7, IsNull);
	LinkCondition(8, IsTrue);
	LinkCondition(9, ObjExists);
	LinkCondition(10,IsObjString);
	LinkCondition(11,IsObjInteger);
	LinkCondition(12,IsObjDouble);
	LinkCondition(13,IsObjObject);
	LinkCondition(14,IsObjArray);
	LinkCondition(15,IsObjBoolean);
	LinkCondition(16,IsObjNull);
	LinkCondition(17,IsObjTrue);
	LinkCondition(18,IsArrString);
	LinkCondition(19,IsArrInteger);
	LinkCondition(20,IsArrDouble);
	LinkCondition(21,IsArrObject);
	LinkCondition(22,IsArrArray);
	LinkCondition(23,IsArrBoolean);
	LinkCondition(24,IsArrNull);
	LinkCondition(25,IsArrTrue);

	LinkExpression(0, GetError);
	LinkExpression(1, GetString);
	LinkExpression(2, GetInteger);
	LinkExpression(3, GetLong);
	LinkExpression(4, GetFloat);
	LinkExpression(5, GetDouble);
	LinkExpression(6, GetNumValues);
	LinkExpression(7, GetBoolNum);
	LinkExpression(8, GetObjString);
	LinkExpression(9, GetObjInteger);
	LinkExpression(10,GetObjLong);
	LinkExpression(11,GetObjFloat);
	LinkExpression(12,GetObjDouble);
	LinkExpression(13,GetObjNumValues);
	LinkExpression(14,GetObjBoolNum);
	LinkExpression(15,GetArrString);
	LinkExpression(16,GetArrInteger);
	LinkExpression(17,GetArrLong);
	LinkExpression(18,GetArrFloat);
	LinkExpression(19,GetArrDouble);
	LinkExpression(20,GetArrNumValues);
	LinkExpression(21,GetArrBoolNum);

	current = root = json_parse("null", 4);
}

/* <destructor>
 * This is your extension's destructor, the
 * replacement for DestroyRunObject. No calling
 * destructors manually or deallocating pointless
 * dynamic memory - in most cases this function
 * won't need any code written.
 */
Extension::~Extension()
{
	current = 0;
	if(root)
	{
		json_value_free(root);
	}
}

/* Handle
 * MMF2 calls this function to let your extension
 * "live" - if you want, you can have MMF2 call this
 * every frame. This is where you'd, for instance,
 * simulate physics or move an object. This is
 * the analagous function to the old HandleRunObject.
 */
short Extension::Handle()
{
	/*
	   If your extension will draw to the MMF window you should first 
	   check if anything about its display has changed :

		   if (rd->roc.rcChanged) return REFLAG_DISPLAY;
		   else return 0;

	   You will also need to make sure you change this flag yourself 
	   to 1 whenever you want to redraw your object
	 
	   If your extension won't draw to the window, but it still needs 
	   to do something every MMF2 loop use:

			return 0;

	   If you don't need to do something every loop, use :

			return REFLAG_ONESHOT;

	   This doesn't mean this function can never run again. If you want MMF2
	   to handle your object again (causing this code to run) use this function:

			Runtime.Rehandle();

	   At the end of the event loop this code will run.

	*/

	//Will not be called next loop	
	return REFLAG_ONESHOT;
}

/* Display
 * This is the analagous function to
 * DisplayRunObject. If you return
 * REFLAG_DISPLAY in Handle() this
 * routine will run. If you want MMF2
 * to apply ink effects for you, then
 * implement GetRunObjectSurface in
 * Runtime.cpp instead.
 */
short Extension::Display()
{
	return 0;
}

/* Pause
 * If your extension plays sound, for
 * example, then MMF2 calls this to
 * let you know to pause the music,
 * usually by another extension's request
 * or by the player pausing the applcation.
 */
short Extension::Pause()
{
	return 0;
}

/* Continue
 * Opposite to the above, MMF2 lets
 * you know that the silence is over;
 * your extension may live again.
 */
short Extension::Continue()
{
	return 0;
}

/* Save
 * When the user uses the Save
 * Frame Position action, you need
 * so serialize your runtime data to
 * the File given. It is a Windows
 * file handle, but you can use some
 * of MMF2's built-in functions for
 * writing files. Check the MMF2SDK
 * Help file for more information.
 */
bool Extension::Save(HANDLE File)
{
	return true;
}

/* Load
 * As opposed to above, here you need to
 * restore your extension's runtime state
 * from the given file. Only read what you
 * wrote!
 */
bool Extension::Load(HANDLE File)
{
	return true;
}


/* Action, Condition, Expression
 * These are called if there's no function linked
 * to an ID. You may want to put MessageBox calls
 * to let you know that the ID is unlinked, or you
 * may just want to use unlinked A/C/Es as a feature.
 */
#include <sstream>
void Extension::Action(int ID, RD *rd, long param1, long param2)
{
#ifdef UNICODE
	std::wostringstream oss;
#else
	std::ostringstream oss;
#endif
	oss << _T("Unlinked Action, ID = ") << ID << std::endl;
	MessageBox(NULL, oss.str().c_str(), _T("JSON Object Debug"), MB_OK);
}

long Extension::Condition(int ID, RD *rd, long param1, long param2)
{
#ifdef UNICODE
	std::wostringstream oss;
#else
	std::ostringstream oss;
#endif
	oss << _T("Unlinked Condition, ID = ") << ID << std::endl;
	MessageBox(NULL, oss.str().c_str(), _T("JSON Object Debug"), MB_OK);
	return false; //hopefully StringComparison (PARAM_CMPSTRING) is not used, or this may crash
}

long Extension::Expression(int ID, RD *rd, long param)
{
#ifdef UNICODE
	std::wostringstream oss;
#else
	std::ostringstream oss;
#endif
	oss << _T("Unlinked Expression, ID = ") << ID << std::endl;
	MessageBox(NULL, oss.str().c_str(), _T("JSON Object Debug"), MB_OK);
	return long(_T("")); //so that unlinked expressions that return strings won't crash
}
