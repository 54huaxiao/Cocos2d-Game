#include "LoginScene.h"
#include "cocostudio/CocoStudio.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "Global.h"
#include "GameScene.h"
#include "HelloWorldScene.h"
#include "Monster.h"
#include <regex>
using std::to_string;
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;
USING_NS_CC;

using namespace cocostudio::timeline;

#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
using namespace  rapidjson;

Scene* LoginScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LoginScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LoginScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    textField = TextField::create("Player Name", "Arial", 30);
    textField->setPosition(Size(visibleWidth / 2, visibleHeight / 4 * 3));
    this->addChild(textField, 2);

    auto button = Button::create();
    button->setTitleText("Login");
    button->setTitleFontSize(30);
    button->setPosition(Size(visibleWidth / 2, visibleHeight / 2));

	button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			HttpRequest* request = new HttpRequest();
			request->setUrl("http://localhost:8080/login");
			request->setRequestType(HttpRequest::Type::POST);
			request->setResponseCallback(CC_CALLBACK_2(LoginScene::onLoginHttpRequestCompleted, this));

			string postData = "username=" + textField->getStringValue();
			const char* temp = postData.c_str();
			request->setRequestData(temp, strlen(temp));
			request->setTag("login POST");
			cocos2d::network::HttpClient::getInstance()->send(request);
			request->release();

			// 创建场景
			if (textField->getStringValue() != "") {
				auto scene = HelloWorld::createScene();
				
				// 让导演用场景替换现在的场景
				Director::getInstance()->replaceScene(scene);
			}
		}
	});

	this->addChild(button, 2);
    return true;
}

void LoginScene::onLoginHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
{
	if (!response)
	{
		return;
	}

	if (0 != strlen(response->getHttpRequest()->getTag()))
	{
		log("%s completed", response->getHttpRequest()->getTag());
	}

	std::vector<char> *header = response->getResponseHeader();
	string temp1 = Global::toString(header);
	log("%s\n", temp1);
	
	Global::gameSessionId = Global::getSessionIdFromHeader(temp1);

	int statusCode = response->getResponseCode();
	log("response code: %d", statusCode);
	if (!response->isSucceed())
	{
		log("response failed");
		log("error buffer: %s", response->getErrorBuffer());
		return;
	}

	std::vector<char> *buffer = response->getResponseData();
	string temp = Global::toString(buffer);
	log("%s\n", temp);
}
