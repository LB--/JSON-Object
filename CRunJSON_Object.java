package Extensions;

import android.app.AlertDialog;
import android.content.DialogInterface;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

import Actions.CActExtension;
import Conditions.CCndExtension;
import Expressions.CValue;
import Runtime.MMFRuntime;

public class CRunJSON_Object extends CRunExtension
{
	public static final int CND_LAST = 26+1;
	@Override
	public int getNumberOfConditions()
	{
		return CND_LAST;
	}

	private void Error(Throwable e)
	{
		error = e;
		ho.generateEvent(0, 0);
		error = null;
	}

	private Object root = null;
	private Object current = null;
	private Stack<Object> parents = new Stack<Object>();
	private Throwable error = null;
	private Map<String, Stack<Object>> bookmarks = new HashMap<String, Stack<Object>>();
	private class Loop
	{
		final String name;
		final Object object;
		final Stack<Object> obj_parents;
		String sub_name;
		int index;
		Object sub;
		Stack<Object> sub_parents;

		Loop(String id, Object obj, Stack<Object> parents)
		{
			name = id;
			object = obj;
			obj_parents = parents;
		}
	}
	private Stack<Loop> loops = new Stack<Loop>();

	{
		LoadJSON("null", 0);
	}

	/* Actions **********************************************************/
	public void LoadJSON(String json, int flags_unused)
	{
		try
		{
			current = root = new JSONTokener(json).nextValue();
			parents.clear();
			bookmarks.clear();
			loops.clear();
		}
		catch(JSONException e)
		{
			Error(e);
		}
	}
	public void EnterObject(String name)
	{
		if(IsObject() && ObjExists(name)) try
		{
			Object parent = current;
			current = ((JSONObject)current).get(name);
			parents.push(parent);
		}
		catch(JSONException e)
		{
			Error(e);
		}
	}
	public void EnterArray(int index)
	{
		if(IsArray()) try
		{
			JSONArray arr = (JSONArray)current;
			if(index < arr.length())
			{
				Object parent = current;
				current = ((JSONArray)current).get(index);
				parents.push(parent);
			}
		}
		catch(JSONException e)
		{
			Error(e);
		}
	}
	public void GoUp()
	{
		if(!parents.empty())
		{
			current = parents.pop();
		}
	}
	public void GotoRoot()
	{
		current = root;
		parents.clear();
	}
	public void SetBookmark(String name)
	{
		Stack<Object> trace = (Stack<Object>)parents.clone();
		trace.push(current);
		bookmarks.put(name, trace);
	}
	public void GotoBookmark(String name)
	{
		if(bookmarks.containsKey(name))
		{
			parents = (Stack<Object>)bookmarks.get(name).clone();
			current = parents.pop();
		}
	}
	void LoopObjects(String loopname)
	{
		Object looperobj = current;
		Stack<Object> looperparents = (Stack<Object>)parents.clone();
		loops.push(new Loop(loopname, looperobj, looperparents));
		try
		{
			if(IsObject())
			{
				final JSONObject looper = (JSONObject)looperobj;
				final JSONArray names = looper.names();
				final JSONArray values = looper.toJSONArray(names);
				int length = looper.length();
				for(int i = 0; i < length; ++i)
				{
					if(loops.size() == 0 || loops.peek().object != looper)
					{
						break;
					}
					loops.peek().index = i;
					loops.peek().sub_name = names.getString(i);
					loops.peek().sub = values.get(i);
					loops.peek().sub_parents = (Stack<Object>)looperparents.clone();
					loops.peek().sub_parents.push(looper);
					ho.generateEvent(26, 0);
				}
			}
			else if(IsArray())
			{
				final JSONArray looper = (JSONArray)looperobj;
				int length = looper.length();
				for(int i = 0; i < length; ++i)
				{
					if(loops.size() == 0 || loops.peek().object != looper)
					{
						break;
					}
					loops.peek().index = i;
					loops.peek().sub_name = null;
					loops.peek().sub = looper.get(i);
					loops.peek().sub_parents = (Stack<Object>)looperparents.clone();
					loops.peek().sub_parents.push(looper);
					ho.generateEvent(26, 0);
				}
			}
		}
		catch(JSONException e)
		{
			Error(e);
		}
		finally
		{
			if(loops.size() > 0 && loops.peek().object == looperobj)
			{
				loops.pop();
			}
			current = looperobj;
			parents = looperparents;
		}
	}
	void DebugWindow()
	{
		new AlertDialog.Builder(MMFRuntime.inst)
			.setTitle("JSON Object Debug Window")
			.setMessage(""+current)
			.setCancelable(false)
			.setPositiveButton("OK", new DialogInterface.OnClickListener()
			{
				@Override
				public void onClick(DialogInterface dialogInterface, int i)
				{
				}
			})
			.create().show();
	}

