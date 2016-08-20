#include "GameScene1.h"
#include <stdlib.h>
#include "SimpleAudioEngine.h"
#include "GameScene2.h"

using namespace CocosDenshion;

//--------------------------------------------------------------------
//   设置难度：   简单【2】；中等【3】；难【4】；变态难【5】
//   难度体现：   攻击（子弹、陨石）个数；清屏工具大小、出现时间；初始血量多少
//
//	 游戏玩法：   按【A/方向左】【D/方向右】移动，【空格键】跳跃；
//              要躲避陨石、子弹、炸弹的攻击，受攻击则掉血；
//              要收集20颗钻石，收集完见到胜利Logo，靠近则跳转到下一场景。
//--------------------------------------------------------------------
//#define DIFFICULTY 2

USING_NS_CC;

void GameScene1::setPhysicsWorld(PhysicsWorld * world) {
	MyWorld = world;
}

Scene* GameScene1::createScene(int difficulty_send)
{
	auto scene = Scene::createWithPhysics();
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vec2(0, -600.0f));
    
	// 传递PhysicsWorld
    auto layer = GameScene1::create(scene->getPhysicsWorld(), difficulty_send);

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool GameScene1::init(PhysicsWorld * world, int difficulty_send)
{
    if ( !Layer::init() )
    {
        return false;
    }

	this->setPhysicsWorld(world);

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	DIFFICULTY = difficulty_send;

	leftMoveFlag = false;
	rightMoveFlag = false;
	jumpFlag = false;
	jump_time = 0.0;
	lastkey = 'D';
	clearTool = true;
	bloodTool = true;
	victory_flag = false;
	leftBlood = 10 - DIFFICULTY;

	addBackgroundEdge();
	preloadMusic();
	play_bgm();

	auto bg_photo = Sprite::create("bg/bg7.jpg");
	bg_photo->setScale(0.8f);
	bg_photo->setPosition(visibleSize / 2);
	this->addChild(bg_photo, 0);

	// 运动动画
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}

	//玩家player
	player = Sprite::createWithSpriteFrame(run.at(0));
	player->setPosition(Vec2(visibleSize.width / 2, 2));
	player->setPhysicsBody(PhysicsBody::createCircle(35));

	player->getPhysicsBody()->setCategoryBitmask(0x03);     //0011
	player->getPhysicsBody()->setCollisionBitmask(0x03);    //0011
	player->getPhysicsBody()->setContactTestBitmask(0x02);  //0010

	this->addChild(player);

	//蓄力条bg
	jumpLength_bg = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(232, 429, 117, 33)));
	jumpLength_bg->setPosition(Vec2(player->getPositionX(), player->getPositionY() + 70));
	jumpLength_bg->setVisible(false);
	this->addChild(jumpLength_bg, 1);

	//蓄力条
	jumpLength_inner = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));
	jumpLength_progress = ProgressTimer::create(jumpLength_inner);
	jumpLength_progress->setScaleX(1);
	jumpLength_progress->setAnchorPoint(Vec2(0, 0));
	jumpLength_progress->setType(ProgressTimerType::BAR);
	jumpLength_progress->setBarChangeRate(Point(1, 0));
	jumpLength_progress->setMidpoint(Point(0, 1));
	jumpLength_progress->setPercentage(100);
	jumpLength_progress->setPosition(Vec2(jumpLength_bg->getPositionX() - 14, jumpLength_bg->getPositionY() - 9));
	jumpLength_progress->setVisible(false);
	addChild(jumpLength_progress, 1);

	//左右头盖骨
	skull_right = Sprite::createWithSpriteFrameName("skull.png");
	skull_left = Sprite::createWithSpriteFrameName("skull.png");
	skull_right->setScale(1.5);
	skull_left->setScale(1.5);
	skull_right->setPosition(Vec2(visibleSize.width - 48, 40));
	skull_right->setRotation(35.0f);
	skull_left->setPosition(Vec2(48, 40));
	skull_left->setRotation(-35.0f);
	skull_left->setFlipX(true);
	this->addChild(skull_right, 1);
	this->addChild(skull_left, 1);

	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/Marker Felt.ttf";
	ttfConfig.fontSize = 44;

	//获得diamond数
	auto diamond_text = Label::createWithTTF(ttfConfig, "Diamond:     / 20");
	diamond_text->setColor(Color3B::WHITE);
	diamond_text->setPosition(Vec2(visibleSize.width - 155, visibleSize.height - 45));
	diamond_num = Label::createWithTTF(ttfConfig, "0");
	diamond_num->setColor(Color3B::WHITE);
	diamond_num->setPosition(Vec2(visibleSize.width - 111, visibleSize.height - 45));
	this->addChild(diamond_text, 2);
	this->addChild(diamond_num, 2);

	//初始化钻石集 diamond_set
	diamond_index = 0;
	get_diamond_num = 0;
	diamond_set.resize(20);
	diamond_in_scene = false;

	//初始化生命值
	auto blood_label = Label::createWithTTF(ttfConfig, "Blood:");
	blood_label->setColor(Color3B::WHITE);
	blood_label->setPosition(Vec2(55, visibleSize.height - 45));
	this->addChild(blood_label, 2);
	for (int i = 0; i < leftBlood; i++) {
		auto blood = Sprite::createWithSpriteFrame(run.at(0));
		blood->setScale(0.6f);
		blood->setPosition(Vec2(blood_label->getPositionX() + (i + 1) * 30 + 50, blood_label->getPositionY()));
		player_blood.pushBack(blood);
		this->addChild(blood, 2);
	}

	//调度器：更新 跳跃蓄力时间、跳跃力度、左右移动、蓄力条的位置、stone拐弯、检测钻石是否被获取、
	//           飞出的子弹清空、检测清屏道具/回血包是否被获取、player是否触碰到stone/bullet/bomb
	schedule(schedule_selector(GameScene1::updateSomething), 0.02f, kRepeatForever, 0);
	//调度器：更新 游戏胜利时刻
	schedule(schedule_selector(GameScene1::VictoryTime), 0.05f, kRepeatForever, 0);
	//调度器：更新 头盖骨喷火
	schedule(schedule_selector(GameScene1::skullFire), 1.0f, kRepeatForever, 0);
	//调度器：更新 产生钻石
	schedule(schedule_selector(GameScene1::createDiamond), 2.0f, kRepeatForever, 0);
	//调度器：更新 产生enemy
	schedule(schedule_selector(GameScene1::createEnemy), 3.0f, kRepeatForever, 0);
	//调度器：更新 炸弹爆炸
	schedule(schedule_selector(GameScene1::bombExplosion), 4.5f, kRepeatForever, 3.0f);

	addKeyboardListener();
    
    return true;
}

