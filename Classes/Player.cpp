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

        listener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event *event){
            if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
                orientation = Movement::Normal;
            }
            else if(keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW){
                orientation = Movement::Normal;
            }

        };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }
#endif



    for( int i = 0;i< Power::TotalPowerUps;++i){
        powerBool[i] = false;
    }
    //TODO remove me
    //for testing boost and slow
    {
        auto testListener = EventListenerKeyboard::create();
        testListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode,Event * event){
            if (keyCode == EventKeyboard::KeyCode::KEY_W) {
                enablePowerUp(SpeedFast);

            }
            else if(keyCode == EventKeyboard::KeyCode::KEY_D){
                enablePowerUp(SpeedSlow);

            }
        };
        testListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode,Event * event){

            if(keyCode == EventKeyboard::KeyCode::KEY_D){
                disablePowerUp(SpeedSlow);

            }
        };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(testListener, this);
    }







//    displaySprite = Sprite::create("Triangle.png");
    displaySprite = Sprite::create("AssetSet1/plane1/plane0001.png");
    displaySprite->setScale(0.40f);
    //setting Animation
    auto animation = Animation::create();
    for(int i=1; i<10; i++)animation->addSpriteFrameWithFile("AssetSet1/plane1/plane000"+std::to_string(i)+".png");
    animation->addSpriteFrameWithFile("AssetSet1/plane1/plane0010.png");
    animation->setLoops(-1);
    animation->setDelayPerUnit(1.0f/30.0f);
    displaySprite->runAction(Animate::create(animation));
    //
    setRandomColor();
    addChild(displaySprite, 100);
    auto filterSprite = Sprite::create("AssetSet1/filter1.png");
    filterSprite->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
    addChild(filterSprite);
    return true;
}

void Player::update(float delta) {


    tilt(delta);
    move(delta);



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

void Player::tilt(float dt) {

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


void Player::move(float dt) {
    float currentAngle = CC_DEGREES_TO_RADIANS(-1*getRotation()+90);
    disVec = settings.displacementDelta*Vec2(cosf(currentAngle),sinf(currentAngle));
    for(int i = 0;i<TotalPowerUps;++i) {
       if(powerBool[i]) {
           switch (i){
           case Power::SpeedFast:
               if(powerBool[i]) {

                   disVec *= sf.getFactor(dt);
                   if(sf.getElapsed() >= sf.time){
                       powerBool[i] = false;
                   }
               }
                break;
           case Power::SpeedSlow:
               disVec *= ss.factor;
               break;
           case Power::None:
               break;

           }

        }
    }
    this->setNextPosition(getPosition()+disVec);

}

Vec2  Player::getDisplacement() const{
    return disVec;
}



void Player::enablePowerUp(const Player::Power &power) {
    CCASSERT(!powerBool[power],"power up is already enabled");
    powerBool[power] = true;
    switch(power){
        case Power::SpeedFast:
            sf = speedFast();
            break;
        case Power::SpeedSlow:
            ss = speedSlow();
            break;
    }

}

void Player::disablePowerUp(const Player::Power &power) {
    CCASSERT(powerBool[power],"power up not enabled");
    powerBool[power] = false;


}


void Player::setRandomColor() {
    std::vector<Color3B> col = {Color3B(241,164,40), Color3B(253,190,0), Color3B::RED, Color3B::GRAY};
    displaySprite->setColor(col[rand()%col.size()]);
}
