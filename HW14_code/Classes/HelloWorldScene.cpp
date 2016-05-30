#include "HelloWorldScene.h"
#include "Monster.h"
#include "GameScene.h"
#pragma execution_character_set("utf-8")
USING_NS_CC;

#define database UserDefault::getInstance()


Scene* HelloWorld::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = HelloWorld::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	// 添加背景
	TMXTiledMap* tmx = TMXTiledMap::create("map.tmx");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScaleX(visibleSize.width / tmx->getContentSize().width);
	tmx->setScaleY(visibleSize.height / tmx->getContentSize().height);
	this->addChild(tmx, 0);

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setTag(01);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/arial.ttf";
	ttfConfig.fontSize = 36;

	// 每隔3秒 生成一个怪物，移动所有怪物，检测是否碰撞
	schedule(schedule_selector(HelloWorld::createMonster), 3.0f, kRepeatForever, 0);
	schedule(schedule_selector(HelloWorld::moveMonster), 3.0f, kRepeatForever, 0);
	schedule(schedule_selector(HelloWorld::hitByMonster), 3.0f, kRepeatForever, 0);

	// 计分
	score = Label::createWithTTF(ttfConfig, "Score: 0");
	score->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - score->getContentSize().height));
	addChild(score, 4);
	// 初始化击杀数量
	killNum = 0;

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	HPbarPercentage = 100;
	pT->setPercentage(HPbarPercentage);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 4);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 3);

	//静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	//攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}
	auto _frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	attack.pushBack(_frame);

	//死亡动画
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	auto _frame1 = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 79, 90)));
	dead.pushBack(_frame1);

	//运动动画
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}

	//Label
	auto menuLabel1 = Label::createWithTTF(ttfConfig, "W");
	auto menuLabel2 = Label::createWithTTF(ttfConfig, "S");
	auto menuLabel3 = Label::createWithTTF(ttfConfig, "A");
	auto menuLabel4 = Label::createWithTTF(ttfConfig, "D");
	auto menuLabel6 = Label::createWithTTF(ttfConfig, "Y");
	//menuItem
	auto item1 = MenuItemLabel::create(menuLabel1, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'W'));
	auto item2 = MenuItemLabel::create(menuLabel2, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'S'));
	auto item3 = MenuItemLabel::create(menuLabel3, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'A'));
	auto item4 = MenuItemLabel::create(menuLabel4, CC_CALLBACK_1(HelloWorld::moveEvent, this, 'D'));
	auto item5 = MenuItemLabel::create(menuLabel6, CC_CALLBACK_1(HelloWorld::actionEvent, this, 'Y'));

	item3->setPosition(Vec2(origin.x + item3->getContentSize().width, origin.y + item3->getContentSize().height));
	item4->setPosition(Vec2(item3->getPosition().x + 3 * item4->getContentSize().width, item3->getPosition().y));
	item2->setPosition(Vec2(item3->getPosition().x + 1.5*item2->getContentSize().width, item3->getPosition().y));
	item1->setPosition(Vec2(item2->getPosition().x, item2->getPosition().y + item1->getContentSize().height));
	item5->setPosition(Vec2(origin.x + visibleSize.width - 2 * item5->getContentSize().width, item1->getPosition().y));

	auto menu = Menu::create(item1, item2, item3, item4, item5, NULL);
	menu->setPosition(Vec2(0, 0));
	addChild(menu, 4);

	return true;
}


