//
// Created by Priyanshu Singh on 19/06/16.
//

#include "CoinSystem.hpp"

USING_NS_CC;

bool CoinSystem::init() {
    if(!Node::init())return false;
    score = 0;
    isMagnet = true;
    return true;
}

void CoinSystem::update(const Player *player, float delta) {
    Node::update(delta);
    auto pts = player->getCollisionRect();
    if(isMagnet){
        for(auto coin: coins){
            if(coin->getPosition().distance(player->getPosition()) < settings.magnetRadius){
                float multi = settings.magnetRadius / (coin->getPosition().distance(player->getPosition()) + 1);
                Point pt = (player->getPosition() - coin->getPosition()); pt.normalize();
                coin->setPosition(coin->getPosition() + pt * multi);
            }
        }
    }
    for(auto coin : coins){
        if(coin->isVisible() && coin->collided(pts)){
            coin->setVisible(false);
            score += coin->getValue();
        }
    }
}

void CoinSystem::addCoin(cocos2d::Vec2 point) {

    int i = CCRANDOM_0_1() * 10;if(i==0)i++;
    Coin *coin = Coin::create("Coin/frame-"+std::to_string(i)+".png");
    coin->setPosition(point);
    coin->setType(Coin::CoinType::SMALL_VALUE_COIN);
    auto animation = Animation::create();
    for(int k=1; k<=10; k++){
        animation->addSpriteFrameWithFile("Coin/frame-" + std::to_string(i) + ".png");
        i++;if(i == 11)i=1;
    }
    animation->setLoops(-1);
    animation->setDelayPerUnit(1.0f/24.0f);
    coin->runAction(Animate::create(animation));
    coin->setVisible(true);
    coin->setScale(0.07);
    coins.push_back(coin);
    addChild(coin);
}

void CoinSystem::clearCoins() {
    coins.clear();
    removeAllChildren();
    setPosition(Vec2::ZERO);
    score = 0;
}
