//
// Created by ashish on 6/1/16.
//

#include "Player.hpp"
USING_NS_CC;

bool Player::init() {
    if(!Node::init()){
        return false;
    }
    {
        auto listener = EventListenerTouchOneByOne::create();
        listener->onTouchBegan = [this](Touch *touch, Event *unused_event) -> bool {

            auto location = touch->getLocation();

            if (location.x <= _director->getWinSize().width / 2) {
                orientation = Movement::Left;
            }
            else {
                orientation = Movement::Right;

            }
            return true;

        };
        listener->onTouchMoved = [this](Touch *touch, Event *unused_event) {

        };
        listener->onTouchEnded = [this](Touch *touch, Event *unused_event) {

            orientation = Movement::Normal;
        };
        listener->onTouchCancelled = [this](Touch *touch, Event *unused_event) {
            orientation = Movement::Normal;
        };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    {
        auto listener = EventListenerKeyboard::create();
        listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event *event) -> bool{

            if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
                orientation = Movement::Left;
            }
            else if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW){
                orientation = Movement::Right;
            }
            return true;
        };

        listener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event *event) -> bool{
            if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
                orientation = Movement::Normal;
            }
            else if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW){
                orientation = Movement::Normal;
            }
            return true;
        };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }
#endif


    displaySprite = Sprite::create("Triangle.png");
    addChild(displaySprite);

    return true;
}


void Player::update(float delta) {


    tilt();
    move();



}


void Player::setPosition(const cocos2d::Vec2 &position) {
    Node::setPosition(position);
    if (!nextPosition.equals(position)) {
        nextPosition = position;
    }

}

cocos2d::Vec2 Player::getNextPosition() {
    return nextPosition;
}


void Player::setNextPosition(const cocos2d::Vec2 &nextPosition) {
    this->nextPosition = nextPosition;
}

void Player::tilt() {

    switch( orientation ) {
        case Movement::Right:
            setRotation(std::min(getRotation()+settings.rotationDelta,settings.maxRotation));
            break;
        case Movement::Left:
            setRotation(std::max(getRotation()-settings.rotationDelta,-settings.maxRotation));
            break;
        default:
            break;
    }


}


void Player::move() {
    float currentAngle = CC_DEGREES_TO_RADIANS(-1*getRotation()+90);
    disVec = settings.displacementDelta*Vec2(cosf(currentAngle),sinf(currentAngle));
    this->setNextPosition(getPosition()+disVec);

}

Vec2  Player::getDisplacement() {
    return disVec;
}






