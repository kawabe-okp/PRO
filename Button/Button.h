/*
 * 
 * mbed Press the Button
 * Copyright (c) 2015 ito shogo
 * 
 */ 

#ifndef Button_h
#define Button_h

#include "mbed.h"

typedef enum ButtonPress{
    BTN_NONE            //押下無し
    ,BTN_UP             //ボタン1個押し
    ,BTN_DOWN
    ,BTN_KEEP_UP
    ,BTN_KEEP_DOWN
    ,BTN_ESC_UP          //ESCとUpキー同時押し(1回)
    ,BTN_ESC_DOWN        //ESCとDownキー同時押し(1回)
    ,BTN_KEEP_ESC_UP     //ESCとUpキー同時押し
    ,BTN_KEEP_ESC_DOWN   //ESCとDownキー同時押し
    
    ,BTN_ENT
    ,BTN_ESC

    ,BTN_INI = 100      //Btnが初期化の状態のとき
}ButtonPress;
    
class Button
{
public:
    //コンストラクタ
    Button(PinName d0, PinName d1, PinName d2, PinName d3);
    
    void ini();             //ボタン値を初期化
    void force_ENTER();     //強制ENTER入力
    void force_ESC();       //強制ESC入力
    void force_DOWN();      //強制DOWN(上ボタン)入力
    void force_UP();        //強制DOWN(上ボタン)入力
    void end_force();       //強制ONをやめる
    void set_ENTER();       //疑似ENTER入力
    void set_ESC();         //疑似ESC入力
    void set_DOWN();        //疑似DOWN(上ボタン)入力
    void set_UP();          //疑似DOWN(上ボタン)入力
    void new_btn();          //ボタン状態を更新
    void new_keep_btn();    //ボタン状態を更新しつつ、押されたボタンを記憶する
    void old_btn();          //ボタン状態を保存
    bool esc();             //escキーが押されていたらtrueを返す
    bool ent();             //enterキーが押されていたらtrueを返す
    bool push_sound();      //プッシュ音がなって欲しいならtrueを返す
    ButtonPress get_btn();   //押下ボタンを定数値にて取得(チャタリング対策済)
    ButtonPress get_one_push();   //押下ボタンを定数値にて取得(チャタリング対策済)
    ButtonPress get_keep_btn(); //押されていたボタン状態を取得
    
    //チャタリング対策無し
    bool down_press();   //Downボタンが押下されたらtrueを返す
    bool up_press();   //Upボタンが押下されたらtrueを返す
    bool ent_press();   //enterボタンが押下されたらtrueを返す
    bool esc_press();   //escボタンが押下されたらtrueを返す
    bool any_press();    //どれかボタンが押下されていたらtrueを返す
    
    //ボタン操作によって値をpuls or Minusする
    void puls_or_minus(int* num);
    void puls_or_minus(float* num);
    void puls_or_minus(int* num, int nomalUp, int highUp);
    
    //void nextOrBack(int* num); //メニュー変更など前か後ろかに表示変更するときに使用
    void next_or_back(int* num,int min, int max);
    
    //特定の操作をロック、解除、状況取得を行う関数群
    bool get_lock_flg();
    void operation_lock();
    void operation_unlock();
private:
    DigitalIn _esc, _ent, _up, _down;
    ButtonPress _Pres;      //押下されたボタン値
    ButtonPress force;
    ButtonPress once;
    
    //スイッチ押下状態取得用
    typedef struct ButtonFlg{
        int esc_old;        //esc btn
        int esc_new;
        int ent_old;        //Enter btn
        int ent_new;
        int up_old;         //up btn
        int up_new;
        int down_old;       //down btn
        int down_new;
    }Btn;
    
    Btn btn;                //新旧のボタン押下状態取得用。チャタリング対策に使用する。
    int _cntDown, _cntUp;   //ボタン押しっぱなし判断ようのカウント変数
    int KEEP;
    bool _operationLock; //操作を受け付けないように監視するフラグ
    ButtonPress _keepState;
    ButtonPress check_button();  //ボタン状態から押下されたボタン値を確認
    void ini_min_max(int* num,int min, int max);//規定する最小値、最大値をオーバーしたら値を初期化
};

#endif
