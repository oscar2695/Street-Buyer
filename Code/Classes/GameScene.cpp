/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */
#include "GameScene.h"
#include "PauseMenu.h"
#include "BuildMenu.h"
#include "GUI/CCControlExtension/CCControlExtensions.h"
#include "json/document.h"
#include "json/filestream.h"
#include "json/prettywriter.h"
#include "json/stringbuffer.h"
#include "SimpleAudioEngine.h"
#include <vector>
#include <unistd.h>
using namespace rapidjson;

USING_NS_CC;
int casi;
float escala; //escala con la que se modifica el sprite del tablero
Size visibleSize; //tamaño visible en la pantalla del dispositivo
Vec2 origin; //origen de coordenadas del dispositivo
int PrimeraEjecucionDados=0;	//flag que controla el mostrado del label de los dados
Vector<MenuItem*> Propiedades(1); //Item menu necesario para la animación de  los dados
Document documentoJSON; //Documento donde se guardan los datos de ejecución
Document tableroJSON;
Document tarjetaEspecialesJSON;

//Fichas de cada uno de los jugadores
Sprite* fichaJugador1;
Sprite* fichaJugador2;
Sprite* fichaJugador3;
Sprite* fichaJugador4;

//Lista x es la lista de coordenadas x donde se colocará la ficha
int listax [40]={937,860,805,750,695,640,585,530,475,420,343,325,325,325,325,325,325,325,325,325,343,420,475,530,585,640,695,750,805,860,937,937,937,937,937,937,937,937,937,937};

//Lista y es la lista de coordenadas y donde se colocará la ficha
int listay [40]={75,75,75,75,75,75,75,75,75,75,75,155,210,265,320,375,430,485,540,595,675,675,675,675,675,675,675,675,675,675,675,595,540,485,430,375,320,265,210,155};

//Dados del juego
Sprite* dado1;
Sprite* dado2;

//jugador que esta jugando en este momento
int jugador;

//Banner con el dinero que tiene cada jugador
Label* dineroJugador1;
Label* dineroJugador2;
Label* dineroJugador3;
Label* dineroJugador4;

//Slider del menu subasta
cocos2d::extension::ControlSlider* sliderSubasta;
Label* label_puja;
std::vector<int> jugadores;
int jugador_ganador;
int puja_ganadora;


Scene* GameScreen::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameScreen::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool GameScreen::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    cocos2d::log("Resolucion: %f x %f",visibleSize.width,visibleSize.height);
    //Creamos y mostarmos el fondo

    auto background = Sprite::create("fondo_verde.jpg");
    background->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    background->setScale(visibleSize.height/background->getContentSize().height);
    this->addChild(background,-1,"background");

    //Colocamos el tablero en el centro de la pantalla dependiendo de como
    auto tablero = Sprite::create("img/tablero/tablero.jpg");
    escala = (visibleSize.height/tablero->getContentSize().height)*0.95;
    cocos2d::log("Escala del tablero: %f",escala);
    tablero->setScale(escala);
    tablero->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    this->addChild(tablero, 0,"tablero");


    //Colocamos los banners de puntuacion
    this->cargarInformacionJSON();
    this->crearBannersPuntuacion();


    //Crear las fichas segun el numero de jugadores
    this->crearFichas();

    //Crear titulos de propiedad
    this->crearMenuTarjetas();

    //Mostrar el cartel con el primer turno
    //this->mostrarTurno(1);
    cocos2d::log("Turno: %d",documentoJSON["Partida1"]["JugadorActual"].GetInt());
    this->mostrarTurno(documentoJSON["Partida1"]["JugadorActual"].GetInt());

    //Mostrar botones menu
    this->mostrarBotonPausa();
    this->mostrarBotonConstruir();

    this->cargarSimbolosPropiedad();



    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(GameScreen::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    return true;
}

void GameScreen::mostrarTarjeta(Ref* pSender){

	this->removeChildByName("popup");
	auto popup = Layer::create();
	auto fondo = Sprite::create("popup.png");
	fondo->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2));
	fondo->setScale(0.8,1);
	popup->addChild(fondo);
	popup->setScale(0.5);


	auto botonPopup = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(GameScreen::cerrarMostrarTarjeta, this));
	botonPopup->setPosition(Vec2(origin.x+visibleSize.width,origin.y+visibleSize.height));
	botonPopup->setScale(0.5);

	Node* node = (Node*) pSender;
	std::string name = node->getName();
	auto tarjeta_mostrar=Sprite::create(name);
	tarjeta_mostrar->setScale(2);
	tarjeta_mostrar->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2));
	popup->addChild(tarjeta_mostrar);

	auto menuPopup = Menu::create(botonPopup, nullptr);
	menuPopup->setPosition(Vec2::ZERO);
	popup->addChild(menuPopup,2);
	this->addChild(popup,3,"popup");

}

void GameScreen::cerrarMostrarTarjeta(Ref* pSender){
	//boton1->setEnabled(true);
	this->removeChildByName("popup",true);
}

void GameScreen::crearDados(Ref* pSender){


    //CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("audio/dieShuffle3.wav");
	//Nodo que trae el numero del jugador.
	Node* envia = (Node*) pSender;
	jugador=envia->getTag();
	cocos2d::log("%d",envia->getTag());

	//Comprobamos que sea la segunda vez que lo mostramos
	if(PrimeraEjecucionDados==1){
		this->removeChildByName("label_prueba",true);
	}

	//Creamos la animacion para el primer dado
	Vector<SpriteFrame*> animFrames(15);
	char str[100] = {0};
	int num1;

	for(int i = 1; i < 15; i++)
	{
		num1=rand() % 6+1;
	    sprintf(str, "dice-%d-md.png",num1);
	    auto frame = SpriteFrame::create(str,Rect(0,0,300,300));
	    animFrames.pushBack(frame);
	}

	auto animation = Animation::createWithSpriteFrames(animFrames, 0.15f);
	auto animate = Animate::create(animation);

	//Creamos la animacion para el segundo dado
	Vector<SpriteFrame*> animFrames2(15);
	char str2[100] = {0};
	int num2;

	for(int i = 1; i < 15; i++)
	{
		num2=rand() % 6+1;
	    sprintf(str2, "dice-%d-md.png",num2);
	    auto frame2 = SpriteFrame::create(str2,Rect(0,0,300,300));
	    animFrames2.pushBack(frame2);
	}

	auto animation2 = Animation::createWithSpriteFrames(animFrames2, 0.15f);
	auto animate2 = Animate::create(animation2);
	PrimeraEjecucionDados=1;


	char str3[10] = {0};
	sprintf(str3,"Jugador%d",jugador);
	int pos = documentoJSON["Partida1"][str3]["Position"].GetInt();
	int tirada=num1+num2;


	cocos2d::log("Jugador que esta tirando: %s",str3);
	cocos2d::log("Posicion de la ficha: %d",pos);
	cocos2d::log("lugar en el que se colocara: %d",pos+tirada);

	if(pos+tirada>=40){
		documentoJSON["Partida1"][str3]["Position"]=pos+tirada-40;
		documentoJSON["Partida1"][str3]["Dinero"]=documentoJSON["Partida1"][str3]["Dinero"].GetInt()+200;
		char str_dinero[10] = {0};
		sprintf(str_dinero,"%d $",documentoJSON["Partida1"][str3]["Dinero"].GetInt());
		if (jugador==1){
			dineroJugador1->setString(str_dinero);
			cocos2d::log("Estoy modificando el dinero del jugador 1");
		}
		if (jugador==2){
			dineroJugador2->setString(str_dinero);
			cocos2d::log("Estoy modificando el dinero del jugador 2");
		}
		if (jugador==3){
			dineroJugador3->setString(str_dinero);
			cocos2d::log("Estoy modificando el dinero del jugador 3");
		}
		if (jugador==4){
			dineroJugador4->setString(str_dinero);
			cocos2d::log("Estoy modificando el dinero del jugador 4");
		}

	}else{
		documentoJSON["Partida1"][str3]["Position"]=pos+tirada;
	}


	dado1->runAction(animate);
	dado2->runAction(animate2);

	this->lanzadorFuncionesJuego(jugador,pos,tirada);


}

void GameScreen::cerrarPopupDados(){
	this->removeChildByName("popup-dados");
}

