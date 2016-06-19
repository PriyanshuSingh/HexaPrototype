#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Player.hpp"
#include "CoinSystem.hpp"


class HexagonGrid;
class GameWorld : public cocos2d::Layer
{
public:
    void setPlayer(Player *player) {
        GameWorld::player = player;
    }

    static cocos2d::Scene* createScene();

    bool init()override;

    void update(float delta)override ;

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameWorld);
//    CoinSystem *coinSystem;
    cocos2d::Label *scoreLabel;

private:
    Player * player = nullptr;
    void updateConst(float delta);
    float accumulator = 0;
    struct UpdateSettings{
        const float timeStep = 1/60.0f;
        const int maxSteps = 5;

    };
    //TODO take care of pause for const update and stuff
    bool gamePaused  = false;
    UpdateSettings updateSettings;

    HexagonGrid *grid = nullptr;


    //TODO just for testing remove this
    std::vector<unsigned long long> stats;
    double accumHolder = 0;
    unsigned long long nextExpected = 1;
    unsigned long long debugInterval = 1;
};

#endif // __HELLOWORLD_SCENE_H__
