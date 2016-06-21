//
// Created by ashish on 6/2/16.
//

#include "HexagonGrid.hpp"
#include "Player.hpp"
#include "GameWorld.hpp"
#include "LaserSystem.hpp"

USING_NS_CC;

HexagonGrid::HexagonGrid(size_t width,size_t height):hexagons(height,std::deque<Hex>(width) ){

}


HexagonGrid *HexagonGrid::create(GameWorld * world,float hexaWidth, size_t width,size_t height) {
    auto h = new(std::nothrow)HexagonGrid(width,height);
    if(h && h->init(world,hexaWidth)){
        h->autorelease();
        return h;
    }
    CC_SAFE_DELETE(h);
    return nullptr;
}

bool HexagonGrid::init(GameWorld * world,float hexaWidth) {
    if(!Node::init()){
        return false;
    }



    //gameWorld
    {
        this->world = world;
    }

    //background
    {
        backGround = Sprite::create("Background.jpg");
        backGround->setAnchorPoint(Vec2::ZERO);
        backGround->setOpacity(backGround->getOpacity()/(GLubyte)4);
        addChild(backGround);
    }

    this->hexaWidth = hexaWidth;

    {
        const float lineWidth = 12;
        hexagonDrawer = DrawNode::create(lineWidth);
        addChild(hexagonDrawer,1);

        dynamicDrawer = DrawNode::create();
        addChild(dynamicDrawer,1);


    }

    float sqrt3 = sqrtf(3.0f);
    float hexaHeight = 1.5f*hexaWidth/(sqrt3);


    //TODO make sure these sprites stay Together{Batching}
    for(size_t i = 0;i< hexagons.size();++i){
        Vec2 initOffset = (i&1)?Vec2(-hexaWidth/2,0):Vec2::ZERO;
        for(size_t j = 0;j< hexagons.at(i).size();++j){
            hexagons[i][j].center = Vec2(j*hexaWidth,i*hexaHeight)+initOffset;

            // Visual shit.. please donot disturb
            CCLOG("Hexawidth = %f, HexaHeight = %fdd",hexaWidth, hexaHeight);
            auto sp = Sprite::create("AssetSet1/hexog.png");
            auto ov = Sprite::create("AssetSet1/hexop.png");
            ov->setPosition(sp->getContentSize().width/2, sp->getContentSize().height/2);
            sp->addChild(ov);
//            auto xx = Sprite::create("AssetSet1/bloack_03.png");
//            xx->setPosition(sp->getContentSize().width/2, sp->getContentSize().height);
//            sp->addChild(xx,1);
            sp->setScale(hexaWidth/sp->getContentSize().width );
            // Visual shit.. end here continue logic

            addChild(sp);

            sp->setPosition(hexagons[i][j].center);

            hexagons[i][j].enterEdge = Hex::FreeEdge::Invalid;
            hexagons[i][j].exitEdge  = Hex::FreeEdge::Invalid;
            hexagons[i][j].index = std::make_pair(i,j);
            hexagons[i][j].width = hexaWidth;


        }
    }
    // CoinSystem
    {
        coinSystem = CoinSystem::create();
        addChild(coinSystem);
    }
    //player
    {
        player = Player::create();
        player->setPosition(hexagons[startIndex.first][startIndex.second].center);
        addChild(player);
        current = startIndex;
        world->setPlayer(player);
    }
    // generatePath add coins on the field so it should be below CoinSystem;
    indices = generatePath(startIndex.second,startIndex.first,pathLength);

    //LaserNode
    {
        //TODO speed based on player displacement delta
        laser = LaserSystem::create(player,[this]{
            laser = nullptr;
        },20,4,4.0f);
        addChild(laser);



    }


    return true;
}


void HexagonGrid::Restart() {


    world->unscheduleUpdate();
//    cocos2d::log("unscheduled");
    //reset stuff here

    coinSystem->clearCoins();
    this->scheduleOnce([this](float delta){
        {
            if(laser!= nullptr)
                laser->removeFromParent();

            laser = LaserSystem::create(player,[this]{
                laser = nullptr;
            },20,4,4.0f);
            addChild(laser);


        }
        world->scheduleUpdate();

//        cocos2d::log("rescheduled");
    },2.0f,"GameResumer");


    setPosition(Vec2::ZERO);


    //player and stuff reset
    {

        float sqrt3 = sqrtf(3.0f);
        float hexaHeight = 1.5f*hexaWidth/(sqrt3);

        for (size_t i = 0; i < hexagons.size(); ++i) {
            Vec2 initOffset = (i & 1) ? Vec2(-hexaWidth / 2, 0) : Vec2::ZERO;
            for (size_t j = 0; j < hexagons.at(i).size(); ++j) {
                hexagons[i][j].center = Vec2(j * hexaWidth, i * hexaHeight) + initOffset;
                hexagons[i][j].enterEdge = Hex::FreeEdge::Invalid;
                hexagons[i][j].exitEdge = Hex::FreeEdge::Invalid;
                hexagons[i][j].index = std::make_pair(i, j);
                hexagons[i][j].width = hexaWidth;

            }
        }
        this->indices = generatePath(startIndex.second, startIndex.first, pathLength);
        current = startIndex;
        player->setPosition(hexagons[startIndex.first][startIndex.second].center);
        player->setRotation(0.0f);



    }

    {
        backGround->setPosition(Vec2::ZERO);

    }


    if(laser!= nullptr){
        laser->removeFromParent();
        laser = nullptr;
    }


    setPosition(Vec2::ZERO);
    {
        pathDirty = true;

    }

}





