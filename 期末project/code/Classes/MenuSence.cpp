#include "MenuSence.h"
#include "UpLoad.h"
USING_NS_CC;

bool MenuSence::gg = false;

Scene* MenuSence::createScene()
{
    auto scene = Scene::create();
    auto layer = MenuSence::create();
    scene->addChild(layer);
    return scene;
}


bool MenuSence::init()
{

    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bgsprite = Sprite::create("black_hole_bg0.jpg");
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);

	auto time = Label::createWithTTF((this->gg ? "Mission Complete" : "Mission Failed"), "fonts/arial.ttf", 36);
	time->setPosition(visibleSize/2);
	addChild(time);

    return true;
}
