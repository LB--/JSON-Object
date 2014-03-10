#include "Common.h"


bool Extension::OnError()
{
	return true;
}
bool Extension::IsString()
{
	return current->type == json_string;
}
bool Extension::IsInteger()
{
	return current->type == json_integer;
}
bool Extension::IsDouble()
{
	return current->type == json_double;
}
bool Extension::IsObject()
{
	return current->type == json_object;
}
bool Extension::IsArray()
{
	return current->type == json_array;
}
bool Extension::IsBoolean()
{
	return current->type == json_boolean;
}
bool Extension::IsNull()
{
	return current->type == json_null;
}
bool Extension::IsTrue()
{
	if(IsBoolean())
	{
		return current->u.boolean ? true : false;
	}
	else if(IsInteger())
	{
		return current->u.integer ? true : false;
	}
	else if(IsString())
	{
		return GetString() == std::basic_string<TCHAR>(_T("true"));
	}
	return false;
}

bool Extension::ObjExists(TCHAR const *Name)
{
	if(IsObject())
	{
		std::string n = UTF8fromUnicode(Name);
		for(unsigned i = 0; i < current->u.object.length; ++i)
		{
			if(current->u.object.values[i].name == n)
			{
				return true;
			}
		}
	}
	return false;
}

bool Extension::IsObjString(TCHAR const *Name)
{
	if(ObjExists(Name))
	{
		TempDelve td (*this, Name);
		return IsString();
	}
	return false;
}
bool Extension::IsObjInteger(TCHAR const *Name)
{
	if(ObjExists(Name))
	{
		TempDelve td (*this, Name);
		return IsInteger();
	}
	return false;
}
bool Extension::IsObjDouble(TCHAR const *Name)
{
	if(ObjExists(Name))
	{
		TempDelve td (*this, Name);
		return IsDouble();
	}
	return false;
}
bool Extension::IsObjObject(TCHAR const *Name)
{
	if(ObjExists(Name))
	{
		TempDelve td (*this, Name);
		return IsObject();
	}
	return false;
}
bool Extension::IsObjArray(TCHAR const *Name)
{
	if(ObjExists(Name))
	{
		TempDelve td (*this, Name);
		return IsArray();
	}
	return false;
}
bool Extension::IsObjBoolean(TCHAR const *Name)
{
	if(ObjExists(Name))
	{
		TempDelve td (*this, Name);
		return IsBoolean();
	}
	return false;
}
bool Extension::IsObjNull(TCHAR const *Name)
{
	if(ObjExists(Name))
	{
		TempDelve td (*this, Name);
		return IsNull();
	}
	return false;
}
bool Extension::IsObjTrue(TCHAR const *Name)
{
	if(ObjExists(Name))
	{
		TempDelve td (*this, Name);
		return IsTrue();
	}
	return false;
}

bool Extension::IsArrString(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return IsString();
	}
	return false;
}
bool Extension::IsArrInteger(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return IsInteger();
	}
	return false;
}
bool Extension::IsArrDouble(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return IsDouble();
	}
	return false;
}
bool Extension::IsArrObject(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return IsObject();
	}
	return false;
}
bool Extension::IsArrArray(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return IsArray();
	}
	return false;
}
bool Extension::IsArrBoolean(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return IsBoolean();
	}
	return false;
}
bool Extension::IsArrNull(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return IsNull();
	}
	return false;
}
bool Extension::IsArrTrue(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return IsTrue();
	}
	return false;
}
