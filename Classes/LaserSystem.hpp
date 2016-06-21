//
// Created by ashish on 6/21/16.
//

#ifndef MYGAME_LASERNODE_HPP
#define MYGAME_LASERNODE_HPP


#include "cocos2d.h"
class Player;
class LaserSystem :public cocos2d::Node{
public:
    static LaserSystem * create(Player * player,const std::function<void()> & parentCallBack,float height,float duration,float speed);
    void update(float delta)override;
    float getSpeed(){
        return speed;
    }


    bool collided();
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
    void move();
protected:
    bool init(Player * player,const std::function<void()> & parentCallBack,float height,float duration,float speed);



private:
    std::function<void()> parentCallBack;
    float duration;
    float timer = 0.0f;
    float speed;
    float height;
//    cocos2d::Sprite * leftEnd = nullptr;
//    cocos2d::Sprite * rightEnd = nullptr;

    cocos2d::DrawNode * laserDrawer = nullptr;
    Player *player = nullptr;


};


#endif //MYGAME_LASERNODE_HPP