GameScene1* GameScene1::create(PhysicsWorld * world, int difficulty_send) {
	GameScene1* pRet = new(std::nothrow) GameScene1();
	if (pRet && pRet->init(world, difficulty_send)) {
		pRet->autorelease();
		return pRet;
	}
	delete pRet;
	pRet = NULL;
	return NULL;
}

void GameScene1::addBackgroundEdge() {
	auto edgeSp = Sprite::create();
	auto boundbody = PhysicsBody::createEdgeBox(visibleSize);
	boundbody->setDynamic(false);

	edgeSp->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	edgeSp->setPhysicsBody(boundbody);

	this->addChild(edgeSp);

}

void GameScene1::preloadMusic() {
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("sound/bgm1.mp3");
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("sound/bgm2.mp3");
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("sound/bgm3.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/beingAttack.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/blood_Tool.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/bomb.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/bullet.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/clear_Tool.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/get_diamond.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/jump.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/stone.wav");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/explore.wav");
}

void GameScene1::play_bgm() {
	SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/bgm1.mp3", true);
}

void GameScene1::addKeyboardListener() {
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene1::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene1::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void GameScene1::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	//-----------------------
	//  跳跃和移动状态并发执行
	//-----------------------

	// 跳跃的情况
	if (player->getPositionY() <= 37 && code == EventKeyboard::KeyCode::KEY_SPACE) {
		jumpFlag = true;
		jumpLength_bg->setVisible(true);
		jumpLength_progress->setVisible(true);
	}

	// 左右移动
	if (code == EventKeyboard::KeyCode::KEY_A || code == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
		leftMoveFlag = true;
		if (lastkey == 'D') {
			lastkey = 'A';
			player->setFlipX(true);
		}
	}
	else if (code == EventKeyboard::KeyCode::KEY_D || code == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
		rightMoveFlag = true;
		if (lastkey == 'A') {
			lastkey = 'D';
			player->setFlipX(false);
		}
	}

	if (code == EventKeyboard::KeyCode::KEY_Q) {
		this->Quit(this, 'Q');
	}
}

