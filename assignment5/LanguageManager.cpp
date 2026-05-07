#include "LanguageManager.h"
#include <fstream>
#include <iostream>

LanguageManager* LanguageManager::mpsInstance = nullptr;
const std::string LanguageManager::MISSING = "???";

LanguageManager* LanguageManager::getInstance()
{
    if (!mpsInstance)
    {
        mpsInstance = new LanguageManager();
        mpsInstance->buildEnglish();
        mpsInstance->buildSpanish();
        mpsInstance->buildFrench();
        mpsInstance->mpCurrent = &mpsInstance->mEnglish;
    }
    return mpsInstance;
}

LanguageManager::~LanguageManager() {}

void LanguageManager::destroyInstance()
{
    delete mpsInstance;
    mpsInstance = nullptr;
}

LanguageManager::LanguageManager() {}

void LanguageManager::setLanguage(Language lang)
{
    mLanguage = lang;
    switch (lang)
    {
    case Language::SPANISH: mpCurrent = &mSpanish; break;
    case Language::FRENCH: mpCurrent = &mFrench;  break;
    default: mpCurrent = &mEnglish;  break;
    }
    std::cout << "LanguageManager: language set to " << (int)lang << "\n";
}

const std::string& LanguageManager::get(StringID id) const
{
    if (!mpCurrent) return MISSING;
    auto it = mpCurrent->find((int)id);
    if (it == mpCurrent->end())
    {
        auto en = mEnglish.find((int)id);
        if (en != mEnglish.end()) return en->second;
        return MISSING;
    }
    return it->second;
}

void LanguageManager::buildEnglish()
{
    auto& t = mEnglish;
    t[(int)StringID::HUD_HP] = "HP";
    t[(int)StringID::HUD_SCORE] = "Score";
    t[(int)StringID::HUD_FPS] = "FPS";
    t[(int)StringID::HUD_WEB] = "Web";
    t[(int)StringID::HUD_BLOCK] = "BLOCK";
    t[(int)StringID::HUD_BLAST] = "BLAST!";

    t[(int)StringID::MENU_PLAY] = "Play New Game";
    t[(int)StringID::MENU_LOAD] = "Load Game";
    t[(int)StringID::MENU_OPTIONS] = "Options";
    t[(int)StringID::MENU_QUIT] = "Quit";

    t[(int)StringID::OPT_TITLE] = "Options";
    t[(int)StringID::OPT_DIFFICULTY] = "Difficulty";
    t[(int)StringID::OPT_EASY] = "Easy";
    t[(int)StringID::OPT_NORMAL] = "Normal";
    t[(int)StringID::OPT_HARD] = "Hard";
    t[(int)StringID::OPT_SOUND_FX] = "Sound FX";
    t[(int)StringID::OPT_MUSIC] = "Music";
    t[(int)StringID::OPT_VOLUME] = "Volume";
    t[(int)StringID::OPT_LANGUAGE] = "Language";
    t[(int)StringID::OPT_ENGLISH] = "English";
    t[(int)StringID::OPT_SPANISH] = "Espanol";
    t[(int)StringID::OPT_FRENCH] = "Francais";
    t[(int)StringID::OPT_BACK] = "Back";
    t[(int)StringID::OPT_ON] = "On";
    t[(int)StringID::OPT_OFF] = "Off";

    t[(int)StringID::PAUSE_TITLE] = "Paused";
    t[(int)StringID::PAUSE_RESUME] = "Resume";
    t[(int)StringID::PAUSE_OPTIONS] = "Options";
    t[(int)StringID::PAUSE_QUIT_TO_MAIN] = "Quit to Main Menu";
    t[(int)StringID::DEATH_TITLE] = "You Died";
    t[(int)StringID::DEATH_RESTART] = "Play Again";
    t[(int)StringID::DEATH_QUIT_MAIN] = "Main Menu";
    t[(int)StringID::DEATH_SCORE] = "Score";

    t[(int)StringID::WIN_TITLE] = "Level Complete!";
    t[(int)StringID::WIN_FINAL_TITLE] = "You Win!";
    t[(int)StringID::WIN_NEXT_LEVEL] = "Next Level";
    t[(int)StringID::WIN_MAIN_MENU] = "Main Menu";
    t[(int)StringID::WIN_SCORE] = "Score";


    t[(int)StringID::BOSS_NAME] = "VENOM";
}

