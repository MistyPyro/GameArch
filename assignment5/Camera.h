#pragma once
#include <Trackable.h>
#include "Vector2D.h"
#include <string>

class FileManager;

class Camera : public Trackable
{
    friend class FileManager;
public:
    static Camera* createInstance();
    static Camera* getInstance() { return mpsInstance; };
    static void destroyInstance();

    void init(unsigned int screenW, unsigned int screenH, float worldW, float worldH);
    void update(Vector2D playerPos, double dt);
    Vector2D getOffset() const { return mOffset; }
    Vector2D worldToScreen(Vector2D worldPos) const
    {
        return Vector2D(worldPos.getX() - mOffset.getX(), worldPos.getY() - mOffset.getY());
    }

    float getScrollX() const { return mOffset.getX(); }

private:
    static Camera* mpsInstance;
    Camera() {}
    ~Camera() {}

    Vector2D mOffset;    

    unsigned int mScreenW = 800;
    unsigned int mScreenH = 600;
    float mWorldW = 3200; 
    float mWorldH = 600;

    inline void setLerpSpeed(float speed) { LERP_SPEED = speed; };
    inline void setDeadZoneX(float deadZone) { DEAD_ZONE_X = deadZone; };

    const std::string DATA_FILE = "CameraData.txt";

    // THESE ARE DATA DRIVEN - CHANGE FILE VALUES NOT THESE | values here are fallback if file cannot be read
    float LERP_SPEED = 6.0f; 
    float DEAD_ZONE_X = 100.0f;
};
