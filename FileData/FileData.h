/*
 * 
 * mbed the FileData
 * Copyright (c) 2016 ito shogo
 * 2016.01.15 ファイル管理、操作用のライブラリを作成
 */ 

#ifndef FILE_DATA_H
#define FILE_DATA_H

#include "mbed.h"
#include "Common_t.h"
#include "Display.h"
#include "main.h"

class FileData
{
public:
#include "File_signal_set2.h" 
    void read_all_file();
    void save_preset();
    void save_SAGYOU(int ji, int hun, int presetno,int target,int kumi_total,float average );
    void save_param();                              //設定データを上書き
    void save_option();                              //オプションデータを上書き
    void read_time();                               //時間設定
    void save_time();                               //時間設定
    void save_stop_cells(int stop_cells);                         //不使用セル
    void read_stop_cells();
    void save_loadcell();                           //ロードセル関連のデータ上書き
    void read_unitweight();
    void save_unitweight();
    void save_basic_setting(const basicSetting_t bscData);  //基本設定の上書き
    void save_span();                               //スパン値の書き込み
    void save_span_preset(int presetNo);
    void save_span_hundo();
    void read_span();           //スパン値を読み込む
    void read_spanc();           //スパン値を読み込む
    void read_span_preset(int presetNo);           //スパン値を読み込む
    void read_span_hundo();
    void save_name();
    void save_limit1(int limit1);           //回数制限情報の読み取り
    void save_limit2(int limit2);           //回数制限情報の読み取り
    void read_limit1();           //回数制限情報の読み取り
    void read_limit2();           //回数制限情報の読み取り
    void read_product();
    void save_product();
    void save_hosei(int presetNo);
    void read_hosei(int presetNo);
    void read_name();
    void set_preset(int preset);
    void set_span(const span_t* s);                 //スパンデータのみの上書き
    void set_span2(const span_t* s,int i);                           //スパンデータ1個のみの上書き
    void set_load(const int data[], int head);
    void set_load(const loadcell_t *l, int head);
    void set_lweight(const int data[], int head, int i, int data_p, int data_r, int data_s);
    void set_param(const int data[], const float fdata[]);  //設定データの受取(多重配列型)
    void set_param(const parameter_t data);                       //設定データの受取(構造体型)
    void set_product(const product_t data);
    void set_option(const int data[]);
    void error_log(int err_no);
    void read_nagamono_buffer();
    void save_nagamono_buffer();

    int get_preset();                   //選択されているプリセット
    data_t get_limit_data(int num);
    data_t get_limit_sdata(int num);
    parameter_t get_param();            //設定情報(パラメータ)を返す
    product_t   get_product();            //設定情報(パラメータ)を返す
    option_t get_option();            //設定情報(オプションデータ)を返す
    loadcell_t get_load(int head);      //指定ヘッドのロードセルデータを返す
    multiWeight_t get_lweight(int head);
    span_t* get_span(int head);         //スパンデータを返す
    loadcell_t* get_load();             //セルデータをポインタ(構造体配列)で渡す(2016.05.13)
    basicSetting_t get_basic();         //基本設定値のデータを返す
    span_t* get_span();                 //スパンデータを構造体配列ポインタで返す
    int get_count_param();              //PARAMの設定数を取得
    int get_start_fparam();         //PARAMの浮動小数の設定が始まる位置
    
    void read_data_by_product(int type);    //製品別のデータ読み込みを行う
    
    void make_new_pointer2();        //動的配列を確保
    void read_param();          //読み取った設定データを取得
    void read_paramg();          //読み取った設定データを取得
    void read_option();          //読み取ったオプションデータを取得
    
    int _preset; //現在選択されている計量皿 
private:
    //int _preset; //現在選択されている計量皿 
    product_t _product;
    parameter_t _param;
    loadcell_t* _pCel;
    span_t* _pSpan;
    basicSetting_t _basicData;
    data_t* _pData;
    data_t* _pSData;
    multiWeight_t* _pMweight;
    option_t _option;
    
    void make_new_pointer();        //動的配列を確保
    void read_data(const char *fileName);
    void read_SData();
    void read_preset();         //プリセットに登録されているデータ
    //void read_param();          //読み取った設定データを取得
    //void read_option();          //読み取ったオプションデータを取得
    void read_loadcell();       //ロードセル関連のデータ読み込み
    void read_basic_setting();  //基本設定値を読み込む
};

#endif