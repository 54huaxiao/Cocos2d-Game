#include "BreakoutScene.h"
#include <string>
using namespace std;
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

void Breakout::setPhysicsWorld(PhysicsWorld* world) { 

	Size winSize = Director::getInstance()->getWinSize();

	m_world = world;

	rope = Sprite::create("rope.png");
	rope->setPhysicsBody(PhysicsBody::createBox(rope->getContentSize()));
	rope->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	rope->setPosition(winSize.width / 2 + rope->getContentSize().width / 2, winSize.height - 50);
	addChild(rope);

	BigStone = Sprite::create("stone3.png");
	BigStone->setPhysicsBody(PhysicsBody::createCircle(BigStone->getContentSize().width / 2));
	BigStone->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	BigStone->setPosition(winSize.width / 2 + BigStone->getContentSize().width, winSize.height - 50);
	BigStone->getPhysicsBody()->setTag(2);
	addChild(BigStone);

	// 绳子旋转点
	static Node* ropeBase = Node::create();
	ropeBase->setPhysicsBody(PhysicsBody::createCircle(1));
	ropeBase->getPhysicsBody()->setDynamic(false);
	ropeBase->setPosition(Vec2(winSize.width / 2, winSize.height - 50));
	addChild(ropeBase);

	// 连接
	PhysicsJointPin* j_point = PhysicsJointPin::construct(ropeBase->getPhysicsBody(), rope->getPhysicsBody(), ropeBase->getPosition());
	m_world->addJoint(j_point);

	// 陨石与绳子连接
	a_point = PhysicsJointFixed::construct(BigStone->getPhysicsBody(), rope->getPhysicsBody(), BigStone->getPosition());
	m_world->addJoint(a_point);
}

Scene* Breakout::createScene() {
    auto scene = Scene::createWithPhysics();
    // scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    scene->getPhysicsWorld()->setGravity(Point(0, 0));

    auto layer = Breakout::create();
    layer->setPhysicsWorld(scene->getPhysicsWorld());

    scene->addChild(layer);
    return scene;
}

bool Breakout::init() {
    if (!Layer::init()) {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
	cocos2d::Vec2 origin = Director::getInstance()->getVisibleOrigin();
	// no bullet
	count = 0;

    preloadMusic();
    playBgm();
	addBackground();
    addEdge();
    addPlayer();

	addContactListener();
    addTouchListener();
    addKeyboardListener();

	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/arial.ttf";
	ttfConfig.fontSize = 36;

	//倒计时
	time = Label::createWithTTF(ttfConfig, "30");
	//倒计时周期性调用调度器
	schedule(schedule_selector(Breakout::updateTime), 1.0f, kRepeatForever, 0);
	//倒计时的数字
	dtime = 30;
	time->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - time->getContentSize().height));
	addChild(time);

    this->schedule(schedule_selector(Breakout::update), 1);
	this->schedule(schedule_selector(Breakout::addbonus), 5);

    return true;
}

void Breakout::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music/meet_stone.wav");
}

void Breakout::playBgm() {
    SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

void Breakout::playEffect() {
	SimpleAudioEngine::getInstance()->playEffect("music/meet_stone.wav", false);
}

void Breakout::addBackground() {
    auto bgsprite = Sprite::create("black_hole_bg0.jpg");
    bgsprite->setPosition(visibleSize / 2);
    bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, visibleSize.height / bgsprite->getContentSize().height);
    this->addChild(bgsprite, 0);

    auto ps = ParticleSystemQuad::create("black_hole.plist");
    ps->setPosition(visibleSize / 2);
    this->addChild(ps);
}

void Breakout::addEdge() {
    auto edgeSp = Sprite::create();
    auto boundBody = PhysicsBody::createEdgeBox(visibleSize);
	boundBody->setDynamic(false);
    boundBody->setTag(0);
    edgeSp->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
    edgeSp->setPhysicsBody(boundBody);
    this->addChild(edgeSp);
}

void Breakout::addPlayer() {
    player = Sprite::create("player.png");
    player->setAnchorPoint(Vec2(0.5, 0.5));
    player->setPhysicsBody(PhysicsBody::createCircle(35));
	player->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
    player->setPosition(visibleSize / 2);
    player->getPhysicsBody()->setTag(1);
    player->getPhysicsBody()->setAngularVelocityLimit(0);
    addChild(player);
}