void GameScene1::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	if (jumpFlag && code == EventKeyboard::KeyCode::KEY_SPACE) {
		player->getPhysicsBody()->setVelocity(Vec2(0, jump_time * 400.0f + 400.0f));
		SimpleAudioEngine::getInstance()->playEffect("sound/jump.wav");
		jump_time = 0.0;
		jumpFlag = false;
		jumpLength_bg->setVisible(false);
		jumpLength_progress->setVisible(false);
		jumpLength_progress->setScaleX(1);
	}

	if (code == EventKeyboard::KeyCode::KEY_A || code == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
		leftMoveFlag = false;
	}
	else if (code == EventKeyboard::KeyCode::KEY_D || code == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
		rightMoveFlag = false;
	}
}

void GameScene1::updateSomething(float dt) {
	//跳跃蓄力时间、跳跃力度
	if (jumpFlag) {
		if (jump_time <= 1.00f) {
			jump_time += 0.02;
			jumpLength_progress->setScaleX(jump_time * 15);
		}
	}

	//左右移动
	if (leftMoveFlag) {
		player->setPositionX(player->getPositionX() - 150 * dt);
	}
	else if (rightMoveFlag) {
		player->setPositionX(player->getPositionX() + 150 * dt);
	}
	//蓄力条的位置跟着player移动
	jumpLength_bg->setPosition(Vec2(player->getPositionX(), player->getPositionY() + 70));
	jumpLength_progress->setPosition(Vec2(jumpLength_bg->getPositionX() - 14, jumpLength_bg->getPositionY() - 9));

	//stone拐弯
	for (int i = 0; i < stone_set.size(); i++) {
		if (stone_set.at(i)->getPositionX() == stone_set.at(i)->getTag()) {
			if (stone_set.at(i)->getPositionY() <= visibleSize.height + 25) {
				stone_set.at(i)->setPositionY(stone_set.at(i)->getPositionY() + 7);
			}
			else {  //飞出界面的stone
				stone_set.at(i)->removeFromParentAndCleanup(true);
				stone_set.eraseObject(stone_set.at(i), false);
			}
		}
	}

	//检测钻石是否被获取
	if (diamond_in_scene && diamond_index <= 20) {
		if (diamond_set.at(diamond_index - 1)->getPosition().getDistance(player->getPosition()) < 25) {
			SimpleAudioEngine::getInstance()->playEffect("sound/get_diamond.wav");
			get_diamond_num++;
			char num[10];
			sprintf(num, "%d", get_diamond_num);
			diamond_num->setString(num);
			diamond_set.at(diamond_index - 1)->setVisible(false);
			diamond_set.at(diamond_index - 1) = NULL;
			diamond_in_scene = false;

			//获得20个钻石，出现胜利label，清屏，跳转页面
			if (get_diamond_num == 20) {
				win_label = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 610, 320)));
				win_label->setPosition(visibleSize / 2);
				this->addChild(win_label, 3);
				victory_flag = true;
			}
		}
	}

	//飞出的子弹清空
	for (int i = 0; i < bullet_set.size(); i++) {
		if (bullet_set.at(i)->getPositionX() < -40 || bullet_set.at(i)->getPositionX() > visibleSize.width + 40) {
			bullet_set.at(i)->removeFromParentAndCleanup(true);
			bullet_set.eraseObject(bullet_set.at(i), false);
		}
	}

	//检测清屏道具是否被获取
	if (clear_Tool != NULL && clear_Tool->getPosition().getDistance(player->getPosition()) < (75 - DIFFICULTY * 10)) {
		ClearAllAttack();
	}

	//检测回血包是否被获取
	if (blood_Tool != NULL && blood_Tool->getPosition().getDistance(player->getPosition()) < 30) {
		SimpleAudioEngine::getInstance()->playEffect("sound/blood_Tool.mp3");
		int count = 0;
		for (int i = leftBlood; i < player_blood.size(); i++) {
			if (count < 3) {
				player_blood.at(i)->setVisible(true);
				count++;
				leftBlood++;
			}
			else
				break;
		}
		blood_Tool->setVisible(false);
		blood_Tool = NULL;
	}

	//player是否触碰到stone
	for (int i = 0; i < stone_set.size(); i++) {
		if (stone_set.at(i)->getPosition().getDistance(player->getPosition()) < 25) {
			//爆炸效果
			auto hurtExploration = ParticleSun::createWithTotalParticles(150);
			hurtExploration->setPosition(player->getPosition());
			hurtExploration->setDuration(1.5f);
			this->addChild(hurtExploration);

			BeingAttacked();
		}
	}
	//player是否触碰到bullet
	for (int i = 0; i < bullet_set.size(); i++) {
		if (bullet_set.at(i)->getPosition().getDistance(player->getPosition()) < 25) {
			//爆炸效果
			auto hurtExploration = ParticleSun::createWithTotalParticles(150);
			hurtExploration->setPosition(player->getPosition());
			hurtExploration->setDuration(1.5f);
			this->addChild(hurtExploration);

			BeingAttacked();
		}
	}
	//player是否触碰到bomb
	for (int i = 0; i < bomb_set.size(); i++) {
		if (bomb_set.at(i)->getPosition().getDistance(player->getPosition()) < 25) {
			//爆炸效果
			auto hurtExploration = ParticleSun::createWithTotalParticles(150);
			hurtExploration->setPosition(player->getPosition());
			hurtExploration->setDuration(1.5f);
			this->addChild(hurtExploration);

			BeingAttacked();
		}
	}
}

