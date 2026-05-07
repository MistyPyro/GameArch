#include "GraphicsBufferManager.h"
#include "GraphicsBuffer.h"

GraphicsBufferManager::~GraphicsBufferManager()
{
	cleanup();
}

void GraphicsBufferManager::cleanup()
{
	for (auto iter : mMap)
	{
		GraphicsBuffer* pGraphicsBuffer = iter.second;
		delete pGraphicsBuffer;
	}

	mMap.clear();
}

GraphicsBuffer* GraphicsBufferManager::createAndManageGraphicsBuffer(const GraphicsBufferKey& key, unsigned int width, unsigned int height)
{
	GraphicsBuffer* pGraphicsBuffer = nullptr;

	auto iter = mMap.find(key);

	if (iter == mMap.end())
	{
		pGraphicsBuffer = new GraphicsBuffer(width, height);
		mMap[key] = pGraphicsBuffer;
	}

	return pGraphicsBuffer;
}

GraphicsBuffer* GraphicsBufferManager::createAndManageGraphicsBuffer(const GraphicsBufferKey& key, const std::string filename)
{
	GraphicsBuffer* pGraphicsBuffer = nullptr;

	auto iter = mMap.find(key);

	if (iter == mMap.end())
	{
		pGraphicsBuffer = new GraphicsBuffer(filename);
		mMap[key] = pGraphicsBuffer;
	}

	return pGraphicsBuffer;
}

void GraphicsBufferManager::deleteGraphicsBuffer(const GraphicsBufferKey& key)
{
	auto iter = mMap.find(key);

	if (iter != mMap.end())
	{
		delete iter->second;
		mMap.erase(iter);
	}
}

void GraphicsBufferManager::deleteGraphicsBuffer(GraphicsBuffer* pGraphicsBuffer)
{
	for (auto iter : mMap)
	{
		if (iter.second == pGraphicsBuffer)
		{
			delete pGraphicsBuffer;
			//erase uses key
			mMap.erase(iter.first);
			return;
		}
	}
}

GraphicsBuffer* GraphicsBufferManager::getGraphicsBuffer(const GraphicsBufferKey& key)
{
	auto iter = mMap.find(key);

	if (iter != mMap.end())
	{
		return iter->second;
	}
	//if none found
	return nullptr;
}