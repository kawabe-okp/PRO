





#ifndef MAIN_H
#define MAIN_H

#include "mbed.h"
#include "FileData.h"
#include "Settings.h"
#include "Common_t.h"
#include "Display.h"
#include "Button.h"
#include "LoadCell.h"
#include "SoundEffct.h"
#include "Voice.h"
#include "Led.h"
//#include "Led_v4.h"
#include "led_matrix.h"
#include "MSCFileSystem.h"
#include "PowerControl.h"
#include "signal_set.h"

//製品種別
typedef enum productType{
    STANDARD = 1
    ,JUNIOR
    ,BIG
    ,MATRIX
    ,ITIGO
    ,PRODUCT_END
    ,STANDARD_D =101  //DEBUGの意味
    ,JUNIOR_D
    ,BIG_D
    ,MATRIX_D
    ,ITIGO_D
}proType;

//
typedef enum menu{
    MENU_START
    ,MENU_CHANGE_VALUE
    ,MENU_OPTION
    //,MENU_STOP_CELLS
    ,MENU_SPAN
    //,MENU_TRAINIG
    ,MENU_KEIRYO
    ,MENU_KEIRYO2
    ,MENU_KEIRYO3
    ,MENU_PASSWORD
    ,MENU_DEMO
    ,MENU_END
    ,MENU       //メニュー選択画面へ
}menu;


typedef enum menuop{
/* 0  1*/    OP_2ND          //準最適値
/* 1  2*/    ,OP_ZENNOSE     //全載せ再計量
/* 2  3*/    ,OP_ATONOSE     //後乗せ再計算
/* 3  4*/    ,OP_STOP_CELLS  //使用皿選択
/* 4  5*/    ,OP_USB_EN      //USB出力
/* 5  6*/    ,OP_CHAR_ROTATE //文字回転
/* 6  7*/    ,OP_RANK_OVER   //ランクオーバー時の警告ON
/* 7  8*/    ,OP_KOKUTI      //(ﾕｯｸﾘﾓｰﾄﾞ)成立した時・全て取られた時・その両方で「ﾋﾟｯ」と鳴らす
/* 8  9*/    ,OP_KIKAKUGAI_V //「規格外です」を喋らせるか否か
/* 9 10*/    ,OP_RANKSPEED   //「ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂ」を行うか
/*10 11*/    ,OP_BTN_SETTING //緑ﾎﾞﾀﾝを押したときの設定が「ｶｳﾝﾄ数変更」か「ｹｲﾘｮｳﾓｰﾄﾞ移行」か
/*11 12*/    ,OP_BEGINNER  //
/*12 13*/    ,OP_SEL_BEEP  //
/*13 14*/    ,OP_BUFFER  //BUFFERを変更する
/*14 15*/    ,OP_TIMER  //時計の変更
/*15 16*/    ,OP_NO_COMBI_V //時計の変更
/*16 17*/    ,OP_SECONDMBI_V //時計の変更
/*17 18*/    ,OP_NUMBER_COLOR //時計の変更
/*18 19*/    ,OP_O_count_lim
/*19 20*/    ,OP_O_lit_count_lim
/*20 21*/    ,OP_NAGAMONO_HOSEI  //servoを変更する
/*20 21*/    ,OP_NAGAMONO_HOSEI2  //servoを変更する
/*21 22*/    ,OP_SERVO1  //servoを変更する
/*22 23*/    ,OP_SERVO2  //servoを変更する
/*23 24*/    ,OP_SERVO3  //servoを変更する
/*23 24*/    ,OP_SERVO4  //servoを変更する
/*23 24*/    ,OP_ZERO_CHECK
/*23 24*/    ,OP_WAITING_LIMMIT
/*19 20*/    ,OP_SELAUTOZERO
/*19 20*/    ,OP_KEKKA_HYOUJI
/*19 20*/    ,OP_ALART_AUTOZERO
/*19 20*/    ,OP_FIX_VALUE
/* 0  */     ,OP_END

             ,OP_KUMI_COUNT
             ,OP_JUURYO_CHECK
             ,OP_TL_KUMI_FLAG
             ,OP_TL_SPMODE
             ,OP_TL_HYOUJI
             ,OP_TL_TARGET
             ,OP_TL_KURI_FLAG
             ,OP_END_END
            
}menuop;