void GameScene1::skullFire(float dt) {
	int timePlus, destPlus;
	float destX;
	timePlus = rand() % 7 + 1;
	if (timePlus == 1 || timePlus == 7) {
		destPlus = rand() % 4 + 1;
		destX = visibleSize.width / 5 * destPlus;
		if (timePlus == 1)
			skull_fire_stone(destX, 'L');
		else
			skull_fire_stone(destX, 'R');
	}
}

void GameScene1::skull_fire_stone(float destX, char direc) {
	for (int i = 0; i < DIFFICULTY; i++) {
		SimpleAudioEngine::getInstance()->playEffect("sound/stone.wav");
		auto stone = Sprite::create("stone.png");
		MoveTo* moveto;

		if (direc == 'L') {
			stone->setPosition(Vec2(skull_left->getPositionX() + 50, skull_left->getPositionY() - 5));
			moveto = MoveTo::create((destX - stone->getPositionX()) * 0.003, Vec2(destX, stone->getPositionY()));
		}
		else {
			stone->setPosition(Vec2(skull_right->getPositionX() - 50, skull_right->getPositionY() - 5));
			moveto = MoveTo::create((stone->getPositionX() - destX) * 0.003, Vec2(destX, stone->getPositionY()));
		}

		stone->setScale(0.5);
		stone->setTag(destX);
		this->addChild(stone, 1);
		stone_set.pushBack(stone);

		auto delayTime = DelayTime::create(i * 0.15f);
		auto sequ = Sequence::create(delayTime, moveto, NULL);
		stone->runAction(sequ);
	}
}

void GameScene1::createDiamond(float dt) {
	int randX, randY;
	float destX, destY;
	randX = rand() % 15 + 1; // 1 - 15
	randY = rand() % 10 + 1; // 1 - 10
	destX = visibleSize.width / 17 * randX;
	destY = visibleSize.height / 2 / 15 * randY + visibleSize.height / 2;

	if (diamond_index == 0) {
		auto diamond = Sprite::create("diamond.png");
		diamond_set.at(diamond_index) = diamond;
		diamond_set.at(diamond_index)->setPosition(Vec2(destX, destY));
		this->addChild(diamond_set.at(diamond_index), 1);
		diamond_index++;
		diamond_in_scene = true;
	}
	else if (diamond_index < 20) {
		if (diamond_set.at(diamond_index - 1) == NULL) {
			auto diamond = Sprite::create("diamond.png");
			diamond_set.at(diamond_index) = diamond;
			diamond_set.at(diamond_index)->setPosition(Vec2(destX, destY));
			this->addChild(diamond_set.at(diamond_index), 1);
			diamond_index++;
			diamond_in_scene = true;
		}
	}
}

