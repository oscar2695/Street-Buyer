/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */
#include "PauseMenu.h"
#include "GameScene.h"
#include "json/document.h"
#include "json/filestream.h"
#include "json/stringbuffer.h"
#include "json/prettywriter.h"
#include <cstdio>
using namespace rapidjson;

USING_NS_CC;
Size visibleSize5;
Vec2 origin5;
MenuItemImage* boton_guardar;
MenuItemImage* boton_salir;
MenuItemImage* boton_atras;
Label* label_atras;

bool PauseScreen::init()
{
	 if ( !Layer::init() )
	    {
	        return false;
	    }

    visibleSize5 = Director::getInstance()->getVisibleSize();
    origin5 = Director::getInstance()->getVisibleOrigin();

	auto fondo = Sprite::create("popup.png");
	fondo->setPosition(Vec2(origin5.x+visibleSize5.width/2,origin5.y+visibleSize5.height/2));
	fondo->setScale(visibleSize5.height/fondo->getContentSize().height);
	this->addChild(fondo);


	boton_guardar = MenuItemImage::create("boton_guardar.png","boton_guardar.png",CC_CALLBACK_1(PauseScreen::menuGuardar, this));
	boton_guardar->setPosition(Vec2(origin5.x+visibleSize5.width/2-boton_guardar->getContentSize().width*2,origin5.y+visibleSize5.height*0.7));
	boton_guardar->setScale(2);
	boton_guardar->setName("boton_guardar");

	boton_salir = MenuItemImage::create("boton_salir.png","boton_salir.png",CC_CALLBACK_1(PauseScreen::menuSalir, this));
	boton_salir->setPosition(Vec2(origin5.x+visibleSize5.width/2+boton_guardar->getContentSize().width*2,origin5.y+visibleSize5.height*0.7));
	boton_salir->setScale(2);
	boton_salir->setName("boton_salir");

	boton_atras = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(PauseScreen::cerrarPausa,this));
	boton_atras->setPosition(Vec2(origin5.x+visibleSize5.width/2,origin5.y+visibleSize5.height*0.2));
	boton_atras->setName("boton_atras");
	//boton_atras->setScale(1.5);

    label_atras = Label::createWithTTF("volver","fonts/Kabel Regular.ttf",50);
    label_atras->setPosition(Vec2(origin5.x+visibleSize5.width/2,origin5.y+visibleSize5.height*0.2));
    label_atras->setColor(Color3B(255,255,255));
    this->addChild(label_atras,4,"label");

	auto menuPausa = Menu::create(boton_guardar, boton_salir,boton_atras, NULL);
	menuPausa->setPosition(Vec2::ZERO);
	this->addChild(menuPausa,2);

    return true;
}

void PauseScreen::menuGuardar(Ref* psender){
	cocos2d::log("Guardando");
	boton_guardar->setVisible(false);
	boton_salir->setVisible(false);
	boton_atras->setVisible(false);
	//label_atras->setVisible(false);


    auto LabelNuevaPartida = Label::createWithTTF("Guardar Partida","fonts/Kabel Regular.ttf",90);
    LabelNuevaPartida->setPosition(Vec2(origin5.x+visibleSize5.width/2,origin5.y+visibleSize5.height-LabelNuevaPartida->getContentSize().height/2));
    LabelNuevaPartida->setColor(Color3B(255,255,255));
    this->addChild(LabelNuevaPartida,4,"label_guardar_partida");

	auto partida1 = MenuItemImage::create("boton_guardar_partida_1.png","boton_guardar_partida_1.png",CC_CALLBACK_1(PauseScreen::guardarPartida, this));
	partida1->setPosition(Vec2(origin5.x+visibleSize5.width/2-boton_guardar->getContentSize().width*3,origin5.y+visibleSize5.height*0.6));
	partida1->setName("Partida1");
	partida1->setScale(2);

	auto partida2 = MenuItemImage::create("boton_guardar_partida_2.png","boton_guardar_partida.png_2",CC_CALLBACK_1(PauseScreen::guardarPartida, this));
	partida2->setPosition(Vec2(origin5.x+visibleSize5.width/2,origin5.y+visibleSize5.height*0.6));
	partida2->setName("Partida2");
	partida2->setScale(2);

	auto partida3 = MenuItemImage::create("boton_guardar_partida_3.png","boton_guardar_partida_3.png",CC_CALLBACK_1(PauseScreen::guardarPartida, this));
	partida3->setPosition(Vec2(origin5.x+visibleSize5.width/2+boton_guardar->getContentSize().width*3,origin5.y+visibleSize5.height*0.6));
	partida3->setName("Partida3");
	partida3->setScale(2);

	auto boton_atras = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(PauseScreen::volverAtras, this));
	boton_atras->setPosition(Vec2(origin5.x+visibleSize5.width/2,origin5.y+visibleSize5.height*0.2));
	boton_atras->setName("boton_atras");
	boton_atras->setScale(1);

	auto menuGuardar = Menu::create(partida1, partida2, partida3, boton_atras, nullptr);
	menuGuardar->setPosition(Vec2::ZERO);
	this->addChild(menuGuardar,3,"menu_partidas");

}

