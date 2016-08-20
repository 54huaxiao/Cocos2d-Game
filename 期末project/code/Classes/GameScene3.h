#ifndef __GAME_SCENE3_H__
#define __GAME_SCENE3_H__

#include "cocos2d.h"
using namespace cocos2d;
class GameScene3 : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	void addKeyboardListener();
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);
	void HitByMons();
	void gg(bool s);
	void checkHit(cocos2d::Sprite* player);
	void killMon(float dt);
	void moveEvent(Ref*, char);
	void actionEvent(Ref*, char);
	void stopAc();
	void updateTime(float dt);
	void Quit(Ref*, char cid);
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene3);
	//void update(float dt) override;
private:
	cocos2d::Sprite* player;
	int playerStatus;  // 0 refuse damage 1 free 2 busy
	int score = 0;
	double ttt;
	bool on = true;
	cocos2d::Action* movement;
	cocos2d::Vector<SpriteFrame*> dragonBreathe;
	cocos2d::Vector<SpriteFrame*> illusion;
	cocos2d::Vector<SpriteFrame*> soulStone;
	cocos2d::Vector<SpriteFrame*> move;
	cocos2d::Vector<SpriteFrame*> stand;
	//cocos2d::Vector<SpriteFrame*> attack;
	//cocos2d::Vector<SpriteFrame*> dead;
	//cocos2d::Vector<SpriteFrame*> run;
	//cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	int dtime;
	cocos2d::ProgressTimer* pT;
	cocos2d::ProgressTimer* pT1;
};

#endif // __HELLOWORLD_SCENE_H__