void GameScene1::createEnemy(float dt) {
	// enemyB 某时候出现： 4、7、10、13（其一）掉清屏道具、15掉回血包、其余掉炸弹
	if (get_diamond_num == DIFFICULTY * 3 - 2 || 
		get_diamond_num == 15 || 
		get_diamond_num == 6 || get_diamond_num == 9 || get_diamond_num == 16 || 
		get_diamond_num == 17 || get_diamond_num == 18 || get_diamond_num == 19) {
		int X_rand = rand() % 13;
		float X_pos = visibleSize.width / 16 * X_rand + 50;

		auto enemyB = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(860, 130, 100, 100)));
		enemyB->setPosition(Vec2(X_pos, visibleSize.height + 60));
		this->addChild(enemyB, 2);

		auto moveDown = MoveBy::create(0.4f, Vec2(0, -110));
		auto moveUp = MoveBy::create(0.4f, Vec2(0, 110));
		auto delay = DelayTime::create(1.4f);
		auto seq = Sequence::create(moveDown, delay, moveUp, NULL);
		enemyB->runAction(seq);

		char op;
		if (clearTool && get_diamond_num == DIFFICULTY * 3 - 2) {  //清屏clear
			op = 'C';
			clearTool = false;
		}
		else if (bloodTool && get_diamond_num == 15) {  //回血blood
			op = 'B';
			bloodTool = false;
		}
		else {  //放炸弹
			op = 'A';
		}
		enemyBout(X_pos, op);
	}

	// enemyA 永远出现
	int LorR_rand = rand() % 2; // 左0、右1
	int Y_rand = rand() % 10;
	float verti_height = visibleSize.height / 3 * 2 / 13 * Y_rand + visibleSize.height / 3;

	auto enemyA = Sprite::create("enemyA.png");
	MoveBy* moveOut;
	MoveBy* moveIn;

	if (LorR_rand == 0) { //左
		enemyA->setPosition(Vec2(-30, verti_height));
		enemyA->setRotation(90.0f);
		moveOut = MoveBy::create(0.2f, Vec2(65, 0));
		moveIn = MoveBy::create(0.5f, Vec2(-65, 0));
	}
	else {                //右
		enemyA->setPosition(Vec2(visibleSize.width + 30, verti_height));
		enemyA->setRotation(-90.0f);
		moveOut = MoveBy::create(0.2f, Vec2(-65, 0));
		moveIn = MoveBy::create(0.5f, Vec2(65, 0));
	}
	this->addChild(enemyA, 1);
	auto delay = DelayTime::create(1.0f);
	auto seq = Sequence::create(moveOut, delay, moveIn, NULL);
	enemyA->runAction(seq);
	enemyAfire(verti_height, LorR_rand);
}

void GameScene1::enemyAfire(float verti_height, int direc) {
	for (int i = 0; i < DIFFICULTY; i++) {
		SimpleAudioEngine::getInstance()->playEffect("sound/bullet.wav");
		auto bullet = Sprite::create("bullet.png");
		MoveTo* moveto;

		if (direc == 0) { // 左
			bullet->setRotation(90.0f);
			bullet->setPosition(Vec2(80, verti_height));
			moveto = MoveTo::create(2.2f, Vec2(visibleSize.width + 50, bullet->getPositionY()));
		}
		else {            // 右
			bullet->setRotation(-90.0f);
			bullet->setPosition(Vec2(visibleSize.width - 80, verti_height));
			moveto = MoveTo::create(2.2f, Vec2(-50, bullet->getPositionY()));
		}

		this->addChild(bullet, 2);
		bullet_set.pushBack(bullet);

		auto delayTime = DelayTime::create(i * 0.15f);
		auto sequ = Sequence::create(delayTime, moveto, NULL);
		bullet->runAction(sequ);
	}
}

