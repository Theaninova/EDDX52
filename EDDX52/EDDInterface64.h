// EDD Interface
#pragma once  

#ifdef _USRDLL 
#define EDD_API __declspec(dllexport)   
#else  
#define EDD_API __declspec(dllimport)   
#endif  


extern "C"
{
	struct JournalEntry
	{
	public:

		int ver;		// version of this structure. Order critical.
		int indexno;	// event index.  If -1, invalid Journal Entry.  Can happen for EDDRefresh if no history
						// 1 = first entry, to totalrecords

		BSTR utctime;
		BSTR name;
		BSTR info;
		BSTR detailedinfo;
		SAFEARRAY& materials;		// BSTRs in the format name:count
		SAFEARRAY& commodities;		// BSTRs in the format name:count

		BSTR systemname;
		double x;
		double y;
		double z;

		double travelleddistance;
		long travelledseconds;

		bool islanded;
		bool isdocked;

		BSTR whereami;
		BSTR shiptype;
		BSTR gamemode;
		BSTR group;
		long credits;

		BSTR eventid;

		SAFEARRAY& currentmissions;		// BSTRs listing current missions

		long long jid;					// jid of event
		long totalrecords;				// number of records 
										// Version 1 ends
	};

	typedef bool(*EDDRequestHistory)(long index, bool isjid, JournalEntry *f);		// if idjid=false, then 1 = first entry, onwards.
	typedef bool(*EDDRunAction)(BSTR eventname, BSTR parameters); // parameters in format v="k",X="k"

	struct EDDCallBacks
	{
		int ver;			// version of this structure = 1. Order critical.
		EDDRequestHistory RequestHistory;		// may be null - check
		EDDRunAction RunAction;					// may be null - check
												// Version 1 ends
	};

	// Copy data don't ref it, c# may remove them after the return at any time.

	// Called with EDD version A.B.C.D, return NULL if can't operate, or your version as X.Y.Z.B
	EDD_API BSTR __cdecl EDDInitialise(BSTR ver, BSTR dllfolder, EDDCallBacks requestcallback);		// mandatory

	//EDD_API void EDDRefresh(BSTR cmdr, JournalEntry last_je);			// optional, last_je is the last one received. last_je.indexno =-1 if no history is present.
	//EDD_API void EDDNewJournalEntry(JournalEntry nje);		// optional. nje will always be set.  Called when a new Journal Entry received
	//EDD_API void __cdecl EDDTerminate();					// optional
	EDD_API BSTR __cdecl EDDActionCommand(BSTR action, SAFEARRAY& args);		// optional. Called by Action DLLCall. Always return string. Args could be an empty array.
	//EDD_API void EDDActionJournalEntry(JournalEntry je);		// optional. Called by Action DLLCall to feed a journal entry to you.
}