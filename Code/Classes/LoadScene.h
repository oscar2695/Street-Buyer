/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */
#ifndef __LOAD_SCENE_H__
#define __LOAD_SCENE_H__

#include "cocos2d.h"
#include "GUI/CCControlExtension/CCControlExtensions.h"
#include "ui/UIEditBox/UIEditBox.h"

#include "json/document.h"
#include "json/filestream.h"
#include "json/prettywriter.h"
#include "json/stringbuffer.h"
#include <cstdio>


class LoadScreen : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
	rapidjson::Document documentoJSONcargar;

	static cocos2d::Scene* createScene();

    void crearNuevaPartida(cocos2d::Ref* pSender);
    void volverMainMenu(cocos2d::Ref* pSender);
    void crearMenuCargar();
    void cargarPartida(cocos2d::Ref* pSender);
    void borrarPartida(cocos2d::Ref* pSender);
    void ocultarBotones();
    void cargarArchivoJSON();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    // implement the "static create()" method manually
    CREATE_FUNC(LoadScreen);
};

#endif // __LOAD_SCENE_H__
