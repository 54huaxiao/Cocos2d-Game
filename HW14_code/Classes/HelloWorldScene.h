#pragma once
#include "cocos2d.h"
#include<sstream>
using namespace cocos2d;
class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
	void moveEvent(Ref*, char);
	void actionEvent(Ref*, char);
	void stopAc();
	// ���·�������ʾ
	void updateScore(int);
	// �������һ������
	void createMonster(float);
	// �Ƴ�����
	void moveMonster(float);
	// ����ɫ�Ƿ�������������Ŷ���
	void hitByMonster(float);
	// ��ɫ����
	int attackMonster();
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
	//void update(float dt) override;
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* score;
	int killNum;
	float HPbarPercentage;
	cocos2d::ProgressTimer* pT;
};
