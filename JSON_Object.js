// Definition of the conditions, actions and expressions codes.
// ---------------------------------------------------------------
// This list must be identical to the list defined in the C version
// of your extension.

CRunJSON_Object.ACT_LoadJSON = 0;
CRunJSON_Object.ACT_EnterObject = 1;
CRunJSON_Object.ACT_EnterArray = 2;
CRunJSON_Object.ACT_GoUp = 3;
CRunJSON_Object.ACT_GotoRoot = 4;
CRunJSON_Object.ACT_DebugWindow = 5;
CRunJSON_Object.ACT_SetBookmark = 6;
CRunJSON_Object.ACT_GotoBookmark = 7;

CRunJSON_Object.CND_OnError = 0;
CRunJSON_Object.CND_IsString = 1;
CRunJSON_Object.CND_IsInteger = 2;
CRunJSON_Object.CND_IsDouble = 3;
CRunJSON_Object.CND_IsObject = 4;
CRunJSON_Object.CND_IsArray = 5;
CRunJSON_Object.CND_IsBoolean = 6;
CRunJSON_Object.CND_IsNull = 7;
CRunJSON_Object.CND_IsTrue = 8;
CRunJSON_Object.CND_ObjExists = 9;
CRunJSON_Object.CND_IsObjString = 10;
CRunJSON_Object.CND_IsObjInteger = 11;
CRunJSON_Object.CND_IsObjDouble = 12;
CRunJSON_Object.CND_IsObjObject = 13;
CRunJSON_Object.CND_IsObjArray = 14;
CRunJSON_Object.CND_IsObjBoolean = 15;
CRunJSON_Object.CND_IsObjNull = 16;
CRunJSON_Object.CND_IsObjTrue = 17;
CRunJSON_Object.CND_IsArrString = 18;
CRunJSON_Object.CND_IsArrInteger = 19;
CRunJSON_Object.CND_IsArrDouble = 20;
CRunJSON_Object.CND_IsArrObject = 21;
CRunJSON_Object.CND_IsArrArray = 22;
CRunJSON_Object.CND_IsArrBoolean = 23;
CRunJSON_Object.CND_IsArrNull = 24;
CRunJSON_Object.CND_IsArrTrue = 25;
// Important. CND_LAST must contain the number of conditions.
// Do not forget to update it if you add or remove a condition from the list.
CRunJSON_Object.CND_LAST = 26;

CRunJSON_Object.EXP_GetError = 0;
CRunJSON_Object.EXP_GetString = 1;
CRunJSON_Object.EXP_GetInteger = 2;
CRunJSON_Object.EXP_GetLong = 3;
CRunJSON_Object.EXP_GetFloat = 4;
CRunJSON_Object.EXP_GetDouble = 5;
CRunJSON_Object.EXP_GetNumValues = 6;
CRunJSON_Object.EXP_GetBoolNum = 7;
CRunJSON_Object.EXP_GetObjString = 8;
CRunJSON_Object.EXP_GetObjInteger = 9;
CRunJSON_Object.EXP_GetObjLong = 10;
CRunJSON_Object.EXP_GetObjFloat = 11;
CRunJSON_Object.EXP_GetObjDouble = 12;
CRunJSON_Object.EXP_GetObjNumValues = 13;
CRunJSON_Object.EXP_GetObjBoolNum = 14;
CRunJSON_Object.EXP_GetArrString = 15;
CRunJSON_Object.EXP_GetArrInteger = 16;
CRunJSON_Object.EXP_GetArrLong = 17;
CRunJSON_Object.EXP_GetArrFloat = 18;
CRunJSON_Object.EXP_GetArrDouble = 19;
CRunJSON_Object.EXP_GetArrNumValues = 20;
CRunJSON_Object.EXP_GetArrBoolNum = 21;

// Constructor of the object.
// ----------------------------------------------------------------
// Called during the creation process of the object, but before any
// initialization. You may want(although you can do it in CreateRunObject),
// to instantiate variables.
function CRunJSON_Object()
{
	this.root = null;
	this.current = null;
	this.parents = [];
	this.error = "";
	this.bookmarks = {};
}

