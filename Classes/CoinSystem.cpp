//
// Created by Priyanshu Singh on 19/06/16.
//

#include "CoinSystem.hpp"

USING_NS_CC;

bool CoinSystem::init() {
    if(!Node::init())return false;
    score = 0;
    return true;
}

void CoinSystem::update(const Player *player, float delta) {
    Node::update(delta);
    auto pts = player->getCollisionRect();

    for(auto coin : coins){
        if(coin->isVisible() && coin->collided(pts)){
            coin->setVisible(false);
            score += coin->getValue();
        }
    }
}

void CoinSystem::addCoin(cocos2d::Vec2 point) {
    Coin *coin = Coin::create("Coin/frame-1.png");
    coin->setPosition(point);
    coin->setType(Coin::CoinType::SMALL_VALUE_COIN);
    auto animation = Animation::create();
    for(int i=1; i<=10; i++){
        animation->addSpriteFrameWithFile("Coin/frame-" + std::to_string(i) + ".png");
    }
    animation->setLoops(-1);
    coin->runAction(Animate::create(animation));
    coin->setVisible(true);
    coin->setScale(0.05);
    coins.push_back(coin);
    addChild(coin);
}

void CoinSystem::clearCoins() {
    coins.clear();
    removeAllChildren();
    setPosition(Vec2::ZERO);
    score = 0;
}
