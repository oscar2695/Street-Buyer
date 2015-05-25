/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */
#ifndef __BUILDMENU_H__
#define __BUILDMENU_H__

#include "cocos2d.h"
#include "GUI/CCControlExtension/CCControlExtensions.h"
USING_NS_CC;
class BuildScreen : public cocos2d::Layer{
public:
    virtual bool init();
    CREATE_FUNC(BuildScreen);
    void cargarPartida();
    void comprobarGrupoColor(int jugador);
    void crearTarjetas(const char * color,int columna);
    void crearTarjetasGrupoColor(int columna);
    void mostrarMenuConstruir(Ref* pSender);
    void valueSliderChange(Ref* sender, cocos2d::extension::Control::EventType evnt);
    void construirEdificio(Ref* pSender);
    void cerrarConstruir(Ref* pSender);
    void escribirArchivoJSON();

};

#endif // __BUILDMENU_H__