void GameScreen::moverFicha(Sprite* ficha,int pos,int tirada){
	if(tirada+pos>10 and pos>=0 and pos<=10){
		auto secuencia = Sequence::create(MoveTo::create(1,Vec2(listax[10],listay[10])),
										  MoveTo::create(1,Vec2(listax[tirada+pos],listay[tirada+pos])),
										  nullptr);
		ficha->runAction(secuencia);

	}else{
		if(tirada+pos>20 and pos>=10 and pos<=20){
				auto secuencia = Sequence::create(MoveTo::create(1,Vec2(listax[20],listay[20])),
												  MoveTo::create(1,Vec2(listax[tirada+pos],listay[tirada+pos])),
												  nullptr);
				ficha->runAction(secuencia);

			}else{
				if(tirada+pos>30 and pos>=20 and pos<=30){
					auto secuencia = Sequence::create(MoveTo::create(1,Vec2(listax[30],listay[30])),
													  MoveTo::create(1,Vec2(listax[tirada+pos],listay[tirada+pos])),
													  nullptr);
					ficha->runAction(secuencia);

				}else{
					if(tirada+pos>40 and pos>=30 and pos<=40){
						auto secuencia = Sequence::create(MoveTo::create(1,Vec2(listax[0],listay[0])),
														  MoveTo::create(1,Vec2(listax[tirada+pos-40],listay[tirada+pos-40])),
													      nullptr);
						ficha->runAction(secuencia);


						}else{
							ficha->runAction(MoveTo::create(1,Vec2(listax[tirada+pos],listay[tirada+pos])));
						}
				}
			}

	}
	//cocos2d::log("%s",str);
	//cocos2d::log("%s",tableroJSON[str]["Nombre"].GetString());
}

void GameScreen::mostrarPopupDados(Ref* pSender){

	this->removeChildByName("layer_turno");
	Node* envia = (Node* )pSender;
	auto popupDados = Layer::create();

	dado1 = Sprite::create("dice-1-md.png");
	dado1->setPosition(Vec2(origin.x+visibleSize.width/2-dado1->getContentSize().width/5,origin.y+visibleSize.height/2));
	dado1->setScale(0.3);

	dado2 = Sprite::create("dice-1-md.png");
	dado2->setPosition(Vec2(origin.x+visibleSize.width/2+dado1->getContentSize().width/5,origin.y+visibleSize.height/2));
	dado2->setScale(0.3);

	popupDados->addChild(dado1);
	popupDados->addChild(dado2);

	auto botonDados = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(GameScreen::crearDados, this));
	botonDados->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2-botonDados->getContentSize().height));
	botonDados->setScale(0.5);
	botonDados->setTag(envia->getTag());

	auto menuDados = Menu::create(botonDados, nullptr);
	menuDados->setPosition(Vec2::ZERO);
	popupDados->addChild(menuDados,3,"dados");

	auto label_tirar_dados = Label::createWithTTF("Tirar","fonts/Marker Felt.ttf",32);
	label_tirar_dados->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2-botonDados->getContentSize().height));
	label_tirar_dados->setColor(Color3B(255,255,255));
	popupDados->addChild(label_tirar_dados,3);
	this->addChild(popupDados,5,"popup-dados");
}

void GameScreen::crearMenuTarjetas(){
	//calculamos dinamicamente la separacion entre las tarjetas de propiedad y las posiciones relativas necesarias
	auto muestra = MenuItemImage::create("img/propiedades/propiedad-plaza-de-comercio.jpg","img/propiedades/propiedad-plaza-de-comercio.jpg",CC_CALLBACK_1(GameScreen::mostrarTarjeta, this));
	float separacion = visibleSize.height/20.5;
	float columna1 = origin.x+visibleSize.width-((muestra->getContentSize().width/4)*2.8);
	float columna2 = origin.x+visibleSize.width-(muestra->getContentSize().width/4);
	float altura_inicial= origin.y+visibleSize.height-(((muestra->getContentSize().height)*0.4)/2)*1.2;

	//Creamos cada una de las tarjetas
	crearTituloPropiedad("img/propiedades/propiedad-plaza-de-comercio.jpg",columna1,altura_inicial);
	crearTituloPropiedad("img/propiedades/propiedad-calle-augusta.jpg",columna1,altura_inicial-separacion);
	crearTituloPropiedad("img/propiedades/propiedad-plaza-roja.jpg",columna1,altura_inicial-separacion*2);
	crearTituloPropiedad("img/propiedades/propiedad-kremlin.jpg",columna1,altura_inicial-separacion*3);
	crearTituloPropiedad("img/propiedades/propiedad-teatro-bolshoi.jpg",columna1,altura_inicial-separacion*4);
	crearTituloPropiedad("img/propiedades/propiedad-gran-via.jpg",columna1,altura_inicial-separacion*5);
	crearTituloPropiedad("img/propiedades/propiedad-puerta-del-sol.jpg",columna1,altura_inicial-separacion*6);
	crearTituloPropiedad("img/propiedades/propiedad-plaza-de-cibeles.jpg",columna1,altura_inicial-separacion*7);
	crearTituloPropiedad("img/propiedades/propiedad-kudamm.jpg",columna1,altura_inicial-separacion*8);
	crearTituloPropiedad("img/propiedades/propiedad-posdamer-platz.jpg",columna1,altura_inicial-separacion*9);
	crearTituloPropiedad("img/propiedades/propiedad-bebelplatz.jpg",columna1,altura_inicial-separacion*10);
	crearTituloPropiedad("img/propiedades/propiedad-plaza-de-san-pedro.jpg",columna1,altura_inicial-separacion*11);
	crearTituloPropiedad("img/propiedades/propiedad-coliseo.jpg",columna1,altura_inicial-separacion*12);
	crearTituloPropiedad("img/propiedades/propiedad-plaza-de-campidoglio.jpg",columna1,altura_inicial-separacion*13);
	crearTituloPropiedad("img/propiedades/propiedad-grand-place.jpg",columna2,altura_inicial);
	crearTituloPropiedad("img/propiedades/propiedad-manneken-pis.jpg",columna2,altura_inicial-separacion);
	crearTituloPropiedad("img/propiedades/propiedad-atomium.jpg",columna2,altura_inicial-separacion*2);
	crearTituloPropiedad("img/propiedades/propiedad-los-campos-eliseos.jpg",columna2,altura_inicial-separacion*3);
	crearTituloPropiedad("img/propiedades/propiedad-avenida-de-la-opera.jpg",columna2,altura_inicial-separacion*4);
	crearTituloPropiedad("img/propiedades/propiedad-calle-rivoli.jpg",columna2,altura_inicial-separacion*5);
	crearTituloPropiedad("img/propiedades/propiedad-trafalgar-square.jpg",columna2,altura_inicial-separacion*6);
	crearTituloPropiedad("img/propiedades/propiedad-piccadilly-circus.jpg",columna2,altura_inicial-separacion*7);
	crearTituloPropiedad("img/propiedades/compania-aguas.jpg",columna2,altura_inicial-separacion*8);
	crearTituloPropiedad("img/propiedades/compania-telecomunicaciones.jpg",columna2,altura_inicial-separacion*9);
	crearTituloPropiedad("img/propiedades/compania-gas.jpg",columna2,altura_inicial-separacion*10);
	crearTituloPropiedad("img/propiedades/compania-electricidad.jpg",columna2,altura_inicial-separacion*11);
	crearTituloPropiedad("img/propiedades/estacion-trenes.jpg",columna2,altura_inicial-separacion*12);
	crearTituloPropiedad("img/propiedades/estacion-metro.jpg",columna2,altura_inicial-separacion*13);
	crearTituloPropiedad("img/propiedades/aeropuerto.jpg",columna2,altura_inicial-separacion*14);
	crearTituloPropiedad("img/propiedades/estacion-autobuses.jpg",columna1,altura_inicial-separacion*14);


	//añadimos el menu para cuando hacemos click en una tarjeta
	auto menuPropiedades = Menu::createWithArray(Propiedades);
	menuPropiedades->setPosition(Vec2::ZERO);
	this->addChild(menuPropiedades,0);

}

void GameScreen::crearTituloPropiedad(std::string nombre, float columna, float fila){
	auto tarjeta = MenuItemImage::create(nombre,nombre,CC_CALLBACK_1(GameScreen::mostrarTarjeta, this));
	tarjeta->setScale(0.4);
	tarjeta->setPosition(Vec2(columna,fila));
	tarjeta->setName(nombre);

	Propiedades.pushBack(tarjeta);
}

void GameScreen::cargarInformacionJSON(){
	 //Leemos el archivo de configuracion

	    FILE* file = fopen("/data/data/org.ordominguez.monopoly/files/partidaActual.json", "r");
	    if(file==NULL){
	    	auto valorJson = Label::createWithTTF("no existe el archivo","fonts/Kabel Regular.ttf",32);
	    	valorJson->setColor(Color3B(0,0,0));
	    	valorJson->setPosition(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2);
	    	this->addChild(valorJson,2,"valor_json");
	    }
	    FileStream fs(file);
	    documentoJSON.ParseStream<0>(fs);

	 //Cargamos la información referente al tablero

	    std::string fullPath = FileUtils::getInstance()->fullPathForFilename( "json/tablero.json" );

	    std::string doc = FileUtils::getInstance()->getStringFromFile( fullPath );

	    tableroJSON.Parse<0>(doc.c_str());

	 //Cargamos informacion de las tarjetas especiales

	    std::string fullPathEspeciales = FileUtils::getInstance()->fullPathForFilename( "json/tarjetasEspeciales.json" );

	    std::string docEspeciales = FileUtils::getInstance()->getStringFromFile( fullPathEspeciales );

	    tarjetaEspecialesJSON.Parse<0>(docEspeciales.c_str());

}

