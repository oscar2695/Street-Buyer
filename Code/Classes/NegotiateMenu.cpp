/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */
#include "NegotiateMenu.h"
#include "GameScene.h"
#include "json/document.h"
#include "json/filestream.h"
#include "json/stringbuffer.h"
#include "json/prettywriter.h"
#include <cstdio>
using namespace rapidjson;

USING_NS_CC;

Vector<MenuItem*> Jugadores(1);
Vector<MenuItem*> PropiedadesJugadorCambio(1);
Vector<MenuItem*> PropiedadesOponenteCambio(1);

bool NegotiateScreen::init()
{
	 if ( !Layer::init() )
	    {
	        return false;
	    }

	this->cargarArchivoJSON();

	jugadorNegotiate = partidaActualJSON["Partida1"]["JugadorActual"].GetInt();
	cocos2d::log("Este es el jugador para el que estamos mirando para negociar: %d",jugadorNegotiate);

	visibleSizeNegotiate = Director::getInstance()->getVisibleSize();
	originNegotiate = Director::getInstance()->getVisibleOrigin();


	JugadoresNegociar = Layer::create();
	auto fondo = Sprite::create("popup.png");
	fondo->setPosition(Vec2(originNegotiate.x+visibleSizeNegotiate.width/2,originNegotiate.y+visibleSizeNegotiate.height/2));
	fondo->setScale(visibleSizeNegotiate.height/fondo->getContentSize().height);
	this->addChild(fondo,0);

	auto label_titulo = Label::createWithTTF("Negociar","fonts/Kabel Bold.ttf",60);
	label_titulo->setPosition(Vec2(originNegotiate.x+visibleSizeNegotiate.width/2,originNegotiate.y+visibleSizeNegotiate.height-label_titulo->getContentSize().height));
	JugadoresNegociar->addChild(label_titulo,1);


    auto label_atras = Label::createWithTTF("volver","fonts/Kabel Regular.ttf",50);
    label_atras->setPosition(Vec2(originNegotiate.x+visibleSizeNegotiate.width/2,originNegotiate.y+visibleSizeNegotiate.height*0.2));
    label_atras->setColor(Color3B(255,255,255));
    JugadoresNegociar->addChild(label_atras,4,"label");

	auto boton_atras = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(NegotiateScreen::cerrarNegociar,this));
	boton_atras->setPosition(Vec2(originNegotiate.x+visibleSizeNegotiate.width/2,originNegotiate.y+visibleSizeNegotiate.height*0.2));
	boton_atras->setName("boton_atras");

	auto menuConstruir = Menu::create(boton_atras, nullptr);
	menuConstruir->setPosition(Vec2::ZERO);
	JugadoresNegociar->addChild(menuConstruir,2);

	this->mostrarOponentes(jugadorNegotiate);

	this->addChild(JugadoresNegociar,1,"Jugadores_Negociar");
	//this->comprobarGrupoColor(jugadorBuild);
    return true;
}