void HelloWorld::moveEvent(Ref*, char cid) {
	static char lastMove = 'D';
	switch (cid) {
	case 'W':
	{
		auto child_player = getChildByTag(01);
		Point player_pos = child_player->getPosition();
		auto animation = Animation::createWithSpriteFrames(run, 0.1f);
		auto animate = Animate::create(animation);
		if (player_pos.y < visibleSize.height - 30) {
			auto moveto = MoveTo::create(0.1f, player_pos + Point(0, 30));
			auto _animation = Spawn::create(animate, moveto, NULL);
			player->runAction(_animation);
		}
		else {
			player->runAction(animate);
		}
		break;
	}
	case 'A':
	{
		if (lastMove == 'D') {
			player->setFlipX(true);
		}
		lastMove = 'A';
		auto child_player = getChildByTag(01);
		Point player_pos = child_player->getPosition();
		auto animation = Animation::createWithSpriteFrames(run, 0.1f);
		auto animate = Animate::create(animation);
		if (player_pos.x > 30) {
			auto moveto = MoveTo::create(0.1f, player_pos + Point(-30, 0));
			auto _animation = Spawn::create(animate, moveto, NULL);
			player->runAction(_animation);
		}
		else {
			player->runAction(animate);
		}
		break;
	}
	case 'S':
	{
		auto child_player = getChildByTag(01);
		Point player_pos = child_player->getPosition();
		auto animation = Animation::createWithSpriteFrames(run, 0.1f);
		auto animate = Animate::create(animation);
		if (player_pos.y > 30) {
			auto moveto = MoveTo::create(0.1f, player_pos + Point(0, -30));
			auto _animation = Spawn::create(animate, moveto, NULL);
			player->runAction(_animation);
		}
		else {
			player->runAction(animate);
		}
		break;
	}
	case 'D':
	{
		if (lastMove == 'A') {
			player->setFlipX(false);
		}
		lastMove = 'D';
		auto child_player = getChildByTag(01);
		Point player_pos = child_player->getPosition();
		auto animation = Animation::createWithSpriteFrames(run, 0.1f);
		auto animate = Animate::create(animation);
		if (player_pos.x < visibleSize.width - 30) {
			auto moveto = MoveTo::create(0.1f, player_pos + Point(30, 0));
			auto _animation = Spawn::create(animate, moveto, NULL);
			player->runAction(_animation);
		}
		else {
			player->runAction(animate);
		}
		break;
	}
	}
}


void HelloWorld::actionEvent(Ref*, char cid) {
	Animation* actionAnimation = nullptr;
	if (HPbarPercentage == 20) {
		auto scene = GameScene::createScene();

		// 让导演用场景 ☆替换☆ 现在的场景，注意是替换！
		Director::getInstance()->replaceScene(scene);
	}
	switch (cid) {
	case 'X':
		actionAnimation = Animation::createWithSpriteFrames(dead, 0.1f);
		if (HPbarPercentage > 0) { // 扣血
			HPbarPercentage -= 20;
			pT->setPercentage(HPbarPercentage);
		}
		break;
	case 'Y':
		actionAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
		int attack = attackMonster();
		if (attack) {
			if (HPbarPercentage < 100) { // 回血
				HPbarPercentage += 20;
				pT->setPercentage(HPbarPercentage);
			}
			updateScore(attack);
		}

		break;
	}
	auto action = Animate::create(actionAnimation);
	auto idleAnimation = Animation::createWithSpriteFrames(idle, 0.1f);
	auto idelAnimate = Animate::create(idleAnimation);
	auto seq = Sequence::createWithTwoActions(action, idelAnimate); // 播放完动画之后恢复原样
	player->runAction(seq);
}

void HelloWorld::stopAc() {
}

void HelloWorld::updateScore(int win) {
	killNum += win;
	database->setIntegerForKey("killNum", killNum);  // 存入UserDefault

	std::stringstream ss;
	std::string newScore;
	ss << killNum;
	ss >> newScore;
	score->setString("Score: " + newScore);
}

void HelloWorld::createMonster(float) {
	auto fac = Factory::getInstance();
	auto newMonster = fac->createMonster();
	Rect playerRect = player->getBoundingBox();
	Rect monRect;
	float x, y;
	// 确保新生成的怪物不会直接落在player上
	do {
		x = random(origin.x, visibleSize.width);
		y = random(origin.y, visibleSize.height);

		monRect = Rect(x, y,
			newMonster->getContentSize().width, newMonster->getContentSize().height);
	} while (playerRect.intersectsRect(monRect));

	newMonster->setPosition(x, y);
	this->addChild(newMonster, 3);
}

void HelloWorld::moveMonster(float) {
	auto fac = Factory::getInstance();
	fac->moveMonster(player->getPosition(), 1);
}

void HelloWorld::hitByMonster(float) {
	auto fac = Factory::getInstance();
	Vector<Sprite*> collisionMonster = fac->collider(player->getBoundingBox());

	if (!collisionMonster.empty()) {
		actionEvent(this, 'X');
		fac->removeMonster(collisionMonster);
	}
}

int HelloWorld::attackMonster() {
	auto fac = Factory::getInstance();
	Rect playerRect = player->getBoundingBox();
	// 攻击前后方距离30的敌人
	Rect attackRect = Rect(playerRect.getMinX() - 30, playerRect.getMinY(),
		playerRect.size.width + 80, playerRect.size.height);

	Vector<Sprite*> attackedMonster = fac->collider(attackRect);

	if (attackedMonster.empty()) return 0;

	fac->removeMonster(attackedMonster);

	return attackedMonster.size();
}