void GameScreen::crearBannersPuntuacion(){
	int numJugadores = documentoJSON["Partida1"]["NumeroJugadores"].GetInt();
	cocos2d::log("Numero de jugadores en el game: %d",numJugadores);
	if(numJugadores>=2){
		auto banner1 = Sprite::create("banner-puntuacion-verde.png");
		auto banner2 = Sprite::create("banner-puntuacion-rojo.png");

		banner1->setPosition(Vec2(origin.x+banner1->getContentSize().width*0.6,origin.y+visibleSize.height-banner1->getContentSize().height*0.7));
		banner2->setPosition(Vec2(origin.x+banner1->getContentSize().width*0.6,origin.y+visibleSize.height-banner1->getContentSize().height*1.8));

		this->addChild(banner1,0,"banner1");
		this->addChild(banner2,0,"banner2");

		auto nombreJugador1 = Label::createWithTTF(documentoJSON["Partida1"]["Jugador1"]["Nombre"].GetString(),"fonts/Kabel Bold.ttf",26);
		nombreJugador1->setAnchorPoint(Vec2(0,0.5f));
		nombreJugador1->setPosition(Vec2(origin.x+banner1->getContentSize().width*0.2,origin.y+visibleSize.height-banner1->getContentSize().height*0.5));
		this->addChild(nombreJugador1);

		auto nombreJugador2 = Label::createWithTTF(documentoJSON["Partida1"]["Jugador2"]["Nombre"].GetString(),"fonts/Kabel Bold.ttf",26);
		nombreJugador2->setAnchorPoint(Vec2(0,0.5f));
		nombreJugador2->setPosition(Vec2(origin.x+banner1->getContentSize().width*0.2,origin.y+visibleSize.height-banner1->getContentSize().height*1.6));
		this->addChild(nombreJugador2);


		char mostrar1 [100] = {0};
		sprintf(mostrar1, "%d $",documentoJSON["Partida1"]["Jugador1"]["Dinero"].GetInt());
		std::string dinero1 (mostrar1);

		dineroJugador1 = Label::createWithTTF(dinero1,"fonts/Kabel Bold.ttf",26);
		dineroJugador1->setAnchorPoint(Vec2(1,0.5f));
		dineroJugador1->setPosition(Vec2(origin.x+banner1->getContentSize().width,origin.y+visibleSize.height-banner1->getContentSize().height*0.9));
		this->addChild(dineroJugador1);


		char mostrar2 [100] = {0};
		sprintf(mostrar2, "%d $",documentoJSON["Partida1"]["Jugador2"]["Dinero"].GetInt());
		std::string dinero2 (mostrar2);

		dineroJugador2 = Label::createWithTTF(dinero2,"fonts/Kabel Bold.ttf",26);
		dineroJugador2->setAnchorPoint(Vec2(1,0.5f));
		dineroJugador2->setPosition(Vec2(origin.x+banner1->getContentSize().width,origin.y+visibleSize.height-banner1->getContentSize().height*2.0));
		this->addChild(dineroJugador2);
	}
	if(numJugadores>=3){
		auto banner3 = Sprite::create("banner-puntuacion-azul.png");

		banner3->setPosition(Vec2(origin.x+banner3->getContentSize().width*0.6,origin.y+visibleSize.height-banner3->getContentSize().height*2.9));

		this->addChild(banner3,0,"banner3");

		auto nombreJugador3 = Label::createWithTTF(documentoJSON["Partida1"]["Jugador3"]["Nombre"].GetString(),"fonts/Kabel Bold.ttf",26);
		nombreJugador3->setAnchorPoint(Vec2(0,0.5f));
		nombreJugador3->setPosition(Vec2(origin.x+banner3->getContentSize().width*0.2,origin.y+visibleSize.height-banner3->getContentSize().height*2.7));
		this->addChild(nombreJugador3);

		char mostrar3 [100] = {0};
		sprintf(mostrar3, "%d $",documentoJSON["Partida1"]["Jugador3"]["Dinero"].GetInt());
		std::string dinero3 (mostrar3);

		dineroJugador3 = Label::createWithTTF(dinero3,"fonts/Kabel Bold.ttf",26);
		dineroJugador3->setAnchorPoint(Vec2(1,0.5f));
		dineroJugador3->setPosition(Vec2(origin.x+banner3->getContentSize().width,origin.y+visibleSize.height-banner3->getContentSize().height*3.1));
		this->addChild(dineroJugador3);
	}
	if(numJugadores==4){
		auto banner4 = Sprite::create("banner-puntuacion-naranja.png");

		banner4->setPosition(Vec2(origin.x+banner4->getContentSize().width*0.6,origin.y+visibleSize.height-banner4->getContentSize().height*4.0));

		this->addChild(banner4,0,"banner4");

		auto nombreJugador4 = Label::createWithTTF(documentoJSON["Partida1"]["Jugador4"]["Nombre"].GetString(),"fonts/Kabel Bold.ttf",26);
		nombreJugador4->setAnchorPoint(Vec2(0,0.5f));
		nombreJugador4->setPosition(Vec2(origin.x+banner4->getContentSize().width*0.2,origin.y+visibleSize.height-banner4->getContentSize().height*3.8));
		this->addChild(nombreJugador4);

		char mostrar4 [100] = {0};
		sprintf(mostrar4, "%d $",documentoJSON["Partida1"]["Jugador4"]["Dinero"].GetInt());
		std::string dinero4 (mostrar4);

		dineroJugador4 = Label::createWithTTF(dinero4,"fonts/Kabel Bold.ttf",26);
		dineroJugador4->setAnchorPoint(Vec2(1,0.5f));
		dineroJugador4->setPosition(Vec2(origin.x+banner4->getContentSize().width,origin.y+visibleSize.height-banner4->getContentSize().height*4.2));
		this->addChild(dineroJugador4);
	}
}

void GameScreen::mostrarBotonPausa(){

	auto pausa = MenuItemImage::create("boton_pausa.png","boton_pausa.png",CC_CALLBACK_1(GameScreen::mostrarMenuPausa, this));
	pausa->setPosition(Vec2(origin.x+pausa->getContentSize().width/2,origin.y+pausa->getContentSize().height/2));
	pausa->setScale(0.5);
	pausa->setName("boton_pausa");

	auto menuPausa = Menu::create(pausa, nullptr);
	menuPausa->setPosition(Vec2::ZERO);
	this->addChild(menuPausa,0);
}

void GameScreen::mostrarMenuPausa(Ref* pSender){
	this->escribirArchivoJSON();
	auto layer = PauseScreen::create();
	layer->setScale(0.45);

	this->addChild(layer,5,"Menu Pausa");
}

void GameScreen::mostrarBotonConstruir(){

	auto construir = MenuItemImage::create("boton_pausa.png","boton_pausa.png",CC_CALLBACK_1(GameScreen::mostrarMenuConstruir, this));
	construir->setPosition(Vec2(origin.x+construir->getContentSize().width/2,origin.y+construir->getContentSize().height/2*3));
	construir->setScale(0.5);
	construir->setName("boton_construir");

	auto menuPausa = Menu::create(construir, nullptr);
	menuPausa->setPosition(Vec2::ZERO);
	this->addChild(menuPausa,0);
}

void GameScreen::mostrarMenuConstruir(Ref* pSender){
	this->escribirArchivoJSON();
	auto layer = BuildScreen::create();
	layer->setScale(0.8);

	this->addChild(layer,5,"Menu Construir");
}

void GameScreen::mostrarTurno(int jugador){
	auto layer = Layer::create();

	auto fondo = Sprite::create("turno.png");
	fondo->setPosition(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2);
	fondo->setScale(layer->getContentSize().height/fondo->getContentSize().height);
	layer->addChild(fondo);

	auto boton = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(GameScreen::mostrarPopupDados,this));
	boton->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height*0.2));
	boton->setTag(jugador);
	//boton_atras->setScale(1.5);

	auto menu = Menu::create(boton, nullptr);
	menu->setPosition(Vec2::ZERO);
	layer->addChild(menu,2);

    auto label = Label::createWithTTF("Jugar","fonts/Kabel Regular.ttf",50);
    label->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height*0.2));
    label->setColor(Color3B(255,255,255));
    layer->addChild(label,2,"label_boton");

    char str[100] = {0};
    sprintf(str,"Jugador%d",jugador);
    std::string texto_nombre = documentoJSON["Partida1"][str]["Nombre"].GetString();

    auto nombre = Label::createWithTTF(texto_nombre,"fonts/Kabel Regular.ttf",100);
    nombre->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2));
    nombre->setColor(Color3B(255,255,255));
    layer->addChild(nombre,2,"nombre");

    layer->setScale(0.3);
    this->addChild(layer,2,"layer_turno");
}

