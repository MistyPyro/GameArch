#pragma once
#include "Trackable.h"
#include <string>
#include <unordered_map>


//https://stackoverflow.com/questions/14208412/static-string-literal-table
//https://cplusplus.com/forum/general/72340/
//https://www.w3schools.com/cpp/cpp_ref_string.asp
//https://www.geeksforgeeks.org/cpp/array-of-strings-in-cpp-5-different-ways-to-create/
//https://forums.unrealengine.com/t/using-string-tables-from-c/417239
//https://www.chilkatsoft.com/refdoc/vcCkStringTableRef.html
//https://stackoverflow.com/questions/29800551/how-to-create-a-string-table
//https://forums.codeguru.com/showthread.php?60236-How-to-use-a-String-Table-
//https://www.experts-exchange.com/questions/28871479/Windows-Visual-C-String-table-Comparison-tool-WIndows-Exes-and-DLLs.html
//https://developercommunity.visualstudio.com/t/c-editing-the-string-table-in-the-resource-file-ma/1144691
//https://stackoverflow.com/questions/15486235/c-string-table-res-file


enum class StringID
{
	HUD_HP,
	HUD_SCORE,
	HUD_FPS,
	HUD_WEB,
	HUD_BLOCK,
	HUD_BLAST,
	MENU_PLAY,
	MENU_LOAD,
	MENU_OPTIONS,
	MENU_QUIT,
	OPT_TITLE,
	OPT_DIFFICULTY,
	OPT_EASY,
	OPT_NORMAL,
	OPT_HARD,
	OPT_SOUND_FX,
	OPT_MUSIC,
	OPT_VOLUME,
	OPT_LANGUAGE,
	OPT_ENGLISH,
	OPT_SPANISH,
	OPT_FRENCH,
	OPT_BACK,
	OPT_ON,
	OPT_OFF,
	PAUSE_TITLE,
	PAUSE_RESUME,
	PAUSE_OPTIONS,
	PAUSE_QUIT_TO_MAIN,
	BOSS_NAME,
	DEATH_TITLE,
	DEATH_RESTART,
	DEATH_QUIT_MAIN,
	DEATH_SCORE,
	WIN_TITLE,
	WIN_FINAL_TITLE,
	WIN_NEXT_LEVEL,
	WIN_MAIN_MENU,
	WIN_SCORE,
	NUM_STRINGS
};

class LanguageManager : public Trackable
{
public:
	enum class Language
	{
		ENGLISH = 0,
		SPANISH = 1,
		FRENCH = 2
	};

	static LanguageManager* getInstance();
	static void destroyInstance();
	void setLanguage(Language lang);
	Language getLanguage() const { return mLanguage; }
	const std::string& get(StringID id) const;
	bool loadFromFile(const std::string& filename);

private:
	LanguageManager();
	~LanguageManager();
	void buildEnglish();
	void buildSpanish();
	void buildFrench();
	static LanguageManager* mpsInstance;
	Language mLanguage = Language::ENGLISH;
	using StringTable = std::unordered_map<int, std::string>;
	StringTable mEnglish;
	StringTable mSpanish;
	StringTable mFrench;
	const StringTable* mpCurrent = nullptr;
	static const std::string MISSING;

};

inline const std::string& LS(StringID id)
{
	return LanguageManager::getInstance()->get(id);
}