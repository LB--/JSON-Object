
#include <sstream>
#include <iomanip>

class Extension
{
public:
	/* rd
	 * This is a pointer to the classic
	 * RunData structure. Some things still
	 * have to be stored there for MMF2's
	 * sake, so you need to have acces to
	 * that data. You should not modify
	 * the rundata structure, however,
	 * as it is global to all extensions.
	 */
	RD *rd;

	/* rh
	 * This is a pointer to the RunHeader
	 * structure, which contains some
	 * application information.
	 */
	RunHeader *rh;

	/* Runtime
	 * This is EDIF's compendium of useful
	 * functions for every day life as an
	 * extension. You can trigger immediate
	 * events, read and write global data,
	 * etc. See the EDIF wiki for more
	 * information.
	 */
	Edif::Runtime Runtime;

	/* MinimumBuild
	 * Always set this to the latest build
	 * of MMF2 that has been released, unless
	 * you need to set it higher for a beta
	 * version of MMF2 to get access to some
	 * new SDK feature.
	 */
	static const int MinimumBuild = 255;

	/* Version
	 * This is the version of your extension
	 * that is stored in the editdata. You should
	 * only change this when your editdata structure
	 * changes, not just when you have a new version
	 * of your extension with new features.
	 */
	static const int Version = 1;

	/* OEFLAGS, OEPREFS
	 * These are settings for your extension
	 * that tell MMF2 how to treat your extension
	 * and what behaviors and responsibilities
	 * to add. Check the MMF2SDK Help file for
	 * more detailed information on the flags you
	 * can set and what they implicate.
	 */
	static const int OEFLAGS = OEFLAG_VALUES|OEFLAG_RUNBEFOREFADEIN|OEFLAG_NEVERKILL;
	static const int OEPREFS = 0;

	/* WindowProcPriority
	 * If your extension creates a Window
	 * and implements the WindowProc function,
	 * this is the priority for that stuff.
	 */
	static const int WindowProcPriority = 100;

	Extension(RD *rd, SerializedED *SED, createObjectInfo *COB);	//defined & documented in Extension.cpp
	~Extension();													//defined & documented in Extension.cpp

	json_value *root;
	json_value const*current;
	stdtstring error;

#ifdef UNICODE
	std::string UTF8fromUnicode(std::wstring s)
	{
		int size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, NULL, NULL);
		char *buf = new char[size];
		if(!WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, buf, size, NULL, NULL))
		{
			return "";
		}
		std::string t (buf);
		delete[] buf;
		return t;
	}
#else
	typedef std::string UTF8fromUnicode;
#endif

	//Actions
	void LoadJSON(TCHAR const*JSON, int flags)
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
	void EnterObject(TCHAR const*Name)
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
	void EnterArray(unsigned index)
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
	void GoUp()
	{
		current = current->parent ? current->parent : current;
	}
	void GotoRoot()
	{
		current = root;
	}

	//Conditions
	bool OnError() //0
	{
		return true;
	}
	bool IsString()
	{
		return current->type == json_string;
	}
	bool IsInteger()
	{
		return current->type == json_integer;
	}
	bool IsDouble()
	{
		return current->type == json_double;
	}
	bool IsObject()
	{
		return current->type == json_object;
	}
	bool IsArray()
	{
		return current->type == json_array;
	}
	bool IsBoolean()
	{
		return current->type == json_boolean;
	}
	bool IsNull()
	{
		return current->type == json_null;
	}
	bool IsTrue()
	{
		return IsBoolean() && current->u.boolean;
	}

	//Expressions
	TCHAR const*GetError()
	{
		return Runtime.CopyString(error.c_str());
	}
	TCHAR const*GetString()
	{
		if(!IsString()) return _T("");
		TCHAR const*t = Edif::ConvertString(current->u.string.ptr);
		TCHAR *c = Runtime.CopyString(t);
		Edif::FreeString(c);
		return c;
	}
	int GetInteger()
	{
		return IsInteger() ? static_cast<int>(current->u.integer) : 0;
	}
	TCHAR const*GetLong()
	{
		if(IsInteger())
		{
			std::basic_ostringstream<TCHAR> ss;
			ss << current->u.integer;
			return Runtime.CopyString(ss.str().c_str());
		}
		return _T("0");
	}
	float GetFloat()
	{
		return IsDouble() ? static_cast<float>(current->u.dbl) : 0.0f;
	}
	TCHAR const*GetDouble()
	{
		if(IsDouble())
		{
			std::basic_ostringstream<TCHAR> ss;
			ss << std::setprecision(20) << current->u.dbl;
			return Runtime.CopyString(ss.str().c_str());
		}
		return _T("0.0");
	}
	unsigned GetNumValues()
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
	unsigned GetBoolNum()
	{
		return IsBoolean() ? (current->u.boolean ? 1 : 0) : 0;
	}
	


	short Handle();			//defined & documented in Extension.cpp
	short Display();		//defined & documented in Extension.cpp

	short Pause();			//defined & documented in Extension.cpp
	short Continue();		//defined & documented in Extension.cpp

	bool Save(HANDLE File);	//defined & documented in Extension.cpp
	bool Load(HANDLE File);	//defined & documented in Extension.cpp

	//defined & documented in Extension.cpp
	void Action(int ID, RD *rd, long param1, long param2);
	long Condition(int ID, RD *rd, long param1, long param2);
	long Expression(int ID, RD *rd, long param);
};