//設定パラメータ
typedef enum paramet{
  /*  P_hundo          //分銅の重さ
    ,P_KURI           //繰り返し→平均値の回数 3000
    ,P_KURI_Y         //ゆっくりモード組み合わせ成立時の繰り返し回数
    ,P_KURI_J         //重量表示時の繰り返し回数
    ,P_CUT            //CUT以下の重量は組合せに参加させないように0gを代入する。
    ,P_CUT_Y          //CUT_Y以下の重量は、ゆっくりモード時に重量無しと判断する
    ,P_sabun          //ゆっくりモード時重量が安定したと判断する重量差
    ,P_liteLedOff     //ライト版ゆっくりモード時にLEDが消灯する条件(g)
    ,P_tairyu         //滞留していると判断する回数
    ,P_vivration      //振動検知機能 0:OFF 1:ON 2:ON LOWER LIMIT(目標重量+5gの範囲)
    ,P_zeroErr        //ゼロ点確認(0点エラー)と判断する重量
    ,P_zeroRange      //ゼロ点確認時の許容差分重量
    ,P_zerocount
    ,P_ZEROTOLERANCE
    ,P_ZEROAREA     //オートゼロ点＆スパン調整する重量の範囲(初期7.0)
    ,P_ZEROAREA2    //オートゼロ点＆スパン調整する重量の範囲(初期7.0)
    ,P_BARATSUKI    //各重量ﾎｯﾊﾟｰの重量安定したと判断する範囲(初期1.5)
    ,P_buffer       //計量値が組合せ範囲下限値の場合に、何g余裕をもたせるかの設定
    ,P_MINI_target   //対象計量物の最小値
    ,P_BARATSUKI_R    //各重量ﾎｯﾊﾟｰの重量安定したと判断する範囲(マトリクスのﾗﾝｸ用)
    ,P_BARATSUKI_GRAM    //各重量ﾎｯﾊﾟｰの重量安定したと判断する範囲(マトリクスのチェッカー用)
    ,P_buffer2        //皿数によるバッファ値を決める値
    ,P_bufferT        //単重のバッファを決める値(ランク・残量に適用)
    ,P_buffer_KAISU   //回数計量の最終組合せ値のバッファ値を入れる
    ,P_END*/
    
    
    P_hundo          //分銅の重さ
    ,P_KURI           //繰り返し→平均値の回数 3000
    ,P_KURI_Y         //ゆっくりモード組み合わせ成立時の繰り返し回数
    ,P_KURI_J         //重量表示時の繰り返し回数
    //,P_AUTOCUT
    ,P_CUT            //CUT以下の重量は組合せに参加させないように0gを代入する。
    ,P_CUT_Y          //CUT_Y以下の重量は、ゆっくりモード時に重量無しと判断する
    ,P_sabun          //ゆっくりモード時重量が安定したと判断する重量差
    ,P_BARA_KAKUNIN   //バラツキの範囲に何回入った時に安定したと認識させるか
    ,P_liteLedOff     //ライト版ゆっくりモード時にLEDが消灯する条件(g)
    ,P_tairyu         //滞留していると判断する回数
    ,P_vivration      //振動検知機能 0:OFF 1:ON 2:ON LOWER LIMIT(目標重量+5gの範囲)
    ,P_zeroErr        //ゼロ点確認(0点エラー)と判断する重量
    ,P_zeroRange      //ゼロ点確認時の許容差分重量
    ,P_zerocount
    ,P_ZEROTOLERANCE
    ,P_ZEROAREA     //オートゼロ点＆スパン調整する重量の範囲(初期7.0)
    ,P_ZEROAREA2    //オートゼロ点＆スパン調整する重量の範囲(初期7.0)
    ,P_BARATSUKI    //各重量ﾎｯﾊﾟｰの重量安定したと判断する範囲(初期1.5)
    ,P_buffer       //計量値が組合せ範囲下限値の場合に、何g余裕をもたせるかの設定
    ,P_MINI_target   //対象計量物の最小値
    ,P_BARATSUKI_R
    ,P_BARATSUKI_GRAM
    ,P_buffer2        //皿数によるバッファ値を決める値
    ,P_bufferT        //単重のバッファを決める値(ランク・残量に適用)
    ,P_buffer_KAISU   //回数計量の最終組合せ値のバッファ値を入れる
    ,P_BARATSUKI_SA   //各重量で安定した値が、九に異変化しても、この範囲内なら元の値を保持する機能
    ,P_END
}paramet;