void GameScene1::enemyBout(float X_pos, char op) {
	int height_rand = rand() % 6 + 1;
	auto moveby = MoveBy::create(0.75f + height_rand*0.08, Vec2(0, -180 - height_rand * 50));

	if (op == 'A') {           //放炸弹
		auto bomb = Sprite::createWithSpriteFrameName("TNT.png");
		bomb->setPosition(Vec2(X_pos, visibleSize.height + 40));
		bomb->setScale(0.7f);
		bomb_set.pushBack(bomb);
		this->addChild(bomb, 1);
		bomb->runAction(moveby);
	}
	else if (op == 'B') {      //回血包
		blood_Tool = Sprite::createWithSpriteFrameName("treasure-bag.png");
		blood_Tool->setPosition(Vec2(X_pos, visibleSize.height + 40));
		this->addChild(blood_Tool, 1);
		blood_Tool->runAction(moveby);
	}
	else if (op == 'C') {      //清屏道具
		SimpleAudioEngine::getInstance()->playEffect("sound/clear_Tool.wav");
		clear_Tool = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(620, 0, 242, 245)));
		clear_Tool->setPosition(Vec2(X_pos, visibleSize.height + 40));
		float scale = 0.45f - 0.05f * DIFFICULTY;
		clear_Tool->setScale(scale);
		this->addChild(clear_Tool, 1);
		clear_Tool->runAction(moveby);
	}
}

void GameScene1::bombExplosion(float dt) {
	if (bomb_set.size() > 0) {
		//爆炸
		SimpleAudioEngine::getInstance()->playEffect("sound/bomb.wav");
		auto explosion = ParticleExplosion::createWithTotalParticles(50);
		explosion->setPosition(bomb_set.at(0)->getPosition());
		this->addChild(explosion, 2);

		//判断玩家是否在爆炸周围
		if (bomb_set.at(0)->getPosition().getDistance(player->getPosition()) < 80) {
			BeingAttacked();
		}
		else {
			//清除炸弹
			bomb_set.at(0)->removeFromParentAndCleanup(true);
			bomb_set.eraseObject(bomb_set.at(0), false);
		}
	}
}

void GameScene1::ClearAllAttack() {
	SimpleAudioEngine::getInstance()->playEffect("sound/explore.wav");
	while (stone_set.size() > 0) {
		auto explosion = ParticleExplosion::createWithTotalParticles(50);
		explosion->setPosition(stone_set.at(0)->getPosition());
		this->addChild(explosion, 2);
		stone_set.at(0)->removeFromParentAndCleanup(true);
		stone_set.eraseObject(stone_set.at(0), false);
	}
	while (bullet_set.size() > 0) {
		auto explosion = ParticleExplosion::createWithTotalParticles(50);
		explosion->setPosition(bullet_set.at(0)->getPosition());
		this->addChild(explosion, 2);
		bullet_set.at(0)->removeFromParentAndCleanup(true);
		bullet_set.eraseObject(bullet_set.at(0), false);
	}
	while (bomb_set.size() > 0) {
		auto explosion = ParticleExplosion::createWithTotalParticles(50);
		explosion->setPosition(bomb_set.at(0)->getPosition());
		this->addChild(explosion, 2);
		bomb_set.at(0)->removeFromParentAndCleanup(true);
		bomb_set.eraseObject(bomb_set.at(0), false);
	}
}

void GameScene1::BeingAttacked() {
	SimpleAudioEngine::getInstance()->playEffect("sound/beingAttack.wav");
	//扣血
	if (leftBlood > 0) {
		player_blood.at(leftBlood - 1)->setVisible(false);
		leftBlood--;
		if (leftBlood == 0) {
			//----------------------------------
			// player血量为0，GameOver
			//----------------------------------
			auto label = Label::createWithTTF("Game Over!!", "fonts/Marker Felt.TTF", 50);
			label->setColor(Color3B(255, 255, 255));
			label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - label->getContentSize().height - 180));
			this->addChild(label, 1);
			ClearAllAttack();
			_eventDispatcher->removeAllEventListeners();
			return;
		}
	}

	//清屏
	ClearAllAttack();

	//玩家回到固定位置
	player->setPosition(Vec2(visibleSize.width / 2, 100));
}

void GameScene1::VictoryTime(float dt) {
	if (victory_flag) {
		ClearAllAttack();
		if (win_label != NULL) {
			if (win_label->getPosition().getDistance(player->getPosition()) < 160) {
				auto scene = GameScene2::createScene();

				// 让导演用场景替换现在的场景
				Director::getInstance()->replaceScene(scene);
			}
		}
	}
}

void GameScene1::Quit(Ref*, char cid) {
	if (cid == 'Q') {
		auto gameEnd = GameScene2::createScene();
		Director::getInstance()->replaceScene(gameEnd);
	}
}