void HexagonGrid::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) {






    if(pathDirty){
        pathDirty = false;
        hexagonDrawer->clear();
        float radius = hexaWidth/sqrtf(3.0f);
        float theta = CC_DEGREES_TO_RADIANS(210);
        //TODO replace this drawing with line segments in release
        std::vector<Vec2> points(6,Vec2::ZERO);

        for(size_t i =0;i<points.size();++i){
            points[i] = Vec2(radius*cosf(theta),radius*sinf(theta));
            theta-=CC_DEGREES_TO_RADIANS(60);

        }

        std::vector<std::pair<Vec2,Vec2> > greenGuys;

        for(size_t i = 0;i< hexagons.size();++i){
            for(size_t j = 0;j< hexagons.at(i).size();++j){


                //TODO reduce operations here
                for(size_t l = 0;l<6;++l){
                    auto off = hexagons[i][j].center;

                    //TODO remove enter edge and adding the same edge twice
                    if((static_cast<Hex::FreeEdge>(l) == hexagons[i][j].enterEdge) || (static_cast<Hex::FreeEdge>(l) == hexagons[i][j].exitEdge)) {
                        greenGuys.push_back(std::make_pair(Vec2(off+points[l]),Vec2(off+points[(l+ 1) % 6])));
                    }
                    else{

                    }
                }


            }
        }
        for(size_t k= 0;k<greenGuys.size();++k){
            hexagonDrawer->drawLine(greenGuys.at(k).first,greenGuys.at(k).second,Color4F::GREEN);
        }



    }





    dynamicDrawer->clear();
    auto pRect = player->getCollisionRect();
    dynamicDrawer->drawPoly(&pRect[0],pRect.size(),true,Color4F::BLUE);





}

void HexagonGrid::update(float delta) {

    //checks whether player collides
    if(collision() || (laser!= nullptr && laser->collided())){
        player->setVisible(false);
        Restart();
        return;
    }
    else{

        player->setVisible(true);
    }
    //moves player
    player->update(delta);
    coinSystem->update(player, delta);

    setPosition(getPosition() - player->getDisplacement());
    backGround->setPosition(backGround->getPosition()+paraFactor*player->getDisplacement());

    player->setPosition(player->getNextPosition());

}


int getDirection(float val,float * prob){

    int k;
    for(k=0 ;k < 6;++k){
        val-=prob[k];
        if(val<=0){
            break;
        }
    }
    return k;

}
int HexagonGrid::mapper(Hex::FreeEdge edge) {

    switch( edge ) {
        case Hex::FreeEdge::Left:
            return Hex::FreeEdge::Right;
        case Hex::FreeEdge::TopLeft:
            return Hex::FreeEdge::BottomRight;
        case Hex::FreeEdge::TopRight:
            return Hex::FreeEdge::BottomLeft;
        case Hex::FreeEdge::Right:
            return Hex::FreeEdge::Left;
        case Hex::FreeEdge::BottomRight:
            return Hex::FreeEdge::TopLeft;
        case Hex::FreeEdge::BottomLeft:
            return Hex::FreeEdge::TopRight;
        default: {

        }
    }
    return Hex::FreeEdge::Invalid;
}