void Breakout::addContactListener() {
	auto touchListener = EventListenerPhysicsContact::create();
	touchListener->onContactBegin = CC_CALLBACK_1(Breakout::onConcactBegan, this);
	_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
}

void Breakout::addTouchListener(){
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(Breakout::onTouchBegan,this);
    touchListener->onTouchMoved = CC_CALLBACK_2(Breakout::onTouchMoved,this);
    touchListener->onTouchEnded = CC_CALLBACK_2(Breakout::onTouchEnded,this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void Breakout::addKeyboardListener() {
    auto keboardListener = EventListenerKeyboard::create();
    keboardListener->onKeyPressed = CC_CALLBACK_2(Breakout::onKeyPressed, this);
    keboardListener->onKeyReleased = CC_CALLBACK_2(Breakout::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keboardListener, this);
}

bool Breakout::onTouchBegan(Touch *touch, Event *unused_event) {
    Vec2 postion = touch->getLocation();
    return true;
}

void Breakout::onTouchMoved(Touch *touch, Event *unused_event) {
	auto beginpos = touch->getLocationInView();
	beginpos = Director::getInstance()->convertToGL(beginpos);

	if (player != NULL) {
		auto playerpos = player->getPosition();
		auto playersize = player->getContentSize();

		auto rect = Rect(playerpos.x - playersize.width / 2, playerpos.y - playersize.height / 2, playersize.width + 10, playersize.height + 10);
		if (rect.containsPoint(beginpos)) {
			auto endpos = touch->getPreviousLocationInView();
			endpos = Director::sharedDirector()->convertToGL(endpos);

			auto offset = Point(beginpos - endpos);
			auto nextpos = Point(beginpos + offset);
			player->setPosition(nextpos);
			getbonus();
		}
	}
}

void Breakout::onTouchEnded(Touch *touch, Event *unused_event) {
	// fire();
}

bool Breakout::onConcactBegan(PhysicsContact& contact) {
	auto sp1 = contact.getShapeA()->getBody();
	auto sp2 = contact.getShapeB()->getBody();
	if ((sp2->getTag() == 1 && sp1->getTag() == 10) || (sp2->getTag() == 1 && sp1->getTag() == 20) || (sp2->getTag() == 1 && sp1->getTag() == 30) || (sp2->getTag() == 1 && sp1->getTag() == 2))
	{
		pos = player->getPosition();
		if ((sp2->getTag() == 1 && sp1->getTag() == 2)) {
			sp2->getNode()->removeFromParentAndCleanup(true);
		} else {
			sp1->getNode()->removeFromParentAndCleanup(true);
			sp2->getNode()->removeFromParentAndCleanup(true);
		}
		playEffect();
		unschedule(schedule_selector(Breakout::updateTime));
		unschedule(schedule_selector(Breakout::update));
		unschedule(schedule_selector(Breakout::addbonus));
		_eventDispatcher->removeAllEventListeners();
		time->setString("You Lose!");
		testExternalParticle();
	}
	return true;
}

void Breakout::update(float f) {
    newEnemys();
}

void Breakout::newEnemys() {
	if (enemys.size() > 50) return;
    int newNum = 2;
    while (newNum--) {
        int type = 0;
        if (CCRANDOM_0_1() > 0.85) { type = 2; }
        else if (CCRANDOM_0_1() > 0.6) { type = 1; }

        Point location = Vec2(0, 0);
        switch (rand() % 4)
        {
        case 0:
            location.y = visibleSize.height;
            location.x = rand() % (int)(visibleSize.width);
            break;
        case 1:
            location.x = visibleSize.width;
            location.y = rand() % (int)(visibleSize.height);
            break;
        case 2:
            location.y = 0;
            location.x = rand() % (int)(visibleSize.width);
            break;
        case 3:
            location.x = 0;
            location.y = rand() % (int)(visibleSize.height);
            break;
        default:
            break;
        }
        addEnemy(type, location);
    }
}

void Breakout::addEnemy(int type, Point p) {
    char path[100];
    int tag;
    switch (type)
    {
    case 0:
        sprintf(path, "stone1.png");
        tag = 10;
        break;
    case 1:
        sprintf(path, "stone2.png");
        tag = 20;
        break;
    case 2:
        sprintf(path, "stone3.png");
        tag = 30;
        break;
    default:
        sprintf(path, "stone1.png");
        tag = 10;
        break;
    }
    auto re = Sprite::create(path);
    re->setPhysicsBody(PhysicsBody::createCircle(re->getContentSize().height / 2));
    re->setAnchorPoint(Vec2(0.5, 0.5));
    re->setScale(0.5, 0.5);
    re->setPosition(p);
	re->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
    re->getPhysicsBody()->setTag(tag);
	if (player != NULL) {
		if (rand() % 100 < 50) {
			re->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.25));
		}
		else {
			re->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.25));
		}
	}
    re->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
    enemys.push_back(re->getPhysicsBody());
    addChild(re);
}

