
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>

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
	static const int MinimumBuild = 257;

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
	json_value const *current;
	stdtstring error;
	std::map<stdtstring, json_value const *> bookmarks;
	struct Loop
	{
		stdtstring name;
		json_value const *object;
		std::string sub_name;
		std::size_t index;
		json_value const *sub;

		Loop(stdtstring const &id, json_value const *obj)
		: name(id)
		, object(obj)
		, index()
		, sub()
		{
		}
		Loop(Loop const &from)
		: name(from.name)
		, object(from.object)
		, sub_name(from.sub_name)
		, index(from.index)
		, sub(from.sub)
		{
		}
		Loop &operator=(Loop const &from)
		{
			name = from.name;
			object = from.object;
			sub_name = from.sub_name;
			index = from.index;
			sub = from.sub;
			return *this;
		}
	};
	std::vector<Loop> loops;

#ifdef UNICODE
	std::string UTF8fromUnicode(std::wstring s)
	{
		int size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, NULL, NULL);
		char *buf = new char[size]();
		if(!WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, buf, size, NULL, NULL))
		{
			delete[] buf;
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

	void SetBookmark(TCHAR const *Name);
	void GotoBookmark(TCHAR const *Name);

	void LoopObjects(TCHAR const *LoopName);

	void DebugWindow();

	struct TempDelve
	{
		Extension &ext;
		TempDelve(Extension &e, TCHAR const *Name)
		: ext(e)
		{
			ext.EnterObject(Name);
		}
		TempDelve(Extension &e, unsigned index)
		: ext(e)
		{
			ext.EnterArray(index);
		}
		~TempDelve()
		{
			ext.GoUp();
		}
	};

	//Conditions
	bool OnError(); //0
	bool OnLoop(TCHAR const *LoopName); //26

	bool IsString();
	bool IsInteger();
	bool IsDouble();
	bool IsObject();
	bool IsArray();
	bool IsBoolean();
	bool IsNull();
	bool IsTrue();

	bool ObjExists(TCHAR const *Name);

	bool IsObjString(TCHAR const *Name);
	bool IsObjInteger(TCHAR const *Name);
	bool IsObjDouble(TCHAR const *Name);
	bool IsObjObject(TCHAR const *Name);
	bool IsObjArray(TCHAR const *Name);
	bool IsObjBoolean(TCHAR const *Name);
	bool IsObjNull(TCHAR const *Name);
	bool IsObjTrue(TCHAR const *Name);

	bool IsArrString(unsigned index);
	bool IsArrInteger(unsigned index);
	bool IsArrDouble(unsigned index);
	bool IsArrObject(unsigned index);
	bool IsArrArray(unsigned index);
	bool IsArrBoolean(unsigned index);
	bool IsArrNull(unsigned index);
	bool IsArrTrue(unsigned index);

	//Expressions
	TCHAR const *GetError();
	TCHAR const *GetIteratedName();
	int GetIteratedIndex();

	TCHAR const *GetString();
	int GetInteger();
	TCHAR const *GetLong();
	float GetFloat();
	TCHAR const *GetDouble();
	unsigned GetNumValues();
	unsigned GetBoolNum();

	TCHAR const *GetObjString(TCHAR const *Name);
	int GetObjInteger(TCHAR const *Name);
	TCHAR const *GetObjLong(TCHAR const *Name);
	float GetObjFloat(TCHAR const *Name);
	TCHAR const *GetObjDouble(TCHAR const *Name);
	unsigned GetObjNumValues(TCHAR const *Name);
	unsigned GetObjBoolNum(TCHAR const *Name);

	TCHAR const *GetArrString(unsigned index);
	int GetArrInteger(unsigned index);
	TCHAR const *GetArrLong(unsigned index);
	float GetArrFloat(unsigned index);
	TCHAR const *GetArrDouble(unsigned index);
	unsigned GetArrNumValues(unsigned index);
	unsigned GetArrBoolNum(unsigned index);


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
