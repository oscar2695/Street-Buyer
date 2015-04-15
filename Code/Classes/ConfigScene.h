/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */
#ifndef __CONFIG_SCENE_H__
#define __CONFIG_SCENE_H__

#include "cocos2d.h"
#include "GUI/CCControlExtension/CCControlExtensions.h"
#include "ui/UIEditBox/UIEditBox.h"

#include "json/document.h"
#include "json/filestream.h"
#include "json/prettywriter.h"
#include "json/stringbuffer.h"
#include <cstdio>


class ConfigScreen : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    void crearNuevaPartida(cocos2d::Ref* pSender);
    void volverMainMenu(cocos2d::Ref* pSender);
    void valueSliderChange(Ref* sender, cocos2d::extension::Control::EventType evnt);
    void crearEditBox();
    void ocultarEditBox(int num);
    void crearPartidaJson(int num);



    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    // implement the "static create()" method manually
    CREATE_FUNC(ConfigScreen);
};

#endif // __CONFIG_SCENE_H__
