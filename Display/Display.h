/*
 * 
 * LCD screen display
 * Copyright (c) 2015 ito shogo
 * 
 */ 

#ifndef DISPLAY_H
#define DISPLAY_H

#include "TextLCD.h"
#include "Common_t.h"
enum type{
    TOP      //メッセージを1行目に表示
    ,LOW      //メッセージを2行目に表示
    ,CLS      //画面クリアを行う場合に使用
};

/** 各種表示メッセージ */
enum msgNo {
    DIS_ERR         //エラー
    ,DIS_TABLE      //テーブルコンビ
    ,DIS_WAIT       //お待ちください
    ,DIS_ZERO       //0g設定中
    ,DIS_CHE_HEAD   //チェックヘッド
    ,DIS_ZERO_ERR   //チェックヘッド
    ,DIS_MENU       //メニュータイトル
};

enum msgNo0 {
    DIS_CNT_SET =10    //カウントセッテイ
    ,DIS_START_WHT      //計量スタート
    ,DIS_PASS_INPUT     //パスワード入力
    ,DIS_PASS_ERR       //パスワードエラー
};

enum msgNo1 {
    DIS_MODE_SELECT =20    //カウントセッテイ
    ,DIS_NORMAL             //ノーマル
    ,DIS_SOUND              //サウンド
    ,DIS_G                  //g
    ,DIS_MSEC               //ﾋﾞｮｳ
    ,DIS_IJO                //以上
    ,DIS_IKA                //以下
    ,DIS_KANTAN_NO          //カンタントウロクNO
};

enum msgNo2 {
    DIS_TRAINING =30    //カウントセッテイ
};

class Display : public TextLCD {
public:
    Display(PinName rs, PinName e, PinName d4, PinName d5, PinName d6, PinName d7, LCDType type = LCD16x2);
    
    // add to ito
    void ArrowMosion();         //アロー( > )アニメーション
    void blank(int row);        //指定行を空白で埋める
    void disSum(float sum);  //桁に合わせてトータル重量を表示
    void disSums(float sum);  //桁に合わせてトータル重量を表示
    //void disSums(float sum,float onewaight);
    void menu(int m);           //メニュー項目を表示
    void Option(int m);
    
    char get_char_mode(int mode);           //ユックリモード種別をアルファベットで取得
    void easy_view(const loadcell_t cel);  //簡単なモード説明を見せる
    void settings(const loadcell_t *cel);   //設定内容を表示
    void settings_rank2(const loadcell_t *cel);   //設定内容を表示
    void settings(int row, loadcell_t a);

    void displayNum(int num, int max);
    void displayNum2(int num, int max);
    void displayNum(float num, int max);
    void count(int cnt,int set_cnt);
    void setTitle(int t);           //目標設定のタイトル
    void setTitle2(int t);           //目標設定のタイトル
    void setTitle_kaisu3(int i ,int j);
    void setTitleTimen(int t);      //対面設定のタイトル
    
    void msgPrintf(int m);                   //指定メッセージ表示
    void messege(int row, int m);            //各種メッセージを表示
    void messege(int row, int m, int clsFlg);//画面クリア後に各種メッセージを表示
    void set_secret(int s);
    void set_secret_product(int s);
    void set_product_list(int s);
    void set_limit_list(int s);
    void set_option(int s);
    void set_basic(int s);
    void hit(int training_cnt, int sec);
   
    //大きい数字を表示するために各処理
    void createChar(uint8_t location, uint8_t charmap[]);   //ドットデータ登録
    void bigNumber(float num);      //大きい数字4桁まで表示
    void customNumbers(int num);    //0～9までの数字を表示
    void custom0();                 //大文字「1」を表示
    void custom1();
    void custom2();
    void custom3();
    void custom4();
    void custom5();
    void custom6();
    void custom7();
    void custom8();
    void custom9();
    
protected:

private:
    float _jm;      //Just　Moment　アニメの間隔
    int _bigColumn;
};

#endif