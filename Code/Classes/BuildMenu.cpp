/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */
#include "BuildMenu.h"
#include "GameScene.h"
#include "json/document.h"
#include "json/filestream.h"
#include "json/stringbuffer.h"
#include "json/prettywriter.h"
#include <cstdio>
using namespace rapidjson;

USING_NS_CC;
Size visibleSizeBuild;
Vec2 originBuild;
Document grupoColorJSON;
Document partidaActualJSON;
Document tableroBuildJSON;
int jugadorBuild;
Vector<MenuItem*> PropiedadesJugador(1);
std::vector<std::string> GrupodeColor;


//Cosas referentes al slider para comprar propiedades
cocos2d::extension::ControlSlider* sliderConstruir;
Label* label_construir;
Label* label_precio;
int precio;
int numPropiedades;

bool BuildScreen::init()
{
	 if ( !Layer::init() )
	    {
	        return false;
	    }

	this->cargarPartida();

	jugadorBuild = partidaActualJSON["Partida1"]["JugadorActual"].GetInt();
	cocos2d::log("Este es el jugador para el que estamos mirnado si hay grupos de color: %d",jugadorBuild);

    visibleSizeBuild = Director::getInstance()->getVisibleSize();
    originBuild = Director::getInstance()->getVisibleOrigin();

	auto fondo = Sprite::create("popup.png");
	fondo->setPosition(Vec2(originBuild.x+visibleSizeBuild.width/2,originBuild.y+visibleSizeBuild.height/2));
	fondo->setScale(visibleSizeBuild.height/fondo->getContentSize().height);
	this->addChild(fondo,0);

	auto label_titulo = Label::createWithTTF("Construir-Derruir","fonts/Kabel Bold.ttf",60);
	label_titulo->setPosition(Vec2(originBuild.x+visibleSizeBuild.width/2,originBuild.y+visibleSizeBuild.height-label_titulo->getContentSize().height));
	this->addChild(label_titulo,1);


    auto label_atras = Label::createWithTTF("volver","fonts/Kabel Regular.ttf",50);
    label_atras->setPosition(Vec2(originBuild.x+visibleSizeBuild.width/2,originBuild.y+visibleSizeBuild.height*0.2));
    label_atras->setColor(Color3B(255,255,255));
    this->addChild(label_atras,4,"label");

	auto boton_atras = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(BuildScreen::cerrarConstruir,this));
	boton_atras->setPosition(Vec2(originBuild.x+visibleSizeBuild.width/2,originBuild.y+visibleSizeBuild.height*0.2));
	boton_atras->setName("boton_atras");

	auto menuConstruir = Menu::create(boton_atras, nullptr);
	menuConstruir->setPosition(Vec2::ZERO);
	this->addChild(menuConstruir,2);


	this->comprobarGrupoColor(jugadorBuild);
    return true;
}

void BuildScreen::cargarPartida(){

	 //Leemos el archivo de configuracion

	    FILE* file = fopen("/data/data/org.ordominguez.monopoly/files/partidaActual.json", "r");
	    if(file==NULL){
	    	auto valorJson = Label::createWithTTF("no existe el archivo","fonts/Kabel Regular.ttf",32);
	    	valorJson->setColor(Color3B(0,0,0));
	    	valorJson->setPosition(originBuild.x+visibleSizeBuild.width/2,originBuild.y+visibleSizeBuild.height/2);
	    	this->addChild(valorJson,2,"valor_json");
	    }
	    FileStream fs(file);
	    partidaActualJSON.ParseStream<0>(fs);

	 //Cargamos informacion de las tarjetas especiales

	    std::string fullPathColor = FileUtils::getInstance()->fullPathForFilename( "json/grupoColor.json" );

	    std::string docEspeciales = FileUtils::getInstance()->getStringFromFile( fullPathColor );

	    grupoColorJSON.Parse<0>(docEspeciales.c_str());

	//Cargamos informacion de las tarjetas especiales

		std::string fullPathTablero = FileUtils::getInstance()->fullPathForFilename( "json/tablero.json" );

		std::string docTablero = FileUtils::getInstance()->getStringFromFile( fullPathTablero );

		tableroBuildJSON.Parse<0>(docTablero.c_str());


}

void BuildScreen::comprobarGrupoColor(int jugadorBuild){
	char str_jugador[10]={0};
	sprintf(str_jugador,"Jugador%d",jugadorBuild);

	Document comprobar;
	comprobar.SetObject();
	rapidjson::Value::MemberIterator M;
	rapidjson::Document::AllocatorType& allocator = comprobar.GetAllocator();

	for(M = partidaActualJSON["Partida1"][str_jugador]["Propiedades"].MemberonBegin();M != partidaActualJSON["Partida1"][str_jugador]["Propiedades"].MemberonEnd();M++){
		cocos2d::log("Color: %s",grupoColorJSON["Propiedades"][M->name.GetString()].GetString());

		if(comprobar.HasMember(grupoColorJSON["Propiedades"][M->name.GetString()].GetString())){
			comprobar[grupoColorJSON["Propiedades"][M->name.GetString()].GetString()].SetInt(comprobar[grupoColorJSON["Propiedades"][M->name.GetString()].GetString()].GetInt()+1);
			cocos2d::log("Si que esta dentro del grupo");
		}else{
			comprobar.AddMember(grupoColorJSON["Propiedades"][M->name.GetString()].GetString(),1,allocator);
			cocos2d::log("No esta dentro del grupo");
		}
	}
	int columna = 1;
	rapidjson::Value::MemberIterator N;
	for(N = grupoColorJSON["Grupos"].MemberonBegin(); N != grupoColorJSON["Grupos"].MemberonEnd(); N++){
		if(N->value.GetInt()==comprobar[N->name.GetString()].GetInt()){
			cocos2d::log("Tiene un el grupo de color completo: %s",N->name.GetString());
			this->crearTarjetas(N->name.GetString(),columna);
			columna++;
		}
	}
}