void NegotiateScreen::mostrarOponentes(int jugador){
	int numJugadores = partidaActualJSON["Partida1"]["NumeroJugadores"].GetInt();
	if (numJugadores>=2){
		if (jugador!=1){
			auto jugador1 = MenuItemImage::create("boton_negociar_1.png","boton_negociar_1.png",CC_CALLBACK_1(NegotiateScreen::mostrarMenuNegociar, this));
			jugador1->setName("Jugador1");
			jugador1->setScale(1.5);
			Jugadores.pushBack(jugador1);

		}
		if (jugador!=2){
			auto jugador2 = MenuItemImage::create("boton_negociar_2.png","boton_negociar_2.png",CC_CALLBACK_1(NegotiateScreen::mostrarMenuNegociar, this));
			jugador2->setName("Jugador2");
			jugador2->setScale(1.5);
			Jugadores.pushBack(jugador2);
		}
	}
	if (numJugadores>=3){
		if (jugador!=3){
			auto jugador3 = MenuItemImage::create("boton_negociar_3.png","boton_negociar_3.png",CC_CALLBACK_1(NegotiateScreen::mostrarMenuNegociar, this));
			jugador3->setName("Jugador3");
			jugador3->setScale(1.5);
			Jugadores.pushBack(jugador3);
		}
	}
	if (numJugadores==4){
		if (jugador!=4){
			auto jugador4 = MenuItemImage::create("boton_negociar_4.png","boton_negociar_4.png",CC_CALLBACK_1(NegotiateScreen::mostrarMenuNegociar, this));
			jugador4->setName("Jugador4");
			jugador4->setScale(1.5);
			Jugadores.pushBack(jugador4);
		}
	}

	auto menuJugadores = Menu::createWithArray(Jugadores);
	menuJugadores->alignItemsHorizontallyWithPadding(25.0);
	menuJugadores->setPosition(Vec2(originNegotiate.x+visibleSizeNegotiate.width/2,originNegotiate.y+visibleSizeNegotiate.height/2));
	JugadoresNegociar->addChild(menuJugadores);


}

void NegotiateScreen::mostrarMenuNegociar(Ref* pSender){
	this->removeChildByName("Jugadores_Negociar");


	sprintf(jugador,"Jugador%d",jugadorNegotiate);
	std::string nombreJugador = partidaActualJSON["Partida1"][jugador]["Nombre"].GetString();
	cocos2d::log("Jugadro: %s", nombreJugador.c_str());

	Node * nodo = (Node*) pSender;
	std::string oponente_numero = nodo->getName();
	sprintf(oponente,"%s",oponente_numero.c_str());
	std::string nombreOponente = partidaActualJSON["Partida1"][oponente]["Nombre"].GetString();

	cocos2d::log("Oponente: %s", nombreOponente.c_str());

	Cambio = Layer::create();
	//Colocamos los labels de los jugadores.
    auto labelJugador = Label::createWithTTF(nombreJugador.c_str(),"fonts/Kabel Regular.ttf",50);
    labelJugador->setPosition(Vec2(originNegotiate.x+visibleSizeNegotiate.width*0.2,originNegotiate.y+visibleSizeNegotiate.height*0.9));
    labelJugador->setColor(Color3B(255,255,255));
    Cambio->addChild(labelJugador,4,"label");

    auto labelOponente = Label::createWithTTF(nombreOponente.c_str(),"fonts/Kabel Regular.ttf",50);
    labelOponente->setPosition(Vec2(originNegotiate.x+visibleSizeNegotiate.width*0.8,originNegotiate.y+visibleSizeNegotiate.height*0.9));
    labelOponente->setColor(Color3B(255,255,255));
    Cambio->addChild(labelOponente,4,"label");

    //Propiedades del jugador
    this->mostrarPropiedadesJugador(jugador);
    this->mostrarPropiedadesOponente(oponente);

	auto boton_cancelar = MenuItemImage::create("boton_cancelar.png","boton_cancelar.png",CC_CALLBACK_1(NegotiateScreen::cerrarNegociar,this));
	boton_cancelar->setPosition(Vec2(originNegotiate.x+visibleSizeNegotiate.width/2,originNegotiate.y+visibleSizeNegotiate.height*0.4));
	boton_cancelar->setName("boton_cancelar");

	auto boton_cambiar = MenuItemImage::create("boton_cambiar.png","boton_cambiar.png",CC_CALLBACK_1(NegotiateScreen::cambiarPropiedades,this));
	boton_cambiar->setPosition(Vec2(originNegotiate.x+visibleSizeNegotiate.width/2,originNegotiate.y+visibleSizeNegotiate.height*0.6));
	boton_cambiar->setName("boton_cambiar");

	auto menuConstruir = Menu::create(boton_cancelar,boton_cambiar, nullptr);
	menuConstruir->setPosition(Vec2::ZERO);
	Cambio->addChild(menuConstruir,2);

    this->addChild(Cambio);
}

