#ifndef __MENU_SEBCE_H__
#define __MENU_SEBCE_H__

#include "cocos2d.h"

class MenuSence : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	static bool gg;

    CREATE_FUNC(MenuSence);
};

#endif // __MENU_SEBCE_H__
