#include "Common.h"

TCHAR const *Extension::GetError()
{
	return Runtime.CopyString(error.c_str());
}

TCHAR const *Extension::GetString()
{
	if(IsString())
	{
		TCHAR *t = Edif::ConvertString(current->u.string.ptr);
		TCHAR *c = Runtime.CopyString(t);
		Edif::FreeString(t);
		return c;
	}
	else if(IsInteger())
	{
		std::basic_ostringstream<TCHAR> oss;
		oss << GetLong();
		return Runtime.CopyString(oss.str().c_str());
	}
	else if(IsDouble())
	{
		std::basic_ostringstream<TCHAR> oss;
		oss << GetDouble();
		return Runtime.CopyString(oss.str().c_str());
	}
	else if(IsBoolean())
	{
		if(GetBoolNum())
		{
			return _T("true");
		}
		else
		{
			return _T("false");
		}
	}
	else if(IsNull())
	{
		return _T("null");
	}
	return _T("");
}
int Extension::GetInteger()
{
	if(IsInteger())
	{
		return static_cast<int>(current->u.integer);
	}
	else if(IsDouble())
	{
		return static_cast<int>(current->u.dbl);
	}
	else if(IsString())
	{
		TCHAR *t = Edif::ConvertString(current->u.string.ptr);
		std::basic_istringstream<TCHAR> iss (t);
		Edif::FreeString(t);
		int x = 0;
		iss >> x;
		return x;
	}
	return 0;
}
TCHAR const *Extension::GetLong()
{
	if(IsInteger())
	{
		std::basic_ostringstream<TCHAR> oss;
		oss << current->u.integer;
		return Runtime.CopyString(oss.str().c_str());
	}
	else if(IsDouble())
	{
		std::basic_ostringstream<TCHAR> oss;
		oss << static_cast<long long>(current->u.dbl);
		return Runtime.CopyString(oss.str().c_str());
	}
	else if(IsString())
	{
		TCHAR *t = Edif::ConvertString(current->u.string.ptr);
		std::basic_istringstream<TCHAR> iss (t);
		Edif::FreeString(t);
		long long x = 0;
		iss >> x;
		std::basic_ostringstream<TCHAR> oss;
		oss << x;
		return Runtime.CopyString(oss.str().c_str());
	}
	return _T("0");
}
float Extension::GetFloat()
{
	if(IsDouble())
	{
		return static_cast<float>(current->u.dbl);
	}
	else if(IsInteger())
	{
		return static_cast<float>(current->u.integer);
	}
	else if(IsString())
	{
		TCHAR *t = Edif::ConvertString(current->u.string.ptr);
		std::basic_istringstream<TCHAR> iss (t);
		Edif::FreeString(t);
		float x = 0;
		iss >> x;
		return x;
	}
	return 0.0f;
}
TCHAR const *Extension::GetDouble()
{
	if(IsDouble())
	{
		std::basic_ostringstream<TCHAR> oss;
		oss << std::setprecision(20) << current->u.dbl;
		return Runtime.CopyString(oss.str().c_str());
	}
	else if(IsInteger())
	{
		std::basic_ostringstream<TCHAR> oss;
		oss << static_cast<double>(current->u.integer);
		return Runtime.CopyString(oss.str().c_str());
	}
	else if(IsString())
	{
		TCHAR *t = Edif::ConvertString(current->u.string.ptr);
		std::basic_istringstream<TCHAR> iss (t);
		Edif::FreeString(t);
		double x = 0;
		iss >> x;
		std::basic_ostringstream<TCHAR> oss;
		oss << x;
		return Runtime.CopyString(oss.str().c_str());
	}
	return _T("0.0");
}
unsigned Extension::GetNumValues()
{
	if(IsObject())
	{
		return current->u.object.length;
	}
	else if(IsArray())
	{
		return current->u.array.length;
	}
	return 0;
}
unsigned Extension::GetBoolNum()
{
	if(IsBoolean())
	{
		return current->u.boolean ? 1 : 0;
	}
	else if(IsInteger())
	{
		return current->u.integer ? 1 : 0;
	}
	else if(IsString())
	{
		return GetString() == std::basic_string<TCHAR>(_T("true")) ? 1 : 0;
	}
	return 0;
}

TCHAR const *Extension::GetObjString(TCHAR const *Name)
{
	if(IsObject())
	{
		TempDelve td (*this, Name);
		return GetString();
	}
	return _T("");
}
int Extension::GetObjInteger(TCHAR const *Name)
{
	if(IsObject())
	{
		TempDelve td (*this, Name);
		return GetInteger();
	}
	return 0;
}
TCHAR const *Extension::GetObjLong(TCHAR const *Name)
{
	if(IsObject())
	{
		TempDelve td (*this, Name);
		return GetLong();
	}
	return _T("0");
}
float Extension::GetObjFloat(TCHAR const *Name)
{
	if(IsObject())
	{
		TempDelve td (*this, Name);
		return GetFloat();
	}
	return 0.0f;
}
TCHAR const *Extension::GetObjDouble(TCHAR const *Name)
{
	if(IsObject())
	{
		TempDelve td (*this, Name);
		return GetDouble();
	}
	return _T("0.0");
}
unsigned Extension::GetObjNumValues(TCHAR const *Name)
{
	if(IsObject())
	{
		TempDelve td (*this, Name);
		return GetNumValues();
	}
	return 0;
}
unsigned Extension::GetObjBoolNum(TCHAR const *Name)
{
	if(IsObject())
	{
		TempDelve td (*this, Name);
		return GetBoolNum();
	}
	return 0;
}

TCHAR const *Extension::GetArrString(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return GetString();
	}
	return _T("");
}
int Extension::GetArrInteger(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return GetInteger();
	}
	return 0;
}
TCHAR const *Extension::GetArrLong(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return GetLong();
	}
	return _T("0");
}
float Extension::GetArrFloat(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return GetFloat();
	}
	return 0.0f;
}
TCHAR const *Extension::GetArrDouble(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return GetDouble();
	}
	return _T("0.0");
}
unsigned Extension::GetArrNumValues(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return GetNumValues();
	}
	return 0;
}
unsigned Extension::GetArrBoolNum(unsigned index)
{
	if(IsArray() && index < current->u.array.length)
	{
		TempDelve td (*this, index);
		return GetBoolNum();
	}
	return 0;
}