void GameScreen::crearFichas(){
	int numJugadores = documentoJSON["Partida1"]["NumeroJugadores"].GetInt();


	if(numJugadores>=2){

		int posicion1 = documentoJSON["Partida1"]["Jugador1"]["Position"].GetInt();
		fichaJugador1=Sprite::create("ficha_verde.png");
		fichaJugador1->setPosition(Vec2(listax[posicion1]-10,listay[posicion1]));
		fichaJugador1->setScale(0.5);
		this->addChild(fichaJugador1,1);

		int posicion2 = documentoJSON["Partida1"]["Jugador2"]["Position"].GetInt();
		fichaJugador2=Sprite::create("ficha_roja.png");
		fichaJugador2->setPosition(Vec2(listax[posicion2],listay[posicion2]));
		fichaJugador2->setScale(0.5);
		this->addChild(fichaJugador2,1);
	}
	if(numJugadores>=3){
		int posicion3 = documentoJSON["Partida1"]["Jugador3"]["Position"].GetInt();
		fichaJugador3=Sprite::create("ficha_azul.png");
		fichaJugador3->setPosition(Vec2(listax[posicion3]+10,listay[posicion3]));
		fichaJugador3->setScale(0.5);
		this->addChild(fichaJugador3,1);
	}
	if(numJugadores>=4){
		int posicion4 = documentoJSON["Partida1"]["Jugador4"]["Position"].GetInt();
		fichaJugador4=Sprite::create("ficha_naranja.png");
		fichaJugador4->setPosition(Vec2(listax[posicion4]+20,listay[posicion4]));
		fichaJugador4->setScale(0.5);
		this->addChild(fichaJugador4,1);
	}
}

void GameScreen::escribirArchivoJSON(){
	//actualizamos el turno en el JSON

	cocos2d::log("Jugador actual pausa: %d",documentoJSON["Partida1"]["JugadorActual"].GetInt());
	cocos2d::log("Numero Jugadores pausa: %d",documentoJSON["Partida1"]["NumeroJugadores"].GetInt());
	if (documentoJSON["Partida1"]["JugadorActual"].GetInt()==documentoJSON["Partida1"]["NumeroJugadores"].GetInt()){
		documentoJSON["Partida1"]["JugadorActual"].SetInt(1);
	}else{
		documentoJSON["Partida1"]["JugadorActual"].SetInt(jugador+1);
	}

	rapidjson::StringBuffer  buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	documentoJSON.Accept(writer);
	FILE* fileWrite = fopen("/data/data/org.ordominguez.monopoly/files/partidaActual.json", "w");
	if (fileWrite)
		{
		    fputs(buffer.GetString(),fileWrite);
		    fclose(fileWrite);
		}
}

void GameScreen::lanzadorFuncionesJuego(int jugador,int pos,int tirada){
	const float myDelay = 4.0f;

	int siguiente;
	int numjugadores = documentoJSON["Partida1"]["NumeroJugadores"].GetInt();

	//aqui comprobamos cual es el jugador siguiente
	if(jugador<numjugadores){
		siguiente=jugador+1;

	}else{
		siguiente=1;
	}

	if(jugador==1){
		this->runAction( Sequence::create(
		                                  DelayTime::create(myDelay),
		                                  CallFunc::create(std::bind(&GameScreen::moverFicha, this, fichaJugador1, pos, tirada)),
		                                  CallFunc::create(CC_CALLBACK_0(GameScreen::cerrarPopupDados, this)),
		                                  CallFunc::create(std::bind(&GameScreen::comprobarCasilla, this, jugador)),
		                                  CallFunc::create(std::bind(&GameScreen::mostrarTurno, this, siguiente)),
		                                  nullptr));
	}
	if(jugador==2){
		this->runAction( Sequence::create(
		                                  DelayTime::create(myDelay),
		                                  CallFunc::create(std::bind(&GameScreen::moverFicha, this, fichaJugador2, pos, tirada)),
		                                  CallFunc::create(CC_CALLBACK_0(GameScreen::cerrarPopupDados, this)),
		                                  CallFunc::create(std::bind(&GameScreen::comprobarCasilla, this, jugador)),
		                                  CallFunc::create(std::bind(&GameScreen::mostrarTurno, this, siguiente)),
		                                  nullptr));
	}
	if(jugador==3){
		this->runAction( Sequence::create(
		                                  DelayTime::create(myDelay),
		                                  CallFunc::create(std::bind(&GameScreen::moverFicha, this, fichaJugador3, pos, tirada)),
		                                  CallFunc::create(CC_CALLBACK_0(GameScreen::cerrarPopupDados, this)),
		                                  CallFunc::create(std::bind(&GameScreen::comprobarCasilla, this, jugador)),
		                                  CallFunc::create(std::bind(&GameScreen::mostrarTurno, this, siguiente)),
		                                  nullptr));
	}
	if(jugador==4){
		this->runAction( Sequence::create(
		                                  DelayTime::create(myDelay),
		                                  CallFunc::create(std::bind(&GameScreen::moverFicha, this, fichaJugador4, pos, tirada)),
		                                  CallFunc::create(CC_CALLBACK_0(GameScreen::cerrarPopupDados, this)),
		                                  CallFunc::create(std::bind(&GameScreen::comprobarCasilla, this, jugador)),
		                                  CallFunc::create(std::bind(&GameScreen::mostrarTurno, this, siguiente)),
		                                  nullptr));
	}

}

void GameScreen::comprobarCasilla(int jugador){
	char str_jugador[10] = {0};
	sprintf(str_jugador, "Jugador%d",jugador);

	int pos=documentoJSON["Partida1"][str_jugador]["Position"].GetInt();

	char str_pos[1] = {0};
	sprintf(str_pos,"%d",pos);

	std::string tipo = tableroJSON[str_pos]["Tipo"].GetString();

	cocos2d::log("Tipo de casilla: %s",tipo.c_str());

	if(tipo == "Propiedad"){
		const char * propiedad = tableroJSON[str_pos]["Nombre"].GetString();
		cocos2d::log("Esta es la propiedad que miramos si esta comprada: %s",propiedad);
		if (!documentoJSON["Partida1"]["Jugador1"]["Propiedades"].HasMember(propiedad)
				&& !documentoJSON["Partida1"]["Jugador2"]["Propiedades"].HasMember(propiedad)
				&& !documentoJSON["Partida1"]["Jugador3"]["Propiedades"].HasMember(propiedad)
				&& !documentoJSON["Partida1"]["Jugador4"]["Propiedades"].HasMember(propiedad))
		{
			menuComprarSubastar(pos);

		}else{
			cocos2d::log("Propiedad ya comprada: Pagando por el alquiler");
			this->pagarAlquiler(pos);
		}
	}

	if(tipo == "Impuesto"){
		int dinero = documentoJSON["Partida1"][str_jugador]["Dinero"].GetInt()-tableroJSON[str_pos]["Cantidad"].GetInt();
		documentoJSON["Partida1"][str_jugador]["Dinero"].SetInt(dinero);
		this->actualizarPuntuacion();
	}

	if(tipo == "Estacion"){
		const char * estacion = tableroJSON[str_pos]["Nombre"].GetString();
				cocos2d::log("Esta es la propiedad que miramos si esta comprada: %s",estacion);
				if (!documentoJSON["Partida1"]["Jugador1"]["Propiedades"].HasMember(estacion)
						&& !documentoJSON["Partida1"]["Jugador2"]["Propiedades"].HasMember(estacion)
						&& !documentoJSON["Partida1"]["Jugador3"]["Propiedades"].HasMember(estacion)
						&& !documentoJSON["Partida1"]["Jugador4"]["Propiedades"].HasMember(estacion))
				{
					menuComprarSubastar(pos);

				}else{
					cocos2d::log("Estacion ya comprada: Pagando por el alquiler");
				}
	}

	if(tipo == "Compania"){
			const char * compania = tableroJSON[str_pos]["Nombre"].GetString();
					cocos2d::log("Esta es la propiedad que miramos si esta comprada: %s",compania);
					if (!documentoJSON["Partida1"]["Jugador1"]["Propiedades"].HasMember(compania)
							&& !documentoJSON["Partida1"]["Jugador2"]["Propiedades"].HasMember(compania)
							&& !documentoJSON["Partida1"]["Jugador3"]["Propiedades"].HasMember(compania)
							&& !documentoJSON["Partida1"]["Jugador4"]["Propiedades"].HasMember(compania))
					{
						menuComprarSubastar(pos);

					}else{
						cocos2d::log("Compania ya comprada: Pagando por el alquiler");
					}
		}

	if(tipo == "Tarjeta"){
		std::string nombre = tableroJSON[str_pos]["Nombre"].GetString();
		if (nombre == "suerte"){
			this->tarjetaSuerte();
		}else{
			this->tarjetaComunidad();
		}
	}


}

