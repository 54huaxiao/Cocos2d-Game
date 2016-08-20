#include"Monster.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Factory* Factory::factory = NULL;
int Factory::maxNum = 12;

Factory::Factory() {
	initSpriteFrame();
}
Factory* Factory::getInstance() {
	if (factory == NULL) {
		factory = new Factory();
	}
	return factory;
}
void Factory::initSpriteFrame(){
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258-48*i,0,42,42)));
		monsterDead.pushBack(frame);
	}
}

Sprite* Factory::createMonster(Layer* scene) {
	if (monster.size() >= maxNum) return monster.at(11);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Sprite* mons = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364,0,42,42)));
	mons->setPosition(random(0, (int)visibleSize.width), random(0, (int)visibleSize.height - 180));
	scene->addChild(mons, 3);
	monster.pushBack(mons);
	return mons;
}

void Factory::removeMonster(Sprite* sp) {
	if (sp == NULL) return;
	sp->stopAllActions();  // ·ÀÖ¹Áé³µÆ¯ÒÆ
	Animation* anim = Animation::createWithSpriteFrames(monsterDead, 0.1f);
	Animate* ani = Animate::create(anim);
	Sequence* seq = Sequence::create(ani, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sp)), NULL);
	sp->runAction(seq);
	monster.eraseObject(sp);
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("/Monster/2150000.Die.mp3");
}
void Factory::moveMonster(Vec2 playerPos,float time){
	for (auto it = monster.begin(); it < monster.end(); it++) {
		Vec2 mosterPos = (*it)->getPosition();
		Vec2 direction = playerPos - mosterPos;
		direction.normalize();
		Vec2 move = direction * 60 + Vec2(random(-5, 5), random(-5, 5));
		if (move.x > 0) (*it)->setFlippedX(true);
		else (*it)->setFlippedX(false);
		(*it)->runAction(MoveBy::create(time, move)); 
	}
}

Vector<Sprite*> Factory::collider(Rect rect) {
	Vector<Sprite*> collided;
	for (auto it = monster.begin(); it < monster.end(); it++) {
		if (rect.containsPoint((*it)->getPosition()))
			collided.pushBack(*it);
	}
	return collided;
}
