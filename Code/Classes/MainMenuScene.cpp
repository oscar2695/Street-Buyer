/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */
#include "MainMenuScene.h"
#include "ConfigScene.h"
#include "LoadScene.h"
#include "json/document.h"
#include "json/prettywriter.h"
#include "json/filestream.h"

#include <cstdio>
using namespace rapidjson;

USING_NS_CC;
Size visibleSize2;
Vec2 origin2;
Menu* menu;
MenuItemImage* boton1;
MenuItemImage* boton2;


Scene* MainMenu::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainMenu::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainMenu::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    visibleSize2 = Director::getInstance()->getVisibleSize();
    origin2 = Director::getInstance()->getVisibleOrigin();


    //Creamos y mostarmos el fondo
    auto background = Sprite::create("fondo_inicio.png");
    background->setPosition(Vec2(visibleSize2.width/2 + origin2.x, visibleSize2.height/2 + origin2.y));
    background->setScale(visibleSize2.width/background->getContentSize().width);
    this->addChild(background,-1,"background");

    //boton nueva partida

    boton1= MenuItemImage::create("boton_nueva_partida_OFF.png","boton_nueva_partida_ON.png",CC_CALLBACK_1(MainMenu::nuevaPartida, this));
    boton1->setPosition(Vec2(origin2.x+visibleSize2.width/2-boton1->getContentSize().width*0.6,origin2.y+visibleSize2.height/2));
    boton1->setScale(visibleSize2.width/background->getContentSize().width);
    boton1->setTag(1);

    //boton nueva partida

    boton2= MenuItemImage::create("boton_cargar_partida_OFF.png","boton_cargar_partida_ON.png",CC_CALLBACK_1(MainMenu::cargarPartida, this));
    boton2->setPosition(Vec2(origin2.x+visibleSize2.width/2+boton1->getContentSize().width*0.6,origin2.y+visibleSize2.height/2));
    boton2->setScale(visibleSize2.width/background->getContentSize().width);
    boton2->setTag(1);

    auto menu2 = Menu::create(boton1,boton2, NULL);
    menu2->setPosition(Vec2::ZERO);
    this->addChild(menu2,2,"menu");

    std::string fullPath = FileUtils::getInstance()->fullPathForFilename( "json/tarjetasEspeciales.json" );

    std::string doc = FileUtils::getInstance()->getStringFromFile( fullPath );

    rapidjson::Document parsedFromString;
    parsedFromString.Parse<0>(doc.c_str());

	cocos2d::log("%s",fullPath.c_str());
	cocos2d::log("%s",parsedFromString["Suerte"]["Tarjeta1"]["Mensaje"].GetString());


    return true;
}

void MainMenu::nuevaPartida(Ref* pSender){

	auto scene = ConfigScreen::createScene();
	Director::getInstance()->replaceScene(TransitionSlideInR::create(0.5, scene));

}

void MainMenu::cargarPartida(Ref* pSender){

	if(!FileUtils::sharedFileUtils()->isFileExist("/data/data/org.ordominguez.monopoly/files/partidasGuardadas.json")){
		rapidjson::Document d;
		rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
		d.SetObject();
		rapidjson::StringBuffer  buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		d.Accept(writer);
		FILE* fileWrite = fopen("/data/data/org.ordominguez.monopoly/files/partidasGuardadas.json", "w");
		if (fileWrite)
			{
		    	fputs(buffer.GetString(),fileWrite);
		    	fclose(fileWrite);
		    }
	}


	/*if(FileUtils::sharedFileUtils()->isFileExist("/data/data/org.ordominguez.monopoly/files/partidasGuardadas.json")){
		cocos2d::log("si que existe el archivo");
	}*/
	auto scene = LoadScreen::createScene();
	Director::getInstance()->replaceScene(TransitionSlideInR::create(0.5, scene));




}




