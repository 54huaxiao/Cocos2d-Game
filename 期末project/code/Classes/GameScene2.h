#pragma once

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GameScene3.h"
USING_NS_CC;

class GameScene2 :public Layer {
public:
	void setPhysicsWorld(PhysicsWorld * world);
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init(PhysicsWorld* world);

    // implement the "static create()" method manually
	static GameScene2* create(PhysicsWorld* world);

private:
    //“Ù¿÷º”‘ÿ”Î≤•∑≈
    Sprite* player;
	PhysicsWorld* m_world;
	Size visibleSize;
	float HPbarPercentage;
	bool victory_flag;
	cocos2d::ProgressTimer* pT;
	std::vector<PhysicsBody*> enemys;
	Sprite* boom;
	Sprite* win_label;
	Sprite* wall1;
	Sprite* wall2;

    void preloadMusic();
    void playBgm();

    void addBackground();
    void addEdge();
	void addPlayer();

	void addContactListener();
	void addTouchListener();
	void addKeyboardListener();

	void update(float f);
	void Quit(Ref*, char cid);
	bool onConcactBegan(PhysicsContact& contact);
    bool onTouchBegan(Touch *touch, Event *unused_event);
    void onTouchMoved(Touch *touch, Event *unused_event);
    void onTouchEnded(Touch *touch, Event *unused_event);
    void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode code, Event* event);

	void newEnemys();
	void addEnemy(int type, Point p);

	cocos2d::Label* time;
	cocos2d::Vec2 origin;
	int dtime;
	void updateTime(float);
	void Boom(PhysicsBody* m);
	void addboom();
	void addwall1();
	void addwall2();
};