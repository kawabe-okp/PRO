//----------------------------------------------------------
// 共通ヘッダファイル
// 各定数を列挙。設定をを変更しやすくSettingsファイルに値をまとめる
// Copyright (c) 2015 ito shogo
//----------------------------------------------------------

#ifndef SETTINGS_H
#define SETTINGS_H

//ソフトバージョン
extern const float VERSION;
extern const char VERSION_C[7];     //バージョン

//マクロじゃないと今のところ動かないのでDefineでしている
#define BK_CNT 7         //組合せ結果を一時保存する配列の数
#define SIZE  17        //スクロール文字数の制限
#define WHT_CNT 3       //重量(風袋込)の確認回数  //LoadCellで主に使用

//シフトレジスタを使用したLED点灯などに使用
//要素0番目がLED1番目の点灯
//要素11番目がLED12番目の点灯を行う
//想定最大数は16ヘッド
extern const int oneBit[];
extern const int oneLocation[];
//各種設定定数
extern const int TEST_Ver1T;
extern const int TEST_Ver2T;
extern const int TEST_Ver3T;
extern const int TEST_CHANGE;


extern const int PASS_SECRET   ;    //シークレットモードのパスワード(初期値7147)
extern const int PASS_DEFAULT_PARAMETER; //回数制限の数値を変更するパスワード
extern const int PASS_COMBI_PARAMETER; //組合せ系のパラメータ数値を変更するパスワード
extern const int PASS_SINGLE_PARAMETER; //単重系のパラメータ数値を変更するパスワード
extern const int PASS_ADMIN;        //管理者権限へのパスワード
extern const int PASS_ADMIN2;        //管理者権限へのパスワード
extern const int PASS_VIEW_PARAM;
extern const int PASS_VIEW_PARAM2;
extern const int PASS_SECRET2;
extern const int PASS_LIMITAD;      //回数制限の数値を変更するパスワード
extern const int PASS_MATRIX ;
extern const int PASS_MATRIX2 ;
extern const float SPEED           ;     //ループ間の待ち時間
extern const int   COMBI_MAX_CNT   ;      //ゆっくりモード時のカウント数の最大値
extern const int   DAMMY_MAX       ;
extern const float TRE_WHT         ;
extern const int KURI_TRE          ;       //トレーニングモード時の繰り返し回数
extern const int PASS_DEMO         ;    //マトリクス表示用へのパスワード
extern const int PRINT_ON          ;
extern const int TEST_MODE1;
extern const int TEST_MODE2;
extern const int TEST_MODE3;
extern const int PASS_TOP_USER;

extern const int SERVO_CHECK;

extern const float MINUS ;        //組み合わせ時に少し重量を軽くする
extern const int HUNDO_LIGHT;      //ライト版の場合の分銅重量予測(恐らく200g以下はライト版と判断する)
extern const int LOW_LIMIT_START;  //下限値から何gで振動検知を開始するか指定する

extern const int LOW_LEVEL_MENU;     //Lowレベル時の表示メニュー数(対面なし)
extern const int MENU_NO;          //メニュー表示される数(0から数える)(対面なし)
extern const int KINJITI_MAX_WHIGHT;        //近似値計測時の目標上限値重量

//ランク選別モード時に使用
extern const int RANK_KURI;
extern const int RANK_LED_TIME;

//mask用,各処理用にbit立ち上げ
extern       int LOW_HEAD;   //1-6ヘッド分のbitを立ち上げ
extern       int LOW_HEAD2;   //1-6ヘッド分のbitを立ち上げ
extern       int UP_HEAD;   //7-12ヘッド分のbitを立ち上げ
extern       int TOP1_HEAD;       //1-3,7-9   ヘッド分のbitを立ち上げ
extern       int TOP2_HEAD;       //2-4,8-10  ヘッド分のbitを立ち上げ
extern       int BOT2_HEAD;       //3-5,9-11  ヘッド分のbitを立ち上げ
extern       int BOT1_HEAD;       //4-6,10-12 ヘッド分のbitを立ち上げ
extern       int ALL_HEAD;   //1-12ヘッド分のbitを立ち上げ

//温度センサー用の各定数
extern const int TEMP_POSITION;     //温度センサーマルチプレクサの位置
extern const int TEMP_GRAM;         //1度につき2g重量差分を
extern const int TEMP_MAX;          //温度検知の最高温度
extern const int TEMP_MIN;          //温度検知の最低温度
extern const int ONE_COLUMN_HEAD;   //1列のテーブルコンビは8皿までありうる

#endif