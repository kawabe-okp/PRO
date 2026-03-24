
int gLevel               =0;            //現在の操作レベル権限
bool TOP_USER  = false;                  //上位操作者
bool testmode1 = false;                  //テストプログラムONフラグ
bool testmode2 = false;                  //テストプログラムONフラグ
bool testmode3 = false;                  //テストプログラムONフラグ

    char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
                   ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼﾀ

const char suuhunomatikudasai[]={0xBD,0xB3,0xCC,0xDD,' ' ,0xB5,0xCF,0xC1,0xB8,0xC0,0xDE,0xBB,0xB2,0x00};      //ｽｳﾌﾝ ｵﾏﾁｸﾀﾞｻｲ
const char omatikudasai[]={0xB5,0xCF,0xC1,0xB8,0xC0,0xDE,0xBB,0xB2,0x00};      //ｽｳﾌﾝ ｵﾏﾁｸﾀﾞｻｲ


extern int KUMI_limit;            //回数制限の場合の上限
extern int KUMI_now;            //回数制限の場合の現在値
int MAX_HEAD = 14;        //最大ヘッド数
int MAX_TOUROKU = 24;
//ヘッド数(計量部)に応じて各設定値を変更するためグローバル
int gOyaHead;                       //親の場所
int gOyaHead_backup;                       //親の場所
int gKoHead;                       //親の場所
int gStartKinjiti;                  //近似値処理を行うまでに、何回通常の組み合わせ計算を行うかを設定
int gNowHeadBit;                    //TCヘッドの数だけbit立ち上げ
float OneWeight;                    //川 一つのヘッドの重さを一時的に入れておく場所
//0g確認する場合の計れる重量を指定
int gZeroMaxGram;
int gZeroMinGram;
float befour_anteiw[12]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float oldweightVale[12]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float nowweightVale[12]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float nowweightVale2[12]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float nowweightVale3[12]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float nowweightVale4[12]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
int   nowrank[12]= {0,0,0,0,0,0,0,0,0,0,0,0};
int   nearrank[4][12]= {{0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
};//振動があっても、ﾗﾝｸの変わり目より内側にあればOKとするための変数
int   recrank[12]= {0,0,0,0,0,0,0,0,0,0,0,0};
float totalweight0103[11]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float totalweight0406[11]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float totalweight0709[11]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float totalweight1012[11]= {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float totalweightA[11]   = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float totalweightB[11]   = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float totalweightC[11]   = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float totalweightD[11]   = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float totalweightN[12]   = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
int   totalranker0103[11]= {0,0,0,0,0,0,0,0,0,0,0};
int   totalranker0406[11]= {0,0,0,0,0,0,0,0,0,0,0};
int   totalranker0709[11]= {0,0,0,0,0,0,0,0,0,0,0};
int   totalranker1012[11]= {0,0,0,0,0,0,0,0,0,0,0};
int   totalrankerA[11]=    {0,0,0,0,0,0,0,0,0,0,0};
int   totalrankerB[11]=    {0,0,0,0,0,0,0,0,0,0,0};
int   totalrankerC[11]=    {0,0,0,0,0,0,0,0,0,0,0};
int   totalrankerD[11]=    {0,0,0,0,0,0,0,0,0,0,0};
float totalcheckwei[12]=   {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};      //大量パック詰めの重量を保存する変数
float image_weight[14]=   {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
int file_lost_sig=0;       //ファイル読み込みに失敗した際、出力するデータ
int start_r=R_3S; //ファイルに書き込むﾗﾝｸの最小
int iro_st_r=C_3S;//ﾗﾝｸの最初を指定する変数。いずれ変更可にするためのもの
int end_r=R_6L;
extern struct tm date;      //日付保存データ
extern bool time_reset;        //日付がリセットされているか t=された
struct tm dated;
struct tm w_date;           //起動時間を一時的保存データ
struct tm run_date;         //作業開始時間の一時保存データ
time_t seconds;
char dates[64];
int log_flag = 0;
int sphun_count = 11;      //スパン分銅の数
int span_mode_f = 0;            //現在スパン中かどうかのフラグ
int span_auto_f = 0;          //オートスパンで行うかどうかの選択
int span_item_f = 0;          //個々でスパン取るかの選択＿0は全て行う
int span_hundo[10];
int keiryou_kurikaesi = 2;
int no_use_cell = 0;          //計量時に重さを計量しないセルの選択(2進)
int re_meas2nd=0;               //準最適値での再計量フラグ
//int re_meas3rd=0;             //準最適値での再計量フラグ
extern int stop_cells = 0;      //計量時に強制的に0にするセルを選択
extern int stop_cells2 = 0;     //計量時に強制的に0にするセルを選択(データで保存するところ)
extern int stop_cells3 = 0;     //計量時に強制的に0にするセルを選択(機能で殺す必要があるところ)
extern int stop_cellsd = 0;     //計量時に強制的に0にするセルを選択(表示するLEDのｾｯﾃｲ)
int total_kumi_cnt = 0;         //組合せ成立(して取り終わった)した回数
//int sd_en=0;                  //SDが入ってるかのフラグ 1がfalse
float average_kumi_value = 0;   //
float kumi_value_data = 0;      //
int cnt_miss_and_kikakugai=0;   //取り間違いと規格外が発生した回数
int cnt_junsaiteki=0;           //準最適値が発生した回数

int bit_num = 0;    //成立した個数の保存
int bit_place = 0;    //成立した場所の保存
int kumiawase_er[12]= {0,0,0,0,0,0,0,0,0,0,0,0};
int comp_kumi=0;                //成立済みでまだとられてない箇所(ドットマトリクスの色反転用)
int comp_rank=0;                //ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂで安定した場所
//int match_kumi=0;
extern int iro[12];
int cel_led[12];
int matrix_mode         =0;                      //川  マトリクス表示モード
float res = 0;    //組み合わせ重量の上限値を取得
float res2  = 0;    //組み合わせ重量の上限値を取得
float res3  = 0;    //組み合わせ重量の上限値を取得
float res4  = 0;    //組み合わせ重量の上限値を取得
float res5  = 0;    //組み合わせ重量の上限値を取得
float res6  = 0;    //組み合わせ重量の上限値を取得
float res7  = 0;    //組み合わせ重量の上限値を取得
float res8  = 0;    //組み合わせ重量の上限値を取得
float res9  = 0;    //組み合わせ重量の上限値を取得
float res10 = 0;    //組み合わせ重量の上限値を取得
float rank1ave  = 0;    //組み合わせ重量の中間値を取得
float rank2ave  = 0;    //組み合わせ重量の中間値を取得
float rank3ave  = 0;    //組み合わせ重量の中間値を取得
float rank4ave  = 0;    //組み合わせ重量の中間値を取得
float rank5ave  = 0;    //組み合わせ重量の中間値を取得
float rank6ave  = 0;    //組み合わせ重量の中間値を取得
float rank7ave  = 0;    //組み合わせ重量の中間値を取得
float rank8ave  = 0;    //組み合わせ重量の中間値を取得
float rank9ave  = 0;    //組み合わせ重量の中間値を取得
float rank1dif  = 0;    //組み合わせ重量の上限値を取得
float rank2dif  = 0;    //組み合わせ重量の上限値を取得
float rank3dif  = 0;    //組み合わせ重量の上限値を取得
float rank4dif  = 0;    //組み合わせ重量の上限値を取得
float rank5dif  = 0;    //組み合わせ重量の上限値を取得
float rank6dif  = 0;    //組み合わせ重量の上限値を取得
float rank7dif  = 0;    //組み合わせ重量の上限値を取得
float rank8dif  = 0;    //組み合わせ重量の上限値を取得
float rank9dif  = 0;    //組み合わせ重量の上限値を取得
int name[25][10]= {
    {0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}
    ,{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}
    ,{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}
    ,{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}
};             //名前入力データ
bool led_r=true;  //ドットマトリクス表示の赤・緑選択
//bool led_r=false;  //ドットマトリクス表示の赤・緑選択
bool reck_alin    = false;       //全載せ再計算のON/OFF
bool reck_atonose = false;   //後載せ再計算のON/OFF
//int  defo_startKinji=0;      //片面表示で変更した時にデフォルトを取っておく
extern int katamen=0;              //0:no 1:1~6 2:7~12  12ﾍｯﾄﾞを片面だけ使用する(自分の皿でLEDが隠れる場合,6ﾍｯﾄﾞで使用するときの表示)
extern int para_head =0;
extern int para_array=0;

bool kumi_flug=false;       //今登録中の値(KUMI_FLUG判定用)
int ro_kumi=0;              //ランクオーバーした箇所(ビット換算)
extern bool auto_zero_off = false;  //強制ｵｰﾄｽﾊﾟﾝ

extern int headqty;
bool oneshoot_OK=false;     //音声発声を1回行ったか否か
int junsaiteki_loop = 0; //準最適値表示中のフラグ
int combi_chk = 0; //check_combi_conditionで、1~6、7~12のどちらを成立させるかを決める値
int nowlap=0;
int coulap=0;
int no_combi_rap = 0;
int no_combi_minits = 1;
bool demo_flg = false; //デモﾓｰﾄﾞかを判断するフラグ
int demo_continue=0; //0_OFF:1_組合せ:2_ランク  デモﾓｰﾄﾞで、組合せ/ランク選別に切り替えるために戻った場合
bool no_combi_v_flg = false;    //組合せがありません true：1度言ったひらめいた false: まだやってない
bool Compulsion_no_combi_v_flg = false;
int rank_RG = 0;
int miri_end=0;
int antei_checker=0;
extern int oya_hundo = 300;
float wait_old[12];
float disp_wait=0;
int   weight_stack[12]= {0,0,0,0,0,0,0,0,0,0,0,0};
int span_OK_kaisu = 15;
bool sentaku_back_flg = false;
bool esc_flg = false;




extern int DEBUG_MODE;
extern int use_span=0;
int now_TC_mode=0; //テーブルコンビの状態　0:メニュ　1:計量中　2:記録中  3:ゼロ点

cntData_t cnt= {0,0,0};                     //カウント機能の値保持用
int test_target[50];
float test_datas[50];
int loop_a = 0;
int signal_loop =0;

int bug=0;
int MARU_counter[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int MARU_lit_counter[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

bool servo_first_settting = false;
bool servo_settting = false;
bool keiryo_start_flg = false;
extern float op_n_hosei = 0.0;//OP_NAGAMONO_HOSEIの値をfloat型で保存する

bool forced_zero = false ;


int servo_histry=0;    //累計稼働回数
int servo_n_histry[2][12] = /*0列目：累計。１列目：現在カウント*/
                            {{0,0,0,0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0,0,0,0}};

int servo_b_histry[12] = /*ファイルから読み込んだ数*/
                            {0,0,0,0,0,0,0,0,0,0,0,0};
int zero_between_kyousei[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int kyousei_zero_counter[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

int servo_h_time[2] = {0,0}; /*サーボが稼働していた時間*0列目：時。１列目：分*/
                      
int kyousei_zero_kaisu = 0 ; /*サーボが何回動いたらゼロ点を取り直すか*/
int close_wait_flg = 0; //排出後、自動で閉まらせないフラグ
int close_wait_flg2 = 0; //排出後、自動で閉まらせないフラグ

int old_combi = 0; //前回排出した皿。
int op_h_buffer1[12] = /*ヘッド毎のﾊﾞｯﾌｧ(ﾅｶﾞﾓﾉﾎｾｲ)*/
                        {0,0,0,0,0,0,0,0,0,0,0,0};
float op_h_buffer2[12] = /*ヘッド毎のﾊﾞｯﾌｧ(ﾅｶﾞﾓﾉﾎｾｲ)*/
                        {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
int antei_flug[12] = {0,0,0,0,0,0,0,0,0,0,0,0};       //一度安定したかを見極めるフラグ
int antei_flug2[12] = {0,0,0,0,0,0,0,0,0,0,0,0};       //一度安定したかを見極めるフラグ

float AD_now_max[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float AD_now_min[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float AD_now_dif[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float AD_bas_max[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float AD_bas_min[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float AD_bas_dif[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
int hosei[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
float hosei_f[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
int KURI2nd = 3000;



int   no_zero_counter_zero = 0;
int   no_zero_counter[14] = {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0};
bool  forced_zero_flg[14] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false};


int auto_zero_check = 0;


int befoure_check = 0;      //一個前に確認した、排出待ちのビット
int now_____check = 0;      //一個前に確認した、排出待ちのビット
int check_same_count = 0;      //排出待ちのビットが同一だった時のカウント
int check_same_max   = 0;         //排出待ちのビットが同一だった時の最大

float span_preset[12][2]=  //[0g],[?g]登録ごとのスパン値、必要な時に1トウロク分だけ読み出し、ここに入れる
                         {{0.0 ,0.0}
                         ,{0.0 ,0.0}
                         ,{0.0 ,0.0}
                         ,{0.0 ,0.0}
                         ,{0.0 ,0.0}
                         ,{0.0 ,0.0}
                         ,{0.0 ,0.0}
                         ,{0.0 ,0.0}
                         ,{0.0 ,0.0}
                         ,{0.0 ,0.0}
                         ,{0.0 ,0.0}
                         ,{0.0 ,0.0}};
int span_hundo_preset = 100;
bool print_flg = false;










