void NegotiateScreen::mostrarPropiedadesJugador(char str[]){
    int i =0;
    rapidjson::Value::MemberIterator M;
    for(M = partidaActualJSON["Partida1"][str]["Propiedades"].MemberonBegin();M != partidaActualJSON["Partida1"][str]["Propiedades"].MemberonEnd();M++){
			char img_propiedad[100];
			sprintf(img_propiedad,"img/propiedades/%s.jpg",M->name.GetString());
			auto propiedad = MenuItemImage::create(img_propiedad,img_propiedad,CC_CALLBACK_1(NegotiateScreen::cambiarPropiedadJugador, this));
			//propiedad->setScale(0.55);
			propiedad->setName(M->name.GetString());
			PropiedadesJugadorCambio.pushBack(propiedad);
			cocos2d::log("%s",img_propiedad);
			i++;
    }

	auto menuPropiedadesJugador = Menu::createWithArray(PropiedadesJugadorCambio);
	int num = 0;
	if (PropiedadesJugadorCambio.size()/3<5){
		num = (PropiedadesJugadorCambio.size()/3)+1;
	}else{
		num = (PropiedadesJugadorCambio.size()/3)+2;
	}
	cocos2d::log("Numero de for: %f",originNegotiate.y+visibleSizeNegotiate.height*0.3);

	menuPropiedadesJugador->alignItemsInColumns(num,num,num,nullptr);
	menuPropiedadesJugador->setScale(0.25);
	menuPropiedadesJugador->setPosition(Vec2(-250,200));
	Cambio->addChild(menuPropiedadesJugador);
}

void NegotiateScreen::mostrarPropiedadesOponente(char str[]){
    int i =0;
    rapidjson::Value::MemberIterator M;
    for(M = partidaActualJSON["Partida1"][str]["Propiedades"].MemberonBegin();M != partidaActualJSON["Partida1"][str]["Propiedades"].MemberonEnd();M++){
			char img_propiedad[100];
			sprintf(img_propiedad,"img/propiedades/%s.jpg",M->name.GetString());
			auto propiedad = MenuItemImage::create(img_propiedad,img_propiedad,CC_CALLBACK_1(NegotiateScreen::cambiarPropiedadOponente, this));
			//propiedad->setScale(0.55);
			propiedad->setName(M->name.GetString());
			PropiedadesOponenteCambio.pushBack(propiedad);
			cocos2d::log("%s",img_propiedad);
			i++;
    }

	auto menuPropiedadesOponente = Menu::createWithArray(PropiedadesOponenteCambio);
	int num = 0;
	if (PropiedadesOponenteCambio.size()/3<5){
		num = (PropiedadesOponenteCambio.size()/3)+1;
	}else{
		num = (PropiedadesOponenteCambio.size()/3)+2;
	}

	cocos2d::log("Numero de for: %d",i);

	menuPropiedadesOponente->alignItemsInColumns(num,num,num,nullptr);
	menuPropiedadesOponente->setScale(0.25);
	menuPropiedadesOponente->setPosition(Vec2(550,200));
	Cambio->addChild(menuPropiedadesOponente);
}

void NegotiateScreen::cambiarPropiedadJugador(Ref* pSender){
	MenuItem* propiedad = (MenuItem*) pSender;
	if (propiedad->getColor()== Color3B::RED){
		propiedad->setColor(Color3B::WHITE);
		std::vector<std::string>::iterator begin = PropiedadesParaOponente.begin();
		std::vector<std::string>::iterator end = PropiedadesParaOponente.end();


		for (std::vector<std::string>::iterator it = begin; it != end; it++) {
			std::string prop = *it;
			if (prop == propiedad->getName()){
				PropiedadesParaOponente.erase(it);
			}
		}

	}else{
		propiedad->setColor(Color3B::RED);
		PropiedadesParaOponente.push_back(propiedad->getName());
	}

}

