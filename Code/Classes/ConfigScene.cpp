/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */

#include "ConfigScene.h"
#include "GameScene.h"
#include "MainMenuScene.h"
#include "GUI/CCControlExtension/CCControlExtensions.h"
#include "json/document.h"
#include "json/filestream.h"
#include "json/prettywriter.h"
#include "json/stringbuffer.h"
#include <cstdio>
using namespace rapidjson;

USING_NS_CC;
Size visibleSize3;
Vec2 origin3;
Label* numero_jugadores;
cocos2d::extension::ControlSlider* slider;
ui::EditBox*  editJugador1;
ui::EditBox* editJugador2;
ui::EditBox* editJugador3;
ui::EditBox* editJugador4;
int numeroJugadores;


Scene* ConfigScreen::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = ConfigScreen::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// inicializa la instancia de la escena
bool ConfigScreen::init()

{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    visibleSize3 = Director::getInstance()->getVisibleSize();
    origin3 = Director::getInstance()->getVisibleOrigin();



    //Creamos y mostarmos el fondo
    auto background = Sprite::create("fondo_inicio_nb.png");
    background->setPosition(Vec2(visibleSize3.width/2 + origin3.x, visibleSize3.height/2 + origin3.y));
    background->setScale(visibleSize3.width/background->getContentSize().width);
    this->addChild(background,-1,"background");


    //Layer Nueva Partida

    auto LabelNuevaPartida = Label::createWithTTF("Nueva Partida","fonts/Kabel Regular.ttf",90);
    LabelNuevaPartida->setPosition(Vec2(origin3.x+visibleSize3.width/2,origin3.y+visibleSize3.height-LabelNuevaPartida->getContentSize().height));
    LabelNuevaPartida->setColor(Color3B(255,255,255));
    this->addChild(LabelNuevaPartida,4,"label");

    //Menu adelante y atrás

    auto botonAtras= MenuItemImage::create("boton_flecha.png","boton_flecha.png",CC_CALLBACK_1(ConfigScreen::volverMainMenu, this));
    botonAtras->setPosition(Vec2(origin3.x+botonAtras->getContentSize().width,origin3.y+visibleSize3.height-botonAtras->getContentSize().height));

    auto botonAdelante= MenuItemImage::create("boton_flecha.png","boton_flecha.png",CC_CALLBACK_1(ConfigScreen::crearNuevaPartida, this));
    botonAdelante->setPosition(Vec2(origin3.x+visibleSize3.width-botonAtras->getContentSize().width,origin3.y+visibleSize3.height-botonAtras->getContentSize().height));
    botonAdelante->setRotation(180);
    //botonAdelante->setScale(visibleSize3.width/background->getContentSize().width);
    botonAdelante->setTag(1);

    auto menu2 = Menu::create(botonAdelante,botonAtras, NULL);
    menu2->setPosition(Vec2::ZERO);
    this->addChild(menu2,2,"menu");

    //Slider para elegir el numero de jugadores

    slider = cocos2d::extension::ControlSlider::create("contenedor_slider.png", "progreso_slider.png", "circulo_slider.png");
    slider->setMinimumValue(2);
    slider->setMaximumValue(4);
    slider->setValue(3);
    slider->addTargetWithActionForControlEvents(this, cccontrol_selector(ConfigScreen::valueSliderChange), cocos2d::extension::Control::EventType::VALUE_CHANGED);
    slider->setPosition(Vec2(origin3.x+visibleSize3.width/2,origin3.y+visibleSize3.height-LabelNuevaPartida->getContentSize().height*2));
    this->addChild(slider);

    //Label que muestra el numero de juegadores elegidos

    char mostrar[15] = {0};
    int valor = slider->getValue();
    sprintf(mostrar, "%d Jugadores",valor);
    std::string cadena (mostrar);
    numero_jugadores = Label::createWithTTF(cadena,"fonts/Kabel Regular.ttf",32);
    numero_jugadores->setColor(Color3B(0,0,0));
    numero_jugadores->setPosition(origin3.x+visibleSize3.width/2,origin3.y+visibleSize3.height-numero_jugadores->getContentSize().height-LabelNuevaPartida->getContentSize().height*2);
    this->addChild(numero_jugadores,2,"numero_jugadores");

    //creamos el editbox
    this->crearEditBox();

    return true;
}
//Cuando cambia el valor del slider se cambia el numero de editbox que aparecen en pantalla.
void ConfigScreen::valueSliderChange(Ref* sender, cocos2d::extension::Control::EventType evnt)
    {
        int numeroJugadores = static_cast<cocos2d::extension::ControlSlider*>(sender)->getValue();
        char mostrar2[30] = {0};
        sprintf(mostrar2, "%d Jugadores",numeroJugadores);
        std::string cadena2 (mostrar2);
        numero_jugadores->setString(cadena2);

        if(numeroJugadores==2){
        	editJugador1->setVisible(true);
        	editJugador2->setVisible(true);
        	editJugador3->setVisible(false);
        	editJugador4->setVisible(false);
        }
        if(numeroJugadores==3){
        	editJugador1->setVisible(true);
        	editJugador2->setVisible(true);
        	editJugador3->setVisible(true);
        	editJugador4->setVisible(false);
        }
        if(numeroJugadores==4){
        	editJugador1->setVisible(true);
        	editJugador2->setVisible(true);
        	editJugador3->setVisible(true);
        	editJugador4->setVisible(true);
        }


}
//Crea una nueva partida(nueva escena) además de guardar los datos creados en la pantalla de configuración en un archivo JSON
void ConfigScreen::crearNuevaPartida(Ref* pSender){

	//Guardamos la configuración de la partida en un archivo JSON
	int num=slider->getValue();

	this->crearPartidaJson(num);

	//Creamos la siguiente Escena y la colocamos.
	auto scene = GameScreen::createScene();
	Director::getInstance()->replaceScene(TransitionSlideInR::create(0.5, scene));

}
//Vuelve al menú principal
void ConfigScreen::volverMainMenu(Ref* pSender){

	auto scene = MainMenu::createScene();
	Director::getInstance()->replaceScene(TransitionSlideInL::create(0.5, scene));

}
//crea los editbox para escribir el nombre de los participantes en la partida
void ConfigScreen::crearEditBox(){
	auto sprite = ui::Scale9Sprite::create("banner-puntuacion.png");
	editJugador1 = ui::EditBox::create(sprite->getContentSize(),sprite);
	editJugador1->setPosition(Vec2(origin3.x+visibleSize3.width/2-sprite->getContentSize().width*0.55, origin3.y+visibleSize3.height/2));
	editJugador1->setFontName("fonts/Kabel Regular.ttf");
	editJugador1->setFontSize(40);
	editJugador1->setFontColor(Color3B::WHITE);
	editJugador1->setPlaceHolder("Jugador 1");
	editJugador1->setPlaceholderFontColor(Color3B::WHITE);
	editJugador1->setMaxLength(14);
	editJugador1->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	this->addChild(editJugador1);

	auto sprite2 = ui::Scale9Sprite::create("banner-puntuacion.png");
	editJugador2 = ui::EditBox::create(sprite->getContentSize(),sprite2);
	editJugador2->setPosition(Vec2(origin3.x+visibleSize3.width/2+sprite->getContentSize().width*0.55, origin3.y+visibleSize3.height/2));
	editJugador2->setFontName("fonts/Kabel Regular.ttf");
	editJugador2->setFontSize(40);
	editJugador2->setFontColor(Color3B::WHITE);
	editJugador2->setPlaceHolder("Jugador 2");
	editJugador2->setPlaceholderFontColor(Color3B::WHITE);
	editJugador2->setMaxLength(14);
	editJugador2->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	this->addChild(editJugador2);

	auto sprite3 = ui::Scale9Sprite::create("banner-puntuacion.png");
	editJugador3 = ui::EditBox::create(sprite->getContentSize(),sprite3);
	editJugador3->setPosition(Vec2(origin3.x+visibleSize3.width/2-sprite->getContentSize().width*0.55, origin3.y+visibleSize3.height/2-sprite->getContentSize().height*1.1));
	editJugador3->setFontName("fonts/Kabel Regular.ttf");
	editJugador3->setFontSize(40);
	editJugador3->setFontColor(Color3B::WHITE);
	editJugador3->setPlaceHolder("Jugador 3");
	editJugador3->setPlaceholderFontColor(Color3B::WHITE);
	editJugador3->setMaxLength(14);
	editJugador3->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	this->addChild(editJugador3);

	auto sprite4 = ui::Scale9Sprite::create("banner-puntuacion.png");
	editJugador4 = ui::EditBox::create(sprite->getContentSize(),sprite4);
	editJugador4->setPosition(Vec2(origin3.x+visibleSize3.width/2+sprite->getContentSize().width*0.55, origin3.y+visibleSize3.height/2-sprite->getContentSize().height*1.1));
	editJugador4->setFontName("fonts/Kabel Regular.ttf");
	editJugador4->setFontSize(40);
	editJugador4->setFontColor(Color3B::WHITE);
	editJugador4->setPlaceHolder("Jugador 4");
	editJugador4->setPlaceholderFontColor(Color3B::WHITE);
	editJugador4->setMaxLength(14);
	editJugador4->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
	this->addChild(editJugador4);

}
//Guardar en el archivo JSON la nueva partida que acabamos de crear en la pantalla de configuración
void ConfigScreen::crearPartidaJson(int num){

	    rapidjson::Document d;
	    rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	    d.SetObject();

	    rapidjson::Value Partida1(rapidjson::kObjectType);

		if(num>=2){
			rapidjson::Value Jugador1(rapidjson::kObjectType);
			rapidjson::Value Propiedades1(rapidjson::kObjectType);

			rapidjson::Value Jugador2(rapidjson::kObjectType);
			rapidjson::Value Propiedades2(rapidjson::kObjectType);


			GenericValue<UTF8<char> > cadena (editJugador1->getText());
			Jugador1.AddMember("Nombre",cadena,allocator);
			Jugador1.AddMember("Dinero",2000,allocator);
			Jugador1.AddMember("Ficha","ficha.png",allocator);
			Jugador1.AddMember("Position",0,allocator);
			Jugador1.AddMember("Propiedades",Propiedades1,allocator);

			Jugador2.AddMember("Nombre",editJugador2->getText(),allocator);
			Jugador2.AddMember("Dinero",2000,allocator);
			Jugador2.AddMember("Ficha","ficha.png",allocator);
			Jugador2.AddMember("Position",0,allocator);
			Jugador2.AddMember("Propiedades",Propiedades2,allocator);

			Partida1.AddMember("Jugador1",Jugador1,allocator);
			Partida1.AddMember("Jugador2",Jugador2,allocator);
		}

		if(num>=3){
			rapidjson::Value Jugador3(rapidjson::kObjectType);
			rapidjson::Value Propiedades3(rapidjson::kObjectType);

			Jugador3.AddMember("Nombre",editJugador3->getText(),allocator);
			Jugador3.AddMember("Dinero",2000,allocator);
			Jugador3.AddMember("Ficha","ficha.png",allocator);
			Jugador3.AddMember("Position",0,allocator);
			Jugador3.AddMember("Propiedades",Propiedades3,allocator);

			Partida1.AddMember("Jugador3",Jugador3,allocator);
		}

		if(num==4){
			rapidjson::Value Jugador4(rapidjson::kObjectType);
			rapidjson::Value Propiedades4(rapidjson::kObjectType);

			Jugador4.AddMember("Nombre",editJugador4->getText(),allocator);
			Jugador4.AddMember("Dinero",2000,allocator);
			Jugador4.AddMember("Ficha","ficha.png",allocator);
			Jugador4.AddMember("Position",0,allocator);
			Jugador4.AddMember("Propiedades",Propiedades4,allocator);

			Partida1.AddMember("Jugador4",Jugador4,allocator);
		}

		Partida1.AddMember("NumeroJugadores",num,allocator);
		Partida1.AddMember("JugadorActual",1,allocator);

		d.AddMember("Partida1",Partida1,allocator);

		//Escribimos a archivo de la partida actual
		rapidjson::StringBuffer  buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		d.Accept(writer);
		FILE* fileWrite = fopen("/data/data/org.ordominguez.monopoly/files/partidaActual.json", "w");
		if (fileWrite)
			{
		    	fputs(buffer.GetString(),fileWrite);
		    	fclose(fileWrite);
		    }
}
/////////////////////////////////////////////////////////////////////////////////////
/*CODIGO QUE FUNCIONA PARA ESCRIBIR JSON A ARCHIVO

const char* json = "{\"a\":\"1\",\"b\":\"2\",\"c\":\"3\"}";
	auto  path =FileUtils::getInstance()->getWritablePath();
	path.append("config.json");
	rapidjson::Document d;
	d.Parse<0>(json);

	rapidjson::StringBuffer  buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	FILE* file = fopen(path.c_str(), "w");
	if (file)
		{
	    	fputs("{\"hola\":\"mundo\"}",file);
	    	fclose(file);
	    }
*/
///////////////////////////////////////////////////////////////////////////////////


