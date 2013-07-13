
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
	void LoadJSON(TCHAR const *JSON, int flags);
	void EnterObject(TCHAR const *Name);
	void EnterArray(unsigned index);
	void GoUp();
	void GotoRoot();

	//Conditions
	bool OnError(); //0
	bool IsString();
	bool IsInteger();
	bool IsDouble();
	bool IsObject();
	bool IsArray();
	bool IsBoolean();
	bool IsNull();
	bool IsTrue();

	//Expressions
	TCHAR const *GetError();
	TCHAR const *GetString();
	int GetInteger();
	TCHAR const *GetLong();
	float GetFloat();
	TCHAR const *GetDouble();
	unsigned GetNumValues();
	unsigned GetBoolNum();


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