void GameScreen::menuComprarSubastar(int propiedad){
	auto popupVenta = Layer::create();
	auto fondo = Sprite::create("popup.png");
	fondo->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2));
	popupVenta->addChild(fondo);
	popupVenta->setScale(0.5);

	char str_numero[5] = {0};
	sprintf(str_numero, "%d",propiedad);
	char str_propiedad[100] = {0};
	sprintf(str_propiedad, "img/propiedades/%s.jpg",tableroJSON[str_numero]["Nombre"].GetString());
	auto tarjeta_mostrar=Sprite::create(str_propiedad);
	tarjeta_mostrar->setScale(1.8);
	tarjeta_mostrar->setPosition(Vec2(origin.x+tarjeta_mostrar->getContentSize().width*0.6,origin.y+visibleSize.height/2));
	popupVenta->addChild(tarjeta_mostrar);

	auto botonVenta = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(GameScreen::crearVenta, this));
	botonVenta->setPosition(Vec2(origin.x+visibleSize.width-tarjeta_mostrar->getContentSize().width,origin.y+visibleSize.height/2));
	botonVenta->setScale(1.7);
	botonVenta->setTag(propiedad);

    auto labelComprar = Label::createWithTTF("Comprar","fonts/Kabel Regular.ttf",50);
    labelComprar->setPosition(Vec2(origin.x+visibleSize.width-tarjeta_mostrar->getContentSize().width,origin.y+visibleSize.height/2));
    labelComprar->setColor(Color3B(255,255,255));
    popupVenta->addChild(labelComprar,4,"labelComprar");


	auto botonSubasta = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(GameScreen::crearSubasta, this));
	botonSubasta->setPosition(Vec2(origin.x+visibleSize.width-tarjeta_mostrar->getContentSize().width,origin.y+botonVenta->getContentSize().height));
	botonSubasta->setScale(1.7);
	botonSubasta->setTag(propiedad);

    auto labelSubastar = Label::createWithTTF("Subastar","fonts/Kabel Regular.ttf",50);
    labelSubastar->setPosition(Vec2(origin.x+visibleSize.width-tarjeta_mostrar->getContentSize().width,origin.y+botonVenta->getContentSize().height));
    labelSubastar->setColor(Color3B(255,255,255));
    popupVenta->addChild(labelSubastar,4,"labelSubastar");

    char str[30] = {0};
    sprintf(str,"Precio\n  %d $",tableroJSON[str_numero]["Precio"].GetInt());
    auto labelPrecio = Label::createWithTTF(str,"fonts/Kabel Regular.ttf",60);
    labelPrecio->setPosition(Vec2(origin.x+visibleSize.width-tarjeta_mostrar->getContentSize().width,origin.y+visibleSize.height-botonVenta->getContentSize().height));
    labelPrecio->setColor(Color3B(255,255,255));
    popupVenta->addChild(labelPrecio,4,"labelPrecio");

	auto menuPopup = Menu::create(botonVenta, botonSubasta, nullptr);
	menuPopup->setPosition(Vec2::ZERO);
	popupVenta->addChild(menuPopup,2);
	this->addChild(popupVenta,3,"popupVenta");

	char str_jugador[10] = {0};
	sprintf(str_jugador, "Jugador%d",jugador);
	if (documentoJSON["Partida1"][str_jugador]["Dinero"].GetInt()<tableroJSON[str_numero]["Precio"].GetInt()){
		botonVenta->setEnabled(false);
	}
}

void GameScreen::cerrarComprarSubastar(Ref* pSender){
	//boton1->setEnabled(true);
	this->removeChildByName("popupVenta",true);
}

void GameScreen::crearVenta(Ref* pSender){
	cocos2d::log("Comprada");
	Node * nodo = (Node*) pSender;
	int propiedad = nodo->getTag();

	this->anadirPropiedad(propiedad);
	this->crearMenuVenta(propiedad);
	this->removeChildByName("popupVenta");

	this->runAction( Sequence::create(
			                                  DelayTime::create(5.0f),
			                                  CallFunc::create(CC_CALLBACK_0(GameScreen::cerrarMenuVenta, this)),
			                                  nullptr));

}

void GameScreen::crearMenuVenta(int propiedad){
	Layer * venta = Layer::create();
	auto fondo = Sprite::create("popup_venta.png");
	fondo->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2));
	venta->addChild(fondo);

	char str_propiedad[5] = {0};
	sprintf(str_propiedad,"%d",propiedad);
	char str_dinero[20] = {0};
	sprintf(str_dinero, "%d $",tableroJSON[str_propiedad]["Precio"].GetInt());
	auto labelPrecio = Label::createWithTTF(str_dinero,"fonts/Kabel Regular.ttf",100);
	labelPrecio->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y));
	labelPrecio->setColor(Color3B(255,255,255));
	venta->addChild(labelPrecio,4,"labelPrecio");

	char nombre_propiedad[100] = {0};
	sprintf(nombre_propiedad,"img/propiedades/%s.jpg",tableroJSON[str_propiedad]["Nombre"].GetString());
	auto tarjeta = Sprite::create(nombre_propiedad);
	tarjeta->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height*0.9));
	tarjeta->setScale(0.6);
	venta->addChild(tarjeta);

	Sprite* ficha;
	if(jugador==1){
		ficha = Sprite::create("ficha_verde.png");
	}else{
		if(jugador==2){
			ficha = Sprite::create("ficha_roja.png");
		}else{
			if(jugador==3){
				ficha = Sprite::create("ficha_azul.png");
			}else{
				if(jugador==4){
					ficha = Sprite::create("ficha_naranja.png");
				}
			}
		}
	}
	ficha->setPosition(Vec2(origin.x+visibleSize.width*0.9,origin.y+visibleSize.height*0.45));
	ficha->setScale(4);
	venta->addChild(ficha);
	venta->setScale(0.5);
	this->addChild(venta,4,"MenuVenta");
}

void GameScreen::cerrarMenuVenta(){
	this->removeChildByName("MenuVenta");
}

void GameScreen::crearSubasta(Ref* pSender){
	cocos2d::log("Comprada");
	Node * nodo = (Node*) pSender;
	int propiedad = nodo->getTag();

	int numJugadores = documentoJSON["Partida1"]["NumeroJugadores"].GetInt();
	for (int i =1;i<=numJugadores;i++){
		jugadores.push_back(i);
	}
	cocos2d::log("Jugador : %d", jugadores[2]);

	this->crearMenuSubasta(propiedad,0,1);
	this->removeChildByName("popupVenta");

	//this->removeChildByName("popupVenta");
}

void GameScreen::crearMenuSubasta(int propiedad, int puja,int jugador){
	this->mostrarPopupPuja(jugador);

	this->runAction( Sequence::create(
			                                  DelayTime::create(3.0f),
			                                  CallFunc::create(CC_CALLBACK_0(GameScreen::cerrarPopupPuja, this)),
			                                  CallFunc::create(std::bind(&GameScreen::crearMenuPuja, this, propiedad, puja, jugador)),
			                                  nullptr));

}

void GameScreen::crearMenuPuja(int propiedad, int puja,int jugador){
	auto popupSubasta = Layer::create();
	auto fondo = Sprite::create("popup.png");
	fondo->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2));
	popupSubasta->addChild(fondo);
	popupSubasta->setScale(0.5);

	char str_numero[5] = {0};
	sprintf(str_numero, "%d",propiedad);
	char str_propiedad[100] = {0};
	sprintf(str_propiedad, "img/propiedades/%s.jpg",tableroJSON[str_numero]["Nombre"].GetString());
	auto tarjeta_mostrar=Sprite::create(str_propiedad);
	tarjeta_mostrar->setScale(1.8);
	tarjeta_mostrar->setPosition(Vec2(origin.x+tarjeta_mostrar->getContentSize().width*0.6,origin.y+visibleSize.height/2));
	popupSubasta->addChild(tarjeta_mostrar);

	auto botonPujar = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(GameScreen::pujarJugador, this));
	botonPujar->setPosition(Vec2(origin.x+visibleSize.width*0.6,origin.y+botonPujar->getContentSize().height));
	botonPujar->setScale(1.5);
	botonPujar->setTag(propiedad);

    auto labelPujar = Label::createWithTTF("Pujar","fonts/Kabel Regular.ttf",50);
    labelPujar->setPosition(Vec2(origin.x+visibleSize.width*0.6,origin.y+botonPujar->getContentSize().height));
    labelPujar->setColor(Color3B(255,255,255));
    popupSubasta->addChild(labelPujar,4,"labelPujar");


	auto botonRetirarse = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(GameScreen::retirarseJugador, this));
	botonRetirarse->setPosition(Vec2(origin.x+visibleSize.width,origin.y+botonRetirarse->getContentSize().height));
	botonRetirarse->setScale(1.5);
	botonRetirarse->setTag(propiedad);

    auto labelRetirarse = Label::createWithTTF("Retirarse","fonts/Kabel Regular.ttf",50);
    labelRetirarse->setPosition(Vec2(origin.x+visibleSize.width,origin.y+botonPujar->getContentSize().height));
    labelRetirarse->setColor(Color3B(255,255,255));
    popupSubasta->addChild(labelRetirarse,4,"labelSubastar");

    sliderSubasta = cocos2d::extension::ControlSlider::create("contenedor_slider.png", "progreso_slider.png", "circulo_slider.png");
    sliderSubasta->setMinimumValue(puja+10);
    sliderSubasta->setMaximumValue(puja+500);
    sliderSubasta->setValue(puja+10);
    sliderSubasta->addTargetWithActionForControlEvents(this, cccontrol_selector(GameScreen::valueSliderChange), cocos2d::extension::Control::EventType::VALUE_CHANGED);
    sliderSubasta->setPosition(Vec2(origin.x+visibleSize.width*0.8,origin.y+visibleSize.height/2));
    popupSubasta->addChild(sliderSubasta);

    char str_puja[50]={0};
    sprintf(str_puja,"%d $",(int)sliderSubasta->getValue());
    label_puja = Label::createWithTTF(str_puja,"fonts/Kabel Regular.ttf",50);
    label_puja->setPosition(Vec2(origin.x+visibleSize.width*0.8,origin.y+visibleSize.height*0.4));
    label_puja->setColor(Color3B(255,255,255));
    popupSubasta->addChild(label_puja,4,"labelSubastar");

    char str[30] = {0};
    sprintf(str,"Ultima Puja\n   %d $",puja);
    auto labelPrecio = Label::createWithTTF(str,"fonts/Kabel Regular.ttf",100);
    labelPrecio->setPosition(Vec2(origin.x+visibleSize.width*0.8,origin.y+visibleSize.height-botonPujar->getContentSize().height));
    labelPrecio->setColor(Color3B(255,255,255));
    popupSubasta->addChild(labelPrecio,4,"labelPuja");

	auto menuPopup = Menu::create(botonPujar, botonRetirarse, nullptr);
	menuPopup->setPosition(Vec2::ZERO);
	popupSubasta->addChild(menuPopup,2);
	this->addChild(popupSubasta,3,"popupSubasta");
}