	/* Conditions *******************************************************/
	public boolean OnError()
	{
		return true;
	}
	public boolean OnLoop(String loopname)
	{
		if(loops.size() > 0 && loops.peek().name.equals(loopname))
		{
			current = loops.peek().sub;
			parents = loops.peek().sub_parents;
			return true;
		}
		return false;
	}
	public boolean IsString()
	{
		return current instanceof String;
	}
	public boolean IsInteger()
	{
		return current instanceof Integer || current instanceof Long;
	}
	public boolean IsDouble()
	{
		return current instanceof Double;
	}
	public boolean IsObject()
	{
		return current instanceof JSONObject;
	}
	public boolean IsArray()
	{
		return current instanceof JSONArray;
	}
	public boolean IsBoolean()
	{
		return current instanceof Boolean;
	}
	public boolean IsNull()
	{
		return current == JSONObject.NULL;
	}
	public boolean IsTrue()
	{
		if(IsBoolean())
		{
			return (Boolean)current;
		}
		else if(current instanceof Integer)
		{
			return (Integer)current != 0;
		}
		else if(current instanceof Long)
		{
			return (Long)current != 0;
		}
		else if(IsString())
		{
			return ((String)current).equalsIgnoreCase("true");
		}
		return false;
	}
	public boolean ObjExists(String name)
	{
		if(IsObject())
		{
			return ((JSONObject)current).has(name);
		}
		return false;
	}
	public boolean IsObjString(String name)
	{
		try
		{
			((JSONObject)current).getString(name);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsObjInteger(String name)
	{
		try
		{
			((JSONObject)current).getLong(name);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsObjDouble(String name)
	{
		try
		{
			((JSONObject)current).getDouble(name);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsObjObject(String name)
	{
		try
		{
			((JSONObject)current).getJSONObject(name);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsObjArray(String name)
	{
		try
		{
			((JSONObject)current).getJSONArray(name);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsObjBoolean(String name)
	{
		try
		{
			((JSONObject)current).getBoolean(name);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsObjNull(String name)
	{
		try
		{
			return ((JSONObject)current).get(name) == JSONObject.NULL;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsObjTrue(String name)
	{
		try
		{
			return ((JSONObject)current).getBoolean(name);
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsArrString(int index)
	{
		try
		{
			((JSONArray)current).getString(index);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsArrInteger(int index)
	{
		try
		{
			((JSONArray)current).getLong(index);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsArrDouble(int index)
	{
		try
		{
			((JSONArray)current).getDouble(index);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsArrObject(int index)
	{
		try
		{
			((JSONArray)current).getJSONObject(index);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsArrArray(int index)
	{
		try
		{
			((JSONArray)current).getJSONArray(index);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsArrBoolean(int index)
	{
		try
		{
			((JSONArray)current).getBoolean(index);
			return true;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsArrNull(int index)
	{
		try
		{
			return ((JSONArray)current).get(index) == JSONObject.NULL;
		}
		catch(Throwable t)
		{
		}
		return false;
	}
	public boolean IsArrTrue(int index)
	{
		try
		{
			return ((JSONArray)current).getBoolean(index);
		}
		catch(Throwable t)
		{
		}
		return false;
	}

	/* Expressions ******************************************************/
	public String GetError()
	{
		return error.toString();
	}
	public String GetIteratedName()
	{
		if(loops.size() > 0 && loops.peek().sub_name != null)
		{
			return loops.peek().sub_name;
		}
		return "";
	}
	public int GetIteratedIndex()
	{
		if(loops.size() > 0)
		{
			return loops.peek().index;
		}
		return 0;
	}
	public String GetString()
	{
		if(IsString() || IsInteger() || IsDouble() || IsBoolean() || IsNull())
		{
			return current.toString();
		}
		return "";
	}
	public int GetInteger()
	{
		if(current instanceof Integer)
		{
			return (int)(Integer)current;
		}
		else if(current instanceof Long)
		{
			return (int)(long)(Long)current;
		}
		else if(IsDouble())
		{
			return (int)(double)(Double)current;
		}
		else if(IsString()) try
		{
			return (int)Long.parseLong(current.toString());
		}
		catch(Throwable t)
		{
		}
		return 0;
	}
	public String GetLong()
	{
		if(IsInteger())
		{
			return current.toString();
		}
		else if(IsDouble())
		{
			return "" + (long)(double)(Double)current;
		}
		else if(IsString()) try
		{
			return "" + Long.parseLong(current.toString());
		}
		catch(Throwable t)
		{
		}
		return "0";
	}
	float GetFloat()
	{
		if(IsDouble())
		{
			return (float)(double)(Double)current;
		}
		else if(current instanceof Integer)
		{
			return (float)(int)(Integer)current;
		}
		else if(current instanceof Long)
		{
			return (float)(long)(Long)current;
		}
		else if(IsString()) try
		{
			return (float)Double.parseDouble(current.toString());
		}
		catch(Throwable t)
		{
		}
		return 0.f;
	}
	public String GetDouble()
	{
		if(IsDouble())
		{
			return current.toString();
		}
		else if(current instanceof Integer)
		{
			return "" + (double)(int)(Integer)current;
		}
		else if(current instanceof Long)
		{
			return "" + (double)(long)(Long)current;
		}
		else if(IsString()) try
		{
			return "" + Double.parseDouble(current.toString());
		}
		catch(Throwable t)
		{
		}
		return "0.0";
	}
	public int GetNumValues()
	{
		if(IsObject())
		{
			return ((JSONObject)current).length();
		}
		else if(IsArray())
		{
			return ((JSONArray)current).length();
		}
		return 0;
	}
	public int GetBoolNum()
	{
		if(IsBoolean())
		{
			return (Boolean)current? 1 : 0;
		}
		else if(current instanceof Integer)
		{
			return ((int)(Integer)current != 0)? 1 : 0;
		}
		else if(current instanceof Long)
		{
			return ((long)(Long)current != 0)? 1 : 0;
		}
		else if(IsString())
		{
			return (current.toString().equalsIgnoreCase("true"))? 1 : 0;
		}
		return 0;
	}
	public String GetObjString(String name)
	{
		if(IsObject() && ObjExists(name)) try
		{
			EnterObject(name);
			return GetString();
		}
		finally
		{
			GoUp();
		}
		return "";
	}
	public int GetObjInteger(String name)
	{
		if(IsObject() && ObjExists(name)) try
		{
			EnterObject(name);
			return GetInteger();
		}
		finally
		{
			GoUp();
		}
		return 0;
	}
	public String GetObjLong(String name)
	{
		if(IsObject() && ObjExists(name)) try
		{
			EnterObject(name);
			return GetLong();
		}
		finally
		{
			GoUp();
		}
		return "0";
	}
	public float GetObjFloat(String name)
	{
		if(IsObject() && ObjExists(name)) try
		{
			EnterObject(name);
			return GetFloat();
		}
		finally
		{
			GoUp();
		}
		return 0.f;
	}
	public String GetObjDouble(String name)
	{
		if(IsObject() && ObjExists(name)) try
		{
			EnterObject(name);
			return GetDouble();
		}
		finally
		{
			GoUp();
		}
		return "0.0";
	}
	public int GetObjNumValues(String name)
	{
		if(IsObject() && ObjExists(name)) try
		{
			EnterObject(name);
			return GetNumValues();
		}
		finally
		{
			GoUp();
		}
		return 0;
	}
	public int GetObjBoolNum(String name)
	{
		if(IsObject() && ObjExists(name)) try
		{
			EnterObject(name);
			return GetBoolNum();
		}
		finally
		{
			GoUp();
		}
		return 0;
	}
	public String GetArrString(int index)
	{
		if(IsArray() && index < GetNumValues()) try
		{
			EnterArray(index);
			return GetString();
		}
		finally
		{
			GoUp();
		}
		return "";
	}
	public int GetArrInteger(int index)
	{
		if(IsArray() && index < GetNumValues()) try
		{
			EnterArray(index);
			return GetInteger();
		}
		finally
		{
			GoUp();
		}
		return 0;
	}
	public String GetArrLong(int index)
	{
		if(IsArray() && index < GetNumValues()) try
		{
			EnterArray(index);
			return GetLong();
		}
		finally
		{
			GoUp();
		}
		return "0";
	}
	public float GetArrFloat(int index)
	{
		if(IsArray() && index < GetNumValues()) try
		{
			EnterArray(index);
			return GetFloat();
		}
		finally
		{
			GoUp();
		}
		return 0.f;
	}
	public String GetArrDouble(int index)
	{
		if(IsArray() && index < GetNumValues()) try
		{
			EnterArray(index);
			return GetDouble();
		}
		finally
		{
			GoUp();
		}
		return "0.0";
	}
	public int GetArrNumValues(int index)
	{
		if(IsArray() && index < GetNumValues()) try
		{
			EnterArray(index);
			return GetNumValues();
		}
		finally
		{
			GoUp();
		}
		return 0;
	}
	public int GetArrBoolNum(int index)
	{
		if(IsArray() && index < GetNumValues()) try
		{
			EnterArray(index);
			return GetBoolNum();
		}
		finally
		{
			GoUp();
		}
		return 0;
	}

	/* ACEs *************************************************************/
	@Override
	public void action(int num, CActExtension act)
	{
		switch(num)
		{
			default: break;
			case 0: LoadJSON(act.getParamExpString(rh, 0), act.getParamExpression(rh, 1)); break;
			case 1: EnterObject(act.getParamExpString(rh, 0)); break;
			case 2: EnterArray(act.getParamExpression(rh, 0)); break;
			case 3: GoUp(); break;
			case 4: GotoRoot(); break;
			case 5: DebugWindow(); break;
			case 6: SetBookmark(act.getParamExpString(rh, 0)); break;
			case 7: GotoBookmark(act.getParamExpString(rh, 0)); break;
			case 8: LoopObjects(act.getParamExpString(rh, 0)); break;
		}
	}
	@Override
	public boolean condition(int num, CCndExtension cnd)
	{
		switch(num)
		{
			default: break;
			case 0: return OnError();
			case 1: return IsString();
			case 2: return IsInteger();
			case 3: return IsDouble();
			case 4: return IsObject();
			case 5: return IsArray();
			case 6: return IsBoolean();
			case 7: return IsNull();
			case 8: return IsTrue();
			case 9: return ObjExists(cnd.getParamExpString(rh, 0));
			case 10: return IsObjString(cnd.getParamExpString(rh, 0));
			case 11: return IsObjInteger(cnd.getParamExpString(rh, 0));
			case 12: return IsObjDouble(cnd.getParamExpString(rh, 0));
			case 13: return IsObjObject(cnd.getParamExpString(rh, 0));
			case 14: return IsObjArray(cnd.getParamExpString(rh, 0));
			case 15: return IsObjBoolean(cnd.getParamExpString(rh, 0));
			case 16: return IsObjNull(cnd.getParamExpString(rh, 0));
			case 17: return IsObjTrue(cnd.getParamExpString(rh, 0));
			case 18: return IsArrString(cnd.getParamExpression(rh, 0));
			case 19: return IsArrInteger(cnd.getParamExpression(rh, 0));
			case 20: return IsArrDouble(cnd.getParamExpression(rh, 0));
			case 21: return IsArrObject(cnd.getParamExpression(rh, 0));
			case 22: return IsArrArray(cnd.getParamExpression(rh, 0));
			case 23: return IsArrBoolean(cnd.getParamExpression(rh, 0));
			case 24: return IsArrNull(cnd.getParamExpression(rh, 0));
			case 25: return IsArrTrue(cnd.getParamExpression(rh, 0));
			case 26: return OnLoop(cnd.getParamExpString(rh, 0));
		}
		return false;
	}
	@Override
	public CValue expression(int num)
	{
		switch(num)
		{
			default: break;
			case 0: return new CValue(GetError());
			case 1: return new CValue(GetString());
			case 2: return new CValue(GetInteger());
			case 3: return new CValue(GetLong());
			case 4: return new CValue(GetFloat());
			case 5: return new CValue(GetDouble());
			case 6: return new CValue(GetNumValues());
			case 7: return new CValue(GetBoolNum());
			case 8: return new CValue(GetObjString(ho.getExpParam().getString()));
			case 9: return new CValue(GetObjInteger(ho.getExpParam().getString()));
			case 10: return new CValue(GetObjLong(ho.getExpParam().getString()));
			case 11: return new CValue(GetObjFloat(ho.getExpParam().getString()));
			case 12: return new CValue(GetObjDouble(ho.getExpParam().getString()));
			case 13: return new CValue(GetObjNumValues(ho.getExpParam().getString()));
			case 14: return new CValue(GetObjBoolNum(ho.getExpParam().getString()));
			case 15: return new CValue(GetArrString(ho.getExpParam().getInt()));
			case 16: return new CValue(GetArrInteger(ho.getExpParam().getInt()));
			case 17: return new CValue(GetArrLong(ho.getExpParam().getInt()));
			case 18: return new CValue(GetArrFloat(ho.getExpParam().getInt()));
			case 19: return new CValue(GetArrDouble(ho.getExpParam().getInt()));
			case 20: return new CValue(GetArrNumValues(ho.getExpParam().getInt()));
			case 21: return new CValue(GetArrBoolNum(ho.getExpParam().getInt()));
			case 22: return new CValue(GetIteratedName());
			case 23: return new CValue(GetIteratedIndex());
		}
		return new CValue(0);
	}
}
