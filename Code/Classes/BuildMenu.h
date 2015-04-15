/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */
#ifndef __BUILDMENU_H__
#define __BUILDMENU_H__

#include "cocos2d.h"
USING_NS_CC;
class BuildScreen : public cocos2d::Layer{
public:
    virtual bool init();
    CREATE_FUNC(BuildScreen);
    void cargarPartida();
    void volverAtras(Ref* pSender);
    void cerrarPausa(Ref* pSender);
    void comprobarGrupoColor(int jugador);
    void crearTarjetasGrupoColor(const char * color,int columna);
    void construirEdificio(Ref* pSender);

};

#endif // __BUILDMENU_H__