void GameScreen::mostrarPopupPuja(int jugador){

	auto layerPuja = Layer::create();

	auto fondo = Sprite::create("popup_turno_puja.png");
	fondo->setPosition(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2);
	fondo->setScale(layerPuja->getContentSize().height/fondo->getContentSize().height);
	layerPuja->addChild(fondo);


	char str_jugador[10] = {0};
	sprintf(str_jugador,"Jugador%d",jugador);


	Sprite* ficha;
	if(jugador==1){
			ficha = Sprite::create("ficha_verde.png");
		}else{
			if(jugador==2){
				ficha = Sprite::create("ficha_roja.png");
			}else{
				if(jugador==3){
					ficha = Sprite::create("ficha_azul.png");
				}else{
					if(jugador==4){
						ficha = Sprite::create("ficha_naranja.png");
					}
				}
			}
		}
	ficha->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2));
	ficha->setScale(3);
	layerPuja->addChild(ficha);

    auto nombre = Label::createWithTTF(documentoJSON["Partida1"][str_jugador]["Nombre"].GetString(),"fonts/Kabel Regular.ttf",60);
    nombre->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+nombre->getContentSize().height*2));
    nombre->setColor(Color3B(255,255,255));
    layerPuja->addChild(nombre,2,"nombre");

    layerPuja->setScale(0.4);
    this->addChild(layerPuja,4,"popupPuja");
}

void GameScreen::cerrarPopupPuja(){
	this->removeChildByName("popupPuja");
}

void GameScreen::mostrarPopupGanadorPuja(){

	auto layerPuja = Layer::create();

	auto fondo = Sprite::create("popup.png");
	fondo->setPosition(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2);
	fondo->setScale(layerPuja->getContentSize().height/fondo->getContentSize().height);
	layerPuja->addChild(fondo);

	auto label_ganador = Label::createWithTTF("GANADOR","fonts/Kabel Bold.ttf",60);
	label_ganador->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height*0.9));
	label_ganador->setColor(Color3B(255,255,255));
	layerPuja->addChild(label_ganador,2,"nombre");


	char str_puja_ganadora[20]={0};
	sprintf(str_puja_ganadora,"%d $",puja_ganadora);
	auto label_puja_ganadora = Label::createWithTTF(str_puja_ganadora,"fonts/Kabel Regular.ttf",60);
	label_puja_ganadora->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height*0.8));
	label_puja_ganadora->setColor(Color3B(255,255,255));
	layerPuja->addChild(label_puja_ganadora,2,"nombre");

	char str_jugador[10] = {0};
	sprintf(str_jugador,"Jugador%d",jugador_ganador);


	Sprite* ficha;
	if(jugador_ganador==1){
			ficha = Sprite::create("ficha_verde.png");
		}else{
			if(jugador_ganador==2){
				ficha = Sprite::create("ficha_roja.png");
			}else{
				if(jugador_ganador==3){
					ficha = Sprite::create("ficha_azul.png");
				}else{
					if(jugador_ganador==4){
						ficha = Sprite::create("ficha_naranja.png");
					}
				}
			}
		}
	ficha->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2));
	ficha->setScale(3);
	layerPuja->addChild(ficha);

    auto nombre = Label::createWithTTF(documentoJSON["Partida1"][str_jugador]["Nombre"].GetString(),"fonts/Kabel Regular.ttf",60);
    nombre->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+nombre->getContentSize().height*2));
    nombre->setColor(Color3B(255,255,255));
    layerPuja->addChild(nombre,2,"nombre");

    layerPuja->setScale(0.5);
    this->addChild(layerPuja,4,"popupGanadorPuja");
}

void GameScreen::cerrarPopupGanadorPuja(){
	this->removeChildByName("popupGanadorPuja");
}

void GameScreen::valueSliderChange(Ref* sender, cocos2d::extension::Control::EventType evnt){
	char str_puja[50]={0};
	sprintf(str_puja,"%d $",(int)sliderSubasta->getValue());
	label_puja->setString(str_puja);
}

void GameScreen::pujarJugador(Ref* pSender){
	//Actualizamos la puja
	Node* nodo = (Node*)pSender;
	int propiedad = nodo->getTag();

	jugador_ganador=jugadores[0];
	puja_ganadora=(int)sliderSubasta->getValue();
	jugadores.push_back(jugadores[0]);
	jugadores.erase(jugadores.begin());
	cocos2d::log("Primer Elemento: %d",jugadores[0]);
	int ultimo_jugador=jugadores.back();
	cocos2d::log("Ultimo Elemento: %d",ultimo_jugador);
	this->removeChildByName("popupSubasta");
	this->crearMenuSubasta(propiedad,puja_ganadora,jugadores[0]);

	//lanzamos en turno siguiente


}

void GameScreen::retirarseJugador(Ref* pSender){

	Node* nodo = (Node*)pSender;
	int propiedad = nodo->getTag();

	jugadores.erase(jugadores.begin());
	cocos2d::log("Tamano: %d", jugadores.size());
	if(jugadores.size()==0){
		char str_jugador[10] = {0};
		sprintf(str_jugador, "Jugador%d",jugador_ganador);
		char str_propiedad[5] = {0};
		sprintf(str_propiedad, "%d",propiedad);

		rapidjson::Document::AllocatorType& allocator = documentoJSON.GetAllocator();
		documentoJSON["Partida1"][str_jugador]["Propiedades"].AddMember(tableroJSON[str_propiedad]["Nombre"].GetString(),0,allocator);
		//Descontamos el dinero de la cuenta del usuario y actualizamos el contador
		documentoJSON["Partida1"][str_jugador]["Dinero"].SetInt(documentoJSON["Partida1"][str_jugador]["Dinero"].GetInt()-puja_ganadora);
		this->actualizarPuntuacion();
		//Colocamos un simbolo para saber quien es el dueño
		this->colocarSimboloPropiedad(propiedad,jugador_ganador);
		this->removeChildByName("popupSubasta");
		this->mostrarPopupGanadorPuja();
		this->runAction( Sequence::create(
				                                  DelayTime::create(3.0f),
				                                  CallFunc::create(CC_CALLBACK_0(GameScreen::cerrarPopupGanadorPuja, this)),
				                                  nullptr));
	}else{
		this->removeChildByName("popupSubasta");
		this->crearMenuSubasta(propiedad,puja_ganadora,jugadores[0]);
	}
}

void GameScreen::anadirPropiedad(int propiedad){
	//Añadimos la propiedad a las propiedades del usuario
	char str_jugador[10] = {0};
	sprintf(str_jugador, "Jugador%d",jugador);
	char str_propiedad[5] = {0};
	sprintf(str_propiedad, "%d",propiedad);

	rapidjson::Document::AllocatorType& allocator = documentoJSON.GetAllocator();
	documentoJSON["Partida1"][str_jugador]["Propiedades"].AddMember(tableroJSON[str_propiedad]["Nombre"].GetString(),0,allocator);
	//Descontamos el dinero de la cuenta del usuario y actualizamos el contador
	documentoJSON["Partida1"][str_jugador]["Dinero"].SetInt(documentoJSON["Partida1"][str_jugador]["Dinero"].GetInt()-tableroJSON[str_propiedad]["Precio"].GetInt());
	this->actualizarPuntuacion();
	//Colocamos un simbolo para saber quien es el dueño
	this->colocarSimboloPropiedad(propiedad,jugador);

}

