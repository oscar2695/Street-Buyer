/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */

#include "LoadScene.h"
#include "GameScene.h"
#include "MainMenuScene.h"
#include "json/document.h"
#include "json/filestream.h"
#include "json/stringbuffer.h"
#include <cstdio>
using namespace rapidjson;

USING_NS_CC;
Size visibleSize4;
Vec2 origin4;
Vector<MenuItem*> Item(1);
MenuItemImage* partida1;
MenuItemImage* partida2;
MenuItemImage* partida3;
MenuItemImage* borrar_partida1;
MenuItemImage* borrar_partida2;
MenuItemImage* borrar_partida3;
Label* label_partida1;
Label* label_partida2;
Label* label_partida3;



Scene* LoadScreen::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LoadScreen::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// inicializa la instancia de la escena
bool LoadScreen::init()

{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    visibleSize4 = Director::getInstance()->getVisibleSize();
    origin4 = Director::getInstance()->getVisibleOrigin();



    //Creamos y mostarmos el fondo
    auto background = Sprite::create("fondo_inicio_nb.png");
    background->setPosition(Vec2(visibleSize4.width/2 + origin4.x, visibleSize4.height/2 + origin4.y));
    background->setScale(visibleSize4.width/background->getContentSize().width);
    this->addChild(background,-1,"background");


    //Layer Nueva Partida

    auto LabelNuevaPartida = Label::createWithTTF("Cargar Partida","fonts/Kabel Regular.ttf",90);
    LabelNuevaPartida->setPosition(Vec2(origin4.x+visibleSize4.width/2,origin4.y+visibleSize4.height-LabelNuevaPartida->getContentSize().height));
    LabelNuevaPartida->setColor(Color3B(255,255,255));
    this->addChild(LabelNuevaPartida,4,"label");

    //Menu adelante y atrás

    auto botonAtras= MenuItemImage::create("boton_flecha.png","boton_flecha.png",CC_CALLBACK_1(LoadScreen::volverMainMenu, this));
    botonAtras->setPosition(Vec2(origin4.x+botonAtras->getContentSize().width,origin4.y+visibleSize4.height-botonAtras->getContentSize().height));

    auto menu2 = Menu::create(botonAtras, NULL);
    menu2->setPosition(Vec2::ZERO);
    this->addChild(menu2,2,"menu");

    //Creamos el menu de partidas a guardar
    this->cargarArchivoJSON();
    this->crearMenuCargar();


    return true;
}

//Crea una nueva partida(nueva escena) además de guardar los datos creados en la pantalla de configuración en un archivo JSON
void LoadScreen::crearNuevaPartida(Ref* pSender){

	//Creamos la siguiente Escena y la colocamos.
	auto scene = GameScreen::createScene();
	Director::getInstance()->replaceScene(TransitionSlideInR::create(0.5, scene));

}
//Vuelve al menú principal
void LoadScreen::volverMainMenu(Ref* pSender){

	auto scene = MainMenu::createScene();
	Director::getInstance()->replaceScene(TransitionSlideInL::create(0.5, scene));

}

void LoadScreen::cargarArchivoJSON(){
	FILE* fileRead = fopen("/data/data/org.ordominguez.monopoly/files/partidasGuardadas.json", "r");
	FileStream fs(fileRead);
	this->documentoJSONcargar.ParseStream<0>(fs);
	//cocos2d::log("%s",documentoJSONcargar["Partida1"].GetString());
}

