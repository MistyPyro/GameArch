#pragma once
#include <Trackable.h>
#include <string>

class GraphicsBuffer;
class Camera;

class ScrollingBackground : public Trackable
{
    friend class FileManager;
public:
    static ScrollingBackground* createInstance();
    static ScrollingBackground* getInstance() { return mpsInstance; };
    static void destroyInstance();

    void init(GraphicsBuffer* pFarBg, GraphicsBuffer* pMidBg, GraphicsBuffer* pNearBg, unsigned int screenW, unsigned int screenH);
    void cleanup();
    void draw(float camaeraScrollX);
    void setFarBackground(GraphicsBuffer* pBuf) { mpFarBg = pBuf; }

private:
    static ScrollingBackground* mpsInstance;
    ScrollingBackground() {}
    ~ScrollingBackground() { cleanup(); }

    void drawLayer(GraphicsBuffer* pBuf, float scrollX, float parallaxFactor, unsigned int screenW, unsigned int screenH);

    GraphicsBuffer* mpFarBg = nullptr;
    GraphicsBuffer* mpMidBg = nullptr;
    GraphicsBuffer* mpNearBg = nullptr;
    unsigned int mScreenW = 800;
    unsigned int mScreenH = 600;

    inline void setFarParallax(float parallax) { FAR_PARALLAX = parallax; };
    inline void setMidParallax(float parallax) { MID_PARALLAX = parallax; };
    inline void setNearParallax(float parallax) { NEAR_PARALLAX = parallax; };

    const std::string DATA_FILE = "ScrollingBackgroundData.txt";

    // THESE ARE DATA DRIVEN values here are fallback if file cannot be read
    float FAR_PARALLAX = 0.2f;
    float MID_PARALLAX = 0.5f;
    float NEAR_PARALLAX = 0.8f;

};