void BuildScreen::crearTarjetas(const char * color,int columna){
	rapidjson::Value::MemberIterator N;
	float fila = 0.5;
	for(N = grupoColorJSON["Propiedades"].MemberonBegin(); N != grupoColorJSON["Propiedades"].MemberonEnd(); N++){
		if (strcmp (N->value.GetString(), color) == 0){
			char str_propiedad[50]={0};
			sprintf(str_propiedad,"%s",N->name.GetString());
			std::string propiedad (str_propiedad);
			GrupodeColor.push_back(propiedad);

		}
	}
	this->crearTarjetasGrupoColor(columna);
	GrupodeColor.clear();
}

void BuildScreen::crearTarjetasGrupoColor(int columna){
	MenuItemImage* tarjeta1;
	MenuItemImage* tarjeta2;
	MenuItemImage* tarjeta3;

	if(GrupodeColor.size()>=2){
		char str_propiedad1[100]={0};
		sprintf(str_propiedad1,"img/propiedades/%s.jpg",GrupodeColor[0].c_str());
		cocos2d::log("Tarjeta1: %s",str_propiedad1);
		tarjeta1 = MenuItemImage::create(str_propiedad1,str_propiedad1,CC_CALLBACK_1(BuildScreen::mostrarMenuConstruir, this));
		tarjeta1->setScale(0.4);
		tarjeta1->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*(0.11*columna),originBuild.y+visibleSizeBuild.height*0.65));
		tarjeta1->setName(GrupodeColor[0]);
		cocos2d::log("Tarjeta1: %s",GrupodeColor[0].c_str());
		PropiedadesJugador.pushBack(tarjeta1);

		char str_propiedad2[100]={0};
		sprintf(str_propiedad2,"img/propiedades/%s.jpg",GrupodeColor[1].c_str());
		tarjeta2 = MenuItemImage::create(str_propiedad2,str_propiedad2,CC_CALLBACK_1(BuildScreen::mostrarMenuConstruir, this));
		tarjeta2->setScale(0.4);
		tarjeta2->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*(0.11*columna),originBuild.y+visibleSizeBuild.height*0.55));
		tarjeta2->setName(GrupodeColor[1]);
		PropiedadesJugador.pushBack(tarjeta2);
	}
	if(GrupodeColor.size()==3){
		char str_propiedad3[100]={0};
		sprintf(str_propiedad3,"img/propiedades/%s.jpg",GrupodeColor[2].c_str());
		tarjeta3 = MenuItemImage::create(str_propiedad3,str_propiedad3,CC_CALLBACK_1(BuildScreen::mostrarMenuConstruir, this));
		tarjeta3->setScale(0.4);
		tarjeta3->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*(0.11*columna),originBuild.y+visibleSizeBuild.height*0.45));
		tarjeta3->setName(GrupodeColor[2]);
		PropiedadesJugador.pushBack(tarjeta3);
	}

	auto menuConstruir = Menu::create(tarjeta1,tarjeta2,tarjeta3, nullptr);
	menuConstruir->setPosition(Vec2::ZERO);
	this->addChild(menuConstruir,2,"menuTarjetas");


}

