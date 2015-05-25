/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */
#ifndef __NEGOTIATEMENU_H__
#define __NEGOTIATEMENU_H__

#include "cocos2d.h"
#include "json/document.h"
#include <cstdio>
#include "GUI/CCControlExtension/CCControlExtensions.h"
using namespace rapidjson;
using namespace cocos2d;
class NegotiateScreen : public cocos2d::Layer{
private:
	Document partidaActualJSON;
	Size visibleSizeNegotiate;
	Vec2 originNegotiate;

	int jugadorNegotiate;

	//Cambios de propiedades
	std::vector<std::string> PropiedadesParaJugador;
	std::vector<std::string> PropiedadesParaOponente;

	Layer * JugadoresNegociar;
	Layer * Cambio;

	char jugador[10];
	char oponente[10];

public:
    virtual bool init();
    CREATE_FUNC(NegotiateScreen);
    void mostrarOponentes(int jugador);
    void mostrarMenuNegociar(Ref* pSender);
    void mostrarPropiedadesJugador(char str[]);
    void mostrarPropiedadesOponente(char str[]);
    void cambiarPropiedadJugador(Ref* pSender);
    void cambiarPropiedadOponente(Ref* pSender);
    void cambiarPropiedades(Ref* pSender);
    void cargarArchivoJSON();
    void escribirArchivoJSON();
    void cerrarNegociar(Ref* pSender);


};

#endif // __NEGOTIATEMENU_H__
