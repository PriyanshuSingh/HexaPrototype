//
// Created by ashish on 6/21/16.
//

#include "LaserSystem.hpp"
#include "Player.hpp"
USING_NS_CC;


//TODO bad dependency(only required line segment class here)
#include "HexagonGrid.hpp"

LaserSystem *LaserSystem::create(Player * player,const std::function<void()> & parentCallBack,float height,float duration, float speed) {

    auto ln = new(std::nothrow)LaserSystem;
    if(ln!= nullptr && ln->init(player,parentCallBack,height,duration,speed)){

        ln->autorelease();
        return ln;
    }
    CC_SAFE_DELETE(ln);
    return nullptr;
}

bool LaserSystem::init(Player * player,const std::function<void()> & parentCallBack,float height,float duration, float speed) {
    if(!Node::init()){
        return false;
    }
    this->duration = duration;
    this->player = player;
    this->speed = speed;
    this->height = height;
    this->parentCallBack = parentCallBack;
    //InVariant:dont move laser drawer relative to laser system
    laserDrawer = DrawNode::create(1);
    addChild(laserDrawer);


//TODO replace update with schedule once
    scheduleUpdate();
    return true;


}


void LaserSystem::update(float delta) {

    if(timer <= duration){

        move();
        timer+=delta;
    }
    else{
        runAction(Sequence::create(CallFunc::create(parentCallBack),RemoveSelf::create(), nullptr));

    }


}


void LaserSystem::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {



    laserDrawer->clear();
    auto & width = _director->getWinSize().width;
    auto first  = laserDrawer->convertToNodeSpace(Vec2::ZERO);
    auto second = laserDrawer->convertToNodeSpace(Vec2(width,0));
    std::vector<Vec2> points;
    points.push_back(Vec2(first.x,0));
    points.push_back(Vec2(second.x,0));
    points.push_back(Vec2(second.x,height));
    points.push_back(Vec2(first.x,height));

    laserDrawer->drawSolidPoly(&points[0],(unsigned int)points.size(),Color4F::RED);




}
void LaserSystem::move(){


    setPositionY(getPositionY()+speed);

}

bool LaserSystem::collided() {


    if(player!= nullptr) {
        auto vec = player->getCollisionRect();
        auto &width = _director->getWinSize().width;
        auto first = laserDrawer->convertToNodeSpace(Vec2::ZERO);
        auto second = laserDrawer->convertToNodeSpace(Vec2(width, 0));
        std::vector<Vec2> points;
        Rect colRect(first.x, 0, second.x - first.x, height);

        for (auto &p:vec) {

            p = laserDrawer->convertToNodeSpace(_parent->convertToWorldSpace(p));
            if (colRect.containsPoint(p))
                return true;

        }
    }
    return false;
}

