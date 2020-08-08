#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "olcConsoleGameEngine.h"

class OneLoneCoder_Asteroids : public olcConsoleGameEngine
{
public:
    OneLoneCoder_Asteroids()
    {
        m_sAppName = L"Asteroids";
    }
private:
    struct sSpaceObject
    {
        float x;
        float y;
        float dx;
        float dy;
        int nSize;
        float angle;
    };

    vector<sSpaceObject> vecAsteroids;
    vector<sSpaceObject> vecBullets;
    sSpaceObject player;
    bool bDead = false;
    int nScore = 0;

    vector<pair<float, float>> vecModelShip;
    vector<pair<float, float>> vecModelAsteroid;

protected:
    // Called by olcConsoleGameEngine
    virtual bool OnUserCreate()
    {
        vecModelShip =
        {
            { 0.0f, -5.0f },
            { -2.5f, +3.5f },
            { +2.5f, +3.5f},
            { 0.0f, -5.0f },
            { +2.5f, +3.5f},
            { +2.5f, +5.5f},
            { 0.0f, -5.0f },
            { -2.5f, +3.5f },
            { -2.5f, +5.5f},
        };

        //Asteroid verts
        int verts = 20;
        for (int i = 0; i < verts; i++)
        {
            float radius = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
            //angle
            float a = ((float)i / (float)verts) * 6.28318f; // 2 pi
            vecModelAsteroid.push_back(make_pair(radius * sinf(a), radius * cosf(a)));
        }

        resetGame();

        return true;
    }