//Guardar en el archivo JSON la nueva partida que acabamos de crear en la pantalla de configuración
void LoadScreen::crearMenuCargar(){
	auto layer_texto=Layer::create();

	partida1 = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(LoadScreen::cargarPartida, this));
	partida1->setPosition(Vec2(origin4.x+visibleSize4.width/2,origin4.y+visibleSize4.height/2+partida1->getContentSize().height*1.1));
	partida1->setName("Partida1");
	Item.pushBack(partida1);

	borrar_partida1 = MenuItemImage::create("boton_guardar_partida.png","boton_guardar_partida.png",CC_CALLBACK_1(LoadScreen::borrarPartida, this));
	borrar_partida1->setPosition(Vec2(origin4.x+visibleSize4.width/2+partida1->getContentSize().width*0.8,origin4.y+visibleSize4.height/2+partida1->getContentSize().height*1.1));
	borrar_partida1->setName("Partida1");
	Item.pushBack(borrar_partida1);

	label_partida1 = Label::createWithTTF("Partida1","fonts/Kabel Regular.ttf",25);
	label_partida1->setColor(Color3B(255,255,255));
	label_partida1->setPosition(Vec2(origin4.x+visibleSize4.width/2,origin4.y+visibleSize4.height/2+partida1->getContentSize().height*1.1));
	layer_texto->addChild(label_partida1,3,"label_partida1");


	partida2 = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(LoadScreen::cargarPartida, this));
	partida2->setPosition(Vec2(origin4.x+visibleSize4.width/2,origin4.y+visibleSize4.height/2));
	partida2->setName("Partida2");
	Item.pushBack(partida2);

	borrar_partida2 = MenuItemImage::create("boton_guardar_partida.png","boton_guardar_partida.png",CC_CALLBACK_1(LoadScreen::borrarPartida, this));
	borrar_partida2->setPosition(Vec2(origin4.x+visibleSize4.width/2+partida2->getContentSize().width*0.8,origin4.y+visibleSize4.height/2));
	borrar_partida2->setName("Partida2");
	Item.pushBack(borrar_partida2);

	label_partida2 = Label::createWithTTF("Partida2","fonts/Kabel Regular.ttf",25);
	label_partida2->setColor(Color3B(255,255,255));
	label_partida2->setPosition(Vec2(origin4.x+visibleSize4.width/2,origin4.y+visibleSize4.height/2));
	layer_texto->addChild(label_partida2,3,"label_partida2");

	partida3 = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(LoadScreen::cargarPartida, this));
	partida3->setPosition(Vec2(origin4.x+visibleSize4.width/2,origin4.y+visibleSize4.height/2-partida3->getContentSize().height*1.1));
	partida3->setName("Partida3");
	Item.pushBack(partida3);

	borrar_partida3 = MenuItemImage::create("boton_guardar_partida.png","boton_guardar_partida.png",CC_CALLBACK_1(LoadScreen::borrarPartida, this));
	borrar_partida3->setPosition(Vec2(origin4.x+visibleSize4.width/2+partida3->getContentSize().width*0.8,origin4.y+visibleSize4.height/2-partida3->getContentSize().height*1.1));
	borrar_partida3->setName("Partida3");
	Item.pushBack(borrar_partida3);

	label_partida3 = Label::createWithTTF("Partida3","fonts/Kabel Regular.ttf",25);
	label_partida3->setColor(Color3B(255,255,255));
	label_partida3->setPosition(Vec2(origin4.x+visibleSize4.width/2,origin4.y+visibleSize4.height/2-partida3->getContentSize().height*1.1));
	layer_texto->addChild(label_partida3,3,"label_partida3");

	auto menuCargar = Menu::createWithArray(Item);
	menuCargar->setPosition(Vec2::ZERO);
	this->addChild(menuCargar,2,"menuCargar");
	this->addChild(layer_texto,3,"layer_texto");
	this->ocultarBotones();

}

