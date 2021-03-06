//
// Created by ashish on 6/1/16.
//

#ifndef MYGAME_PLAYER_HPP
#define MYGAME_PLAYER_HPP

#include "cocos2d.h"

class Player:public cocos2d::Node{
public:
    CREATE_FUNC(Player);
    bool init()override;
    void update(float delta)override;
    cocos2d::Vec2 getNextPosition();
    void setNextPosition(const cocos2d::Vec2 & nextPosition);
    void setPosition(const cocos2d::Vec2 & position)override;



    std::vector<cocos2d::Vec2> getCollisionRect() const{
        using cocos2d::Vec4;
        using cocos2d::Vec2;
        std::vector<Vec2> points;
        auto size =displaySprite->getTextureRect().size*settings.boxFactor*displaySprite->getScale();

        Vec4 topLeft = getNodeToParentTransform()*Vec4(-size.width/2, size.height/2,0,1);
        Vec4 topRight= getNodeToParentTransform()*Vec4(size.width/2, size.height/2,0,1);
        Vec4 bottomRight = getNodeToParentTransform()*Vec4(size.width/2, -size.height/2,0,1);
        Vec4 bottomLeft = getNodeToParentTransform()*Vec4(-size.width/2,-size.height/2,0,1);

        points.push_back(Vec2(topLeft.x,topLeft.y));
        points.push_back(Vec2(topRight.x,topRight.y));
        points.push_back(Vec2(bottomRight.x,bottomRight.y));
        points.push_back(Vec2(bottomLeft.x,bottomLeft.y));
        return points;


    }


    cocos2d::Vec2  getDisplacement() const;

    void setCollided(bool val);

    void setRandomColor();

private:

    cocos2d::Sprite * displaySprite = nullptr;
    cocos2d::Vec2 nextPosition;
    struct PlayerSettings {
        float rotationDelta = 4;
        //TODO make sure displacement delta is always less than a particular value so no tunelling with player
        float displacementDelta = 4;
        float maxRotation = 180*100;
        //can increase or decrease this factor
        float boxFactor = 0.2;


    };
    PlayerSettings settings;

    void tilt(float dt);
    void move(float dt);

    enum class Movement{
        Normal,
        Left,
        Right

    };
    Movement orientation = Movement::Normal;


    cocos2d::Vec2 disVec;



    //powerups
    enum Power{
        SpeedFast,
        SpeedSlow,
        None,
        TotalPowerUps,

    };

    struct speedFast{
        float getFactor(float delta){
            elapsed+=delta;

            const float half = time/2;
            float factor = elapsed <= half?elapsed:(time-elapsed);

            factor/=time;
            factor*=2;
            return exp(factor);

        }
        float getElapsed(){
            return elapsed;
        }
        float time = 0.4f;
        private:
            float elapsed = 0.0f;

    };
    speedFast sf;

    struct speedSlow{
        float factor = 0.5f;
    };
    speedSlow ss;


    void enablePowerUp(const Power & power);
    void disablePowerUp(const Power & power);
    bool powerBool[TotalPowerUps];


};


#endif //MYGAME_PLAYER_HPP