    bool isPointInsideCircle(float cx, float cy, float radius, float x, float y)
    {
        // this isn't good for large numbers of bullets and asteroids but game is small enough to be ok
        return sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy)) < radius;
    }

    void resetGame()
    {
        vecAsteroids.clear();
        vecBullets.clear();
        //x    //y    //dx  //dy   //size  //angle
        vecAsteroids.push_back({ 20.0f, 20.0f, 8.0f, -6.0f, (int)16, 0.0f });
        vecAsteroids.push_back({ 100.0f, 20.0f, -5.0f, 3.0f, (int)16, 0.0f });

        // Initialise player position
        player.x = ScreenWidth() / 2.0f;
        player.y = ScreenHeight() / 2.0f;
        player.dx = 0.0f;
        player.dy = 0.0f;
        player.angle = 0.0f;

        bDead = false;
    }

    virtual bool OnUserUpdate(float fElapsedTime)
    {
        if (bDead)
            resetGame();

        // Clear Screen
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);

        // ============================================================
        // Game Logic
        // ============================================================

        //-------------
        //Player input
        //-------------

        // ROTATE(steer) Player (store new angle value)
        if (m_keys[VK_LEFT].bHeld)
            player.angle -= 5.0f * fElapsedTime;
        if (m_keys[VK_RIGHT].bHeld)
            player.angle += 5.0f * fElapsedTime;

        // Thrust (update VELOCITY in direction of angle using sin and cos for x and y)
        if (m_keys[VK_UP].bHeld)
        {
            // ACCELERATION changes VELOCITY with respect to time
            player.dx += sin(player.angle) * 20.0f * fElapsedTime;
            player.dy += -cos(player.angle) * 20.0f * fElapsedTime; // negative because graph(screen) is upside down
        }

        if (m_keys[VK_SPACE].bReleased)
            vecBullets.push_back({player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 0, 0 });

        //--------------------
        //Player logic/output
        //--------------------

        // VELOCITY changes POSITION with respect to time
        player.x += player.dx * fElapsedTime;
        player.y += player.dy * fElapsedTime;

        // Keep ship in gamespace
        wrapCoordinates(player.x, player.y, player.x, player.y);

        for (auto& a : vecAsteroids)
            if (isPointInsideCircle(a.x, a.y, a.nSize, player.x, player.y))
                bDead = true; //game over man...

        // ============================================================
        // Drawing
        // ============================================================

        //------------------
        //DRAWING asteroids
        //------------------

        // UPDATE and DRAW Asteroids | a = asteroid
        for (auto& a : vecAsteroids)
        {
            a.x += a.dx * fElapsedTime;
            a.y += a.dy * fElapsedTime;
            a.angle += 0.5f * fElapsedTime;
            wrapCoordinates(a.x, a.y, a.x, a.y);
            DrawWireFrameModel(vecModelAsteroid, a.x, a.y, a.angle, a.nSize, FG_YELLOW);
        }

        vector<sSpaceObject> newAsteroids;

        //------------------
        //DRAWING Bullets
        //------------------

        // DRAW Bullets
        for (auto& b : vecBullets)
        {
            b.x += b.dx * fElapsedTime;
            b.y += b.dy * fElapsedTime;
            wrapCoordinates(b.x, b.y, b.x, b.y);

            Draw(b.x, b.y);

            // Check for collision with asteroids
            for (auto& a : vecAsteroids)
            {
                if (isPointInsideCircle(a.x, a.y, a.nSize, b.x, b.y))
                {
                    // Asteroid hit
                    b.x = -100; //temporary

                    if (a.nSize > 4)
                    {
                        // Create two child asteroids
                        float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
                        float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
                        newAsteroids.push_back({ a.x, a.y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), (int)a.nSize >> 1, 0.0f });
                        newAsteroids.push_back({ a.x, a.y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), (int)a.nSize >> 1, 0.0f });
                    }

                    a.x = -100;
                    nScore += 100;
                }
            }
        }

        // Append new asteroids to existing vector
        for (auto a : newAsteroids)
            vecAsteroids.push_back(a);

        // Remove bullet when off screen
        if (vecBullets.size() > 0)
        {
            //What is this magic?????
            auto i = remove_if(vecBullets.begin(), vecBullets.end(), 
                [&](sSpaceObject o) {
                    return (o.x < 1 || o.y < 1 || o.x >= ScreenWidth() - 1 || o.y >= ScreenHeight() - 1); 
                }
            );
            if (i != vecBullets.end())
                vecBullets.erase(i);
        }

        // Remove asteroid when hit by bullet
        if (vecAsteroids.size() > 0)
        {
            auto i = remove_if(vecAsteroids.begin(), vecAsteroids.end(), 
                [&](sSpaceObject o) { 
                    return (o.x < 0); 
                }
            );
            if (i != vecAsteroids.end())
                vecAsteroids.erase(i);
        }

        if (vecAsteroids.empty())
        {
            // Level cleared
            nScore += 1000;
            vecAsteroids.clear();
            vecBullets.clear();

            // Add two new asteroids, but not where the player is
            // add them perpindicular to the right and left of the player
            // their coordinates will be wrapped by the next asteroid update
            // if placed outside of bounds
            vecAsteroids.push_back({ 30.0f * sinf(player.angle - 3.14159f / 2.0f),
                                    30.0f * cosf(player.angle - 3.14159f / 2.0f),
                                    10.0f * sinf(player.angle),
                                    10.0f * cosf(player.angle),
                                    (int)16, 0.0f });

            vecAsteroids.push_back({ 30.0f * sinf(player.angle + 3.14159f / 2.0f),
                                    30.0f * cosf(player.angle + 3.14159f / 2.0f),
                                    10.0f * sinf(-player.angle),
                                    10.0f * cosf(-player.angle),
                                    (int)16, 0.0f });
        }

        //-----------------
        //DRAWING the ship
        //-----------------

        DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle);
        
        //DRAWING score
        DrawString (2, 2, L"SCORE: " + to_wstring(nScore));

        return true;
    }

    void DrawWireFrameModel(const vector<pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE) {
        // pair.first = x coordinate
        // pair.second = y coordinate

        // Create translated model vector of coordinate pairs (original model stays unchanged)
        vector<pair<float, float>> vecTransformedCoordinates;
        int verts = vecModelCoordinates.size();
        vecTransformedCoordinates.resize(verts);

        // ROTATE the model
        //        __              __  __   __
        //  P2x = | cos02 (-sin02) |  | P1x |
        //  p2y = | sin02 cos0 2   |  | P1y |
        //        --              --  --   --
        for (int i = 0; i < verts; i++)
        {
            vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
            vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
        }

        //SCALE the model
        // Scale
        for (int i = 0; i < verts; i++)
        {
            vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
            vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
        }

        //TRANSLATE model to player position
        for (int i = 0; i < verts; i++)
        {
            vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
            vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
        }

        //DRAW closed polygon (ship model)
        for (int i = 0; i < verts + 1; i++)
        {
            int j = (i + 1);
            DrawLine(vecTransformedCoordinates[i % verts].first, vecTransformedCoordinates[i % verts].second,
                vecTransformedCoordinates[j % verts].first, vecTransformedCoordinates[j % verts].second, PIXEL_SOLID, col);
        }


    }

    void wrapCoordinates(float ix, float iy, float& ox, float& oy)
    {
        ox = ix;
        oy = iy;
        
        if (ix < 0.0) ox = ix + (float)ScreenWidth();                       // if beyond top border, move to bottom
        if (ix >= (float)ScreenWidth()) ox = ix - (float)ScreenWidth();     // if beyond bottom border, move to top
        if (iy < 0.0) oy = iy + (float)ScreenHeight();                      // if beyond left border, move to right
        if (iy >= (float)ScreenHeight()) oy = iy - (float)ScreenHeight();   // if beyond right border, move to left
    }

    // overide
    virtual void Draw(int x, int y, wchar_t c = 0x2588, short col = 0x000F)
    {
        float fx, fy;
        wrapCoordinates( x, y, fx, fy);
        olcConsoleGameEngine::Draw(fx, fy, c, col);
    }
};


int main()
{
    // Use olcConnsoleGameEngine derived app
    OneLoneCoder_Asteroids game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();
}