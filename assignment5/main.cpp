#include <iostream>
#include <cassert>
#include <string>
#include <random>
#include <ctime>
#include <PerformanceTracker.h>
#include <MemoryTracker.h>
#include <GraphicsSystem.h>
#include <GraphicsBuffer.h>
#include <Sprite.h>
#include <Color.h>
#include <Font.h>
#include "Game.h"
#include "Animation.h"
#include "EventSystem.h"
#include "../LanguageManager.h"
#include <direct.h>

using namespace std;


int main()
{
    char cwd[512];
    _getcwd(cwd, sizeof(cwd));
    std::cout << "Working directory: " << cwd << "\n";

    EventSystem::createInstance();
    EventSystem* pEventSystem = EventSystem::getInstance();
    pEventSystem->init();
    LanguageManager::getInstance();
    Game* pGamePtr = Game::createInstance();
    pGamePtr->init();
    bool ret = pGamePtr->doLoop();
    LanguageManager::destroyInstance();
    pGamePtr->cleanup();
    pGamePtr->destroyInstance();
    pEventSystem->cleanup();
    pEventSystem->destroyInstance();
    MemoryTracker::getInstance()->reportAllocations(cout);
    system("pause");
    return ret;
}
