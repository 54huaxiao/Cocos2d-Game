#include "GameScene2.h"
#include <string>
using namespace std;
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

void GameScene2::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* GameScene2::createScene() {
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setGravity(Point(0, 0));

    auto layer = GameScene2::create(scene->getPhysicsWorld());

    scene->addChild(layer);
    return scene;
}

bool GameScene2::init(PhysicsWorld* world) {
    if (!Layer::init()) {
        return false;
    }

	this->setPhysicsWorld(world);
    visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

    preloadMusic();
    playBgm();

    addBackground();
    addEdge();

	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/Marker Felt.ttf";
	ttfConfig.fontSize = 44;

	victory_flag = false;

	//初始化生命值
	//hp条
	Sprite* sp0 = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

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
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

    addPlayer();
	addwall1();
	addwall2();

	addContactListener();
    addTouchListener();
    addKeyboardListener();


	//倒计时
	time = Label::createWithTTF("30", "fonts/Marker Felt.TTF", 50);
	schedule(schedule_selector(GameScene2::updateTime), 1.0f, kRepeatForever, 0);
	dtime = 30;
	time->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - time->getContentSize().height));
	addChild(time);

    this->schedule(schedule_selector(GameScene2::update), 1);

    return true;
}

GameScene2 * GameScene2::create(PhysicsWorld * world) {
	GameScene2* pRet = new(std::nothrow) GameScene2();
	if (pRet && pRet->init(world)) {
		pRet->autorelease();
		return pRet;
	}
	delete pRet;
	pRet = NULL;
	return NULL;
}

void GameScene2::preloadMusic() {
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("sound/bgm.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("sound/meet_stone.wav");
}

void GameScene2::playBgm() {
    SimpleAudioEngine::getInstance()->playBackgroundMusic("sound/bgm.mp3", true);
}

void GameScene2::addBackground() {
    auto bgsprite = Sprite::create("black_hole_bg0.jpg");
    bgsprite->setPosition(visibleSize / 2);
    bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, visibleSize.height / bgsprite->getContentSize().height);
    this->addChild(bgsprite, 0);

    auto ps = ParticleSystemQuad::create("black_hole.plist");
    ps->setPosition(visibleSize / 2);
    this->addChild(ps);
}

void GameScene2::addEdge() {
    auto edgeSp = Sprite::create();
    auto boundBody = PhysicsBody::createEdgeBox(visibleSize);
	boundBody->setDynamic(false);
    boundBody->setTag(0);
    edgeSp->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
    edgeSp->setPhysicsBody(boundBody);
    this->addChild(edgeSp);
}

void GameScene2::addPlayer() {
    player = Sprite::create("player.png");
    player->setAnchorPoint(Vec2(0.5, 0.5));
    player->setPhysicsBody(PhysicsBody::createBox(player->getContentSize()));
    player->setPosition(visibleSize / 2);
    player->getPhysicsBody()->setTag(1);
	player->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
    player->getPhysicsBody()->setAngularVelocityLimit(0);
    addChild(player);
}

void GameScene2::addContactListener() {
	auto touchListener = EventListenerPhysicsContact::create();
	touchListener->onContactBegin = CC_CALLBACK_1(GameScene2::onConcactBegan, this);
	_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
}

void GameScene2::addTouchListener(){
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(GameScene2::onTouchBegan,this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameScene2::onTouchMoved,this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameScene2::onTouchEnded,this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void GameScene2::addKeyboardListener() {
    auto keboardListener = EventListenerKeyboard::create();
    keboardListener->onKeyPressed = CC_CALLBACK_2(GameScene2::onKeyPressed, this);
    keboardListener->onKeyReleased = CC_CALLBACK_2(GameScene2::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keboardListener, this);
}

bool GameScene2::onTouchBegan(Touch *touch, Event *unused_event) {
    Vec2 postion = touch->getLocation();
	player->runAction(MoveTo::create(0.5, Point(postion.x, postion.y)));
    return true;
}

void GameScene2::onTouchMoved(Touch *touch, Event *unused_event) {
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
		}
	}
}

void GameScene2::onTouchEnded(Touch *touch, Event *unused_event) {

}

bool GameScene2::onConcactBegan(PhysicsContact& contact) {
	auto body1 = contact.getShapeA()->getBody();
	auto body2 = contact.getShapeB()->getBody();
	if (body1 && body2) {
		if (body1->getTag() == 1&& body2->getTag() != 2 || body2->getTag() == 1&& body1->getTag() != 2) {
			ParticleExplosion* bao1 = ParticleExplosion::create();
			bao1->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
			bao1->setPosition(player->getPosition().x, player->getPosition().y);
			bao1->setScale(1.0f);
			addChild(bao1);
			SimpleAudioEngine::getInstance()->playEffect("sound/meet_stone.wav", false, 1.0f, 1.0f, 1.0f);
			if (HPbarPercentage > 0) { // 扣血
				HPbarPercentage -= 25;
				pT->setPercentage(HPbarPercentage);
			}
			if (HPbarPercentage == 0) {
				auto label = Label::createWithTTF("Game Over!!", "fonts/Marker Felt.TTF", 50);
				label->setColor(Color3B(255, 255, 255));
				label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - label->getContentSize().height - 180));
				this->addChild(label, 1);
				unschedule(schedule_selector(GameScene2::update));
				unschedule(schedule_selector(GameScene2::updateTime));
				_eventDispatcher->removeAllEventListeners();
				body1->getNode()->removeFromParentAndCleanup(true);
				body2->getNode()->removeFromParentAndCleanup(true);
			}
		}
		else if ((body1->getTag() == 1 && body2->getTag() == 2)|| (body1->getTag() == 2 && body2->getTag() == 1)) {
			for (unsigned i = 0; i < enemys.size(); i++) {
				Boom(enemys[i]);
			}
			enemys.clear();
			if (body1->getTag() == 1) {
				body2->getNode()->removeFromParentAndCleanup(true);
			}
			else {
				body1->getNode()->removeFromParentAndCleanup(true);
			}
		}
	}
	return true;
}