void NegotiateScreen::cambiarPropiedadOponente(Ref* pSender){
	MenuItem* propiedad = (MenuItem*) pSender;
	if (propiedad->getColor()== Color3B::GREEN){
		propiedad->setColor(Color3B::WHITE);
		std::vector<std::string>::iterator begin = PropiedadesParaJugador.begin();
		std::vector<std::string>::iterator end = PropiedadesParaJugador.end();


		for (std::vector<std::string>::iterator it = begin; it != end; it++) {
			std::string prop = *it;
			if (prop == propiedad->getName()){
				PropiedadesParaJugador.erase(it);
			}
		}

	}else{
		propiedad->setColor(Color3B::GREEN);
		PropiedadesParaJugador.push_back(propiedad->getName());
	}
}

void NegotiateScreen::cambiarPropiedades(Ref* pSender){
	rapidjson::Document::AllocatorType& allocator = partidaActualJSON.GetAllocator();
	std::vector<std::string>::iterator begin = PropiedadesParaOponente.begin();
	std::vector<std::string>::iterator end = PropiedadesParaOponente.end();

	for (std::vector<std::string>::iterator it = begin; it != end; it++) {
		std::string prop = *it;
		char nombre_propiedad[100];
		sprintf(nombre_propiedad,"%s",prop.c_str());
		partidaActualJSON["Partida1"][jugador]["Propiedades"].RemoveMember(prop.c_str());
		partidaActualJSON["Partida1"][oponente]["Propiedades"].AddMember(nombre_propiedad,0,allocator);
		cocos2d::log("Propiedad para Oponente: %s",prop.c_str());
	}

	std::vector<std::string>::iterator begin2 = PropiedadesParaJugador.begin();
	std::vector<std::string>::iterator end2 = PropiedadesParaJugador.end();

	for (std::vector<std::string>::iterator it = begin2; it != end2; it++) {
		std::string prop = *it;
		char nombre_propiedad[100];
		sprintf(nombre_propiedad,"%s",prop.c_str());
		partidaActualJSON["Partida1"][oponente]["Propiedades"].RemoveMember(prop.c_str());
		partidaActualJSON["Partida1"][jugador]["Propiedades"].AddMember(nombre_propiedad,0,allocator);
		cocos2d::log("Propiedad para Jugador: %s",prop.c_str());
	}

	this->escribirArchivoJSON();
	GameScreen* parent = (GameScreen *) this->getParent();
	parent->cargarInformacionJSON();
	parent->actualizarPuntuacion();
	parent->cargarSimbolosPropiedad();
	parent->actualizarConstrucciones();

	//cerramos el popup
	Jugadores.clear();
	PropiedadesJugadorCambio.clear();
	PropiedadesOponenteCambio.clear();
	this->getParent()->removeChildByName("Menu Negociar");

}

void NegotiateScreen::cargarArchivoJSON(){

	FILE* file = fopen("/data/data/org.ordominguez.monopoly/files/partidaActual.json", "r");
	FileStream fs(file);
	partidaActualJSON.ParseStream<0>(fs);

}

void NegotiateScreen::escribirArchivoJSON(){
	//actualizamos el turno en el JSON
	rapidjson::StringBuffer  buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	partidaActualJSON.Accept(writer);
	FILE* fileWrite = fopen("/data/data/org.ordominguez.monopoly/files/partidaActual.json", "w");
	if (fileWrite)
		{
		    fputs(buffer.GetString(),fileWrite);
		    fclose(fileWrite);
		}
}

void NegotiateScreen::cerrarNegociar(Ref* pSender){

	/*this->escribirArchivoJSON();
	GameScreen* parent = (GameScreen *) this->getParent();
	parent->cargarInformacionJSON();
	parent->actualizarPuntuacion();
	parent->actualizarConstrucciones();*/

	Jugadores.clear();
	PropiedadesJugadorCambio.clear();
	PropiedadesOponenteCambio.clear();
	this->getParent()->removeChildByName("Menu Negociar");
}