void Breakout::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	if (player != NULL) {
		switch (code)
		{
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_A:
			player->getPhysicsBody()->setVelocity(Point(-200, player->getPhysicsBody()->getVelocity().y));
			getbonus();
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_D:
			player->getPhysicsBody()->setVelocity(Point(200, player->getPhysicsBody()->getVelocity().y));
			getbonus();
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_W:
			player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, 200));
			getbonus();
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_S:
			player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, -200));
			getbonus();
			break;
		default:
			break;
		}
	}
}

void Breakout::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	if (player != NULL) {
		switch (code)
		{
		case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_A:
			player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(-200, 0));
			getbonus();
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_D:
			player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(200, 0));
			getbonus();
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_W:
			player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, 200));
			getbonus();
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case cocos2d::EventKeyboard::KeyCode::KEY_S:
			player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, -200));
			getbonus();
			break;
		case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
			fire();
			break;
		default:
			break;
		}
	}
}

void Breakout::testExternalParticle()
{
	auto explode = ParticleSystemQuad::create("explode.plist");
	explode->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
	explode->setPosition(pos);
	explode->setScale(2.0f);
	this->addChild(explode);
}

void Breakout::stopAc() {
	unschedule(schedule_selector(Breakout::updateTime)); // 解除调度器
	unschedule(schedule_selector(Breakout::update));
	unschedule(schedule_selector(Breakout::addbonus));
}

void Breakout::updateTime(float dt) {
	if (dtime > 1) {
		dtime--;
		std::stringstream ss;
		std::string newTime;
		ss << dtime;
		ss >> newTime;
		time->setString(newTime);
	}
	else if (dtime == 1) {
		time->setString("You Win!");
		stopAc();
	}
}

void Breakout::addbonus(float f) {
	Point location = Vec2(0, 0);
	location.x = rand() % (int)(visibleSize.width);
	location.y = rand() % (int)(visibleSize.height);
	auto bonu = Sprite::create("bonus.jpg");
	bonu->setAnchorPoint(Vec2(0.1, 0.1));
	bonu->setScale(0.1, 0.1);
	bonu->setPosition(location);
	bonus.push_back(bonu);
	addChild(bonu);
}

void Breakout::getbonus() {
	auto playerpos = player->getPosition();
	auto playersize = player->getContentSize();
	auto rect = Rect(playerpos.x - playersize.width / 2, playerpos.y - playersize.height / 2, playersize.width + 10, playersize.height + 10);
	for (int i = 0; i < bonus.size(); i++) {
		if (bonus[i] != NULL) {
			if (rect.containsPoint(bonus[i]->getPosition())) {
				CCActionInterval *forwardOut = CCFadeOut::create(0.1f);
				CCActionInterval *backOut = forwardOut->reverse();
				CCAction *actionOut = CCSequence::create(forwardOut, backOut, NULL);
				bonus[i]->runAction(forwardOut);
				bonus[i] = NULL;
				count++;
			}
		}
	}
}

void Breakout::fire() {
	if (player != NULL && count != 0) {
		for (unsigned i = 0; i < enemys.size(); i++) {
			enemys[i]->getNode()->removeFromParentAndCleanup(true);
			ParticleExplosion* explode = ParticleExplosion::create();
			explode->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
			explode->setPosition(player->getPosition());
			explode->setScale(1.0f);
			addChild(explode);
			playEffect();
		}
		count--;
		enemys.erase(enemys.begin(), enemys.end());
	}
}