typedef enum produt{
    PR_productType     //製品種別
    ,PR_limit_on       //0:通常1:回数制限ユーザー用2:ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂ用
    ,PR_Array          //配列0:2列(通常)1:リニア2:ワイド3:その他(ワンタッチャブル？)
    ,PR_Rating         //ロードセルの定格0:2kg 1:10kg 2:20kg
    ,PR_matrix_mode    //S+仕様か
    ,PR_Use_USB        //データ取り出せる仕様か
    ,RP_head           //テーブルコンビのヘッド数
    ,PR_END
} produt;

//目標設定する項目
typedef enum targetList{
    TL_KUMI_FLAG
    ,TL_COMBI_FLAG
    ,TL_KUMI_MIN
    ,TL_KUMI_MAX
    ,TL_KUMI_PRIORITY_1
    ,TL_KUMI_PRIORITY_2 
    ,TL_KATAMEN_HYOUJI
    ,TL_TANJU   
    ,TL_TARGET
    ,TL_JOUGEN
    ,TL_TAIMEN_TARGET
    ,TL_TAIMEN_JOUGEN
    ,TL_FUZOROI_BOUSHI
    ,TL_SECOND_ON_OFF
    ,TL_SPMODE
    ,TL_HYOUJI
    ,TL_RANK_ON_OFF
    ,TL_RANK_MIN
    ,TL_RANK_MAX
    ,TL_CHECK_HYOUUJI
    ,TL_KURI_FLAG
    ,TL_HOSEI
    ,TL_SPAN
    ,TL_END
}tList;

//目標設定する項目(複数重量設定の場合)
typedef enum targetList2{
     TL2_RANK_EN
    ,TL2_RANK_NO
    ,TL2_RANK_MIN
    ,TL2_RANK_MAX
    ,TL2_KUMI_MIN
    ,TL2_KUMI_MAX 
    ,TL2_TARGET
    ,TL2_JOUGEN
    ,TL2_END
}tList2;


//組合せ方法の種類
typedef enum KumiMethod{
/* 0 */    KUMI_NONE           //制限なし
/* 1 */    ,KUMI_LIMIT         //制限あり
    //,KUMI_COMBI         //組合せ制限
/* 2 */    ,KUMI_RANK          //ランク選別機能
/* 3 */    ,KUMI_KAIKYU        //階級区分(ランクごとの組合せ数、量設定)
/* 4 */    ,KUMI_KOSUU         //チェッカー(皿毎に残り個数を指示)
/* 5 */    ,KUMI_JUURYO        //重量表示タイプ
/* 6 */    ,KUMI_OYAKO         //親子組合せ(親:2,個10)
/* 7 */    ,KUMI_OYAKO_HALF    //親子組合せ(親:6,個6)
/* 8 */    ,KUMI_TAIMEN        //対面組合せ方法
/* 9 */    ,KUMI_KAISU1
/*10 */    ,KUMI_KAISU2
/*11 */    ,KUMI_PACK
/*12 */    ,KUMI_PACK2
/*13 */    ,KUMI_DENSO
/*14 */    ,KUMI_ZANCOM
/*15 */    ,KUMI_ZANCOM2
/*16 */    ,KUMI_COMZAN
    //,KUMI_RANKON        //内容は組合せ、ﾗﾝｸを表示
    
    ,KUMI_END      
}KumiMethod;

//組合せモードの種類、ゆっくりモードで使用
typedef enum Speed{
    SPEED_HIGH      //高速モード
    ,SPEED_LOW      //ユックリモード
    ,SPEED_DAMP     //ハイパーモード 
    ,SPEED_HYPER    //ハイパーモード 
    ,SPEED_END      
}Speed;

//組合せモードの種類、ゆっくりモードで使用
typedef enum Kinji{
    KINJI_ON      //高速モード
    ,KINJI_OFF    //音階出力モード
    ,KINJI_END      
}Kinji;

