#ifndef COMMON_T_h                                                                                                     
#define COMMON_T_h

#include "Settings.h"
#define WHT_CNT 3       //重量(風袋込)の確認回数  //LoadCellで主に使用
#define FILTER_SIZE 5   //

//構造体の定義
//組み合わせ場所を記憶しておく(昇順)
typedef struct combinationData_t {
    int kumi;       //組み合わせが成立箇所
    double total;      //組み合わせ分の総重量
    int priolity;      //組み合わせ分の優先度
} combiData_t;

//各種状態の位置、場所を取得
typedef struct statePosition_t{
    //position
    int combi;      //組み合わせ位置
    int combiYoungNum;
    int conbiOldNum;
    
    int on;         //計量物が置かれている場所
    int onOld;     //前回計量物が置かれていた場所
    int miss;       //取り間違えた場所
    int over;       //目標重量よりも過積重量の場所
}statePosition_t;

//各種状態の位置、場所を取得
typedef struct stateWeight_t{
    //weight
    float sum;          //組み合わせ最適値の重量値(1-12)
    float sumYoungNum;     //計量皿若番(1-6)の重量合計値
    float sumOldNum;       //計量皿後番(7-12)の重量合計値
}stateWeight_t;

//重量関係,保存しないデータ
typedef struct Val{
    float volt;             //ロードセルから取得した電圧値
    float vol_old;          //前回の取得電圧値
    float vol_old1;          //前回の取得電圧値
    float vol_old2;          //前回の取得電圧値
    float weight_old1;
    float weight_old2;
    float weight;           //重量値
    float wVolt[WHT_CNT];   //平均算出用(取得電圧値を保管していく)
    float hVolt;            //風袋引き電圧
    float hweight;          //風袋引きの重量
    float aweight;          //安定した重量
    float aweighb;          //安定した重量
    float spanvolt;             //ロードセルから取得した電圧値
    
}Val;


//各計量皿の設定値
//AIN(アナログイン)
typedef struct loadcell_t{
    int target;             //目情重量の下限値
    int jougen;             //目情重量の上限値(下限からの量)
    int katamen;            //片面使用か否か
    int fuzoroiBoushi;      //不揃い防止するための設定値(重量)
    int taimenTarget;       //対面計量時の目標重量
    int taimenJougen;       //対面計量時の重量上限値
    int secondOnOff;        //第二目標重量を使用するかのフラグ
    int kumi_flag;          //組み合わせ方法の指定。
    int kumi_min;           //組み合わせ本数指定時の最小値
    int kumi_max;           //組み合わせ本数指定時の最大値
    int lightTime;          //ledの点灯時間
    int spMode;             //高速mode→0、低速(ゆっくり)mode→1
    int rankOnOff;          //ランク警告機能のON/OFF切り替え
    int rankMin;            //ランク外警告の最小値を設定。最小値以下は警告
    int rankMak;            //ランク外警告の最大値を設定。最大値以上は警告
    int kumi_priolity_1;    //優先組み合わせ1
    int kumi_priolity_2;    //優先組み合わせ2
    int tanjuu;             //単重：1個分の重量
    int check_hyouji;       //単重：1個分の重量
    int combiflug;          //組合せ制限の種類
    int kuri_flag;          //１セル計測繰返し回数の変更
    
    //int now_no;       //川使用中のpreset番号
}loadcell_t;


//複数値を入れた場合の上限、下限値の入れ物(単体)
typedef struct unitWeight_t{
    int rank_en;     //最小重量 0=設定なし ,1=設定あり
    int rank_no;     //ランクの表示内容(led_matrix.hのenumに準拠)
    int rank_min;     //最小重量
    int rank_max;     //重量範囲
    int kumi_min;   //組み合わせ本数指定時の最小値
    int kumi_max;   //組み合わせ本数指定時の最大値
    int target;     //最小重量
    int jougen;     //重量範囲
}unitWeight_t;
//複数値を入れた場合の上限、下限値の入れ物(単体)
typedef struct multiWeight_t{
    struct unitWeight_t u01;
    struct unitWeight_t u02;
    struct unitWeight_t u03;
    struct unitWeight_t u04;
    struct unitWeight_t u05;
    struct unitWeight_t u06;
    struct unitWeight_t u07;
    struct unitWeight_t u08;
    struct unitWeight_t u09;
    struct unitWeight_t u10;
    //int priority;
    int Rcolor;
    int Rsort;
    int Rselect;
}multiWeight_t;




