#include "UpLoad.h"
#include "HelloWorldScene.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <regex>
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;

USING_NS_CC;

cocos2d::Scene* GameScene::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool GameScene::init() {
    if (!Layer::init())
    {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

	int s = rand() % 1000;
	string score;

	std::ostringstream oss;
	oss << s;

	score = oss.str();
    score_field = TextField::create(score, "Arial", 30);
    score_field->setPosition(Size(visibleWidth / 4, visibleHeight / 4 * 3));
    this->addChild(score_field, 2);

    submit_button = Button::create();
    submit_button->setTitleText("Submit");
    submit_button->setTitleFontSize(30);
    submit_button->setPosition(Size(visibleWidth / 4, visibleHeight / 4));
    this->addChild(submit_button, 2);

    rank_field = TextField::create("", "Arial", 30);
    rank_field->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4 * 3));
    this->addChild(rank_field, 2);

    rank_button = Button::create();
    rank_button->setTitleText("Rank");
    rank_button->setTitleFontSize(30);
    rank_button->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4));
    this->addChild(rank_button, 2);

	submit_button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			HttpRequest* request = new HttpRequest();
			request->setUrl("http://localhost:8080/submit");
			request->setRequestType(HttpRequest::Type::POST);
			request->setResponseCallback(CC_CALLBACK_2(GameScene::onLoginHttpRequestCompleted, this));

			string postData = "score=" + score_field->getStringValue();
			const char* temp = postData.c_str();
			request->setRequestData(temp, strlen(temp));

			std::vector<std::string> headers;
			headers.push_back("Cookies: GAMESESSIONID=" + Global::gameSessionId);
			request->setHeaders(headers);

			request->setTag("submit POST");
			cocos2d::network::HttpClient::getInstance()->send(request);
			request->release();
		}
	});

	rank_button->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			HttpRequest* request = new HttpRequest();
			request->setUrl("http://localhost:8080/rank?top=10");
			request->setRequestType(HttpRequest::Type::GET);
			request->setResponseCallback(CC_CALLBACK_2(GameScene::onLoginHttpRequestCompleted, this));

			std::vector<std::string> headers;
			headers.push_back("Cookie: GAMESESSIONID=" + Global::gameSessionId);
			request->setHeaders(headers);

			request->setTag("rank GET");
			cocos2d::network::HttpClient::getInstance()->send(request);
			request->release();
		}
	});

	return true;
}


void GameScene::onLoginHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response)
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
	string t0 = Global::toString(header);
	log("%s\n", t0);

	int statusCode = response->getResponseCode();
	log("response code: %d", statusCode);
	if (!response->isSucceed())
	{
		log("response failed");
		log("error buffer: %s", response->getErrorBuffer());
		return;
	}

	std::vector<char> *buffer = response->getResponseData();
	string t1 = Global::toString(buffer);
	log("%s\n", t1);

	rank_field->setText("");
	for (unsigned i = 23, j = i; i < t1.length(); i++) {
		if (t1[i] == '|') {
			string top = t1.substr(j, i - j);
			rank_field->setText(rank_field->getStringValue() + "\n" + top);
			j = i + 1;
		}
	}
}

