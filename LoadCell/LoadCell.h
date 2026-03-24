/*
 * 
 * mbed Load Cell
 * Copyright (c) 2015 ito shogo
 * 
 */

/*
  2015.06.12 定数SABUNで重量比較を電圧ではなくグラムで確認するよう変更
  AUTOZERO追加 0g重量が安定しているか確認する回数を設定できるように変更
  
  2015.06.13 setAinSpeedDummy 電圧値を取得しなおさずに、持っている電圧値をコピーするように変更
  2015.07.30 風袋引き用の電圧を配列で取得していたが配列の必要ないため単体の変数に変更
             不要な関数関数ArrayHeikinHWを削除
             定数定数HWHT_CNTを削除
*/

#ifndef LoadCELL_H
#define LoadCELL_H

#include "mbed.h"
#include "Settings.h"
#include "Common_t.h"

class LoadCell
{
public:
    //コンストラクタ
    LoadCell(PinName s0, PinName s1, PinName s2, PinName s3, PinName cell);
    
    void set_num_head(int num);     //テーブルコンビのヘッド数を指定
    void set_head(int num);         //取得するロードセル(AnalogIn)を指定
    void make_new_pointer();        //動的配列を確保
    void set_ini();
    //現在(_nowLoad)の各処理を行う関数群
    void set_fain();
    void set_fainV(float fain_volt);
    void set_sain();
    void set_sainV(float sain_volt);
    void set_sainVs(float sain_volt , int f_s_flag);
    void set_sain_2();
    void set_sain_3();
    void set_sain_4();
    void set_sain_5();
    void set_sain_6();
    void set_sain_7();
    void set_sain_8();
    void set_sain_9();
    void set_sain_10();
    void plus_fain();
    void minus_fain();
    float get_now_fain();
    float get_now_sain(int i);
    float get_now_weight_span(int f_s_flag);
    float get_now_weight();
    
    //指定ヘッドの値を取得
    float get_fain();
    float get_sain();

    void set_param(const parameter_t p);                //設定パラメータをse
    void set_product(const product_t p);                //設定パラメータをset
    void set_BARATSUKI(float baratuki3);
    void set_HUNDO(float hundo);
    void set_ZEROAREA(float zeroarea);
    void set_option(const option_t p);
    void set_span(const span_t* s);                    //スパンデータをセット
    void set_span_preset();
    
    void set_now_loadcell_data(const loadcell_t *l);    //選択されているロードセル位置のデータをset
    void set_now_span_data(const span_t *s);            //現在選択されているロードセルのスパン値を取得
    void set_imageweight(int head, float im_weight);
    void setAin(int repeat);        //重量取得用に電圧を取得
    void setAinSpeed(int repeat);   //ゆっくりモード時に電圧を取得。スピードが要求される場合に使用
    float diff_gram(/*int repeat*/);
    
    void setIniH_one();
    void setIniHWeight();
    void setIniHWeight_one(int a);
    void setInitial_onehead(int k);
    void setAin_zero(int _head);    //強制的に0にする(未積載とする)
    float getAin();
    float getAin_now(int head);
    float getAin_old(int head);
    float getWeight(int head);      //風袋込の重量取得
    float get_lastheadWeight();      //風袋込の重量取得
    //float getWeight2(int head);      //風袋込の重量取得
    float anteiWeight(int head);    //風袋込の検出重量が安定している重量を取得
    void break_antei(int head);    //風袋込の検出重量が安定している重量を取得
    
    float anteiWeight_rank(int head);    //風袋込の検出重量が安定している重量を取得
    float anteiWeight_rank3(int head);    //風袋込の検出重量が安定している重量を取得
    void getZero();                 //組み合わせ実行前にゼロ取得用
    void setZero(float zerodata); //指定値をゼロとする
        
    int get_lasthead(); 
    
    void set_cel_status(int head,int now);
    void reset_cel_status(int head);
    bool checkIn(int head);
    bool checkIn2(int head,int now);
    bool checkOut();
    float get_HWeight(int head);    //風袋の重量取得
    float check_Weight_ZERO();      //ゼロを取るための重量
    float ask_H_Weight(float volt); //風袋引き重量を取得
    
    void ArraySort(float f[], int num);         //配列ソート(データ内容を1要素ずらす)
    float ArrayHeikin(float f[], int num);      //配列の値の平均を取得する(重量)
        
    bool ArrayCheck();                  //配列の各要素の値(重量)が規定内の差分かを確認
    float getArrayWeight();             //配列に取得した電圧値全ての平均から重量取得
    float getArrayWeight(int setNum);   //配列に取得した電圧値を指定した分の平均から重量取得
    
    int getInHead();        //計量物が載っている位置を返す
    int getAnteiIn();       //計量が安定している位置を返す
    int getKaraHead();      //空計量皿の位置を返す(各Bitで1の場所が空とする)
    int getAnteiKara();     //空計量皿の位置を返す(重量安定していないなら空と判断)
    float getSum(int com);  //組み合わせ位置の合計重量を返す
    int over_weight();
    int over_weight_taimen();
    
    float now_voit(int loop);       //iHead位置の電圧を取得
    float now_volt(int loop);       //iHead位置の電圧を取得
    float median_filter(float data[]);
    void setAinSpeedDummy(int bit);
    int get_temperature(int head, int loop);  //温度データを返す
private:
    DigitalOut _s0, _s1, _s2, _s3;
    AnalogIn _cell;
    
    int _numHead;         //テーブルコンビのヘッドの数
    int _head;            //現在の操作ヘッド位置

    int* _pZero;       //重量値0gを渡すかの判断0は重量無し,1は重量有り
    int* _pAutoCount; //autoZero実行するか判断するフラグカウント
    int* _pAnteiCount; //170926 安定した重量がどれだけ繰り返されたか
    
    //データ構造体
    Val* _pVal;         //重量関係,保存しないデータ
    Val* _pVal_span;         //重量関係,保存しないデータ
    Val* _pVal_keiryo;         //重量関係,保存しないデータ
    loadcell_t _preset; //現在選択されているプリセットデータ
    span_t* _pSpan;
    span_t _nowSpan;
    product_t _product;
    parameter_t _param;               //設定パラメータ    
    option_t _option;   
    
    void antei_reset();
    void checkAntei();              //計量物の検出重量が安定していくか確認
    //float now_voit(int loop);       //iHead位置の電圧を取得
    float askWeight(float volt);    //電圧から重量を取得
    
    
    void MULUTIPLEXER(int taget);   //マルチプレクサ(計量皿を指定)
    void autoTare(float diff);      //ゼロ点調整
    void checkZeroRum();            //重量範囲を確認後、ゼロ点取得を実行
};

#endif
