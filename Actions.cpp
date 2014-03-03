#include "Common.h"


void Extension::LoadJSON(TCHAR const *JSON, int flags)
{
	json_settings settings = {0};
	settings.settings = (flags ? json_enable_comments : 0);
	char Error[json_error_max];
#ifdef UNICODE
	std::string json = UTF8fromUnicode(JSON);
	if(json.empty())
	{
		error = L"Could not convert Unicode JSON to UTF8";
		Runtime.GenerateEvent(0);
		error.clear();
		return;
	}
	json_value *temp = json_parse_ex(&settings, json.c_str(), json.length(), Error);
#else
	json_value *temp = json_parse_ex(&settings, JSON, strlen(JSON), Error);
#endif
	if(!temp)
	{
		TCHAR *t = 0;
		error = (t = Edif::ConvertString(Error));
		Edif::FreeString(t), t = 0;
		Runtime.GenerateEvent(0);
		error.clear();
		return;
	}
	if(root)
	{
		current = 0;
		json_value_free(root);
	}
	current = root = temp;
}

void Extension::EnterObject(TCHAR const *Name)
{
	if(IsObject())
	{
		json_value const*temp = &((*current)[UTF8fromUnicode(Name).c_str()]);
		if(temp)
		{
			current = temp;
		}
	}
}

void Extension::EnterArray(unsigned index)
{
	if(IsArray())
	{
		json_value const*temp = &((*current)[index]);
		if(temp)
		{
			current = temp;
		}
	}
}

void Extension::GoUp()
{
	current = current->parent ? current->parent : current;
}

void Extension::GotoRoot()
{
	current = root;
}

#include <sstream>
void Extension::DebugWindow()
{
#ifdef UNICODE
	std::wostringstream oss;
#else
	std::ostringstream oss;
#endif
	oss << _T("IsString:  ") << std::boolalpha << IsString()  << std::endl;
	oss << _T("IsInteger: ") << std::boolalpha << IsInteger() << std::endl;
	oss << _T("IsDouble:  ") << std::boolalpha << IsDouble()  << std::endl;
	oss << _T("IsObject:  ") << std::boolalpha << IsObject()  << std::endl;
	oss << _T("IsArray:   ") << std::boolalpha << IsArray()   << std::endl;
	oss << _T("IsBoolean: ") << std::boolalpha << IsBoolean() << std::endl;
	oss << _T("IsNull:    ") << std::boolalpha << IsNull()    << std::endl;
	oss << _T("IsTrue:    ") << std::boolalpha << IsTrue()    << std::endl;
	oss << std::endl;
	oss << _T("GetError: ")     << GetError()     << std::endl;
	oss << _T("GetString: ")    << GetString()    << std::endl;
	oss << _T("GetInteger: ")   << GetInteger()   << std::endl;
	oss << _T("GetLong: ")      << GetLong()      << std::endl;
	oss << _T("GetFloat: ")     << GetFloat()     << std::endl;
	oss << _T("GetDouble: ")    << GetDouble()    << std::endl;
	oss << _T("GetNumValues: ") << GetNumValues() << std::endl;
	oss << _T("GetBoolNum: ")   << GetBoolNum()   << std::endl;
	MessageBox(NULL, oss.str().c_str(), _T("JSON Object Debug"), MB_OK);
}
