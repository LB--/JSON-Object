#include "Common.h"


void Extension::LoadJSON(TCHAR const *JSON, int flags)
{
	json_settings settings = {0};
	settings.settings = (flags ? json_relaxed_commas : 0);
	char Error[128];
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
