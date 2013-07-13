#include "Common.h"


TCHAR const *Extension::GetError()
{
	return Runtime.CopyString(error.c_str());
}
TCHAR const *Extension::GetString()
{
	if(!IsString()) return _T("");
	TCHAR *t = Edif::ConvertString(current->u.string.ptr);
	TCHAR *c = Runtime.CopyString(t);
	Edif::FreeString(t);
	return c;
}
int Extension::GetInteger()
{
	return IsInteger() ? static_cast<int>(current->u.integer) : 0;
}
TCHAR const *Extension::GetLong()
{
	if(IsInteger())
	{
		std::basic_ostringstream<TCHAR> ss;
		ss << current->u.integer;
		return Runtime.CopyString(ss.str().c_str());
	}
	return _T("0");
}
float Extension::GetFloat()
{
	return IsDouble() ? static_cast<float>(current->u.dbl) : 0.0f;
}
TCHAR const *Extension::GetDouble()
{
	if(IsDouble())
	{
		std::basic_ostringstream<TCHAR> ss;
		ss << std::setprecision(20) << current->u.dbl;
		return Runtime.CopyString(ss.str().c_str());
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
	return IsBoolean() ? (current->u.boolean ? 1 : 0) : 0;
}