//ランク管理時にの種別
typedef enum Rank{
    RANK_NONE       //ランク制限なし
    ,RANK_HIGH      //上限値制限
    ,RANK_LOW       //下限値制限
    ,RANK_HIGE_LOW  //上下制限有り
    ,RANK_END      
}Rank;

typedef enum P_LIMIT_count{
    LIMIT_OFF           //=0
    ,NUMBER_LIMIT       //=1
    ,SELECTOR_LIMIT     //=2
    ,WIDE_LIMIT         //=3
    ,NIRA_LIMIT         //=3
    ,LIMIT_END
}P_LIMIT_count;

typedef enum P_ARRAY_count{
    ARRAY_DUAL           //=0
    ,ARRAY_LINEAR       //=1
    ,ARRAY_WIDE         //=2
    ,ARRAY_OTHER         //=3
}P_ARRAY_count;

typedef enum P_RATING_count{
    RATING_TWO           //=0
    ,RATING_BIG          //=1
    ,RATING_JUNIOR       //=2
    ,RATING_XL           //=3
}P_RATING_count;

typedef enum MATRIX_MODE{
    MATRIX_OFF
    ,MATRIX_ON
}MATRIX_MODE;

typedef enum RANKSPEED{
    RANK_SLOW
    ,RANK_FAST
}RANKSPEED;
typedef enum C_ROTATE{
    R_OFF
    //,R_ON
    ,R_01
    ,R_02
}C_ROTATE;

//yes_no判定(1=NO)
typedef enum yesno{
    YES
    ,NO
}yesno;
//ランク判別用
typedef enum RANK{
    R_NG1
    ,R_3S
    ,R_2S
    ,R_S
    ,R_M
    ,R_L
    ,R_2L
    ,R_3L
    ,R_4L
    ,R_5L
    ,R_6L
    ,R_7L
    ,R_8L
    ,R_9L
    ,R_NG2
    ,R_NG3
}RANK;
//ファイル読み込み時のデータ名
typedef enum read_f{
    R_PARAM
    ,R_PRE
    ,R_SPAN
    ,R_CELL
    ,R_BASIC
    ,R_DATAP
    ,R_CELLM
    ,R_NAME
    ,R_OPTION
    ,R_GENK
    ,R_GIRI
    ,R_DATA
    ,R_TIME
    ,R_END    
    
}read_f;


void print_body_param();
void starting();
void setUp();                                   //各種準備、前処理を実行
void auto_zero_point();                         //強制ゼロ点取得
void rtc_start();
void set_OOHEAD();

void rap_count();
void measure_1head();

//drive処理
void initial_drive_function();                  //計量時に使う関数の初期化
void driveStart();                              //組み合わせ処理開始
void drive_remeas();                            //ゆっくりモード中での「成立後での準最適値成立個数達成による再計量」時に使う処理 
void driveSlowly(combiData_t *com);             //ゆっくりモード。計量物を取り終わるまで処理ループ
//void driveSlowly(combiData_t *com,cntData_t *cnt);             //ゆっくりモード。計量物を取り終わるまで処理ループ
void drive_Rank_Sorting();                      //ランク選別スタート
void drive_Rank_Sorting2();                      //マトリクスランク選別スタート
void drive_Rank_Sorting3();                      //マトリクスランク選別スタート
void drive_Rank_Sorting4();                      //マトリクスランク選別スタート
void drive_kosuu();
void drive_juuryo();
combiData_t get_rank_location();
combiData_t get_rank_location2();
combiData_t get_rank_location3();
combiData_t get_rank_location4();
combiData_t get_check_location();
combiData_t get_juuryo_location();
void get_combi_location(combiData_t com[], int *secondCnt);         //組み合わせ取得
void get_combi_location_2nd(combiData_t com[], int *secondCnt, int combit );
void get_combi_location_multi(combiData_t com[], int *secondCnt);         //組み合わせ取得
void get_combi_location_AUTO(combiData_t com[], int *secondCnt, int no_combit);
void show_results_auto(const combiData_t *com , const combiData_t *com1 , const combiData_t *com2);

