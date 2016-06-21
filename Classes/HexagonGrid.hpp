//
// Created by ashish on 6/2/16.
//

#ifndef MYGAME_HEXAGONGRID_HPP
#define MYGAME_HEXAGONGRID_HPP

#include "cocos2d.h"
#include "CoinSystem.hpp"

class Player;
class GameWorld;
class LaserSystem;
class HexagonGrid:public cocos2d::Node{
public:
    static HexagonGrid* create(GameWorld * world,float hexaWidth,size_t width,size_t height);
    void draw(cocos2d::Renderer * renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

    void update(float delta) override;

    CoinSystem* coinSystem = nullptr;


    struct LineSegment{
        LineSegment(const cocos2d::Vec2 & start,const cocos2d::Vec2 & end):start(start),end(end){}
        cocos2d::Vec2 start;
        cocos2d::Vec2 end;
        int getSign(const cocos2d::Vec2 & point)const{
            float out = point.x*(end.y-start.y)-point.y*(end.x-start.x)+start.y*(end.x-start.x)-start.x*(end.y-start.y);
            //TODO see this test
            if(fabs(out-0) <FLT_EPSILON){
                return 0;
            }
            else if(out > FLT_EPSILON) {
                return 1;

            }
            else{
                return -1;

            }
        }
        bool intersects(const LineSegment & other){

            return (getSign(other.start)!=getSign(other.end)) && (other.getSign(start)!=other.getSign(end));
        }
        //check if the polygon formed by a bunch of points intersect this line
        bool intersects(const std::vector<cocos2d::Vec2> & other){
            std::vector<LineSegment> segments;
            for(size_t i =0;i<other.size();++i){
                segments.push_back(LineSegment(other[i],other[(i+1)%other.size()]));
            }
            bool ans =  false;
            for(size_t i =0;i<segments.size();++i){
                ans = ans || intersects(segments[i]);
            }
            return ans;
        }


    };

protected:
    HexagonGrid(size_t width,size_t height);
    bool init(GameWorld * world,float hexaWidth);




private:
    typedef std::pair<int,int> IndexPair;
    struct Hex{
        cocos2d::Vec2 center;
        float width;
        std::pair<size_t ,size_t >index;
        enum FreeEdge{
            Invalid = -1,
            Left = 0,
            TopLeft = 1,
            TopRight = 2,
            Right = 3,
            BottomRight = 4,
            BottomLeft  = 5
        };
        bool checkInside(const cocos2d::Vec2 & testPoint){

            float v = width/(2*sqrtf(3.0f));

            float halfW = width/2;
            const float tx = std::abs(testPoint.x - center.x);
            const float ty = std::abs(testPoint.y - center.y);
            //bounding box check
            if (tx > halfW || ty > v*2) return false;
            return (2*(v*(halfW)) - (v*tx) - ((halfW)*ty) )>= 0;



        }
        bool checkInside(const std::vector<cocos2d::Vec2> & testPoints) {
            bool ans = true;
            for(size_t i= 0;i<testPoints.size();++i){
                ans = ans & (checkInside(testPoints[i]));
            }
            return ans;

        }
        //TODO see if any use of enter edge or delete it?
        FreeEdge enterEdge = FreeEdge::Invalid;
        FreeEdge exitEdge = FreeEdge::Invalid;

    };
    cocos2d::DrawNode * hexagonDrawer = nullptr;
    cocos2d::DrawNode * dynamicDrawer = nullptr;

    std::deque<std::deque<Hex> >hexagons;
    float hexaWidth;

    std::vector<std::pair<int,int> > generatePath(size_t startX,size_t startY,size_t pathLength);

    std::pair<int,int> getNextIndex(const std::pair<int,int> & coords,int k);

    std::vector<std::pair<int,int> > indices;

    int mapper(Hex::FreeEdge edge);

    Player * player = nullptr;

    bool collision();

    IndexPair current;
    const Hex & getNextHexagon(const Hex & currentHexagon, int dir);

    bool transferring = false;
    float paraFactor = 0.8;

    cocos2d::Sprite *backGround = nullptr;

    void Restart();
    GameWorld * world = nullptr;
    int pathLength = 80;
    IndexPair startIndex = std::make_pair(2,2);
    bool pathDirty = true;
    LaserSystem *laser = nullptr;
};


#endif //MYGAME_HEXAGONGRID_HPP
