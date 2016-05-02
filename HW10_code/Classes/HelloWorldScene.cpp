#include "HelloWorldScene.h"
#pragma execution_character_set("utf-8")
USING_NS_CC;

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
    if ( !Layer::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	//添加背景图片
	auto background = Sprite::create("HelloWorld.png");
	background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
	addChild(background, 1);

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵

	player = Sprite::createWithSpriteFrame(frame0);
	player->setTag(01);
	player->setPosition(Vec2(origin.x+visibleSize.width / 2,
		visibleSize.height / 2 +origin.y));
	addChild(player, 3);


	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/arial.ttf";
	ttfConfig.fontSize = 36;
	
	//倒计时
	time = Label::createWithTTF(ttfConfig, "180");
	//倒计时周期性调用调度器
	schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);
	//倒计时的数字
	dtime = 180;
	time->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - time->getContentSize().height));
	addChild(time);

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
	pT->setPosition(Vec2(origin.x+14*pT->getContentSize().width,origin.y + visibleSize.height - 2*pT->getContentSize().height));
	addChild(pT,1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0,0);

	//静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	//攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113*i,0,113,113)));
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
	auto menuLabel5 = Label::createWithTTF(ttfConfig, "X");
	auto menuLabel6 = Label::createWithTTF(ttfConfig, "Y");
	//menuItem
	auto item1 = MenuItemLabel::create(menuLabel1, CC_CALLBACK_0(HelloWorld::moveEvent, this,'W'));
	auto item2 = MenuItemLabel::create(menuLabel2, CC_CALLBACK_0(HelloWorld::moveEvent, this,'S'));
	auto item3 = MenuItemLabel::create(menuLabel3, CC_CALLBACK_0(HelloWorld::moveEvent, this,'A'));
	auto item4 = MenuItemLabel::create(menuLabel4, CC_CALLBACK_0(HelloWorld::moveEvent, this,'D'));
	auto item5 = MenuItemLabel::create(menuLabel5, CC_CALLBACK_0(HelloWorld::actionEvent, this, 'X'));
	auto item6 = MenuItemLabel::create(menuLabel6, CC_CALLBACK_0(HelloWorld::actionEvent, this, 'Y'));

	item3->setPosition(Vec2(origin.x+item3->getContentSize().width,origin.y+item3->getContentSize().height));
	item4->setPosition(Vec2(item3->getPosition().x + 3 * item4->getContentSize().width, item3->getPosition().y));
	item2->setPosition(Vec2(item3->getPosition().x + 1.5*item2->getContentSize().width, item3->getPosition().y));
	item1->setPosition(Vec2(item2->getPosition().x, item2->getPosition().y + item1->getContentSize().height));
	item5->setPosition(Vec2(origin.x+visibleSize.width-item5->getContentSize().width,item1->getPosition().y));
	item6->setPosition(Vec2(item5->getPosition().x-item6->getContentSize().width,item3->getPosition().y));
	
	auto menu = Menu::create(item1, item2, item3, item4, item5, item6, NULL);
	menu->setPosition(Vec2(0, 0));
	addChild(menu, 1);

	return true;
}


void HelloWorld::moveEvent(char cid) {
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


void HelloWorld::actionEvent(char cid) {
	switch (cid) {
	case 'X':
	{
		if (HPbarPercentage > 0) { // 扣血
			HPbarPercentage -= 20;
			pT->setPercentage(HPbarPercentage);
		}
		auto animation = Animation::createWithSpriteFrames(dead, 0.1f);
		auto animate = Animate::create(animation);
		player->runAction(animate);
		break;
	}
	case 'Y': 
	{
		if (HPbarPercentage < 100) { // 回血
			HPbarPercentage += 20;
			pT->setPercentage(HPbarPercentage);
		}
		auto animation = Animation::createWithSpriteFrames(attack, 0.1f);
		auto animate = Animate::create(animation);
		player->runAction(animate);
		break;
	}
	}
}

void HelloWorld::stopAc() {
	unschedule(schedule_selector(HelloWorld::updateTime)); // 解除调度器
}

void HelloWorld::updateTime(float dt) {
	if (dtime > 1) {
		dtime--;
		std::stringstream ss;
		std::string newTime;
		ss << dtime;
		ss >> newTime;
		time->setString(newTime);
	}
	else if (dtime == 1) {
		time->setString("Time Out!");
		stopAc();
	}
}