bool checkWeightTaget(combiData_t *com);
bool check_max_weight(int overHead);            //規定重量を超えていないか確認
void show_count(const countData_t *cnt);        //カウント状態を表示
void show_results(const combiData_t *com);      //組み合わせ結果表示
void show_results(const int com,const stateWeight_t stateW);
void show_rank(const combiData_t *com);         //ランク選別時の結果表示
void show_rank2(const combiData_t *com);         //ランク選別時の結果表示
void show_check(const combiData_t *com);
void show_check_juuryo(const combiData_t *com);
void get_miss_position(statePosition_t *stateP);
void get_taimen_miss(statePosition_t *stateP, const int mask);
void save_sagyou_file(int total_kumi_cnt,float average_kumi_value,int d_target,int d_jougen);
void save_sagyou_file2(int total_kumi_cnt,float average_kumi_value,int d_target,int d_jougen ,int ji,int hun ,int SerialNumber);
void save_sagyou_file2_date(int total_kumi_cnt,float average_kumi_value,int d_target,int d_jougen ,int ji,int hun ,int SerialNumber ,int year,int mon,int mday,int hour,int min);
void save_sagyou_file3(int total_kumi_cnt,float average_kumi_value,int d_target,int d_jougen ,int ji,int hun ,int SerialNumber);




//スパン系
void set_span();
void set_span_preset(int i);
void set_zero_span();
void set_zero_span_onehead(int i);
void check_zero_point(void);
int auto_span_ck();
int span_item_ck();

//各メニュー処理
bool select_menu_use(int m);
int selectMenu();
bool select_option_use(int m);
int selectMenu_option(int mode_op);
void displaySelect(int disNo);
void select_preset(int *headPosition);
void preset_default(int i,int data[],int headPosition);
bool preset_use(int i,int data[]);
void set_preset(int headPosition);
void set_preset2(int headPosition ,int set);
int multi_weight_set2(int headPosition, int data_KUMI_FLAG);
int multi_weight_set3(int headPosition, int data_KUMI_FLAG);
int set_mokuhyo_no(int Num);
void SetteiMokuhyo(int *xNum,data_t limitdata,int mode,const int *data);
void SetteiMokuhyo2(int *xNum,data_t limitData,int mode,int kumi_flug);
void change_data(int *xNum ,int limit_min ,int limit_max);
void Settei_suru_sinai(int *xNum,data_t limitData,int mode,int kumi_flug);
void startTraining(int treMode);
void password();
void start_keiryo(int head);
void start_keiryo2(int head);
void set_sercret_product();
void set_sercret_parameter();     //パラメーターデータを変更
void set_sercret_parameter_one(int set_memo , int endflg);
bool set_basic_setting();           //基本項目の設定を行う
bool set_stop_cells();
bool select_OPTION();
bool select_ZENNOSE();
bool select_ATONOSE();
bool select_katamen();
bool select_USB();
bool select_RANK_OVER();
bool select_char_rotate();
bool select_KOKUTI();
bool select_KIKAKUGAI_V();
bool set_option_setting(int modeop);
void matrix_test();
void time_set();

//ボタン上下キーで値変更をするテンプレート処理
template <class T>
bool Chenge_vaule_time(T *pnum, data_t limit);
template <class T>
bool Chenge_vaule_digit(T *pnum, data_t limit,int digi,  int row, int col);
template <class T>
bool Chenge_vaule(T *pnum, data_t limit);
template <class T>
bool Chenge_vaule2(T *pnum, data_t limit);
template <class T>
bool Chenge_vaule_JOUGEN(T *pnum, data_t limit, int minimum);
template <class T>
bool Chenge_name_vaule(T *pnum, data_t limit,int paramhead,int col);
template <class T>
bool Chenge_Measured_value(T *pnum, data_t limit);
void set_stopcell();   //不使用セル(＋12Headの対面点灯を組み合わせる)
int off_on_no_yes_result(int off_no,int now);//on off で判別させる処理
//int yesno_result(int now);//on off で判別させる処理
int RorG_result(int now, int rank_no);//次のランクの色を赤or緑に変更する