void GameScreen::actualizarPuntuacion(){

	int numJugadores = documentoJSON["Partida1"]["NumeroJugadores"].GetInt();
	//Actualizamos el jugador 1

	char str_dinero_jugador1[10] = {0};
	sprintf(str_dinero_jugador1,"%d $",documentoJSON["Partida1"]["Jugador1"]["Dinero"].GetInt());
	dineroJugador1->setString(str_dinero_jugador1);

	//Actualizamos el jugador 2
	char str_dinero_jugador2[10] = {0};
	sprintf(str_dinero_jugador2,"%d $",documentoJSON["Partida1"]["Jugador2"]["Dinero"].GetInt());
	dineroJugador2->setString(str_dinero_jugador2);

	if(numJugadores>=3){
		//Actualizamos el jugador 3
		char str_dinero_jugador3[10] = {0};
		sprintf(str_dinero_jugador3,"%d $",documentoJSON["Partida1"]["Jugador3"]["Dinero"].GetInt());
		dineroJugador3->setString(str_dinero_jugador3);
	}

	if(numJugadores==4){
		//Actualizamos el jugador 4
		char str_dinero_jugador4[10] = {0};
		sprintf(str_dinero_jugador4,"%d $",documentoJSON["Partida1"]["Jugador4"]["Dinero"].GetInt());
		dineroJugador4->setString(str_dinero_jugador4);
	}

}

void GameScreen::colocarSimboloPropiedad(int propiedad,int jugador){

	Sprite* simbolo;
	if (jugador==1){
		simbolo = Sprite::create("ficha_verde.png");
	}else{
		if (jugador==2){
				simbolo = Sprite::create("ficha_roja.png");
		}else{
			if (jugador==3){
					simbolo = Sprite::create("ficha_azul.png");
			}else{
				if (jugador==4){
						simbolo = Sprite::create("ficha_naranja.png");
				}
			}

		}
	}
	if (propiedad>0 && propiedad<10){
		simbolo->setPosition(Vec2(listax[propiedad],listay[propiedad]+70));
	}else{
		if (propiedad>10 && propiedad<20){
			simbolo->setPosition(Vec2(listax[propiedad]+80,listay[propiedad]));
			simbolo->setRotation(90);
		}else{
			if (propiedad>20 && propiedad<30){
				simbolo->setPosition(Vec2(listax[propiedad],listay[propiedad]-70));
				simbolo->setRotation(180);
			}else{
				if (propiedad>30 && propiedad<40){
					simbolo->setPosition(Vec2(listax[propiedad]-70,listay[propiedad]));
					simbolo->setRotation(270);
				}
			}
		}
	}
	simbolo->setScale(0.3);
	this->addChild(simbolo,1,propiedad);
	cocos2d::log("he colocado e simbolo");

}

void GameScreen::cargarSimbolosPropiedad(){
	rapidjson::Value::MemberIterator M;
	for(M = tableroJSON.MemberonBegin();M!=tableroJSON.MemberonEnd();M++){
		if (documentoJSON["Partida1"]["Jugador1"]["Propiedades"].HasMember(M->value["Nombre"].GetString())){
			cocos2d::log("%s",M->value["Nombre"].GetString());
			int propiedad = atoi(M->name.GetString());
			cocos2d::log("Numero propiedad: %d",propiedad);
			this->colocarSimboloPropiedad(propiedad,1);
		}
		if (documentoJSON["Partida1"]["Jugador2"]["Propiedades"].HasMember(M->value["Nombre"].GetString())){
					cocos2d::log("%s",M->value["Nombre"].GetString());
					int propiedad = atoi(M->name.GetString());
					cocos2d::log("Numero propiedad: %d",propiedad);
					this->colocarSimboloPropiedad(propiedad,2);
				}
		if (documentoJSON["Partida1"]["Jugador3"]["Propiedades"].HasMember(M->value["Nombre"].GetString())){
					cocos2d::log("%s",M->value["Nombre"].GetString());
					int propiedad = atoi(M->name.GetString());
					cocos2d::log("Numero propiedad: %d",propiedad);
					this->colocarSimboloPropiedad(propiedad,3);
				}
		if (documentoJSON["Partida1"]["Jugador4"]["Propiedades"].HasMember(M->value["Nombre"].GetString())){
					cocos2d::log("%s",M->value["Nombre"].GetString());
					int propiedad = atoi(M->name.GetString());
					cocos2d::log("Numero propiedad: %d",propiedad);
					this->colocarSimboloPropiedad(propiedad,4);
				}

	}
}

void GameScreen::pagarAlquiler(int propiedad){
	char str_nombre[100]={0};
	char str_num_propiedad[5]={0};
	char str_alquilado[10]={0};

	sprintf(str_num_propiedad,"%d",propiedad);
	sprintf(str_nombre,"%s",tableroJSON[str_num_propiedad]["Nombre"].GetString());
	sprintf(str_alquilado,"Jugador%d",jugador);

	int alquiler;

	if (documentoJSON["Partida1"]["Jugador1"]["Propiedades"].HasMember(str_nombre)){
		int num_casas=documentoJSON["Partida1"]["Jugador1"]["Propiedades"][str_nombre].GetInt();
		char str_num_casas[5]={0};
		sprintf(str_num_casas,"%d",num_casas);
		alquiler = tableroJSON[str_num_propiedad]["Alquiler"][str_num_casas].GetInt();


		cocos2d::log("Jugador que alquila: %d",documentoJSON["Partida1"][str_alquilado]["Dinero"].GetInt());
		cocos2d::log("Jugador que cobra: %d",documentoJSON["Partida1"]["Jugador1"]["Dinero"].GetInt());
		cocos2d::log("Alquiler: %d",alquiler);


		documentoJSON["Partida1"][str_alquilado]["Dinero"].SetInt(documentoJSON["Partida1"][str_alquilado]["Dinero"].GetInt()-alquiler);
		documentoJSON["Partida1"]["Jugador1"]["Dinero"].SetInt(documentoJSON["Partida1"]["Jugador1"]["Dinero"].GetInt()+alquiler);




	}else{
		if (documentoJSON["Partida1"]["Jugador2"]["Propiedades"].HasMember(str_nombre)){
			int num_casas=documentoJSON["Partida1"]["Jugador2"]["Propiedades"][str_nombre].GetInt();
			char str_num_casas[5]={0};
			sprintf(str_num_casas,"%d",num_casas);
			alquiler = tableroJSON[str_num_propiedad]["Alquiler"][str_num_casas].GetInt();


			cocos2d::log("Jugador que alquila: %d",documentoJSON["Partida1"][str_alquilado]["Dinero"].GetInt());
			cocos2d::log("Jugador que cobra: %d",documentoJSON["Partida1"]["Jugador2"]["Dinero"].GetInt());
			cocos2d::log("Alquiler: %d",alquiler);


			documentoJSON["Partida1"][str_alquilado]["Dinero"].SetInt(documentoJSON["Partida1"][str_alquilado]["Dinero"].GetInt()-alquiler);
			documentoJSON["Partida1"]["Jugador2"]["Dinero"].SetInt(documentoJSON["Partida1"]["Jugador2"]["Dinero"].GetInt()+alquiler);



		}else{
			if (documentoJSON["Partida1"]["Jugador3"]["Propiedades"].HasMember(str_nombre)){
				int num_casas=documentoJSON["Partida1"]["Jugador3"]["Propiedades"][str_nombre].GetInt();
				char str_num_casas[5]={0};
				sprintf(str_num_casas,"%d",num_casas);
				alquiler = tableroJSON[str_num_propiedad]["Alquiler"][str_num_casas].GetInt();
				documentoJSON["Partida1"][str_alquilado]["Dinero"].SetInt(documentoJSON["Partida1"][str_alquilado]["Dinero"].GetInt()-alquiler);
				documentoJSON["Partida1"]["Jugador3"]["Dinero"].SetInt(documentoJSON["Partida1"]["Jugador3"]["Dinero"].GetInt()+alquiler);
			}else{
				if (documentoJSON["Partida1"]["Jugador4"]["Propiedades"].HasMember(str_nombre)){
					int num_casas=documentoJSON["Partida1"]["Jugador4"]["Propiedades"][str_nombre].GetInt();
					char str_num_casas[5]={0};
					sprintf(str_num_casas,"%d",num_casas);
					alquiler = tableroJSON[str_num_propiedad]["Alquiler"][str_num_casas].GetInt();
					documentoJSON["Partida1"][str_alquilado]["Dinero"].SetInt(documentoJSON["Partida1"][str_alquilado]["Dinero"].GetInt()-alquiler);
					documentoJSON["Partida1"]["Jugador4"]["Dinero"].SetInt(documentoJSON["Partida1"]["Jugador4"]["Dinero"].GetInt()+alquiler);
				}
			}
		}
	}
	this->actualizarPuntuacion();
}

