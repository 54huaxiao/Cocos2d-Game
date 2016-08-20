#include "GameScene3.h"
#include "SimpleAudioEngine.h"
#include "MenuSence.h"
#include "cstdlib"
#include "Monster.h"
#include "UpLoad.h"

#define database UserDefault::getInstance()

#pragma execution_character_set("utf-8")

USING_NS_CC;

Scene* GameScene3::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameScene3::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}


void GameScene3::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {  // vs 居然崩溃了
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		this->moveEvent(this, 'W');
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
	case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		this->moveEvent(this, 'S');
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		this->moveEvent(this, 'A');
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		this->moveEvent(this, 'D');
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_J:
		this->actionEvent(this, 'J');
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_K:
		this->actionEvent(this, 'K');
		break;
	default:
		break;
	}
}
void GameScene3::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		this->moveEvent(this, 'W');
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
	case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		this->moveEvent(this, 'S');
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		this->moveEvent(this, 'A');
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		this->actionEvent(this, 'D');
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_J:
		this->actionEvent(this, 'J');
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_K:
		this->moveEvent(this, 'K');
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_Q:
		this->Quit(this, 'Q');
		break;
	default:
		break;
	}
}
void GameScene3::addKeyboardListener() {
	auto keboardListener = EventListenerKeyboard::create();
	keboardListener->onKeyPressed = CC_CALLBACK_2(GameScene3::onKeyPressed, this);
	keboardListener->onKeyReleased = CC_CALLBACK_2(GameScene3::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keboardListener, this);
}

