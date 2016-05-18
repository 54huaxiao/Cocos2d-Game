#include "Thunder.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;
static int num = 0;

Scene* Thunder::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Thunder::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool Thunder::init() {
    if ( !Layer::init() ) {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();

    auto bgsprite = Sprite::create("background.jpg");
    bgsprite->setPosition(visibleSize / 2);
    this->addChild(bgsprite, 0);

    player = Sprite::create("player.png");
    player->setPosition(visibleSize.width / 2, player->getContentSize().height + 5);
    player->setName("player");
    this->addChild(player, 1);

    addEnemy(5);

    playBgm();

    addTouchListener();
    addKeyboardListener();
    addCustomListener();

    // add schedule
	schedule(schedule_selector(Thunder::update), 0.1f, kRepeatForever, 0);
	schedule(schedule_selector(Thunder::moveto), 2.0f, kRepeatForever, 0);
    return true;
}

void Thunder::meet(EventCustom* event) {
	playEffect();
}

void Thunder::preloadMusic() {
	SimpleAudioEngine::getInstance()->playEffect("music/fire.wav");
}

void Thunder::playBgm() {
	SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

void Thunder::playEffect() {
	SimpleAudioEngine::getInstance()->playEffect("music/explore.wav", false);
}

void Thunder::addEnemy(int n) {
    enemys.resize(n * 3);
    for(unsigned i = 0; i < 3; ++i) {
        char enemyPath[20];
        sprintf(enemyPath, "stone%d.png", 3 - i);
        double width  = (visibleSize.width - 20) / (n + 1.0),
               height = visibleSize.height - (50 * (i + 1));
        for(int j = 0; j < n; ++j) {
            auto enemy = Sprite::create(enemyPath);
            enemy->setAnchorPoint(Vec2(0.5, 0.5));
            enemy->setScale(0.5, 0.5);
            enemy->setPosition(width * (j + 1), height);
            enemys[i * n + j] = enemy;
			addChild(enemy);
        }
    }
}

void Thunder::addTouchListener(){
	auto listener = EventListenerTouchOneByOne::create();//创建一个触摸监听(单点触摸）
	listener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);//指定触摸的回调函数
	listener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
	listener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);//将listener放入事件委托中
}

void Thunder::addKeyboardListener() {
    // add the keyboardlistener to catch the event
	auto KeyBoardListener = EventListenerKeyboard::create();
	KeyBoardListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	KeyBoardListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(KeyBoardListener, player);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(KeyBoardListener->clone(), player);
}

void Thunder::update(float f) {
	player->setPosition(player->getPosition() + Vec2(move, 0));
	static double count = 0;
	static int dir = 1;
	count += f;
	if (count > 1) { count = 0.0; dir = -dir; }

	for (unsigned i = 0; i < bullets.size(); i++) {
		if (bullets[i] != NULL) {
			bullets[i]->setPosition(bullets[i]->getPositionX(), bullets[i]->getPositionY() + 5);
			if (bullets[i]->getPositionY() > visibleSize.height - 10) {
				bullets[i]->removeFromParentAndCleanup(true);
				bullets[i] = NULL;
			}
		}
	}

	for (unsigned i = 0; i < enemys.size(); i++) {
		if (enemys[i] != NULL) {
			enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(dir, 0));
			for (unsigned j = 0; j < bullets.size(); j++) {
				if (bullets[j] != NULL && bullets[j]->getPosition().getDistance(enemys[i]->getPosition()) < 30) {
					EventCustom e("meet");
					e.setUserData(&i);
					_eventDispatcher->dispatchEvent(&e);
					bullets[j]->removeFromParentAndCleanup(true);
					bullets[j] = NULL;
					enemys[i]->removeFromParentAndCleanup(true);
					enemys[i] = NULL;
					break;
				}
			}
		}
	}
}

void Thunder::moveto(float f) {
	for (unsigned i = 0; i < enemys.size(); i++) {
		if (enemys[i] != NULL) {
			enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(0, -5));
			if (player != NULL && player->getPosition().getDistance(enemys[i]->getPosition()) < 30) {
				EventCustom e("meet");
				e.setUserData(&i);
				_eventDispatcher->dispatchEvent(&e);
				player->removeFromParentAndCleanup(true);
				player = NULL;
				unschedule(schedule_selector(Thunder::update));
				unschedule(schedule_selector(Thunder::moveto));
			}
		}
	}
}

void Thunder::fire() {
	if (player != NULL) {
		preloadMusic();
		auto bullet = Sprite::create("bullet.png");
		bullet->setPosition(player->getPosition());
		bullets.push_back(bullet);
		addChild(bullet);
	}
}

void Thunder::addCustomListener() {
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

bool Thunder::onTouchBegan(Touch *touch, Event *unused_event) {
	return true;
}

void Thunder::onTouchMoved(Touch *touch, Event *unused_event) {
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

void Thunder::onTouchEnded(Touch *touch, Event *unused_event) {
	fire();
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
    switch (code) {
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
			player->setPosition(player->getPosition().x - 1, player->getPosition().y);
			move -= 5;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
			player->setPosition(player->getPosition().x + 1, player->getPosition().y);
			move += 5;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
			fire();
			break;
        default:
            break;
    }
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		player->setPosition(player->getPosition().x + 1, player->getPosition().y);
		move += 5;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		player->setPosition(player->getPosition().x - 1, player->getPosition().y);
		move -= 5;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		break;
	default:
		break;
	}
}