void GameScreen::tarjetaSuerte(){

	auto popup = Layer::create();
	auto fondo = Sprite::create("popup.png");
	fondo->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2));
	fondo->setScale(0.8,1);
	popup->addChild(fondo);
	popup->setScale(0.5);


	auto tarjeta_mostrar=Sprite::create("img/especiales/suerte.jpg");
	tarjeta_mostrar->setScale(2.5);
	tarjeta_mostrar->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height*0.7));
	popup->addChild(tarjeta_mostrar);


	auto botonPopup = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(GameScreen::ejecutarAccionSuerte, this));
	botonPopup->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y));
	botonPopup->setScale(1.5);


	auto labelBoton = Label::createWithTTF("Aceptar","fonts/Kabel Regular.ttf",60);
	labelBoton->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y));
	popup->addChild(labelBoton,4);

	char str_tarjeta[10]={0};
	int num=rand() % 6+1;
	sprintf(str_tarjeta, "Tarjeta%d",num);
	botonPopup->setTag(num);

	char str_mensaje[300]= {0};
	sprintf(str_mensaje,"%s",tarjetaEspecialesJSON["Suerte"][str_tarjeta]["Mensaje"].GetString());
	cocos2d::log("Mensaje: %s",tarjetaEspecialesJSON["Suerte"][str_tarjeta]["Mensaje"].GetString());
	auto labelMensaje = Label::createWithTTF(str_mensaje,"fonts/Kabel Regular.ttf",42);
	labelMensaje->setPosition(Vec2(origin.x+visibleSize.width*0.35,origin.y+visibleSize.height*0.7));
	labelMensaje->setAnchorPoint(Vec2(0,0.5f));
	popup->addChild(labelMensaje,4);

	auto menuPopup = Menu::create(botonPopup, nullptr);
	menuPopup->setPosition(Vec2::ZERO);
	popup->addChild(menuPopup,2);
	this->addChild(popup,3,"popup_tarjeta_especial");
}

void GameScreen::tarjetaComunidad(){
	auto popup = Layer::create();
	auto fondo = Sprite::create("popup.png");
	fondo->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height/2));
	fondo->setScale(0.8,1);
	popup->addChild(fondo);
	popup->setScale(0.5);


	auto tarjeta_mostrar=Sprite::create("img/especiales/comunidad.jpg");
	tarjeta_mostrar->setScale(2.5);
	tarjeta_mostrar->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y+visibleSize.height*0.7));
	popup->addChild(tarjeta_mostrar);

	char str_tarjeta[10]={0};
	int num=rand() % 6+1;
	sprintf(str_tarjeta, "Tarjeta%d",num);

	char str_mensaje[300]= {0};
	sprintf(str_mensaje,"%s",tarjetaEspecialesJSON["Suerte"][str_tarjeta]["Mensaje"].GetString());
	cocos2d::log("Mensaje: %s",tarjetaEspecialesJSON["Suerte"][str_tarjeta]["Mensaje"].GetString());
	auto labelMensaje = Label::createWithTTF(str_mensaje,"fonts/Kabel Regular.ttf",42);
	labelMensaje->setPosition(Vec2(origin.x+visibleSize.width*0.35,origin.y+visibleSize.height*0.7));
	labelMensaje->setAnchorPoint(Vec2(0,0.5f));
	popup->addChild(labelMensaje,4);


	auto botonPopup = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(GameScreen::ejecutarAccionComunidad, this));
	botonPopup->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y));
	botonPopup->setScale(1.5);
	botonPopup->setTag(num);

	auto labelTarjeta = Label::createWithTTF("Aceptar","fonts/Kabel Regular.ttf",60);
	labelTarjeta->setPosition(Vec2(origin.x+visibleSize.width/2,origin.y));
	popup->addChild(labelTarjeta,4);

	auto menuPopup = Menu::create(botonPopup, nullptr);
	menuPopup->setPosition(Vec2::ZERO);
	popup->addChild(menuPopup,2);
	this->addChild(popup,3,"popup_tarjeta_especial");
}

void GameScreen::ejecutarAccionSuerte(Ref* pSender){
	Node* nodo = (Node*) pSender;
	int numTarjeta=nodo->getTag();

	char str_tarjeta[10]={0};
	sprintf(str_tarjeta,"Tarjeta%d",numTarjeta);

	char str_jugador[10]={0};
	sprintf(str_jugador,"Jugador%d",jugador);

	std::string accion = tarjetaEspecialesJSON["Suerte"][str_tarjeta]["Accion"].GetString();

	if (accion == "Cobrar"){
		cocos2d::log("Accion: %s",accion.c_str());
		documentoJSON["Partida1"][str_jugador]["Dinero"].SetInt(documentoJSON["Partida1"][str_jugador]["Dinero"].GetInt()+tarjetaEspecialesJSON["Suerte"][str_tarjeta]["Cantidad"].GetInt());
		this->actualizarPuntuacion();
	}else{
		if (accion == "Pagar"){
			cocos2d::log("Accion: %s",accion.c_str());
			documentoJSON["Partida1"][str_jugador]["Dinero"].SetInt(documentoJSON["Partida1"][str_jugador]["Dinero"].GetInt()-tarjetaEspecialesJSON["Suerte"][str_tarjeta]["Cantidad"].GetInt());
			this->actualizarPuntuacion();
		}else{
			if(accion == "Mover"){
				int posicion = tarjetaEspecialesJSON["Suerte"][str_tarjeta]["Posicion"].GetInt();
				if(jugador==1){
					this->moverFicha(fichaJugador1,0,posicion);
					documentoJSON["Partida1"][str_jugador]["Position"].SetInt(posicion);
					this->comprobarCasilla(jugador);
				}else{
					if(jugador==2){
						this->moverFicha(fichaJugador2,0,posicion);
						documentoJSON["Partida1"][str_jugador]["Position"].SetInt(posicion);
						this->comprobarCasilla(jugador);
					}else{
						if(jugador==3){
							this->moverFicha(fichaJugador3,0,posicion);
							documentoJSON["Partida1"][str_jugador]["Position"].SetInt(posicion);
							this->comprobarCasilla(jugador);
						}else{
							if(jugador==4){
								this->moverFicha(fichaJugador4,0,posicion);
								documentoJSON["Partida1"][str_jugador]["Position"].SetInt(posicion);
								this->comprobarCasilla(jugador);
							}
						}
					}
				}
			}
		}
	}

	this->removeChildByName("popup_tarjeta_especial");
}

void GameScreen::ejecutarAccionComunidad(Ref* pSender){
	Node* nodo = (Node*) pSender;
	int numTarjeta=nodo->getTag();

	char str_tarjeta[10]={0};
	sprintf(str_tarjeta,"Tarjeta%d",numTarjeta);

	char str_jugador[10]={0};
	sprintf(str_jugador,"Jugador%d",jugador);

	std::string accion = tarjetaEspecialesJSON["Comunidad"][str_tarjeta]["Accion"].GetString();

	if (accion == "Cobrar"){
		cocos2d::log("Accion: %s",accion.c_str());
		documentoJSON["Partida1"][str_jugador]["Dinero"].SetInt(documentoJSON["Partida1"][str_jugador]["Dinero"].GetInt()+tarjetaEspecialesJSON["Comunidad"][str_tarjeta]["Cantidad"].GetInt());
		this->actualizarPuntuacion();
	}else{
		if (accion == "Pagar"){
			cocos2d::log("Accion: %s",accion.c_str());
			documentoJSON["Partida1"][str_jugador]["Dinero"].SetInt(documentoJSON["Partida1"][str_jugador]["Dinero"].GetInt()-tarjetaEspecialesJSON["Comunidad"][str_tarjeta]["Cantidad"].GetInt());
			this->actualizarPuntuacion();
		}else{
			if(accion == "Mover"){
				int posicion = tarjetaEspecialesJSON["Comunidad"][str_tarjeta]["Posicion"].GetInt();
				if(jugador==1){
					this->moverFicha(fichaJugador1,0,posicion);
					documentoJSON["Partida1"][str_jugador]["Position"].SetInt(posicion);
					this->comprobarCasilla(jugador);
				}else{
					if(jugador==2){
						this->moverFicha(fichaJugador2,0,posicion);
						documentoJSON["Partida1"][str_jugador]["Position"].SetInt(posicion);
						this->comprobarCasilla(jugador);
					}else{
						if(jugador==3){
							this->moverFicha(fichaJugador3,0,posicion);
							documentoJSON["Partida1"][str_jugador]["Position"].SetInt(posicion);
							this->comprobarCasilla(jugador);
						}else{
							if(jugador==4){
								this->moverFicha(fichaJugador4,0,posicion);
								documentoJSON["Partida1"][str_jugador]["Position"].SetInt(posicion);
								this->comprobarCasilla(jugador);
							}
						}
					}
				}
			}
		}
	}

	this->removeChildByName("popup_tarjeta_especial");
}



bool GameScreen::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    char mostrar[100] = {0};
    float x = touch->getLocationInView().x;
    float y = touch->getLocationInView().y;
    sprintf(mostrar,"You touched %f, %f", x, y);
    std::string cadena (mostrar);
    cocos2d::log("You touched %f, %f", x, y);

    return true;
}

/*bool GameScreen::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    ficha->setPosition(Vec2(listax[pos],listay[pos]));
    if(pos==39){
    	pos=0;
    }else{
    	pos++;

    }
    return true;
}*/
