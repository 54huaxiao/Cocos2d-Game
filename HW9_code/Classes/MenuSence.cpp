#include "MenuSence.h"
#include "GameSence.h"
USING_NS_CC;

Scene* MenuSence::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MenuSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuSence::init()
{

    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg_sky = Sprite::create("menu-background-sky.jpg");
	bg_sky->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + 150));
	this->addChild(bg_sky, 0);

	auto bg = Sprite::create("menu-background.png");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(bg, 0);

	auto miner = Sprite::create("menu-miner.png");
	miner->setPosition(Vec2(150 + origin.x, visibleSize.height / 2 + origin.y - 60));
	this->addChild(miner, 1);

	auto leg = Sprite::createWithSpriteFrameName("miner-leg-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	leg->runAction(RepeatForever::create(legAnimate));
	leg->setPosition(110 + origin.x, origin.y + 102);
	this->addChild(leg, 1);
	
	auto title = Sprite::create("gold-miner-text.png");
	title->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 170));
	this->addChild(title, 1);

	auto base = Sprite::create("menu-start-gold.png");
	base->setPosition(Vec2(visibleSize.width / 2 + 290, visibleSize.height / 2 - 230));
	this->addChild(base, 1);

	auto start = MenuItemImage::create(
										"start-0.png",
										"start-1.png",
										CC_CALLBACK_1(MenuSence::startMenuCallback, this));
	//auto start = Sprite::create("start-0.png");
	auto menu = Menu::create(start, NULL);
	menu->setPosition(Vec2(visibleSize.width / 2 + 300, visibleSize.height / 2 - 190));
	this->addChild(menu, 2);

	/*auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch * t, Event * e) {
		Director::getInstance()->replaceScene(GameSence::createScene());
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, start);
    return true;*/
}

void MenuSence::startMenuCallback(Ref* pSender) {
	// Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, start);
	// Director::getInstance()->replaceScene(GameSence::createScene());
	TransitionScene* reScene = NULL;
	float t = 0.8f;
	auto sc = GameSence::createScene();
	reScene = TransitionProgressRadialCCW::create(t, sc);
	Director::sharedDirector()->replaceScene(reScene);

}