std::vector<std::pair<int,int> > HexagonGrid::generatePath(size_t startX,size_t startY,size_t pathLength) {
    std::vector<std::pair<int,int > > indices;

    //TODO replace with single vector
    std::vector<std::vector<bool> > mark(hexagons.size(),std::vector<bool>(hexagons.at(0).size()));




    using std::pair;
    using std::make_pair;
    indices.push_back(make_pair(startY,startX));

    mark[startY][startX] = true;

    //TODO add to difficulty controls
    float probs[6] = {0.05,0.45,0.45,0.05,0.0,0.0};
    typedef std::mt19937 MyRng;
    //TODO test seed 2 and 4
    uint32_t seed_cal = time(nullptr);
    MyRng rng;
    rng.seed(seed_cal);

    std::uniform_real_distribution<float > distro(0,1);

    int currentX = startX;
    int currentY = startY;
    hexagons.at(currentY).at(currentX).enterEdge = Hex::FreeEdge::Invalid;

    for(size_t i = 0;i<pathLength-1;++i){
        float randNum = distro(rng);
        auto dir = getDirection(randNum,probs);
        auto hold = getNextIndex(std::make_pair(currentY,currentX),dir);
        while(mark[hold.first][hold.second]){
            randNum = distro(rng);
            dir = getDirection(randNum,probs);
            hold = getNextIndex(std::make_pair(currentY,currentX),dir);
        }
        mark[hold.first][hold.second] = true;

        hexagons[currentY][currentX].exitEdge = static_cast<Hex::FreeEdge>(dir);
//        cocos2d::log("the %d %d guy exit edge is %d",currentY,currentX,dir);


        //ADDING COINS TO THE CENTER
        coinSystem->addCoin(hexagons[hold.first][hold.second].center);

        hexagons[hold.first][hold.second].enterEdge = static_cast<Hex::FreeEdge>(mapper(hexagons[currentY][currentX].exitEdge));
        currentX = hold.second;
        currentY = hold.first;
        indices.push_back(hold);


    }

    hexagons[currentY][currentX].exitEdge = Hex::FreeEdge::Invalid;
    return indices;

}

std::pair<int,int> HexagonGrid::getNextIndex(const std::pair<int,int> & coords,int k) {
    using std::make_pair;

    std::pair<int,int> p = make_pair(0,0);
    switch( k ) {
        case Hex::FreeEdge::Left: {
            p.first = coords.first;
            p.second = coords.second-1;
            break;
        }
        case Hex::FreeEdge::TopLeft: {
            p.first = coords.first+1;
            p.second = coords.second + ((coords.first % 2 != 0) ? -1 : 0);
            break;
        }
        case Hex::FreeEdge::TopRight: {
            p.first = coords.first + 1;
            p.second = coords.second + ((coords.first % 2 != 0) ? 0 : 1);
            break;
        }
        case Hex::FreeEdge::Right: {
            p.first = coords.first;
            p.second = coords.second +1;
            break;
        }
        case Hex::FreeEdge::BottomRight: {
            p.first = coords.first - 1;
            p.second = coords.second + ((coords.first % 2 != 0) ? 0 : 1);
            break;
        }
        case Hex::FreeEdge::BottomLeft: {
            p.first = coords.first - 1;
            p.second = coords.second + ((coords.first % 2 != 0) ? -1 : 0);
            break;
        }
        default: {
            p.first = -1;
            p.second = -1;
            CCASSERT(false,"Cannot reach here");
            return p;
            break;
        }
    }

    p.first = std::min(std::max(0,p.first),(int)hexagons.size()-1);
    p.second = std::min(std::max(0,p.second),(int)hexagons[0].size()-1);

    return p;

}




bool HexagonGrid::collision() {
    auto c = hexagons[current.first][current.second].center;
    float radius = hexaWidth/sqrtf(3.0f);
    float theta = CC_DEGREES_TO_RADIANS(210);
    Vec2 points[6];
    for(size_t i =0;i<6;++i){
        points[i] = Vec2(radius*cosf(theta),radius*sinf(theta));
        theta-=CC_DEGREES_TO_RADIANS(60);

    }

    auto pNext = player->getNextPosition();
    //TODO collision rect returns the current matrix not the expected matrix
    //TODO correct this or leave?
    auto pRect = player->getCollisionRect();

    //TODO correct this for last guy
    auto nextHexa = getNextIndex(current,hexagons[current.first][current.second].exitEdge);

    int i;
    for(i =0 ;i<6;++i) {


        auto seg = LineSegment(c + points[i], c + points[(i + 1) % 6]);
        auto current = seg.intersects(pRect);
        if(current){
            break;
        }

    }

    //no collision
    if(i == 6){
        if(transferring && !hexagons[current.first][current.second].checkInside(pRect)){

            transferring = false;
            current = nextHexa;
        }
        return false;
    }
    //collision with right edge
    else if(i == hexagons[current.first][current.second].exitEdge ){

        transferring = true;
        return false;
    }
    else {
        transferring = false;
        //collision with wrong edge
        return true;
    }
}

const HexagonGrid::Hex & HexagonGrid::getNextHexagon(const Hex & currentHexagon, int dir) {
    auto nextIndex = getNextIndex(currentHexagon.index,dir);
    return hexagons[nextIndex.first][nextIndex.second];
}