// on "init" you need to initialize your instance
bool GameScene3::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	this->playerStatus = 1;

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	TMXTiledMap* bg = TMXTiledMap::create("map.tmx");
	bg->setPosition(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);
	bg->setAnchorPoint(Vec2(0.5, 0.5));
	bg->setScale(Director::getInstance()->getContentScaleFactor());
	this->addChild(bg, 0);


	addKeyboardListener();
	
	char buf[21];
	auto Mir = Sprite::create("Mir/stand.0.png");
	stand.reserve(12);
	std::string prepath = "Mir/";
	for (int i = 0; i <= 11; i++) {
		std::string path = prepath + "stand."+itoa(i, buf, 10) + ".png";
		auto frame = SpriteFrame::create(path, cocos2d::Rect(0, 0, 202, 202));
		// frame->setOffsetInPixels(Vec2(202-97, -6));
		stand.pushBack(frame);
	}
	
	auto animation = Animation::createWithSpriteFrames(stand, 0.15f);
	auto animate = Animate::create(animation);
	Mir->runAction(RepeatForever::create(animate));

	Mir->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(Mir, 3);
	// Mir->setAnchorPoint(Point(0.5, 0.5));  // 97/202, -6/202));
	player = Mir;  // later on Dragon Master will be added;

	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/arial.ttf";
	ttfConfig.fontSize = 36;
	
	//游戏逝去时间
	dtime = 0;
	score = database->getIntegerForKey("score");

	//计分
	time = Label::createWithTTF(ttfConfig, itoa(score, buf, 10));
	//倒计时周期性调用调度器
	schedule(schedule_selector(GameScene3::updateTime), 0, kRepeatForever, 0);
	//倒计时的数字
	time->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - time->getContentSize().height));
	addChild(time, 7);

	//mp条
	Sprite* sp0 = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用mp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x+14*pT->getContentSize().width,origin.y + visibleSize.height - 3*pT->getContentSize().height));
	addChild(pT,8);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0,7);

	Sprite* sp1 = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp2 = Sprite::create("photo.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT1 = ProgressTimer::create(sp2);
	pT1->setScaleX(90);
	pT1->setAnchorPoint(Vec2(0, 0));
	pT1->setType(ProgressTimerType::BAR);
	pT1->setBarChangeRate(Point(1, 0));
	pT1->setMidpoint(Point(0, 1));
	pT1->setPercentage(100);
	pT1->setPosition(Vec2(origin.x + 14 * pT1->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT1, 8);
	sp1->setAnchorPoint(Vec2(0, 0));
	sp1->setPosition(Vec2(origin.x + pT1->getContentSize().width, origin.y + visibleSize.height - sp1->getContentSize().height));
	addChild(sp1, 7);
	illusion.reserve(7);
	for (int i = 0; i < 7; i++) {
		std::string path = prepath + "illusion." + itoa(i, buf, 10) + ".png";
		auto frame = SpriteFrame::create(path, cocos2d::Rect(0, 0, 583, 547));
		frame->setOffsetInPixels(Vec2(220, -40));
		illusion.pushBack(frame);
	}

	//dragonBreathe
	dragonBreathe.reserve(11);
	for (int i = 0; i < 11; i++) {
		std::string path = prepath + "dragonBreathe." + itoa(i, buf, 10) + ".png";
		auto frame = SpriteFrame::create(path, cocos2d::Rect(0, 0, 583, 547));
		frame->setOffsetInPixels(Vec2(0, -244));
		dragonBreathe.pushBack(frame);
	}

	//soulStone
	soulStone.reserve(14);
	for (int i = 0; i < 14; i++) {
		std::string path = prepath + "soulStone." + itoa(i, buf, 10) + ".png";
		auto frame = SpriteFrame::create(path, cocos2d::Rect(0, 0, 583, 547));
		frame->setOffsetInPixels(Vec2(252, -139));
		soulStone.pushBack(frame);
	}

	auto menuLabel1 = Label::createWithTTF(ttfConfig, "W");
	auto menuLabel2 = Label::createWithTTF(ttfConfig, "S");
	auto menuLabel3 = Label::createWithTTF(ttfConfig, "A");
	auto menuLabel4 = Label::createWithTTF(ttfConfig, "D");
	auto menuLabel5 = Label::createWithTTF(ttfConfig, "J");  // atk
	auto menuLabel6 = Label::createWithTTF(ttfConfig, "K");  // ss
	//menuItem
	auto item1 = MenuItemLabel::create(menuLabel1, CC_CALLBACK_1(GameScene3::moveEvent, this,'W'));
	auto item2 = MenuItemLabel::create(menuLabel2, CC_CALLBACK_1(GameScene3::moveEvent, this,'S'));
	auto item3 = MenuItemLabel::create(menuLabel3, CC_CALLBACK_1(GameScene3::moveEvent, this,'A'));
	auto item4 = MenuItemLabel::create(menuLabel4, CC_CALLBACK_1(GameScene3::moveEvent, this,'D'));
	auto item5 = MenuItemLabel::create(menuLabel5, CC_CALLBACK_1(GameScene3::actionEvent, this, 'J'));
	auto item6 = MenuItemLabel::create(menuLabel6, CC_CALLBACK_1(GameScene3::actionEvent, this, 'K'));

	item3->setPosition(Vec2(origin.x+item3->getContentSize().width,origin.y+item3->getContentSize().height));
	item4->setPosition(Vec2(item3->getPosition().x + 3 * item4->getContentSize().width, item3->getPosition().y));
	item2->setPosition(Vec2(item3->getPosition().x + 1.5*item2->getContentSize().width, item3->getPosition().y));
	item1->setPosition(Vec2(item2->getPosition().x, item2->getPosition().y + item1->getContentSize().height));
	item5->setPosition(Vec2(origin.x+visibleSize.width-item5->getContentSize().width,item1->getPosition().y));
	item6->setPosition(Vec2(item5->getPosition().x-item6->getContentSize().width,item3->getPosition().y));
	
	auto menu = Menu::create(item1, item2, item3, item4, item5, item6, NULL);
	menu->setPosition(Vec2(0, 0));
	addChild(menu, 7);
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("Bgm00.img.RestNPeace.mp3", true);
	return true;
}

void GameScene3::moveEvent(Ref*,char cid) {
	if (this->playerStatus == 1) {
		this->playerStatus = 2;
		visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 p(player->getPosition());
		switch (cid) {
		case 'W':
		{
			p.y += 20;
			if (p.y + 50 > visibleSize.height) p.y = visibleSize.height - 50;
		}
			break;
		case 'A':
		{
			if (player->isFlippedX()) {
				player->setFlippedX(false);  
				player->setPosition(player->getPosition()+Vec2(70, 0));
				p.x += 50;
			} else {
				p.x -= 20;
				if (p.x - 120 < 0) p.x = 120;
			}
		}
			break;
		case 'S':
		{
			p.y -= 20;
			if (p.y - 50 < 0) p.y = 50;
		}
			break;
		case 'D':
		{
			if (player->isFlippedX()) {
				p.x += 20;
				if (p.x + 120 > visibleSize.width) p.x = visibleSize.width - 120;
			} else {
				player->setPosition(player->getPosition() - Vec2(70, 0));
				player->setFlippedX(true);
				p.x -= 50;
			}
		}
			break;
		}
		movement = MoveTo::create(0.2, p);
		player->runAction(movement);
		playerStatus = 1;
	}
}

void GameScene3::actionEvent(Ref*, char cid) {
	if (this->playerStatus == 1) {
		this->playerStatus = 0;
		float mp = this->pT->getPercentage();
		switch (cid) {
		case 'J':
		{
			if (mp >= 30) {
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("/Skill.img/22151001.Use.mp3", false, (1.0F), (0.0F), (0.1F));
				this->pT->runAction(ProgressTo::create(0.1, mp - 20));
				auto animation = Animation::createWithSpriteFrames(this->dragonBreathe, 0.09f);
				auto animate = Animate::create(animation);
				this->player->stopAllActions();
				schedule(schedule_selector(GameScene3::killMon), 0.09f, 7, 0.06);
				Action* skill = Sequence::create(animate, CallFunc::create([=]() {
					this->playerStatus = 1;
					auto animation2 = Animation::createWithSpriteFrames(this->stand, 0.15f);
					auto animate2 = Animate::create(animation2);
					this->player->runAction(RepeatForever::create(animate2));
				}), nullptr);
				this->player->runAction(skill);
				// this->player->runAction(animate);
			} else {
				this->playerStatus = 1;
			}
		}
		break;
		case 'K':
		{
			if (mp != -1) {
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("/Skill.img/22181003.Use.mp3");
				this->pT->runAction(ProgressTo::create(0.1, mp - 10));
				auto animation = Animation::createWithSpriteFrames(this->soulStone, 0.12f);
				auto animate = Animate::create(animation);
				this->player->stopAllActions();

				Action* skill = Sequence::create(animate, CallFunc::create([=]() {
					this->playerStatus = 1;
					auto progress2 = ProgressTo::create(0.3, mp + 50);
					this->pT->runAction((progress2));
					auto animation2 = Animation::createWithSpriteFrames(this->stand, 0.15f);
					auto animate2 = Animate::create(animation2);
					this->player->runAction(RepeatForever::create(animate2));
				}), nullptr);
				this->player->runAction(skill);
			} else {
				this->playerStatus = 1;
			}
		}
		break;
		default:
		{
			this->playerStatus = 1;
		}
		}
	}
}

void GameScene3::stopAc() {
}


void GameScene3::updateTime(float dt) {
	if (!on) return;
	float hp = this->pT1->getPercentage();
	if (hp <= 0) gg(false);
	if (score > 30) gg(true);
	ttt += dt;
	this->dtime++;  // dtime is the frame count
	if (dtime % 240 == 0) {  // 4s
		Factory::getInstance()->createMonster(this);
	}
	if (dtime % 60 == 0)
		Factory::getInstance()->moveMonster(this->player->getPosition(), 2.5);
	if (this->playerStatus) {
		checkHit(this->player);
	}
}

void GameScene3::killMon(float dt) {
	Rect a(this->player->getPositionX()-400, this->player->getPositionY() - 70, 320, 80);
	if (this->player->isFlippedX()) {
		a.setRect(this->player->getPositionX(), this->player->getPositionY() - 70, 320, 80);
	}
	int score = database->getIntegerForKey("score");
	Vector<Sprite*> colliped = Factory::getInstance()->collider(a);
	int delta = colliped.size();
	if (colliped.size() == 0) return;
	for (auto it = colliped.begin(); it < colliped.end(); it++) {
		Factory::getInstance()->removeMonster(*it);
	}
	score += delta;
	char buf[20];
	auto progress2 = ProgressTo::create(0.1, this->pT->getPercentage() + 3*delta);
	this->pT->runAction(progress2);
	this->time->setString(itoa(score, buf, 10));
	database->setIntegerForKey("score", score);
}

void GameScene3::checkHit(cocos2d::Sprite* player) {
	Vector<Sprite*> colliped = Factory::getInstance()->collider(player->getBoundingBox());
	if (colliped.empty()) return;
	for (auto it = colliped.begin(); it < colliped.end(); it++) {
		Factory::getInstance()->removeMonster(*it);
	}
	HitByMons();
}

void GameScene3::HitByMons() {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("/Skill.img/22141004.Use.mp3");
	this->playerStatus = 0;

	float hp = this->pT1->getPercentage();
	this->pT1->runAction(ProgressTo::create(0.1, hp - 20));
	auto animation = Animation::createWithSpriteFrames(this->illusion, 0.08f);
	auto animate = Animate::create(animation);
	this->player->stopAllActions();
	Action* skill = Sequence::create(animate, CallFunc::create([=]() {
		this->playerStatus = 1;
		auto animation2 = Animation::createWithSpriteFrames(this->stand, 0.15f);
		auto animate2 = Animate::create(animation2);
		this->player->runAction(RepeatForever::create(animate2));
	}), nullptr);
	this->player->runAction(skill);
}

void GameScene3::gg(bool s) {
	on = false;
	MenuSence::gg = s;
	if (s == true) {
		auto gameEnd = GameScene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(2, gameEnd, Color3B(255, 0, 0)));
	}
	else {
		auto gameEnd = MenuSence::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(2, gameEnd, Color3B(255, 0, 0)));
	}
}

void GameScene3::Quit(Ref*, char cid) {
	if (cid == 'Q') {
		auto gameEnd = GameScene::createScene();
		Director::getInstance()->replaceScene(gameEnd);
	}
}