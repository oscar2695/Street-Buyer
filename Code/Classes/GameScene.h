/*
 * Proyecto: Street Buyer. Desarrollo de un juego de mesa para dispositivos Android.
 * Project: Street Buyer. Develop of a table game for Android device.
 * Autor: Óscar Rodríguez Domínguez
 */

#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "GUI/CCControlExtension/CCControlExtensions.h"
using namespace cocos2d;

class GameScreen : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    //Metodo que al tocar una de las tarjetas se muestra en grande
    void mostrarTarjeta(cocos2d::Ref* pSender);
    void cerrarMostrarTarjeta(cocos2d::Ref* pSender);
    void crearMenuTarjetas();
    void crearDados(Ref* pSender);
    void crearTituloPropiedad(std::string nombre,float columna, float fila);
    void cargarInformacionJSON();
    void crearBannersPuntuacion();
    void mostrarPopupDados(Ref* pSender);
    void cerrarPopupDados();
    void moverFicha(Sprite* ficha,int pos,int tirada);
    void mostrarBotonPausa();
    void mostrarMenuPausa(Ref* psender);
    void mostrarBotonConstruir();
    void mostrarMenuConstruir(Ref* psender);
    void mostrarTurno(int jugador);
    void crearFichas();
    void escribirArchivoJSON();
    void lanzadorFuncionesJuego(int jugador,int pos, int tirada);
    void comprobarCasilla(int jugador);
    void menuComprarSubastar(int propiedad);
    void cerrarComprarSubastar(Ref* pSender);
    void crearVenta(Ref* pSender);
    void crearMenuVenta(int propiedad);
    void cerrarMenuVenta();
    void crearSubasta(Ref* pSender);
    void crearMenuSubasta(int propiedad, int puja,int jugador);
    void crearMenuPuja(int propiedad, int puja,int jugador);
    void mostrarPopupPuja(int jugador);
    void cerrarPopupPuja();
    void mostrarPopupGanadorPuja();
    void cerrarPopupGanadorPuja();
    void valueSliderChange(Ref* sender, cocos2d::extension::Control::EventType evnt);
    void pujarJugador(Ref* pSender);
    void retirarseJugador(Ref* pSender);
    void anadirPropiedad(int propiedad);
    void actualizarPuntuacion();
    void colocarSimboloPropiedad(int propiedad,int jugador);
    void cargarSimbolosPropiedad();
    void pagarAlquiler(int propiedad);
    void tarjetaSuerte();
    void tarjetaComunidad();
    void ejecutarAccionSuerte(Ref* pSender);
    void ejecutarAccionComunidad(Ref* pSender);


    //Prueba para la ficha  muestra donde estamos tocando
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

    // implement the "static create()" method manually
    CREATE_FUNC(GameScreen);
};

#endif // __GAME_SCENE_H__