void GameScene2::update(float f) {
    newEnemys();
}

void GameScene2::newEnemys() {
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

void GameScene2::addEnemy(int type, Point p) {
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
        sprintf(path, "stone.png");
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
    re->getPhysicsBody()->setTag(tag);
	re->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
    if (rand() % 100 < 50) {
        re->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.25));
    }
    else {
        re->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.25));
    }
    re->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
    enemys.push_back(re->getPhysicsBody());
    addChild(re);
}

void GameScene2::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
    switch (code)
    {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
        player->getPhysicsBody()->setVelocity(Point(-200, player->getPhysicsBody()->getVelocity().y));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
        player->getPhysicsBody()->setVelocity(Point(200, player->getPhysicsBody()->getVelocity().y));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_W:
        player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, 200));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_S:
        player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, -200));
        break;
    default:
        break;
    }
}

void GameScene2::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
    switch (code)
    {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_A:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(-200, 0));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_D:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(200, 0));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_W:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, 200));
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    case cocos2d::EventKeyboard::KeyCode::KEY_S:
        player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, -200));
        break;
	case cocos2d::EventKeyboard::KeyCode::KEY_Q:
		this->Quit(this, 'Q');
		break;
    default:
        break;
    }
}

void GameScene2::Boom(PhysicsBody* m) {
	ParticleExplosion* bao1 = ParticleExplosion::create();
	bao1->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
	bao1->setPosition(m->getPosition().x, m->getPosition().y);
	bao1->setScale(1.0f);
	addChild(bao1);
	m->getNode()->removeFromParentAndCleanup(true);
	SimpleAudioEngine::getInstance()->playEffect("sound/meet_stone.wav", false, 1.0f, 1.0f, 1.0f);
}

void GameScene2::updateTime(float dt) {
	if (dtime == 20) {
		addboom();
	}

	if (dtime > 0) {
		dtime--;
		std::stringstream s;
		std::string ss;
		s << dtime;
		s >> ss;
		time->setString(ss);
	}
	else {
		for (unsigned i = 0; i < enemys.size(); i++) {
			Boom(enemys[i]);
		}
		unschedule(schedule_selector(GameScene2::update));
		unschedule(schedule_selector(GameScene2::updateTime));
		_eventDispatcher->removeAllEventListeners();
		win_label = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 610, 320)));
		win_label->setPosition(visibleSize / 2);
		this->addChild(win_label, 3);
		victory_flag = true;
		auto gameEnd = GameScene3::createScene();
		Director::getInstance()->replaceScene(gameEnd);
	}
}

void GameScene2::addboom() {
	boom = Sprite::create("boom.png");
	boom->setPhysicsBody(PhysicsBody::createCircle(boom->getContentSize().height / 2));
	boom->setAnchorPoint(Vec2(0.5, 0.5));
	boom->setScale(0.5, 0.5);
	boom->setPosition(rand() % (int)(visibleSize.width), rand() % (int)(visibleSize.height));
	boom->getPhysicsBody()->setTag(2);
	boom->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	addChild(boom);
}

void GameScene2::addwall1() {
	wall1 = Sprite::create("wall.png");
	wall1->setPhysicsBody(PhysicsBody::createBox(wall1->getContentSize()));
	wall1->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	wall1->setPosition(280,300);
	addChild(wall1);
	static Node* wallBase = Node::create();
	wallBase->setPhysicsBody(PhysicsBody::createCircle(1));
	wallBase->getPhysicsBody()->setDynamic(false);
	wallBase->setPosition(Vec2(280, 250));
	addChild(wallBase);
	PhysicsJointPin* j_point = PhysicsJointPin::construct(wallBase->getPhysicsBody(), wall1->getPhysicsBody(), wallBase->getPosition());
	m_world->addJoint(j_point);
}

void GameScene2::addwall2() {
	wall2 = Sprite::create("wall.png");
	wall2->setPhysicsBody(PhysicsBody::createBox(wall2->getContentSize()));
	wall2->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	wall2->setPosition(660, 300);
	addChild(wall2);
	static Node* wallBase2 = Node::create();
	wallBase2->setPhysicsBody(PhysicsBody::createCircle(1));
	wallBase2->getPhysicsBody()->setDynamic(false);
	wallBase2->setPosition(Vec2(660, 250));
	addChild(wallBase2);
	PhysicsJointPin* j_point2 = PhysicsJointPin::construct(wallBase2->getPhysicsBody(), wall2->getPhysicsBody(), wallBase2->getPosition());
	m_world->addJoint(j_point2);
}

void GameScene2::Quit(Ref*, char cid) {
	if (cid == 'Q') {
		auto gameEnd = GameScene3::createScene();
		Director::getInstance()->replaceScene(gameEnd);
	}
}