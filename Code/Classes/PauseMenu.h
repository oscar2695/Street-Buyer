/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */
#ifndef __PAUSEMENU_H__
#define __PAUSEMENU_H__

#include "cocos2d.h"
USING_NS_CC;
class PauseScreen : public cocos2d::Layer{
public:
    virtual bool init();
    CREATE_FUNC(PauseScreen);
    void menuGuardar(Ref* psender);
    void menuSalir(Ref* psender);
    void guardarPartida(Ref* pSender);
    void volverAtras(Ref* pSender);
    void cerrarPausa(Ref* pSender);

};

#endif // __PAUSEMENU_H__
