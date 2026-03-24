/*
 * 
 * mbed the LED
 * Copyright (c) 2015 ito shogo
 * 
 */ 

#ifndef LED_H
#define LED_H

#include "mbed.h"

class Led
{
public:
    //コンストラクタ
//***********基板の改造具合によって変更が必要****************************************************************
    //***********デフォルト************************************************
        //Led(PinName d0, PinName d1, PinName d2, PinName d3);
    //***********未改造の基板用***************************************************************************
        //Led(PinName d0, PinName d1, PinName d2, PinName d3, PinName d4, PinName d5, PinName d6);
    //***********改造済の基板用***************************************************************************
        Led(PinName d0, PinName d1, PinName d2, PinName d3, PinName d4, PinName d5);
//******************************************************************************************************
    
    void set(int s);            //操作するLED数を指定
    void ledAllOff();           //青,赤LEDを全て消灯
    void REGIS(int x);          //シフトレジスタ操作関数(青LED点灯)
    void REGIS_RED(int x);          //シフトレジスタ操作関数(赤LED点灯)
    void REGIS_BATU(int x);          //シフトレジスタ操作関数([×]点灯)
    void REGIS3(int x);          //シフトレジスタ操作関数(青LED点灯)
    void regis2();          //シフトレジスタ操作関数(ドットマトリクス)
    //void regis2(int para_limit);          //シフトレジスタ操作関数(ドットマトリクス)
    //void regis2(int para_limit,int para_array);         //シフトレジスタ操作関数(ドットマトリクス)
    void RedOn();               //赤LED点灯
    void RedOff();              //赤LED消灯
    void Flash(int k);          //LED点滅
    void Flash_RED(int k);       //LED赤点滅※ドットマトリクス専用
    void Flash_BATU(int k);       //LED[×]点滅※ドットマトリクス専用
    void Flash2(int k);          //LED点滅 青のみ
    void Flash_r(int k);          //LED点滅※ドットマトリクス専用の機能
    void ANIME();               //LEDを順番に点灯させるアニメーション
private:
//***********基板の改造具合によって変更が必要****************************************************************
    //***********デフォルト************************************************
        //DigitalOut _SI ,_SCK, _RCK, _red;
    //***********未改造の基板用***************************************************************************
        //DigitalOut _SI ,_SCK, _RCK, _red, _SI2 ,_SCK2, _RCK2;
    //***********改造済の基板用***************************************************************************
        //DigitalOut _SI ,_SCK, _RCK, _SI2 ,_SCK2, _RCK2;
        DigitalOut _SI ,_SCK, _RCK, _RCK2,_SI2 , _SCK2;
//******************************************************************************************************
    float _timeAnime;
    int _headNum;
};

#endif