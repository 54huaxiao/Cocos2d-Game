#ifndef __GAME_SCENE1_H__
#define __GAME_SCENE1_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

class GameScene1 : public cocos2d::Layer
{
public:
	void setPhysicsWorld(PhysicsWorld * world);

    static cocos2d::Scene* createScene(int difficulty_send);

    virtual bool init(PhysicsWorld * world, int difficulty_send);
    
    // implement the "static create()" method manually
	static GameScene1* create(PhysicsWorld * world, int difficulty_send);

private:
	PhysicsWorld * MyWorld;

	Size visibleSize;
	Vec2 origin;

	Sprite* player;
	Sprite* jumpLength_bg;
	Sprite* jumpLength_inner;
	Sprite* skull_right;
	Sprite* skull_left;
	Sprite* clear_Tool;
	Sprite* blood_Tool;
	Sprite* win_label;
	ProgressTimer* jumpLength_progress;
	Label* diamond_num;

	Vector<SpriteFrame*> run;
	Vector<Sprite*> stone_set;
	Vector<Sprite*> bullet_set;
	Vector<Sprite*> bomb_set;
	Vector<Sprite*> player_blood;
	std::vector<Sprite*> diamond_set;

	// 给边界加上物理边框
	void addBackgroundEdge();
	//加载音乐、音效
	void preloadMusic();
	//播放bgm
	void play_bgm();

	// 头盖骨发射石头
	void skull_fire_stone(float dist, char direc);
	// enemyA开火
	void enemyAfire(float verti_height, int direc);
	// enemyB出现
	void enemyBout(float X_pos, char op);
	// 清屏
	void ClearAllAttack();
	// 被攻击击中
	void BeingAttacked();
	void Quit(Ref*, char cid);
	void addKeyboardListener();
	void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);

	//---------------------
	//   调度器回调函数
	//---------------------
	void updateSomething(float dt);
	void skullFire(float dt);
	void createDiamond(float dt);
	void createEnemy(float dt);
	void bombExplosion(float dt);
	void VictoryTime(float dt);

	int DIFFICULTY;

	float jump_time;
	char lastkey;
	int diamond_index;
	int get_diamond_num;
	int leftBlood;
	bool jumpFlag;
	bool leftMoveFlag, rightMoveFlag;
	bool diamond_in_scene;
	bool clearTool, bloodTool;
	bool victory_flag;
};

#endif // __GAME_SCENE1_H__