int Chenge_vaule_btnD(int option);
void setHeadVolt(int head, int repert);     //指定したロードセル電圧取得
void set_lastHeadVolt(int repert);
void set_count(cntData_t *cnt);
void set_count_force(cntData_t *cnt);

void ArrayIni(combiData_t *ikk, int num);   //配列初期化
int comp( const void *c1, const void *c2 ); //クイックソートで使用
int comp2( const void *c1, const void *c2 ); //クイックソートで使用
void slideMessage();                        //メッセージ(スライド移動する)
int bit_count(int bits);                    //立っているビットの数を数える

//設定ファイル処理
int combiHalf();            //1-6の組み合わせを取得
void drive_face_to_face();  //対面での組合せ計算処理
void selectTrainingMode();
bool check_combi_condition(int bits, const loadcell_t *valData);
bool set_name(int paramhead);
int no_chara(int headPos,int c_no);

//保存ファイル処理
void SD_update();
void SD_initialise();
void rankdata_sort(int Rsort);
void save_rankdata(int Rsort,int ji,int hun);
void save_rankdata2(int Rsort,int d_target,int d_jougen,int ji,int hun);
void USB_rankdata(int Rsort,int ji,int hun);
void USB_rankdata_date(int Rsort ,int ji,int hun,int SerialNumber ,int year,int mon,int mday,int hour,int min);
void USB_rankdata2(int Rsort,int d_target,int d_jougen,int ji,int hun);
void USB_check();
int USB_filecheck();
int USB_filecheck2(int year,int mon,int mday,int hour,int min);
int USB_filecheck3(int year,int mon,int mday,int hour,int min);
void result_LCD(int ji,int hun);
void result_LCD_servo(int ji,int hun);
void result_LCD_servo2();

//各処理用の関数
void announceRetention(int com, int stay_cnt[]);
int COMBI(loadcell_t val, int mask);
void driveFaceToFace();
void get_head_mask_volt(const int mask);    //マスク箇所の電圧(計量)を取得
void get_taimen_miss(statePosition_t *stateP, const int mask);      //マスク箇所の取り間違え位置を取得
void get_combi_half(loadcell_t val, statePosition_t *stateP, int mask);   //片側の組み合わせ状態を取得
int infoNow(int head, const int now);
void noCombiNews();
int get_title_push_esc(int iEsc, const int *data);  //目標設定のesc押下時の次のタイトルを取得
int get_next_title(int i, int* data);
void printname(int headPosition);
void miri_count();
//ゼロ点エラー処理を追加2017/01/26
//int get_nomal_position(int errBit);
//int check_weight(int errBit, int nomalBit);
//void zeroErrInquire(int ledOn);
//int is_zero_err(int ledOn, const float nomal, const float *checkWeight);

//ドットマトリクス用関数
void Setteirank(int *xNum,data_t limitData,int mode,int kumi_flug ,int i ,int en2 ,int en3 ,int en4 ,int en5 ,int en6 ,int en7 ,int en8 ,int en9 ,int en10);
//void Setteirank(int *xNum,data_t limitData,int mode,int kumi_flug);
void matrix();
int cel_led_bit(int i,int char_rank_col ,int char_antei_col ,int col);
int cel_led_bit_red(int i,int char_rank_col ,int char_antei_col ,int col);
int cel_led_bit_green(int i,int char_rank_col ,int char_antei_col ,int col);
int cel_led_bit(int i,int char_rank_col ,int char_antei_col ,int col);
void matrix_rank_column_RED (int i, int char_nn, int char_rn,  int col);
void matrix_column_RED (int i, int h_d, int char_nn, int char_rn,  int col);

bool kumi_en_check();
void set_limit_count();

void drive_Rank_Sorting_d();
combiData_t get_rank_location_d();
void drive_juuryo_d();
combiData_t get_juuryo_location_d();

//コンベア種別
typedef enum comveor_flger{
    COMV_BUTTON
    ,COMV_AUTO
    ,COMV_SUN1
    ,COMV_SUN2
    ,COMV_SUNBUT1
    ,COMV_SUNBUT2
    ,COMV_SENS
    ,COMV_NO_SEN
    ,COMV_STOP
    ,COMV_KANKETU1
    ,COMV_END
}comv_flg;

#include "Prototype_function.h"

#endif
