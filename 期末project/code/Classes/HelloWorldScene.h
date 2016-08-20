#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "network/HttpClient.h"
#include <string>
#include <vector>
using std::vector;
using std::string;
using namespace cocos2d::ui;
using namespace cocos2d::network;

USING_NS_CC;

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
	void DifficultyMenuCallback(Ref* pSender, int difficulty);
	void onLoginHttpRequestCompleted(cocos2d::network::HttpClient * sender, cocos2d::network::HttpResponse * response);
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

private:
	Label* difficulty_label[4];
	int difficulty_send;
	float visibleHeight;
	float visibleWidth;
	TextField * textField;
};

#endif // __HELLOWORLD_SCENE_H__