void LanguageManager::buildSpanish()
{
    auto& t = mSpanish;
    t[(int)StringID::HUD_HP] = "Vida";
    t[(int)StringID::HUD_SCORE] = "Puntos";
    t[(int)StringID::HUD_FPS] = "FPS";
    t[(int)StringID::HUD_WEB] = "Tela";
    t[(int)StringID::HUD_BLOCK] = "BLOQUEAR";
    t[(int)StringID::HUD_BLAST] = "DISPARO!";

    t[(int)StringID::MENU_PLAY] = "Nuevo Juego";
    t[(int)StringID::MENU_LOAD] = "Cargar Juego";
    t[(int)StringID::MENU_OPTIONS] = "Opciones";
    t[(int)StringID::MENU_QUIT] = "Salir";

    t[(int)StringID::OPT_TITLE] = "Opciones";
    t[(int)StringID::OPT_DIFFICULTY] = "Dificultad";
    t[(int)StringID::OPT_EASY] = "Facil";
    t[(int)StringID::OPT_NORMAL] = "Normal";
    t[(int)StringID::OPT_HARD] = "Dificil";
    t[(int)StringID::OPT_SOUND_FX] = "Efectos";
    t[(int)StringID::OPT_MUSIC] = "Musica";
    t[(int)StringID::OPT_VOLUME] = "Volumen";
    t[(int)StringID::OPT_LANGUAGE] = "Idioma";
    t[(int)StringID::OPT_ENGLISH] = "English";
    t[(int)StringID::OPT_SPANISH] = "Espanol";
    t[(int)StringID::OPT_FRENCH] = "Francais";
    t[(int)StringID::OPT_BACK] = "Volver";
    t[(int)StringID::OPT_ON] = "Si";
    t[(int)StringID::OPT_OFF] = "No";

    t[(int)StringID::PAUSE_TITLE] = "Pausa";
    t[(int)StringID::PAUSE_RESUME] = "Continuar";
    t[(int)StringID::PAUSE_OPTIONS] = "Opciones";
    t[(int)StringID::PAUSE_QUIT_TO_MAIN] = "Menu Principal";
    t[(int)StringID::DEATH_TITLE] = "Has Muerto";
    t[(int)StringID::DEATH_RESTART] = "Jugar de Nuevo";
    t[(int)StringID::DEATH_QUIT_MAIN] = "Menu Principal";
    t[(int)StringID::DEATH_SCORE] = "Puntos";

    t[(int)StringID::WIN_TITLE] = "Nivel Completado!";
    t[(int)StringID::WIN_FINAL_TITLE] = "Has Ganado!";
    t[(int)StringID::WIN_NEXT_LEVEL] = "Siguiente Nivel";
    t[(int)StringID::WIN_MAIN_MENU] = "Menu Principal";
    t[(int)StringID::WIN_SCORE] = "Puntos";


    t[(int)StringID::BOSS_NAME] = "VENENO";
}

void LanguageManager::buildFrench()
{
    auto& t = mFrench;
    t[(int)StringID::HUD_HP] = "Vie";
    t[(int)StringID::HUD_SCORE] = "Score";
    t[(int)StringID::HUD_FPS] = "IPS";
    t[(int)StringID::HUD_WEB] = "Toile";
    t[(int)StringID::HUD_BLOCK] = "BLOQUER";
    t[(int)StringID::HUD_BLAST] = "TIRER!";

    t[(int)StringID::MENU_PLAY] = "Nouvelle Partie";
    t[(int)StringID::MENU_LOAD] = "Charger";
    t[(int)StringID::MENU_OPTIONS] = "Options";
    t[(int)StringID::MENU_QUIT] = "Quitter";

    t[(int)StringID::OPT_TITLE] = "Options";
    t[(int)StringID::OPT_DIFFICULTY] = "Difficulte";
    t[(int)StringID::OPT_EASY] = "Facile";
    t[(int)StringID::OPT_NORMAL] = "Normal";
    t[(int)StringID::OPT_HARD] = "Difficile";
    t[(int)StringID::OPT_SOUND_FX] = "Effets";
    t[(int)StringID::OPT_MUSIC] = "Musique";
    t[(int)StringID::OPT_VOLUME] = "Volume";
    t[(int)StringID::OPT_LANGUAGE] = "Langue";
    t[(int)StringID::OPT_ENGLISH] = "English";
    t[(int)StringID::OPT_SPANISH] = "Espanol";
    t[(int)StringID::OPT_FRENCH] = "Francais";
    t[(int)StringID::OPT_BACK] = "Retour";
    t[(int)StringID::OPT_ON] = "Oui";
    t[(int)StringID::OPT_OFF] = "Non";

    t[(int)StringID::PAUSE_TITLE] = "Pause";
    t[(int)StringID::PAUSE_RESUME] = "Reprendre";
    t[(int)StringID::PAUSE_OPTIONS] = "Options";
    t[(int)StringID::PAUSE_QUIT_TO_MAIN] = "Menu Principal";
    t[(int)StringID::DEATH_TITLE] = "Vous etes mort";
    t[(int)StringID::DEATH_RESTART] = "Rejouer";
    t[(int)StringID::DEATH_QUIT_MAIN] = "Menu Principal";
    t[(int)StringID::DEATH_SCORE] = "Score";

    t[(int)StringID::WIN_TITLE] = "Niveau Termine!";
    t[(int)StringID::WIN_FINAL_TITLE] = "Vous avez gagne!";
    t[(int)StringID::WIN_NEXT_LEVEL] = "Niveau Suivant";
    t[(int)StringID::WIN_MAIN_MENU] = "Menu Principal";
    t[(int)StringID::WIN_SCORE] = "Score";

    t[(int)StringID::BOSS_NAME] = "VENIN";
}

bool LanguageManager::loadFromFile(const std::string& filename)
{
    // For now strings are hardcoded
    std::cout << "LanguageManager: loadFromFile stub (" << filename << ")\n";
    return true;
}
