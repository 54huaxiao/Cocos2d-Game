#include "GameSence.h"

#pragma execution_character_set("utf-8")
USING_NS_CC;

Scene* GameSence::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameSence::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameSence::init()
{

	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// add layers
	mouseLayer = Layer::create();
	stoneLayer = Layer::create();
	mouseLayer->setAnchorPoint(ccp(0, 0));
	mouseLayer->setPosition(Vec2(0, visibleSize.height / 2));
	stoneLayer->setAnchorPoint(ccp(0, 0));
	stoneLayer->setPosition(Vec2(0, 0));
	this->addChild(mouseLayer, 1);
	this->addChild(stoneLayer, 1);

	// add background
	Sprite* background = Sprite::create("level-background-0.jpg");
	background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	this->addChild(background, 0);

	// add stone
	stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(visibleSize.width / 2 + 100, visibleSize.height / 2 + 150));
	stoneLayer->addChild(stone, 1);

	// add shoot item
	/*auto label = Label::createWithTTF("Shoot", "fonts/arial.ttf", 40);
	label->setPosition(Vec2(visibleSize.width / 2 + 200, visibleSize.height / 2 + 150));
	addChild(label, 1);*/
	MenuItemLabel* button = MenuItemLabel::create(Label::createWithTTF("Shoot", "fonts/Marker Felt.ttf", 32), CC_CALLBACK_1(GameSence::onShootBegan, this));
	button->setPosition(Vec2(visibleSize.width / 2 - 100, -visibleSize.height / 16 + 180));
	auto _menu = Menu::create(button, NULL);
	this->addChild(_menu, 1);

	// add mouse
	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	Animate* mouseAnimation = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimation));
	mouse->setPosition(Vec2(visibleSize.width / 2 - 100, -visibleSize.height / 4 + 150));
	mouseLayer->addChild(mouse, 1);


	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void GameSence::onShootBegan(Ref* ref) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	CCPoint f_pos = mouse->getPosition();
	CCPoint pos = mouseLayer->convertToWorldSpace(f_pos);
	pos = stoneLayer->convertToNodeSpace(pos);
	auto move = MoveTo::create(0.2f, pos);
	stone->runAction(move);

	// the stone fade out
	CCActionInterval *forwardOut = CCFadeOut::create(2.0f);
	CCActionInterval *backOut = forwardOut->reverse();
	CCAction *actionOut = CCSequence::create(forwardOut, backOut, NULL);
	stone->runAction(forwardOut);

	// Sleep(1000);

	// the mouse left the diamond and return to its house
	Sprite* diamond = Sprite::create("diamond.png");
	diamond->setPosition(f_pos.x, f_pos.y + 330);
	this->addChild(diamond, 3);
	move = MoveTo::create(1.7f, Vec2::ZERO);
	mouse->runAction(move);

	// the stone shoot to the mouse and another stone will turn out at the same position
	stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(visibleSize.width / 2 + 100, visibleSize.height / 2 + 150));
	stoneLayer->addChild(stone, 1);
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {

	// auto location = touch->getLocation();
	CCPoint c_pos = mouse->getPosition();
	CCPoint n_pos = touch->getLocation();
	c_pos = mouseLayer->convertToNodeSpace(c_pos);
	n_pos = mouseLayer->convertToNodeSpace(n_pos);

	if (c_pos.x < n_pos.x) {
		mouse->setFlipX(true);
	}
	else {
		mouse->setFlipX(false);
	}
	// touch the screen and a cheese turn out
	Sprite* cheese = Sprite::create("cheese.png");
	cheese->setPosition(Vec2(touch->getLocation().x, touch->getLocation().y));
	this->addChild(cheese, 1);

	// the mouse runs to the cheese
	auto move = MoveTo::create(1.7f, n_pos);
	mouse->runAction(move);

	// when the mouse runs to the cheese, then the mouse eats it and the cheese fade out
	CCActionInterval *forwardOut = CCFadeOut::create(2.0f);
	CCActionInterval *backOut = forwardOut->reverse();
	CCAction *actionOut = CCSequence::create(forwardOut, backOut, NULL);
	cheese->runAction(forwardOut);

    return true;
}
