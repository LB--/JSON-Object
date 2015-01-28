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
	bookmarks.clear();
	loops.clear();
}

void Extension::EnterObject(TCHAR const *Name)
{
	if(IsObject() && ObjExists(Name))
	{
		current = &((*current)[UTF8fromUnicode(Name).c_str()]);
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

void Extension::SetBookmark(TCHAR const *Name)
{
	bookmarks[Name] = current;
}
void Extension::GotoBookmark(TCHAR const *Name)
{
	if(bookmarks.find(Name) != bookmarks.end())
	{
		current = bookmarks[Name];
	}
}

void Extension::LoopObjects(TCHAR const *LoopName)
{
	json_value const *const looper = current;
	loops.push_back(Loop(LoopName, looper));
	if(IsObject())
	{
		std::size_t length = looper->u.object.length;
		for(std::size_t i = 0; i < length; ++i)
		{
			if(loops.size() == 0 || loops.back().object != looper)
			{
				break;
			}
			loops.back().index = i;
			loops.back().sub_name = std::string(looper->u.object.values[i].name, looper->u.object.values[i].name_length);
			loops.back().sub = looper->u.object.values[i].value;
			Runtime.GenerateEvent(26);
		}
	}
	else if(IsArray())
	{
		std::size_t length = looper->u.array.length;
		for(std::size_t i = 0; i < length; ++i)
		{
			if(loops.size() == 0 || loops.back().object != looper)
			{
				break;
			}
			loops.back().index = i;
			loops.back().sub_name.clear();
			loops.back().sub = looper->u.array.values[i];
			Runtime.GenerateEvent(26);
		}
	}
	if(loops.size() > 0 && loops.back().object == looper)
	{
		loops.pop_back();
		current = looper;
	}
}

void Extension::DebugWindow()
{
	std::basic_ostringstream<TCHAR> oss;
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