void LoadScreen::cargarPartida(Ref* pSender){
	CCNode* node = (CCNode*) pSender;
	std::string name = node->getName();

	rapidjson::Document d;
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	d.SetObject();

	rapidjson::Value Partida1(rapidjson::kObjectType);
	int num=documentoJSONcargar[name.c_str()]["NumeroJugadores"].GetInt();
	int jugador = documentoJSONcargar[name.c_str()]["JugadorActual"].GetInt();
	cocos2d::log("Jugador en load secene: %d",jugador);
	rapidjson::Value::MemberIterator M;
	if(num>=2){
		rapidjson::Value Jugador1(rapidjson::kObjectType);
		rapidjson::Value Propiedades1(rapidjson::kObjectType);

		rapidjson::Value Jugador2(rapidjson::kObjectType);
		rapidjson::Value Propiedades2(rapidjson::kObjectType);


		Jugador1.AddMember("Nombre",documentoJSONcargar[name.c_str()]["Jugador1"]["Nombre"].GetString(),allocator);
		Jugador1.AddMember("Dinero",documentoJSONcargar[name.c_str()]["Jugador1"]["Dinero"].GetInt(),allocator);
		Jugador1.AddMember("Ficha",documentoJSONcargar[name.c_str()]["Jugador1"]["Ficha"].GetString(),allocator);
		Jugador1.AddMember("Position",documentoJSONcargar[name.c_str()]["Jugador1"]["Position"].GetInt(),allocator);


	    for(M = documentoJSONcargar[name.c_str()]["Jugador1"]["Propiedades"].MemberonBegin();M != documentoJSONcargar[name.c_str()]["Jugador1"]["Propiedades"].MemberonEnd();M++){
		    cocos2d::log("Cargando en JSON");
		    cocos2d::log("Clave: %s",M->name.GetString());
		    cocos2d::log("Este es el numero de casas %d",M->value.GetInt());
		    Propiedades1.AddMember(M->name.GetString(),M->value.GetInt(),allocator);
	    }
	    Jugador1.AddMember("Propiedades",Propiedades1,allocator);


		Jugador2.AddMember("Nombre",documentoJSONcargar[name.c_str()]["Jugador2"]["Nombre"].GetString(),allocator);
		Jugador2.AddMember("Dinero",documentoJSONcargar[name.c_str()]["Jugador2"]["Dinero"].GetInt(),allocator);
		Jugador2.AddMember("Ficha",documentoJSONcargar[name.c_str()]["Jugador2"]["Ficha"].GetString(),allocator);
		Jugador2.AddMember("Position",documentoJSONcargar[name.c_str()]["Jugador2"]["Position"].GetInt(),allocator);

	    for(M = documentoJSONcargar[name.c_str()]["Jugador2"]["Propiedades"].MemberonBegin();M != documentoJSONcargar[name.c_str()]["Jugador2"]["Propiedades"].MemberonEnd();M++){
		    cocos2d::log("Cargando en JSON");
		    cocos2d::log("Clave: %s",M->name.GetString());
		    cocos2d::log("Este es el numero de casas %d",M->value.GetInt());
		    Propiedades2.AddMember(M->name.GetString(),M->value.GetInt(),allocator);
	    }
	    Jugador2.AddMember("Propiedades",Propiedades2,allocator);

		Partida1.AddMember("Jugador1",Jugador1,allocator);
		Partida1.AddMember("Jugador2",Jugador2,allocator);
	}

	if(num>=3){
		rapidjson::Value Jugador3(rapidjson::kObjectType);
		rapidjson::Value Propiedades3(rapidjson::kObjectType);

		Jugador3.AddMember("Nombre",documentoJSONcargar[name.c_str()]["Jugador3"]["Nombre"].GetString(),allocator);
		Jugador3.AddMember("Dinero",documentoJSONcargar[name.c_str()]["Jugador3"]["Dinero"].GetInt(),allocator);
		Jugador3.AddMember("Ficha",documentoJSONcargar[name.c_str()]["Jugador3"]["Ficha"].GetString(),allocator);
		Jugador3.AddMember("Position",documentoJSONcargar[name.c_str()]["Jugador3"]["Position"].GetInt(),allocator);

	    for(M = documentoJSONcargar[name.c_str()]["Jugador3"]["Propiedades"].MemberonBegin();M != documentoJSONcargar[name.c_str()]["Jugador3"]["Propiedades"].MemberonEnd();M++){
		    cocos2d::log("Cargando en JSON");
		    cocos2d::log("Clave: %s",M->name.GetString());
		    cocos2d::log("Este es el numero de casas %d",M->value.GetInt());
		    Propiedades3.AddMember(M->name.GetString(),M->value.GetInt(),allocator);
	    }
	    Jugador3.AddMember("Propiedades",Propiedades3,allocator);

		Partida1.AddMember("Jugador3",Jugador3,allocator);
	}

	if(num==4){
		rapidjson::Value Jugador4(rapidjson::kObjectType);
		rapidjson::Value Propiedades4(rapidjson::kObjectType);

		Jugador4.AddMember("Nombre",documentoJSONcargar[name.c_str()]["Jugador4"]["Nombre"].GetString(),allocator);
		Jugador4.AddMember("Dinero",documentoJSONcargar[name.c_str()]["Jugador4"]["Dinero"].GetInt(),allocator);
		Jugador4.AddMember("Ficha",documentoJSONcargar[name.c_str()]["Jugador4"]["Ficha"].GetString(),allocator);
		Jugador4.AddMember("Position",documentoJSONcargar[name.c_str()]["Jugador4"]["Position"].GetInt(),allocator);

	    for(M = documentoJSONcargar[name.c_str()]["Jugador4"]["Propiedades"].MemberonBegin();M != documentoJSONcargar[name.c_str()]["Jugador4"]["Propiedades"].MemberonEnd();M++){
		    cocos2d::log("Cargando en JSON");
		    cocos2d::log("Clave: %s",M->name.GetString());
		    cocos2d::log("Este es el numero de casas %d",M->value.GetInt());
		    Propiedades4.AddMember(M->name.GetString(),M->value.GetInt(),allocator);
	    }
	    Jugador4.AddMember("Propiedades",Propiedades4,allocator);

		Partida1.AddMember("Jugador4",Jugador4,allocator);
	}

	Partida1.AddMember("NumeroJugadores",num,allocator);
	Partida1.AddMember("JugadorActual",jugador,allocator);
	d.AddMember("Partida1",Partida1,allocator);

	rapidjson::StringBuffer  buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	FILE* fileWrite = fopen("/data/data/org.ordominguez.monopoly/files/partidaActual.json", "w");
	if (fileWrite)
		{
	    	fputs(buffer.GetString(),fileWrite);
	    	fclose(fileWrite);
	    }

	auto scene = GameScreen::createScene();
	Director::getInstance()->replaceScene(TransitionSlideInR::create(0.5, scene));
}

void LoadScreen::borrarPartida(Ref* pSender){
	CCNode* node = (CCNode*) pSender;
	std::string name = node->getName();

	this->documentoJSONcargar.RemoveMember(name.c_str());

	rapidjson::StringBuffer  buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	this->documentoJSONcargar.Accept(writer);
	FILE* fileWrite = fopen("/data/data/org.ordominguez.monopoly/files/partidasGuardadas.json", "w");
	if (fileWrite)
		{
		    fputs(buffer.GetString(),fileWrite);
		    fclose(fileWrite);
		}
	this->ocultarBotones();
}

void LoadScreen::ocultarBotones(){
	if(this->documentoJSONcargar["Partida1"].Empty() || this->documentoJSONcargar["Partida1"].IsNull()){
		partida1->setVisible(false);
		label_partida1->setVisible(false);
		borrar_partida1->setVisible(false);
	}

	if(this->documentoJSONcargar["Partida2"].Empty()){
		partida2->setVisible(false);
		label_partida2->setVisible(false);
		borrar_partida2->setVisible(false);
	}

	if(this->documentoJSONcargar["Partida3"].Empty()){
		partida3->setVisible(false);
		label_partida3->setVisible(false);
		borrar_partida3->setVisible(false);
	}
}