void BuildScreen::mostrarMenuConstruir(Ref* pSender){

	Node* nodo=(Node*) pSender;
	std::string propiedad = nodo->getName();

	char str_propiedad[100] = {0};
	sprintf(str_propiedad,"img/propiedades/%s.jpg",propiedad.c_str());
	const char * lugar = grupoColorJSON["Numeros"][propiedad.c_str()].GetString();
	precio= tableroBuildJSON[lugar]["Precio-Edificio"].GetInt();


	char str_jugador[10]={0};
	sprintf(str_jugador,"Jugador%d",jugadorBuild);
	numPropiedades=partidaActualJSON["Partida1"][str_jugador]["Propiedades"][propiedad.c_str()].GetInt();
	cocos2d::log("Numero Propiedades: %d",numPropiedades);

	this->removeAllChildren();

	auto fondo = Sprite::create("popup.png");
	fondo->setPosition(Vec2(originBuild.x+visibleSizeBuild.width/2,originBuild.y+visibleSizeBuild.height/2));
	fondo->setScale(visibleSizeBuild.height/fondo->getContentSize().height);
	this->addChild(fondo,0);

	auto tarjeta_mostrar=Sprite::create(str_propiedad);
	tarjeta_mostrar->setScale(1.2);
	tarjeta_mostrar->setPosition(Vec2(originBuild.x+tarjeta_mostrar->getContentSize().width*0.7,originBuild.y+visibleSizeBuild.height/2));
	this->addChild(tarjeta_mostrar);

    sliderConstruir = cocos2d::extension::ControlSlider::create("contenedor_slider.png", "progreso_slider.png", "circulo_slider.png");
    sliderConstruir->setMinimumValue(0);
    sliderConstruir->setMaximumValue(5);
    sliderConstruir->setValue(numPropiedades);
    sliderConstruir->addTargetWithActionForControlEvents(this, cccontrol_selector(BuildScreen::valueSliderChange), cocos2d::extension::Control::EventType::VALUE_CHANGED);
    sliderConstruir->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*0.7,originBuild.y+visibleSizeBuild.height*0.6));
    this->addChild(sliderConstruir);


    char str_puja[50]={0};
    sprintf(str_puja,"%d casas",numPropiedades);
    label_construir = Label::createWithTTF(str_puja,"fonts/Kabel Regular.ttf",50);
    label_construir->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*0.7,originBuild.y+visibleSizeBuild.height*0.7));
    label_construir->setColor(Color3B(255,255,255));
    this->addChild(label_construir,4,"label_construir");

    char str_precio[50]={0};
    sprintf(str_precio,"0 $");
    label_precio = Label::createWithTTF(str_precio,"fonts/Kabel Regular.ttf",50);
    label_precio->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*0.7,originBuild.y+visibleSizeBuild.height*0.5));
    label_precio->setColor(Color3B(255,255,255));
    this->addChild(label_precio,4,"label_construir");



	auto label_titulo = Label::createWithTTF("Construir-Derruir","fonts/Kabel Bold.ttf",60);
	label_titulo->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*0.7,originBuild.y+visibleSizeBuild.height-label_titulo->getContentSize().height));
	this->addChild(label_titulo,1);

	auto label_atras = Label::createWithTTF("volver","fonts/Kabel Regular.ttf",50);
	label_atras->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*0.6,originBuild.y+visibleSizeBuild.height*0.2));
	label_atras->setColor(Color3B(255,255,255));
	this->addChild(label_atras,4,"label");

	auto boton_atras = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(BuildScreen::cerrarConstruir,this));
	boton_atras->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*0.6,originBuild.y+visibleSizeBuild.height*0.2));
	boton_atras->setName("boton_atras");


	auto label_construir = Label::createWithTTF("construir","fonts/Kabel Regular.ttf",50);
	label_construir->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*0.8,originBuild.y+visibleSizeBuild.height*0.2));
	label_construir->setColor(Color3B(255,255,255));
	this->addChild(label_construir,4,"label");

	auto boton_construir = MenuItemImage::create("banner-puntuacion.png","banner-puntuacion.png",CC_CALLBACK_1(BuildScreen::construirEdificio,this));
	boton_construir->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*0.8,originBuild.y+visibleSizeBuild.height*0.2));
	boton_construir->setName(propiedad.c_str());

	auto menuComprar = Menu::create(boton_atras,boton_construir, nullptr);
	menuComprar->setPosition(Vec2::ZERO);
	this->addChild(menuComprar,2);

}

void BuildScreen::valueSliderChange(Ref* sender, cocos2d::extension::Control::EventType evnt){
	char str_puja[50]={0};
	char str_precio[50]={0};
	if(sliderConstruir->getValue()==1){
		sprintf(str_puja,"1 casa");
	}else{
		if(sliderConstruir->getValue()==5){
			sprintf(str_puja,"1 hotel");
		}else{
			sprintf(str_puja,"%d casas",(int) sliderConstruir->getValue());
		}
	}
	int cobrar = (precio * (int) sliderConstruir->getValue()) - (precio*numPropiedades);
	char str_cobrar[50]={0};
	sprintf(str_cobrar,"%d $",cobrar);
	label_precio->setString(str_cobrar);
	label_construir->setString(str_puja);
}

void BuildScreen::construirEdificio(Ref* pSender){
	Node* nodo = (Node * )pSender;
	std::string propiedad = nodo->getName();

	char str_jugador[10]={0};
	sprintf(str_jugador,"Jugador%d",jugadorBuild);

	partidaActualJSON["Partida1"][str_jugador]["Propiedades"][propiedad.c_str()].SetInt((int)sliderConstruir->getValue());
	partidaActualJSON["Partida1"][str_jugador]["Dinero"].SetInt(partidaActualJSON["Partida1"][str_jugador]["Dinero"].GetInt()-precio);

}

void BuildScreen::cerrarConstruir(Ref* pSender){

	this->escribirArchivoJSON();

	GameScreen* parent = (GameScreen *) this->getParent();
	parent->cargarInformacionJSON();
	parent->actualizarPuntuacion();
	parent->actualizarConstrucciones();
	this->getParent()->removeChildByName("Menu Construir");
}

void BuildScreen::escribirArchivoJSON(){
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