typedef struct span_t{
    float fain;     //スパン調整した時の電圧(荷重が無いときの重さ)first ain
    float sain;     //スパン調整した時の電圧(荷重が有るときの重さ)sabun ain
    float sain2;     //スパン調整した時の電圧(荷重が有るときの重さ)sabun ain
    float sain3;     //スパン調整した時の電圧(荷重が有るときの重さ)sabun ain
    float sain4;     //スパン調整した時の電圧(荷重が有るときの重さ)sabun ain
    float sain5;     //スパン調整した時の電圧(荷重が有るときの重さ)sabun ain
    float sain6;     //スパン調整した時の電圧(荷重が有るときの重さ)sabun ain
    float sain7;     //スパン調整した時の電圧(荷重が有るときの重さ)sabun ain
    float sain8;     //スパン調整した時の電圧(荷重が有るときの重さ)sabun ain
    float sain9;     //スパン調整した時の電圧(荷重が有るときの重さ)sabun ain
    float sain10;     //スパン調整した時の電圧(荷重が有るときの重さ)sabun ain
}span_t;

typedef struct basicSetting_t{
    int startKinji; //準最適値処理を開始するヘッド数(メニューから設定)
    int temperature; //温度(1皿目のスパン調整時に取得)
    //int soundPitch; //音の高さ(現在使用していない)
    //int volume;   //音量(ボリューム)(現在使用していない)
}basicSetting_t;

//設定パラメータ
typedef struct parameter_t{
//    int productType;    //製品種別
//    int limit_on;        //1:回数制限ユーザー用2:セレクター
//    int Array;             //配列0:2列(通常)1:リニア2:その他(ワンタッチャブル？)
//    int Rating;            //ロードセルの定格0:2kg 1:10kg 2:20kg
//    int matrix_mode;     //S+仕様か0:NO 1:YES
//    int use_usb;         //データ取り出せる仕様か
//    int head;           //テーブルコンビのヘッド数
    int hundo;          //分銅の重さ
    int KURI;           //繰り返し→平均値の回数 3000
    int KURI_Y;         //ゆっくりモード組み合わせ成立時の繰り返し回数
    int KURI_J;         //重量表示時の繰り返し回数
    int CUT;            //CUT以下の重量は組合せに参加させないように0gを代入する。
    int CUT_Y;          //CUT_Y以下の重量は、ゆっくりモード時に重量無しと判断する
    int sabun;          //重量が安定したと判断する重量差
    int BARA_KAKUNIN;   //バラツキの範囲に何回入った時に安定したと認識させるか
    int liteLedOff;     //ライト版ゆっくりモード時にLEDが消灯する条件(g)
    int tairyu;         //滞留していると判断する回数
    int vivration;      //振動検知機能 0:OFF 1:ON 2:ON LOWER LIMIT(目標重量+5gの範囲)
    int zeroErr;        //ゼロ点確認(0点エラー)と判断する重量
    int zeroRange;      //ゼロ点確認時の許容差分重量
    int zerocount;      //オートゼロ適用するのに何回安定するか
    float ZEROTOLERANCE;//オートゼロ適用の、安定したと判断する範囲
    float ZEROAREA;     //オートゼロ点＆スパン調整する重量の範囲(初期7.0)
    float ZEROAREA2;     //重量表示、ﾗﾝｸ選別(超高速)で使用する、ZEROAREA
    float BARATSUKI;    //各重量ﾎｯﾊﾟｰの重量安定したと判断する範囲(初期1.5)
    float buffer;       //計量値が組合せ範囲下限値の場合に、何g余裕をもたせるかの設定
    float MINI_target;   //対象計量物の最小値
    float BARATSUKI_R;    //各重量ﾎｯﾊﾟｰの重量安定したと判断する範囲(マトリクスのﾗﾝｸ用)
    float BARATSUKI_GRAM;    //各重量ﾎｯﾊﾟｰの重量安定したと判断する範囲(マトリクスのﾗﾝｸ用)
    float buffer2;       //皿数によるバッファ値を決める値
    float bufferT;       //皿数によるバッファ値を決める値
    float buffer_kaisu;       //ass皿数によるバッファ値を決める値
    float BARATSUKI_SA;  //各重量ﾎｯﾊﾟｰの重量安定したと判断する範囲(マトリクスのﾗﾝｸ用)
    
    
    
}parameter_t;


