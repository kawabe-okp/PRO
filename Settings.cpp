#include "Settings.h"

//使用定数 使用状況によって変更する変数
const int oneBit[]     = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536};
const int oneLocation[]= {1,2,3,4, 5, 6, 7,  8,  9, 10,  11,  12,};
const int PASS_ADMIN                = 2;    //管理者権限へのパスワード
const int PASS_ADMIN2               = 2000;    //管理者権限へのパスワード
const int PASS_VIEW_PARAM           = 3;         //シークレットモードの一部を見るパスワード(初期値3)
const int PASS_VIEW_PARAM2          = 3000;      //シークレットモードの一部を見るパスワード(初期値3)
const int PASS_DEFAULT_PARAMETER    = 4; //パラメータ全体を変更するパスワード
const int PASS_MATRIX               = 5;    //マトリクス表示用へのパスワード
const int PASS_MATRIX2              = 5000;    //マトリクス表示用へのパスワード
//const int PASS_SECRET             = 4;         //シークレットモードのパスワード(初期値7147)
const int PASS_SECRET               = 7147;      //シークレットモードのパスワード(初期値7147)
const int PASS_SECRET2              = 4000;         //シークレットモードの一部を見るパスワード(初期値3)
const int TEST_MODE1                = 8000;         //シークレットモードの一部を見るパスワード(初期値3)
const int TEST_MODE2                = 8100;         //シークレットモードの一部を見るパスワード(初期値3)
const int TEST_MODE3                = 8200;         //シークレットモードの一部を見るパスワード(初期値3)
const int PASS_TOP_USER             = 100;


const int PASS_COMBI_PARAMETER      = 88; //組合せ系のパラメータ数値を変更するパスワード
const int PASS_SINGLE_PARAMETER     = 77; //単重系のパラメータ数値を変更するパスワード

const int TEST_Ver1T                = 7;
const int TEST_Ver2T                = 8;
const int TEST_Ver3T                = 9;
const int TEST_CHANGE               = 10;
const int SERVO_CHECK               = 1000;         //シークレットモードの一部を見るパスワード(初期値3)

const int PASS_LIMITAD              = 2355;      //回数制限の数値を変更するパスワード
//const int PASS_LIMITAD            = 5;      //回数制限の数値を変更するパスワード
const int PASS_DEMO                 = 9000;    //マトリクス表示用へのパスワード
const int PRINT_ON                  = 1111;    //マトリクス表示用へのパスワード


const float SPEED           =0.003;     //ループ間の待ち時間
const int   COMBI_MAX_CNT   =9999;      //ゆっくりモード時のカウント数の最大値
const int   DAMMY_MAX       =10000;
const float TRE_WHT         =100.0;
const int KURI_TRE          =150;       //トレーニングモード時の繰り返し回数

const float MINUS = 0.0;        //組み合わせ時に少し重量を軽くする
const int LOW_LIMIT_START = 5;  //下限値から何gで振動検知を開始するか指定する
const int KINJITI_MAX_WHIGHT =9000; //準最適値ON時の最大重量(計量開始時)
const int RANK_KURI = 3000;     //ランク選別モード時の移動平均回数
const int RANK_LED_TIME = 2;    //ランク選別モード時にLED点灯時間
const int HUNDO_LIGHT = 110;    //ライト版の場合の分銅重量(恐らく200g以下はライト版

//mask用,各処理用にbit立ち上げ
      int LOW_HEAD = 0x3f;          //1-6ヘッド分のbitを立ち上げ //8ヘッドも扱い出したので、固定値では無くした
      int LOW_HEAD2 = 0xff;          //1-8ヘッド分のbitを立ち上げ
      int UP_HEAD  = 0xfc0;          //7-12ヘッド分のbitを立ち上げ
      int TOP1_HEAD = 0x1c7;        //1-3,7-9   ヘッド分のbitを立ち上げ
      int TOP2_HEAD = 0x38e;        //2-4,8-10  ヘッド分のbitを立ち上げ
      int BOT2_HEAD = 0x71c;        //3-5,9-11  ヘッド分のbitを立ち上げ
      int BOT1_HEAD = 0xe38;        //4-6,10-12 ヘッド分のbitを立ち上げ
      int ALL_HEAD = 0xfff;         //1-12ヘッド分のbitを立ち上げ

//温度検知に関する各種設定
const int TEMP_POSITION = 15;       //温度センサーマルチプレクサの位置
const int TEMP_GRAM = 2;            //1度につき2g重量差分を
const int TEMP_MAX = 40;            //温度検知の最高温度
const int TEMP_MIN = -20;           //温度検知の最低温度

//1列のテーブルコンビは8皿までありうる
//ただし,2017-02-21日現在は6ヘッド版のみ
const int ONE_COLUMN_HEAD = 8;