void PauseScreen::menuSalir(Ref* pSender){
	cocos2d::log("Saliendo");
	cocos2d::Director::sharedDirector()->end();
}

void PauseScreen::guardarPartida(Ref* pSender){

	FILE* fileRead = fopen("/data/data/org.ordominguez.monopoly/files/partidaActual.json", "r");
	FileStream fs(fileRead);
	rapidjson::Document documentoJSONactual;
	documentoJSONactual.ParseStream<0>(fs);
	rapidjson::Document documentoJSONguardadas;
	if(FileUtils::sharedFileUtils()->isFileExist("/data/data/org.ordominguez.monopoly/files/partidasGuardadas.json"))
	{
		FILE* fileRead2 = fopen("/data/data/org.ordominguez.monopoly/files/partidasGuardadas.json", "r");
		FileStream fs2(fileRead2);
		documentoJSONguardadas.ParseStream<0>(fs2);
	}else{
		documentoJSONguardadas.SetObject();
	}

	rapidjson::Document::AllocatorType& allocator = documentoJSONguardadas.GetAllocator();

	cocos2d::log("Guardado");
	Node* node = (Node*) pSender;
	std::string name = node->getName();





	rapidjson::Value::MemberIterator M;
	rapidjson::Value Partida1(rapidjson::kObjectType);
	int num=documentoJSONactual["Partida1"]["NumeroJugadores"].GetInt();
	int jugador = documentoJSONactual["Partida1"]["JugadorActual"].GetInt();
	cocos2d::log("Jugador en Pause Menu: %d",jugador);
	if(num>=2){
		rapidjson::Value Jugador1(rapidjson::kObjectType);
		rapidjson::Value Propiedades1(rapidjson::kObjectType);

		rapidjson::Value Jugador2(rapidjson::kObjectType);
		rapidjson::Value Propiedades2(rapidjson::kObjectType);


		Jugador1.AddMember("Nombre",documentoJSONactual["Partida1"]["Jugador1"]["Nombre"].GetString(),allocator);
		Jugador1.AddMember("Dinero",documentoJSONactual["Partida1"]["Jugador1"]["Dinero"].GetInt(),allocator);
		Jugador1.AddMember("Ficha",documentoJSONactual["Partida1"]["Jugador1"]["Ficha"].GetString(),allocator);
		Jugador1.AddMember("Position",documentoJSONactual["Partida1"]["Jugador1"]["Position"].GetInt(),allocator);


	    for(M = documentoJSONactual["Partida1"]["Jugador1"]["Propiedades"].MemberonBegin();M != documentoJSONactual["Partida1"]["Jugador1"]["Propiedades"].MemberonEnd();M++){
		    cocos2d::log("Guardando en JSON");
		    cocos2d::log("Clave: %s",M->name.GetString());
		    cocos2d::log("Este es el numero de casas %d",M->value.GetInt());
		    Propiedades1.AddMember(M->name.GetString(),M->value.GetInt(),allocator);
	    }
	    //Propiedades1.AddMember("Propiedad1","Calle mercado",allocator);
	    Jugador1.AddMember("Propiedades",Propiedades1,allocator);

		Jugador2.AddMember("Nombre",documentoJSONactual["Partida1"]["Jugador2"]["Nombre"].GetString(),allocator);
		Jugador2.AddMember("Dinero",documentoJSONactual["Partida1"]["Jugador2"]["Dinero"].GetInt(),allocator);
		Jugador2.AddMember("Ficha",documentoJSONactual["Partida1"]["Jugador2"]["Ficha"].GetString(),allocator);
		Jugador2.AddMember("Position",documentoJSONactual["Partida1"]["Jugador2"]["Position"].GetInt(),allocator);

	    for(M = documentoJSONactual["Partida1"]["Jugador2"]["Propiedades"].MemberonBegin();M != documentoJSONactual["Partida1"]["Jugador2"]["Propiedades"].MemberonEnd();M++){
		    cocos2d::log("Guardando en JSON");
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

		Jugador3.AddMember("Nombre",documentoJSONactual["Partida1"]["Jugador3"]["Nombre"].GetString(),allocator);
		Jugador3.AddMember("Dinero",documentoJSONactual["Partida1"]["Jugador3"]["Dinero"].GetInt(),allocator);
		Jugador3.AddMember("Ficha",documentoJSONactual["Partida1"]["Jugador3"]["Ficha"].GetString(),allocator);
		Jugador3.AddMember("Position",documentoJSONactual["Partida1"]["Jugador3"]["Position"].GetInt(),allocator);

	    for(M = documentoJSONactual["Partida1"]["Jugador3"]["Propiedades"].MemberonBegin();M != documentoJSONactual["Partida1"]["Jugador3"]["Propiedades"].MemberonEnd();M++){
		    cocos2d::log("Guardando en JSON");
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

		Jugador4.AddMember("Nombre",documentoJSONactual["Partida1"]["Jugador4"]["Nombre"].GetString(),allocator);
		Jugador4.AddMember("Dinero",documentoJSONactual["Partida1"]["Jugador4"]["Dinero"].GetInt(),allocator);
		Jugador4.AddMember("Ficha",documentoJSONactual["Partida1"]["Jugador4"]["Ficha"].GetString(),allocator);
		Jugador4.AddMember("Position",documentoJSONactual["Partida1"]["Jugador4"]["Position"].GetInt(),allocator);

	    for(M = documentoJSONactual["Partida1"]["Jugador4"]["Propiedades"].MemberonBegin();M != documentoJSONactual["Partida1"]["Jugador4"]["Propiedades"].MemberonEnd();M++){
		    cocos2d::log("Guardando en JSON");
		    cocos2d::log("Clave: %s",M->name.GetString());
		    cocos2d::log("Este es el numero de casas %d",M->value.GetInt());
		    Propiedades4.AddMember(M->name.GetString(),M->value.GetInt(),allocator);
	    }
	    Jugador4.AddMember("Propiedades",Propiedades4,allocator);

		Partida1.AddMember("Jugador4",Jugador4,allocator);
	}

	Partida1.AddMember("NumeroJugadores",num,allocator);
	Partida1.AddMember("JugadorActual",jugador,allocator);
	documentoJSONguardadas.AddMember(name.c_str(),Partida1,allocator);

	rapidjson::StringBuffer  buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	documentoJSONguardadas.Accept(writer);
	FILE* fileWrite = fopen("/data/data/org.ordominguez.monopoly/files/partidasGuardadas.json", "w");
	if (fileWrite)
		{
	    	fputs(buffer.GetString(),fileWrite);
	    	fclose(fileWrite);
	    }

}

void PauseScreen::volverAtras(Ref* psender){

	this->removeChildByName("menu_partidas");
	this->removeChildByName("label_guardar_partida");


	boton_guardar->setVisible(true);
	boton_salir->setVisible(true);
	boton_atras->setVisible(true);

}

void PauseScreen::cerrarPausa(Ref* pSender){
	this->getParent()->removeChildByName("Menu Pausa");
}