typedef struct product_t{
    int productType;    //製品種別
    int limit_on;        //1:回数制限ユーザー用2:セレクター 3:ワイド 4:PRO_暖機運転
    int Array;             //配列0:2列(通常)1:リニア2:その他(ワンタッチャブル？)
    int Rating;            //ロードセルの定格0:2kg 1:10kg 2:20kg
    int matrix_mode;     //S+仕様か0:NO 1:YES
    int use_usb;         //データ取り出せる仕様か
    int head;           //テーブルコンビのヘッド数
//    int hundo;          //分銅の重さ
}product_t;

//オプションデータ(特殊情報)
typedef struct option_t{
    int  d_jun;      //ダミー_ｼﾞｭﾝｻｲﾃｷﾁ
    bool reck_zen;   //全載せ再計算のON/OFF
    bool reck_ato;   //後載せ再計算のON/OFF
    int  d_siyo;     //ダミー_使用皿
    bool usb_EN;     //(S+のみ)USB保存のON/OFF
    int  c_rotate;    //(S+のみ)文字180回転
    bool rank_over;  //(通常のみ)ﾗﾝｸｾﾝﾍﾞﾂのover警告
    //int  katamen;    //(12Hのみ)0:no 1:1~6 2:7~12  12ﾍｯﾄﾞを片面だけ使用する(自分の皿でLEDが隠れる場合,6ﾍｯﾄﾞで使用するときの表示)
    int  kokuti;        //0:ﾄﾘｵﾜﾘ 1:ｾｲﾘﾂｼﾞ 2:ｾｲﾘﾂ/ﾄﾘｵﾜﾘ 3:ﾒﾛﾃﾞｨｰ
    int  kikakugai_v;
    int  rank_speed;  //ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂに行くか否かの選択
    int  btn_sett;  //ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂに行くか否かの選択
    int  beginner;  //初心者ﾓｰﾄﾞON/OFFの選択
    int  selbeep;  //取り間違い警告音の選択
    int  no_combi_v;  //「組合せがありません」音声ON/OFFの選択
    int  second_target; //第2目標値
    int  number_color;  //数次の色0：少ない→緑、多い→赤　1:少ない→赤、多い→緑
    int  O_count_lim;    //丸表示がx回あれば、しばらく点灯するモードへ移行する
    int  O_lit_count_lim;//点灯モードで点灯しておく回数
    int  nagamono_hosei;
    int  waiting_limit;
    int  selautozero;  //計量中のオートゼロをするかの選択
    int  kekka_hyouji;
    int  alart_autozero;
    int  alart_below_cut;
    int  fix_value;  //計量中のオートゼロをするかの選択


    
    //
}option_t;

//各種データ制限値
typedef struct data_t{
    int min;    //最小値
    int max;    //最大値
    int lcd;    //led表示位置
}data_t;

//カウント機能に必要な各種データ
typedef struct countData_t{
    int nowMode;    //現在のモード。モードによってカウント機能を使うか使わないか判断する。
    int num;        //カウント値
    int max;        //max値
}cntData_t;


#endif