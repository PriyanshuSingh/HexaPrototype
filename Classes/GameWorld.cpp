#include "GameWorld.hpp"
#include "HexagonGrid.hpp"
USING_NS_CC;

Scene* GameWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    scheduleUpdate();
    Size visibleSize = _director->getVisibleSize();
    Vec2 visibleOrigin = _director->getVisibleOrigin();
    Size winSize = _director->getWinSize();

















    // position the sprite on the center of the screen

//    //BAD HACK COIN
//    coinSystem = CoinSystem::create();
//    addChild(coinSystem);

    grid = HexagonGrid::create(this,230,100,100);
    addChild(grid);
//    grid->setPosition(winSize/3);
    //score Label
    {
        scoreLabel = Label::createWithTTF("0","fonts/arial.ttf", 20);
//        scoreLabel->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        scoreLabel->setPosition(Vec2(winSize.width, winSize.height) + visibleOrigin - Vec2(60, 60));
        addChild(scoreLabel, 2);
    }
    //closing button
    {
        auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",CC_CALLBACK_1(GameWorld::menuCloseCallback, this));

        closeItem->setPosition(Vec2(visibleOrigin.x+visibleSize.width - closeItem->getContentSize().width / 2,
                                    visibleOrigin.y+closeItem->getContentSize().height / 2));
        auto menu = Menu::createWithItem(closeItem);
        menu->setPosition(Vec2::ZERO);
        this->addChild(menu, 1);
    }





    //stats
    {

        stats.push_back(0);
        stats.push_back(0);
        stats.push_back(0);
        stats.push_back(0);
        stats.push_back(0);


    }
    return true;
}


void GameWorld::update(float delta) {

    //non fixed update
//    {
//
//        if (delta >= 0.032) {
//            cocos2d::log("delta is %0.4f",delta);
//        }
//
//        updateConst(delta);
//    }



    //fixed uodate
    {
    accumHolder+=delta;
    if(accumHolder>=nextExpected){

        //display stats
//        cocos2d::log("Stats for second %llu",nextExpected);
        auto sumEle = stats[0]-stats[0];

        for (auto n : stats)
            sumEle += n;

        for(size_t i= 0;i<stats.size();++i){
//            cocos2d::log("%ld happened percentage is %0.2lf",i,((double)stats[i]*100)/sumEle);
            //clear off stats
            stats[i] = 0;

        }


//        cocos2d::log("");
//        cocos2d::log("");
//        cocos2d::log("");

        nextExpected+=debugInterval;
    }



    accumulator+=delta;
    if(gamePaused){
        accumulator = 0;
    }

    const int nSteps = static_cast<int>(std::floor(accumulator/updateSettings.timeStep));
    if (nSteps > 0)
    {
        accumulator -= nSteps * updateSettings.timeStep;
    }

    CCASSERT(accumulator < updateSettings.timeStep + FLT_EPSILON,"Accumulator must have a value lesser than the fixed time step");

    //avoid spiral of death
    const int nStepsClamped = std::min(nSteps,updateSettings.maxSteps);

    stats[nStepsClamped]++;

    for (int i = 0; i < nStepsClamped; ++i)
    {
        updateConst(updateSettings.timeStep);

    }
}


}


void GameWorld::updateConst(float delta) {

    grid->update(delta);
    scoreLabel->setString(std::to_string(grid->coinSystem->getScore()));
}


void GameWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
