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
Vector<MenuItem*> PropiedadesJugador(1);

bool BuildScreen::init()
{
	 if ( !Layer::init() )
	    {
	        return false;
	    }

	this->cargarPartida();

	cocos2d::log("ARchivo partida: %s",partidaActualJSON["Partida1"]["Jugador1"]["Nombre"].GetString());
	cocos2d::log("ARchivo color: %d",grupoColorJSON["Grupos"]["Rojo"].GetInt());


	int jugador = partidaActualJSON["Partida1"]["JugadorActual"].GetInt();

	this->comprobarGrupoColor(jugador);
    visibleSizeBuild = Director::getInstance()->getVisibleSize();
    originBuild = Director::getInstance()->getVisibleOrigin();

	auto fondo = Sprite::create("popup.png");
	fondo->setPosition(Vec2(originBuild.x+visibleSizeBuild.width/2,originBuild.y+visibleSizeBuild.height/2));
	fondo->setScale(visibleSizeBuild.height/fondo->getContentSize().height);
	this->addChild(fondo,0);

	auto label_titulo = Label::createWithTTF("Construir-Derruir","fonts/Kabel Bold.ttf",60);
	label_titulo->setPosition(Vec2(originBuild.x+visibleSizeBuild.width/2,originBuild.y+visibleSizeBuild.height-label_titulo->getContentSize().height));
	this->addChild(label_titulo,1);

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



}

void BuildScreen::volverAtras(Ref* psender){

	this->removeChildByName("menu_partidas");
	this->removeChildByName("label_guardar_partida");


}

void BuildScreen::cerrarPausa(Ref* pSender){
	this->getParent()->removeChildByName("Menu Pausa");
}

void BuildScreen::comprobarGrupoColor(int jugador){
	char str_jugador[10]={0};
	sprintf(str_jugador,"Jugador%d",jugador);

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
			this->crearTarjetasGrupoColor(N->name.GetString(),columna);
			columna++;


		}
	}
	auto menuConstruir = Menu::createWithArray(PropiedadesJugador);
	//menuConstruir->setPosition(Vec2::ZERO);
	this->addChild(menuConstruir,2);
	/*cocos2d::log("Color Violeta tiene: %d",comprobar["Violeta"].GetInt());
	cocos2d::log("Color Cian tiene: %d",comprobar["Cian"].GetInt());
	cocos2d::log("Color Rosa tiene: %d",comprobar["Rosa"].GetInt());
	cocos2d::log("Color Naranja tiene: %d",comprobar["Naranja"].GetInt());
	cocos2d::log("Color Roja tiene: %d",comprobar["Rojo"].GetInt());
	cocos2d::log("Color Amarillo tiene: %d",comprobar["Amarillo"].GetInt());
	cocos2d::log("Color Verde tiene: %d",comprobar["Verde"].GetInt());
	cocos2d::log("Color Azul tiene: %d",comprobar["Azul"].GetInt());*/

}

void BuildScreen::crearTarjetasGrupoColor(const char * color,int columna){
	rapidjson::Value::MemberIterator N;
	float fila = 0.5;
	for(N = grupoColorJSON["Propiedades"].MemberonBegin(); N != grupoColorJSON["Propiedades"].MemberonEnd(); N++){
		if (strcmp (N->value.GetString(), color) == 0){
			char str_propiedad[50]={0};
			sprintf(str_propiedad,"img/propiedades/%s.jpg",N->name.GetString());
			auto tarjeta1 = MenuItemImage::create(str_propiedad,str_propiedad,CC_CALLBACK_1(BuildScreen::construirEdificio,this));
			tarjeta1->setPosition(Vec2(originBuild.x+visibleSizeBuild.width*0.11*columna,originBuild.y+visibleSizeBuild.height*fila));
			tarjeta1->setScale(0.4);
			tarjeta1->setName(str_propiedad);
			PropiedadesJugador.pushBack(tarjeta1);
			fila=fila+0.1;

			//aqui lo que deberia hacer es coger cada una de las propiedades de color y hacer estaticamente el
			//menu que sino salen todas juntas unas encima de las otras.
			//Podría guardar los nombres en variables para luego usarlas segun el numero de propiedades que tenga el
			//grupo de color.
		}
	}
}

void BuildScreen::construirEdificio(Ref* pSender){

}
