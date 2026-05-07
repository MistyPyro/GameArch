#pragma once
#include <Trackable.h>
#include <string>
#include <unordered_map>

class GraphicsBuffer;

typedef std::string GraphicsBufferKey;

class GraphicsBufferManager : public Trackable
{
public:
	GraphicsBufferManager() {};
	~GraphicsBufferManager();

	void init() {};
	void cleanup();

	GraphicsBuffer* createAndManageGraphicsBuffer(const GraphicsBufferKey& key, unsigned int width, unsigned int height);
	GraphicsBuffer* createAndManageGraphicsBuffer(const GraphicsBufferKey& key, const std::string filename);
	void deleteGraphicsBuffer(const GraphicsBufferKey& key);
	void deleteGraphicsBuffer(GraphicsBuffer* deleteGraphicsBuffer);
	GraphicsBuffer* getGraphicsBuffer(const GraphicsBufferKey& key);

private:
	std::unordered_map<GraphicsBufferKey, GraphicsBuffer*> mMap;
};