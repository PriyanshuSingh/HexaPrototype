//
// Created by Priyanshu Singh on 19/06/16.
//

#ifndef HEXAPROTOTYPE_COINSYSTEM_HPP
#define HEXAPROTOTYPE_COINSYSTEM_HPP

#include "cocos2d.h"
#include "Player.hpp"

class Coin;
class CoinSystem: public cocos2d::Node {

public:
    int getScore() const {
        return score;
    }

    void update(const Player* player, float delta);

    void addCoin(cocos2d::Vec2 point);
    CREATE_FUNC(CoinSystem);

    void clearCoins();

protected:
    virtual bool init() override;
    std::vector<Coin *> coins;
    int score;
};


class Coin: public cocos2d::Sprite{
public:
    enum CoinType{
        SMALL_VALUE_COIN,
        BIG_VALUE_COIN
    };

    static Coin* create(const std::string &filename){
        auto h = new(std::nothrow)Coin();
        if(h && h->initWithFile(filename)){
            h->autorelease();
            return h;
        }
        CC_SAFE_DELETE(h);
        return nullptr;
    }
    bool collided(std::vector<cocos2d::Vec2> pRect) {
        auto rec = getBoundingBox();
        for (auto p: pRect)if (rec.containsPoint(p))return true;
        return false;
    }
    void setType(CoinType type){
        this->type = type;
    }
    int getValue(){
        switch (type){
            case CoinType::SMALL_VALUE_COIN:return settings.smallCoinValue;
            case CoinType::BIG_VALUE_COIN:return settings.largeCoinValue;
        }
        CCLOG("Unknown Coin Type!!!");
        return 0;
    }

private:
    CoinType type;
    struct CoinSettings{
        const int smallCoinValue = 10;
        const int largeCoinValue = 30;
    };
    CoinSettings settings;

};


#endif //HEXAPROTOTYPE_COINSYSTEM_HPP
