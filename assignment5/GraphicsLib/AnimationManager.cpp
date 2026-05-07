#include "AnimationManager.h"
#include "Animation.h"

void AnimationManager::cleanup()
{
	for (auto iter : mMap)
	{
		Animation* pAnimation = iter.second;
		delete pAnimation;
	}

	mMap.clear();
}

Animation* AnimationManager::createAndAddAnimation(bool isLooping, std::vector<Sprite> spriteList, const AnimationKey& key)
{
	auto iter = mMap.find(key);
	if (iter != mMap.end())
	{
		delete iter->second;
		mMap.erase(iter);
	}

	Animation* pAnimation = new Animation(isLooping);
	for (unsigned int i = 0; i < spriteList.size(); i++)
		pAnimation->addSprite(spriteList[i]);

	mMap[key] = pAnimation;
	return pAnimation;
}

Animation* AnimationManager::getAnimation(const AnimationKey& key)
{
	auto iter = mMap.find(key);
	if (iter != mMap.end())
		return iter->second;

	return nullptr;
}

void AnimationManager::toggleAnimationFreeze(const AnimationKey& key)
{
	Animation* pTempAnim = getAnimation(key);

	if (pTempAnim != nullptr)
	{
		pTempAnim->toggleFrozen();
	}
}

void AnimationManager::handleEvent(const Event& theEvent)
{

}