// Prototype definition
// -----------------------------------------------------------------
// This class is a sub-class of CRunExtension, by the mean of the
// CServices.extend function which copies all the properties of
// the parent class to the new class when it is created.
// As all the necessary functions are defined in the parent class,
// you only need to keep the ones that you actually need in your code.
CRunJSON_Object.prototype = CServices.extend(new CRunExtension(),
{
	// Returns the number of conditions
	// --------------------------------------------------------------------
	// Warning, if this number is not correct, the application _will_ crash
	getNumberOfConditions:function()
	{
		return CRunJSON_Object.CND_LAST;
	},											  // Don't forget the comma between each function

	// Handling of the object
	// ---------------------------------------------------------------------
	// This function is called at every loop of the game. You have to perform
	// in it all the tasks necessary for your object to function.
	// Return values:
	//	- 0 : this function will be called during the next loop
	//	- CRunExtension.REFLAG_ONESHOT : this function will not be called anymore
	//	  In this case, call the this.reHandle(); function of the base class
	//	  to have it called again.
	handleRunObject: function()
	{
		return CRunExtension.REFLAG_ONESHOT;
	},

	// Destruction of the object
	// ---------------------------------------------------------------
	// Called when the object is actually destroyed. This will always be
	// after the main game loop, and out of the actions processing : the
	// destroy process is queued until the very end of the game loop.
	destroyRunObject: function(bFast)
	{
	},

	// Action entry
	// --------------------------------------------------------------
	// Called when an action of this object is executed
	//   - num : number of the action, as defined in the list on top of this source
	//   - act : a CActExtension object, allowing you to retreive the parameters
	//		   of the action
	action:function(num, act)
	{
		switch(num)
		{
			case CRunJSON_Object.ACT_LoadJSON:
			{
				var json = act.getParamExpString(this.rh, 0);
				var flags = act.getParamExpression(this.rh, 1);
				try
				{
					this.current = this.root = JSON.parse(json);
					this.parents = [];
					this.bookmarks = {};
				}
				catch(e)
				{
					this.error = ""+e;
					this.ho.generateEvent(CRunJSON_Object.CND_OnError, 0);
				}
			} break;
			case CRunJSON_Object.ACT_EnterObject:
			{
				var name = act.getParamExpString(this.rh, 0);
				if((this.current !== null)
				&& (typeof this.current === 'object')
				&& (typeof this.current[name] !== 'undefined'))
				{
					this.parents.push(this.current);
					this.current = this.current[name];
				}
			} break;
			case CRunJSON_Object.ACT_EnterArray:
			{
				var index = act.getParamExpression(this.rh, 0);
				if((this.current !== null)
				&& (Object.prototype.toString.call(this.current) === '[object Array]')
				&& (typeof this.current[index] !== 'undefined'))
				{
					this.parents.push(this.current);
					this.current = this.current[index];
				}
			} break;
			case CRunJSON_Object.ACT_GoUp:
			{
				if(this.parents.length > 0)
				{
					this.current = this.parents.pop();
				}
			} break;
			case CRunJSON_Object.ACT_GotoRoot:
			{
				this.current = this.root;
				this.parents = [];
			} break;
			case CRunJSON_Object.ACT_SetBookmark:
			{
				var name = act.getParamExpString(this.rh, 0);
				this.bookmarks[name] = {v: this.current, p: this.parents.slice()};
			} break;
			case CRunJSON_Object.ACT_GotoBookmark:
			{
				var name = act.getParamExpString(this.rh, 0);
				if(typeof this.bookmarks[name] !== 'undefined')
				{
					this.current = this.bookmarks[name].v;
					this.parents = this.bookmarks[name].p.slice();
				}
			} break;
			case CRunJSON_Object.ACT_DebugWindow:
			{
				alert(JSON.stringify(this.root));
			} break;
		}
	},

	// Condition entry
	// -----------------------------------------------------------------
	// Called when a condition of this object is evaluated
	//	- num : the number of the condition, as defined on top of this source
	//	- cnd : a CCndExtension object, allowing you to retrieve the parameters
	//			of the condition
	// Return value :
	//	true or false
	condition:function(num, cnd)
	{
		switch(num)
		{
			case CRunJSON_Object.CND_OnError:
			{
				return true;
			} break;
			case CRunJSON_Object.CND_IsString:
			{
				return typeof this.current === 'string';
			} break;
			case CRunJSON_Object.CND_IsInteger:
			{
				var n = this.current;
				return n === +n && n === (n|0);
			} break;
			case CRunJSON_Object.CND_IsDouble:
			{
				var n = this.current;
				return n === +n && n !== (n|0);
			} break;
			case CRunJSON_Object.CND_IsObject:
			{
				return typeof this.current === 'object';
			} break;
			case CRunJSON_Object.CND_IsArray:
			{
				return Object.prototype.toString.call(this.current) === '[object Array]';
			} break;
			case CRunJSON_Object.CND_IsBoolean:
			{
				return this.current === true || this.current === false;
			} break;
			case CRunJSON_Object.CND_IsNull:
			{
				return this.current === null;
			} break;
			case CRunJSON_Object.CND_IsTrue:
			{
				return this.current === true || (this.current === +this.current && this.current !== 0) || (typeof this.current === 'string' && this.current !== "false");
			} break;
			case CRunJSON_Object.CND_ObjExists:
			{
				var name = cnd.getParamExpString(this.rh, 0);
				return typeof this.current[name] !== 'undefined';
			} break;
			case CRunJSON_Object.CND_IsObjString:
			{
				var name = cnd.getParamExpString(this.rh, 0);
				return typeof this.current === 'object'
				&&     typeof this.current[name] === 'string';
			} break;
			case CRunJSON_Object.CND_IsObjInteger:
			{
				var name = cnd.getParamExpString(this.rh, 0);
				if(typeof this.current === 'object'
				&& typeof this.current[name] !== 'undefined')
				{
					var n = this.current[name];
					return n === +n && n === (n|0);
				}
			} break;
			case CRunJSON_Object.CND_IsObjDouble:
			{
				var name = cnd.getParamExpString(this.rh, 0);
				if(typeof this.current === 'object'
				&& typeof this.current[name] !== 'undefined')
				{
					var n = this.current[name];
					return n === +n && n !== (n|0);
				}
			} break;
			case CRunJSON_Object.CND_IsObjObject:
			{
				var name = cnd.getParamExpString(this.rh, 0);
				return typeof this.current === 'object'
				&&     typeof this.current[name] === 'object';
			} break;
			case CRunJSON_Object.CND_IsObjArray:
			{
				var name = cnd.getParamExpString(this.rh, 0);
				return typeof this.current === 'object'
				&&     Object.prototype.toString.call(this.current[name]) === '[object Array]';
			} break;
			case CRunJSON_Object.CND_IsObjBoolean:
			{
				var name = cnd.getParamExpString(this.rh, 0);
				return typeof this.current === 'object'
				&&     (this.current[name] === true || this.current[name] === false);
			} break;
			case CRunJSON_Object.CND_IsObjNull:
			{
				var name = cnd.getParamExpString(this.rh, 0);
				return typeof this.current === 'object'
				&&     this.current[name] === null;
			} break;
			case CRunJSON_Object.CND_IsObjTrue:
			{
				var name = cnd.getParamExpString(this.rh, 0);
				return typeof this.current === 'object'
				&&     (this.current[name] === true || (this.current[name] === +this.current[name] && this.current[name] !== 0) || (typeof this.current[name] === 'string' && this.current[name] !== "false"));
			} break;
			case CRunJSON_Object.CND_IsArrString:
			{
				var index = cnd.getParamExpression(this.rh, 0);
				return Object.prototype.toString.call(this.current) === '[object Array]'
				&&     typeof this.current[index] === 'string';
			} break;
			case CRunJSON_Object.CND_IsArrInteger:
			{
				var index = cnd.getParamExpression(this.rh, 0);
				if(Object.prototype.toString.call(this.current) === '[object Array]'
				&& typeof this.current[index] !== 'undefined')
				{
					var n = this.current[index];
					return n === +n && n === (n|0);
				}
			} break;
			case CRunJSON_Object.CND_IsArrDouble:
			{
				var index = cnd.getParamExpression(this.rh, 0);
				if(Object.prototype.toString.call(this.current) === '[object Array]'
				&& typeof this.current[index] !== 'undefined')
				{
					var n = this.current[index];
					return n === +n && n !== (n|0);
				}
			} break;
			case CRunJSON_Object.CND_IsArrObject:
			{
				var index = cnd.getParamExpression(this.rh, 0);
				return Object.prototype.toString.call(this.current) === '[object Array]'
				&&     typeof this.current[index] === 'object';
			} break;
			case CRunJSON_Object.CND_IsArrArray:
			{
				var index = cnd.getParamExpression(this.rh, 0);
				return Object.prototype.toString.call(this.current) === '[object Array]'
				&&     Object.prototype.toString.call(this.current[index]) === '[object Array]';
			} break;
			case CRunJSON_Object.CND_IsArrBoolean:
			{
				var index = cnd.getParamExpression(this.rh, 0);
				return Object.prototype.toString.call(this.current) === '[object Array]'
				&&     (this.current[index] === true || this.current[index] === false);
			} break;
			case CRunJSON_Object.CND_IsArrNull:
			{
				var index = cnd.getParamExpression(this.rh, 0);
				return Object.prototype.toString.call(this.current) === '[object Array]'
				&&     this.current[index] === null;
			} break;
			case CRunJSON_Object.CND_IsArrTrue:
			{
				var index = cnd.getParamExpression(this.rh, 0);
				return Object.prototype.toString.call(this.current) === '[object Array]'
				&&     (this.current[index] === true || (this.current[index] === +this.current[index] && this.current[index] !== 0) || (typeof this.current[index] === 'string' && this.current[index] !== "false"));
			} break;
		}
		return false;
	},

	// Expression entry
	// ------------------------------------------------------------------
	// Called during the evaluation of an expression.
	//	- num : the number of the expression, as defined on top of this source.
	// Note that it is important that your expression function asks for
	// each and every one of the parameters of the function, each time it is
	// called. The runtime will crash if you miss parameters.
	// Return value :
	//	- The result of the calculation, a number or a string
	expression: function(num)
	{
		switch(num)
		{
			case CRunJSON_Object.EXP_GetError:
			{
				return this.error;
			} break;
			case CRunJSON_Object.EXP_GetString:
			{
				return ""+this.current;
			} break;
			case CRunJSON_Object.EXP_GetInteger:
			{
				return (+this.current)|0;
			} break;
			case CRunJSON_Object.EXP_GetLong:
			{
				return ""+((+this.current)|0);
			} break;
			case CRunJSON_Object.EXP_GetFloat:
			{
				return +this.current;
			} break;
			case CRunJSON_Object.EXP_GetDouble:
			{
				return ""+(+this.current);
			} break;
			case CRunJSON_Object.EXP_GetNumValues:
			{
				return Object.prototype.toString.call(this.current) === '[object Array]'? this.current.length : Object.keys(this.current).length;
			} break;
			case CRunJSON_Object.EXP_GetBoolNum:
			{
				return this.condition(CRunJSON_Object.CND_IsTrue, null)? 1 : 0;
			} break;
			case CRunJSON_Object.EXP_GetObjString:
			{
				var name = this.ho.getExpParam();
				return typeof this.current === 'object'? ""+this.current[name] : "";
			} break;
			case CRunJSON_Object.EXP_GetObjInteger:
			{
				var name = this.ho.getExpParam();
				return typeof this.current === 'object'? (+this.current[name])|0 : 0;
			} break;
			case CRunJSON_Object.EXP_GetObjLong:
			{
				var name = this.ho.getExpParam();
				return typeof this.current === 'object'? ""+((+this.current[name])|0) : "0";
			} break;
			case CRunJSON_Object.EXP_GetObjFloat:
			{
				var name = this.ho.getExpParam();
				return typeof this.current === 'object'? +this.current[name] : 0;
			} break;
			case CRunJSON_Object.EXP_GetObjDouble:
			{
				var name = this.ho.getExpParam();
				return typeof this.current === 'object'? ""+(+this.current[name]) : "0";
			} break;
			case CRunJSON_Object.EXP_GetObjNumValues:
			{
				var name = this.ho.getExpParam();
				return typeof this.current === 'object'? (Object.prototype.toString.call(this.current[name]) === '[object Array]'? this.current[name].length : Object.keys(this.current[name]).length) : 0;
			} break;
			case CRunJSON_Object.EXP_GetObjBoolNum:
			{
				var name = this.ho.getExpParam();
				if(typeof this.current === 'object')
				{
					var old = this.current
					this.current = this.current[name];
					var result = this.condition(CRunJSON_Object.CND_IsTrue, null)? 1 : 0;
					this.current = old;
					return result;
				}
				return 0;
			} break;
			case CRunJSON_Object.EXP_GetArrString:
			{
				var index = this.ho.getExpParam();
				return Object.prototype.toString.call(this.current) === '[object Array]'? ""+this.current[index] : "";
			} break;
			case CRunJSON_Object.EXP_GetArrInteger:
			{
				var index = this.ho.getExpParam();
				return Object.prototype.toString.call(this.current) === '[object Array]'? (+this.current[index])|0 : 0;
			} break;
			case CRunJSON_Object.EXP_GetArrLong:
			{
				var index = this.ho.getExpParam();
				return Object.prototype.toString.call(this.current) === '[object Array]'? ""+((+this.current[index])|0) : "0";
			} break;
			case CRunJSON_Object.EXP_GetArrFloat:
			{
				var index = this.ho.getExpParam();
				return Object.prototype.toString.call(this.current) === '[object Array]'? +this.current[index] : 0;
			} break;
			case CRunJSON_Object.EXP_GetArrDouble:
			{
				var index = this.ho.getExpParam();
				return Object.prototype.toString.call(this.current) === '[object Array]'? ""+(+this.current[index]) : "0";
			} break;
			case CRunJSON_Object.EXP_GetArrNumValues:
			{
				var index = this.ho.getExpParam();
				return Object.prototype.toString.call(this.current) === '[object Array]'? (Object.prototype.toString.call(this.current[index]) === '[object Array]'? this.current[index].length : Object.keys(this.current[index]).length) : 0;
			} break;
			case CRunJSON_Object.EXP_GetArrBoolNum:
			{
				var index = this.ho.getExpParam();
				if(Object.prototype.toString.call(this.current) === '[object Array]')
				{
					var old = this.current
					this.current = this.current[index];
					var result = this.condition(CRunJSON_Object.CND_IsTrue, null)? 1 : 0;
					this.current = old;
					return result;
				}
				return 0;
			} break;
		}
		return 0;
	}
});
