#include "HelloWorldScene.h"
#include "GameScene1.h"
#include "Global.h"
#include "cocostudio/CocoStudio.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
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
    if ( !Layer::init() )
    {
        return false;
    }

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	visibleHeight = visibleSize.height;
	visibleWidth = visibleSize.width;

	textField = TextField::create("Player Name", "Arial", 30);
	textField->setPosition(Size(visibleWidth / 2, visibleHeight / 4 * 3));
	this->addChild(textField, 2);

	//初始化难度为 easy
	difficulty_send = 2;

	//------------------------------
	//  以下为4个难度的选择label
	//  点击后label由原来的白色变红色
	//  位置你们可以改一下
	//------------------------------
	difficulty_label[0] = Label::createWithTTF("easy", "fonts/Marker Felt.ttf", 60);
	difficulty_label[1] = Label::createWithTTF("middle", "fonts/Marker Felt.ttf", 60);
	difficulty_label[2] = Label::createWithTTF("hard", "fonts/Marker Felt.ttf", 60);
	difficulty_label[3] = Label::createWithTTF("horrible", "fonts/Marker Felt.ttf", 60);

	MenuItemLabel* difficulty_Button[4];
	difficulty_Button[0] = MenuItemLabel::create(difficulty_label[0], CC_CALLBACK_1(HelloWorld::DifficultyMenuCallback, this, 2));
	difficulty_Button[1] = MenuItemLabel::create(difficulty_label[1], CC_CALLBACK_1(HelloWorld::DifficultyMenuCallback, this, 3));
	difficulty_Button[2] = MenuItemLabel::create(difficulty_label[2], CC_CALLBACK_1(HelloWorld::DifficultyMenuCallback, this, 4));
	difficulty_Button[3] = MenuItemLabel::create(difficulty_label[3], CC_CALLBACK_1(HelloWorld::DifficultyMenuCallback, this, 5));

	difficulty_Button[0]->setPosition(Vec2(200 + origin.x, visibleSize.height / 2 + origin.y - 50));
	difficulty_Button[1]->setPosition(Vec2(350 + origin.x, visibleSize.height / 2 + origin.y - 50));
	difficulty_Button[2]->setPosition(Vec2(500 + origin.x, visibleSize.height / 2 + origin.y - 50));
	difficulty_Button[3]->setPosition(Vec2(700 + origin.x, visibleSize.height / 2 + origin.y - 50));

	Menu* difficulty_menu[4];
	difficulty_menu[0] = Menu::create(difficulty_Button[0], NULL);
	difficulty_menu[1] = Menu::create(difficulty_Button[1], NULL);
	difficulty_menu[2] = Menu::create(difficulty_Button[2], NULL);
	difficulty_menu[3] = Menu::create(difficulty_Button[3], NULL);

	difficulty_menu[0]->setPosition(Vec2::ZERO);
	difficulty_menu[1]->setPosition(Vec2::ZERO);
	difficulty_menu[2]->setPosition(Vec2::ZERO);
	difficulty_menu[3]->setPosition(Vec2::ZERO);

	this->addChild(difficulty_menu[0], 1);
	this->addChild(difficulty_menu[1], 1);
	this->addChild(difficulty_menu[2], 1);
	this->addChild(difficulty_menu[3], 1);

	//start 按钮

	auto button = Button::create();
	button->setTitleText("Login");
	button->setTitleFontSize(30);
	button->setPosition(Size(visibleWidth / 2, visibleHeight / 2));

	button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			HttpRequest* request = new HttpRequest();
			request->setUrl("http://localhost:8080/login");
			request->setRequestType(HttpRequest::Type::POST);
			request->setResponseCallback(CC_CALLBACK_2(HelloWorld::onLoginHttpRequestCompleted, this));

			string postData = "username=" + textField->getStringValue();
			const char* temp = postData.c_str();
			request->setRequestData(temp, strlen(temp));
			request->setTag("login POST");
			cocos2d::network::HttpClient::getInstance()->send(request);
			request->release();

			// 创建场景
			if (textField->getStringValue() != "") {
				auto scene = GameScene1::createScene(difficulty_send);

				// 让导演用场景替换现在的场景
				Director::getInstance()->replaceScene(scene);
			}
		}
	});
	this->addChild(button, 2);

    
    return true;
}


void HelloWorld::DifficultyMenuCallback(Ref* pSender, int difficulty)
{
	int i;
	for (i = 0; i < 4; i++)
		difficulty_label[i]->setColor(Color3B::WHITE);

	difficulty_label[difficulty - 2]->setColor(Color3B::RED);
	difficulty_send = difficulty;
}

void HelloWorld::onLoginHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
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
