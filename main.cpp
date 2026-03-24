

#include "main.h"
//PRO用プログラム
//class
Display lcd(p24,p26,p27,p28,p29,p30);   //LCD表示用
FileData file;                          //ファイル読み書き
//***********基板の改造具合によって変更が必要**************************
//***********デフォルト************************************************
//Led led(p13,p14,p15,p11);               //LED操作
//***********未改造の基板用************************************************
//Led led(p13,p14,p15,p11,p17,p18,p19);               //LED操作
//***********改造済の基板用************************************************
Serial signal_in1(p13, p14);  // tx, rx
//Led led(p13,p14,p15,p11,p17,p18,p19);               //LED操作
Led led(p12,p17,p15,p11,p18,p19);               //LED操
  //Led led(p15,p18,p19,p11,p17);               //LED操作
  
  //Led led(p12,p17,p15,p11,p18,p19);               //LED操作
//***********************************************************
SoundEffct se(p25);                     //音(効果音)
Button btn(p5, p6, p7, p8);             //ボタン
LoadCell cel(p20, p21, p22, p23, p16);  //ロードセル
Voice v(p9, p10);                       //音声
Timer t;                                //時間トレーニングモードで使用
Ticker flipper;                         //川  マトリクス用
Ticker miri_sec;
Ticker meas_rap;                         //川  ラップ取得用
Ticker wait_rap;                        //あtる機能を一定秒数動作させない＠＠＠
Ticker meas_rap2;                         //川  ラップ取得用

MSCFileSystem msc("usb"); // Mount flash drive under the name "msc"
Serial pc(USBTX, USBRX);


//構造体
product_t product;
loadcell_t load;                        //各ロードセルなどのデータ値
parameter_t param;                      //パラーメータ値
basicSetting_t bscData;                 //基本設定値
combiData_t  olddata;                    //川 前回計測のデータ
combiData_t  newdata;                    //川 今回計測のデータ
combiData_t resultCombi2;            //組み合わせ結果(最小値取得用)(drivesloryで使用)
combiData_t  littleCombi[BK_CNT] = {0,0.0,0};;
option_t option;                        //オプションデータ17/10/16
unitWeight_t  uweight;
multiWeight_t mweight;

char VERSION_MODEL[3] = {'P' ,'R'};  //機種
const float VERSION = 5.04;  //バージョン
const char VERSION_C[7] = {'.' ,'4' ,'5' ,' ' ,' ' ,' ' ,0x00};     //バージョン
combiData_t Confilcombi[5] = {{0,0.0,0}
                             ,{0,0.0,0}
                             ,{0,0.0,0}
                             ,{0,0.0,0}
                             ,{0,0.0,0}};     //(決定済組合せ)
int total_combi_count = 0;
int next_meas_head = 0;
bool go_password = false;
bool test_print_mode = false;

#include "main_gloval_function.h"

#include "op_nagamono_hosei.h"
#include "signal.h" 
#include "serial_servo.h"
#include "main_touroku.h"
#include "main_code2.h"
#include "main_code3.h"
#include "main_code4.h"
#include "kaisuu3.h"
//#include "kaisuu.h"
//#include "kaisuu2.h"
#include "pack_checker.h"
#include "denso.h"
#include "PC_Serial.h"
#include "zanryoukumiawase.h"
#include "zanryoukumiawase2.h"
#include "kumiawasezanryo.h"

#include "led_matrix2.h"
#include "new_combi.h"
#include "demo1.h"
#include "option_chenge.h"


//Main関数
int main()
{
    BrownOut_Reset_Enable();        //すべてのモードでブラウンアウト検出（BOD）回路をオンのままにします。
    BrownOut_Global_Enable();       //すべてのモードでブラウンアウト検出（BOD）リセット回路をオンのままにします。
    pc.baud(115200);
    pc.attach(pc_rx, Serial::RxIrq);
    //pc.printf("AAA\r\n");
    signal_in1.baud(115200);
    signal_in1.attach(&signal1_in, Serial::RxIrq);
    for(int a=0; a<12; a++) {
        iro[a]=C_NULL;
    }
    led.ledAllOff();        //LED消灯
    /*iro[1]=0;    iro[2]=0;    iro[3]=0;    iro[4]=0;    iro[5]=0;
    iro[6]=0;    iro[7]=0;    iro[8]=0;    iro[9]=0;    iro[10]=0;    iro[11]=0;*/
    flipper.attach( &matrix, 0.002);   //川  マトリクス表示用のタイマー割り込みピッチ設定
    int mode = MENU_START;  //初期表示するモードを設定
    int mode_op = 0;  //初期表示するオプションのモードを設定
    bool auto_zero_go =true; //ゼロ点調整するかのフラグ(初回以降は飛ばすためのフラグ)
    bool ch_back = true; //バックしたか(注バックだったらfalse)
    btn.new_btn();          //スイッチ(New)取得
    file.make_new_pointer2();               //FileRead
    starting(); //起動処理
    rtc_start();            //RTC(マイコン内時計)開始
    wait(2);
    //下ボタン押下時パスワード入力へ
    now_TC_mode = -1;
    if(btn.get_btn() == BTN_DOWN) {
        send_i(WAIT_SIG, 0, 0);
        password();
        mode = MENU;
        if(demo_flg==true){
            mode = MENU_START;
        }
        send_i(WAIT_END_SIG, 0, 0);
    }
    now_TC_mode = 0;

    // v.change_vpitch(1,64);   //
    //v.change_vpitch(2,32);   //
    //v.change_vpitch(0,32);   //
    //printf("A:");
    setUp();                //前処理、セットアップ
    //file.save_unitweight();      //変更されたデータを受け渡し
    //wait(2);
    //*******************時間表示*******************
    //for(int i=0;i<1000;i++){
    
    if(product.use_usb==1 && option.usb_EN==true){   
        if(time_reset == false && gLevel ==0 && demo_flg ==false){
            lcd.blank(0);
            lcd.blank(1);
            for(int d=0;d<50;d++){
                seconds = time(NULL);
                tm *dates = localtime(&seconds);
                lcd.locate(1,0);
                lcd.printf("%04d/%02d/%02d",
                    dates->tm_year + 1900,dates->tm_mon + 1,dates->tm_mday);  //ヘッド数表示
                lcd.locate(2,1);
                lcd.printf("%02d:%02d:%02d",
                    dates->tm_hour,dates->tm_min,dates->tm_sec);  //ヘッド数表示
                wait(0.1);
            }
        }
    }
        
    if(gLevel == 99){
        gLevel = 0;
        matrix_test();
    }
    
    
    //printf("Time as a string = %s\r\n", ctime(&seconds));
    //lcd.locate(0,0);
    //strftime(dates, sizeof(dates), "%Y/%m/%d %a", localtime(&seconds));
    //lcd.printf("%s\n", dates);
    //lcd.locate(0,1);
    //strftime(dates, sizeof(dates), "%H:%M:%S", localtime(&seconds));
    //lcd.printf("%s\n", dates);
    //wait(1);
    //lcd.cls();
    //}
    //ただのテスト***********************************************
    //printf("A:");
    //printf("A:%d",oneBit[product.head+1]*2-1);
    //***********************************************
    
    servo_set();
    servo_first_settting = true;
    //裏操作の場合にはゼロ点エラーを確認しない
    if(mode != MENU) {
        set_stopcell();
        /*
        //if(demo_flg==false){
            
        if(0){
            //check_zero_point();     //ゼロ点確認
            v.output_audio(MSG_KIDO);   //起動音声
            
            led.ANIME();                //起動アニメーション
            v.wait_echo();
            //printf("0000000000000000000000000000000000\r\n");
            auto_zero_point();          //ゼロ点取得
            //printf("1111111111111111111111111111111111\r\n");
            
        }*/
        // pc.printf("%d\r\n",option.usb_EN);
        if(product.use_usb==1 && option.usb_EN==true) {
            USB_check();
        }
        wait(0.5);
    } else {
        v.wait_echo2();
    }
    //if(product.limit_on==SELECTOR_LIMIT) {
    //    option.rank_speed=1;
    //} 
    btn.old_btn();               //スイッチ更新
    if(product.use_usb==1 && option.usb_EN==true) {
        SD_initialise();
    }
    //トレーニングモード、乱数の種にtimerを利用する
    
    t.reset();
    t.start();
    set_stopcell();   //不使用セル(＋12Headの対面点灯を組み合わせる)
    // for(int a=0;a<20;a++){
    //     iro[0]=CR_1+a;
    //     wait(1);
    // }
    //int x= 59;
    /*char bit_led=0x00 ;
    for(int i=0;i<(para_head/2);i++){
        if(i>0){
            bit_led=bit_led<<1;
        }
        bit_led=bit_led | 1;
        printf("%3d bit:%3x\r\n",i,bit_led);
    }
    printf("A:%3d    ",(x & bit_led) << para_head/2);
    printf("B:%3d\r\n",(x & bit_led<<para_head/2));
    x = ((x & bit_led) << para_head/2 )| ((x & bit_led<<para_head/2)  >> para_head/2);
    printf("AB:%3d\r\n",x);*/
    
    
    
    for(int l=0; l<product.head; l++) {
        servo_b_histry[l] = servo_b_histry[l] + servo_n_histry[0][l] + servo_n_histry[1][l];
        servo_n_histry[0][l] = 0;
        servo_n_histry[1][l] = 0;
    }
    
    //defo_startKinji = bscData.startKinji;
    
    v.wait_echo2();
    btn.end_force();

    /*
    if (product.limit_on == 6 ){
        bool loop_timer_end_flg = false;
        int loop_timers_count = 0;
        int loop_count_bunbo = (5 * 60) / 16;
        
        while(loop_timer_end_flg == false){
            seconds = time(NULL);
            tm *dated = localtime(&seconds);
            if ((w_date.tm_min+5 <= dated->tm_min || w_date.tm_hour < dated->tm_hour || w_date.tm_mday < dated->tm_mday) ||
                (w_date.tm_min > dated->tm_min && ((w_date.tm_min+5)-(dated->tm_min+60))<1)) {
                    loop_timer_end_flg = true;
            }else{
                loop_timers_count = ((5 * 60) - (w_date.tm_min * 60 +  w_date.tm_sec)) / loop_count_bunbo;
                lcd.locate(0,0);
                lcd.printf(suuhunomatikudasai);
                lcd.locate(0,1);
                for(int p = 0; p < 16 ; p++){
                    
                    if(p < loop_timers_count + 1){
                        lcd.printf("0");
                    }else{
                        lcd.printf(" ");
                    }
                }
            }
            wait(1);
        }

    }
    */
    while(1) {
            btn.old_btn();               //スイッチ更新
            btn.new_btn();               //スイッチ更新
        
        switch(mode) {
            case MENU:
                now_TC_mode = 0;
                auto_zero_go = true;
                set_stopcell();   //不使用セル(＋12Headの対面点灯を組み合わせる)
                printf("MENU\r\n");
                mode = selectMenu();    //実行するメニューを選択
                led.ledAllOff();        // LED消灯
                if(btn.up_press() == true && btn.down_press() == true){
                    se.Touch_01();
                    se.Touch_01();
                    se.Touch_01();
                    wait(1);
                    se.chime_01();
                    password();
                    mode = MENU;
                }
                break;
            case MENU_START:            //簡単登録どれにする？(プリセット選択)
            case MENU_CHANGE_VALUE:     //簡単登録する？(目標設定)
                //回数制限チェック**************************************************
                if(product.limit_on==NUMBER_LIMIT && mode == MENU_START) {
                    if(kumi_en_check()==false) {
                        se.mistake();                   //警告音
                        v.over_limit();
                        mode = MENU;
                        continue;
                    } else {
                        lcd.cls();
                        lcd.locate(0,1);
                        lcd.printf("%d",KUMI_limit-KUMI_now);
                        v.last_limit(KUMI_limit-KUMI_now);
                        //KUMI_now =KUMI_now +1;
                        //file.save_limit2(KUMI_now);
                    }
                }
                //****************************************************************
                if(demo_flg==false ||demo_flg==true&&demo_continue==0){
                    if(mode == MENU_START) {         printf("KEIRYO\r\n");}
                    if(mode == MENU_CHANGE_VALUE) {  printf("TOUROKU\r\n");}
                    displaySelect(mode);    //プリセット選択
                }

                if(btn.esc()) {
                    se.Touch_01();
                    mode = MENU;  //esc押下なら処理終了
                }
                //組合せ方法ﾁｪｯｸ(制限してるTCで、使用可能外だったら、強制的に登録へ進む)**************************************************
                if(mode == MENU_START /*&& product.limit_on != 0*/) {
                    file.save_preset();                 //選択されたプリセットを上書き
                    if(touroku_data_check() == false){
                        mode = MENU_CHANGE_VALUE;
                    }
                    /*
                    cel.set_now_loadcell_data(&load);   //現在選択されているプリセット(ロードセル)をset
                    if((product.limit_on==NUMBER_LIMIT && (load.kumi_flag >= KUMI_LIMIT || load.spMode == 0 )) ||
                            (product.limit_on==SELECTOR_LIMIT && load.kumi_flag != KUMI_RANK )||
                            (product.head==1 && (load.kumi_flag != KUMI_RANK || load.kumi_flag != KUMI_KOSUU))) {
                        v.Unregister();
                        v.output_audio2();
                        mode = MENU_CHANGE_VALUE;
                    }*/
                }
                //****************************************************************

                //目標設定を行う
                if(mode == MENU_CHANGE_VALUE) {
                    set_preset(file.get_preset());
                    katamen=0; //グローバル変数へ値保存
                    load.katamen = 0;
                    set_stopcell();   //不使用セル(＋12Headの対面点灯を組み合わせる)
                    led.REGIS(0xfff^stop_cellsd);
                    if(btn.esc()) continue;  //esc終了は、再度目標設定
                }

                //組合せスタート
                if(mode == MENU_START) {
                    now_TC_mode = 1;
                    cnt.nowMode = 0;       //処理モード取得
                    cnt.num     = 0;       //処理モード取得
                    cnt.max     = 0;       //処理モード取得
                    keiryo_start_flg = true;
                    if(load.kumi_flag > KUMI_LIMIT){load.spMode = SPEED_DAMP;}
                    if(servo_connection==true && load.spMode == SPEED_DAMP){
                        //moving2=99;
                        send_i(EJECT_RESET, 0, 0);
                        loop_a = 0;
                        while(check_signal()==false&&loop_a<=10){
                            loop_a+=1;
                            wait(0.1);
                        }
                    }
                    //if(demo_flg==false ||demo_flg==true&&demo_continue==0){
                        file.save_preset();                 //選択されたプリセットを上書き
                        cel.set_now_loadcell_data(&load);   //現在選択されているプリセット(ロードセル)をset
                        katamen=load.katamen; //グローバル変数へ値保存
                        set_stopcell();   //不使用セル(＋12Headの対面点灯を組み合わせる)
    
                        //USB差し込みチェック
                        if(product.use_usb==1 && option.usb_EN==true &&
                                //(load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_COMBI || load.kumi_flag == KUMI_LIMIT || load.kumi_flag == KUMI_RANK)) {
                                (load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT || load.kumi_flag == KUMI_RANK)) {
                            USB_check();
                        }
                        if(demo_flg==false ||(demo_flg==true && auto_zero_go == true)){
                            if(servo_connection==true){
                                send_i(KEIRYO_ST,0,0); wait(0.2);
                                for(int header=0; header<product.head; header++){
                                    send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
                                }
                            }
                            lcd.messege(TOP,DIS_WAIT,CLS);
                            lcd.messege(LOW,DIS_ZERO);
                            wait(0.5);
                            //auto_zero_point();                  //ゼロ点取得
                            //auto_zero_go = false;
                            if(servo_connection==true){
                                for(int header=0; header<product.head; header++){
                                    send_i(SERVO_1 ,header,servo_span[header][0]); wait_res();
                                }
                            }      
                            file.read_span_preset(file.get_preset());

                            if(span_preset[0][0] != 0.0 &&
                            (load.kumi_flag == KUMI_JUURYO /*|| load.kumi_flag <= KUMI_LIMIT*/) &&
                            product.limit_on == NIRA_LIMIT &&(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)
                            ) {
                                file.read_span_preset(file.get_preset());
                                cel.set_span_preset();
                                cel.set_fainV(span_preset[i][0]);      //現在のロードセルのFAIN値を取得
                                cel.set_sainV(span_preset[i][1]);      //現在のロードセルのSAIN値を取得
                                cel.set_sainVs(0.0           ,2);      //現在のロードセルのSAIN値を取得
                                cel.set_sainVs(0.0           ,3);      //現在のロードセルのSAIN値を取得
                                cel.set_sainVs(0.0           ,4);      //現在のロードセルのSAIN値を取得
                                cel.set_sainVs(0.0           ,5);      //現在のロードセルのSAIN値を取得
                                cel.set_sainVs(0.0           ,6);      //現在のロードセルのSAIN値を取得
                                cel.set_sainVs(0.0           ,7);      //現在のロードセルのSAIN値を取得
                                cel.set_sainVs(0.0           ,8);      //現在のロードセルのSAIN値を取得
                                cel.set_sainVs(0.0           ,9);      //現在のロードセルのSAIN値を取得
                                cel.set_sainVs(0.0           ,10);      //現在のロードセルのSAIN値を取得
                            }else{
                                file.read_span();                       //各スパン値
                                cel.set_span(file.get_span());      //スパン値を取得
                                file.read_span_hundo();
                            }
                            wait(1);   
                            auto_zero_point();                  //ゼロ点取得
                            auto_zero_go = false;           
                        }
                    //}
                    /*if(demo_flg==true){
                        if(demo_continue==1)     {
                            load.kumi_flag=KUMI_NONE;}
                        else if(demo_continue==2){
                            load.kumi_flag=KUMI_RANK;}
                        else if(demo_continue==3){
                            load.kumi_flag=KUMI_JUURYO;}
                        while(btn.any_press()==true){
                            wait(0.1);
                        }
                    }
                    demo_continue=0;*/
                    printf("KEIRYO_START\r\n");
                    if(0){
                    }else{
                        //対面だった場合、特殊な処理
                        if(load.kumi_flag == KUMI_TAIMEN) {
                            drive_face_to_face();
                        } else if(load.kumi_flag == KUMI_RANK) {
                            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                                //if(option.rank_speed == RANK_FAST || product.limit_on == SELECTOR_LIMIT) {
                                if(product.use_usb==0 || option.usb_EN == 0) {   
                                    drive_Rank_Sorting4();
                                } else if(option.rank_speed == RANK_FAST) {
                                    //option.rank_speed = RANK_FAST;
                                    drive_Rank_Sorting3();
                                } else {
                                    drive_Rank_Sorting2();
                                }
                                //BARATSUKIを変えてるので、読み込み直す
                                file.read_param();
                                param = file.get_param();           //パラメータ(設定情報)を取得
                            } else {
                                drive_Rank_Sorting();
                            }
                        } else if((load.kumi_flag == KUMI_KOSUU)&&(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                            drive_kosuu();
                        } else if((load.kumi_flag == KUMI_JUURYO)&&(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                            drive_juuryo();
                            
                        } else if((load.kumi_flag == KUMI_PACK)&&(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                            drive_pack();
                        } else if((load.kumi_flag == KUMI_PACK2)&&(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                            drive_pack();
                        } else if(load.kumi_flag == KUMI_DENSO){
                            gOyaHead_backup = gOyaHead;
                            gOyaHead = 0x01;
                            gKoHead  = ALL_HEAD & (ALL_HEAD ^ gOyaHead);
                            driveStart_denso();
                            gOyaHead = gOyaHead_backup;
                            gKoHead  = ALL_HEAD & (ALL_HEAD ^ gOyaHead);
                        } else if((load.kumi_flag == KUMI_KAISU1)&&(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                            drive_kaisuu3();
                            set_OOHEAD();
                        } else if((load.kumi_flag == KUMI_KAISU2)&&(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                            drive_kaisuu3();
                            set_OOHEAD();
                        } else if((load.kumi_flag == KUMI_ZANCOM)&&(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                            driveStart_zancom();
                        } else if((load.kumi_flag == KUMI_ZANCOM2)&&(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                            driveStart_zancom2();
                        } else if((load.kumi_flag == KUMI_COMZAN)&&(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                            driveStart_comzan();
                        } else {
                            //if(product.use_usb==1 && option.usb_EN==true) {
                                SD_initialise();
                            //}
                            //defo_startKinji = bscData.startKinji;
                            //if(load.katamen > 0){
                            //    bscData.startKinji = 6;
                            //}
                            if(load.kuri_flag==0){
                                KURI2nd = param.KURI;
                            }else if(load.kuri_flag==1){
                                KURI2nd = 9000;
                            }else if(load.kuri_flag==2){
                                KURI2nd = 6000;
                            }else if(load.kuri_flag==3){
                                KURI2nd = 3000;
                            }else{
                                KURI2nd = param.KURI;
                            }
                            driveStart();
                            
                            
                        }
                    }
                    keiryo_start_flg = false;
                    v.wait_echo2();
                }
                if(servo_connection==true){
                    send_i(KEIRYO_EN,0,0);
                }
                katamen=0; //グローバル変数へ値保存(データリセット)
                load.katamen = 0;
                set_stopcell();
                //bscData.startKinji = defo_startKinji;
                if(demo_flg==false){
                    mode = MENU;    //処理終了時は、メニューへ
                }else if(demo_flg==true && btn.esc_press() == false){
                    mode = MENU_START;
                }
                btn.end_force();
                now_TC_mode = 0;
                cnt.nowMode = 0;       //処理モード取得
                cnt.num     = 0;       //処理モード取得
                cnt.max     = 0;       //処理モード取得
                break;
                //オプション設定
            case MENU_OPTION:
                mode_op = selectdriving_option(mode_op);    //実行するメニューを選択
                //mode_op = selectMenu_option(mode_op);    //実行するメニューを選択
                switch(mode_op) {
                    case OP_2ND:
                        ch_back = set_basic_setting();
                        break;
                    case OP_ZENNOSE:
                    case OP_ATONOSE:
                        ch_back = set_option_setting(mode_op);
                        break;
                    case OP_STOP_CELLS:
                        ch_back = set_stop_cells();
                        break;
                    case OP_USB_EN:
                    case OP_CHAR_ROTATE:
                    case OP_RANK_OVER:
                    case OP_KOKUTI:
                    case OP_KIKAKUGAI_V:
                    case OP_RANKSPEED:
                    case OP_BTN_SETTING:
                    case OP_BEGINNER:
                    case OP_SEL_BEEP:
                    case OP_NO_COMBI_V:
                    case OP_SECONDMBI_V:
                    case OP_NUMBER_COLOR:
                    case OP_O_count_lim:
                    case OP_O_lit_count_lim:
                    case OP_NAGAMONO_HOSEI:
                    case OP_WAITING_LIMMIT:
                    case OP_SELAUTOZERO:
                    case OP_KEKKA_HYOUJI:
                    case OP_ALART_AUTOZERO:
                    case OP_FIX_VALUE:
                        ch_back = set_option_setting(mode_op);
                        break;
                    case OP_NAGAMONO_HOSEI2:
                        ch_back = hosei_for_preset(file.get_preset());
                        //@;:8ch_back = set_nagamono_hosei();
                        //ch_back = file.get_preset(file.get_preset());
                        break;
                    case OP_BUFFER:
                        set_sercret_parameter_one(P_buffer,1); //保存指令付
                        ch_back = true;
                        break;
                    case OP_TIMER: //時計設定
                        time_set();
                        ch_back = true;
                        break;
                    case OP_SERVO1 : //
                        ch_back = servo_setting1();
                        break;
                    case OP_SERVO2 : //
                        ch_back = servo_setting2();
                        break;
                    case OP_SERVO3 : //
                        ch_back = servo_setting3();
                        break;
                    case OP_SERVO4 : //
                        ch_back = servo_setting4();
                        break;
                    case OP_ZERO_CHECK:
                        ch_back = zerocheck_set();
                    case 99:
                        ch_back = true;
                        break;
                    default:
                        ch_back = false;
                        break;
                }
                if(ch_back == false) {
                    mode = MENU_OPTION;
                    ch_back = true;
                } else {
                    mode = MENU;
                    mode_op=0;
                }
                break;
                //使用しないセルを選択
                /*case MENU_STOP_CELLS:
                    set_stop_cells();
                    mode = MENU;
                    break;*/
                //トレーニング
            /*case MENU_TRAINIG:
                // 乱数系列の変更 トレーニングモードで乱数使用
                srand(t.read_ms());
                selectTrainingMode();
                mode=MENU;
                break;
*/
                //スパン調整
            case MENU_SPAN:
                span_mode_f = 1;
                set_span();
                span_mode_f = 0;
                mode = MENU;
                break;
            case MENU_KEIRYO:
            
                katamen=0; //グローバル変数へ値保存
                load.katamen = 0;
                set_stopcell();   //不使用セル(＋12Headの対面点灯を組み合わせる)
                //LED点灯箇所のみ重量を計る
                start_keiryo(0);
                mode = MENU;
                break;
            case MENU_KEIRYO2:
                katamen=0; //グローバル変数へ値保存
                load.katamen = 0;
                set_stopcell();   //不使用セル(＋12Headの対面点灯を組み合わせる)
                //LED点灯箇所のみ重量を計る
                start_keiryo2(0);
                mode = MENU;
                break;
                
            case MENU_KEIRYO3:
                set_stopcell();   //不使用セル(＋12Headの対面点灯を組み合わせる)
                auto_zero_point();                  //ゼロ点取得
                auto_zero_go = false;
                Drive_keiryo();
                mode = MENU;
                break;
            case MENU_PASSWORD:
                password();
                mode = MENU;
                break;
            case MENU_DEMO:
                demo_mode();
                v.wait_echo2();
                mode = MENU;
                //BARATSUKIを変えてるので、読み込み直す
                file.read_param();
                param = file.get_param();           //パラメータ(設定情報)を取得
                break;
        }//switch
        //if(gLevel==1){
        //    file.set_preset(0);
        //    file.save_preset();
        //}
        btn.end_force();
        while(btn.any_press() == true){btn.end_force();wait(0.2);}
        btn.end_force();
        
        btn.old_btn();               //スイッチ更新
        btn.new_btn();               //スイッチ更新
    }//while(無限ループ)
}//main


//起動
void starting()
{
    lcd.cls();
    led.ledAllOff();        //LED消灯

    lcd.messege(TOP, DIS_TABLE, CLS);   //ﾃｰﾌﾞﾙｺﾝﾋﾞ
    lcd.locate(0,1);
    lcd.printf("ver.");  //ﾊﾞｰｼﾞｮﾝ表示
    lcd.printf(VERSION_MODEL);  //ﾊﾞｰｼﾞｮﾝ表示(機種)
    lcd.printf("%03.02f",VERSION);  //ﾊﾞｰｼﾞｮﾝ表示
    lcd.printf(VERSION_C);  //ﾊﾞｰｼﾞｮﾝ表示(特殊仕様)
    se.openning();                      //オープニング音
    //wait(1);
}
void rap_count(){
    coulap+=1;
    if(coulap >10000){
        coulap=10000;
    }
    
    no_combi_rap += 1;
    if(no_combi_rap >100000){
        no_combi_rap=100000;
    }
    
    no_zero_counter_zero += 1;
    if(no_zero_counter_zero >= 10){
        no_zero_counter_zero = 0;
    }

    for(int i=0;i<14;i++){
        no_zero_counter[i] += 1;
        if(no_zero_counter[i] > 18000){
            no_zero_counter[i] = 18000;
        }
    }
    
}

void measure_1head(){
    if(now_TC_mode==1){
        printf("mesure%2dh\r\n",next_meas_head);
        setHeadVolt(next_meas_head, KURI2nd);
        next_meas_head++;
        if(next_meas_head >= product.head){
            next_meas_head = 0;
        }
    }
}


void setUp()
{   const char name[2][SIZE]={{0xB4,0xD7,0xB0,0x21,0x21,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｴﾗｰ!!
                             ,{0xC3,0xDE,0xB0,0xC0,0xB6,0xDE,' ' ,0xB7,0xB4,0xCF,0xBC,0xC0,0x00}};      //ﾃﾞｰﾀｶﾞ ｷｴﾏｼﾀ
    bool ng_flg=false; //デフォルト起動できないファイルが消された
    file.read_all_file();               //FileRead
    param = file.get_param();           //パラメータ(設定情報)を取得
    product = file.get_product();
    printf("%2d %2d %4d\r\n",product.productType,product.head,param.hundo);
    cel.set_product(file.get_product());
    option = file.get_option();
    file.read_stop_cells();             //未使用セル設定
    if(product.head>3) {
        led.set(product.head);                //ヘッド数を設定
    } else {
        led.set(4);                //ヘッド数を設定
    }
    
    cel.set_num_head(product.head);
    cel.make_new_pointer();

    bscData = file.get_basic();         //オプション設定を取得
    cel.set_param(file.get_param());    //パラメータをロードセルクラスへ
    oya_hundo = param.hundo;
    cel.set_span(file.get_span());      //スパン値を取得
    cel.set_option(file.get_option());
    //printf("%d  %d  %d \r\n",option.reck_zen,option.reck_ato,option.usb_EN);
    file.read_hservo();
            no_zero_counter[i] = 18000;
    if(file_lost_sig>0){
        lcd.locate(0,0);
        lcd.printf(name[0]);
        lcd.locate(11,0);
        lcd.printf("%5d",file_lost_sig);
        lcd.locate(0,1);
        lcd.printf(name[1]);
        printf("%d\r\n",file_lost_sig);
        lcd.locate(0,0);
        for (int file_ck_flg = 0;file_ck_flg<R_END;file_ck_flg++){
            if ((file_lost_sig & (1<<file_ck_flg))>0){
                /*switch(file_ck_flg){
                    case 0:     printf("param\r\n");break;
                    case 1:     printf("preset\r\n");break;
                    case 2:     printf("span\r\n");break;
                    case 3:     printf("loadcell\r\n");break;
                    case 4:     printf("basic_setting\r\n");break;
                    case 5:     printf("SData\r\n");break;
                    case 6:     printf("unitweight\r\n");break;
                    case 7:     printf("name\r\n");break;
                    case 8:     printf("option\r\n");break;
                    case 9:     printf("limit1\r\n");break;
                    case 10:     printf("limit2\r\n");break;
                    case 11:     printf("read_data\r\n");break;
                    case 12`P{+~?><M}:     printf("time\r\n");break;
                }*/
                if(gLevel>0||
                    file_ck_flg==R_PRE    ||
                    file_ck_flg==R_BASIC  ||
                    file_ck_flg==R_OPTION ||
                    file_ck_flg==R_NAME   ||
                    file_ck_flg==R_GENK   ||
                    file_ck_flg==R_GIRI   ||
                    file_ck_flg==R_TIME){
                    v.output_losfile(file_ck_flg+1);
                    wait(4);
                    if( file_ck_flg==R_PRE    ||
                        //file_ck_flg==R_SPAN   ||
                        file_ck_flg==R_BASIC  ||
                        file_ck_flg==R_OPTION ||
                        file_ck_flg==R_NAME   ||
                        file_ck_flg==R_GENK   ||
                        file_ck_flg==R_GIRI   ||
                        file_ck_flg==R_TIME
                        ){
                        v.output_defoult();
                        wait(4);
                    }else if(file_ck_flg==R_SPAN){
                        v.output_retastar_span();
                        wait(4);
                    }
                }
                if( file_ck_flg==R_PRE    ||
                    //file_ck_flg==R_SPAN   ||
                    file_ck_flg==R_BASIC  ||
                    file_ck_flg==R_OPTION ||
                    file_ck_flg==R_NAME   ||
                    file_ck_flg==R_GENK   ||
                    file_ck_flg==R_GIRI   ||
                    file_ck_flg==R_TIME
                    ){
                    //v.output_defoult();
                }else if(file_ck_flg==R_SPAN){
                    //v.output_retastar_span();
                }else{
                    ng_flg=true;
                }
                //printf("%3d  A\r\n",file_ck_flg);
                printf("%3d  B\r\n",file_ck_flg);
                file.error_log(file_ck_flg);
            }
            if(file_ck_flg==R_END-1){
                file.error_log(0);
            }
        }
        if(gLevel==0){
        //if(gLevel!=0){
            v.output_losfile(0);
        }
        if(gLevel==0 && ng_flg==true){
        //if(gLevel!=0){
            exit(EXIT_FAILURE);
        }
    }

    print_body_param(); 
    //printf("%d\r\n",matrix_mode);
    // ******************************************
    set_OOHEAD();
    
    //通常のテーブルコンビの設定
    //gOyaHead = 0x820;         //親2箇所
    //gKoHead = 0x7df;
    gStartKinjiti = 0;  //近似値処理を行うまでに、何回通常の組み合わせ計算を行うかを設定(元々は1)
    gNowHeadBit = ALL_HEAD;

    //1列版のテーブルコンビだったらそれに応じた設定に変更
    //if(product.head <= ONE_COLUMN_HEAD) {
    if(product.head <= 7) {
        //gOyaHead = 0x20;          //親1箇所
        //gKoHead  = 0x1f;
        gStartKinjiti = 0;  //近似値処理を行うまでに、何回通常の組み合わせ計算を行うかを設定(元々は2)
        //gNowHeadBit = LOW_HEAD;
    } else if(product.head <= ONE_COLUMN_HEAD) {//８ﾍｯﾄﾞの場合。
        LOW_HEAD = LOW_HEAD2;
        //gOyaHead = 0x80;          //親1箇所
        //gKoHead  = 0x7f;
        gStartKinjiti = 0;  //近似値処理を行うまでに、何回通常の組み合わせ計算を行うかを設定(元々は2)
        //gNowHeadBit = LOW_HEAD;
    }
    //printf("%5d\r\n",gOyaHead);
    // ******************************************

    //引き継ぎ優先のためゼロ確認処理をコメントアウトし
    //ゼロ点エラー処理は昔の状態へ変更2017-03-27
    //ゼロ確認処理をする場合には、下記のコメントを解除する
    /*

    //下記は定数。コメントアウト箇所を使用する場合には必要となる
    //TC(テーブルコンビ)のゼロ点確認時の計量(g)範囲
    const int TC_MAX_WEIGHT = 400;      //通常
    const int TC_MIN_WEIGHT = 40;
    const int JR_MAX_WEIGHT = 100;      //ジュニア
    const int JR_MIN_WEIGHT = 10;

    switch (product.productType){
        //通常のテーブルコンビの設定を行う
        case STANDARD:
            gZeroMaxGram = TC_MAX_WEIGHT;   //ゼロ確認時の計量可能な重量を指定
            gZeroMinGram = TC_MIN_WEIGHT;
            break;
        //ジュニアの場合の設定を行う
        case JUNIOR:
            gZeroMaxGram = JR_MAX_WEIGHT;   //ゼロ確認時の計量可能な重量を指定
            gZeroMinGram = JR_MIN_WEIGHT;
            break;
    }
    */
    // ******************************************
}
void print_body_param(){
    para_head  = product.head;
    para_array = product.Array;
    lcd.locate(10,0);
    lcd.printf("%02d",product.head);  //ヘッド数表示
    if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
        matrix_mode = 1;
        lcd.printf("S");
    } else {
        matrix_mode = 0;
        flipper.detach();
        lcd.printf("L");
    }
    if(product.Array==ARRAY_DUAL){          //セル配置表示
        lcd.printf("D");
    }else if(product.Array==ARRAY_LINEAR){ 
        lcd.printf("L");
    }else if(product.Array==ARRAY_WIDE){
        lcd.printf("W");
    }else if(product.Array==ARRAY_OTHER){
        lcd.printf("O");
    }else{
        lcd.printf("-");
    } 
    if(product.limit_on==LIMIT_OFF){          //限定内容表示
        lcd.printf("N");
    }else if(product.limit_on==NUMBER_LIMIT){
        lcd.printf("C");
    }else if(product.limit_on==SELECTOR_LIMIT){
        lcd.printf("S");
    }else if(product.limit_on==WIDE_LIMIT){
        lcd.printf("W");
    }else if(product.limit_on==NIRA_LIMIT){
        lcd.printf("I");
    }else{
        lcd.printf("-");
    }
    if(product.Rating==RATING_TWO){          //セル重量法事
        lcd.printf("T");
    }else if(product.Rating==RATING_BIG){
        lcd.printf("B");
    }else if(product.Rating==RATING_JUNIOR){ 
        lcd.printf("J");
    }else if(product.Rating==RATING_XL){
        lcd.printf("X");
    }else{
        lcd.printf("-");
    } 
}
void rtc_start()
{
     /*   time_t seconds = time(NULL);
        if ( seconds == -1 ){  // <1>
            date.tm_sec = 00;    // 0-59
            date.tm_min = 00;    // 0-59
            date.tm_hour = 10;   // 0-23
            date.tm_mday = 6;   // 1-31
            date.tm_mon = 5;     // 0-11
            date.tm_year = 117;  // year since 1900
            seconds = mktime(&date);
            set_time(seconds);
        }*/
    seconds = time(NULL);
    //printf("A00Time as a string = %s\r\n", ctime(&seconds));
    //再起動時はdateの値が消えているので登録
    tm *dated = localtime(&seconds);
    date.tm_sec  = dated->tm_sec;
    date.tm_min  = dated->tm_min;
    date.tm_hour = dated->tm_hour;
    date.tm_mday = dated->tm_mday;
    date.tm_mon  = dated->tm_mon;
    date.tm_year = dated->tm_year;
    //printf("Time as a string = %04d/%02d/%02d  %02d:%02d:%02d   %d\r\n",
    //    date.tm_year + 1900,date.tm_mon + 1,date.tm_mday,
    //    date.tm_hour,date.tm_min,date.tm_sec,time_reset);
        
    
    file.read_time();
    //起動時間もここで登録
    seconds = time(NULL);
    tm *dated2 = localtime(&seconds);
    w_date.tm_year = dated2->tm_year;
    w_date.tm_sec  = dated2->tm_sec;
    w_date.tm_min  = dated2->tm_min;
    w_date.tm_hour = dated2->tm_hour;
    w_date.tm_mday = dated2->tm_mday;
    w_date.tm_mon  = dated2->tm_mon;
    w_date.tm_year = dated2->tm_year;
    //printf("Time as a string = %04d/%02d/%02d  %02d:%02d:%02d   %d\r\n",
    //   w_date.tm_year + 1900,w_date.tm_mon + 1,w_date.tm_mday,
    //    w_date.tm_hour,w_date.tm_min,w_date.tm_sec,time_reset);
    // pc.printf("A01Time as a string = %s\r\n", ctime(&seconds));
    //Read RTC Data
    //for(int i=0;i<10;i++){
    //seconds = time(NULL);
    //printf("Time as a string = %s", ctime(&seconds));
    //wait(1);
    //}
}
void time_set(){
    data_t limitData;
    seconds = time(NULL);
    tm *datedset = localtime(&seconds);
    lcd.blank(0);
    lcd.blank(1);
    lcd.locate(0,0);
    lcd.printf("% 04d/ %02d/ %02d",
        datedset->tm_year + 1900,datedset->tm_mon + 1,datedset->tm_mday);
    lcd.locate(1,1);
    lcd.printf(" %02d: %02d",
        datedset->tm_hour,datedset->tm_min);
        
       
    int data[]={ datedset->tm_year+1900,datedset->tm_mon + 1 ,datedset->tm_mday,datedset->tm_hour,datedset->tm_min};
        
        
    for(int i=0;i<5;i++){
        switch(i){
            case 0: //year
                limitData.min = 1900;
                limitData.max = 3000;
                limitData.lcd = 1;
                break;
            case 1: //mon
                limitData.min = 1;
                limitData.max = 12;
                limitData.lcd = 7;
                break;
            case 2: //day
                limitData.min = 1;
                limitData.max = 31;
                limitData.lcd = 11;
                break;
            case 3: //hour
                limitData.min = 0;
                limitData.max = 23;
                limitData.lcd = 102;
                break;
            case 4: //min
                limitData.min = 0;
                limitData.max = 59;
                limitData.lcd = 106;
                break;
        }
        Chenge_vaule_time(&data[i], limitData);
        
        
        if(btn.get_btn() == BTN_ESC) {
            i-=2;
            if(i==-2){
                i=99;   
            }
        }
        if(i==4){
            time_reset = false;
            date.tm_sec  = 0;    // 0-59
            date.tm_min  = data[4];    // 0-59
            date.tm_hour = data[3];   // 0-23
            date.tm_mday = data[2];   // 1-31
            date.tm_mon  = data[1]-1;     // 0-11
            date.tm_year = data[0]-1900;  // year since 1900
            seconds = mktime(&date);
            set_time(seconds);
            file.save_time();
        }
    }
    
    if(1/*gLevel == 0*/){
        lcd.blank(0);
        lcd.blank(1);
        for(int i=0;i<50;i++){
            seconds = time(NULL);
            tm *datedset = localtime(&seconds);
            lcd.locate(0,0);
            lcd.printf("% 04d/ %02d/ %02d",
                datedset->tm_year + 1900,datedset->tm_mon + 1,datedset->tm_mday);
            lcd.locate(1,1);
            lcd.printf(" %02d: %02d: %02d",
                datedset->tm_hour,datedset->tm_min,datedset->tm_sec);
            wait(0.1);
        }
    }
    
}


//自動的にゼロ点を取得
void auto_zero_point()
{
    led.ledAllOff();        //LED消灯
    lcd.messege(TOP,DIS_WAIT,CLS);
    lcd.messege(LOW,DIS_ZERO);
    cel.setIniHWeight();    //風袋込重量を強制取得
    forced_zero = true;
    set_zero_span();        //ゼロ点確認、取得
    forced_zero = false;
    se.OK();                //オートゼロ終了の合図
    close_wait_flg  =0;
    close_wait_flg2 =0;
}





bool kumi_en_check()
{
    if(KUMI_limit-KUMI_now>0) {
        return true;
    } else {
        return false;
    }

}
//全皿のビットや片面等、成立させるための判断基準を、headやArrayで判断させる
//set_upで一度使用するのみ。
void set_OOHEAD(){
    int bit_led=0x00;
    for(int i=0;i<(product.head/2);i++){
        if(i>0){
            bit_led=bit_led<<1;
        }
        bit_led=bit_led | 1;
    }
    LOW_HEAD = bit_led;
    UP_HEAD = bit_led<<(product.head/2);
    bit_led=0x00;
    for(int i=0;i<(product.head/4+((product.head/2)%2));i++){
        if(i>0){
            bit_led=bit_led<<1;
        }
        bit_led=bit_led | 1;
    }
    TOP1_HEAD=bit_led|bit_led<<product.head/2;
    bit_led=bit_led<<1;
    TOP2_HEAD=bit_led|bit_led<<product.head/2;
    bit_led=bit_led<<1;
    BOT1_HEAD=bit_led|bit_led<<product.head/2;
    if((product.head/2)<6){
        BOT2_HEAD = BOT1_HEAD;
    }else{
        bit_led=bit_led<<1;
        BOT1_HEAD=bit_led|bit_led<<product.head/2;
    }
    
    bit_led=0x00;
    for(int i=0;i<(product.head);i++){
        if(i>0){
            bit_led=bit_led<<1;
        }
        bit_led=bit_led | 1;
        
    }
    ALL_HEAD = bit_led;
        
    bit_led=0x00;
    bit_led=bit_led | 1;
    if (product.Array==0){
        bit_led =bit_led<<(product.head/2-1);
        gOyaHead = bit_led|bit_led<<product.head/2;
    }else{
        gOyaHead = bit_led<<(product.head-1);
    }
    
    gKoHead  = ALL_HEAD & (ALL_HEAD ^ gOyaHead);
    gNowHeadBit  = ALL_HEAD;
    
    //printf("LOW_HEAD  =%3d\r\n",LOW_HEAD);
    //printf("UP_HEAD   =%3d\r\n",UP_HEAD);
    //printf("TOP1_HEAD =%3d\r\n",TOP1_HEAD);
    //printf("TOP2_HEAD =%3d\r\n",TOP2_HEAD);
    //printf("BOT1_HEAD =%3d\r\n",BOT1_HEAD);
    //printf("BOT2_HEAD =%3d\r\n",BOT2_HEAD);
    //printf("ALL_HEAD  =%3d\r\n",ALL_HEAD);
    //printf("gOyaHead  =%3d\r\n",gOyaHead);
    //printf("gKoHead   =%3d\r\n",gKoHead);
}
void set_stopcell()    //不使用セル(＋12Headの対面点灯を組み合わせる)
{

    //設定に伴う不使用セルの設定
    if(load.katamen==1) {
        stop_cells = stop_cells2 | UP_HEAD ;
        //stop_cellsd= (stop_cells2 << 6) | LOW_HEAD;
        stop_cellsd= stop_cells2 | UP_HEAD;
    } else if(load.katamen==2) {
        stop_cells = stop_cells2 | LOW_HEAD ;
        //stop_cellsd= (stop_cells2 >> 6) | UP_HEAD;
        stop_cellsd= stop_cells2 | LOW_HEAD;
    } else {
        stop_cells = stop_cells2;
        stop_cellsd= stop_cells2;
    }
}
//組み合わせ処理開始
void driveStart()
{
    char multiweight[SIZE] = {0xD7,0xDD,0xB8,0xCD,0xDE,0xC2,' ' ,0xB8,0xD0,0xB1,0xDC,0xBE,0x00};
    char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
                   ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼﾀ
    int secondCnt = 0;                        //一定数に達したら準最適値処理開始
    int *stay_cnt = new int[product.head];      //滞留警告判断のためのカウント変数
    //float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    //cntData_t cnt= {0,0,0};                     //カウント機能の値保持用
    combiData_t orderCombi[BK_CNT] = {0,0.0,0};   //組み合わせ候補のデータ(配列)
    combiData_t resultCombi;                    //組み合わせ結果(最小値取得用)
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    cnt.nowMode = load.spMode;       //処理モード取得
    cnt.num     = 0;                 //処理モード取得
    cnt.max     = 0;                 //処理モード取得
    cnt_miss_and_kikakugai=0;      //取り間違いと規格外が発生した回数
    cnt_junsaiteki=0;               //準最適値が発生した回数
    int btnset =0;//計量中の下ボタン押しで出てくる値保存
    int SerialNumber = 0; //USBを連続で使用する際の連番値
    int first_target = load.target;
    int first_jougen = load.jougen;
    int scond_target = load.taimenTarget;
    int scond_jougen = load.taimenJougen;
    
    befoure_check = 0;      //一個前に確認した、排出待ちのビット
    now_____check = 0;      //一個前に確認した、排出待ちのビット
    check_same_count = 0;      //排出待ちのビットが同一だった時のカウント
    check_same_max = 0;

    file.read_hosei(file.get_preset());
    no_combi_v_flg = false;
    set_stopcell();
    if(servo_connection==true && load.spMode == SPEED_DAMP){
        send_i(EJECT_RESET, 11+load.kumi_flag, 0);
    }
    //1行目LCD表示
    if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
        lcd.locate(0,0);
        lcd.printf(multiweight);
        lcd.blank(LOW);
        lcd.printf(":----.-g       ");
    } else {
        lcd.settings(&load);      //設定内容を表示
    }
    if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
        for(int l=0;l<product.head;l++){
            iro[l]=C_NUSE;
        }
    }

    //作業開始時間記録
    seconds = time(NULL);
    tm *dated3 = localtime(&seconds);
    run_date.tm_year = dated3->tm_year+1900;
    run_date.tm_sec  = dated3->tm_sec;
    run_date.tm_min  = dated3->tm_min;
    run_date.tm_hour = dated3->tm_hour;
    run_date.tm_mday = dated3->tm_mday;
    run_date.tm_mon  = dated3->tm_mon+1;
    run_date.tm_year = dated3->tm_year+1900;
    //
    //printf("st%d:%d:%d\r\n",run_date.tm_hour,run_date.tm_min,run_date.tm_sec);

    initial_drive_function();
    /*for(int l; l<product.head; l++) {
        nowweightVale3[l]=0.0;
        servo_b_histry[l]   =0;
        servo_n_histry[0][l]=0;
        servo_n_histry[1][l]=0;
    }*/
    for(int i = 0; i < product.head; i++) {
        no_zero_counter[i] = 0;
    }
    close_wait_flg = 0;

    if(product.use_usb==1 && option.usb_EN==true){
        //SerialNumber = USB_filecheck();
        SerialNumber = USB_filecheck2(run_date.tm_year,run_date.tm_mon,run_date.tm_mday,run_date.tm_hour,run_date.tm_min);
        if(SerialNumber<999){
            /*lcd.blank(0);
            lcd.blank(1);
            lcd.locate(0,1);
            lcd.printf(SAVEEROK[1]);
            se.OK();    //警告音
            //wait(3);
            lcd.blank(0);
            lcd.blank(1);*/
        }
    }

    //準最適値を行うヘッド数は、HEAD数より
    //大きい値はありえない(エラー対策)
    if(bscData.startKinji > product.head) {
        bscData.startKinji = product.head;
    }
    tm *dated4 = localtime(&seconds);
    if(product.limit_on==NUMBER_LIMIT) {
        set_count_force(&cnt);
    }
    meas_rap.attach( &rap_count, 0.1);
    //meas_rap2.attach( &measure_1head, 0.15);
    //testmode3=true;
    while(1) {
        //se.Touch_01();   //ボタン操作時の効果音
        //se.Touch_01();   //ボタン操作時の効果音
        if(testmode3==true){se.puter(6);}
        //se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();
        
            //printf("A:moving1 %5d  || moving2 %5d\r\n",moving1,moving2);
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
        } else {
            led.ledAllOff();            // LED消灯
            if(moving1 == 0){
                led.ledAllOff();            // LED消灯
            }else{
                led.REGIS(moving1);
            }
        }
        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            for(int l=0;l<product.head;l++){
                if(iro[l] == C_R_EXCLA){

                }else if( (moving1 & (1<<l))>0 && load.spMode == SPEED_DAMP){
                    //if(gLevel == 1){
                    //    iro[l] = CG_01 + cel.anteiWeight(l) + hosei[l]-1;
                    //}else{
                        iro[l] = C_BUI_Y;
                    //}
                }else {
                    if(cel.anteiWeight(l) > 0){
                    //if(antei_flug[l]==1){
                        //if((int(cel.anteiWeight(l)) + int(hosei[l])-1) < 300){
                        if((int(cel.anteiWeight(l) + hosei_f[l])-1) < 300){
                            //iro[l] = CR_01 + (int(cel.anteiWeight(l) + hosei_f[l])-1);
                            iro[l] = CR_00 + (int(cel.anteiWeight(l) + op_n_hosei + hosei_f[l]));
                            //iro[l] = CR_00 + (int(cel.anteiWeight(l)));
                        }else{
                            iro[l] = C_NUSE_2D;
                        }
                        //if(gLevel == 1){
                            //iro[l] = CR_01 + (int(cel.anteiWeight(l)) + int(hosei[l])-1) % 100;
                        //}else{
                        //    iro[l] = C_NUSE_1D;
                        //}
                    }else{
                        iro[l] = C_NUSE;
                    }
                }
                if(iro[l]==C_NULL){
                    if( (moving1 & (1<<l))>0 && load.spMode == SPEED_DAMP){
                        //if(gLevel == 1){
                        //    iro[l] = CG_01 + cel.anteiWeight(l) + hosei[l]-1;
                        //}else{
                            iro[l] = C_BUI_Y;
                        //}
                    }else {
                        iro[l] = C_NULL;
                    }
                }
            }
        }else{
            if(moving1 == 0){
                led.ledAllOff();            // LED消灯
            }else{
                led.REGIS(moving1);
            }
        }
        btn.new_keep_btn();         //スイッチ(New)取得
        if(product.limit_on==NUMBER_LIMIT) {
            if(kumi_en_check()==false) {
                se.mistake();                   //警告音
                v.over_limit();
                btn.force_ESC();
            } else {
                /*lcd.cls();
                lcd.locate(0,1);
                lcd.printf("%d",KUMI_limit-KUMI_now);
                v.last_limit(KUMI_limit-KUMI_now);
                KUMI_now =KUMI_now +1;
                file.save_limit2(KUMI_now);*/
            }
        }
        if(option.waiting_limit > 0 && no_combi_rap > (option.waiting_limit * 60 * 10)){
            se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();
            btn.force_ESC();
            no_combi_rap = 0;
        }
        /*    //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        if(btn.up_press() == true) {
            se.chime_01();
            while(btn.any_press()==true){
                if(demo_flg==true&&btn.down_press() == true&&file.get_preset()==product.head*2){
                    demo_continue=2;
        printf("a%d  %d\r\n",file.get_preset(),demo_continue);
                    btn.force_ESC();
                    continue;
                }
                wait(0.1);
            }
            if(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2){
                    demo_continue=2;
        printf("b%d  %d\r\n",file.get_preset(),demo_continue);
                    btn.force_ESC();
                    continue;
            }
        printf("c%d  %d\r\n",file.get_preset(),demo_continue);
            //ゆっくりモード時には、
            if(load.spMode == SPEED_LOW) {
                btnset = Chenge_vaule_btnD(option.btn_sett);//ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
                if(btnset==0) {
                    set_count(&cnt);
                } else if(btnset==1) {
                    auto_zero_off = true;
                    start_keiryo(0);
                    auto_zero_off = false;
                }else if(btnset==2){
                    set_preset2(file.get_preset(),TL_SPMODE);
                }else if(btnset==3){
                    set_preset2(file.get_preset(),TL_KUMI_FLAG);
                }else if(btnset==4){
                    set_option_setting(OP_BEGINNER);
                }else if(btnset==5){
                    set_option_setting(OP_KIKAKUGAI_V);
                }else if(btnset==6){
                    set_option_setting(OP_KOKUTI);
                }else if(btnset==7){
                    set_option_setting(OP_SEL_BEEP);
                }else if(btnset==8){
                    set_sercret_parameter_one(P_buffer,1); //保存指令付
                }
            } else if(load.spMode == SPEED_HIGH || load.spMode == SPEED_HYPER) {
                btnset = Chenge_vaule_btnD(0);
                if(btnset==0) {
                    set_preset2(file.get_preset(),TL_HYOUJI);
                } else if(btnset==1) {
                    auto_zero_off = true;
                    start_keiryo(0);
                    auto_zero_off = false;
                }else if(btnset==2){
                    set_preset2(file.get_preset(),TL_SPMODE);
                }else if(btnset==3){
                    set_preset2(file.get_preset(),TL_KUMI_FLAG);
                }else if(btnset==4){
                    set_sercret_parameter_one(P_buffer,1); //保存指令付
                }
            }
            //1行目LCD表示
            if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                lcd.locate(0,0);
                lcd.printf(multiweight);
                lcd.blank(LOW);
                lcd.printf(":----.-g");
            } else {
                lcd.settings(&load);      //設定内容を表示
            }
        }
        printf("%d  %d\r\n",file.get_preset(),demo_continue);*/
        //esc_swを押すと、Menu画面に戻る
        if(btn.esc_press()||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)) {
            meas_rap.detach();
            meas_rap2.detach();
            btn.end_force();
            se.Touch_01();
            //total_kumi_cnt=10;
            //if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT || load.kumi_flag == KUMI_COMBI )&& total_kumi_cnt>0) {
            
            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                for(int l=0;l<product.head;l++){
                    iro[l]=C_NULL;
                }
            }
            if(demo_flg==false){
                if(servo_connection==true && load.spMode == SPEED_DAMP){
                    se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();
                    for(int header=0; header<product.head; header++){
                        send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
                    }
                    wait(0.5);
                    for(int header=0; header<product.head; header++){
                        send_i(SERVO_1 ,header,servo_span[header][0]); wait_res();
                    }

                }
            }
            if(servo_connection==true && load.spMode == SPEED_DAMP){
                send_i(EJECT_RESET, 0, 0);
                wait_signal();
            }
            if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT)&& total_kumi_cnt>0) {
                seconds = time(NULL);
                tm *dated4 = localtime(&seconds);
                //printf("en%d:%d:%d\r\n",dated4->tm_hour,dated4->tm_min,dated4->tm_sec);
                int ji=0;
                int hun=0;
                ji = dated4->tm_hour - run_date.tm_hour;
                if(ji<0) {
                    ji += 24;
                }
                hun = dated4->tm_min - run_date.tm_min;
                if(hun<0) {
                    hun += 60;
                    ji -= 1;
                }
                lcd.blank(1);
                lcd.blank(0);
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                if(product.use_usb==1 && option.usb_EN==true) {
                    lcd.printf(saving[0]);
                }
                if(product.use_usb==1 && option.usb_EN==true) {
                    file.save_SAGYOU(ji,hun,file.get_preset()+1,load.target,total_kumi_cnt,average_kumi_value);
                }        
                    
                    
                
                printf("total:%4d\r",total_kumi_cnt);
                printf("ave  :%6.1f\r",average_kumi_value);
                printf("time:%02dh%02dm\r",ji,hun);
                if((ji*60)+hun==0){
                    printf("1min:%03d\r",total_kumi_cnt);
                }else{
                    printf("1min:%03d\r",int(total_kumi_cnt/(ji*60)+hun));
                }
                printf("err :%4d(%4.1f%%)\r",cnt_miss_and_kikakugai,float(cnt_miss_and_kikakugai)/float(total_kumi_cnt));
                printf("jst :%4d(%4.1f%%)\r",cnt_junsaiteki,float(cnt_junsaiteki)/float(total_kumi_cnt));
                printf("\r\n");
                //printf("err :%4d%s%04.1f%s%s\r",cnt_miss_and_kikakugai,0x28,float(cnt_miss_and_kikakugai)/float(total_kumi_cnt),0x25,0x29);
                //printf("jst :%4d%s%04.1f%s%s\r",cnt_junsaiteki,0x28,float(cnt_junsaiteki)/float(total_kumi_cnt),0x25,0x29);
                int num=0;
                printf("%02dH:%5d:%6d\r",num*2+1,servo_n_histry[0][num*2  ],servo_b_histry[num*2  ] + servo_n_histry[0][num*2  ]);
                printf("%02dH:%5d:%6d\r",num*2+2,servo_n_histry[0][num*2+1],servo_b_histry[num*2+1] + servo_n_histry[0][num*2+1]);
                num=1;
                printf("%02dH:%5d:%6d\r",num*2+1,servo_n_histry[0][num*2  ],servo_b_histry[num*2  ] + servo_n_histry[0][num*2  ]);
                printf("%02dH:%5d:%6d\r",num*2+2,servo_n_histry[0][num*2+1],servo_b_histry[num*2+1] + servo_n_histry[0][num*2+1]);
                num=2;
                printf("%02dH:%5d:%6d\r",num*2+1,servo_n_histry[0][num*2  ],servo_b_histry[num*2  ] + servo_n_histry[0][num*2  ]);
                printf("%02dH:%5d:%6d\r",num*2+2,servo_n_histry[0][num*2+1],servo_b_histry[num*2+1] + servo_n_histry[0][num*2+1]);
                
                for(int a=0 ; a < product.head ; a++){                                
                    if( servo_n_histry[1][a] == 1){
                        servo_n_histry[0][a] += 1;
                        servo_n_histry[1][a] =  0;
                    }
                }
                servo_histry += total_kumi_cnt;
                
                file.save_hservo();
                if(option.kekka_hyouji == 1){
                    result_LCD(ji,hun);
                    result_LCD_servo(ji,hun);
                }
                
                
                for(int l=0; l<product.head; l++) {
                    servo_b_histry[l] = servo_b_histry[l] + servo_n_histry[0][l] + servo_n_histry[1][l];
                    servo_n_histry[0][l] = 0;
                    servo_n_histry[1][l] = 0;
                }
                servo_h_time[1] += hun;
                
                if(servo_h_time[1]>=60){
                    servo_h_time[1] -= 60;
                    servo_h_time[0] +=  1;
                }
                servo_h_time[0] += ji;
                
                
                lcd.blank(1);
                lcd.blank(0);
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.printf(saving[1]);
                //file.save_hservo();
                
                //save_sagyou_file(total_kumi_cnt,average_kumi_value,load.target,load.jougen);
                if(product.use_usb==1 && option.usb_EN==true) {
                    //save_sagyou_file2(total_kumi_cnt,average_kumi_value,load.target,load.jougen,ji,hun,SerialNumber);
                    save_sagyou_file2_date(total_kumi_cnt,average_kumi_value,load.target,load.jougen,ji,hun,SerialNumber,run_date.tm_year,run_date.tm_mon,run_date.tm_mday,run_date.tm_hour,run_date.tm_min);
                }
                //save_sagyou_file3(total_kumi_cnt,average_kumi_value,load.target,load.jougen,ji,hun,SerialNumber);
                if(product.use_usb==1 && option.usb_EN==true) {
                    SD_initialise();
                }
                if(product.use_usb==1 && option.usb_EN==true) {
                    lcd.blank(1);
                    lcd.blank(0);
                    lcd.locate(0,1);
                    lcd.printf("                ");
                    lcd.printf(saving[1]);
                }
            }
            delete[] stay_cnt;
            total_kumi_cnt = 0;
            average_kumi_value = 0;
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            wait(0.2);
            if(servo_connection==true && load.spMode == SPEED_DAMP){
                send_i(KEIRYO_EN,0,0);wait(0.2);
                //moving2=99;
                    
                send_i(EJECT_RESET, 0, 0);
                wait_signal();
            }
            return;
        }

        //上ボタンが離されたら、操作ロック解除
        if( btn.down_press() == false) {
            btn.operation_unlock();
        }
        if( btn.down_press() == true) {
            se.Touch_02();
            if(servo_connection==true && load.spMode == SPEED_DAMP){
                //moving2=99;
                send_i(EJECT_RESET, 11+load.kumi_flag, 0);
                wait_signal();
                //loop_a = 0;
                //while((check_signal()==false&&loop_a<=10) /*|| moving2!=0*/){
                ////while(check_signal()==false&&loop_a<=10){
               //     send_i(ON_CHECK,0,0);  //動作してる皿が無いか確認
                //    loop_a+=1;
                 //   wait(0.1);
                //}
                send_i(ON_CHECK,0,close_wait_flg);
                wait_signal();

                while(btn.down_press() == true) {wait(0.1);}
            }
            btn.end_force();
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            now_TC_mode = 3;
            for(int header=0; header<product.head; header++){
                send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
            }
            lcd.messege(TOP,DIS_WAIT,CLS);
            lcd.messege(LOW,DIS_ZERO);
            wait(1);
            //auto_zero_point();                  //ゼロ点取得
            for(int header=0; header<product.head; header++){
                send_i(SERVO_1 ,header,servo_span[header][0]); wait_res();
            }
            wait(1);
            auto_zero_point();                  //ゼロ点取得
            
            now_TC_mode = 1;
            //1行目LCD表示
            if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                lcd.locate(0,0);
                lcd.printf(multiweight);
            } else {
                lcd.settings(&load);      //設定内容を表示
            }
            no_combi_rap = 0;
            
        } else if(btn.up_press() == true) {
            se.chime_01();
            btn.end_force();
            //send_i(OPTION_ST,0,0);wait(0.2);
            while(btn.any_press()==true){
                if(demo_flg==true&&btn.down_press() == true&&file.get_preset()==product.head*2){
                    demo_continue=2;
                    btn.force_ESC();
                    se.Touch_01();
                    continue;
                }
                wait(0.1);
            }
            /*
            if(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2){
                    demo_continue=2;
                    btn.force_ESC();
                    continue;
            }
            */
            //send_i(OPTION_ST,0,0);wait(0.2);
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            
            
            
            if(load.kumi_flag == KUMI_OYAKO || load.kumi_flag == KUMI_OYAKO_HALF) {
                driving_option();
                /*
                no_combi_rap = 0;
                btnset = Chenge_vaule_btnD(0);
                if(btnset==0) {
                    auto_zero_off = true;
                    start_keiryo(0);
                    auto_zero_off = false;
                } else if(btnset==1) {
                    set_preset2(file.get_preset(),TL_SPMODE);
                }else if(btnset==2){
                    set_option_setting(OP_BEGINNER);
                }else if(btnset==3){
                    set_option_setting(OP_KIKAKUGAI_V);
                }else if(btnset==4){
                    set_option_setting(OP_KOKUTI);
                }else if(btnset==5){
                    set_option_setting(OP_SEL_BEEP);
                }else if(btnset==6){
                    set_sercret_parameter_one(P_buffer,1); //保存指令付
                }else if(btnset==7){
                    set_option_setting(OP_NO_COMBI_V);
                } 
                */
            }
            //ゆっくりモード時には、
            else if(load.spMode == SPEED_LOW || load.spMode == SPEED_HIGH || load.spMode == SPEED_HYPER || load.spMode == SPEED_DAMP) {
                driving_option();
                no_combi_rap = 0;
                
                //select_keiryo_option(0, load.kumi_flag, 0);
                /*
                btnset = Chenge_vaule_btnD(option.btn_sett);//ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
                if(btnset==0) {
                    set_count(&cnt);
                } else if(btnset==1) {
                    auto_zero_off = true;
                    start_keiryo(0);
                    auto_zero_off = false;
                }else if(btnset==2){
                    set_preset2(file.get_preset(),TL_SPMODE);
                }else if(btnset==3){
                    set_preset2(file.get_preset(),TL_KUMI_FLAG);
                }else if(btnset==4){
                    set_option_setting(OP_BEGINNER);
                }else if(btnset==5){
                    set_option_setting(OP_KIKAKUGAI_V);
                }else if(btnset==6){
                    set_option_setting(OP_KOKUTI);
                }else if(btnset==7){
                    set_option_setting(OP_SEL_BEEP);
                }else if(btnset==8){
                    set_sercret_parameter_one(P_buffer,1); //保存指令付
                }else if(btnset==9){
                    set_option_setting(OP_NO_COMBI_V);
                }*/
            } /*else if(load.spMode == SPEED_HIGH || load.spMode == SPEED_HYPER || load.spMode == SPEED_DAMP) {
                btnset = Chenge_vaule_btnD(0);
                if(btnset==0) {
                    set_preset2(file.get_preset(),TL_HYOUJI);
                } else if(btnset==1) {
                    auto_zero_off = true;
                    start_keiryo(0);
                    auto_zero_off = false;
                }else if(btnset==2){
                    set_preset2(file.get_preset(),TL_SPMODE);
                }else if(btnset==3){
                    set_preset2(file.get_preset(),TL_KUMI_FLAG);
                }else if(btnset==4){
                    set_sercret_parameter_one(P_buffer,1); //保存指令付
                }else if(btnset==5){
                    set_option_setting(OP_NO_COMBI_V);
                }else if(btnset==6){
                    set_option_setting(OP_KOKUTI);
                }else if(btnset==7){
                    servo_setting2();
                    auto_zero_point();
                    btn.end_force();
                }
            }*/
            //1行目LCD表示
            if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                lcd.locate(0,0);
                lcd.printf(multiweight);
                lcd.blank(LOW);
                lcd.printf(":----.-g       ");
            } else {
                lcd.settings(&load);      //設定内容を表示
            }
            if(servo_connection==true){
                send_i(EJECT_RESET, 11+load.kumi_flag, 0);
                wait_signal();
            }
            btn.end_force();
        }

        btn.old_btn();       //スイッチ更新
        
        if(testmode3==true){se.puter(5);}

        if(servo_connection==true && load.spMode == SPEED_DAMP){
            send_i(ON_CHECK,0,close_wait_flg);
            wait_signal();
            /*for(int a = 0; a < product.head; a++) {
                if(((movingb1 & (1<<a)) > 0) && ((movingb1 & (1<<a)) == 0)){
                    cel.break_antei(a);
                    antei_flug[a]=0;
                }
            }*/
        }else{
            moving1=0;
            moving2=0;
        }
        
        if(testmode3==true){se.puter(5);}
        //se.puter(5);
        for(int i = 0; i < product.head; i++) {
            //if(antei_flug[i] == 0){
                setHeadVolt(i, KURI2nd);
            //}else{
            //    setHeadVolt(i, param.KURI_Y);
            //}
        }
        //se.puter(1);
        if(testmode3==true){se.puter(5);}
        bool error_atonose = false;
        for(int i = 0; i < product.head; i++) {
            if(cel.anteiWeight(i) > param.CUT){
                antei_flug[i] = 1;
            }else{
                antei_flug[i] = 0;
            }
            if((moving1 & (1<<i)) > 0 && befour_anteiw[i] > 0){
                //if (((befour_anteiw[i] - cel.anteiWeight(i)) <-5) || befour_anteiw[i] - cel.anteiWeight(i) > 5){
                if (cel.getWeight(i) > param.CUT && (((befour_anteiw[i] - cel.getWeight(i)) < -param.MINI_target) || ((befour_anteiw[i] - cel.getWeight(i)) > param.MINI_target))){
                    
                    if(option.reck_ato == true) {
                        iro[i] = C_R_EXCLA;
                        error_atonose = true;
                    }
                }
            }
        }
        if(testmode3==true){se.puter(5);}
        if(error_atonose == true){
            se.mistake3(6);
            error_atonose = false;
        }

        bool force_zero_flg = false;
        int force_zero_sec = 30;
        /**/
        for(int i = 0; i < product.head; i++) {
            if(no_zero_counter[i] > (force_zero_sec * 10)){
                if(force_zero_flg == false){
                    force_zero_flg = true;
                    close_wait_flg = close_wait_flg | (1 << i);
                }else{
                    no_zero_counter[i] = ((force_zero_sec - 20) * 10);
                }
            }
        }
        /**/
        /*
        //if(no_zero_counter_zero == 0){
            for(int i = 0; i < product.head; i++) {
                //pc.printf(" %02dH: %5d  ",i+1,no_zero_counter[i]);
                if((i+1) %3 == 0){
                    pc.printf("\r\n");
                }
                if(i == product.head-1){
                   // pc.printf("flg: %5d\r\n",close_wait_flg);
                }
            }
        //}
        */
        /*
        for(int i = 0; i < product.head; i++) {
            if(antei_flug[i] == 0){
                setHeadVolt(i, param.KURI);
            }else{
                setHeadVolt(i, param.KURI_Y);
            }
            if(cel.anteiWeight(i) > param.CUT){
                antei_flug[i] = 1;
            }else{
                antei_flug[i] = 0;
            }
        }//for
        */
        //デバッグ用ここから
        
        //if(print_flg == true){  
        if(0){  
            for(int i = 0; i < product.head; i++) {
                if(i==product.head-1){
                    cel.set_head(i);
                    //send_data(i+1,cel.getWeight(i),cel.get_HWeight(i),1);
                    send_data(i+1,cel.getWeight(i),cel.getAin(),1);
                }else{
                    cel.set_head(i);
                    //send_data(i+1,cel.getWeight(i),cel.get_HWeight(i),0);
                    send_data(i+1,cel.getWeight(i),cel.getAin(),0);
                }
            }
        }
        if(print_flg == true){ 
        //if( product.productType >= STANDARD_D) {
            for(int i = 0; i < product.head; i++) {
                /**/
                cel.set_head(i);
                pc.printf("n%02d: w%8.3f aw%8.3f h%7.3f ad%0.8f\r\n",
                i+1,cel.getWeight(i),cel.anteiWeight(i), cel.get_HWeight(i),cel.getAin());
                if(i == product.head - 1) {
                    //printf("%d %f C\r\n",14, temp);
                    pc.printf("========================\r\n");
                }
                /**/
                /*
                cel.set_head(i);
                pc.printf("%02d: %4d %4d\r\n",
                i+1,int(cel.getWeight(i)),int(cel.anteiWeight(i)));
                if(i == product.head - 1) {
                    //printf("%d %f C\r\n",14, temp);
                    pc.printf("========================\r\n");
                }
                */
            }
            /*for(int l=0;l<product.head;l++){
                if(int(cel.anteiWeight(l)) > 0){
                    v.speak_numbers(l+1);
                    v.gram_voice(int(cel.anteiWeight(l)));
                    wait(0.5);
                }
            }*/
        }
        //se.Touch_01();se.Touch_02();se.Touch_01();se.Touch_02();se.Touch_01();se.Touch_02();se.Touch_01();se.Touch_02();
        
            //printf("B:moving1 %5d  || moving2 %5d\r\n",moving1,moving2);
        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            for(int l=0;l<product.head;l++){
                if(iro[l] == C_R_EXCLA){

                }else if( (moving1 & (1<<l))>0 && load.spMode == SPEED_DAMP){
                    //if(gLevel == 1){
                    //    iro[l] = CG_01 + cel.anteiWeight(l) + hosei[l]-1;
                    //}else{
                        iro[l] = C_BUI_Y;
                    //}
                }else {
                    if(cel.anteiWeight(l) > 0){
                    //if(antei_flug[l]==1){
                        //if((int(cel.anteiWeight(l)) + int(hosei[l])-1) < 300){
                        if((int(cel.anteiWeight(l) + hosei_f[l])-1) < 300){
                            //iro[l] = CR_01 + (int(cel.anteiWeight(l) + hosei_f[l])-1);
                            iro[l] = CR_00 + (int(cel.anteiWeight(l) + op_n_hosei + hosei_f[l]));
                            //iro[l] = CR_00 + (int(cel.anteiWeight(l)));
                        }else{
                            iro[l] = C_NUSE_2D;
                        }
                        //if(gLevel == 1){
                            //iro[l] = CR_01 + (int(cel.anteiWeight(l)) + int(hosei[l])-1) % 100;
                        //}else{
                        //    iro[l] = C_NUSE_1D;
                        //}
                    }else{
                        iro[l] = C_NUSE;
                    }
                }
                if(iro[l]==C_NULL){
                    if( (moving1 & (1<<l))>0 && load.spMode == SPEED_DAMP){
                        //if(gLevel == 1){
                        //    iro[l] = CG_01 + cel.anteiWeight(l) + hosei[l]-1;
                        //}else{
                            iro[l] = C_BUI_Y;
                        //}
                    }else {
                        iro[l] = C_NULL;
                    }
                }
            }
        }else{
            if(moving1 == 0){
                led.ledAllOff();            // LED消灯
            }else{
                led.REGIS(moving1);
            }
        }
        
        if(testmode3==true){se.puter(4);}
        //デバッグ用ここまで
        
        //第2上限値適用するか否か
        /*
        if(option.second_target == 1){
            antei_checker=0;
            for(int i = 0; i < product.head; i++) {
                if(cel.anteiWeight(i)>0){
                    antei_checker = antei_checker | (1 << i);
                }
            }
            if( load.kumi_flag <= KUMI_LIMIT && scond_target >= 0 && (bit_count(antei_checker) >= (bscData.startKinji-bit_count(stop_cells)))){
                load.target = scond_target;
                load.jougen = scond_jougen;
            }else{
                load.target = first_target;
                load.jougen = first_jougen;
            }
        }//第2上限値ここまで
        */
        
        //1行目LCD表示
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            lcd.locate(0,0);
            lcd.printf(multiweight);
            lcd.blank(LOW);
            lcd.printf(":----.-g       ");
        } else {
            lcd.settings(&load);      //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
        }

        if(load.kumi_flag == KUMI_KAIKYU) {
            //if(product.productType == MATRIX || product.productType == MATRIX_D || product.productType == ITIGO || product.productType == ITIGO_D){
        } else {
            //重量確認(適正でない重量があるため処理を飛ばす)
            if(check_max_weight(cel.over_weight()) == false) {
                continue;
            }
        }
        
        if(testmode3==true){se.puter(3);}
        if(servo_connection==true && load.spMode == SPEED_DAMP){
            send_i(ON_CHECK,0,close_wait_flg);
            wait_signal();
        }
        if(check_same_count > 50){
            se.mistake3(2);
            btn.force_ESC();
            continue;
        }




//////////////////////皿を動かすか確認     
        //if(moving2==0 || servo_connection==false){
        total_combi_count = 0;
        //if(1){
            
        for(int k=0;k<3;k++){
            orderCombi[k].kumi     = 0;
            orderCombi[k].total    = 0;
            orderCombi[k].priolity = 0;
        }
        if(combi_chk==0){
            combi_chk=1;
        }else {
            combi_chk=0;
        }
        if(load.kumi_flag == KUMI_KAIKYU) {
            //if(product.productType == MATRIX || product.productType == MATRIX_D || product.productType == ITIGO || product.productType == ITIGO_D){
            get_combi_location_multi(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
        } else {
            for(int j=0;j<5;j++){
                Confilcombi[j].kumi     = 0;
                Confilcombi[j].total    = 0;
                Confilcombi[j].priolity = 0;
            }
            //if(testmode1 == false){
            //if(testmode1 == false && load.spMode == SPEED_DAMP){
                get_combi_location_AUTO(orderCombi, &secondCnt,0);     //組み合わせ確認,組み合わせ候補を抽出
                //get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
            //}else{
                //get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
                //get_combi_location_AUTO(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
            //}
        }
        resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得


        //if(gLevel == 1 && orderCombi[0].total > 0 && orderCombi[0].total<10000){
        //    v.speak_numbers(int(orderCombi[0].total));
        //}



        if(testmode3==true){se.puter(2);}
        //printf("A3\r\n");
        
        //振動検知機能がONだったら
        if(param.vivration >= 1) {
            //繰り返し確認する
            for(int i = 0; i < BK_CNT; i++) {
                //組み合わせ候補が安定しているか
                if(checkWeightTaget(&orderCombi[i]) == true) {
                    resultCombi = orderCombi[i];
                    break;
                }
            }
        }
        total_combi_count = 0;
        Confilcombi[0] = resultCombi;                    //最小組み合わせ結果を取得
        //printf("%2d | %4d | to %4d\r\n",total_combi_count , Confilcombi[0].kumi , int(Confilcombi[0].total));
        //
        if(Confilcombi[0].kumi > 0 && 
            Confilcombi[0].total >= (load.target) && 
            Confilcombi[0].total <= (load.target + load.jougen) && 
            load.kumi_flag <= KUMI_LIMIT && load.spMode == SPEED_DAMP && option.btn_sett != 0){
            for(int j = 1;j < 5; j++){
                //Confilcombi[j].kumi     = 0;
                //Confilcombi[j].total    = 0;
                //Confilcombi[j].priolity = 0;
                for(int k=0;k<3;k++){
                    orderCombi[k].kumi     = 0;
                    orderCombi[k].total    = 0;
                    orderCombi[k].priolity = 0;
                }
                get_combi_location_AUTO(orderCombi, &secondCnt, Confilcombi[0].kumi | 
                                                                Confilcombi[1].kumi | 
                                                                Confilcombi[2].kumi | 
                                                                Confilcombi[3].kumi | 
                                                                Confilcombi[4].kumi  );     //組み合わせ確認,組み合わせ候補を抽出
                resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得
                if(param.vivration >= 1) {
                    //繰り返し確認する
                    for(int i = 0; i < BK_CNT; i++) {
                        //組み合わせ候補が安定しているか
                        if(checkWeightTaget(&orderCombi[i]) == true) {
                            resultCombi = orderCombi[i];
                            break;
                        }
                    }
                }
                Confilcombi[j] = resultCombi;                    //最小組み合わせ結果を取得
                printf("%2d | %4d | to %4d\r\n", j , Confilcombi[j].kumi , int(Confilcombi[j].total));
                if(Confilcombi[j].kumi > 0 && 
                    Confilcombi[j].total >= (load.target) && 
                    Confilcombi[j].total <= (load.target + load.jougen)){
                }else{
                    total_combi_count = j;
                    j = 6;
                }
            }
        }
        if(load.kumi_flag <= KUMI_LIMIT){
            resultCombi   = Confilcombi[0];
            orderCombi[1] = Confilcombi[1];
            orderCombi[2] = Confilcombi[2];
        }
        //1行目LCD表示
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            lcd.locate(0,0);
            lcd.printf(multiweight);
            lcd.blank(LOW);
            lcd.printf(":----.-g       ");
        } else {
            lcd.settings(&load);      //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
            if((product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)&& resultCombi.kumi > 0) {
                //for(int l=0;l<product.head;l++){
                //    iro[l]=C_NULL;
                //}
            }
        }
        int inChk=1;
        if(load.kumi_flag == KUMI_KAIKYU) {
            for(int k=0; k < product.head; k++) {
                if((inChk & resultCombi.kumi) > 0) {
                    oldweightVale[k] = cel.getWeight(k);
                    if(oldweightVale[k] > param.CUT && oldweightVale[k] < mweight.u02.rank_min - 1) {
                        load.target = mweight.u01.target;
                        load.jougen = mweight.u01.jougen;
                        load.kumi_min = mweight.u01.kumi_min;
                        load.kumi_max = mweight.u01.kumi_max;
                    } else if(oldweightVale[k] > mweight.u02.rank_min && oldweightVale[k] < mweight.u02.rank_max) {
                        load.target = mweight.u02.target;
                        load.jougen = mweight.u02.jougen;
                        load.kumi_min = mweight.u02.kumi_min;
                        load.kumi_max = mweight.u02.kumi_max;
                    } else if(oldweightVale[k] > mweight.u03.rank_min && oldweightVale[k] < mweight.u03.rank_max) {
                        load.target = mweight.u03.target;
                        load.jougen = mweight.u03.jougen;
                        load.kumi_min = mweight.u03.kumi_min;
                        load.kumi_max = mweight.u03.kumi_max;
                    } else if(oldweightVale[k] > mweight.u04.rank_min && oldweightVale[k] < mweight.u04.rank_max) {
                        load.target = mweight.u04.target;
                        load.jougen = mweight.u04.jougen;
                        load.kumi_min = mweight.u04.kumi_min;
                        load.kumi_max = mweight.u04.kumi_max;
                    } else if(oldweightVale[k] > mweight.u04.rank_max+1) {
                        load.target = mweight.u05.target;
                        load.jougen = mweight.u05.jougen;
                        load.kumi_min = mweight.u05.kumi_min;
                        load.kumi_max = mweight.u05.kumi_max;
                    }
                    res = load.jougen;    //組み合わせ重量の上限値を取得
                    load.jougen -= load.target;   //制限ｱﾘ等と同じ計算式に合わせるための計算
                }
                inChk = inChk <<1;
            }
        }
        
        if(testmode3==true){se.puter(1);}
        //printf("A4\r\n");
                //printf("b0\r\n");
    /*
        if(cel.anteiWeight(0) == 0 || resultCombi.kumi == 0 ){
            se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();
        }
*/

        for(int j=0;j<5;j++){
            if(Confilcombi[j].kumi > 0 && 
               Confilcombi[j].total >= (load.target) && 
               Confilcombi[j].total <= (load.target + load.jougen)){
            }else{
                Confilcombi[j].kumi     = 0;
                Confilcombi[j].total    = 0;
                Confilcombi[j].priolity = 0;
            }
        }


        //if(testmode1 == false){
        if(load.spMode == SPEED_DAMP){
            //show_results_auto(&resultCombi,&orderCombi[1],&orderCombi[2]);     //組み合わせに応じたLED点灯,総重量表
            show_results_auto(&resultCombi,&Confilcombi[1],&Confilcombi[2]);     //組み合わせに応じたLED点灯,総重量表
            orderCombi[1].kumi     = 0;
            orderCombi[1].total    = 0;
            orderCombi[1].priolity = 0;
            orderCombi[2].kumi     = 0;
            orderCombi[2].total    = 0;
            orderCombi[2].priolity = 0;
            if(Confilcombi[3].kumi > 0 && 
                Confilcombi[3].total >= (load.target) && 
                Confilcombi[3].total <= (load.target + load.jougen)){
                //resultCombi   = Confilcombi[0];
                if(Confilcombi[4].kumi > 0){
                    orderCombi[1] = Confilcombi[4];
                }
                se.puter(1);
                se.puter(1);
                show_results_auto(&Confilcombi[3],&Confilcombi[4],&orderCombi[1]);     //組み合わせに応じたLED点灯,総重量表
            }
            //show_results(&resultCombi);     //組み合わせに応じたLED点灯,総重量表示
        }else{
            show_results(&resultCombi);     //組み合わせに応じたLED点灯,総重量表示
            //show_results_auto(&resultCombi,&orderCombi[1],&orderCombi[2]);     //組み合わせに応じたLED点灯,総重量表示
        }
        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
        }else{
            if(moving1 == 0){
            }else{
                led.REGIS(moving1);
            }
        }
        show_count(&cnt);               //カウント表示
        if(testmode3==true){se.puter(0);}
        //printf("A5\r\n");
                // printf("b1\r\n");
                //printf("%d/%d :%d/%d :lp %d\r\n",load.kumi_flag,KUMI_RANK,load.spMode,SPEED_HYPER,junsaiteki_loop);
//*********準最適値だった場合、もう一度組み合わせてみる**************************
        // if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT)  && 
        //     ((load.spMode == SPEED_HYPER) || (load.spMode == SPEED_LOW  ))&&
        //     junsaiteki_loop == 2){
        //     
        //     get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
        //     resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得
        //     if(int(resultCombi.total) > res){
        //     //*********再度準最適値だった場合は無視
        //     }else{
        //         lcd.settings(&load);      //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
        //         show_results_auto(&resultCombi,&orderCombi[1],&orderCombi[2]);     //組み合わせに応じたLED点灯,総重量表示
        //         show_count(&cnt);               //カウント表示
        //     }
        // }
//*********準最適値だった場合**************************
        //printf("%7.2f\r\n", float(load.target) + (float(bit_count(resultCombi.kumi))*param.buffer2));

        if(load.spMode == SPEED_HIGH || load.spMode == SPEED_HYPER || load.spMode == SPEED_DAMP) {
            announceRetention(resultCombi.kumi, stay_cnt);   //滞留アナウンス
        }
        //if(resultCombi.kumi > 0 ){
        //    printf("%8.1f \r\n",resultCombi.total);
        // }
        
        //printf("A6\r\n");

        //ゆっくりモードかつ組み合わせ成立していれば
        if( (load.spMode == SPEED_LOW && resultCombi.kumi > 0 && int(resultCombi.total) <= res) //||
                //(load.spMode == SPEED_LOW && resultCombi.kumi > 0 && int(resultCombi.total) >  res && /*product.limit_on != WIDE_LIMIT &&*/
                //(btn.get_lock_flg() == true || (load.secondOnOff == 1 && (bit_count(cel.getAnteiIn()) >= (bscData.startKinji-bit_count(stop_cells))))))
                ){ 
            
            //printf("AA\r\n");
            if(option.kokuti==1||option.kokuti==2||option.kokuti==3) {
                se.Touch_02();
            }
            //se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();
            driveSlowly(&resultCombi);
            //driveSlowly(&resultCombi,&cnt);
            comp_kumi=0;
            //se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();
            if(product.limit_on==NUMBER_LIMIT && resultCombi.kumi == 0) {
                KUMI_now =KUMI_now +1;
                file.save_limit2(KUMI_now);
                if(KUMI_limit-KUMI_now>0 && KUMI_limit-KUMI_now < 10) {
                    v.last_limit(KUMI_limit-KUMI_now);
                }
            }

            //組み合わせ分を取り終わっていたら
            if(resultCombi.kumi == 0 && cnt.max > 0) {
                if(cnt.num < cnt.max) {
                    cnt.num++;
                    //残りカウント数に合わせて音声お知らせ
                    v.output_audio_Count(cnt.max,cnt.num);
                }
            }//if
        }//ifゆっくりモード終了
        junsaiteki_loop = 0;
        //ﾗﾝｸﾍﾞﾂ組合せのみ1行目LCD表示
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            lcd.locate(0,0);
            lcd.printf(multiweight);
            lcd.blank(LOW);
            lcd.printf(":----.-g       ");
        }
        //se.Touch_01();se.Touch_01();se.Touch_01();
        if(servo_connection==true && load.spMode == SPEED_DAMP){
            send_i(ON_CHECK,0,close_wait_flg);
            wait_signal();
        }

        
        //printf("C:moving1 %5d  || moving2 %5d\r\n",moving1,moving2);
        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            for(int l=0;l<product.head;l++){
                if(iro[l] == C_R_EXCLA){

                }else if( (moving1 & (1<<l))>0 && load.spMode == SPEED_DAMP){
                    //if(gLevel == 1){
                    //    iro[l] = CG_01 + cel.anteiWeight(l) + hosei[l]-1;
                    //}else{
                        iro[l] = C_BUI_Y;
                    //}
                }else {
                    if(cel.anteiWeight(l) > 0){
                    //if(antei_flug[l]==1){
                        //if((int(cel.anteiWeight(l)) + int(hosei[l])-1) < 300){
                        if((int(cel.anteiWeight(l) + hosei_f[l])-1) < 300){
                            //iro[l] = CR_01 + (int(cel.anteiWeight(l) + hosei_f[l])-1);
                            iro[l] = CR_00 + (int(cel.anteiWeight(l) + op_n_hosei + hosei_f[l]));
                            //iro[l] = CR_00 + (int(cel.anteiWeight(l)));
                        }else{
                            iro[l] = C_NUSE_2D;
                        }
                        //if(gLevel == 1){
                            //iro[l] = CR_01 + (int(cel.anteiWeight(l)) + int(hosei[l])-1) % 100;
                        //}else{
                        //    iro[l] = C_NUSE_1D;
                        //}
                    }else{
                        iro[l] = C_NUSE;
                    }
                }
                if(iro[l]==C_NULL){
                    if( (moving1 & (1<<l))>0 && load.spMode == SPEED_DAMP){
                        //if(gLevel == 1){
                        //    iro[l] = CG_01 + cel.anteiWeight(l) + hosei[l]-1;
                        //}else{
                            iro[l] = C_BUI_Y;
                        //}
                    }else {
                        iro[l] = C_NULL;
                    }
                }
            }
        }else{
            if(moving1 == 0){
                led.ledAllOff();            // LED消灯
            }else{
                led.REGIS(moving1);
            }
        }
        //}
        //printf("A1end\r\n");
    }//while
}

//ランク選別モードのみ使用する。
//各ヘッド毎に、目標重量を計測したらLEDを点滅させる
void drive_Rank_Sorting()
{
    combiData_t resultCombi;        //組み合わせ結果(最小値取得用)
    lcd.settings(&load);            //設定内容を表示

    while(1) {
        led.ledAllOff();            // LED消灯
        btn.new_keep_btn();               //スイッチ(New)取得

        //esc_swを押すと、Menu画面に戻る
        if(btn.esc()) {
            se.Touch_01();
            return;
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            auto_zero_point();
            lcd.settings(&load);      //設定内容を表示
        }

        btn.old_btn();       //スイッチ更新

        for(int i = 0; i < product.head; i++) {
            setHeadVolt(i, RANK_KURI);
        }
        resultCombi = get_rank_location();
        //ランクオーバーしたものを表示する
        if(option.rank_over == true && ro_kumi != 0) {
            se.mistake();                     //警告音
            if(option.kikakugai_v==1) {
                //v.output_audio(MSG_NO_RANK);    //音声出力
                v.output_rank_over();
            }
            led.Flash2(ro_kumi);
            if(option.kikakugai_v==1) {
                wait(0.5);
                v.wait_echo();
            }
        } else {
            //組み合わせ確認,最適な組み合わせ候補を抽出
            //組み合わせに応じたLED点灯,総重量表示
            show_rank(&resultCombi);
        }
    }//while
}

//ランク選別モード時に使用する関数
//指定範囲内の計量物の場所を取得する
combiData_t get_rank_location()
{
    const float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    combiData_t com = {0};

    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)
    float *weight = new float[product.head];

    ro_kumi = 0;

    //安定している重量を取得
    for(int i = 0; i < product.head; i++) {
        weight[i] = cel.anteiWeight(i);
    }

    //ヘッド数分の重量確認
    for(int i = 0; i < product.head; i++) {

        //一番最適な重量を取得する
        if((int(weight[i]) > int(load.target)) && (int(weight[i]) <= int(res))) {

            //値を取得
            com.kumi += (1 << i);
            com.total += weight[i];
        } else if(int(weight[i])>= int(res)) {
            ro_kumi += (1 << i);
        }
    }//loop終了

    delete[] weight;
    return com;
}

//ランク選別モードのみ使用する。
//各ヘッド毎に、目標重量を計測したらLEDを点滅させる
void drive_Rank_Sorting2()
{
    char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
                   ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼﾀ
    combiData_t resultCombi;        //組み合わせ結果(最小値取得用)
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    float baratsuki=param.BARATSUKI;//一時保存フォルダ
    int SerialNumber = 0;
    param.BARATSUKI=param.BARATSUKI_R;
    res   = mweight.u01.rank_max ;    //組み合わせ重量の上限値を取得
    res2  = mweight.u02.rank_max;    //組み合わせ重量の上限値を取得
    res3  = mweight.u03.rank_max;    //組み合わせ重量の上限値を取得
    res4  = mweight.u04.rank_max;    //組み合わせ重量の上限値を取得
    res5  = mweight.u05.rank_max;    //組み合わせ重量の上限値を取得
    res6  = mweight.u06.rank_max;    //組み合わせ重量の上限値を取得
    res7  = mweight.u07.rank_max;    //組み合わせ重量の上限値を取得
    res8  = mweight.u08.rank_max;    //組み合わせ重量の上限値を取得
    res9  = mweight.u09.rank_max;    //組み合わせ重量の上限値を取得
    res10 = mweight.u10.rank_max;    //組み合わせ重量の上限値を取得
    //if(mweight.Rcolor==1){
    //    led_r=false;
    //}else{
    //    led_r=true;
    //}
    lcd.settings_rank2(&load);            //設定内容を表示
    cel.set_BARATSUKI(param.BARATSUKI_R);
    for(int i = 0; i < product.head+1; i++) {
        nowrank[i]=99;
        totalweightN[i]=0;
    }
    for(int i = 0; i < 10; i++) {
        totalranker0103[i]=0;
        totalranker0406[i]=0;
        totalranker0709[i]=0;
        totalranker1012[i]=0;
        totalweight0103[i]=0;
        totalweight0406[i]=0;
        totalweight0709[i]=0;
        totalweight1012[i]=0;
        totalrankerA[i] = 0;
        totalweightA[i] = 0;
        totalrankerB[i] = 0;
        totalweightB[i] = 0;
    }

    //作業開始時間記録
    seconds = time(NULL);
    tm *dated3 = localtime(&seconds);
    run_date.tm_year = dated3->tm_year+1900;
    run_date.tm_sec  = dated3->tm_sec;
    run_date.tm_min  = dated3->tm_min;
    run_date.tm_hour = dated3->tm_hour;
    run_date.tm_mday = dated3->tm_mday;
    run_date.tm_mon  = dated3->tm_mon+1;
    run_date.tm_year = dated3->tm_year+1900;
    //
    if(product.use_usb==1 && option.usb_EN==true){
        //SerialNumber = USB_filecheck();
        SerialNumber = USB_filecheck3(run_date.tm_year,run_date.tm_mon,run_date.tm_mday,run_date.tm_hour,run_date.tm_min);
        if(SerialNumber<999){
            SerialNumber=0;
            /*lcd.blank(0);
            lcd.blank(1);
            lcd.locate(0,1);
            lcd.printf(SAVEEROK[1]);
            se.OK();    //警告音
            //wait(3);
            lcd.blank(0);
            lcd.blank(1);*/
        }
    }
    //led.RedOn();
    while(1) {
        //led.ledAllOff();            // LED消灯
        //led.RedOff();
        btn.new_keep_btn();               //スイッチ(New)取得
        //esc_swを押すと、Menu画面に戻る
        if(btn.esc()||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)) {
            se.Touch_01();
            //LCD表示
            lcd.blank(0);
            lcd.blank(1);
            lcd.locate(3,1);
            if(product.use_usb==1 && option.usb_EN==true) {
                lcd.printf(saving[1]);
            }
            //戻りきるまで時間が掛かるので、マトリクス表示を消す
            for(int i = 0; i < product.head; i++) {
                iro[i]=C_NULL;
            }
            //個別データ書き込み(1個以上あったら)
            int total_qty=0;
            for(int i = 0; i < 12; i++) {
                total_qty += totalranker0103[i]+totalranker0406[i]+totalranker0709[i]+totalranker1012[i];
            }
            if(total_qty>0) {
                seconds = time(NULL);
                tm *dated4 = localtime(&seconds);
                int ji=0;
                int hun=0;
                ji = dated4->tm_hour - run_date.tm_hour;
                if(ji<0) {
                    ji += 24;
                }
                hun = dated4->tm_min - run_date.tm_min;
                if(hun<0) {
                    hun += 60;
                    ji -= 1;
                }
                if(product.use_usb==1 && option.usb_EN==true) {
                    save_rankdata(mweight.Rsort,ji,hun);
                }
                if(product.use_usb==1 && option.usb_EN==true) {
                    //USB_rankdata(mweight.Rsort,ji,hun);
                    USB_rankdata_date(mweight.Rsort,ji,hun,SerialNumber,run_date.tm_year,run_date.tm_mon,run_date.tm_mday,run_date.tm_hour,run_date.tm_min);
                    if(product.use_usb==1 && option.usb_EN==true) {
                        lcd.printf(saving[2]);
                    }
                }
            }

            for(int i = 0; i < product.head; i++) {
                oldweightVale[i] = 0;
                nowweightVale[i] = 0;
            }
            param.BARATSUKI=baratsuki;
            file.read_param();          //設定値
            cel.set_BARATSUKI(param.BARATSUKI);
            lcd.blank(0);
            lcd.blank(1);
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            return;
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            auto_zero_point();
            lcd.settings_rank2(&load);            //設定内容を表示
        }
        //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        else if(btn.up_press() == true) {
            se.chime_01();
            /*
            while(btn.any_press()==true){
                if(demo_flg==true&&btn.down_press() == true&&file.get_preset()==product.head*2){
                    demo_continue=3;
                    btn.force_ESC();
                    se.Touch_01();
                    continue;
                }
                wait(0.1);
            }
            if(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2){
                    demo_continue=3;
                    btn.force_ESC();
                    continue;
            }
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            auto_zero_off = true;
            start_keiryo(0);
            auto_zero_off = false;*/
            select_keiryo_option(0, load.kumi_flag, 0);
            lcd.settings_rank2(&load);            //設定内容を表示
        }
        //ﾓｼﾞ回転ON/OFF変更
        else if(btn.get_btn() == BTN_DOWN ) {
            se.Touch_01();
            //if(product.head >= 8) {
            if(product.Array == 0) {
                option.c_rotate =(option.c_rotate +1)%3;
            } else {
                option.c_rotate =(option.c_rotate +1)%2;
            }
        }


        btn.old_btn();       //スイッチ更新

        for(int i = 0; i < product.head; i++) {
            //setHeadVolt(i, 200);
            if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
                cel.set_head(i);
                cel.setAin_zero(i);
            } else {
                //cel.set_head(i);
                //cel.setAinSpeed(200);
                //pc.printf("%d %08.2f %08.2f %08.2f %2d %2d %08.2f\r\n", i+1, cel.getWeight(i), cel.get_HWeight(i), cel.anteiWeight_rank3(i),nowrank[i],iro[i],param.BARATSUKI);

                setHeadVolt(i, 200);
                if(cel.anteiWeight(i)>0) {
                    //setHeadVolt(i, param.KURI);
                    setHeadVolt(i, (1000));
                    if(nowweightVale[i] - cel.anteiWeight(i)>1 && nowweightVale[i] - cel.anteiWeight(i)<-1) {
                        setHeadVolt(i, param.KURI);
                    }
                } else if(cel.getWeight(i)>param.CUT) {
                    setHeadVolt(i, param.KURI);
                    //setHeadVolt(i, (1000));
                }
            }

        }

        //********     デバッグ用ここから
        if( product.productType >= STANDARD_D) {
            for(int i = 0; i < product.head; i++) {
                //temp = cel.temperature();   //温度を取得する
                //for(int i = 0; i < product.head; i++){
                cel.set_head(i);
                //printf("%d %6.3f %6.3f   %0.8f\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.getAin());
                pc.printf("%d %08.2f %08.2f %08.2f %2d %2d %08.2f\r\n", i+1, cel.getWeight(i), cel.get_HWeight(i), cel.anteiWeight_rank3(i),nowrank[i],iro[i],param.BARATSUKI);

                //}
                if(i == product.head - 1) {
                    //printf("%d %f C\r\n",14, temp);

                    printf("========================\r\n");
                    printf("========================\r\n");
                    printf("========================\r\n");
                }
            }
        }
        //********デバッグ用ここまで

        //組み合わせ確認,最適な組み合わせ候補を抽出
        resultCombi = get_rank_location2();

        //組み合わせに応じたLED点灯,総重量表示
        show_rank2(&resultCombi);
    }//while
}
/*{mweight.u01.rank_en ,mweight.u01.rank_min ,mweight.u01.rank_max ,mweight.u01.kumi_min ,mweight.u01.kumi_max ,mweight.u01.target ,mweight.u01.jougen}
 ,{mweight.u02.rank_en ,mweight.u02.rank_min ,mweight.u02.rank_max ,mweight.u02.kumi_min ,mweight.u02.kumi_max ,mweight.u02.target ,mweight.u02.jougen}
 ,{mweight.u03.rank_en ,mweight.u03.rank_min ,mweight.u03.rank_max ,mweight.u03.kumi_min ,mweight.u03.kumi_max ,mweight.u03.target ,mweight.u03.jougen}
 ,{mweight.u04.rank_en ,mweight.u04.rank_min ,mweight.u04.rank_max ,mweight.u04.kumi_min ,mweight.u04.kumi_max ,mweight.u04.target ,mweight.u04.jougen}
 ,{mweight.u05.rank_en ,mweight.u05.rank_min ,mweight.u05.rank_max ,mweight.u05.kumi_min ,mweight.u05.kumi_max ,mweight.u05.target ,mweight.u05.jougen}};
*/




//ランク選別モード時に使用する関数
//指定範囲内の計量物の場所を取得する
combiData_t get_rank_location2()
{
    combiData_t com = {0};
    //printf("%4.0f:%4.0f:%4.0f:%4.0f:%4.0f:%4.0f:%4.0f\r\n",res,res2,res3,res4,res5,res6,res7);
    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)
    float *weight = new float[product.head];
    //param.BARATSUKI=param.BARATSUKI_R + 0.5;
    //安定している重量を取得
    for(int i = 0; i < product.head; i++) {
        weight[i] = cel.anteiWeight_rank(i);
        if(weight[i] > 0) {
            totalweightN[i]=weight[i];
        } else if(weight[i] == 0 && (cel.getWeight(i) < param.CUT+3 && oldweightVale[i]< param.CUT+3) && recrank[i]>=0 && recrank[i]<=10) { //安定が無くなって、現在の値が0付近になったら
            printf("%2d : %5.1f\r\n",recrank[i],totalweightN[i]);
            if(i<=2) {
                totalweight0103[recrank[i]] += totalweightN[i];
                totalranker0103[recrank[i]] += 1;
            } else if(i<=5) {
                totalweight0406[recrank[i]] += totalweightN[i];
                totalranker0406[recrank[i]] += 1;
            } else if(i<=8) {
                totalweight0709[recrank[i]] += totalweightN[i];
                totalranker0709[recrank[i]] += 1;
            } else if(i<=11) {
                totalweight1012[recrank[i]] += totalweightN[i];
                totalranker1012[recrank[i]] += 1;
            }
            totalweightN[i] = 0;
            recrank[i]      = 99;
        }
        if((weight[i] == 0 && cel.getWeight(i) > param.CUT && cel.getWeight(i) <= param.CUT)||cel.anteiWeight_rank(i)>=param.CUT) {
            weight[i] = cel.anteiWeight_rank(i);
        }
        //if(i==0){
        //    printf("%d %6.3f %6.3f  %6.3f  %d\r\n", i+1,cel.anteiWeight(i),cel.anteiWeight(i), weight[i],iro[i]);
        //}
        //if(weight[i]==0){
        //    weight[i] = cel.anteiWeight(i);
        //}
        int data_sa =oldweightVale[i] - nowweightVale[i];
        if(data_sa < -0.3 || data_sa > 0.4) {

        } else {
            nowweightVale[i] = oldweightVale[i];
            //weight[i] = oldweightVale[i];
        }
        oldweightVale[i] = nowweightVale[i];
    }

    //ヘッド数分の重量確認
    for(int i = 0; i < product.head; i++) {
        if(weight[i]!=0) {
            //一番最適な重量を取得する
            if((int(weight[i]) > -param.CUT) && (int(weight[i]) <= param.CUT)) {
                iro[i]=PENTA_R;
                nowrank[i]=99;
                //値を取得
                //com.kumi += (1 << i);
                //com.total += weight[i];
            } else if((int(weight[i]) > param.CUT) && (int(weight[i]) < int(res))) {
                if(mweight.u01.rank_en==1) {
                    iro[i]=C_BATU;
                    nowrank[i]=0;
                    //値を取得
                    //com.kumi += (1 << i);
                    //com.total += weight[i];
                } else {
                    //iro[i]=C_NULL;
                    iro[i]=C_BATU;
                    nowrank[i]=99;
                }
            } else if((int(weight[i]) >= int(res)) && (int(weight[i]) < int(res2))) {
                if(mweight.u02.rank_en==1) {
                    iro[i]=mweight.u02.rank_no;
                    nowrank[i]=1;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((int(weight[i]) >= int(res2)) && (int(weight[i]) < int(res3))) {
                if(mweight.u03.rank_en==1) {
                    iro[i]=mweight.u03.rank_no;
                    nowrank[i]=2;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((int(weight[i]) >= int(res3)) && (int(weight[i]) < int(res4))) {
                if(mweight.u04.rank_en==1) {
                    iro[i]=mweight.u04.rank_no;
                    nowrank[i]=3;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((int(weight[i]) >= int(res4)) && (int(weight[i]) < int(res5))) {
                if(mweight.u05.rank_en==1) {
                    iro[i]=mweight.u05.rank_no;
                    nowrank[i]=4;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((int(weight[i]) >= int(res5)) && (int(weight[i]) < int(res6))) {
                if(mweight.u06.rank_en==1) {
                    iro[i]=mweight.u06.rank_no;
                    nowrank[i]=5;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((int(weight[i]) >= int(res6)) && (int(weight[i]) < int(res7))) {
                if(mweight.u07.rank_en==1) {
                    iro[i]=mweight.u07.rank_no;
                    nowrank[i]=6;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((int(weight[i]) >= int(res7)) && (int(weight[i]) < int(res8))) {
                if(mweight.u08.rank_en==1) {
                    iro[i]=mweight.u08.rank_no;
                    nowrank[i]=7;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            }
             else if((int(weight[i]) >= int(res8)) && (int(weight[i]) < int(res9))) {
                if(mweight.u09.rank_en==1) {
                    iro[i]=mweight.u09.rank_no;
                    nowrank[i]=8;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            }
             else if((int(weight[i]) >= int(res9)) && (int(weight[i]) < int(res10))) {
                if(mweight.u10.rank_en==1) {
                    iro[i]=mweight.u10.rank_no;
                    nowrank[i]=9;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            }
            else if(int(weight[i]) >= int(res10)) {
                iro[i]=C_BATU;
                nowrank[i]=10;
                com.kumi += (1 << i);
                com.total += weight[i];
            } else {
                iro[i]=PENTA_R;
                nowrank[i]=99;
            }
        } else if(cel.getWeight(i) < param.CUT+3 && oldweightVale[i]< param.CUT+3) {
            iro[i]=C_NULL;
        }
        if(cel.anteiWeight_rank(i)>0) {
            recrank[i]= nowrank[i];
        }
    }//loop終了

    delete[] weight;
    return com;


}


//ランク選別モードのみ使用する。
//各ヘッド毎に、目標重量を計測したらLEDを点滅させる
//ｺｳｿｸﾓｰﾄﾞ
void drive_Rank_Sorting3()
{
    char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
                   ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼﾀ
    combiData_t resultCombi;        //組み合わせ結果(最小値取得用)
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    float baratsuki=param.BARATSUKI;//一時保存フォルダ
    int SerialNumber = 0;
    param.BARATSUKI=param.BARATSUKI_R;
    res    = float(mweight.u01.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res2   = float(mweight.u02.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res3   = float(mweight.u03.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res4   = float(mweight.u04.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res5   = float(mweight.u05.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res6   = float(mweight.u06.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res7   = float(mweight.u07.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res8   = float(mweight.u08.rank_max);//*1.03;   //組み合わせ重量の上限値を取得
    res9   = float(mweight.u09.rank_max);//*1.03;   //組み合わせ重量の上限値を取得
    res10  = float(mweight.u10.rank_max);//*1.03;   //組み合わせ重量の上限値を取得
    rank1ave  = (float(mweight.u01.rank_max)+float(mweight.u02.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank2ave  = (float(mweight.u02.rank_max)+float(mweight.u03.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank3ave  = (float(mweight.u03.rank_max)+float(mweight.u04.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank4ave  = (float(mweight.u04.rank_max)+float(mweight.u05.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank5ave  = (float(mweight.u05.rank_max)+float(mweight.u06.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank6ave  = (float(mweight.u06.rank_max)+float(mweight.u07.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank7ave  = (float(mweight.u07.rank_max)+float(mweight.u08.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank8ave  = (float(mweight.u08.rank_max)+float(mweight.u09.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank9ave  = (float(mweight.u09.rank_max)+float(mweight.u10.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank1dif  = (float(mweight.u02.rank_max)+float(mweight.u01.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank2dif  = (float(mweight.u03.rank_max)+float(mweight.u02.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank3dif  = (float(mweight.u04.rank_max)+float(mweight.u03.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank4dif  = (float(mweight.u05.rank_max)+float(mweight.u04.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank5dif  = (float(mweight.u06.rank_max)+float(mweight.u05.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank6dif  = (float(mweight.u07.rank_max)+float(mweight.u06.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank7dif  = (float(mweight.u08.rank_max)+float(mweight.u07.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank8dif  = (float(mweight.u09.rank_max)+float(mweight.u08.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank9dif  = (float(mweight.u10.rank_max)+float(mweight.u09.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    //if(mweight.Rcolor==1){
    //    led_r=false;
    //}else{
    //    led_r=true;
    //}
    int btnset = 0;
    lcd.settings_rank2(&load);            //設定内容を表示
    for(int i = 0; i < product.head; i++) {
        nowrank[i]=0;
        recrank[i]=99;
    }
    for(int i = 0; i < 11; i++) {
        totalranker0103[i]=0;
        totalranker0406[i]=0;
        totalranker0709[i]=0;
        totalranker1012[i]=0;
        totalweight0103[i]=0;
        totalweight0406[i]=0;
        totalweight0709[i]=0;
        totalweight1012[i]=0;
        totalrankerA[i] = 0;
        totalweightA[i] = 0;
        totalrankerB[i] = 0;
        totalweightB[i] = 0;
        totalrankerC[i] = 0;
        totalweightC[i] = 0;
        totalrankerD[i] = 0;
        totalweightD[i] = 0;
    }

    //作業開始時間記録
    seconds = time(NULL);
    tm *dated3 = localtime(&seconds);
    run_date.tm_year = dated3->tm_year+1900;
    run_date.tm_sec  = dated3->tm_sec;
    run_date.tm_min  = dated3->tm_min;
    run_date.tm_hour = dated3->tm_hour;
    run_date.tm_mday = dated3->tm_mday;
    run_date.tm_mon  = dated3->tm_mon+1;
    run_date.tm_year = dated3->tm_year+1900;
    //

    if(product.use_usb==1 && option.usb_EN==true){
        //SerialNumber = USB_filecheck();
        SerialNumber = USB_filecheck3(run_date.tm_year,run_date.tm_mon,run_date.tm_mday,run_date.tm_hour,run_date.tm_min);
        if(SerialNumber<999){
            SerialNumber=0;
            /*lcd.blank(0);
            lcd.blank(1);
            lcd.locate(0,1);
            lcd.printf(SAVEEROK[1]);
            se.OK();    //警告音
            //wait(3);
            lcd.blank(0);
            lcd.blank(1);*/
        }
    }
    //led.RedOn();
    while(1) {
        //led.ledAllOff();            // LED消灯
        //led.RedOff();
        btn.new_keep_btn();               //スイッチ(New)取得
        

        //esc_swを押すと、Menu画面に戻る
        if(btn.esc()||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)) {
            se.Touch_01();
            comp_rank=0;
            //LCD表示
            lcd.blank(0);
            lcd.blank(1);
            lcd.locate(3,1);
            if(product.use_usb==1 && option.usb_EN==true) {
                     if(product.use_usb==1 && option.usb_EN==true) {
                        lcd.printf(saving[2]);
                    }
                    lcd.printf(saving[1]);
            }
            //戻りきるまで時間が掛かるので、マトリクス表示を消す
            for(int i = 0; i < product.head; i++) {
                iro[i]=C_NULL;
            }
            //個別データ書き込み(1個以上あったら)
            int total_qty=0;
            for(int i = 0; i < 11; i++) {
                total_qty += totalranker0103[i]+totalranker0406[i]+totalranker0709[i]+totalranker1012[i];
            }
            if(total_qty>0) {
                seconds = time(NULL);
                tm *dated4 = localtime(&seconds);
                int ji=0;
                int hun=0;
                ji = dated4->tm_hour - run_date.tm_hour;
                if(ji<0) {
                    ji += 24;
                }
                hun = dated4->tm_min - run_date.tm_min;
                if(hun<0) {
                    hun += 60;
                    ji -= 1;
                }
                if(product.use_usb==1 && option.usb_EN==true) {
                    save_rankdata(mweight.Rsort,ji,hun);
                }
                if(product.use_usb==1 && option.usb_EN==true) {
                    //USB_rankdata(mweight.Rsort,ji,hun);
                    USB_rankdata_date(mweight.Rsort,ji,hun,SerialNumber,run_date.tm_year,run_date.tm_mon,run_date.tm_mday,run_date.tm_hour,run_date.tm_min);
                    if(product.use_usb==1 && option.usb_EN==true) {
                        lcd.printf(saving[2]);
                    }
                }
            }

            for(int i = 0; i < product.head; i++) {
                oldweightVale[i] = 0;
                nowweightVale[i] = 0;
                nowweightVale2[i] = 0;
                nowweightVale3[i] = 0;
                nowweightVale4[i] = 0;
                recrank[i]=99;
            }
            param.BARATSUKI=baratsuki;
            file.read_param();          //設定値
            cel.set_BARATSUKI(param.BARATSUKI);
            lcd.blank(0);
            lcd.blank(1);
            for(int i = 0; i < product.head; i++) {
                nowrank[i]=0;
                totalweightN[i]=0;
                nearrank[3][i]=0;
                nearrank[2][i]=0;
                nearrank[1][i]=0;
                nearrank[0][i]=0;
            }
            for(int i = 0; i < 11; i++) {
                totalranker0103[i]=0;
                totalranker0406[i]=0;
                totalranker0709[i]=0;
                totalranker1012[i]=0;
                totalweight0103[i]=0;
                totalweight0406[i]=0;
                totalweight0709[i]=0;
                totalweight1012[i]=0;
                totalrankerA[i] = 0;
                totalweightA[i] = 0;
                totalrankerB[i] = 0;
                totalweightB[i] = 0;
                totalrankerC[i] = 0;
                totalweightC[i] = 0;
                totalrankerD[i] = 0;
                totalweightD[i] = 0;
            }
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            return;
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            auto_zero_point();
            lcd.settings_rank2(&load);            //設定内容を表示
        }
        //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        else if(btn.up_press() == true) {
            se.chime_01();
            /*
            while(btn.any_press()==true){
                if(demo_flg==true&&btn.down_press() == true&&file.get_preset()==product.head*2){
                    demo_continue=3;
                    btn.force_ESC();
                    se.Touch_01();
                    continue;
                }
                wait(0.1);
            }
            if(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2){
                    demo_continue=3;
                    btn.force_ESC();
                    continue;
            }
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            
            btnset = Chenge_vaule_btnD(99);//ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
            if(btnset==0) {
                if(multi_weight_set3(file.get_preset(),0 )==0){
                    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
                }
            } else if(btnset==1) {
                auto_zero_off = true;
                start_keiryo(0);
                auto_zero_off = false;
            }*/
            select_keiryo_option(0, load.kumi_flag, 0);
            lcd.settings_rank2(&load);            //設定内容を表示
        }
        //ﾓｼﾞ回転ON/OFF変更
        else if(btn.get_btn() == BTN_DOWN ) {
            se.Touch_01();
            //if(product.head >= 8) {
            if(product.Array == 0) {
                option.c_rotate =(option.c_rotate +1)%3;
            } else {
                option.c_rotate =(option.c_rotate +1)%2;
            }
        }

        btn.old_btn();       //スイッチ更新

        for(int i = 0; i < product.head; i++) {
            //setHeadVolt(i, 200);
            if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
                cel.set_head(i);
                cel.setAin_zero(i);
            } else {
                //cel.set_head(i);
                //cel.setAinSpeed(200);
                param.BARATSUKI = cel.getWeight(i)*0.02;
                cel.set_BARATSUKI(param.BARATSUKI);
                setHeadVolt(i, 600);
                //if(cel.anteiWeight_rank3(i)>param.CUT){//ｺｳｿｸでは取られたか確認できればいいので繰返し回数は少なめに
                if(nowrank[i]!=0) {
                    //setHeadVolt(i, param.KURI);
                    setHeadVolt(i, 1000);
                    /*if(nowweightVale[i] - cel.anteiWeight(i)>1 && nowweightVale[i] - cel.anteiWeight(i)<-1){
                        setHeadVolt(i, param.KURI);
                    }*/
                } else if(cel.getWeight(i)>param.CUT) {
                    setHeadVolt(i, 1500);
                    //setHeadVolt(i, (1000));
                }
                //pc.printf("%d %08.2f %08.2f %08.2f  %2d  %08.2f\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.anteiWeight_rank3(i),nowrank[i],param.BARATSUKI);
            }
        }

        //********     デバッグ用ここから
        if( product.productType >= STANDARD_D) {
            for(int i = 0; i < product.head; i++) {
                //temp = cel.temperature();   //温度を取得する
                //for(int i = 0; i < product.head; i++){
                cel.set_head(i);
                //    pc.printf("%d %08.2f %08.2f %08.2f  %2d\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.anteiWeight_rank3(i),nowrank[i]);
                pc.printf("%d %08.2f %08.2f %08.2f %2d %2d %08.2f\r\n", i+1, cel.getWeight(i), cel.get_HWeight(i), cel.anteiWeight_rank3(i),nowrank[i],iro[i],param.BARATSUKI);
                //}
                if(i == product.head - 1) {
                    //printf("%d %f C\r\n",14, temp);

                    pc.printf("========================\r\n");
                }
            }
        }
        //********デバッグ用ここまで

        //組み合わせ確認,最適な組み合わせ候補を抽出
        resultCombi = get_rank_location3();

        //組み合わせに応じたLED点灯,総重量表示
        show_rank2(&resultCombi);
    }//while
}

//ランク選別モード時に使用する関数
//指定範囲内の計量物の場所を取得する
//ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂ用なので、ﾗﾝｸで指定された値より3パーセント高い値まで、
//範囲内とする様、設定する(1.02)
combiData_t get_rank_location3()
{
    combiData_t com = {0};
    //printf("%4.0f:%4.0f:%4.0f:%4.0f:%4.0f:%4.0f:%4.0f\r\n",res,res2,res3,res4,res5,res6,res7);
    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)

    float *weight = new float[product.head];
    //安定している重量を取得
    for(int i = 0; i < product.head; i++) {
        weight[i] = cel.anteiWeight_rank3(i)+param.bufferT;
        if(weight[i] > 0) {
            totalweightN[i]=weight[i];
        } else if(weight[i] == 0 && (cel.getWeight(i) < param.CUT+3 && oldweightVale[i]< param.CUT+3) && recrank[i]>=0 && recrank[i]<=10) { //安定が無くなって、現在の値が0付近になったら

            if(i<=2) {
                if(float((int(totalweightN[i])) >= param.CUT)){
                    totalweight0103[recrank[i]] += totalweightN[i];
                    totalranker0103[recrank[i]] += 1;
                }
            } else if(i<=5) {
                if(float((int(totalweightN[i])) >= param.CUT)){
                    totalweight0406[recrank[i]] += totalweightN[i];
                    totalranker0406[recrank[i]] += 1;
                }
            } else if(i<=8) {
                if(float((int(totalweightN[i])) >= param.CUT)){
                    totalweight0709[recrank[i]] += totalweightN[i];
                    totalranker0709[recrank[i]] += 1;
                }
            } else if(i<=11) {
                if(float((int(totalweightN[i])) >= param.CUT)){
                    totalweight1012[recrank[i]] += totalweightN[i];
                    totalranker1012[recrank[i]] += 1;
                }
            }
            totalweightN[i] = 0;
            recrank[i]      = 99;
        }
        if((weight[i] == 0 && cel.getWeight(i) > param.CUT && cel.getWeight(i) <= param.CUT)||cel.anteiWeight_rank(i)>=param.CUT) {
            weight[i] = cel.anteiWeight_rank3(i)+param.bufferT;
        }
        //if(i==0){
        //    pc.printf("%d %6.3f %6.3f  %6.3f  %d\r\n", i+1,cel.anteiWeight(i),cel.anteiWeight(i), weight[i],iro[i]);
        //}
        //if(weight[i]==0){
        //    weight[i] = cel.anteiWeight(i);
        //}
        int data_sa =oldweightVale[i] - nowweightVale[i];
        if(data_sa < -0.3 || data_sa > 0.4) {

        } else {
            nowweightVale[i] = oldweightVale[i];
            //weight[i] = oldweightVale[i];
        }
        oldweightVale[i] = nowweightVale[i];
    }
    //ヘッド数分の重量確認
    for(int i = 0; i < product.head; i++) {
        nowweightVale4[i] = nowweightVale3[i];
        nowweightVale3[i] = nowweightVale2[i];
        nowweightVale2[i] = cel.getWeight(i);
        if((weight[i]!=0 && nowrank[i]==99)||(iro[i]==C_NULL && nowrank[i]!=0)) {//一度ﾗﾝｸ確定したものは除外する(ｹｲﾘｮｳﾓｰﾄﾞに行った時はもう一度表示する)
            //一番最適な重量を取得する
            if((int(weight[i]) > -param.CUT) && float((int(weight[i])) <= param.CUT)) {
                iro[i]=C_NULL;
                nowrank[i]=99;
                //値を取得
                //com.kumi += (1 << i);     //com.total += weight[i];
            } else if((float(int(weight[i])) > param.CUT) && (float(int(weight[i])) < float(res))) {
                if(mweight.u01.rank_en==1) {
                    iro[i]=C_BATU;
                    nowrank[i]=0;
                    //値を取得
                    //com.kumi += (1 << i);//com.total += weight[i];
                } else {
                    //iro[i]=C_NULL;
                    iro[i]=C_BATU;
                    nowrank[i]=0;
                }
            } else if((float(int(weight[i])) >= float(res)) && (float(int(weight[i])) < float(res2))) {
                if(mweight.u02.rank_en==1) {
                    iro[i]=mweight.u02.rank_no;
                    nowrank[i]=1;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res2)) && (float(int(weight[i])) < float(res3))) {
                if(mweight.u03.rank_en==1) {
                    iro[i]=mweight.u03.rank_no;
                    nowrank[i]=2;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res3)) && (float(int(weight[i])) < float(res4))) {
                if(mweight.u04.rank_en==1) {
                    iro[i]=mweight.u04.rank_no;
                    nowrank[i]=3;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res4)) && (float(int(weight[i])) < float(res5))) {
                if(mweight.u05.rank_en==1) {
                    iro[i]=mweight.u05.rank_no;
                    nowrank[i]=4;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res5)) && (float(int(weight[i])) < float(res6))) {
                if(mweight.u06.rank_en==1) {
                    iro[i]=mweight.u06.rank_no;
                    nowrank[i]=5;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res6)) && (float(int(weight[i])) < float(res7))) {
                if(mweight.u07.rank_en==1) {
                    iro[i]=mweight.u07.rank_no;
                    nowrank[i]=6;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res7)) && (float(int(weight[i])) < float(res8))) {
                if(mweight.u08.rank_en==1) {
                    iro[i]=mweight.u08.rank_no;
                    nowrank[i]=7;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res8)) && (float(int(weight[i])) < float(res9))) {
                if(mweight.u09.rank_en==1) {
                    iro[i]=mweight.u09.rank_no;
                    nowrank[i]=8;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res9)) && (float(int(weight[i])) < float(res10))) {
                if(mweight.u10.rank_en==1) {
                    iro[i]=mweight.u10.rank_no;
                    nowrank[i]=9;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if(float(int(weight[i])) >= float(res10)) {
                iro[i]=C_BATU;
                nowrank[i]=10;
                com.kumi += (1 << i);
                com.total += weight[i];
            } else {
                iro[i]=PENTA_R;
                nowrank[i]=10;
            }
            //}else if(cel.getWeight(i) > param.CUT && weight[i] < param.CUT) {
        } else if(cel.getWeight(i) > param.CUT && nowrank[i]==0) {
            //iro[i]=C_NUSE;
            //pc.printf("%dc\r\n",i+1);
        } else if(cel.getWeight(i) < param.CUT && nowrank[i]==0) {
            iro[i]=C_NULL;
            //}else if(nowrank[i]!=0 && nowweightVale2[i] < param.CUT+3 && nowweightVale3[i]< param.CUT+3) {
        } else if(nowrank[i]!=0 && nowweightVale2[i] < param.CUT+3) {
            setHeadVolt(i, 1500);
            if(cel.getWeight(i) < param.CUT) {
                iro[i]=C_NULL;
                nowrank[i]=99;
            }
            //pc.printf("%da\r\n",i+1);
        }


        //大体真ん中付近で重量が止まってたら、その値を確定とする
        //for(int m=0;m<4;m++){
        nearrank[3][i]=nearrank[2][i];
        nearrank[2][i]=nearrank[1][i];
        nearrank[1][i]=nearrank[0][i];
        nearrank[0][i]=99;
        //}
        //if(nowrank[i]==0||(float(int(cel.getWeight(i))) > param.CUT)) {
        if(1) {
            if(float(int(cel.getWeight(i)) > -param.CUT) && float(int(cel.getWeight(i))<= param.CUT)) {
                nearrank[0][i]=99;
                //値を取得
                //com.kumi += (1 << i);
                //com.total += weight[i];
            } else if((float(int(cel.getWeight(i))) > param.CUT) && (float(int(cel.getWeight(i))) < float(res))) {
                if(mweight.u01.rank_en==1) {
                    nearrank[0][i]=0;
                    //nearrank[0][i]=0;
                } else {//iro[i]=C_BATU;nowrank[i]=0;
                    nearrank[0][i]=0;
                    //nearrank[0][i]=0;
                }
            } else if((float(int(cel.getWeight(i))) >= float(rank1ave-rank1dif)) && (float(int(cel.getWeight(i))) < float(res2))) {
                if(mweight.u02.rank_en==1) {
                    nearrank[0][i]=mweight.u02.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(cel.getWeight(i))) >= float(rank2ave-rank2dif)) && (float(int(cel.getWeight(i))) < float(res3))) {
                if(mweight.u03.rank_en==1) {
                    nearrank[0][i]=mweight.u03.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(cel.getWeight(i))) >= float(rank3ave-rank3dif)) && (float(int(cel.getWeight(i))) < float(res4))) {
                if(mweight.u04.rank_en==1) {
                    nearrank[0][i]=mweight.u04.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(cel.getWeight(i))) >= float(rank4ave-rank4dif)) && (float(int(cel.getWeight(i))) < float(res5))) {
                if(mweight.u05.rank_en==1) {
                    nearrank[0][i]=mweight.u05.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(cel.getWeight(i))) >= float(rank5ave-rank5dif)) && (float(int(cel.getWeight(i))) < float(res6))) {
                if(mweight.u06.rank_en==1) {
                    nearrank[0][i]=mweight.u06.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(cel.getWeight(i))) >= float(rank6ave-rank6dif)) && (float(int(cel.getWeight(i))) < float(res7))) {
                if(mweight.u07.rank_en==1) {
                    nearrank[0][i]=mweight.u07.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(cel.getWeight(i))) >= float(rank7ave-rank7dif)) && (float(int(cel.getWeight(i))) < float(res8))) {
                if(mweight.u08.rank_en==1) {
                    nearrank[0][i]=mweight.u08.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(cel.getWeight(i))) >= float(rank8ave-rank8dif)) && (float(int(cel.getWeight(i))) < float(res9))) {
                if(mweight.u09.rank_en==1) {
                    nearrank[0][i]=mweight.u09.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(cel.getWeight(i))) >= float(rank9ave-rank9dif)) && (float(int(cel.getWeight(i))) < float(res10))) {
                if(mweight.u10.rank_en==1) {
                    nearrank[0][i]=mweight.u10.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if(float(int(cel.getWeight(i))) >= float(res10)) {
                //nearrank[0][i]=C_BATU;
                nearrank[0][i]=10;
            } else {//iro[i]=PENTA_R;nowrank[i]=0;
                nearrank[0][i]=99;
            }
            //連続で同じﾗﾝｸだった場合、(上の設定値が低めギリギリを除外してるので)ﾗﾝｸとしては確証がもてるので、表示を変える
            //if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i] && nearrank[1][i] == nearrank[2][i] && nearrank[2][i] == nearrank[3][i]){
            if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i] && nearrank[1][i] == nearrank[2][i] && nearrank[1][i] == nearrank[3][i]) {
                if(nearrank[0][i]==0||nearrank[0][i]==10){
                    iro[i] =C_BATU;
                }else{
                    iro[i]=nearrank[0][i];
                }
                if(     nearrank[0][i]==0                  ){nowrank[i]= 0;}
                else if(nearrank[0][i]==mweight.u02.rank_no){nowrank[i]= 1;}
                else if(nearrank[0][i]==mweight.u03.rank_no){nowrank[i]= 2;}
                else if(nearrank[0][i]==mweight.u04.rank_no){nowrank[i]= 3;}
                else if(nearrank[0][i]==mweight.u05.rank_no){nowrank[i]= 4;}
                else if(nearrank[0][i]==mweight.u06.rank_no){nowrank[i]= 5;}
                else if(nearrank[0][i]==mweight.u07.rank_no){nowrank[i]= 6;}
                else if(nearrank[0][i]==mweight.u08.rank_no){nowrank[i]= 7;}
                else if(nearrank[0][i]==mweight.u09.rank_no){nowrank[i]= 8;}
                else if(nearrank[0][i]==mweight.u10.rank_no){nowrank[i]= 9;}
                else if(nearrank[0][i]==10                 ){nowrank[i]=10;}
                else{                                        nowrank[i]=99;}
                
                
                //nowrank[i]=nearrank[0][i];
                if(weight[i] != 0) {
                    //comp_rank = comp_rank | 1<<i;
                } else {
                    //comp_rank = comp_rank & ~(1<<i);
                }
                /*for(int m=0;m<4;m++){
                    nearrank[3][i]=0;
                    nearrank[2][i]=0;
                    nearrank[1][i]=0;
                    nearrank[0][i]=0;
                }*/
                /*if(i==3||i==2){
                    printf("%dd\r\n",i+1);
                }*/
            } else if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i] && nearrank[1][i] == nearrank[2][i]) {
                if(nearrank[0][i]==0||nearrank[0][i]==10){
                    iro[i] =C_BATU;
                    nowrank[i]=nearrank[0][i];
                }else{
                    iro[i]=nearrank[0][i];
                    if(     nearrank[0][i]==0                  ){nowrank[i]= 0;}
                    else if(nearrank[0][i]==mweight.u02.rank_no){nowrank[i]= 1;}
                    else if(nearrank[0][i]==mweight.u03.rank_no){nowrank[i]= 2;}
                    else if(nearrank[0][i]==mweight.u04.rank_no){nowrank[i]= 3;}
                    else if(nearrank[0][i]==mweight.u05.rank_no){nowrank[i]= 4;}
                    else if(nearrank[0][i]==mweight.u06.rank_no){nowrank[i]= 5;}
                    else if(nearrank[0][i]==mweight.u07.rank_no){nowrank[i]= 6;}
                    else if(nearrank[0][i]==mweight.u08.rank_no){nowrank[i]= 7;}
                    else if(nearrank[0][i]==mweight.u09.rank_no){nowrank[i]= 8;}
                    else if(nearrank[0][i]==mweight.u10.rank_no){nowrank[i]= 9;}
                    else if(nearrank[0][i]==10                 ){nowrank[i]=10;}
                    else{                                        nowrank[i]=99;}
                }
            } else if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i]) {
                iro[i]=C_NUSE;
            } else if(nearrank[0][i]!=0 && nearrank[0][i] != nearrank[1][i]) {
                iro[i]=C_NUSE;
            } else if(nearrank[0][i]!=0 && nearrank[1][i] !=0 && nearrank[0][i] != nearrank[1][i]) {
                iro[i]=C_NUSE;
            } else if(nearrank[0][i]!=0 && nearrank[0][i] != nearrank[1][i] && nearrank[1][i] != nearrank[2][i]) {
                iro[i]=C_NUSE;
            } else if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i]) {
                iro[i]=C_NUSE;
            } else {
            }
        }
        if(cel.anteiWeight_rank3(i)>param.CUT) {
            recrank[i]= nowrank[i];
        }
    }//loop終了

    delete[] weight;
    return com;

}
void drive_Rank_Sorting4()
{
    char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
                   ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼﾀ
    combiData_t resultCombi;        //組み合わせ結果(最小値取得用)
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    float baratsuki=param.BARATSUKI;//一時保存フォルダ
    int SerialNumber = 0;
    //param.BARATSUKI=param.BARATSUKI_R;
    res    = float(mweight.u01.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res2   = float(mweight.u02.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res3   = float(mweight.u03.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res4   = float(mweight.u04.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res5   = float(mweight.u05.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res6   = float(mweight.u06.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res7   = float(mweight.u07.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res8   = float(mweight.u08.rank_max);//*1.03;   //組み合わせ重量の上限値を取得
    res9   = float(mweight.u09.rank_max);//*1.03;   //組み合わせ重量の上限値を取得
    res10  = float(mweight.u10.rank_max);//*1.03;   //組み合わせ重量の上限値を取得
    rank1ave  = (float(mweight.u01.rank_max)+float(mweight.u02.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank2ave  = (float(mweight.u02.rank_max)+float(mweight.u03.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank3ave  = (float(mweight.u03.rank_max)+float(mweight.u04.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank4ave  = (float(mweight.u04.rank_max)+float(mweight.u05.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank5ave  = (float(mweight.u05.rank_max)+float(mweight.u06.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank6ave  = (float(mweight.u06.rank_max)+float(mweight.u07.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank7ave  = (float(mweight.u07.rank_max)+float(mweight.u08.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank8ave  = (float(mweight.u08.rank_max)+float(mweight.u09.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank9ave  = (float(mweight.u09.rank_max)+float(mweight.u10.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank1dif  = (float(mweight.u02.rank_max)+float(mweight.u01.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank2dif  = (float(mweight.u03.rank_max)+float(mweight.u02.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank3dif  = (float(mweight.u04.rank_max)+float(mweight.u03.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank4dif  = (float(mweight.u05.rank_max)+float(mweight.u04.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank5dif  = (float(mweight.u06.rank_max)+float(mweight.u05.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank6dif  = (float(mweight.u07.rank_max)+float(mweight.u06.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank7dif  = (float(mweight.u08.rank_max)+float(mweight.u07.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank8dif  = (float(mweight.u09.rank_max)+float(mweight.u08.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank9dif  = (float(mweight.u10.rank_max)+float(mweight.u09.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    //if(mweight.Rcolor==1){
    //    led_r=false;
    //}else{
    //    led_r=true;
    //}
    int btnset = 0;
    lcd.settings_rank2(&load);            //設定内容を表示
    for(int i = 0; i < product.head; i++) {
        nowrank[i]=0;
        recrank[i]=99;
    }
    for(int i = 0; i < 11; i++) {
        totalranker0103[i]=0;
        totalranker0406[i]=0;
        totalranker0709[i]=0;
        totalranker1012[i]=0;
        totalweight0103[i]=0;
        totalweight0406[i]=0;
        totalweight0709[i]=0;
        totalweight1012[i]=0;
        totalrankerA[i] = 0;
        totalweightA[i] = 0;
        totalrankerB[i] = 0;
        totalweightB[i] = 0;
        totalrankerC[i] = 0;
        totalweightC[i] = 0;
        totalrankerD[i] = 0;
        totalweightD[i] = 0;
    }

    //作業開始時間記録
    seconds = time(NULL);
    tm *dated3 = localtime(&seconds);
    run_date.tm_year = dated3->tm_year+1900;
    run_date.tm_sec  = dated3->tm_sec;
    run_date.tm_min  = dated3->tm_min;
    run_date.tm_hour = dated3->tm_hour;
    run_date.tm_mday = dated3->tm_mday;
    run_date.tm_mon  = dated3->tm_mon+1;
    run_date.tm_year = dated3->tm_year+1900;
    //

    if(product.use_usb==1 && option.usb_EN==true){
        //SerialNumber = USB_filecheck();
        SerialNumber = USB_filecheck3(run_date.tm_year,run_date.tm_mon,run_date.tm_mday,run_date.tm_hour,run_date.tm_min);
        if(SerialNumber<999){
            SerialNumber=0;
            /*lcd.blank(0);
            lcd.blank(1);
            lcd.locate(0,1);
            lcd.printf(SAVEEROK[1]);
            se.OK();    //警告音
            //wait(3);
            lcd.blank(0);
            lcd.blank(1);*/
        }
    }
    //led.RedOn();
    while(1) {
        //led.ledAllOff();            // LED消灯
        //led.RedOff();
        btn.new_keep_btn();               //スイッチ(New)取得
        

        //esc_swを押すと、Menu画面に戻る
        if(btn.esc()||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)) {
            se.Touch_01();
            comp_rank=0;
            //LCD表示
            lcd.blank(0);
            lcd.blank(1);
            lcd.locate(3,1);
            if(product.use_usb==1 && option.usb_EN==true) {
                 if(product.use_usb==1 && option.usb_EN==true) {
                    lcd.printf(saving[2]);
                }
                lcd.printf(saving[1]);
            }
            //戻りきるまで時間が掛かるので、マトリクス表示を消す
            for(int i = 0; i < product.head; i++) {
                iro[i]=C_NULL;
            }
            //個別データ書き込み(1個以上あったら)
            int total_qty=0;
            for(int i = 0; i < 11; i++) {
                total_qty += totalranker0103[i]+totalranker0406[i]+totalranker0709[i]+totalranker1012[i];
            }
            if(total_qty>0) {
                seconds = time(NULL);
                tm *dated4 = localtime(&seconds);
                int ji=0;
                int hun=0;
                ji = dated4->tm_hour - run_date.tm_hour;
                if(ji<0) {
                    ji += 24;
                }
                hun = dated4->tm_min - run_date.tm_min;
                if(hun<0) {
                    hun += 60;
                    ji -= 1;
                }
                if(product.use_usb==1 && option.usb_EN==true) {
                    save_rankdata(mweight.Rsort,ji,hun);
                }
            }

            for(int i = 0; i < product.head; i++) {
                oldweightVale[i] = 0;
                nowweightVale[i] = 0;
                nowweightVale2[i] = 0;
                nowweightVale3[i] = 0;
                nowweightVale4[i] = 0;
                recrank[i]=99;
            }
            param.BARATSUKI=baratsuki;
            file.read_param();          //設定値
            cel.set_BARATSUKI(param.BARATSUKI);
            lcd.blank(0);
            lcd.blank(1);
            for(int i = 0; i < product.head; i++) {
                nowrank[i]=0;
                totalweightN[i]=0;
                nearrank[3][i]=0;
                nearrank[2][i]=0;
                nearrank[1][i]=0;
                nearrank[0][i]=0;
            }
            for(int i = 0; i < 11; i++) {
                totalranker0103[i]=0;
                totalranker0406[i]=0;
                totalranker0709[i]=0;
                totalranker1012[i]=0;
                totalweight0103[i]=0;
                totalweight0406[i]=0;
                totalweight0709[i]=0;
                totalweight1012[i]=0;
                totalrankerA[i] = 0;
                totalweightA[i] = 0;
                totalrankerB[i] = 0;
                totalweightB[i] = 0;
                totalrankerC[i] = 0;
                totalweightC[i] = 0;
                totalrankerD[i] = 0;
                totalweightD[i] = 0;
            }
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            return;
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            auto_zero_point();
            lcd.settings_rank2(&load);            //設定内容を表示
        }
        //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        else if(btn.up_press() == true) {
            se.chime_01();
            /*
            while(btn.any_press()==true){
                if(demo_flg==true&&btn.down_press() == true&&file.get_preset()==product.head*2){
                    demo_continue=3;
                    btn.force_ESC();
                    se.Touch_01();
                    continue;
                }
                wait(0.1);
            }
            if(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2){
                    demo_continue=3;
                    btn.force_ESC();
                    continue;
            }
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            
            btnset = Chenge_vaule_btnD(99);//ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
            if(btnset==0) {
                if(multi_weight_set3(file.get_preset(),0 )==0){
                    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
                }
            } else if(btnset==1) {
                auto_zero_off = true;
                start_keiryo(0);
                auto_zero_off = false;
            }*/
            select_keiryo_option(0, load.kumi_flag, 0);
            lcd.settings_rank2(&load);            //設定内容を表示
        }
        //ﾓｼﾞ回転ON/OFF変更
        else if(btn.get_btn() == BTN_DOWN ) {
            se.Touch_01();
            //if(product.head >= 8) {
            if(product.Array == 0) {
                option.c_rotate =(option.c_rotate +1)%3;
            } else {
                option.c_rotate =(option.c_rotate +1)%2;
            }
        }

        btn.old_btn();       //スイッチ更新

        for(int i = 0; i < product.head; i++) {
            //setHeadVolt(i, 200);
            if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
                cel.set_head(i);
                cel.setAin_zero(i);
            } else {
                //cel.set_head(i);
                //cel.setAinSpeed(200);
                //param.BARATSUKI = cel.getWeight(i)*0.02;
                //cel.set_BARATSUKI(param.BARATSUKI);
                setHeadVolt(i, 600);
                //if(cel.anteiWeight_rank3(i)>param.CUT){//ｺｳｿｸでは取られたか確認できればいいので繰返し回数は少なめに
                if(nowrank[i]!=0) {
                    //setHeadVolt(i, param.KURI);
                    setHeadVolt(i, 700);
                    /*if(nowweightVale[i] - cel.anteiWeight(i)>1 && nowweightVale[i] - cel.anteiWeight(i)<-1){
                        setHeadVolt(i, param.KURI);
                    }*/
                } else if(cel.getWeight(i)>param.CUT) {
                    setHeadVolt(i, 800);
                    //setHeadVolt(i, (1000));
                }
                //pc.printf("%d %08.2f %08.2f %08.2f  %2d  %08.2f\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.anteiWeight_rank3(i),nowrank[i],param.BARATSUKI);
            }
        }

        //********     デバッグ用ここから
        if( product.productType >= STANDARD_D) {
            for(int i = 0; i < product.head; i++) {
                //temp = cel.temperature();   //温度を取得する
                //for(int i = 0; i < product.head; i++){
                cel.set_head(i);
                //    pc.printf("%d %08.2f %08.2f %08.2f  %2d\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.anteiWeight_rank3(i),nowrank[i]);
                pc.printf("%d %08.2f %08.2f %08.2f %2d %2d %08.2f\r\n", i+1, cel.getWeight(i), cel.get_HWeight(i), cel.anteiWeight_rank3(i),nowrank[i],iro[i],param.BARATSUKI);
                //}
                if(i == product.head - 1) {
                    //printf("%d %f C\r\n",14, temp);

                    pc.printf("========================\r\n");
                }
            }
        }
        //********デバッグ用ここまで

        //組み合わせ確認,最適な組み合わせ候補を抽出
        resultCombi = get_rank_location4();

        //組み合わせに応じたLED点灯,総重量表示
        show_rank2(&resultCombi);
    }//while
}

//ランク選別モード時に使用する関数
//指定範囲内の計量物の場所を取得する
//ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂ用なので、ﾗﾝｸで指定された値より3パーセント高い値まで、
//範囲内とする様、設定する(1.02)
combiData_t get_rank_location4()
{
    combiData_t com = {0};
    //printf("%4.0f:%4.0f:%4.0f:%4.0f:%4.0f:%4.0f:%4.0f\r\n",res,res2,res3,res4,res5,res6,res7);
    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)

    float *weight = new float[product.head];
    //安定している重量を取得
    for(int i = 0; i < product.head; i++) {
        //weight[i] = cel.anteiWeight_rank3(i);
        //weight[i] = cel.anteiWeight(i);
        weight[i] = cel.getWeight(i)+param.bufferT;
        if(weight[i] > 0) {
            //totalweightN[i]=weight[i];
        } else if(weight[i] == 0 && (cel.getWeight(i) < param.CUT+3 && oldweightVale[i]< param.CUT+3) && recrank[i]>=0 && recrank[i]<=10) { //安定が無くなって、現在の値が0付近になったら

            if(i<=2) {
                if(float((int(totalweightN[i])) >= param.CUT)){
                    totalweight0103[recrank[i]] += totalweightN[i];
                    totalranker0103[recrank[i]] += 1;
                }
            } else if(i<=5) {
                if(float((int(totalweightN[i])) >= param.CUT)){
                    totalweight0406[recrank[i]] += totalweightN[i];
                    totalranker0406[recrank[i]] += 1;
                }
            } else if(i<=8) {
                if(float((int(totalweightN[i])) >= param.CUT)){
                    totalweight0709[recrank[i]] += totalweightN[i];
                    totalranker0709[recrank[i]] += 1;
                }
            } else if(i<=11) {
                if(float((int(totalweightN[i])) >= param.CUT)){
                    totalweight1012[recrank[i]] += totalweightN[i];
                    totalranker1012[recrank[i]] += 1;
                }
            }
            totalweightN[i] = 0;
            recrank[i]      = 99;
        }
        if((weight[i] == 0 && cel.getWeight(i) > param.CUT && cel.getWeight(i) <= param.CUT)||cel.anteiWeight_rank(i)>=param.CUT) {
            //weight[i] = cel.anteiWeight_rank3(i);
            //weight[i] = cel.anteiWeight(i);
            weight[i] = cel.getWeight(i)+param.bufferT;
        }
        //if(i==0){
        //    pc.printf("%d %6.3f %6.3f  %6.3f  %d\r\n", i+1,cel.anteiWeight(i),cel.anteiWeight(i), weight[i],iro[i]);
        //}
        //if(weight[i]==0){
        //    weight[i] = cel.anteiWeight(i);
        //}
        int data_sa =oldweightVale[i] - nowweightVale[i];
        if(data_sa < -0.3 || data_sa > 0.4) {

        } else {
            nowweightVale[i] = oldweightVale[i];
            //weight[i] = oldweightVale[i];
        }
        oldweightVale[i] = nowweightVale[i];
    }
    //ヘッド数分の重量確認
    for(int i = 0; i < product.head; i++) {
        nowweightVale4[i] = nowweightVale3[i];
        nowweightVale3[i] = nowweightVale2[i];
        nowweightVale2[i] = weight[i];
//        if((weight[i]!=0 && nowrank[i]==99)||(iro[i]==C_NULL && nowrank[i]!=0)) {//一度ﾗﾝｸ確定したものは除外する(ｹｲﾘｮｳﾓｰﾄﾞに行った時はもう一度表示する)
        if(weight[i]>0) {//一度ﾗﾝｸ確定したものは除外する(ｹｲﾘｮｳﾓｰﾄﾞに行った時はもう一度表示する)
            //一番最適な重量を取得する
            if((int(weight[i]) > -param.CUT) && float((int(weight[i])) <= param.CUT)) {
                iro[i]=C_NULL;
                nowrank[i]=99;
                //値を取得
                //com.kumi += (1 << i);     //com.total += weight[i];
            } else if((float(int(weight[i])) > param.CUT) && (float(int(weight[i])) < float(res))) {
                if(mweight.u01.rank_en==1) {
                    iro[i]=C_BATU;
                    nowrank[i]=0;
                    //値を取得
                    //com.kumi += (1 << i);//com.total += weight[i];
                } else {
                    //iro[i]=C_NULL;
                    iro[i]=C_BATU;
                    nowrank[i]=0;
                }
            } else if((float(int(weight[i])) >= float(res)) && (float(int(weight[i])) < float(res2))) {
                if(mweight.u02.rank_en==1) {
                    iro[i]=mweight.u02.rank_no;
                    nowrank[i]=1;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res2)) && (float(int(weight[i])) < float(res3))) {
                if(mweight.u03.rank_en==1) {
                    iro[i]=mweight.u03.rank_no;
                    nowrank[i]=2;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res3)) && (float(int(weight[i])) < float(res4))) {
                if(mweight.u04.rank_en==1) {
                    iro[i]=mweight.u04.rank_no;
                    nowrank[i]=3;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res4)) && (float(int(weight[i])) < float(res5))) {
                if(mweight.u05.rank_en==1) {
                    iro[i]=mweight.u05.rank_no;
                    nowrank[i]=4;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res5)) && (float(int(weight[i])) < float(res6))) {
                if(mweight.u06.rank_en==1) {
                    iro[i]=mweight.u06.rank_no;
                    nowrank[i]=5;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res6)) && (float(int(weight[i])) < float(res7))) {
                if(mweight.u07.rank_en==1) {
                    iro[i]=mweight.u07.rank_no;
                    nowrank[i]=6;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res7)) && (float(int(weight[i])) < float(res8))) {
                if(mweight.u08.rank_en==1) {
                    iro[i]=mweight.u08.rank_no;
                    nowrank[i]=7;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res8)) && (float(int(weight[i])) < float(res9))) {
                if(mweight.u09.rank_en==1) {
                    iro[i]=mweight.u09.rank_no;
                    nowrank[i]=8;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if((float(int(weight[i])) >= float(res9)) && (float(int(weight[i])) < float(res10))) {
                if(mweight.u10.rank_en==1) {
                    iro[i]=mweight.u10.rank_no;
                    nowrank[i]=9;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
            } else if(float(int(weight[i])) >= float(res10)) {
                iro[i]=C_BATU;
                nowrank[i]=10;
                com.kumi += (1 << i);
                com.total += weight[i];
            } else {
                iro[i]=PENTA_R;
                nowrank[i]=10;
            }
            //}else if(cel.getWeight(i) > param.CUT && weight[i] < param.CUT) {
        } else if(weight[i]> param.CUT && nowrank[i]==0) {
            //iro[i]=C_NUSE;
            //pc.printf("%dc\r\n",i+1);
        } else if(weight[i]< param.CUT && nowrank[i]==0) {
            iro[i]=C_NULL;
            //}else if(nowrank[i]!=0 && nowweightVale2[i] < param.CUT+3 && nowweightVale3[i]< param.CUT+3) {
        } else if(nowrank[i]!=0 && nowweightVale2[i] < param.CUT+3) {
            setHeadVolt(i, 500);
            if(weight[i]< param.CUT) {
                iro[i]=C_NULL;
                nowrank[i]=99;
            }
            //pc.printf("%da\r\n",i+1);
        }


        //大体真ん中付近で重量が止まってたら、その値を確定とする
        //for(int m=0;m<4;m++){
        nearrank[1][i]=nearrank[0][i];
        nearrank[3][i]=nearrank[1][i];
        nearrank[2][i]=nearrank[1][i];
        nearrank[1][i]=nearrank[0][i];
        nearrank[0][i]=99;
        //}
        //if(nowrank[i]==0||(float(int(cel.getWeight(i))) > param.CUT)) {
        if(nowrank[i]==0||nowrank[i]>9) {
            if(float(int(weight[i]) > -param.CUT) && float(int(weight[i])<= param.CUT)) {
                nearrank[0][i]=99;
                //値を取得
                //com.kumi += (1 << i);
                //com.total += weight[i];
            } else if((float(int(weight[i])) > param.CUT) && (float(int(weight[i])) < float(res))) {
                if(mweight.u01.rank_en==1) {
                    nearrank[0][i]=0;
                    //nearrank[0][i]=0;
                } else {//iro[i]=C_BATU;nowrank[i]=0;
                    nearrank[0][i]=0;
                    //nearrank[0][i]=0;
                }
            } else if((float(int(weight[i])) >= float(rank1ave-rank1dif)) && (float(int(weight[i])) < float(res2))) {
                if(mweight.u02.rank_en==1) {
                    nearrank[0][i]=mweight.u02.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(weight[i])) >= float(rank2ave-rank2dif)) && (float(int(weight[i])) < float(res3))) {
                if(mweight.u03.rank_en==1) {
                    nearrank[0][i]=mweight.u03.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(weight[i])) >= float(rank3ave-rank3dif)) && (float(int(weight[i])) < float(res4))) {
                if(mweight.u04.rank_en==1) {
                    nearrank[0][i]=mweight.u04.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(weight[i])) >= float(rank4ave-rank4dif)) && (float(int(weight[i])) < float(res5))) {
                if(mweight.u05.rank_en==1) {
                    nearrank[0][i]=mweight.u05.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(weight[i])) >= float(rank5ave-rank5dif)) && (float(int(weight[i])) < float(res6))) {
                if(mweight.u06.rank_en==1) {
                    nearrank[0][i]=mweight.u06.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(weight[i])) >= float(rank6ave-rank6dif)) && (float(int(weight[i])) < float(res7))) {
                if(mweight.u07.rank_en==1) {
                    nearrank[0][i]=mweight.u07.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(weight[i])) >= float(rank7ave-rank7dif)) && (float(int(weight[i])) < float(res8))) {
                if(mweight.u08.rank_en==1) {
                    nearrank[0][i]=mweight.u08.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(weight[i])) >= float(rank8ave-rank8dif)) && (float(int(weight[i])) < float(res9))) {
                if(mweight.u09.rank_en==1) {
                    nearrank[0][i]=mweight.u09.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if((float(int(weight[i])) >= float(rank9ave-rank9dif)) && (float(int(weight[i])) < float(res10))) {
                if(mweight.u10.rank_en==1) {
                    nearrank[0][i]=mweight.u10.rank_no;
                } else {//iro[i]=C_NULL;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
            } else if(float(int(weight[i])) >= float(res10)) {
                //nearrank[0][i]=C_BATU;
                nearrank[0][i]=10;
            } else {//iro[i]=PENTA_R;nowrank[i]=0;
                nearrank[0][i]=99;
            }
            nearrank[1][i]=nearrank[0][i];
            nearrank[3][i]=nearrank[1][i];
            nearrank[2][i]=nearrank[1][i];
            //連続で同じﾗﾝｸだった場合、(上の設定値が低めギリギリを除外してるので)ﾗﾝｸとしては確証がもてるので、表示を変える
            //if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i] && nearrank[1][i] == nearrank[2][i] && nearrank[2][i] == nearrank[3][i]){
            if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i] && nearrank[1][i] == nearrank[2][i] && nearrank[1][i] == nearrank[3][i]) {
                if(nearrank[0][i]==0||nearrank[0][i]==10){
                    iro[i] =C_BATU;
                }else{
                    iro[i]=nearrank[0][i];
                }
                if(     nearrank[0][i]==0                  ){nowrank[i]= 0;}
                else if(nearrank[0][i]==mweight.u02.rank_no){nowrank[i]= 1;}
                else if(nearrank[0][i]==mweight.u03.rank_no){nowrank[i]= 2;}
                else if(nearrank[0][i]==mweight.u04.rank_no){nowrank[i]= 3;}
                else if(nearrank[0][i]==mweight.u05.rank_no){nowrank[i]= 4;}
                else if(nearrank[0][i]==mweight.u06.rank_no){nowrank[i]= 5;}
                else if(nearrank[0][i]==mweight.u07.rank_no){nowrank[i]= 6;}
                else if(nearrank[0][i]==mweight.u08.rank_no){nowrank[i]= 7;}
                else if(nearrank[0][i]==mweight.u09.rank_no){nowrank[i]= 8;}
                else if(nearrank[0][i]==mweight.u10.rank_no){nowrank[i]= 9;}
                else if(nearrank[0][i]==10                 ){nowrank[i]=10;}
                else{                                        nowrank[i]=99;}
                
                
                //nowrank[i]=nearrank[0][i];
                if(weight[i] != 0) {
                    //comp_rank = comp_rank | 1<<i;
                } else {
                    //comp_rank = comp_rank & ~(1<<i);
                }
                /*for(int m=0;m<4;m++){
                    nearrank[3][i]=0;
                    nearrank[2][i]=0;
                    nearrank[1][i]=0;
                    nearrank[0][i]=0;
                }*/
                /*if(i==3||i==2){
                    printf("%dd\r\n",i+1);
                }*/
            } else if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i] && nearrank[1][i] == nearrank[2][i]) {
                if(nearrank[0][i]==0||nearrank[0][i]==10){
                    iro[i] =C_BATU;
                    nowrank[i]=nearrank[0][i];
                }else{
                    iro[i]=nearrank[0][i];
                    if(     nearrank[0][i]==0                  ){nowrank[i]= 0;}
                    else if(nearrank[0][i]==mweight.u02.rank_no){nowrank[i]= 1;}
                    else if(nearrank[0][i]==mweight.u03.rank_no){nowrank[i]= 2;}
                    else if(nearrank[0][i]==mweight.u04.rank_no){nowrank[i]= 3;}
                    else if(nearrank[0][i]==mweight.u05.rank_no){nowrank[i]= 4;}
                    else if(nearrank[0][i]==mweight.u06.rank_no){nowrank[i]= 5;}
                    else if(nearrank[0][i]==mweight.u07.rank_no){nowrank[i]= 6;}
                    else if(nearrank[0][i]==mweight.u08.rank_no){nowrank[i]= 7;}
                    else if(nearrank[0][i]==mweight.u09.rank_no){nowrank[i]= 8;}
                    else if(nearrank[0][i]==mweight.u10.rank_no){nowrank[i]= 9;}
                    else if(nearrank[0][i]==10                 ){nowrank[i]=10;}
                    else{                                        nowrank[i]=99;}
                }
            } else if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i]) {
                iro[i]=C_NUSE;
            } else if(nearrank[0][i]!=0 && nearrank[0][i] != nearrank[1][i]) {
                iro[i]=C_NUSE;
            } else if(nearrank[0][i]!=0 && nearrank[1][i] !=0 && nearrank[0][i] != nearrank[1][i]) {
                iro[i]=C_NUSE;
            } else if(nearrank[0][i]!=0 && nearrank[0][i] != nearrank[1][i] && nearrank[1][i] != nearrank[2][i]) {
                iro[i]=C_NUSE;
            } else if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i]) {
                iro[i]=C_NUSE;
            } else {
            }
        }
        if(cel.anteiWeight_rank3(i)>param.CUT) {
            recrank[i]= nowrank[i];
        }
    }//loop終了

    delete[] weight;
    return com;

}
//各ヘッド毎に、目標重量を計測したらLEDを点滅させる
//ｺｳｿｸﾓｰﾄﾞ
void drive_kosuu()
{
    char saving[]= {0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,0x00}; //ﾎｿﾞﾝﾁｭｳ
    combiData_t resultCombi;        //組み合わせ結果(最小値取得用)
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    float baratsuki=param.BARATSUKI;//一時保存フォルダ
    int btnset =0;//計量中の下ボタン押しで出てくる値保存
    int OK_head=0; //iro[i]=C_MARUなものを集めて排出
    int OK_head_b=0; //1個前iro[i]=C_MARUなものを集めて排出
    param.BARATSUKI=param.BARATSUKI_R;
    param.ZEROAREA=param.ZEROAREA2;
    cel.set_ZEROAREA(param.ZEROAREA);
    //param.BARATSUKI=param.BARATSUKI_R;
    if(product.head>1) {
        lcd.settings_rank2(&load);            //設定内容を表示
    } else {
        lcd.blank(0);
        lcd.blank(1);
    }
    for(int i = 0; i < product.head; i++) {
        nowrank[i]=99;
        totalweightN[i]=0;
    }
    for(int i = 0; i < product.head; i++) {
        totalweightN[i]=0;
        MARU_counter[i]=0;
        MARU_lit_counter[i]=0;
    }
    for(int i = 0; i < 10; i++) {
        totalranker0103[i]=0;
        totalranker0406[i]=0;
        totalranker0709[i]=0;
        totalranker1012[i]=0;
        totalweight0103[i]=0;
        totalweight0406[i]=0;
        totalweight0709[i]=0;
        totalweight1012[i]=0;
        totalrankerA[i] = 0;
        totalweightA[i] = 0;
        totalrankerB[i] = 0;
        totalweightB[i] = 0;
    }

    //作業開始時間記録
    seconds = time(NULL);
    tm *dated3 = localtime(&seconds);
    run_date.tm_year = dated3->tm_year+1900;
    run_date.tm_sec  = dated3->tm_sec;
    run_date.tm_min  = dated3->tm_min;
    run_date.tm_hour = dated3->tm_hour;
    run_date.tm_mday = dated3->tm_mday;
    run_date.tm_mon  = dated3->tm_mon+1;
    run_date.tm_year = dated3->tm_year+1900;
    //

    if(servo_connection==true){
        send_i(EJECT_RESET, 11+load.kumi_flag, 0);
    }
          
    //led.RedOn();
    while(1) {
        //led.ledAllOff();            // LED消灯
        //led.RedOff();
        btn.new_keep_btn();               //スイッチ(New)取得
        //esc_swを押すと、Menu画面に戻る
        if(btn.esc()||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)) {
            se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();
            for(int header=0; header<product.head; header++){
                send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
            }
            wait(0.5);
            for(int header=0; header<product.head; header++){
                send_i(SERVO_1 ,header,servo_span[header][0]); wait_res();
            }
            comp_rank=0;
            //LCD表示
            lcd.blank(0);
            lcd.blank(1);
            lcd.locate(3,1);
            if(product.use_usb==1 && option.usb_EN==true) {
                lcd.printf(saving);
            }
            //戻りきるまで時間が掛かるので、マトリクス表示を消す
            for(int i = 0; i < product.head; i++) {
                iro[i]=C_NULL;
            }
            //個別データ書き込み(1個以上あったら)
            int total_qty=0;
            for(int i = 0; i < 10; i++) {
                total_qty += totalranker0103[i]+totalranker0406[i]+totalranker0709[i]+totalranker1012[i];
            }
            if(total_qty>0) {
                seconds = time(NULL);
                tm *dated4 = localtime(&seconds);
                int ji=0;
                int hun=0;
                ji = dated4->tm_hour - run_date.tm_hour;
                if(ji<0) {
                    ji += 24;
                }
                hun = dated4->tm_min - run_date.tm_min;
                if(hun<0) {
                    hun += 60;
                    ji -= 1;
                }
                if(product.use_usb==1 && option.usb_EN==true) {
                    save_rankdata2(mweight.Rsort,load.target,load.jougen,ji,hun);
                }
                //save_rankdata(mweight.Rsort);
                if(product.use_usb==1 && option.usb_EN==true) {
                    USB_rankdata2(mweight.Rsort,load.target,load.jougen,ji,hun);
                }
            }

            for(int i = 0; i < product.head; i++) {
                oldweightVale[i] = 0;
                nowweightVale[i] = 0;
                nowweightVale2[i] = 0;
                nowweightVale3[i] = 0;
                nowweightVale4[i] = 0;
            }
            param.BARATSUKI=baratsuki;
            file.read_param();          //設定値
            cel.set_BARATSUKI(param.BARATSUKI);
            cel.set_ZEROAREA(param.ZEROAREA);
            lcd.blank(0);
            lcd.blank(1);
            for(int i = 0; i < product.head; i++) {
                nowrank[i]=99;
                totalweightN[i]=0;
            }
            for(int i = 0; i < 10; i++) {
                totalranker0103[i]=0;
                totalranker0406[i]=0;
                totalranker0709[i]=0;
                totalranker1012[i]=0;
                totalweight0103[i]=0;
                totalweight0406[i]=0;
                totalweight0709[i]=0;
                totalweight1012[i]=0;
                totalrankerA[i] = 0;
                totalweightA[i] = 0;
                totalrankerB[i] = 0;
                totalweightB[i] = 0;
            }
            if(servo_connection==true){
                //moving2=99;
                send_i(EJECT_RESET, 0, 0);
                loop_a = 0;
                while(check_signal()==false&&loop_a<=10){
                    loop_a+=1;
                    wait(0.1);
                }
            }
            return;
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            for(int header=0; header<product.head; header++){
                send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
            }
            wait(1);
            auto_zero_point();                  //ゼロ点取得
            for(int header=0; header<product.head; header++){
                send_i(SERVO_1 ,header,servo_span[header][0]); wait_res();
            }
            if(product.head>1) {
                lcd.settings_rank2(&load);            //設定内容を表示
            } else {
                lcd.blank(0);
                lcd.blank(1);
            }
        }
        //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        else if(btn.up_press() == true) {
            se.chime_01();
            /*
            while(btn.any_press()==true){
                if(demo_flg==true&&btn.down_press() == true&&file.get_preset()==product.head*2){
                    demo_continue=1;
                    btn.force_ESC();
                    se.Touch_01();
                    continue;
                }
                wait(0.1);
            }
            if(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2){
                    demo_continue=1;
                    btn.force_ESC();
                    continue;
            }
            
            //btn.old_btn();       //スイッチ更新
            //btn.new_btn();       //スイッチ更新
            //auto_zero_off = true;
            //start_keiryo(0);
            //auto_zero_off = false;
            btnset = Chenge_vaule_btnD(0);//ｶｳﾝﾄ以外orｹｲﾘｮｳﾓｰﾄﾞ
            if(btnset==1) {
                auto_zero_off = true;
                start_keiryo(0);
                auto_zero_off = false;
            }else if(btnset==2){
                set_sercret_parameter_one(P_KURI_J,1);
                //set_sercret_parameter_one(P_END,1); //保存指令付
            }else if(btnset==3){
                set_sercret_parameter_one(P_ZEROAREA2,1);
            }else if(btnset==4){
                set_sercret_parameter_one(P_bufferT,1);
                
                
                //set_sercret_parameter_one(P_END,1); //保存指令付
            }
            */
            select_keiryo_option(0, load.kumi_flag, 0);
            if(product.head>1) {
                lcd.settings_rank2(&load);            //設定内容を表示
            } else {
                lcd.blank(0);
                lcd.blank(1);
                lcd.bigNumber(cel.getWeight(0));
            }
        }
        //ﾓｼﾞ回転ON/OFF変更
        else if(btn.get_btn() == BTN_DOWN ) {
            se.Touch_01();
            //if(product.head >= 8) {
            if(product.Array == 0) {
                option.c_rotate =(option.c_rotate +1)%3;
            } else {
                option.c_rotate =(option.c_rotate +1)%2;
            }
        }

        btn.old_btn();       //スイッチ更新

        for(int i = 0; i < product.head; i++) {
            if(product.head<=13) {
                //if(product.head<=6 || (i<=5 && i % 2 == 0) || (i>=6 && i % 2 == 1)){
                //setHeadVolt(i, 200);
                if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
                    cel.set_head(i);
                    cel.setAin_zero(i);
                } else {
                    //cel.set_head(i);
                    //cel.setAinSpeed(200);
                    if(cel.getWeight(i)<80) {
                        if(param.BARATSUKI_GRAM<cel.getWeight(i)*0.015) {
                            param.BARATSUKI = cel.getWeight(i)*0.015;
                        } else {
                            param.BARATSUKI = param.BARATSUKI_GRAM;
                        }
                    }/**/else if(((load.target*0.98)<=cel.getWeight(i)) && (cel.getWeight(i)<=(load.target*1.02)) ||
                                 (((load.target + load.jougen)*0.98)<=cel.getWeight(i)) && (cel.getWeight(i)<=((load.target + load.jougen)*1.02)) ) {
                        if(param.BARATSUKI_GRAM<cel.getWeight(i)*0.005) {
                            param.BARATSUKI = cel.getWeight(i)*0.005;
                        } else {
                            param.BARATSUKI = param.BARATSUKI_GRAM;
                        }
                    }/**/else {
                        if(param.BARATSUKI_GRAM<cel.getWeight(i)*0.01) {
                            param.BARATSUKI = cel.getWeight(i)*0.01;
                        } else {
                            param.BARATSUKI = param.BARATSUKI_GRAM;
                        }
                    }
                    //if(product.head>1){
                    //    param.BARATSUKI=param.BARATSUKI_R;
                    //}
                    cel.set_BARATSUKI(param.BARATSUKI);
                    if(product.head!=1){
                        setHeadVolt(i, 500);
                    }
                    //printf("%8.3f\r\n",param.BARATSUKI);
                    if(nowrank[i]!=0) {
                        //setHeadVolt(i, param.KURI);
                        setHeadVolt(i, 500);
                        /*if(nowweightVale[i] - cel.anteiWeight(i)>1 && nowweightVale[i] - cel.anteiWeight(i)<-1){
                            setHeadVolt(i, param.KURI);
                        }*/
                    } else if(cel.getWeight(i)>param.CUT) {
                        if(product.head==1){
                            setHeadVolt(i, 3000);
                        }else{
                            setHeadVolt(i, 1500);
                        }
                        //setHeadVolt(i, (1000));
                    } else{
                        if(product.head==1){
                            setHeadVolt(i, 500);
                        }
                    }
                    //pc.printf("%d %08.2f %08.2f %08.2f  %2d  %08.2f\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.anteiWeight_rank3(i),nowrank[i],param.BARATSUKI);
                }
            }
        }

        //********     デバッグ用ここから
        if( product.productType >= STANDARD_D) {
            for(int i = 0; i < product.head; i++) {
                //temp = cel.temperature();   //温度を取得する
                //for(int i = 0; i < product.head; i++){
                cel.set_head(i);
                //    pc.printf("%d %08.2f %08.2f %08.2f  %2d\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.anteiWeight_rank3(i),nowrank[i]);
                pc.printf("%d %08.2f %08.2f %08.2f %2d %2d %08.2f\r\n", i+1, cel.getWeight(i), cel.get_HWeight(i), cel.anteiWeight_rank3(i),nowrank[i],iro[i],param.BARATSUKI);
                //}
                if(i == product.head - 1) {
                    //printf("%d %f C\r\n",14, temp);

                    pc.printf("========================\r\n");
                }
            }
        }
        //********デバッグ用ここまで
        //組み合わせ確認,最適な組み合わせ候補を抽出
        if(load.kumi_flag == KUMI_KOSUU){
            //resultCombi = get_juuryo_location();
            resultCombi = get_check_location();
        }else if(load.kumi_flag == KUMI_JUURYO){
            //resultCombi = get_check_location();
            resultCombi = get_juuryo_location();
        }

        //組み合わせに応じたLED点灯,総重量表示
        //show_rank2(&resultCombi);

        if(product.head>1) {
            show_check(&resultCombi);
        }
        
        if(servo_connection==true){
            
            signal_flg=false;
            signal_loop =0;
            send_i(ON_CHECK,0,0);
            while(check_signal()==false&&signal_loop<=10){
                signal_loop+=1;
                wait(0.1);
            }
            
            
            for(int i = 0; i < product.head; i++) {
                if(iro[i]==C_MARU){
                    if(((1 << i) & moving1) == 0){
                        send_i(SERVO_ON,1 << i,0);while(check_signal()==false){wait(0.1);}
                    }
                }
            }
        }
        
    }//while
}

combiData_t get_check_location()
{
    combiData_t com = {0};
    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)
    float *weight = new float[product.head];
    //安定している重量を取得
    for(int i = 0; i < product.head; i++) {
        if(product.head<=13) {
            //if(product.head<=6 || (i<=5 && i % 2 == 0) || (i>=6 && i % 2 == 1)){
            //weight[i] = cel.anteiWeight(i);
            weight[i] = int(cel.getWeight(i)+0.5-param.bufferT);
            //printf("%3d %3d %7.1f %7.1f\r\n",recrank[i],iro[i],weight[i],int(cel.getWeight(i)) );
            if(weight[i] > 0) {
                totalweightN[i]=weight[i];
            } else if(weight[i] == 0 && (int(cel.getWeight(i)) < param.CUT+3 && oldweightVale[i]< param.CUT+3) && ((product.head !=1 && recrank[i]>0) || (product.head ==1 && recrank[i]>=0)) && recrank[i]<8) { //安定が無くなって、現在の値が0付近になったら
                //printf("K");
                if(i<=2) {
                    totalweight0103[recrank[i]] += totalweightN[i];
                    totalranker0103[recrank[i]] += 1;
                } else if(i<=5) {
                    totalweight0406[recrank[i]] += totalweightN[i];
                    totalranker0406[recrank[i]] += 1;
                } else if(i<=8) {
                    totalweight0709[recrank[i]] += totalweightN[i];
                    totalranker0709[recrank[i]] += 1;
                } else if(i<=11) {
                    totalweight1012[recrank[i]] += totalweightN[i];
                    totalranker1012[recrank[i]] += 1;
                }
                oneshoot_OK=false;
                totalweightN[i] = 0;
                recrank[i]      = 0;
            }
            //if((weight[i] == 0 && int(cel.getWeight(i)) > param.CUT && int(cel.getWeight(i)) <= param.CUT)||cel.anteiWeight_rank(i)>=param.CUT) {
            if((weight[i] == 0 && int(cel.getWeight(i)) > param.CUT)){// && int(cel.getWeight(i)) <= param.CUT)||cel.anteiWeight_rank(i)>=param.CUT) {
                weight[i] = cel.anteiWeight(i);
            }
            //printf("g:%5.1f a:%5.1f w:%5.1f  wo:%5.1f\r\n",int(cel.getWeight(i)),cel.anteiWeight(i),weight[i],wait_old[i]);
            //printf("g:%5.1f a:%5.1f w:%5.1f  wo:%5.1f",int(cel.getWeight(i)),cel.anteiWeight(i),weight[i],wait_old[i]);
            //printf(" %3d %3d d:%3d ",int(weight[i]),int(wait_old[i]),(int(weight[i])-int(wait_old[i])));
  //          if(((int(weight[i])-int(wait_old[i]))<0) || ((int(weight[i])-int(wait_old[i]))>1)){
  //              wait_old[i] = weight[i];
  //          }else{
  //              weight[i] = wait_old[i];
  //          }
            wait_old[i] = weight[i];
            
            //printf("  w:%5.1f\r\n",weight[i]);
            
            //printf("%5.1f  %5.1f  ",int(cel.getWeight(i)),cel.anteiWeight_rank(i));
            //printf("%3d  ",int(weight[i]));
            //if(i==0){
            //    printf("%d %6.3f %6.3f  %6.3f  %d\r\n", i+1,cel.anteiWeight(i),cel.anteiWeight(i), weight[i],iro[i]);
            //}
            //if(weight[i]==0){
            //    weight[i] = cel.anteiWeight(i);
            //}
            int data_sa =oldweightVale[i] - nowweightVale[i];
            if(data_sa < -0.3 || data_sa > 0.4) {

            } else {
                nowweightVale[i] = oldweightVale[i];
                //weight[i] = oldweightVale[i];
            }
            oldweightVale[i] = nowweightVale[i];
        } else {
            iro[i] = C_BATU;
            //iro[i] = CR_1+i;
        }
    }
    //ヘッド数分の重量確認
    for(int i = 0; i < product.head; i++) {
           // printf("%3d  ",int(weight[i]));
        if(product.head<=13) {
            //if(product.head<=6 || (i<=5 && i % 2 == 0) || (i>=6 && i % 2 == 1)){
            if( int(cel.getWeight(i)) < -(param.CUT*2)) {//上に押されすぎたら、一旦0点調整してもらう
            } else {
                headqty=1;
                if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                    if(product.head==1) {
                        totalcheckwei[i]=weight[i];
                        if(disp_wait!=weight[i]){
                            lcd.bigNumber(weight[i]);
                            disp_wait = weight[i];
                        }
                    }
                }
                headqty=0;
            }
            //printf("w %5.1f\r\n",weight[i]);
            if(/*cel.anteiWeight_rank(i)==0 || */int(cel.getWeight(i)) <= param.CUT){
                iro[i]=C_NUSE;
                MARU_counter[i]=0;
                nowrank[i]=99;
                weight_stack[i] = 0;
                //printf("w1 %5d\r\n",iro[i]);
            }else if(weight[i]!=0) {
                //一番最適な重量を取得する
                if((int(weight[i]) > -param.CUT) && (int(weight[i]) <= param.CUT)) {
                    //値を取得
                    //com.kumi += (1 << i);
                    //com.total += weight[i];
                
                    MARU_counter[i]=0;
                    MARU_lit_counter[i]=0;
                    if(product.head!=1) {
                        iro[i]=PENTA_R;
                    } else {
                        headqty=1;
                        if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                            if(product.head==1) {
                                totalcheckwei[i]=weight[i];
                                if(disp_wait!=weight[i]){
                                    lcd.bigNumber(weight[i]);
                                    disp_wait = weight[i];
                                }
                            }
                        }
                        headqty=0;
                        //iro[i]=C_NULL;
                        //nowrank[i]=1;
                    }
                    nowrank[i]=99;
                    weight_stack[i] = 0;
                } else if((int(weight[i]) > param.CUT) && (int(weight[i]) < int(load.target))) {
                    //iro[i]=C_NUSE;
                    nowrank[i]=1;
                    headqty=1;
                    if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                        if(product.head==1) {
                            totalcheckwei[i]=weight[i];
                            if(disp_wait!=weight[i]){
                                lcd.bigNumber(weight[i]);
                                disp_wait = weight[i];
                            }
                        }
                    }
                    headqty=0;
                    if(product.head==1) {
                        oneshoot_OK=false;
                    }
                    if(load.target-weight[i]<load.tanjuu*99){
                        //iro[i]= CG_00+(load.target-weight[i]) + 1;
                        
                        MARU_counter[i]=0;    MARU_counter[i]=0;
                        if(MARU_lit_counter[i]>0){
                            MARU_lit_counter[i] -= 1;
                            iro[i] = C_MARU;
                        }else{
                            iro[i]= CR_01+((load.target-weight[i])/load.tanjuu) /*+ 1*/;
                            //printf("w1 %5d\r\n",iro[i]);
                        }

                    
                    }else if(load.target-weight[i]<load.tanjuu*199){
                        
                        MARU_counter[i]=0;
                        MARU_lit_counter[i] = 0;
                        iro[i]= C_NUSE_1D;
                    }else{
                        MARU_counter[i]=0;
                        MARU_lit_counter[i] = 0;
                        iro[i]= C_NUSE_2D;
                    }
                    weight_stack[i] = 0;
               // } else if(((int(weight[i]) >= int(load.target)) && (int(weight[i]*0.998) < int(load.target + load.jougen)))) {
                } else if(((int(weight[i]) >= int(load.target)) && (int(weight[i]) <= int(load.target + load.jougen)))) {
                    //iro[i]=CG_00;
                    if(option.rank_speed == RANK_FAST){
                        weight_stack[i] += 3;
                    }else{
                        weight_stack[i] += 1;
                    }
                    if(weight_stack[i]>=3){
                        iro[i]=C_MARU;
                        weight_stack[i] = 3;
                    }else{
                        iro[i]= C_NUSE_1D;
                    }
                    headqty=1;
                    if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                        if(product.head==1) {
                            totalcheckwei[i]=int(cel.getWeight(i));
                            lcd.bigNumber(int(cel.getWeight(i)));
                        }
                    }
                    headqty=0;
                    nowrank[i]      = 2;
                    //}
                //} else if(((int(weight[i]) >= int(load.target)))) {
                } else if(((int(weight[i]) >= int(load.target + load.jougen)))) {
                    if(product.head==1) {
                        oneshoot_OK=false;
                    }


                    //if((weight[i]-(load.target))<100){
                    if((weight[i]-(load.target + load.jougen))<load.tanjuu*99){
                        //iro[i]= CR_00+(weight[i]-(load.target));
                        //iro[i]= CR_00+(weight[i]-(load.target + load.jougen));
                        iro[i]= CG_01+((weight[i]-(load.target + load.jougen))/load.tanjuu);
                        nowrank[i]      = 2;
                        //printf("w2 %5d\r\n",iro[i]);
                    }else{
                        iro[i]=C_BATU;
                        if(product.head==1) {
                            se.mistake();
                            wait(1);
                        }
                    }
                    weight_stack[i] = 0;


                } else {
                    iro[i]=C_BATU;
                    weight_stack[i] = 0;
                }
            } else if(int(cel.getWeight(i)) < param.CUT+3 && oldweightVale[i]< param.CUT+3) {
                iro[i]=C_NUSE;
                weight_stack[i] = 0;
            }
            if(cel.anteiWeight_rank(i)>0) {
                recrank[i]= nowrank[i];
            }
        }
    }//loop終了

    delete[] weight;
    return com;

}

void drive_juuryo()
{
    //printf("A01_juuryou\r\n");
    char saving[]= {0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,0x00}; //ﾎｿﾞﾝﾁｭｳ
    combiData_t resultCombi;        //組み合わせ結果(最小値取得用)
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    float baratsuki=param.BARATSUKI;//一時保存フォルダ
    int btnset =0;//計量中の下ボタン押しで出てくる値保存
    int OK_head=0; //iro[i]=C_MARUなものを集めて排出
    int OK_head_b=0; //1個前iro[i]=C_MARUなものを集めて排出
    
    cel.set_BARATSUKI(param.BARATSUKI_R);
    //param.BARATSUKI=param.BARATSUKI_R;
    param.ZEROAREA=param.ZEROAREA2;
    cel.set_ZEROAREA(param.ZEROAREA);
    file.read_span_preset(file.get_preset());
    //if(span_preset[0][0] != 0.0){
    //    if(product.limit_on == NIRA_LIMIT){
    //        //file.read_span_preset(file.get_preset());
    //        cel.set_span_preset();
    //        cel.set_fainV(span_preset[0][0]);      //現在のロードセルのFAIN値を取得
    //        cel.set_sainV(span_preset[0][1]);      //現在のロードセルのSAIN値を取得
    //        cel.set_sainVs(0.0           ,2);      //現在のロードセルのSAIN値を取得
    //        cel.set_sainVs(0.0           ,3);      //現在のロードセルのSAIN値を取得
    //        cel.set_sainVs(0.0           ,4);      //現在のロードセルのSAIN値を取得
    //        cel.set_sainVs(0.0           ,5);      //現在のロードセルのSAIN値を取得
    //        cel.set_sainVs(0.0           ,6);      //現在のロードセルのSAIN値を取得
    //        cel.set_sainVs(0.0           ,7);      //現在のロードセルのSAIN値を取得
    //        cel.set_sainVs(0.0           ,8);      //現在のロードセルのSAIN値を取得
    //        cel.set_sainVs(0.0           ,9);      //現在のロードセルのSAIN値を取得
    //        cel.set_sainVs(0.0           ,10);      //現在のロードセルのSAIN値を取得
    //    }
    //}

    //printf("A02\r\n");
    if(product.head>1) {
        lcd.settings_rank2(&load);            //設定内容を表示
    } else {
        lcd.blank(0);
        lcd.blank(1);
    }
    //printf("A03\r\n");
    for(int i = 0; i < product.head; i++) {
        nowrank[i]=99;
        totalweightN[i]=0;
        MARU_counter[i]=0;
        MARU_lit_counter[i]=0;
    }
    for(int i = 0; i < 10; i++) {
        totalranker0103[i]=0;
        totalranker0406[i]=0;
        totalranker0709[i]=0;
        totalranker1012[i]=0;
        totalweight0103[i]=0;
        totalweight0406[i]=0;
        totalweight0709[i]=0;
        totalweight1012[i]=0;
        totalrankerA[i] = 0;
        totalweightA[i] = 0;
        totalrankerB[i] = 0;
        totalweightB[i] = 0;
        
    }

    //作業開始時間記録
    seconds = time(NULL);
    tm *dated3 = localtime(&seconds);
    run_date.tm_year = dated3->tm_year+1900;
    run_date.tm_sec  = dated3->tm_sec;
    run_date.tm_min  = dated3->tm_min;
    run_date.tm_hour = dated3->tm_hour;
    run_date.tm_mday = dated3->tm_mday;
    run_date.tm_mon  = dated3->tm_mon+1;
    run_date.tm_year = dated3->tm_year+1900;
    //
    

    //printf("A04\r\n");
    if(servo_connection==true){
        //send_i(EJECT_RESET, 11+load.kumi_flag, 0);
        send_i(EJECT_RESET, 11+KUMI_NONE, 0);
    }
    //printf("A05\r\n");
            
    //led.RedOn();
    while(1) {
        //led.ledAllOff();            // LED消灯
        //led.RedOff();
        btn.new_keep_btn();               //スイッチ(New)取得
        //esc_swを押すと、Menu画面に戻る
        if(btn.esc()||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)) {
            se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();
            if(product.matrix_mode == MATRIX_ON) {
                for(int l=0;l<product.head;l++){
                    iro[l]=C_NULL;
                }
            }
            if(demo_flg==false){
                if(servo_connection==true){
                    for(int header=0; header<product.head; header++){
                        send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
                    }
                    wait(0.5);
                    for(int header=0; header<product.head; header++){
                        send_i(SERVO_1 ,header,servo_span[header][0]); wait_res();
                    }
                }
            }
            comp_rank=0;
            //LCD表示
            lcd.blank(0);
            lcd.blank(1);
            lcd.locate(3,1);
            if(product.use_usb==1 && option.usb_EN==true) {
                lcd.printf(saving);
            }
            //戻りきるまで時間が掛かるので、マトリクス表示を消す
            for(int i = 0; i < product.head; i++) {
                iro[i]=C_NULL;
            }
            //個別データ書き込み(1個以上あったら)
            int total_qty=0;
            for(int i = 0; i < 10; i++) {
                total_qty += totalranker0103[i]+totalranker0406[i]+totalranker0709[i]+totalranker1012[i];
            }
            if(total_qty>0) {
                seconds = time(NULL);
                tm *dated4 = localtime(&seconds);
                int ji=0;
                int hun=0;
                ji = dated4->tm_hour - run_date.tm_hour;
                if(ji<0) {
                    ji += 24;
                }
                hun = dated4->tm_min - run_date.tm_min;
                if(hun<0) {
                    hun += 60;
                    ji -= 1;
                }
                if(product.use_usb==1 && option.usb_EN==true) {
                    save_rankdata2(mweight.Rsort,load.target,load.jougen,ji,hun);
                }
                //save_rankdata(mweight.Rsort);
                if(product.use_usb==1 && option.usb_EN==true) {
                    USB_rankdata2(mweight.Rsort,load.target,load.jougen,ji,hun);
                }
            }

            for(int i = 0; i < product.head; i++) {
                oldweightVale[i] = 0;
                nowweightVale[i] = 0;
                nowweightVale2[i] = 0;
                nowweightVale3[i] = 0;
                nowweightVale4[i] = 0;
            }
            param.BARATSUKI=baratsuki;
            file.read_param();          //設定値
            cel.set_BARATSUKI(param.BARATSUKI);
            cel.set_ZEROAREA(param.ZEROAREA);
            lcd.blank(0);
            lcd.blank(1);
            for(int i = 0; i < product.head; i++) {
                nowrank[i]=99;
                totalweightN[i]=0;
            }
            for(int i = 0; i < 10; i++) {
                totalranker0103[i]=0;
                totalranker0406[i]=0;
                totalranker0709[i]=0;
                totalranker1012[i]=0;
                totalweight0103[i]=0;
                totalweight0406[i]=0;
                totalweight0709[i]=0;
                totalweight1012[i]=0;
                totalrankerA[i] = 0;
                totalweightA[i] = 0;
                totalrankerB[i] = 0;
                totalweightB[i] = 0;
            }
            if(servo_connection==true){
                //moving2=99;
                send_i(EJECT_RESET, 0, 0);
                loop_a = 0;
                while(check_signal()==false&&loop_a<=10){
                    pc.printf("wait_reset");
                    loop_a+=1;
                    wait(0.1);
                }
            }
            return;
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            if(servo_connection==true){
                for(int header=0; header<product.head; header++){
                    send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
                }
            }
            wait(1);
            auto_zero_point();                  //ゼロ点取得
            if(servo_connection==true){
                for(int header=0; header<product.head; header++){
                    send_i(SERVO_1 ,header,servo_span[header][0]); wait_res();
                }
            }
            if(product.head>1) {
                lcd.settings_rank2(&load);            //設定内容を表示
            } else {
                lcd.blank(0);
                lcd.blank(1);
            }
        }
        //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        else if(btn.up_press() == true) {
            se.chime_01();
            /*
            while(btn.any_press()==true){
                if(demo_flg==true&&btn.down_press() == true&&file.get_preset()==product.head*2){
                    demo_continue=1;
                    btn.force_ESC();
                    se.Touch_01();
                    continue;
                }
                wait(0.1);
            }
            if(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2){
                    demo_continue=1;
                    btn.force_ESC();
                    continue;
            }
            //btn.old_btn();       //スイッチ更新
            //btn.new_btn();       //スイッチ更新
            //auto_zero_off = true;
            //start_keiryo(0);
            //auto_zero_off = false;
            btnset = Chenge_vaule_btnD(0);//ｶｳﾝﾄ以外orｹｲﾘｮｳﾓｰﾄﾞ
            if(btnset==1) {
                auto_zero_off = true;
                start_keiryo(0);
                auto_zero_off = false;
            }else if(btnset==2){
                set_sercret_parameter_one(P_KURI_J,1);
                //set_sercret_parameter_one(P_END,1); //保存指令付
            }else if(btnset==3){
                set_sercret_parameter_one(P_ZEROAREA2,1);
            }else if(btnset==4){
                set_sercret_parameter_one(P_bufferT,1);
                
                
                //set_sercret_parameter_one(P_END,1); //保存指令付
            }*/
            
            if(servo_connection==true){
                //send_i(EJECT_RESET, 11+load.kumi_flag, 0);
                send_i(EJECT_RESET, 11+KUMI_NONE, 0);
                wait_signal();
            }
            select_keiryo_option(0, load.kumi_flag, 0);
            //driving_option();
            if(product.matrix_mode == MATRIX_ON) {
                for(int l=0;l<product.head;l++){
                    iro[l]=C_NULL;
                }
            }
            if(product.head>1) {
                lcd.settings_rank2(&load);            //設定内容を表示
            } else {
                lcd.blank(0);
                lcd.blank(1);
                lcd.bigNumber(cel.getWeight(0));
            }
            if(servo_connection==true){
                //send_i(EJECT_RESET, 11+load.kumi_flag, 0);
                send_i(EJECT_RESET, 11+KUMI_NONE, 0);
                wait_signal();
            }
            
        }
        //ﾓｼﾞ回転ON/OFF変更
        else if(btn.get_btn() == BTN_DOWN ) {
            se.Touch_01();
            //if(product.head >= 8) {
            if(product.Array == 0) {
                option.c_rotate =(option.c_rotate +1)%3;
            } else {
                option.
                c_rotate =(option.c_rotate +1)%2;
            }
        }
        btn.old_btn();       //スイッチ更新
        for(int i = 0; i < product.head; i++) {
            if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
                cel.set_head(i);
                cel.setAin_zero(i);
            } else {
                //cel.set_head(i);
                //cel.setAinSpeed(200);
                /**//*
                if(cel.getWeight(i)<80) {
                    if(param.BARATSUKI_GRAM<cel.getWeight(i)*0.015) {
                        param.BARATSUKI = cel.getWeight(i)*0.015;
                    } else {
                        param.BARATSUKI = param.BARATSUKI_GRAM;
                    }
                }else if(((load.target*0.98)<=cel.getWeight(i)) && (cel.getWeight(i)<=(load.target*1.02)) ||
                                (((load.target + load.jougen)*0.98)<=cel.getWeight(i)) && (cel.getWeight(i)<=((load.target + load.jougen)*1.02)) ) {
                    if(param.BARATSUKI_GRAM<cel.getWeight(i)*0.005) {
                        param.BARATSUKI = cel.getWeight(i)*0.005;
                    } else {
                        param.BARATSUKI = param.BARATSUKI_GRAM;
                    }
                }else {
                    if(param.BARATSUKI_GRAM<cel.getWeight(i)*0.01) {
                        param.BARATSUKI = cel.getWeight(i)*0.01;
                    } else {
                        param.BARATSUKI = param.BARATSUKI_GRAM;
                    }
                }*/
                //if(product.head>1){
                //    param.BARATSUKI=param.BARATSUKI_R;
                //}
                //cel.set_BARATSUKI(param.BARATSUKI);
                
                if(product.head!=1){
                    setHeadVolt(i, param.KURI_J);
                }
                //printf("%8.3f\r\n",param.BARATSUKI);
                //printf("gw%5.1f %2d  ",cel.getWeight(i),nowrank[i]);
                /*
                if(nowrank[i]!=99) {
                    //if(product.head==1){
                    //    setHeadVolt(i, 3000);
                    //}else{
                        //setHeadVolt(i, param.KURI);
                        setHeadVolt(i, param.KURI_J);
                    //}
                } else if(cel.getWeight(i)>param.CUT) {
                    //if(product.head==1){
                    //    setHeadVolt(i, 3000);
                    //}else{
                        setHeadVolt(i, 500);
                    //}
                    //setHeadVolt(i, (1000));
                } else{
                    //if(product.head==1){
                    //    setHeadVolt(i, 500);
                    //}
                }*/
                //pc.printf("%d %08.2f %08.2f %08.2f  %2d  %08.2f  %0.8f\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.anteiWeight_rank3(i),nowrank[i],param.BARATSUKI,cel.getAin());
            }
        }

        //********     デバッグ用ここから
        if( product.productType >= STANDARD_D) {
            pc.printf("00h weight   antei    huutai   rank  iro baratsuki\r\n");
            for(int i = 0; i < product.head; i++) {
                //temp = cel.temperature();   //温度を取得する
                //for(int i = 0; i < product.head; i++){
                cel.set_head(i);
                //    pc.printf("%d %08.2f %08.2f %08.2f  %2d\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.anteiWeight_rank3(i),nowrank[i]);
                //pc.printf("%d %08.2f %08.2f %08.2f %2d %2d %08.2f\r\n", i+1, cel.getWeight(i), cel.get_HWeight(i), cel.anteiWeight_rank3(i),nowrank[i],iro[i],param.BARATSUKI);
                pc.printf("%2dh %08.2f %08.2f %08.2f   %2d %4d %08.2f\r\n", 
                i+1, cel.getWeight(i), cel.anteiWeight(i), cel.get_HWeight(i),nowrank[i],iro[i],param.BARATSUKI);
                //}
                if(i == product.head - 1) {
                    //printf("%d %f C\r\n",14, temp);

                    pc.printf("========================\r\n");
                }
            }
        }
        
        if(servo_connection==true){
            
            send_i(ON_CHECK,0,close_wait_flg);
            wait_signal();
            while(btn.down_press() == true){wait(0.1);}
            /*
            signal_flg=false;
            signal_loop =0;
            send_i(ON_CHECK,0,0);
            while(check_signal()==false&&signal_loop<=10){
                signal_loop+=1;
                wait(0.1);
            }*/
        }
        //********デバッグ用ここまで
        //組み合わせ確認,最適な組み合わせ候補を抽出
        resultCombi = get_juuryo_location();

        //組み合わせに応じたLED点灯,総重量表示
        //show_rank2(&resultCombi);

        if(product.head>1) {
            show_check_juuryo(&resultCombi);
        }
        
        if(servo_connection==true){
            
            send_i(ON_CHECK,0,close_wait_flg);
            wait_signal();
            while(btn.down_press() == true){wait(0.1);}
            //排出終わった後にBUI表示があったら、表示を戻す
            //(0点取っている間に載ると、排出待ちのまま固まるので)
            for(int i = 0; i < product.head; i++) {
                if(((1 << i) & moving1) == 0){//指定ヘッドに排出待ち指示が無い
                    if(iro[i]==C_BUI_G){//排出待ち表示
                        MARU_counter[i]=0;
                        MARU_lit_counter[i]=0;  
                        cel.break_antei(i);
                        iro[i] = C_NUSE;
                    }
                }
            }

            /*
            signal_flg=false;
            signal_loop =0;
            send_i(ON_CHECK,0,0);
            while(check_signal()==false&&signal_loop<=10){
                signal_loop+=1;
                wait(0.1);
            }*/
            
            if( product.productType >= STANDARD_D) {
                printf("a%6d  |  ",moving1);
            }
            
            for(int i = 0; i < product.head; i++) {
                if(iro[i]==C_MARU){
                    if(((1 << i) & moving1) == 0){
                        send_i(SERVO_ON,1 << i,close_wait_flg);wait_signal();
                        //send_i(SERVO_ON,1 << i,0);while(check_signal()==false){wait(0.1);}
                        iro[i]=C_BUI_G;
                        //wait(5);
                        if( product.productType >= STANDARD_D) {
                            printf("1");
                        }
                    }else{
                        if( product.productType >= STANDARD_D) {
                            printf("0");
                        }
                    }
                /*}else if(iro[i]==C_BUI_G){
                    if(((1 << i) & moving1) == 0){
                        send_i(SERVO_ON,1 << i,close_wait_flg);wait_signal();
                        //send_i(SERVO_ON,1 << i,0);while(check_signal()==false){wait(0.1);}
                        iro[i]=C_BUI_G;
                        printf("1");
                    }else{
                        printf("0");
                    }*/
                }else{
                    if( product.productType >= STANDARD_D) {
                        printf("0");
                    }
                }
            }
            //printf("\r\n");
            send_i(ON_CHECK,0,close_wait_flg);
            wait_signal();
            while(btn.down_press() == true){wait(0.1);}
            /*
            signal_flg=false;
            signal_loop =0;
            send_i(ON_CHECK,0,0);
            while(check_signal()==false&&signal_loop<=10){
                signal_loop+=1;
                wait(0.1);
            }*/
            if( product.productType >= STANDARD_D) {
                printf("  | b%6d\r\n",moving1);
            }
        }
        
    }//while
    //file.read_param();          //設定値
    //cel.set_BARATSUKI(param.BARATSUKI);
    //cel.set_ZEROAREA(param.ZEROAREA);
}



//float wait_old[12];
//float disp_wait=0;
combiData_t get_juuryo_location()
{
    combiData_t com = {0};
    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)
    float *weight = new float[product.head];
    //安定している重量を取得
    for(int i = 0; i < product.head; i++) {
        if(product.head<=13) {
            //if(product.head<=6 || (i<=5 && i % 2 == 0) || (i>=6 && i % 2 == 1)){
            //weight[i] = cel.anteiWeight(i);
            weight[i] = int(cel.getWeight(i)+0.5-param.bufferT);
            //printf("%3d %3d %7.1f %7.1f\r\n",recrank[i],iro[i],weight[i],int(cel.getWeight(i)) );
            if(weight[i] > 0) {
                totalweightN[i]=weight[i];
            } else if(weight[i] == 0 && (int(cel.getWeight(i)) < param.CUT+3 && oldweightVale[i]< param.CUT+3) && ((product.head !=1 && recrank[i]>0) || (product.head ==1 && recrank[i]>=0)) && recrank[i]<8) { //安定が無くなって、現在の値が0付近になったら
                //printf("K");
                if(i<=2) {
                    totalweight0103[recrank[i]] += totalweightN[i];
                    totalranker0103[recrank[i]] += 1;
                } else if(i<=5) {
                    totalweight0406[recrank[i]] += totalweightN[i];
                    totalranker0406[recrank[i]] += 1;
                } else if(i<=8) {
                    totalweight0709[recrank[i]] += totalweightN[i];
                    totalranker0709[recrank[i]] += 1;
                } else if(i<=11) {
                    totalweight1012[recrank[i]] += totalweightN[i];
                    totalranker1012[recrank[i]] += 1;
                }
                oneshoot_OK=false;
                totalweightN[i] = 0;
                recrank[i]      = 0;
            }
            //if((weight[i] == 0 && int(cel.getWeight(i)) > param.CUT && int(cel.getWeight(i)) <= param.CUT)||cel.anteiWeight_rank(i)>=param.CUT) {
            if((weight[i] == 0 && int(cel.getWeight(i)) > param.CUT)){// && int(cel.getWeight(i)) <= param.CUT)||cel.anteiWeight_rank(i)>=param.CUT) {
                weight[i] = cel.anteiWeight(i);
            }
            //pc.printf("%2dh %08.2f %08.2f %08.2f   %2d %4d %08.2f\r\n", i+1, cel.getWeight(i), cel.anteiWeight(i), weight[i],nowrank[i],iro[i],param.BARATSUKI);
            //printf("g:%5.1f a:%5.1f w:%5.1f  wo:%5.1f\r\n",int(cel.getWeight(i)),cel.anteiWeight(i),weight[i],wait_old[i]);
            //printf("g:%5.1f a:%5.1f w:%5.1f  wo:%5.1f",int(cel.getWeight(i)),cel.anteiWeight(i),weight[i],wait_old[i]);
            //printf(" %3d %3d d:%3d ",int(weight[i]),int(wait_old[i]),(int(weight[i])-int(wait_old[i])));
  //          if(((int(weight[i])-int(wait_old[i]))<0) || ((int(weight[i])-int(wait_old[i]))>1)){
  //              wait_old[i] = weight[i];
  //          }else{
  //              weight[i] = wait_old[i];
  //          }
            wait_old[i] = weight[i];
            
            //printf("  w:%5.1f\r\n",weight[i]);
            
            //printf("%5.1f  %5.1f  ",int(cel.getWeight(i)),cel.anteiWeight_rank(i));
            //printf("%3d  ",int(weight[i]));
            //if(i==0){
            //    printf("%d %6.3f %6.3f  %6.3f  %d\r\n", i+1,cel.anteiWeight(i),cel.anteiWeight(i), weight[i],iro[i]);
            //}
            //if(weight[i]==0){
            //    weight[i] = cel.anteiWeight(i);
            //}
            int data_sa =oldweightVale[i] - nowweightVale[i];
            if(data_sa < -0.3 || data_sa > 0.4) {

            } else {
                nowweightVale[i] = oldweightVale[i];
                //weight[i] = oldweightVale[i];
            }
            oldweightVale[i] = nowweightVale[i];
        } else {
            iro[i] = C_BATU;
            //iro[i] = CR_1+i;
        }
    }
    //ヘッド数分の重量確認
    for(int i = 0; i < product.head; i++) {
           // printf("%3d  ",int(weight[i]));
        if(product.head<=13) {
            //if(product.head<=6 || (i<=5 && i % 2 == 0) || (i>=6 && i % 2 == 1)){
            if( int(cel.getWeight(i)) < -(param.CUT*2)) {//上に押されすぎたら、一旦0点調整してもらう
            } else {
                headqty=1;
                if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                    if(product.head==1) {
                        totalcheckwei[i]=weight[i];
                        if(disp_wait!=weight[i]){
                            lcd.bigNumber(weight[i]);
                            disp_wait = weight[i];
                        }
                    }
                }
                headqty=0;
            }
            if(/*iro[i]!=C_BUI_G && */(((1<<i) & moving1) == 0 )){
                //printf("w %5.1f\r\n",weight[i]);
                if(/*cel.anteiWem,ight_rank(i)==0 || */int(cel.getWeight(i)) <= param.CUT){
                    iro[i]=C_NUSE;
                    nowrank[i]=99;
                    MARU_counter[i]=0;
                    MARU_lit_counter[i]=0;
                    //printf("w1 %5d\r\n",iro[i]);
                    weight_stack[i] = 0;
                }else if(weight[i]!=0) {
                    //一番最適な重量を取得する
                    if((int(weight[i]) > -param.CUT) && (int(weight[i]) <= param.CUT)) {
                        //値を取得
                        //com.kumi += (1 << i);
                        //com.total += weight[i];
                        if(product.head!=1) {
                            iro[i]=PENTA_R;
                            MARU_counter[i]=0;
                            MARU_lit_counter[i]=0;
                        } else {
                            headqty=1;
                            if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                                if(product.head==1) {
                                    totalcheckwei[i]=weight[i];
                                    if(disp_wait!=weight[i]){
                                        lcd.bigNumber(weight[i]);
                                        disp_wait = weight[i];
                                    }
                                }
                            }
                            headqty=0;
                            //iro[i]=C_NULL;
                            //nowrank[i]=1;
                        }
                        nowrank[i]=99;
                    weight_stack[i] = 0;
                    } else if((int(weight[i]) > param.CUT) && (int(weight[i]) < int(load.target))) {
                        //iro[i]=C_NUSE;
                        nowrank[i]=1;
                        headqty=1;
                        if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                            if(product.head==1) {
                                totalcheckwei[i]=weight[i];
                                if(disp_wait!=weight[i]){
                                    lcd.bigNumber(weight[i]);
                                    disp_wait = weight[i];
                                }
                            }
                        }
                        headqty=0;
                        if(product.head==1) {
                            oneshoot_OK=false;
                        }
                        if(load.target-weight[i]<100){
                            //iro[i]= CG_0+(load.target-weight[i]) + 1;
                            
                            MARU_counter[i]=0;
                            MARU_lit_counter[i]=0;  
                            iro[i]= CR_00+(load.target-weight[i]) /*+ 1*/;
                            
                            //printf("w1 %5d\r\n",iro[i]);
                        
                        }else if(load.target-weight[i]<200){
                            MARU_counter[i]=0;
                            MARU_lit_counter[i]=0;
                            iro[i]= C_NUSE_1D;
                        }else{
                            MARU_counter[i]=0;
                            MARU_lit_counter[i]=0;
                            iro[i]= C_NUSE_2D;
                        }
                            /*
                        if((load.target-weight[i])<load.tanjuu) {
                            if(load.check_hyouji+1 >= 1-1){
                                iro[i]= CR_M1;
                            }else {
                                iro[i]= C_NUSE;
                            }
                        } else if((load.target-weight[i])<load.tanjuu*2) {
                            if(load.check_hyouji+1 >= 2-1){
                                iro[i]= CR_M2;
                            }else {
                                iro[i]= C_NUSE;
                        
                        } else {
                            iro[i]= C_NUSE;
                        }*/
                // } else if(((int(weight[i]) >= int(load.target)) && (int(weight[i]*0.998) < int(load.target + load.jougen)))) {
                } else if(((int(weight[i]) >= int(load.target)) && (int(weight[i]) <= int(load.target + load.jougen)))) {
                        //iro[i]=CG_00;
                        MARU_counter[i] += 1;
                        if(MARU_counter[i] >= option.O_count_lim){
                            if(((1<<i)  & moving1) == 0 && iro[i]!=C_BUI_G){
                                iro[i]=C_MARU;
                            }else{
                                iro[i]=C_BUI_G;
                            }
                            MARU_lit_counter[i] = option.O_lit_count_lim-1;
                            MARU_counter[i] = option.O_count_lim;
                        }else{
                            iro[i]= C_NUSE;
                        }
                        headqty=1;
                        if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                            if(product.head==1) {
                                totalcheckwei[i]=int(cel.getWeight(i));
                                lcd.bigNumber(int(cel.getWeight(i)));
                            }
                        }
                        headqty=0;
                        nowrank[i]      = 2;
                        //}
                    //} else if(((int(weight[i]) >= int(load.target)))) {
                    } else if(((int(weight[i]) >= int(load.target + load.jougen)))) {
                        if(product.head==1) {
                            oneshoot_OK=false;
                        }


                        //if((weight[i]-(load.target))<100){
                        if((weight[i]-(load.target + load.jougen))<100){
                            MARU_counter[i]=0;
                            MARU_lit_counter[i]=0;
                            //iro[i]= CR_00+(weight[i]-(load.target));
                            //iro[i]= CR_00+(weight[i]-(load.target + load.jougen));
                            iro[i]= CG_01-1+(weight[i]-(load.target + load.jougen));
                            nowrank[i]      = 2;
                            //printf("w2 %5d\r\n",iro[i]);
                        }else{
                            iro[i]=C_BATU;
                            if(product.head==1) {
                                se.mistake();
                                wait(1);
                            }
                        }
                        weight_stack[i] = 0;


                    } else {
                        iro[i]=C_BATU;
                        weight_stack[i] = 0;
                    }
                } else if(int(cel.getWeight(i)) < param.CUT+3 && oldweightVale[i]< param.CUT+3) {
                    iro[i]=C_NUSE;
                    weight_stack[i] = 0;
                }
            }
            if(cel.anteiWeight_rank(i)>0) {
                recrank[i]= nowrank[i];
            }
        }
    }//loop終了

    delete[] weight;
    return com;

}
/*combiData_t get_rank_location3(){
    const float res   = float(mweight.u01.rank_max)*1.02;    //組み合わせ重量の上限値を取得
    const float res2  = float(mweight.u02.rank_max)*1.02;    //組み合わせ重量の上限値を取得
    const float res3  = float(mweight.u03.rank_max)*1.02;    //組み合わせ重量の上限値を取得
    const float res4  = float(mweight.u04.rank_max)*1.02;    //組み合わせ重量の上限値を取得
    const float res5  = float(mweight.u05.rank_max)*1.02;    //組み合わせ重量の上限値を取得
    const float res6  = float(mweight.u06.rank_max)*1.02;    //組み合わせ重量の上限値を取得
    const float res7  = float(mweight.u07.rank_max)*1.02;    //組み合わせ重量の上限値を取得
    const float res8  = float(mweight.u08.rank_max)*1.02;    //組み合わせ重量の上限値を取得
    const float rank1ave  = (float(mweight.u01.rank_max)+float(mweight.u02.rank_max))/2;    //組み合わせ重量の上限値を取得
    const float rank2ave  = (float(mweight.u02.rank_max)+float(mweight.u03.rank_max))/2;    //組み合わせ重量の上限値を取得
    const float rank3ave  = (float(mweight.u03.rank_max)+float(mweight.u04.rank_max))/2;    //組み合わせ重量の上限値を取得
    const float rank4ave  = (float(mweight.u04.rank_max)+float(mweight.u05.rank_max))/2;    //組み合わせ重量の上限値を取得
    const float rank5ave  = (float(mweight.u05.rank_max)+float(mweight.u06.rank_max))/2;    //組み合わせ重量の上限値を取得
    const float rank6ave  = (float(mweight.u06.rank_max)+float(mweight.u07.rank_max))/2;    //組み合わせ重量の上限値を取得
    const float rank7ave  = (float(mweight.u07.rank_max)+float(mweight.u08.rank_max))/2;    //組み合わせ重量の上限値を取得
    const float rank1dif  = (float(mweight.u02.rank_max)+float(mweight.u01.rank_max))*0.25;    //組み合わせ重量の上限値を取得
    const float rank2dif  = (float(mweight.u03.rank_max)+float(mweight.u02.rank_max))*0.25;    //組み合わせ重量の上限値を取得
    const float rank3dif  = (float(mweight.u04.rank_max)+float(mweight.u03.rank_max))*0.25;    //組み合わせ重量の上限値を取得
    const float rank4dif  = (float(mweight.u05.rank_max)+float(mweight.u04.rank_max))*0.25;    //組み合わせ重量の上限値を取得
    const float rank5dif  = (float(mweight.u06.rank_max)+float(mweight.u05.rank_max))*0.25;    //組み合わせ重量の上限値を取得
    const float rank6dif  = (float(mweight.u07.rank_max)+float(mweight.u06.rank_max))*0.25;    //組み合わせ重量の上限値を取得
    const float rank7dif  = (float(mweight.u08.rank_max)+float(mweight.u07.rank_max))*0.25;    //組み合わせ重量の上限値を取得
   // const float res9  = mweight.u09.rank_max;    //組み合わせ重量の上限値を取得
    //const float res10 = mweight.u10.rank_max;    //組み合わせ重量の上限値を取得
    combiData_t com = {0};
    //printf("%4.0f:%4.0f:%4.0f:%4.0f:%4.0f:%4.0f:%4.0f\r\n",res,res2,res3,res4,res5,res6,res7);
    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)

    float *weight = new float[product.head];
    //安定している重量を取得
    for(int i = 0; i < product.head; i++){
        weight[i] = cel.anteiWeight_rank3(i);
        if(weight[i] > 0){
            totalweightN[i]=weight[i];
        }else if(weight[i] == 0 && (cel.getWeight(i) < param.CUT+3 && oldweightVale[i]< param.CUT+3) && recrank[i]>0 && recrank[i]<8){//安定が無くなって、現在の値が0付近になったら

            if(i<=2){
                totalweight0103[recrank[i]] += totalweightN[i];
                totalranker0103[recrank[i]] += 1;
            }else if(i<=5){
                totalweight0406[recrank[i]] += totalweightN[i];
                totalranker0406[recrank[i]] += 1;
            }else if(i<=8){
                totalweight0709[recrank[i]] += totalweightN[i];
                totalranker0709[recrank[i]] += 1;
            }else if(i<=11){
                totalweight1012[recrank[i]] += totalweightN[i];
                totalranker1012[recrank[i]] += 1;
            }
            totalweightN[i] = 0;
            recrank[i]      = 0;
        }
        if((weight[i] == 0 && cel.getWeight(i) > param.CUT && cel.getWeight(i) <= param.CUT)||cel.anteiWeight_rank(i)>=param.CUT){
            weight[i] = cel.anteiWeight_rank3(i);
        }
        //if(i==0){
        //    pc.printf("%d %6.3f %6.3f  %6.3f  %d\r\n", i+1,cel.anteiWeight(i),cel.anteiWeight(i), weight[i],iro[i]);
        //}
        //if(weight[i]==0){
        //    weight[i] = cel.anteiWeight(i);
        //}
        int data_sa =oldweightVale[i] - nowweightVale[i];
        if(data_sa < -0.3 || data_sa > 0.4){

        }else{
            nowweightVale[i] = oldweightVale[i];
            //weight[i] = oldweightVale[i];
        }
        oldweightVale[i] = nowweightVale[i];
    }
    //ヘッド数分の重量確認
    for(int i = 0; i < product.head; i++) {
        nowweightVale4[i] = nowweightVale3[i];
        nowweightVale3[i] = nowweightVale2[i];
        nowweightVale2[i] = cel.getWeight(i);
        if((weight[i]!=0 && nowrank[i]==0)||(iro[i]==C_NULL && nowrank[i]!=0)) {//一度ﾗﾝｸ確定したものは除外する(ｹｲﾘｮｳﾓｰﾄﾞに行った時はもう一度表示する)
            //一番最適な重量を取得する
            if((int(weight[i]) > -param.CUT) && float((int(weight[i])) <= param.CUT)) {
                iro[i]=C_NULL;
                    nowrank[i]=0;
                //値を取得
                //com.kumi += (1 << i);
                //com.total += weight[i];
            }
            else if((float(int(weight[i])) > param.CUT) && (float(int(weight[i])) < float(res))) {
                if(mweight.u01.rank_en==1) {
                    iro[i]=C_BATU;
                    nowrank[i]=0;
                    //値を取得
                    //com.kumi += (1 << i);
                    //com.total += weight[i];
                } else {
                    //iro[i]=C_NULL;
                    iro[i]=C_BATU;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res)) && (float(int(weight[i])) < float(res2))) {
                if(mweight.u02.rank_en==1) {
                    iro[i]=iro_st_r;
                    nowrank[i]=1;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res2)) && (float(int(weight[i])) < float(res3))) {
                if(mweight.u03.rank_en==1) {
                    iro[i]=iro_st_r+1;
                    nowrank[i]=2;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res3)) && (float(int(weight[i])) < float(res4))) {
                if(mweight.u04.rank_en==1) {
                    iro[i]=iro_st_r+2;
                    nowrank[i]=3;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res4)) && (float(int(weight[i])) < float(res5))) {
                if(mweight.u05.rank_en==1) {
                    iro[i]=iro_st_r+3;
                    nowrank[i]=4;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res5)) && (float(int(weight[i])) < float(res6))) {
                if(mweight.u06.rank_en==1) {
                    iro[i]=iro_st_r+4;
                    nowrank[i]=5;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res6)) && (float(int(weight[i])) < float(res7))) {
                if(mweight.u07.rank_en==1) {
                    iro[i]=iro_st_r+5;
                    nowrank[i]=6;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res7)) && (float(int(weight[i])) < float(res8))) {
                if(mweight.u08.rank_en==1) {
                    iro[i]=iro_st_r+6;
                    nowrank[i]=7;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            //else if((int(weight[i]) >= int(res6)) && (int(weight[i]) < int(res7))) {
           */ /*else if((int(weight[i]) >= int(res6))) {
                if(mweight.u08.rank_en==1) {
                    iro[i]=C_BATU;
                    nowrank[i]=8;
                    //com.kumi += (1 << i);
                    //com.total += weight[i];
                } else {
                    //iro[i]=C_NULL;
                    iro[i]=C_BATU;
                }
            } *//*
            else if(float(int(weight[i])) >= float(res8)) {
                iro[i]=C_BATU;
                nowrank[i]=0;
                com.kumi += (1 << i);
                com.total += weight[i];
            } else {
                iro[i]=PENTA_R;
                nowrank[i]=0;
            }
        //}else if(cel.getWeight(i) > param.CUT && weight[i] < param.CUT) {
        }else if(cel.getWeight(i) > param.CUT && nowrank[i]==0) {
            iro[i]=C_NUSE;
            printf("%dc\r\n",i+1);
        }else if(cel.getWeight(i) < param.CUT && nowrank[i]==0) {
            iro[i]=C_NULL;
        }else if(nowrank[i]!=0 && nowweightVale2[i] < param.CUT+3 && nowweightVale3[i]< param.CUT+3) {
            iro[i]=C_NULL;
            nowrank[i]=0;
            printf("%da\r\n",i+1);
        }

    //大体真ん中付近で重量が止まってたら、その値を確定とする
        if(weight[i]!=0 && nowrank[i]==0) {//一度ﾗﾝｸ確定したものは除外する(ｹｲﾘｮｳﾓｰﾄﾞに行った時はもう一度表示する)
            //一番最適な重量を取得する
            if((int(weight[i]) > -param.CUT) && float((int(weight[i])) <= param.CUT)) {
                iro[i]=C_NULL;
                    nowrank[i]=0;
                //値を取得
                //com.kumi += (1 << i);
                //com.total += weight[i];
            }
            else if((float(int(weight[i])) > param.CUT) && (float(int(weight[i])) < float(res))) {
                if(mweight.u01.rank_en==1) {
                    iro[i]=C_BATU;
                    nowrank[i]=0;
                    //値を取得
                    //com.kumi += (1 << i);
                    //com.total += weight[i];
                } else {
                    //iro[i]=C_NULL;
                    iro[i]=C_BATU;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res)) && (float(int(weight[i])) < float(res2))) {
                if(mweight.u02.rank_en==1) {
                    iro[i]=iro_st_r;
                    nowrank[i]=1;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res2)) && (float(int(weight[i])) < float(res3))) {
                if(mweight.u03.rank_en==1) {
                    iro[i]=iro_st_r+1;
                    nowrank[i]=2;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res3)) && (float(int(weight[i])) < float(res4))) {
                if(mweight.u04.rank_en==1) {
                    iro[i]=iro_st_r+2;
                    nowrank[i]=3;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res4)) && (float(int(weight[i])) < float(res5))) {
                if(mweight.u05.rank_en==1) {
                    iro[i]=iro_st_r+3;
                    nowrank[i]=4;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res5)) && (float(int(weight[i])) < float(res6))) {
                if(mweight.u06.rank_en==1) {
                    iro[i]=iro_st_r+4;
                    nowrank[i]=5;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res6)) && (float(int(weight[i])) < float(res7))) {
                if(mweight.u07.rank_en==1) {
                    iro[i]=iro_st_r+5;
                    nowrank[i]=6;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            else if((float(int(weight[i])) >= float(res7)) && (float(int(weight[i])) < float(res8))) {
                if(mweight.u08.rank_en==1) {
                    iro[i]=iro_st_r+6;
                    nowrank[i]=7;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=C_NULL;
                    nowrank[i]=0;
                }
            }
            //else if((int(weight[i]) >= int(res6)) && (int(weight[i]) < int(res7))) {
            *//*else if((int(weight[i]) >= int(res6))) {
                if(mweight.u08.rank_en==1) {
                    iro[i]=C_BATU;
                    nowrank[i]=8;
                    //com.kumi += (1 << i);
                    //com.total += weight[i];
                } else {
                    //iro[i]=C_NULL;
                    iro[i]=C_BATU;
                }
            } *//*
            else if(float(int(weight[i])) >= float(res8)) {
                iro[i]=C_BATU;
                nowrank[i]=0;
                com.kumi += (1 << i);
                com.total += weight[i];
            } else {
                iro[i]=PENTA_R;
                nowrank[i]=0;
            }
        //}else if(cel.getWeight(i) > param.CUT && weight[i] < param.CUT) {
        }





        if(cel.anteiWeight_rank3(i)>0) {
            recrank[i]= nowrank[i];
        }
    }//loop終了

    delete[] weight;
    return com;

}*/
int no_chara(int headPos,int c_no)
{
    if(name[headPos][c_no] != -40) {
        return name[headPos][c_no];
    } else {
        return -32;
    }
}
void rankdata_sort(int Rsort){
    if(Rsort==0) {
        for(int rl=0; rl<11; rl++) {
            totalrankerA[rl] = totalranker0103[rl] + totalranker0406[rl] + totalranker0709[rl] + totalranker1012[rl];
            totalweightA[rl] = totalweight0103[rl] + totalweight0406[rl] + totalweight0709[rl] + totalweight1012[rl];
        }
    } else if(Rsort==1) {
        if(product.head>6) {
            for(int rl=0; rl<11; rl++) {
                totalrankerA[rl] = totalranker0103[rl] + totalranker0406[rl];
                totalweightA[rl] = totalweight0103[rl] + totalweight0406[rl];
                totalrankerB[rl] = totalranker0709[rl] + totalranker1012[rl];
                totalweightB[rl] = totalweight0709[rl] + totalweight1012[rl];
            }
        } else {
            for(int rl=0; rl<11; rl++) {
                totalrankerA[rl] = totalranker0103[rl];
                totalweightA[rl] = totalweight0103[rl];
                totalrankerB[rl] = totalranker0406[rl];
                totalweightB[rl] = totalweight0406[rl];
            }
        }
    } else if(Rsort==2) {
        if(product.head>6) {
            for(int rl=0; rl<11; rl++) {
                totalrankerA[rl] = totalranker0709[rl] + totalranker1012[rl];
                totalweightA[rl] = totalweight0709[rl] + totalweight1012[rl];
                totalrankerB[rl] = totalranker0103[rl] + totalranker0406[rl];
                totalweightB[rl] = totalweight0103[rl] + totalweight0406[rl];
            }
        } else {
            for(int rl=0; rl<11; rl++) {
                totalrankerA[rl] = totalranker0406[rl];
                totalweightA[rl] = totalweight0406[rl];
                totalrankerB[rl] = totalranker0103[rl];
                totalweightB[rl] = totalweight0103[rl];
            }
        }
    } else if(Rsort==3) {
        for(int rl=0; rl<11; rl++) {
            totalrankerA[rl] = totalranker0103[rl] + totalranker0406[rl];
            totalweightA[rl] = totalweight0103[rl] + totalweight0406[rl];
            totalrankerB[rl] = totalranker0709[rl];
            totalweightB[rl] = totalweight0709[rl];
            totalrankerC[rl] = totalranker1012[rl];
            totalweightC[rl] = totalweight1012[rl];
        }
    } else if(Rsort==4) {
        for(int rl=0; rl<11; rl++) {
            totalrankerA[rl] = totalranker0709[rl] + totalranker1012[rl];
            totalweightA[rl] = totalweight0709[rl] + totalweight1012[rl];
            totalrankerB[rl] = totalranker0103[rl];
            totalweightB[rl] = totalweight0103[rl];
            totalrankerC[rl] = totalranker0406[rl];
            totalweightC[rl] = totalweight0406[rl];
        }
    } else if(Rsort==5) {
        for(int rl=0; rl<11; rl++) {
            totalrankerA[rl] = totalranker0103[rl];
            totalweightA[rl] = totalweight0103[rl];
            totalrankerB[rl] = totalranker0406[rl];
            totalweightB[rl] = totalweight0406[rl];
            totalrankerC[rl] = totalranker0709[rl];
            totalweightC[rl] = totalweight0709[rl];
            totalrankerD[rl] = totalranker1012[rl];
            totalweightD[rl] = totalweight1012[rl];
        }
    } else if(Rsort==6) {
        for(int rl=0; rl<11; rl++) {
            totalrankerA[rl] = totalranker0709[rl];
            totalweightA[rl] = totalweight0709[rl];
            totalrankerB[rl] = totalranker1012[rl];
            totalweightB[rl] = totalweight1012[rl];
            totalrankerC[rl] = totalranker0103[rl];
            totalweightC[rl] = totalweight0103[rl];
            totalrankerD[rl] = totalranker0406[rl];
            totalweightD[rl] = totalweight0406[rl];
        }
    } else {
    }
}
void save_rankdata(int Rsort ,int ji,int hun)
{
    char SAVEFILE[300];  //選択されているプリセット位置
    int headPosition = file.get_preset();
    sprintf(SAVEFILE,"/local/%02d%c%c%c%c%c%c%c%c%c%ctotal.txt", file.get_preset()+1,0x40+name[headPosition][0],0x40+name[headPosition][1],0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5],0x40+name[headPosition][6],0x40+name[headPosition][7],0x40+name[headPosition][8],0x40+name[headPosition][9]);
    //    sprintf(SAVEFILE,"/local/%02dTOTALR%c%c%c%c%c%c%c%c%c%c.txt", file.get_preset()+1,0x40+name[headPosition][0],0x40+name[headPosition][1],0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5],0x40+name[headPosition][6],0x40+name[headPosition][7],0x40+name[headPosition][8],0x40+name[headPosition][9]);
/*
    FILE *fp3 = fopen(SAVEFILE, "w");
    if(fp3 == NULL) {
        error("Could not open file for write\n");
    }



    fprintf(fp3, "Working hours : %2dh%02dm\r\n",ji,hun);
    fprintf(fp3, "set_No : %02d\r\n",headPosition+1);
    fprintf(fp3, "user   :");
    fprintf(fp3, "%c%c%c%c%c%c%c%c%c%c",0x40+name[headPosition][0],0x40+name[headPosition][1],0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5],0x40+name[headPosition][6],0x40+name[headPosition][7],0x40+name[headPosition][8],0x40+name[headPosition][9]);
    fprintf(fp3, "\r\n");
    if(mweight.u02.rank_en==1) {
        //fprintf(fp3, "3S :%4d.0 ~ %4d.9\r\n",mweight.u01.rank_max,mweight.u02.rank_max);
        fprintf(fp3, "3S :%4d ~ %4d\r\n",mweight.u01.rank_max,mweight.u02.rank_max);
    }
    if(mweight.u03.rank_en==1) {
        //fprintf(fp3, " L :%4d.0 ~ %4d.9\r\n",mweight.u02.rank_max,mweight.u03.rank_max);
        fprintf(fp3, " M :%4d ~ %4d\r\n",mweight.u02.rank_max,mweight.u03.rank_max);
    }
    if(mweight.u04.rank_en==1) {
        //fprintf(fp3, " 2L :%4d.0 ~ %4d.9\r\n",mweight.u03.rank_max,mweight.u04.rank_max);
        fprintf(fp3, " 2L :%4d ~ %4d\r\n",mweight.u03.rank_max,mweight.u04.rank_max);
    }
    //fprintf(fp3, " M :%4d.0 ~ %4d.9\r\n",mweight.u04.rank_max,mweight.u05.rank_max);
    fprintf(fp3, " 3L :%4d ~ %4d\r\n",mweight.u04.rank_max,mweight.u05.rank_max);
    if(mweight.u06.rank_en==1) {
        //fprintf(fp3, " 4L :%4d.0 ~ %4d.9\r\n",mweight.u05.rank_max,mweight.u06.rank_max);
        fprintf(fp3, " 4L :%4d ~ %4d\r\n",mweight.u05.rank_max,mweight.u06.rank_max);
    }
    if(mweight.u07.rank_en==1) {
        //fprintf(fp3, "5L :%4d.0 ~ %4d.9\r\n",mweight.u06.rank_max,mweight.u07.rank_max);
        fprintf(fp3, "5L :%4d ~ %4d\r\n",mweight.u06.rank_max,mweight.u07.rank_max);
    }
    if(mweight.u08.rank_en==1) {
        //fprintf(fp3, "6L :%4d.0 ~ %4d.9\r\n",mweight.u07.rank_max,mweight.u08.rank_max);
        fprintf(fp3, "6L :%4d ~ %4d\r\n",mweight.u07.rank_max,mweight.u08.rank_max);
    }

    rankdata_sort(Rsort);

    fprintf(fp3, "\r\n");
    fprintf(fp3, "\r\n");
    //データの集計
    //fprintf(fp3, " place\t num\t value\r\n");
    fprintf(fp3, "         :");
    for(int rl=0; rl<10; rl++) {
        if(rl >= start_r && rl <= end_r) {
            switch(rl) {
                case R_NG1:
                    fprintf(fp3, "     NG   :");
                    break;
                case R_M:
                    fprintf(fp3, "      M   :");
                    break;
                case R_L:
                    fprintf(fp3, "      L   :");
                    break;
                case R_2L:
                    fprintf(fp3, "      2L   :");
                    break;
                case R_3L:
                    fprintf(fp3, "      3L   :");
                    break;
                case R_4L:
                    fprintf(fp3, "      4L   :");
                    break;
                case R_5L:
                    fprintf(fp3, "     5L   :");
                    break;
                case R_6L:
                    fprintf(fp3, "     6L   :");
                    break;
                case R_NG2:
                    fprintf(fp3, "     NG   :");
                    break;
                case R_NG3:
                    fprintf(fp3, "     NG   :");
                    break;
            }
        }
    }
    fprintf(fp3, "\r\n");
    for(int wl=0; wl<4; wl++) {
        switch(wl) {
            case 0 :
                fprintf(fp3, "A_RANK   :");
                for(int rl=0; rl<10; rl++) {
                    if(rl >= start_r && rl <= end_r) {
                        fprintf(fp3, "%8d  :",totalrankerA[rl]);
                    }
                }
                fprintf(fp3, "\r\n");
                break;
            case 1 :
                if(Rsort>0) {
                    fprintf(fp3, "B_RANK   :");
                    for(int rl=0; rl<10; rl++) {
                        if(rl >= start_r && rl <= end_r) {
                            fprintf(fp3, "%8d  :",totalrankerB[rl]);
                        }
                    }
                    fprintf(fp3, "\r\n");
                }
                break;
            case 2 :
                if(Rsort>2) {
                    fprintf(fp3, "C_RANK   :");
                    for(int rl=0; rl<10; rl++) {
                        if(rl >= start_r && rl <= end_r) {
                            fprintf(fp3, "%8d  :",totalrankerC[rl]);
                        }
                    }
                    fprintf(fp3, "\r\n");
                }
                break;
            case 3 :
                if(Rsort>4) {
                    fprintf(fp3, "D_RANK   :");
                    for(int rl=0; rl<10; rl++) {
                        if(rl >= start_r && rl <= end_r) {
                            fprintf(fp3, "%8d  :",totalrankerD[rl]);
                        }
                    }
                    fprintf(fp3, "\r\n");
                }
                break;
        }
    }

    for(int wl=0; wl<4; wl++) {
        switch(wl) {
            case 0 :
                fprintf(fp3, "A_WEIGHT :");
                for(int rl=0; rl<10; rl++) {
                    if(rl >= start_r && rl <= end_r) {
                        fprintf(fp3, "%8.1f  :",totalweightA[rl]);
                    }
                }
                fprintf(fp3, "\r\n");
                break;
            case 1 :
                if(Rsort>0) {
                    fprintf(fp3, "B_WEIGHT :");
                    for(int rl=0; rl<10; rl++) {
                        if(rl >= start_r && rl <= end_r) {
                            fprintf(fp3, "%8.1f  :",totalweightB[rl]);
                        }
                    }
                    fprintf(fp3, "\r\n");
                }
                break;
            case 2 :
                if(Rsort>2) {
                    fprintf(fp3, "C_WEIGHT :");
                    for(int rl=0; rl<10; rl++) {
                        if(rl >= start_r && rl <= end_r) {
                            fprintf(fp3, "%8.1f  :",totalweightC[rl]);
                        }
                    }
                    fprintf(fp3, "\r\n");
                }
                break;
            case 3 :
                if(Rsort>4) {
                    fprintf(fp3, "D_WEIGHT :");
                    for(int rl=0; rl<10; rl++) {
                        if(rl >= start_r && rl <= end_r) {
                            fprintf(fp3, "%8.1f  :",totalweightD[rl]);
                        }
                    }
                    fprintf(fp3, "\r\n");
                }
                break;
        }
    }
    fclose(fp3);
    fp3 = NULL;
    */
    
    int rank_en=0;
    int rank_no=0;

    FILE *fp3 = fopen(SAVEFILE, "w");
    if(fp3 == NULL) {
        //error("Could not open file for write\n");
    } else {
        fprintf(fp3, "Working hours : %2dh%02dm\r\n",ji,hun);
        fprintf(fp3, "set_No : %02d\r\n",headPosition+1);
        fprintf(fp3, "user   :");
        fprintf(fp3, "%c%c%c%c%c%c%c%c%c%c",0x40+name[headPosition][0],0x40+name[headPosition][1],0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5],0x40+name[headPosition][6],0x40+name[headPosition][7],0x40+name[headPosition][8],0x40+name[headPosition][9]);
        fprintf(fp3, "\r\n");
        for(int i = 0;i<10;i++){
            rank_en=0;
            if      (i==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
            }else if(i==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
            }else if(i==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
            }else if(i==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
            }else if(i==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
            }else if(i==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
            }else if(i==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
            }else if(i==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
            }else if(i==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
            }else                                 { rank_en=0;rank_no=0;
            }
            if(rank_en == 1){
                if(rank_no > CG_RANK_START){
                    rank_no = rank_no - (CG_RANK_START-C_RANK_START);
                }
                if(CR_01<=rank_no&&rank_no<=CR_99){
                    fprintf(fp3,"%2d\t:",rank_no-(CR_01-1));
                }else if(C_9S<=rank_no&&rank_no<=C_MA){
                    if(C_9S<=rank_no&&rank_no<=C_2S){
                        fprintf(fp3, "%dS\t:",C_S-rank_no+1);
                    }else if(C_2L<=rank_no&&rank_no<=C_9L){
                        fprintf(fp3, "%dL\t:",rank_no-C_L+1);
                    }else{
                        switch(rank_no) {
                            case(C_S):  fprintf(fp3, "S\t:");   break;
                            case(C_M):  fprintf(fp3, "M\t:");   break;
                            case(C_L):  fprintf(fp3, "L\t:");   break;
                            case(C_SA): fprintf(fp3, "SA\t:");  break;
                            case(C_MA): fprintf(fp3, "MA\t:");  break;
                        }
                    }
                }
                if      (i==1){
                    fprintf(fp3, "%4d ~ %4d\r\n",mweight.u01.rank_max,mweight.u02.rank_max);
                }else if(i==2){
                    fprintf(fp3, "%4d ~ %4d\r\n",mweight.u02.rank_max,mweight.u03.rank_max);
                }else if(i==3){
                    fprintf(fp3, "%4d ~ %4d\r\n",mweight.u03.rank_max,mweight.u04.rank_max);
                }else if(i==4){
                    fprintf(fp3, "%4d ~ %4d\r\n",mweight.u04.rank_max,mweight.u05.rank_max);
                }else if(i==5){
                    fprintf(fp3, "%4d ~ %4d\r\n",mweight.u05.rank_max,mweight.u06.rank_max);
                }else if(i==6){
                    fprintf(fp3, "%4d ~ %4d\r\n",mweight.u06.rank_max,mweight.u07.rank_max);
                }else if(i==7){
                    fprintf(fp3, "%4d ~ %4d\r\n",mweight.u07.rank_max,mweight.u08.rank_max);
                }else if(i==8){
                    fprintf(fp3, "%4d ~ %4d\r\n",mweight.u08.rank_max,mweight.u09.rank_max);
                }else if(i==9){
                    fprintf(fp3, "%4d ~ %4d\r\n",mweight.u09.rank_max,mweight.u10.rank_max);
                }
            }
        }
        rankdata_sort(Rsort);
        fprintf(fp3, "\r\n");
        fprintf(fp3, "\r\n");
        fprintf(fp3, "\r\n");
        fprintf(fp3, "\r\n");
        fprintf(fp3, "CLASS\t:RANK\t:Qty\t:TOTAL\r\n");
        fprintf(fp3, "A\r\n");
        rank_en=0;
        for(int rl=0; rl<10; rl++) {
            if      (rl==0)                        { rank_en=0;
            }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
            }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
            }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
            }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
            }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
            }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
            }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
            }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
            }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
            }else                                  { rank_en+=1;
            }
            if(rank_en==0){
                //fprintf(fp3, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                fprintf(fp3, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerA[0],totalweightA[0]);
            }
            if(rank_en==1){
                if(rank_no > CG_RANK_START){
                    rank_no = rank_no - (CG_RANK_START-C_RANK_START);
                }
                if(CR_01<=rank_no&&rank_no<=CR_99){
                    fprintf(fp3,"     \t:%2d\t:",rank_no-(CR_01-1));
                }else if(C_9S<=rank_no&&rank_no<=C_MA){
                    if(C_9S<=rank_no&&rank_no<=C_2S){
                        fprintf(fp3, "     \t:%dS\t:",C_S-rank_no+1);
                    }else if(C_2L<=rank_no&&rank_no<=C_9L){
                        fprintf(fp3, "     \t:%dL\t:",rank_no-C_L+1);
                    }else{
                        switch(rank_no) {
                            case(C_S):  fprintf(fp3, "     \t:S\t:");   break;
                            case(C_M):  fprintf(fp3, "     \t:M\t:");   break;
                            case(C_L):  fprintf(fp3, "     \t:L\t:");   break;
                            case(C_SA): fprintf(fp3, "     \t:SA\t:");  break;
                            case(C_MA): fprintf(fp3, "     \t:MA\t:");  break;
                        }
                    }
                }
                fprintf(fp3,      " %5d\t: %8.1f\r\n",totalrankerA[rl],totalweightA[rl]);
            }else if(rank_en==2){
            }
        }
        fprintf(fp3, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerA[9],totalweightA[9]);
        
        
        if(Rsort>0) {
            fprintf(fp3, "B\r\n");
            rank_en=0;
            for(int rl=0; rl<10; rl++) {
                //if(rl >= start_r && rl <= end_r) {
            if      (rl==0)                        { rank_en=0;
            }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
            }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
            }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
            }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
            }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
            }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
            }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
            }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
            }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
            }else                                  { rank_en+=1;
                }
                if(rank_en==0){
                    //fprintf(fp3, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                    fprintf(fp3, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerB[0],totalweightB[0]);
                }
                if(rank_en==1){
                    if(rank_no > CG_RANK_START){
                        rank_no = rank_no - (CG_RANK_START-C_RANK_START);
                    }
                    if(CR_01<=rank_no&&rank_no<=CR_99){
                        fprintf(fp3, "     \t:%2d\t:",rank_no-(CR_01-1));
                    }else if(C_9S<=rank_no&&rank_no<=C_MA){
                        if(C_9S<=rank_no&&rank_no<=C_2S){
                            fprintf(fp3, "     \t:%dS\t:",C_S-rank_no+1);
                        }else if(C_2L<=rank_no&&rank_no<=C_9L){
                            fprintf(fp3, "     \t:%dL\t:",rank_no-C_L+1);
                        }else{
                            switch(rank_no) {
                                case(C_S):  fprintf(fp3, "     \t:S\t:");   break;
                                case(C_M):  fprintf(fp3, "     \t:M\t:");   break;
                                case(C_L):  fprintf(fp3, "     \t:L\t:");   break;
                                case(C_SA): fprintf(fp3, "     \t:SA\t:");  break;
                                case(C_MA): fprintf(fp3, "     \t:MA\t:");  break;
                            }
                        }
                    }
                    fprintf(fp3,      " %5d\t: %8.1f\r\n",totalrankerB[rl],totalweightB[rl]);
                }else if(rank_en==2){
                }
            }
            //fprintf(fp3, " NG : %5d : %8.0f :\r\n",totalrankerA[7],totalweightA[7]);
            fprintf(fp3, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerB[10],totalweightB[10]);
        }    
        
        
        
        if(Rsort>2) {
            fprintf(fp3, "C\r\n");
            rank_en=0;
            for(int rl=0; rl<10; rl++) {
                //if(rl >= start_r && rl <= end_r) {
                if      (rl==0)                        { rank_en=0;
                }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
                }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
                }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
                }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
                }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
                }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
                }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
                }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
                }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
                }else                                  { rank_en+=1;
                }
                if(rank_en==0){
                    //fprintf(fp3, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                    fprintf(fp3, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerC[0],totalweightC[0]);
                }
                if(rank_en==1){
                    if(rank_no > CG_RANK_START){
                        rank_no = rank_no - (CG_RANK_START-C_RANK_START);
                    }
                    if(CR_01<=rank_no&&rank_no<=CR_99){
                        fprintf(fp3, "     \t:%2d\t:",rank_no-(CR_01-1));
                    }else if(C_9S<=rank_no&&rank_no<=C_MA){
                        if(C_9S<=rank_no&&rank_no<=C_2S){
                            fprintf(fp3, "     \t:%dS\t:",C_S-rank_no+1);
                        }else if(C_2L<=rank_no&&rank_no<=C_9L){
                            fprintf(fp3, "     \t:%dL\t:",rank_no-C_L+1);
                        }else{
                            switch(rank_no) {
                                case(C_S):  fprintf(fp3, "     \t:S\t:");   break;
                                case(C_M):  fprintf(fp3, "     \t:M\t:");   break;
                                case(C_L):  fprintf(fp3, "     \t:L\t:");   break;
                                case(C_SA): fprintf(fp3, "     \t:SA\t:");  break;
                                case(C_MA): fprintf(fp3, "     \t:MA\t:");  break;
                            }
                        }
                    }
                    fprintf(fp3,      " %5d\t: %8.1f\r\n",totalrankerC[rl],totalweightC[rl]);
                }else if(rank_en==2){
                }
            }
            //fprintf(fp3, " NG : %5d : %8.0f :\r\n",totalrankerA[7],totalweightA[7]);
            fprintf(fp3, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerC[10],totalweightC[10]);
        }
        if(Rsort>4) {
            fprintf(fp3, "D\r\n");
            rank_en=0;
            for(int rl=0; rl<10; rl++) {
                //if(rl >= start_r && rl <= end_r) {
            if      (rl==0)                        { rank_en=0;
            }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
            }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
            }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
            }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
            }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
            }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
            }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
            }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
            }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
            }else                                  { rank_en+=1;
                }
                if(rank_en==0){
                    //fprintf(fp3, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                    fprintf(fp3, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerD[0],totalweightD[0]);
                }
                if(rank_en==1){
                    if(rank_no > CG_RANK_START){
                        rank_no = rank_no - (CG_RANK_START-C_RANK_START);
                    }
                    if(CR_01<=rank_no&&rank_no<=CR_99){
                        fprintf(fp3, "     \t:%2d\t:",rank_no-(CR_01-1));
                    }else if(C_9S<=rank_no&&rank_no<=C_MA){
                        if(C_9S<=rank_no&&rank_no<=C_2S){
                            fprintf(fp3, "     \t:%dS\t:",C_S-rank_no+1);
                        }else if(C_2L<=rank_no&&rank_no<=C_9L){
                            fprintf(fp3, "     \t:%dL\t:",rank_no-C_L+1);
                        }else{
                            switch(rank_no) {
                                case(C_S):  fprintf(fp3, "     \t:S\t:");   break;
                                case(C_M):  fprintf(fp3, "     \t:M\t:");   break;
                                case(C_L):  fprintf(fp3, "     \t:L\t:");   break;
                                case(C_SA): fprintf(fp3, "     \t:SA\t:");  break;
                                case(C_MA): fprintf(fp3, "     \t:MA\t:");  break;
                            }
                        }
                    }
                    fprintf(fp3,      " %5d\t: %8.1f\r\n",totalrankerD[rl],totalweightD[rl]);
                }else if(rank_en==2){
                }
            }
            //fprintf(fp3, " NG : %5d : %8.0f :\r\n",totalrankerA[7],totalweightA[7]);
            fprintf(fp3, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerD[10],totalweightD[10]);
        }
    }
    fclose(fp3);
    fp3 = NULL;
}
void USB_rankdata(int Rsort ,int ji,int hun)
{
    char SAVEER[2][16]= {{0xB4,0xD7,0xB0,0x21,0x21,0x00}   //ｴﾗｰ!!
        ,{'U' ,'S' ,'B' ,0xB6,0xDE,0xD0,0xC2,0xB6,0xD8,0xCF,0xBE,0xDD,0x00}
    };    //USBｶﾞﾐﾂｶﾘﾏｾﾝ
    char SAVEFILE[300];  //選択されているプリセット位置
    //char SAVEname[300];  //選択されているプリセット位置
    int headPosition = file.get_preset();
    //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%c%c%c%c%c.txt", file.get_preset()+1,0x40+name[headPosition][0],0x40+name[headPosition][1],0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5],0x40+name[headPosition][6],0x40+name[headPosition][7],0x40+name[headPosition][8],0x40+name[headPosition][9]);
    sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%c.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),0x40+no_chara(headPosition,5));
    //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%c.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5]);
    int rank_en=0;
    int rank_no=0;
    if(msc.disk_initialize()!=1) {
        FILE *fp4 = fopen(SAVEFILE, "w");
        if(fp4 == NULL) {
            //error("Could not open file for write\n");
        } else {
            fprintf(fp4, "Working hours : %2dh%02dm\r\n",ji,hun);
            fprintf(fp4, "set_No : %02d\r\n",headPosition+1);
            fprintf(fp4, "user   :");
            fprintf(fp4, "%c%c%c%c%c%c%c%c%c%c",0x40+name[headPosition][0],0x40+name[headPosition][1],0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5],0x40+name[headPosition][6],0x40+name[headPosition][7],0x40+name[headPosition][8],0x40+name[headPosition][9]);
            fprintf(fp4, "\r\n");
            for(int i = 0;i<10;i++){
                rank_en=0;
                if      (i==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
                }else if(i==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
                }else if(i==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
                }else if(i==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
                }else if(i==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
                }else if(i==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
                }else if(i==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
                }else if(i==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
                }else if(i==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
                }else                                 { rank_en=0;rank_no=0;
                }
                if(rank_en == 1){
                    if(rank_no > CG_RANK_START){
                        rank_no = rank_no - (CG_RANK_START-C_RANK_START);
                    }
                    if(CR_01<=rank_no&&rank_no<=CR_99){
                        fprintf(fp4,"%2d\t:",rank_no-(CR_01-1));
                    }else if(C_9S<=rank_no&&rank_no<=C_MA){
                        if(C_9S<=rank_no&&rank_no<=C_2S){
                            fprintf(fp4, "%dS\t:",C_S-rank_no+1);
                        }else if(C_2L<=rank_no&&rank_no<=C_9L){
                            fprintf(fp4, "%dL\t:",rank_no-C_L+1);
                        }else{
                            switch(rank_no) {
                                case(C_S):  fprintf(fp4, "S\t:");   break;
                                case(C_M):  fprintf(fp4, "M\t:");   break;
                                case(C_L):  fprintf(fp4, "L\t:");   break;
                                case(C_SA): fprintf(fp4, "SA\t:");  break;
                                case(C_MA): fprintf(fp4, "MA\t:");  break;
                            }
                        }
                    }
                    if      (i==1){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u01.rank_max,mweight.u02.rank_max);
                    }else if(i==2){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u02.rank_max,mweight.u03.rank_max);
                    }else if(i==3){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u03.rank_max,mweight.u04.rank_max);
                    }else if(i==4){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u04.rank_max,mweight.u05.rank_max);
                    }else if(i==5){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u05.rank_max,mweight.u06.rank_max);
                    }else if(i==6){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u06.rank_max,mweight.u07.rank_max);
                    }else if(i==7){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u07.rank_max,mweight.u08.rank_max);
                    }else if(i==8){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u08.rank_max,mweight.u09.rank_max);
                    }else if(i==9){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u09.rank_max,mweight.u10.rank_max);
                    }
                }
            }
            rankdata_sort(Rsort);
            fprintf(fp4, "\r\n");
            fprintf(fp4, "\r\n");
            fprintf(fp4, "\r\n");
            fprintf(fp4, "\r\n");
            fprintf(fp4, "CLASS\t:RANK\t:Qty\t:TOTAL\r\n");
            fprintf(fp4, "A\r\n");
            rank_en=0;
            for(int rl=0; rl<12; rl++) {
                printf("%2d\t:NGl\t: %5d\t: %8.1f\r\n",rl,totalrankerA[rl],totalweightA[rl]);
            }
            for(int rl=0; rl<10; rl++) {
                if      (rl==0)                        { rank_en=0;
                }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
                }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
                }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
                }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
                }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
                }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
                }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
                }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
                }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
                }else                                  { rank_en+=1;
                }
                if(rank_en==0){
                    //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                    fprintf(fp4, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerA[0],totalweightA[0]);
                }
                if(rank_en==1){
                    if(rank_no > CG_RANK_START){
                        rank_no = rank_no - (CG_RANK_START-C_RANK_START);
                    }
                    if(CR_01<=rank_no&&rank_no<=CR_99){
                        fprintf(fp4,"     \t:%2d\t:",rank_no-(CR_01-1));
                    }else if(C_9S<=rank_no&&rank_no<=C_MA){
                        if(C_9S<=rank_no&&rank_no<=C_2S){
                            fprintf(fp4, "     \t:%dS\t:",C_S-rank_no+1);
                        }else if(C_2L<=rank_no&&rank_no<=C_9L){
                            fprintf(fp4, "     \t:%dL\t:",rank_no-C_L+1);
                        }else{
                            switch(rank_no) {
                                case(C_S):  fprintf(fp4, "     \t:S\t:");   break;
                                case(C_M):  fprintf(fp4, "     \t:M\t:");   break;
                                case(C_L):  fprintf(fp4, "     \t:L\t:");   break;
                                case(C_SA): fprintf(fp4, "     \t:SA\t:");  break;
                                case(C_MA): fprintf(fp4, "     \t:MA\t:");  break;
                            }
                        }
                    }
                    fprintf(fp4,      " %5d\t: %8.1f\r\n",totalrankerA[rl],totalweightA[rl]);
                }else if(rank_en==2){
                }
            }
            fprintf(fp4, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerA[10],totalweightA[10]);
            
            
            if(Rsort>0) {
                fprintf(fp4, "B\r\n");
                rank_en=0;
                for(int rl=0; rl<10; rl++) {
                    //if(rl >= start_r && rl <= end_r) {
                if      (rl==0)                        { rank_en=0;
                }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
                }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
                }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
                }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
                }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
                }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
                }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
                }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
                }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
                }else                                  { rank_en+=1;
                    }
                    if(rank_en==0){
                        //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                        fprintf(fp4, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerB[0],totalweightB[0]);
                    }
                    if(rank_en==1){
                        if(rank_no > CG_RANK_START){
                            rank_no = rank_no - (CG_RANK_START-C_RANK_START);
                        }
                        if(CR_01<=rank_no&&rank_no<=CR_99){
                            fprintf(fp4, "     \t:%2d\t:",rank_no-(CR_01-1));
                        }else if(C_9S<=rank_no&&rank_no<=C_MA){
                            if(C_9S<=rank_no&&rank_no<=C_2S){
                                fprintf(fp4, "     \t:%dS\t:",C_S-rank_no+1);
                            }else if(C_2L<=rank_no&&rank_no<=C_9L){
                                fprintf(fp4, "     \t:%dL\t:",rank_no-C_L+1);
                            }else{
                                switch(rank_no) {
                                    case(C_S):  fprintf(fp4, "     \t:S\t:");   break;
                                    case(C_M):  fprintf(fp4, "     \t:M\t:");   break;
                                    case(C_L):  fprintf(fp4, "     \t:L\t:");   break;
                                    case(C_SA): fprintf(fp4, "     \t:SA\t:");  break;
                                    case(C_MA): fprintf(fp4, "     \t:MA\t:");  break;
                                }
                            }
                        }
                        fprintf(fp4,      " %5d\t: %8.1f\r\n",totalrankerB[rl],totalweightB[rl]);
                    }else if(rank_en==2){
                    }
                }
                //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[7],totalweightA[7]);
                fprintf(fp4, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerB[10],totalweightB[10]);
            }    
            
            
            
            if(Rsort>2) {
                fprintf(fp4, "C\r\n");
                rank_en=0;
                for(int rl=0; rl<10; rl++) {
                    //if(rl >= start_r && rl <= end_r) {
                    if      (rl==0)                        { rank_en=0;
                    }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
                    }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
                    }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
                    }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
                    }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
                    }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
                    }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
                    }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
                    }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
                    }else                                  { rank_en+=1;
                    }
                    if(rank_en==0){
                        //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                        fprintf(fp4, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerC[0],totalweightC[0]);
                    }
                    if(rank_en==1){
                        if(rank_no > CG_RANK_START){
                            rank_no = rank_no - (CG_RANK_START-C_RANK_START);
                        }
                        if(CR_01<=rank_no&&rank_no<=CR_99){
                            fprintf(fp4, "     \t:%2d\t:",rank_no-(CR_01-1));
                        }else if(C_9S<=rank_no&&rank_no<=C_MA){
                            if(C_9S<=rank_no&&rank_no<=C_2S){
                                fprintf(fp4, "     \t:%dS\t:",C_S-rank_no+1);
                            }else if(C_2L<=rank_no&&rank_no<=C_9L){
                                fprintf(fp4, "     \t:%dL\t:",rank_no-C_L+1);
                            }else{
                                switch(rank_no) {
                                    case(C_S):  fprintf(fp4, "     \t:S\t:");   break;
                                    case(C_M):  fprintf(fp4, "     \t:M\t:");   break;
                                    case(C_L):  fprintf(fp4, "     \t:L\t:");   break;
                                    case(C_SA): fprintf(fp4, "     \t:SA\t:");  break;
                                    case(C_MA): fprintf(fp4, "     \t:MA\t:");  break;
                                }
                            }
                        }
                        fprintf(fp4,      " %5d\t: %8.1f\r\n",totalrankerC[rl],totalweightC[rl]);
                    }else if(rank_en==2){
                    }
                }
                //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[7],totalweightA[7]);
                fprintf(fp4, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerC[10],totalweightC[10]);
            }
            if(Rsort>4) {
                fprintf(fp4, "D\r\n");
                rank_en=0;
                for(int rl=0; rl<10; rl++) {
                    //if(rl >= start_r && rl <= end_r) {
                if      (rl==0)                        { rank_en=0;
                }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
                }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
                }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
                }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
                }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
                }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
                }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
                }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
                }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
                }else                                  { rank_en+=1;
                    }
                    if(rank_en==0){
                        //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                        fprintf(fp4, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerD[0],totalweightD[0]);
                    }
                    if(rank_en==1){
                        if(rank_no > CG_RANK_START){
                            rank_no = rank_no - (CG_RANK_START-C_RANK_START);
                        }
                        if(CR_01<=rank_no&&rank_no<=CR_99){
                            fprintf(fp4, "     \t:%2d\t:",rank_no-(CR_01-1));
                        }else if(C_9S<=rank_no&&rank_no<=C_MA){
                            if(C_9S<=rank_no&&rank_no<=C_2S){
                                fprintf(fp4, "     \t:%dS\t:",C_S-rank_no+1);
                            }else if(C_2L<=rank_no&&rank_no<=C_9L){
                                fprintf(fp4, "     \t:%dL\t:",rank_no-C_L+1);
                            }else{
                                switch(rank_no) {
                                    case(C_S):  fprintf(fp4, "     \t:S\t:");   break;
                                    case(C_M):  fprintf(fp4, "     \t:M\t:");   break;
                                    case(C_L):  fprintf(fp4, "     \t:L\t:");   break;
                                    case(C_SA): fprintf(fp4, "     \t:SA\t:");  break;
                                    case(C_MA): fprintf(fp4, "     \t:MA\t:");  break;
                                }
                            }
                        }
                        fprintf(fp4,      " %5d\t: %8.1f\r\n",totalrankerD[rl],totalweightD[rl]);
                    }else if(rank_en==2){
                    }
                }
                //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[7],totalweightA[7]);
                fprintf(fp4, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerD[10],totalweightD[10]);
            }
        }
        fclose(fp4);
        fp4 = NULL;
    } else {
        lcd.blank(0);
        lcd.blank(1);
        lcd.locate(0,0);
        lcd.printf(SAVEER[0]);
        lcd.locate(0,1);
        lcd.printf(SAVEER[1]);
        se.chime_01();    //警告音
        //wait(3);
        lcd.blank(0);
        lcd.blank(1);
    }
}

void USB_rankdata_date(int Rsort ,int ji,int hun,int SerialNumber ,int year,int mon,int mday,int hour,int min)
{
    char SAVEER[2][16]= {{0xB4,0xD7,0xB0,0x21,0x21,0x00}   //ｴﾗｰ!!
        ,{'U' ,'S' ,'B' ,0xB6,0xDE,0xD0,0xC2,0xB6,0xD8,0xCF,0xBE,0xDD,0x00}
    };    //USBｶﾞﾐﾂｶﾘﾏｾﾝ
    char SAVEFILE[300];  //選択されているプリセット位置
    //char SAVEname[300];  //選択されているプリセット位置
    int headPosition = file.get_preset();
    //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%c%c%c%c%c.txt", file.get_preset()+1,0x40+name[headPosition][0],0x40+name[headPosition][1],0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5],0x40+name[headPosition][6],0x40+name[headPosition][7],0x40+name[headPosition][8],0x40+name[headPosition][9]);
    //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%c.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),0x40+no_chara(headPosition,5));
    //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%c.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5]);
    if(time_reset==true){
        sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%03d.TXT",file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
    }else{
        sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%cdate-%04d-%02d-%02d_%02dh%02dm_%03d.TXT",file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),year,mon,mday,hour,min,SerialNumber);
    }
    int rank_en=0;
    int rank_no=0;
    if(msc.disk_initialize()!=1) {
        FILE *fp4 = fopen(SAVEFILE, "w");
        if(fp4 == NULL) {
            //error("Could not open file for write\n");
        } else {
            fprintf(fp4, "Working hours : %2dh%02dm\r\n",ji,hun);
            fprintf(fp4, "set_No : %02d\r\n",headPosition+1);
            fprintf(fp4, "user   :");
            fprintf(fp4, "%c%c%c%c%c%c%c%c%c%c",0x40+name[headPosition][0],0x40+name[headPosition][1],0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5],0x40+name[headPosition][6],0x40+name[headPosition][7],0x40+name[headPosition][8],0x40+name[headPosition][9]);
            fprintf(fp4, "\r\n");
            for(int i = 0;i<10;i++){
                rank_en=0;
                if      (i==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
                }else if(i==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
                }else if(i==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
                }else if(i==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
                }else if(i==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
                }else if(i==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
                }else if(i==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
                }else if(i==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
                }else if(i==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
                }else                                 { rank_en=0;rank_no=0;
                }
                if(rank_en == 1){
                    if(rank_no > CG_RANK_START){
                        rank_no = rank_no - (CG_RANK_START-C_RANK_START);
                    }
                    if(CR_01<=rank_no&&rank_no<=CR_99){
                        fprintf(fp4,"%2d\t:",rank_no-(CR_01-1));
                    }else if(C_9S<=rank_no&&rank_no<=C_MA){
                        if(C_9S<=rank_no&&rank_no<=C_2S){
                            fprintf(fp4, "%dS\t:",C_S-rank_no+1);
                        }else if(C_2L<=rank_no&&rank_no<=C_9L){
                            fprintf(fp4, "%dL\t:",rank_no-C_L+1);
                        }else{
                            switch(rank_no) {
                                case(C_S):  fprintf(fp4, "S\t:");   break;
                                case(C_M):  fprintf(fp4, "M\t:");   break;
                                case(C_L):  fprintf(fp4, "L\t:");   break;
                                case(C_SA): fprintf(fp4, "SA\t:");  break;
                                case(C_MA): fprintf(fp4, "MA\t:");  break;
                            }
                        }
                    }
                    if      (i==1){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u01.rank_max,mweight.u02.rank_max);
                    }else if(i==2){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u02.rank_max,mweight.u03.rank_max);
                    }else if(i==3){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u03.rank_max,mweight.u04.rank_max);
                    }else if(i==4){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u04.rank_max,mweight.u05.rank_max);
                    }else if(i==5){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u05.rank_max,mweight.u06.rank_max);
                    }else if(i==6){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u06.rank_max,mweight.u07.rank_max);
                    }else if(i==7){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u07.rank_max,mweight.u08.rank_max);
                    }else if(i==8){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u08.rank_max,mweight.u09.rank_max);
                    }else if(i==9){
                        fprintf(fp4, "%4d ~ %4d\r\n",mweight.u09.rank_max,mweight.u10.rank_max);
                    }
                }
            }
            rankdata_sort(Rsort);
            fprintf(fp4, "\r\n");
            fprintf(fp4, "\r\n");
            fprintf(fp4, "\r\n");
            fprintf(fp4, "\r\n");
            fprintf(fp4, "CLASS\t:RANK\t:Qty\t:TOTAL\r\n");
            fprintf(fp4, "A\r\n");
            rank_en=0;
            for(int rl=0; rl<12; rl++) {
                printf("%2d\t:NGl\t: %5d\t: %8.1f\r\n",rl,totalrankerA[rl],totalweightA[rl]);
            }
            for(int rl=0; rl<10; rl++) {
                if      (rl==0)                        { rank_en=0;
                }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
                }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
                }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
                }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
                }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
                }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
                }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
                }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
                }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
                }else                                  { rank_en+=1;
                }
                if(rank_en==0){
                    //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                    fprintf(fp4, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerA[0],totalweightA[0]);
                }
                if(rank_en==1){
                    if(CR_01<=rank_no&&rank_no<=CR_99){
                        fprintf(fp4,"     \t:%2d\t:",rank_no-(CR_01-1));
                    }else if(C_9S<=rank_no&&rank_no<=C_MA){
                        if(C_9S<=rank_no&&rank_no<=C_2S){
                            fprintf(fp4, "     \t:%dS\t:",C_S-rank_no+1);
                        }else if(C_2L<=rank_no&&rank_no<=C_9L){
                            fprintf(fp4, "     \t:%dL\t:",rank_no-C_L+1);
                        }else{
                            switch(rank_no) {
                                case(C_S):  fprintf(fp4, "     \t:S\t:");   break;
                                case(C_M):  fprintf(fp4, "     \t:M\t:");   break;
                                case(C_L):  fprintf(fp4, "     \t:L\t:");   break;
                                case(C_SA): fprintf(fp4, "     \t:SA\t:");  break;
                                case(C_MA): fprintf(fp4, "     \t:MA\t:");  break;
                            }
                        }
                    }
                    fprintf(fp4,      " %5d\t: %8.1f\r\n",totalrankerA[rl],totalweightA[rl]);
                }else if(rank_en==2){
                }
            }
            fprintf(fp4, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerA[10],totalweightA[10]);
            
            
            if(Rsort>0) {
                fprintf(fp4, "B\r\n");
                rank_en=0;
                for(int rl=0; rl<10; rl++) {
                    //if(rl >= start_r && rl <= end_r) {
                if      (rl==0)                        { rank_en=0;
                }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
                }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
                }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
                }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
                }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
                }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
                }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
                }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
                }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
                }else                                  { rank_en+=1;
                    }
                    if(rank_en==0){
                        //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                        fprintf(fp4, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerB[0],totalweightB[0]);
                    }
                    if(rank_en==1){
                    if(CR_01<=rank_no&&rank_no<=CR_99){
                        fprintf(fp4, "     \t:%2d\t:",rank_no-(CR_01-1));
                    }else if(C_9S<=rank_no&&rank_no<=C_MA){
                        if(C_9S<=rank_no&&rank_no<=C_2S){
                            fprintf(fp4, "     \t:%dS\t:",C_S-rank_no+1);
                        }else if(C_2L<=rank_no&&rank_no<=C_9L){
                            fprintf(fp4, "     \t:%dL\t:",rank_no-C_L+1);
                        }else{
                            switch(rank_no) {
                                case(C_S):  fprintf(fp4, "     \t:S\t:");   break;
                                case(C_M):  fprintf(fp4, "     \t:M\t:");   break;
                                case(C_L):  fprintf(fp4, "     \t:L\t:");   break;
                                case(C_SA): fprintf(fp4, "     \t:SA\t:");  break;
                                case(C_MA): fprintf(fp4, "     \t:MA\t:");  break;
                            }
                        }
                    }
                        fprintf(fp4,      " %5d\t: %8.1f\r\n",totalrankerB[rl],totalweightB[rl]);
                    }else if(rank_en==2){
                    }
                }
                //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[7],totalweightA[7]);
                fprintf(fp4, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerB[10],totalweightB[10]);
            }    
            
            
            
            if(Rsort>2) {
                fprintf(fp4, "C\r\n");
                rank_en=0;
                for(int rl=0; rl<10; rl++) {
                    //if(rl >= start_r && rl <= end_r) {
                    if      (rl==0)                        { rank_en=0;
                    }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
                    }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
                    }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
                    }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
                    }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
                    }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
                    }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
                    }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
                    }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
                    }else                                  { rank_en+=1;
                    }
                    if(rank_en==0){
                        //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                        fprintf(fp4, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerC[0],totalweightC[0]);
                    }
                    if(rank_en==1){
                        if(CR_01<=rank_no&&rank_no<=CR_99){
                            fprintf(fp4, "     \t:%2d\t:",rank_no-(CR_01-1));
                        }else if(C_9S<=rank_no&&rank_no<=C_MA){
                            if(C_9S<=rank_no&&rank_no<=C_2S){
                                fprintf(fp4, "     \t:%dS\t:",C_S-rank_no+1);
                            }else if(C_2L<=rank_no&&rank_no<=C_9L){
                                fprintf(fp4, "     \t:%dL\t:",rank_no-C_L+1);
                            }else{
                                switch(rank_no) {
                                    case(C_S):  fprintf(fp4, "     \t:S\t:");   break;
                                    case(C_M):  fprintf(fp4, "     \t:M\t:");   break;
                                    case(C_L):  fprintf(fp4, "     \t:L\t:");   break;
                                    case(C_SA): fprintf(fp4, "     \t:SA\t:");  break;
                                    case(C_MA): fprintf(fp4, "     \t:MA\t:");  break;
                                }
                            }
                        }
                        fprintf(fp4,      " %5d\t: %8.1f\r\n",totalrankerC[rl],totalweightC[rl]);
                    }else if(rank_en==2){
                    }
                }
                //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[7],totalweightA[7]);
                fprintf(fp4, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerC[10],totalweightC[10]);
            }
            if(Rsort>4) {
                fprintf(fp4, "D\r\n");
                rank_en=0;
                for(int rl=0; rl<10; rl++) {
                    //if(rl >= start_r && rl <= end_r) {
                if      (rl==0)                        { rank_en=0;
                }else if(rl==1&&mweight.u02.rank_en==1){ rank_en=1;rank_no=mweight.u02.rank_no;
                }else if(rl==2&&mweight.u03.rank_en==1){ rank_en=1;rank_no=mweight.u03.rank_no;
                }else if(rl==3&&mweight.u04.rank_en==1){ rank_en=1;rank_no=mweight.u04.rank_no;
                }else if(rl==4&&mweight.u05.rank_en==1){ rank_en=1;rank_no=mweight.u05.rank_no;
                }else if(rl==5&&mweight.u06.rank_en==1){ rank_en=1;rank_no=mweight.u06.rank_no;
                }else if(rl==6&&mweight.u07.rank_en==1){ rank_en=1;rank_no=mweight.u07.rank_no;
                }else if(rl==7&&mweight.u08.rank_en==1){ rank_en=1;rank_no=mweight.u08.rank_no;
                }else if(rl==8&&mweight.u09.rank_en==1){ rank_en=1;rank_no=mweight.u09.rank_no;
                }else if(rl==9&&mweight.u10.rank_en==1){ rank_en=1;rank_no=mweight.u10.rank_no;
                }else                                  { rank_en+=1;
                    }
                    if(rank_en==0){
                        //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[0],totalweightA[0]);
                        fprintf(fp4, "     \t:NGs\t: %5d\t: %8.1f\r\n",totalrankerD[0],totalweightD[0]);
                    }
                    if(rank_en==1){
                        if(CR_01<=rank_no&&rank_no<=CR_99){
                            fprintf(fp4, "     \t:%2d\t:",rank_no-(CR_01-1));
                        }else if(C_9S<=rank_no&&rank_no<=C_MA){
                            if(C_9S<=rank_no&&rank_no<=C_2S){
                                fprintf(fp4, "     \t:%dS\t:",C_S-rank_no+1);
                            }else if(C_2L<=rank_no&&rank_no<=C_9L){
                                fprintf(fp4, "     \t:%dL\t:",rank_no-C_L+1);
                            }else{
                                switch(rank_no) {
                                    case(C_S):  fprintf(fp4, "     \t:S\t:");   break;
                                    case(C_M):  fprintf(fp4, "     \t:M\t:");   break;
                                    case(C_L):  fprintf(fp4, "     \t:L\t:");   break;
                                    case(C_SA): fprintf(fp4, "     \t:SA\t:");  break;
                                    case(C_MA): fprintf(fp4, "     \t:MA\t:");  break;
                                }
                            }
                        }
                        fprintf(fp4,      " %5d\t: %8.1f\r\n",totalrankerD[rl],totalweightD[rl]);
                    }else if(rank_en==2){
                    }
                }
                //fprintf(fp4, " NG : %5d : %8.0f :\r\n",totalrankerA[7],totalweightA[7]);
                fprintf(fp4, "     \t:NGl\t: %5d\t: %8.1f\r\n",totalrankerD[10],totalweightD[10]);
            }
        }
        fclose(fp4);
        fp4 = NULL;
    } else {
        lcd.blank(0);
        lcd.blank(1);
        lcd.locate(0,0);
        lcd.printf(SAVEER[0]);
        lcd.locate(0,1);
        lcd.printf(SAVEER[1]);
        se.chime_01();    //警告音
        //wait(3);
        lcd.blank(0);
        lcd.blank(1);
    }
}
int USB_filecheck(){
    char SAVEER[2][16]= {{0xB4,0xD7,0xB0,0x21,0x21,0x00}   //ｴﾗｰ!!
        ,{'U' ,'S' ,'B' ,0xB6,0xDE,0xD0,0xC2,0xB6,0xD8,0xCF,0xBE,0xDD,0x00}
    };    //USBｶﾞﾐﾂｶﾘﾏｾﾝ
    int SerialNumber=0;
    char SAVEFILE[300];  //選択されているプリセット位置
    int headPosition = file.get_preset();
    //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%03d.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
    sprintf(SAVEFILE,"/usb/%02dtotal%03d.TXT",file.get_preset()+1,SerialNumber);
 
    if(msc.disk_initialize()!=1) {
        while(SerialNumber < 999){
            //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%03d.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
            sprintf(SAVEFILE,"/usb/%02dtotal%03d.TXT",file.get_preset()+1,SerialNumber);
            FILE *fp4 = fopen(SAVEFILE, "r");
            //printf("%d\r\n",SerialNumber);
            if(fp4 == NULL) {
                //FILE *fp5 = fopen(SAVEFILE, "a");
                //if(fp5 == NULL) {printf("NULL\r\n");}
                //fclose(fp5);
                //fp4 = NULL;
                printf("blank:%02d%c%c%c%c%c%03d.txt\r\n", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
                printf("%02dtotal%03d.TXT",file.get_preset()+1,SerialNumber);
                return SerialNumber;
            }else{
                fclose(fp4);
                fp4 = NULL;
                SerialNumber += 1;
            }
        }
        return 0;//0含めて999+1個既にあるので、0に上書き
    }else{
        lcd.blank(0);
        lcd.blank(1);
        lcd.locate(0,0);
        lcd.printf(SAVEER[0]);
        lcd.locate(0,1);
        lcd.printf(SAVEER[1]);
        se.chime_01();    //警告音
        //wait(3);
        lcd.blank(0);
        lcd.blank(1);
        return 1000;
    }
}

int USB_filecheck2(int year,int mon,int mday,int hour,int min){
    char SAVEER[2][16]= {{0xB4,0xD7,0xB0,0x21,0x21,0x00}   //ｴﾗｰ!!
        ,{'U' ,'S' ,'B' ,0xB6,0xDE,0xD0,0xC2,0xB6,0xD8,0xCF,0xBE,0xDD,0x00}
    };    //USBｶﾞﾐﾂｶﾘﾏｾﾝ
    int SerialNumber=0;
    char SAVEFILE[300];  //選択されているプリセット位置
    int headPosition = file.get_preset();
    //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%03d.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
    if(time_reset==true){
        sprintf(SAVEFILE,"/usb/%02dtotal%03d.TXT",file.get_preset()+1,SerialNumber);
    }else{
        sprintf(SAVEFILE,"/usb/%02ddate-%04d-%02d-%02d_%02dh%02dm_%03d.TXT",file.get_preset()+1,year,mon,mday,hour,min,SerialNumber);
    }
 
    if(msc.disk_initialize()!=1) {
        while(SerialNumber < 999){
            //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%03d.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
            if(time_reset==true){
                sprintf(SAVEFILE,"/usb/%02dtotal%03d.TXT",file.get_preset()+1,SerialNumber);
            }else{
                sprintf(SAVEFILE,"/usb/%02ddate-%04d-%02d-%02d_%02dh%02dm_%03d.TXT",file.get_preset()+1,year,mon,mday,hour,min,SerialNumber);
            }
            FILE *fp4 = fopen(SAVEFILE, "r");
            //printf("%d\r\n",SerialNumber);
            if(fp4 == NULL) {
                //FILE *fp5 = fopen(SAVEFILE, "a");
                //if(fp5 == NULL) {printf("NULL\r\n");}
                //fclose(fp5);
                //fp4 = NULL;
                //printf("blank:%02d%c%c%c%c%c%03d.txt\r\n", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
                printf("%02ddate%4d_%02d_%02d_%02d_%02d__%03d.TXT",file.get_preset()+1,year,mon,mday,hour,min,SerialNumber);
                //printf("%02dtotal%03d.TXT",file.get_preset()+1,SerialNumber);
                return SerialNumber;
            }else{
                fclose(fp4);
                fp4 = NULL;
                SerialNumber += 1;
            }
        }
        return 0;//0含めて999+1個既にあるので、0に上書き
    }else{
        lcd.blank(0);
        lcd.blank(1);
        lcd.locate(0,0);
        lcd.printf(SAVEER[0]);
        lcd.locate(0,1);
        lcd.printf(SAVEER[1]);
        se.chime_01();    //警告音
        //wait(3);
        lcd.blank(0);
        lcd.blank(1);
        return 1000;
    }
}
int USB_filecheck3(int year,int mon,int mday,int hour,int min){
    char SAVEER[2][16]= {{0xB4,0xD7,0xB0,0x21,0x21,0x00}   //ｴﾗｰ!!
        ,{'U' ,'S' ,'B' ,0xB6,0xDE,0xD0,0xC2,0xB6,0xD8,0xCF,0xBE,0xDD,0x00}
    };    //USBｶﾞﾐﾂｶﾘﾏｾﾝ
    int SerialNumber=0;
    char SAVEFILE[300];  //選択されているプリセット位置
    int headPosition = file.get_preset();
    //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%03d.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
    if(time_reset==true){
        sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%03d.TXT",file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
    }else{
        sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%cdate-%04d-%02d-%02d_%02dh%02dm_%03d.TXT",file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),year,mon,mday,hour,min,SerialNumber);
    }
 
    if(msc.disk_initialize()!=1) {
        while(SerialNumber < 999){
            //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%03d.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
            if(time_reset==true){
                sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%03d.TXT",file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
            }else{
                sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%cdate-%04d-%02d-%02d_%02dh%02dm_%03d.TXT",file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),year,mon,mday,hour,min,SerialNumber);
            }
            FILE *fp4 = fopen(SAVEFILE, "r");
            //printf("%d\r\n",SerialNumber);
            if(fp4 == NULL) {
                //FILE *fp5 = fopen(SAVEFILE, "a");
                //if(fp5 == NULL) {printf("NULL\r\n");}
                //fclose(fp5);
                //fp4 = NULL;
                //printf("blank:%02d%c%c%c%c%c%03d.txt\r\n", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),SerialNumber);
                printf("%02ddate%4d_%02d_%02d_%02d_%02d__%03d.TXT",file.get_preset()+1,year,mon,mday,hour,min,SerialNumber);
                //printf("%02dtotal%03d.TXT",file.get_preset()+1,SerialNumber);
                return SerialNumber;
            }else{
                fclose(fp4);
                fp4 = NULL;
                SerialNumber += 1;
            }
        }
        return 0;//0含めて999+1個既にあるので、0に上書き
    }else{
        lcd.blank(0);
        lcd.blank(1);
        lcd.locate(0,0);
        lcd.printf(SAVEER[0]);
        lcd.locate(0,1);
        lcd.printf(SAVEER[1]);
        se.chime_01();    //警告音
        //wait(3);
        lcd.blank(0);
        lcd.blank(1);
        return 1000;
    }
}


/*
void USB_rankdata(int Rsort ,int ji,int hun)
{
    char SAVEER[2][16]= {{0xB4,0xD7,0xB0,0x21,0x21,0x00}   //ｴﾗｰ!!
        ,{'U' ,'S' ,'B' ,0xB6,0xDE,0xD0,0xC2,0xB6,0xD8,0xCF,0xBE,0xDD,0x00}
    };    //USBｶﾞﾐﾂｶﾘﾏｾﾝ
    char SAVEFILE[300];  //選択されているプリセット位置
    //char SAVEname[300];  //選択されているプリセット位置
    int headPosition = file.get_preset();
    //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%c%c%c%c%c.txt", file.get_preset()+1,0x40+name[headPosition][0],0x40+name[headPosition][1],0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5],0x40+name[headPosition][6],0x40+name[headPosition][7],0x40+name[headPosition][8],0x40+name[headPosition][9]);
    sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%c.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+no_chara(headPosition,2),0x40+no_chara(headPosition,3),0x40+no_chara(headPosition,4),0x40+no_chara(headPosition,5));
    //sprintf(SAVEFILE,"/usb/%02d%c%c%c%c%c%c.txt", file.get_preset()+1,0x40+no_chara(headPosition,0),0x40+no_chara(headPosition,1),0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5]);
    if(msc.disk_initialize()!=1) {
        FILE *fp4 = fopen(SAVEFILE, "w");
        if(fp4 == NULL) {
            //error("Could not open file for write\n");
        } else {
            fprintf(fp4, "Working hours : %2dh%02dm\r\n",ji,hun);
            fprintf(fp4, "set_No : %02d\r\n",headPosition+1);
            fprintf(fp4, "user   :");
            fprintf(fp4, "%c%c%c%c%c%c%c%c%c%c",0x40+name[headPosition][0],0x40+name[headPosition][1],0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5],0x40+name[headPosition][6],0x40+name[headPosition][7],0x40+name[headPosition][8],0x40+name[headPosition][9]);
            fprintf(fp4, "\r\n");
            if(mweight.u02.rank_en==1) {
                //fprintf(fp4, "3S :%4d.0 ~ %4d.9\r\n",mweight.u01.rank_max,mweight.u02.rank_max);
                fprintf(fp4, " M :%4d ~ %4d\r\n",mweight.u01.rank_max,mweight.u02.rank_max);
            }
            if(mweight.u03.rank_en==1) {
                //fprintf(fp4, " \\\\\\\\\\\\\\\\\\\\m :%4d.0 ~ %4d.9\r\n",mweight.u02.rank_max,mweight.u03.rank_max);
                fprintf(fp4, " L :%4d ~ %4d\r\n",mweight.u02.rank_max,mweight.u03.rank_max);
            }
            if(mweight.u04.rank_en==1) {
                //fprintf(fp4, " S :%4d.0 ~ %4d.9\r\n",mweight.u03.rank_max,mweight.u04.rank_max);
                fprintf(fp4, " 2L :%4d ~ %4d\r\n",mweight.u03.rank_max,mweight.u04.rank_max);
            }
            //fprintf(fp4, " 3L :%4d.0 ~ %4d.9\r\n",mweight.u04.rank_max,mweight.u05.rank_max);
            fprintf(fp4, " 3L :%4d ~ %4d\r\n",mweight.u04.rank_max,mweight.u05.rank_max);
            if(mweight.u06.rank_en==1) {
                //fprintf(fp4, " 4L :%4d.0 ~ %4d.9\r\n",mweight.u05.rank_max,mweight.u06.rank_max);
                fprintf(fp4, " 4L :%4d ~ %4d\r\n",mweight.u05.rank_max,mweight.u06.rank_max);
            }
            if(mweight.u07.rank_en==1) {
                //fprintf(fp4, "5L :%4d.0 ~ %4d.9\r\n",mweight.u06.rank_max,mweight.u07.rank_max);
                fprintf(fp4, "5L :%4d ~ %4d\r\n",mweight.u06.rank_max,mweight.u07.rank_max);
            }
            if(mweight.u08.rank_en==1) {
                //fprintf(fp4, "6L :%4d.0 ~ %4d.9\r\n",mweight.u07.rank_max,mweight.u08.rank_max);
                fprintf(fp4, "6L :%4d ~ %4d\r\n",mweight.u07.rank_max,mweight.u08.rank_max);
            }
            fprintf(fp4, "\r\n");
            fprintf(fp4, "\r\n");
            fprintf(fp4, "         :");
            for(int rl=0; rl<10; rl++) {
                if(rl >= start_r && rl <= end_r) {
                    switch(rl) {
                        case R_NG1:
                            fprintf(fp4, "     NG   :");
                            break;
                        case R_M:
                            fprintf(fp4, "      M   :");
                            break;
                        case R_L:
                            fprintf(fp4, "     L   :");
                            break;
                        case R_2L:
                            fprintf(fp4, "    2L   :");
                            break;
                        case R_3L:
                            fprintf(fp4, "     3L   :");
                            break;
                        case R_4L:
                            fprintf(fp4, "     4L   :");
                            break;
                        case R_5L:
                            fprintf(fp4, "     5L   :");
                            break;
                        case R_6L:
                            fprintf(fp4, "     6L   :");
                            break;
                        case R_NG2:
                            fprintf(fp4, "     NG   :");
                            break;
                        case R_NG3:
                            fprintf(fp4, "     NG   :");
                            break;
                    }
                }
            }
            fprintf(fp4, "\r\n");

            for(int wl=0; wl<4; wl++) {
                switch(wl) {
                    case 0 :
                        fprintf(fp4, "A_RANK   :");
                        for(int rl=0; rl<10; rl++) {
                            if(rl >= start_r && rl <= end_r) {
                                fprintf(fp4, "%8d  :",totalrankerA[rl]);
                            }
                        }
                        fprintf(fp4, "\r\n");
                        break;
                    case 1 :
                        if(Rsort>0) {
                            fprintf(fp4, "B_RANK   :");
                            for(int rl=0; rl<10; rl++) {
                                if(rl >= start_r && rl <= end_r) {
                                    fprintf(fp4, "%8d  :",totalrankerB[rl]);
                                }
                            }
                            fprintf(fp4, "\r\n");
                        }
                        break;
                    case 2 :
                        if(Rsort>2) {
                            fprintf(fp4, "C_RANK   :");
                            for(int rl=0; rl<10; rl++) {
                                if(rl >= start_r && rl <= end_r) {
                                    fprintf(fp4, "%8d  :",totalrankerC[rl]);
                                }
                            }
                            fprintf(fp4, "\r\n");
                        }
                        break;
                    case 3 :
                        if(Rsort>4) {
                            fprintf(fp4, "D_RANK   :");
                            for(int rl=0; rl<10; rl++) {
                                if(rl >= start_r && rl <= end_r) {
                                    fprintf(fp4, "%8d  :",totalrankerD[rl]);
                                }
                            }
                            fprintf(fp4, "\r\n");
                        }
                        break;
                }
            }

            for(int wl=0; wl<4; wl++) {
                switch(wl) {
                    case 0 :
                        fprintf(fp4, "A_WEIGHT :");
                        for(int rl=0; rl<10; rl++) {
                            if(rl >= start_r && rl <= end_r) {
                                fprintf(fp4, "%8.1f  :",totalweightA[rl]);
                            }
                        }
                        fprintf(fp4, "\r\n");
                        break;
                    case 1 :
                        if(Rsort>0) {
                            fprintf(fp4, "B_WEIGHT :");
                            for(int rl=0; rl<10; rl++) {
                                if(rl >= start_r && rl <= end_r) {
                                    fprintf(fp4, "%8.1f  :",totalweightB[rl]);
                                }
                            }
                            fprintf(fp4, "\r\n");
                        }
                        break;
                    case 2 :
                        if(Rsort>2) {
                            fprintf(fp4, "C_WEIGHT :");
                            for(int rl=0; rl<10; rl++) {
                                if(rl >= start_r && rl <= end_r) {
                                    fprintf(fp4, "%8.1f  :",totalweightC[rl]);
                                }
                            }
                            fprintf(fp4, "\r\n");
                        }
                        break;
                    case 3 :
                        if(Rsort>4) {
                            fprintf(fp4, "D_WEIGHT :");
                            for(int rl=0; rl<10; rl++) {
                                if(rl >= start_r && rl <= end_r) {
                                    fprintf(fp4, "%8.1f  :",totalweightD[rl]);
                                }
                            }
                            fprintf(fp4, "\r\n");
                        }
                        break;
                }
            }
        }
        fclose(fp4);
        fp4 = NULL;
    } else {
        lcd.blank(0);
        lcd.blank(1);
        lcd.locate(0,0);
        lcd.printf(SAVEER[0]);
        lcd.locate(0,1);
        lcd.printf(SAVEER[1]);
        se.chime_01();    //警告音
        //wait(3);
        lcd.blank(0);
        lcd.blank(1);
    }
}*/
void USB_rankdata2(int Rsort,int d_target,int d_jougen,int ji,int hun)
{
    char SAVEER[2][16]= {{0xB4,0xD7,0xB0,0x21,0x21,0x00}   //ｴﾗｰ!!
        ,{'U' ,'S' ,'B' ,0xB6,0xDE,0xD0,0xC2,0xB6,0xD8,0xCF,0xBE,0xDD,0x00}
    };    //USBｶﾞﾐﾂｶﾘﾏｾﾝ
    char SAVEFILE[300];  //選択されているプリセット位置
    //char SAVEname[300];  //選択されているプリセット位置
    int headPosition = file.get_preset();

    sprintf(SAVEFILE,"/usb/%02dcheck.TXT",file.get_preset()+1);
    if(msc.disk_initialize()!=1) {
        FILE *fp4 = fopen(SAVEFILE, "w");
        if(fp4 == NULL) {
            //error("Could not open file for write\n");
        } else {
            fprintf(fp4, "Working hours : %2dh%02dm\r\n",ji,hun);
            fprintf(fp4, "set_No : %02d\r\n",headPosition+1);
            fprintf(fp4, "target : %4d ~ %4d \r\n",d_target,d_target+d_jougen);
            fprintf(fp4, "total  : %d\r\n",totalranker0103[2] + totalranker0406[2] + totalranker0709[2] + totalranker1012[2]);
            fprintf(fp4, "\r\n");
        }
        fclose(fp4);
        fp4 = NULL;
    } else {
        lcd.blank(0);
        lcd.blank(1);
        lcd.locate(0,0);
        lcd.printf(SAVEER[0]);
        lcd.locate(0,1);
        lcd.printf(SAVEER[1]);
        se.chime_01();    //警告音
        //wait(3);
        lcd.blank(0);
        lcd.blank(1);
    }
}
void save_rankdata2(int Rsort,int d_target,int d_jougen,int ji,int hun)
{
    char SAVEFILE[300];  //選択されているプリセット位置
    //char SAVEname[300];  //選択されているプリセット位置
    int headPosition = file.get_preset();

    sprintf(SAVEFILE,"/local/%02dcheck.TXT",file.get_preset()+1);
    FILE *fp4 = fopen(SAVEFILE, "w");
    if(fp4 == NULL) {
        //error("Could not open file for write\n");
    } else {
        fprintf(fp4, "Working hours : %2dh%02dm\r\n",ji,hun);
        fprintf(fp4, "set_No : %02d\r\n",headPosition+1);
        fprintf(fp4, "target : %4d ~ %4d \r\n",d_target,d_target+d_jougen);
        fprintf(fp4, "total  : %d\r\n",totalranker0103[2] + totalranker0406[2] + totalranker0709[2] + totalranker1012[2]);
        fprintf(fp4, "\r\n");
    }
    fclose(fp4);
    fp4 = NULL;
}
void save_sagyou_file(int total_kumi_cnt,float average_kumi_value,int d_target,int d_jougen)
{
    char SAVEFILE1[300];  //選択されているプリセット位置
    char SAVEFILE2[300];  //選択されているプリセット位置
    char test;
    char test2;
    char test3;
    char SAVEER[2][16]= {{0xB4,0xD7,0xB0,0x21,0x21,0x00}   //ｴﾗｰ!!
        ,{'U' ,'S' ,'B' ,0xB6,0xDE,0xD0,0xC2,0xB6,0xD8,0xCF,0xBE,0xDD,0x00}
    };    //USBｶﾞﾐﾂｶﾘﾏｾﾝ
    sprintf(SAVEFILE1,"/local/00INDI.TXT");
    sprintf(SAVEFILE2,"/usb/%02dtotal.TXT",file.get_preset()+1);
    //sprintf(SAVEFILE2,"/local/%02dtotal.TXT",file.get_preset()+1);
    FILE *fp;
    FILE *sfp;

    if ((fp = fopen(SAVEFILE1,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "PRE");
        exit(EXIT_FAILURE);
    } else {
        if(option.usb_EN==true) {
            if (msc.disk_initialize()!=1) {
                //if (1==1){
                sfp = fopen(SAVEFILE2,"w");
                if(sfp== NULL) {
                    //fprintf(stderr, "%s write Err.\n", "PRE");
                    //exit(EXIT_FAILURE);
                } else {
                    fprintf(sfp, "set_No : %d\r\n",file.get_preset()+1);
                    fprintf(sfp, "target : %4d ~ %4d \r\n",d_target,d_target+d_jougen);
                    fprintf(sfp, "total  : %d\r\n",total_kumi_cnt);
                    fprintf(sfp, "average: %6.1f\r\n",average_kumi_value);
                    fprintf(sfp, "\r\n");
                    fprintf(sfp, "Qty      Weight\r\n");
                    fscanf(fp, "%c",&test);
                    pc.printf("%c",test);
                    while(test!='\n'&& test!=NULL) {
                        fprintf(sfp,"%c",test);
                        test=NULL;
                        fscanf(fp, "%c",&test);
                        if(test=='\n'||test==NULL) {
                            fscanf(fp, "%c",&test2);
                            fscanf(fp, "%c",&test3);
                            if(test3!=NULL) {
                                fprintf(sfp, "%c%c%c",test,test2,test3);
                                test2=NULL;
                                test3=NULL;
                                fscanf(fp, "%c",&test);
                            }
                        }
                    }
                    fprintf(sfp,"%c",test);
                }
                fclose(sfp);
                sfp = NULL;
            } else {   //USBが刺さっていない時
                lcd.blank(0);
                lcd.blank(1);
                lcd.locate(0,0);
                lcd.printf(SAVEER[0]);
                lcd.locate(0,1);
                lcd.printf(SAVEER[1]);
                se.chime_01();    //警告音
                //wait(3);
                lcd.blank(0);
                lcd.blank(1);
            }
        }
    }
    fclose(fp);
    fp = NULL;
}

void save_sagyou_file2(int total_kumi_cnt,float average_kumi_value,int d_target,int d_jougen ,int ji,int hun,int SerialNumber)
{
    char SAVEFILE1[300];  //選択されているプリセット位置
    char SAVEFILE2[300];  //選択されているプリセット位置
    char test;
    char test2;
    char test3;
    //int save_cnt=1;
    char SAVEER[2][16]= {{0xB4,0xD7,0xB0,0x21,0x21,0x00}   //ｴﾗｰ!!
        ,{'U' ,'S' ,'B' ,0xB6,0xDE,0xD0,0xC2,0xB6,0xD8,0xCF,0xBE,0xDD,0x00}
    };    //USBｶﾞﾐﾂｶﾘﾏｾﾝ
    //char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
    //               ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼﾀ
    sprintf(SAVEFILE1,"/local/00INDI.TXT");
    //sprintf(SAVEFILE1,"/usb/00INDI.TXT");
    //sprintf(SAVEFILE2,"/usb/%02dtotal.TXT",file.get_preset()+1);
    //sprintf(SAVEFILE2,"/local/%02dtotal.TXT",file.get_preset()+1);
    sprintf(SAVEFILE2,"/usb/%02dtotal%03d.TXT",file.get_preset()+1,SerialNumber);
    FILE *fp;
    //FILE *fp2;
    FILE *sfp;

    if ((fp = fopen(SAVEFILE1,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "USB");
        exit(EXIT_FAILURE);
    } else {
        if(option.usb_EN==true) {
            if (msc.disk_initialize()!=1) {
                //if (1==1){
                //int lop=0;
                /*while ((fp2 = fopen(SAVEFILE2,"r")) != NULL || lop<100) {
                    //fprintf(stderr, "%s Read Err.\n", "USB");
                    //exit(EXIT_FAILURE);
                    fclose(fp2);
                    fp2 = NULL;
                    lop+=1;
                    sprintf(SAVEFILE2,"/usb/%02dtotal%02d.TXT",file.get_preset()+1,lop);
                }*/
                //fclose(fp2);
                //fp2 = NULL;
                sfp = fopen(SAVEFILE2,"w");
                if(sfp== NULL) {
                    //fprintf(stderr, "%s write Err.\n", "PRE");
                    //exit(EXIT_FAILURE);
                } else {
                    //lcd.locate(0,0);
                    //lcd.printf("%6d",save_cnt);
                    fprintf(sfp, "Working hours : %2dh%02dm\r\n",ji,hun);
                    fprintf(sfp, "set_No : %d\r\n",file.get_preset()+1);
                    fprintf(sfp, "target : %4d ~ %4d \r\n",d_target,d_target+d_jougen);
                    fprintf(sfp, "total  : %d\r\n",total_kumi_cnt);
                    fprintf(sfp, "average: %6.1f\r\n",average_kumi_value);
                    fprintf(sfp, "\r\n");
                    fprintf(sfp, "Qty      Place    Weight    lap\r\n");
                    fscanf(fp, "%c",&test);
                    printf("%c",test);
                    while(test!='\n'&& test!=NULL) {
                        fprintf(sfp,"%c",test);
                        test=NULL;
                        fscanf(fp, "%c",&test);
                        if(test=='\n'||test==NULL) {
                            fscanf(fp, "%c",&test2);
                            fscanf(fp, "%c",&test3);
                            if(test3!=NULL) {
                                fprintf(sfp, "%c%c%c",test,test2,test3);
                                fscanf(fp, "%c",&test);
                                test2=NULL;
                                test3=NULL;
                                //save_cnt+=1;
                                //lcd.locate(0,0);
                                //lcd.printf("%6d",save_cnt);
                            }
                        }
                    }
                    fprintf(sfp,"%c",test);
                }
                fclose(sfp);
                sfp = NULL;
                lcd.blank(1);
                lcd.locate(0,1);
                //lcd.printf(saving[1]);
                //wait(1);
                lcd.blank(1);
            } else {   //USBが刺さっていない時
                lcd.blank(0);
                lcd.blank(1);
                lcd.locate(0,0);
                lcd.printf(SAVEER[0]);
                lcd.locate(0,1);
                lcd.printf(SAVEER[1]);
                se.chime_01();    //警告音
                //wait(3);
                lcd.blank(0);
                lcd.blank(1);
            }
        }
    }
    fclose(fp);
    fp = NULL;
}

void save_sagyou_file2_date(int total_kumi_cnt,float average_kumi_value,int d_target,int d_jougen ,int ji,int hun ,int SerialNumber ,int year,int mon,int mday,int hour,int min)
{
    char SAVEFILE1[300];  //選択されているプリセット位置
    char SAVEFILE2[300];  //選択されているプリセット位置
    char test;
    char test2;
    char test3;
    //int save_cnt=1;
    char SAVEER[2][16]= {{0xB4,0xD7,0xB0,0x21,0x21,0x00}   //ｴﾗｰ!!
        ,{'U' ,'S' ,'B' ,0xB6,0xDE,0xD0,0xC2,0xB6,0xD8,0xCF,0xBE,0xDD,0x00}
    };    //USBｶﾞﾐﾂｶﾘﾏｾﾝ
    //char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
    //               ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼﾀ
    sprintf(SAVEFILE1,"/local/00INDI.TXT");
    //sprintf(SAVEFILE1,"/usb/00INDI.TXT");
    //sprintf(SAVEFILE2,"/local/%02dtotal.TXT",file.get_preset()+1);
    //sprintf(SAVEFILE2,"/usb/%02dtotal%03d.TXT",file.get_preset()+1,SerialNumber);
    if(time_reset==true){
        sprintf(SAVEFILE2,"/usb/%02dtotal%03d.TXT",file.get_preset()+1,SerialNumber);
    }else{
        sprintf(SAVEFILE2,"/usb/%02ddate-%04d-%02d-%02d_%02dh%02dm_%03d.TXT",file.get_preset()+1,year,mon,mday,hour,min,SerialNumber);
    }
    FILE *fp;
    //FILE *fp2;
    FILE *sfp;

    if ((fp = fopen(SAVEFILE1,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "USB");
        exit(EXIT_FAILURE);
    } else {
        if(option.usb_EN==true) {
            if (msc.disk_initialize()!=1) {
                //if (1==1){
                //int lop=0;
                /*while ((fp2 = fopen(SAVEFILE2,"r")) != NULL || lop<100) {
                    //fprintf(stderr, "%s Read Err.\n", "USB");
                    //exit(EXIT_FAILURE);
                    fclose(fp2);
                    fp2 = NULL;
                    lop+=1;
                    sprintf(SAVEFILE2,"/usb/%02dtotal%02d.TXT",file.get_preset()+1,lop);
                }*/
                //fclose(fp2);
                //fp2 = NULL;
                sfp = fopen(SAVEFILE2,"w");
                if(sfp== NULL) {
                    //fprintf(stderr, "%s write Err.\n", "PRE");
                    //exit(EXIT_FAILURE);
                } else {
                    //lcd.locate(0,0);
                    //lcd.printf("%6d",save_cnt);
                    fprintf(sfp, "Working hours : %2dh%02dm\r\n",ji,hun);
                    fprintf(sfp, "set_No : %d\r\n",file.get_preset()+1);
                    fprintf(sfp, "target : %4d ~ %4d \r\n",d_target,d_target+d_jougen);
                    fprintf(sfp, "total  : %d\r\n",total_kumi_cnt);
                    fprintf(sfp, "average: %6.1f\r\n",average_kumi_value);
                    fprintf(sfp, "\r\n");
                    fprintf(sfp, "Qty      Place    Weight    lap\r\n");
                    fscanf(fp, "%c",&test);
                    printf("%c",test);
                    while(test!='\n'&& test!=NULL) {
                        fprintf(sfp,"%c",test);
                        test=NULL;
                        fscanf(fp, "%c",&test);
                        if(test=='\n'||test==NULL) {
                            fscanf(fp, "%c",&test2);
                            fscanf(fp, "%c",&test3);
                            if(test3!=NULL) {
                                fprintf(sfp, "%c%c%c",test,test2,test3);
                                fscanf(fp, "%c",&test);
                                test2=NULL;
                                test3=NULL;
                                //save_cnt+=1;
                                //lcd.locate(0,0);
                                //lcd.printf("%6d",save_cnt);
                            }
                        }
                    }
                    fprintf(sfp,"%c",test);
                }
                fclose(sfp);
                sfp = NULL;
                lcd.blank(1);
                lcd.locate(0,1);
                //lcd.printf(saving[1]);
                //wait(1);
                lcd.blank(1);
            } else {   //USBが刺さっていない時
                lcd.blank(0);
                lcd.blank(1);
                lcd.locate(0,0);
                lcd.printf(SAVEER[0]);
                lcd.locate(0,1);
                lcd.printf(SAVEER[1]);
                se.chime_01();    //警告音
                //wait(3);
                lcd.blank(0);
                lcd.blank(1);
            }
        }
    }
    fclose(fp);
    fp = NULL;
}
void save_sagyou_file3(int total_kumi_cnt,float average_kumi_value,int d_target,int d_jougen ,int ji,int hun ,int SerialNumber){
    char SAVEFILE1[300];  //選択されているプリセット位置
    char SAVEFILE2[300];  //選択されているプリセット位置
    char test;
    char test2;
    char test3;
    //char test4;
    sprintf(SAVEFILE1,"/local/00INDI.TXT");
    //sprintf(SAVEFILE1,"/usb/00INDI.TXT");
    //sprintf(SAVEFILE2,"/usb/%02dtotal.TXT",file.get_preset()+1);
    //sprintf(SAVEFILE2,"/local/%02dtotal.TXT",file.get_preset()+1);
    sprintf(SAVEFILE2,"/local/%02dtotal%03d.TXT",file.get_preset()+1,SerialNumber);
    FILE *fp;
    //FILE *fp2;
    FILE *sfp;
    //int save_cnt=0;
    if ((fp = fopen(SAVEFILE1,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "USB");
        exit(EXIT_FAILURE);
    } else {
        if(option.usb_EN==true) {
            //if (1==1){
            //int lop=0;
            /*while ((fp2 = fopen(SAVEFILE2,"r")) != NULL || lop<100) {
                //fprintf(stderr, "%s Read Err.\n", "USB");
                //exit(EXIT_FAILURE);
                fclose(fp2);
                fp2 = NULL;
                lop+=1;
                sprintf(SAVEFILE2,"/usb/%02dtotal%02d.TXT",file.get_preset()+1,lop);
            }
            fclose(fp2);
            fp2 = NULL;*/
            sfp = fopen(SAVEFILE2,"w");
            if(sfp== NULL) {
                //fprintf(stderr, "%s write Err.\n", "PRE");
                //exit(EXIT_FAILURE);
            } else {
                fprintf(sfp, "Working hours : %2dh%02dm\r\n",ji,hun);
                fprintf(sfp, "set_No : %d\r\n",file.get_preset()+1);
                fprintf(sfp, "target : %4d ~ %4d \r\n",d_target,d_target+d_jougen);
                fprintf(sfp, "total  : %d\r\n",total_kumi_cnt);
                fprintf(sfp, "average: %6.1f\r\n",average_kumi_value);
                fprintf(sfp, "\r\n");
                fprintf(sfp, "Qty      Place    Weight    lap\r\n");
                fscanf(fp, "%c",&test);
                printf("%c",test);
                while(test!='\n'&& test!=NULL) {
                    fprintf(sfp,"%c",test);
                    test=NULL;
                    fscanf(fp, "%c",&test);
                    if(test=='\n'||test==NULL) {
                        fscanf(fp, "%c",&test2);
                        fscanf(fp, "%c",&test3);
                        if(test3!=NULL) {
                            fprintf(sfp, "%c%c%c",test,test2,test3);
                            //printf("%c  %c  %c\r\n",test,test2,test3);
                            fscanf(fp, "%c",&test);
                            test2=NULL;
                            test3=NULL;
                        }
                    }
                }
                fprintf(sfp,"%c",test);
            }
            fclose(sfp);
            sfp = NULL;
        }
    }
    fclose(fp);
    fp = NULL;
}

/////////////////////////////////////////////////////////////////////////////////
//重量組合せ関数
//すべての組合せ結果を計算する！
/////////////////////////////////////////////////////////////////////////////////
//int combi_OK = 0;
void get_combi_location(combiData_t com[], int *secondCnt)
{
    int bit,bk,in = 0; //in:物が載っている場所を取得
    int bit_num=0;    //成立した個数の保存
    double sum=0;   //組み合わせ候補箇所の重量を合算していく
    int min = load.target + param.buffer;  //最小値(バッファー有り)
    int res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    int resDefault = res;                   //上限値のデフォルト値をここでバックアップ
    bool checkBtnJunsaiteki = false;            //ボタン押下で準最適値モードになったか
    bool combi_OK_flug = false;             //１個でも成立した物があるか

    ArrayIni(com, BK_CNT);   //初期化(組み合わせ保持している分を初期化)
    in = cel.getAnteiIn();   //計量物の数を取得

    //計量物ないなら即終了
    if(in == 0) {
        return;
    }

    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)
    float *weight = new float[product.head];
    float maxWeight = 0, minWeight = 0;
    float itiji_weight = 0.0;

    for(int i = 0; i < product.head; i++) {
        itiji_weight = cel.anteiWeight(i);
        if(itiji_weight == 0){
            weight[i] = 0.0;
        }else{
            //weight[i] = cel.anteiWeight(i) + op_n_hosei + hosei[i];
            //weight[i] = itiji_weight + op_n_hosei + hosei[i];
            weight[i] = itiji_weight + op_n_hosei + hosei_f[i];
        }
    }

    //上ボタン(赤)が押されたら強制的に準最適値機能を1回だけON
    if( btn.down_press() == true && btn.get_lock_flg() == false) {
        btn.operation_lock();   //上ボタンを押している間
        res = KINJITI_MAX_WHIGHT;
        checkBtnJunsaiteki = true;  //準最適値機能を強制ON状態
        //準最適値開始する条件が満たされているか確認
        //計量物が必要なだけ皿数、載っているか
    } else if(load.secondOnOff == 1 && (bit_count(in) >= (bscData.startKinji-bit_count(stop_cells)))) {
        *secondCnt += 1;
        if(*secondCnt >= gStartKinjiti) {
            res = KINJITI_MAX_WHIGHT;
        }
        //桁あふれしないように念のため0で初期化
        if(*secondCnt > 1000) {
            *secondCnt = 0;
        }
    } else if(junsaiteki_loop == 1) {
        btn.operation_lock();   //上ボタンを押している間
        res = KINJITI_MAX_WHIGHT;
    } else {
        *secondCnt = 0;
    }
    //if(load.secondOnOff == 1 && (bit_count(in) < (bscData.startKinji-bit_count(stop_cells)))) {
    //    no_combi_v_flg = false;
    //}
    combi_OK = 0;
    //計量物がある分だけ組み合わせ確認を行う
    for(int i=in; i; i--) {

        //組み合わせ候補に重量ゼロがあれば次の組み合わせ候補へ
        if( (in & i) != i) continue;
        
        if(servo_connection==true && moving1 > 0){ //組合せに、組合せに入れられないヘッドがあれば次の組み合わせ候補へ
            if((i & moving1) > 0){continue;} 
        }

        //指定された組み合わせ条件を満たしているか確認
        if(check_combi_condition(i,&load) == false) {
            //printf("combiNG\r\n");
            continue;
        }
        
        
       
        //比較用変数を初期化
        maxWeight = 0;
        minWeight = 6000;

        //ビット1になっている箇所の重量を取得していく
        //bkはbitが立っている場所の重量値を取得するために使用
        //sumは取得する重量値の合計重量を保持
        sum = 0;
        bk = i;

        //総重量取得処理
        for(bit = 0; bk; bk >>= 1) {
            if (bk & 0x01) {
                sum += weight[bit] - MINUS;
                if((load.kumi_flag == KUMI_OYAKO || load.kumi_flag == KUMI_DENSO)
                    && (oneBit[bit] & gOyaHead) > 0){
                }else{
                    if( maxWeight < weight[bit]) maxWeight = weight[bit];   //最重量値の場合は値取得
                    if( minWeight > weight[bit]) minWeight = weight[bit];   //最軽量値の場合は値取得
                }
            }
            bit++;
        }
        //不揃い防止値(設定重量)より最大値、最小値の差があれば処理終了
        if( (maxWeight - minWeight) > load.fuzoroiBoushi && load.fuzoroiBoushi > 0) {
            continue;
        }
        //printf("%6.1f\r\n",sum);
        //printf("%6.1f",sum);
        //if(sum < min || (int(sum) > int(res))){
        //    printf("NG\r\n");
        //}
        //一番最適な重量を取得する
        //if((sum >= min) && (int(sum) <= int(res))) {
        if((load.kumi_flag!=KUMI_LIMIT && (( sum >= min) && (int(sum) <= res))) ||
          ((float(bit_count(i))*param.buffer2<float(res))&&
          (load.kumi_flag==KUMI_LIMIT && (((sum >= (min + (bit_count(i)*param.buffer2))) && (int(sum) <= int(res)))))) ||
          (load.kumi_flag==KUMI_LIMIT && (int(sum) >= int(res)) && (int(sum) <= int(res))))
          
        {
            //printf(":x > min:");

            //printf("\r\n");

            //組み合わせ成立があるならば
            //準最適値処理は行わない
                //printf("OK?\r\n");
                
                //printf("%6.1f:%6d\r\n",sum,int(resDefault));
            if(int(sum) <= int(resDefault)/* || combi_OK != 0*/) {
                *secondCnt = 0;
                combi_OK_flug = true;
                //printf("OK\r\n");
            }
                //printf("OK?\r\n");

            //準最適値「組合せなし」通知は不要
            checkBtnJunsaiteki = false;

            // ******************
            //値を取得
            //*******************
            com[BK_CNT -1].kumi = i;
            com[BK_CNT -1].total = sum;
            bit_num = bit_count(i);
            //組合せ優先(99の場合は使用しない)
            if(load.kumi_priolity_1!=99 && bit_num == load.kumi_priolity_1 + 1) {
                com[BK_CNT -1].priolity =0;
            } else if(load.kumi_priolity_2!=99 && bit_num == load.kumi_priolity_2 + 1) {
                com[BK_CNT -1].priolity =1;
            } else {
                com[BK_CNT -1].priolity =2;
            }
            qsort(com, BK_CNT, sizeof(combiData_t), comp );
            //  for (int j=0;j<BK_CNT -1;j++){
            //       pc.printf("%d  %4f  %2d  \r\n",com[j].kumi, com[j].total,com[j].priolity);
            //  }
            //組合せ優先(99の場合は使用しない)
            if(load.kumi_priolity_1 != 99 || load.kumi_priolity_2 != 99) {
                qsort(com, BK_CNT, sizeof(combiData_t), comp2 );
                // for (int j=0;j<BK_CNT -1;j++){
                //      pc.printf("%d  %4f  %2d  \r\n",com[j].kumi, com[j].total,com[j].priolity);
                //   }
            }

            //res(組合せ重量)を更新
            if(com[BK_CNT-1].total != DAMMY_MAX) {
                //res = com[BK_CNT-1].total;
            }
        }
    }//loop終了

    //組み合わせ成立があるならば
    //準最適値のデータを消す
    if(combi_OK_flug == true){//
        //printf("DELETE\r\n");
        for(int i = 0; i < BK_CNT; i++) {
            //printf("%2d :%4d :%6.1f:%4d\r\n",i,com[BK_CNT].kumi,com[BK_CNT].total,com[BK_CNT].priolity);
            if(com[i].total > int(resDefault)){
                com[i].kumi = 0;
                com[i].total = 1;
                com[i].priolity = 999;
                //printf("reset\r\n");
            }
        }
    }

    //ボタン操作で準最適値モードON後、
    //一つも組合せなければ「組合せがない」ことを通知する
    
    //printf("%5d ,%5d,%5d,%8.1f,%8.1f,%5d%5d ,\r\n",bit_count(in),bscData.startKinji-bit_count(stop_cells),bit_count(stop_cells),res,com[0].total,KINJITI_MAX_WHIGHT,int(sum));
    if(combi_OK == 0 && Compulsion_no_combi_v_flg == false){
        if( ((checkBtnJunsaiteki == true) && (res == KINJITI_MAX_WHIGHT))) {
            v.output_audio(MSG_NO_JUNSAITEKI);
            v.wait_echo();
        }else if((int(com[0].total) == DAMMY_MAX) && (bit_count(in) >= (bscData.startKinji-bit_count(stop_cells)))){
            if((     bit_count(in) >= (product.head-bit_count(stop_cells))) //||   //全ての皿に物が載った
              /*((load.katamen > 0) && (bit_count(in) >= ((bscData.startKinji/2)-bit_count(stop_cells))))*/){
                //if(option.no_combi_v==1){
                if(no_combi_v_flg == false){
                    if(option.no_combi_v==1){
                        v.output_audio(MSG_NO_JUNSAITEKI);
                        v.wait_echo();
                    }else if(option.no_combi_v==2){
                        se.Touch_02();
                        se.Touch_02();
                        se.Touch_02();
                        se.Touch_02();
                    }
                    no_combi_v_flg = true;
                }
            }else if((bit_count(in) >= (bscData.startKinji-bit_count(stop_cells))) //||  //準最適値の設定皿数に達した
              /*((load.katamen > 0) && (bit_count(in) >= ((bscData.startKinji/2)-bit_count(stop_cells))))*/){
                if(no_combi_v_flg == false){
                    if(option.no_combi_v==1){
                        v.output_audio(MSG_NO_JUNSAITEKI);
                        v.wait_echo();
                    }else if(option.no_combi_v==2){
                        se.Touch_02();
                        se.Touch_02();
                        se.Touch_02();
                        se.Touch_02();
                    }
                    no_combi_v_flg = true;
                }
            }
        }
    }else{
        no_combi_v_flg = false;
    }

    delete[] weight;
    return;
}


void get_combi_location_2nd(combiData_t com[], int *secondCnt, int combit )
{
    int bit,bk,in = 0; //in:物が載っている場所を取得
    int bit_num=0;    //成立した個数の保存
    double sum=0;   //組み合わせ候補箇所の重量を合算していく
    int min = float(load.target) + param.buffer;  //最小値(バッファー有り)
    int res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    int resDefault = res;                   //上限値のデフォルト値をここでバックアップ
    bool checkBtnJunsaiteki = false;            //ボタン押下で準最適値モードになったか
    bool combi_OK_flug = false;             //１個でも成立した物があるか

    ArrayIni(com, BK_CNT);   //初期化(組み合わせ保持している分を初期化)
    in = cel.getAnteiIn();   //計量物の数を取得
    

    
    
    //計量物ないなら即終了
    if(in == 0) {
        return;
    }
    
    for(int i = 0; i < BK_CNT; i++) {
        littleCombi[i].kumi = 0;
        littleCombi[i].total = 0;
        littleCombi[i].priolity = 999;
    }
    //printf("02target:%5d | jougen:%5d | kumimi:%3d | kumima:%3d\r\n",load.target,load.jougen,load.kumi_min,load.kumi_max);
   
    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)
    //float *weight = new float[product.head];
    float weight[14];
    float maxWeight = 0, minWeight = 0;

    for(int i = 0; i < product.head; i++) {
        weight[i] = cel.anteiWeight(i);
    }
    //イチゴ親子計量は、親ヘッドの組合せを無視して計算したいので、親ヘッドを消す
    //上記分岐で、「親が無い」事は回避できるハズ
    //printf("%5d\r\n",in);
    in = in ^ (in & (1 << (product.head-1)));
    //printf("%5d\r\n",in);
    //printf("010down_press%3d|lock_flg%3d\r\n",btn.down_press(),btn.get_lock_flg());
    //printf("count%3d|Kinji%3d|\r\n",bit_count(in),bscData.startKinji);

    //printf("in:%4d | set:%4d\r\n",bit_count(in & 127),bscData.startKinji-bit_count(stop_cells));
    //上ボタン(赤)が押されたら強制的に準最適値機能を1回だけON
    if( btn.down_press() == true && btn.get_lock_flg() == false) {
        se.chime_01();
        btn.operation_lock();   //上ボタンを押している間
        res = KINJITI_MAX_WHIGHT;
        checkBtnJunsaiteki = true;  //準最適値機能を強制ON状態
        //準最適値開始する条件が満たされているか確認
        //計量物が必要なだけ皿数、載っているか
    /*} else if(load.kumi_flag ==  KUMI_KAISUOYAKO ||
              load.kumi_flag == KUMI_KAISUOYAKO2){
        } else if(load.secondOnOff == 1 && (bit_count(in) >= (bscData.startKinji-bit_count(stop_cells)))) {
            *secondCnt += 1;
            if(*secondCnt >= gStartKinjiti) {
                res = KINJITI_MAX_WHIGHT;
            }
            //桁あふれしないように念のため0で初期化
            if(*secondCnt > 1000) {
                *secondCnt = 0;
            }
        }*/
    } else if(load.secondOnOff >= 1 && (bit_count(in) >= (bscData.startKinji-bit_count(stop_cells)))) {
        *secondCnt += 1;
        if(*secondCnt >= -30
        ) {
            res = KINJITI_MAX_WHIGHT;
        }
        //桁あふれしないように念のため0で初期化
        if(*secondCnt > 1000) {
            *secondCnt = 0;
        }
    } else if(junsaiteki_loop == 1) {
        btn.operation_lock();   //上ボタンを押している間
        res = KINJITI_MAX_WHIGHT;
    } else {
        *secondCnt = 0;
    }
    //if(load.secondOnOff == 1 && (bit_count(in) < (bscData.startKinji-bit_count(stop_cells)))) {
    //    no_combi_v_flg = false;
    //}
    
    //if(res == KINJITI_MAX_WHIGHT){
    //    se.chime_01();
    //}
    
    combi_OK = 0;
    //計量物がある分だけ組み合わせ確認を行う
    for(int i=in; i; i--) {

        //組み合わせ候補に重量ゼロがあれば次の組み合わせ候補へ
        if( (in & i) != i) continue;
        
        //printf("%5.1f\r\n",cel.anteiWeight(product.head - 1));
        if((i & combit) > 0){
            continue;
        }

        //比較用変数を初期化
        maxWeight = 0;
        minWeight = 6000;

        //ビット1になっている箇所の重量を取得していく
        //bkはbitが立っている場所の重量値を取得するために使用
        //sumは取得する重量値の合計重量を保持
        sum = 0;
        bk = i;
        
        
        
        

        //総重量取得処理
        for(bit = 0; bk; bk >>= 1) {
            if (bk & 0x01) {
                sum += weight[bit] - MINUS;
                if((load.kumi_flag == KUMI_OYAKO || load.kumi_flag == KUMI_DENSO)
                    && (oneBit[bit] & gOyaHead) > 0){
                }else{
                    if( maxWeight < weight[bit]) maxWeight = weight[bit];   //最重量値の場合は値取得
                    if( minWeight > weight[bit]) minWeight = weight[bit];   //最軽量値の場合は値取得
                }
            }
            bit++;
        }
                //printf("combi00: %5d |%6.1f\r\n",i,sum );

        //不揃い防止値(設定重量)より最大値、最小値の差があれば処理終了
        if( (maxWeight - minWeight) > load.fuzoroiBoushi && load.fuzoroiBoushi > 0) {
            continue;
        }
        //printf("%6.1f\r\n",sum);
        //printf("%6.1f",sum);
        //if(sum < min || (int(sum) > int(res))){
        //    printf("NG\r\n");
        //}
        //一番最適な重量を取得する
        //if((sum >= min) && (int(sum) <= int(res))) {
        //printf("combi  %3d: %3d |%6.1f\r\n",bit_count(i),i,sum );

        
        //指定された組み合わせ条件を満たしているか確認
        if(check_combi_condition(i,&load) == false) {
            //printf("combiNG: %5d |%6.1f\r\n",i,sum );
            continue;
        }

        if((load.kumi_flag!=KUMI_LIMIT && (( sum >= min                                      ) && (int(sum) <= res))) ||
          ((float(bit_count(i))*param.buffer2<float(res))&&
          (load.kumi_flag==KUMI_LIMIT && (((sum >= (float(min) + (float(bit_count(i))*param.buffer2))) && (int(sum) <= int(res)))))) ||
          (load.kumi_flag==KUMI_LIMIT && (int(sum) >= int(res)) && (int(sum) <= int(res))))
        {
                //printf("combi02: %5d |%6.1f\r\n",i,sum );

            //printf("\r\n");

            //組み合わせ成立があるならば
            //準最適値処理は行わない
                //printf("OK?\r\n");
                
                //printf("%6.1f:%6d\r\n",sum,int(resDefault));
            if(int(sum) <= resDefault/* || combi_OK != 0*/) {
                *secondCnt = 0;
                combi_OK_flug = true;
                //printf("OK\r\n");
            }
                //printf("OK?\r\n");

            //準最適値「組合せなし」通知は不要
            checkBtnJunsaiteki = false;

            // ******************
            //値を取得
            // ******************
            com[BK_CNT -1].kumi = i;
            com[BK_CNT -1].total = sum;
            bit_num = bit_count(i);
            //pc.printf("ku:%3d  to:%6.1f  num:%2d  \r\n",i , sum ,bit_num);
            //組合せ優先(99の場合は使用しない)
            if(load.kumi_priolity_1!=99 && bit_num == load.kumi_priolity_1 + 1) {
                com[BK_CNT -1].priolity =0;
            } else if(load.kumi_priolity_2!=99 && bit_num == load.kumi_priolity_2 + 1) {
                com[BK_CNT -1].priolity =1;
            } else {
                com[BK_CNT -1].priolity =2;
            }
            qsort(com, BK_CNT, sizeof(combiData_t), comp );
              //for (int j=0;j<BK_CNT -1;j++){
              //     pc.printf("%3d  %4f  %2d  \r\n",com[j].kumi, com[j].total,com[j].priolity);
              //}
            //組合せ優先(99の場合は使用しない)
            if(load.kumi_priolity_1 != 99 || load.kumi_priolity_2 != 99) {
                qsort(com, BK_CNT, sizeof(combiData_t), comp2 );
                // for (int j=0;j<BK_CNT -1;j++){
                //      pc.printf("%d  %4f  %2d  \r\n",com[j].kumi, com[j].total,com[j].priolity);
                //   }
            }

            //res(組合せ重量)を更新
            if(com[BK_CNT-1].total != DAMMY_MAX) {
                //res = com[BK_CNT-1].total;
            }
        //}else if(param.CUT < int(sum) && (int(sum) < float(min))){
            
        }else if(param.CUT < int(sum)){

            //準最適値「組合せなし」通知は不要
            checkBtnJunsaiteki = false;

            // ******************
            //値を取得
            // ******************
            for(int a = 0; a < BK_CNT; a++){
                if(littleCombi[a].kumi==0){
                    littleCombi[a].kumi = i;
                    littleCombi[a].total = sum;
                    a = BK_CNT;
                }else if(littleCombi[a].total < sum){
                    for(int b = BK_CNT-1; b >= a; b--){
                        littleCombi[b+1].kumi  = littleCombi[b].kumi;
                        littleCombi[b+1].total = littleCombi[b].total;
                    }
                    littleCombi[a].kumi = i;
                    littleCombi[a].total = sum;
                    a = BK_CNT;

                }
                /*
                printf("test\r\n");
                for(int m = 0; m < BK_CNT; m++){
                    printf("kumi:%4d|total:%6.1f\r\n",littleCombi[m].kumi,littleCombi[m].total);
                }
                printf("test\r\n");
                wait(1);
                */
            }           


        }

    }//loop終了
    //for (int j=0;j<BK_CNT -1;j++){
    //   pc.printf("%d  %4f  %2d  \r\n",com[j].kumi, com[j].total,com[j].priolity);
    //}
    
    //for(int i = 0; i < BK_CNT; i++) {
    //    printf("%2d :%4d :%6.1f:%4d\r\n",i,com[i].kumi,com[i].total,com[i].priolity);
    //}
    //組み合わせ成立があるならば
    //準最適値のデータを消す
    if(combi_OK_flug == true){//
        //printf("DELETE\r\n");
        for(int i = 0; i < BK_CNT; i++) {
            //printf("%2d :%4d :%6.1f:%4d\r\n",i,com[BK_CNT].kumi,com[BK_CNT].total,com[BK_CNT].priolity);
            if(int(com[i].total) > resDefault){
                com[i].kumi = 0;
                com[i].total = 1;
                com[i].priolity = 999;
                //printf("reset\r\n");
            }
        }
    }

    //ボタン操作で準最適値モードON後、
    //一つも組合せなければ「組合せがない」ことを通知する
    
    //printf("%5d ,%5d,%5d,%8.1f,%8.1f,%5d%5d ,\r\n",bit_count(in),bscData.startKinji-bit_count(stop_cells),bit_count(stop_cells),res,com[0].total,KINJITI_MAX_WHIGHT,int(sum));
    /*
    printf("miman\r\n");
    for(int m = 0; m < BK_CNT; m++){
        printf("kumi:%4d|total:%6.1f\r\n",littleCombi[m].kumi,littleCombi[m].total);
    }
    printf("miman\r\n");
    **/
    if(combi_OK == 0){
        /*
        if( ((checkBtnJunsaiteki == true) && (res == KINJITI_MAX_WHIGHT))) {
            //v.output_audio(MSG_NO_JUNSAITEKI);
            //v.wait_echo();
        }else if((int(com[0].total) == DAMMY_MAX) && (bit_count(in) >= (bscData.startKinji-bit_count(stop_cells)))){
            if((bit_count(in) >= (bscData.startKinji-bit_count(stop_cells))) ){
                //v.output_audio(MSG_NO_JUNSAITEKI);
                //v.wait_echo();

                com[0].total = 0;
                for(int i = 0; i < product.head; i++){
                    com[0].total += weight[i];
                }
                if(load.secondOnOff == 2){
                    if(littleCombi[0].kumi > 0  && bit_count(littleCombi[0].kumi) < product.head){

                        se.Touch_02();
                        se.Touch_02();
                        lcd.blank(1);
                        lcd.locate(0,1);
                        lcd.printf(under_over[0]);
                        lcd.printf("%5dg",int(littleCombi[0].total));

                        lcd.locate(10,1);
                        lcd.printf("%5dg",int(int(littleCombi[0].total) - min));

                        //lcd.bigNumber(int(min - littleCombi[0].total));
                        led.REGIS(littleCombi[0].kumi);
                        for(int i = 0; i <= load.lightTime; i += 2) {
                            if(btn.down_press() == true) {
                                se.Touch_02();
                                i = load.lightTime + 1;
                            }
                            led.REGIS(littleCombi[0].kumi);
                            wait(0.12);
                            led.REGIS(0);
                            wait(0.07);
                        }


                    }else if(min > com[0].total && com[0].total > param.CUT){
                        //printf("%d\r\n",weight_siri);
                        se.Touch_02();
                        se.Touch_02();
                        //se.Touch_02();
                        //if(int(min - com[0].total) < 10000){
                        //    lcd.bigNumber(int(min - com[0].total));
                        //}else{
                            //lcd.cls();
                            lcd.blank(1);
                            lcd.locate(0,1);
                            lcd.printf(under_over[0]);
                            lcd.printf("%5dg",int(littleCombi[0].total));
                            lcd.locate(10,1);
                            lcd.printf("%5dg",int(int(littleCombi[0].total) - min));
                        //}
                        for(int i = 0; i <= load.lightTime; i += 1) {
                            if(btn.down_press() == true) {
                                se.Touch_02();
                                i = load.lightTime + 1;
                            }
                            wait(0.1);
                        }
                    }
                }
                
                if(no_combi_v_flg == false){
                    if(option.no_combi_v==1){
                        //v.output_audio(MSG_NO_JUNSAITEKI);
                        //v.wait_echo();
                    }else if(option.no_combi_v==2){
                        se.Touch_02();
                        se.Touch_02();
                        se.Touch_02();
                        se.Touch_02();
                    }
                    no_combi_v_flg = true;
                }


            }
        }*/
    }
    //printf("miman3\r\n");

    //delete[] weight;
    return;
}



/////////////////////////////////////////////////////////////////////////////////
//重量組合せ関数(複数組合せ)
//すべての組合せ結果を計算する！
/////////////////////////////////////////////////////////////////////////////////
void get_combi_location_multi(combiData_t com[], int *secondCnt)
{
    int bit,bk,in = 0; //in:物が載っている場所を取得
    int bit_num=0;    //成立した個数の保存
    double sum=0;   //組み合わせ候補箇所の重量を合算していく
    float min = float(load.target) + param.buffer;  //最小値(バッファー有り)
    float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    float resDefault = res;                   //上限値のデフォルト値をここでバックアップ
    bool checkBtnJunsaiteki = false;            //ボタン押下で準最適値モードになったか
    //bool force_kinji = false;                   //準最適値のボタンが押されたことを示すフラグ(multi_weightだといろいろ変化するから)
    int rankset =0;                             //判別するための値
    ArrayIni(com, BK_CNT);   //初期化(組み合わせ保持している分を初期化)
    in = cel.getAnteiIn();   //計量物の数を取得
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    //計量物ないなら即終了
    if(in == 0) {
        return;
    }

    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)
    float *weight = new float[product.head];
    float maxWeight = 0, minWeight = 0;

    for(int i = 0; i < product.head; i++) {
        weight[i] = cel.anteiWeight(i);
    }

    //上ボタン(赤)が押されたら強制的に準最適値機能を1回だけON
    if( btn.down_press() == true && btn.get_lock_flg() == false) {
        btn.operation_lock();   //上ボタンを押している間
        res = KINJITI_MAX_WHIGHT;
        checkBtnJunsaiteki = true;  //準最適値機能を強制ON状態
        //準最適値開始する条件が満たされているか確認
        //計量物が必要なだけ皿数、載っているか
    } else if(load.secondOnOff == 1 && (bit_count(in) >= (bscData.startKinji-bit_count(stop_cells)))) {
        *secondCnt += 1;
        if(*secondCnt >= gStartKinjiti) {
            res = KINJITI_MAX_WHIGHT;
        }
        //桁あふれしないように念のため0で初期化
        if(*secondCnt > 1000) {
            *secondCnt = 0;
        }
    } else {
        *secondCnt = 0;
    }
    //if(load.secondOnOff == 1 && (bit_count(in) < (bscData.startKinji-bit_count(stop_cells)))) {
    //    no_combi_v_flg = false;
    //}
    //printf("CC\r\n");

    //計量物がある分だけ組み合わせ確認を行う
    for(int i=in; i; i--) {
        //組み合わせ候補に重量ゼロがあれば次の組み合わせ候補へ
        if( (in & i) != i) continue;

        //比較用変数を初期化
        maxWeight = 0;
        minWeight = 6000;

        //ビット1になっている箇所の重量を取得していく
        //bkはbitが立っている場所の重量値を取得するために使用
        //sumは取得する重量値の合計重量を保持
        sum = 0;
        bk = i;

        int count_bk=0;//一回目取得用
        //総重量取得処理
        for(bit = 0; bk; bk >>= 1) {
            if (bk & 0x01) {
                sum += weight[bit] - MINUS;
                if( maxWeight < weight[bit]) maxWeight = weight[bit];   //最重量値の場合は値取得
                if( minWeight > weight[bit]) minWeight = weight[bit];   //最軽量値の場合は値取得
                if(count_bk==0) {
                    //***************************1個目の重さによって、ﾓｸﾋｮｳﾁを変える************************************************
                    //printf("weight[%d]:%6.2f  \r\n",bit,weight[bit]);
                    load.target = 999999;
                    load.jougen = 999999;
                    load.kumi_min = product.head+5;
                    load.kumi_max = product.head+6;
                    rankset = 0;
                    if(weight[bit] > param.CUT && weight[bit] < mweight.u01.rank_min - 1) {
                        //printf("A\r\n");
                        //iro[count_bk] = C_SS;
                        rankset = 0;
                        if(mweight.u01.rank_en == 1) {
                            load.target = 999999;
                            load.jougen = 999999;
                            load.kumi_min = product.head+5;
                            load.kumi_max = product.head+6;
                        }
                    } else if(weight[bit] > mweight.u01.rank_min && weight[bit] < mweight.u01.rank_max) {
                        rankset = 1;
                        if(mweight.u01.rank_en == 1) {
                            load.target = mweight.u01.target;
                            load.jougen = mweight.u01.jougen;
                            load.kumi_min = mweight.u01.kumi_min;
                            load.kumi_max = mweight.u01.kumi_max;
                        }
                        
                    } else if(weight[bit] > mweight.u02.rank_min && weight[bit] < mweight.u02.rank_max) {
                        //printf("B\r\n");
                        //iro[count_bk] = C_S;
                        rankset = 2;
                        if(mweight.u02.rank_en == 1) {
                            load.target = mweight.u02.target;
                            load.jougen = mweight.u02.jougen;
                            load.kumi_min = mweight.u02.kumi_min;
                            load.kumi_max = mweight.u02.kumi_max;
                        }
                    } else if(weight[bit] > mweight.u03.rank_min && weight[bit] < mweight.u03.rank_max) {
                        //printf("B\r\n");
                        //iro[count_bk] = C_M;
                        rankset = 3;
                        if(mweight.u03.rank_en == 1) {
                            load.target = mweight.u03.target;
                            load.jougen = mweight.u03.jougen;
                            load.kumi_min = mweight.u03.kumi_min;
                            load.kumi_max = mweight.u03.kumi_max;
                        }
                        //printf("r:%d  bit:%d target:%4d jougen:%4d %6.3f\r\n",mweight.u03.rank_en,bit+1,load.target,load.jougen, weight[bit]);
                    } else if(weight[bit] > mweight.u04.rank_min && weight[bit] < mweight.u04.rank_max) {
                        //printf("B\r\n");
                        //iro[count_bk] = C_L;
                        rankset = 4;
                        if(mweight.u04.rank_en == 1) {
                            load.target = mweight.u04.target;
                            load.jougen = mweight.u04.jougen;
                            load.kumi_min = mweight.u04.kumi_min;
                            load.kumi_max = mweight.u04.kumi_max;
                        }
                    } else if(weight[bit] > mweight.u04.rank_max+1) {
                        //printf("B\r\n");
                        //iro[count_bk] = C_LL;
                        rankset = 5;
                        if(mweight.u05.rank_en == 1) {
                            load.target = mweight.u05.target;
                            load.jougen = mweight.u05.jougen;
                            load.kumi_min = mweight.u05.kumi_min;
                            load.kumi_max = mweight.u05.kumi_max;
                        }
                    } else if(weight[bit] > mweight.u05.rank_max+1) {
                        //printf("B\r\n");
                        //iro[count_bk] = C_LL;
                        rankset = 6;
                        if(mweight.u05.rank_en == 1) {
                            load.target = mweight.u06.target;
                            load.jougen = mweight.u06.jougen;
                            load.kumi_min = mweight.u06.kumi_min;
                            load.kumi_max = mweight.u06.kumi_max;
                        }
                    } else if(weight[bit] > mweight.u06.rank_max+1) {
                        //printf("B\r\n");
                        //iro[count_bk] = C_LL;
                        rankset = 7;
                        if(mweight.u05.rank_en == 1) {
                            load.target = mweight.u07.target;
                            load.jougen = mweight.u07.jougen;
                            load.kumi_min = mweight.u07.kumi_min;
                            load.kumi_max = mweight.u07.kumi_max;
                        }
                    } else if(weight[bit] > mweight.u07.rank_max+1) {
                        //printf("B\r\n");
                        //iro[count_bk] = C_LL;
                        rankset = 8;
                        if(mweight.u05.rank_en == 1) {
                            load.target = mweight.u08.target;
                            load.jougen = mweight.u08.jougen;
                            load.kumi_min = mweight.u08.kumi_min;
                            load.kumi_max = mweight.u08.kumi_max;
                        }
                    } else if(weight[bit] > mweight.u08.rank_max+1) {
                        //printf("B\r\n");
                        //iro[count_bk] = C_LL;
                        rankset = 9;
                        if(mweight.u05.rank_en == 1) {
                            load.target = mweight.u09.target;
                            load.jougen = mweight.u09.jougen;
                            load.kumi_min = mweight.u09.kumi_min;
                            load.kumi_max = mweight.u09.kumi_max;
                        }
                    } else if(weight[bit] > mweight.u09.rank_max+1) {
                        //printf("B\r\n");
                        //iro[count_bk] = C_LL;
                        rankset = 10;
                        if(mweight.u05.rank_en == 1) {
                            load.target = mweight.u10.target;
                            load.jougen = mweight.u10.jougen;
                            load.kumi_min = mweight.u10.kumi_min;
                            load.kumi_max = mweight.u10.kumi_max;
                        }
                    } else {
                        //printf("C\r\n");
                        load.target = 999999;
                        load.jougen = 999999;
                        load.kumi_min = 999999;
                        load.kumi_max = 999999;
                        rankset = 0;
                        continue;
                    }
                    min = float(load.target) + param.buffer;  //最小値(バッファー有り)
                    res = load.jougen;    //組み合わせ重量の上限値を取得
                    load.jougen -= load.target;
                    resDefault = res;                   //上限値のデフォルト値をここでバックアップ
                    //printf("load.target:%d  \r\n",load.target);
                    //printf("load.jougen:%d  \r\n",load.jougen);
                    //,mweight.u01.kumi_min ,mweight.u01.kumi_max
                    //***************************1個目の重さによって、ﾓｸﾋｮｳﾁを変えるend************************************************
                }
                count_bk += 1;
            }
            bit++;
        }

        //不揃い防止値(設定重量)より最大値、最小値の差があれば処理終了
        if( (maxWeight - minWeight) > load.fuzoroiBoushi && load.fuzoroiBoushi > 0) {
            continue;
        }

        if(rankset == 0) {
            //printf("ng1\r\n");
            //printf("maxWeight:%6.1f  \r\n",maxWeight);
            //printf("minWeight:%6.1f  \r\n",minWeight);
            continue;

        } else if(rankset == 1 && (maxWeight > mweight.u01.rank_max || minWeight <= mweight.u01.rank_min)) {
            //printf("ng1\r\n");
            //printf("maxWeight:%6.1f  \r\n",maxWeight);
            //printf("minWeight:%6.1f  \r\n",minWeight);
            continue;
        } else if(rankset == 2 && (maxWeight > mweight.u02.rank_max || minWeight <= mweight.u02.rank_min)) {
            //printf("ng2\r\n");
            //printf("maxWeight:%6.1f  \r\n",maxWeight);
            //printf("minWeight:%6.1f  \r\n",minWeight);
            continue;
        } else if(rankset == 3 && (maxWeight > mweight.u03.rank_max || minWeight <= mweight.u03.rank_min)) {
            //printf("ng2\r\n");
            //printf("maxWeight:%6.1f  \r\n",maxWeight);
            //printf("minWeight:%6.1f  \r\n",minWeight);
            continue;
        } else if(rankset == 4 && (maxWeight > mweight.u04.rank_max || minWeight <= mweight.u04.rank_min)) {
            //printf("ng2\r\n");
            //printf("maxWeight:%6.1f  \r\n",maxWeight);
            //printf("minWeight:%6.1f  \r\n",minWeight);
            continue;
        } else if(rankset == 5) {
            //printf("ng2\r\n");
            //printf("maxWeight:%6.1f  \r\n",maxWeight);
            //printf("minWeight:%6.1f  \r\n",minWeight);
            continue;
        }
        //一番最適な重量を取得する
        if((sum >= min) && (int(sum) <= int(res))) {

            //指定された組み合わせ条件を満たしているか確認
            if(check_combi_condition(i,&load) == false) {
                continue;
            }

            //組み合わせ成立があるならば
            //準最適値処理は行わない
            if(int(sum) <= int(resDefault)) {
                *secondCnt = 0;
            }

            //準最適値「組合せなし」通知は不要
            checkBtnJunsaiteki = false;

            // ******************
            //値を取得
            //*******************
            //printf("kumi:%2d  ",i );

            //printf("tota:%6.2f  \r\n",sum );



            com[BK_CNT -1].kumi = i;
            com[BK_CNT -1].total = sum;
            bit_num = bit_count(i);
            //組合せ優先(99の場合は使用しない)
            if(load.kumi_priolity_1!=99 && bit_num == load.kumi_priolity_1 + 1) {
                com[BK_CNT -1].priolity =0;
            } else if(load.kumi_priolity_2!=99 && bit_num == load.kumi_priolity_2 + 1) {
                com[BK_CNT -1].priolity =1;
            } else {
                com[BK_CNT -1].priolity =2;
            }
            qsort(com, BK_CNT, sizeof(combiData_t), comp );
            //  for (int j=0;j<BK_CNT -1;j++){
            //       pc.printf("%d  %4f  %2d  \r\n",com[j].kumi, com[j].total,com[j].priolity);
            //  }
            //組合せ優先(99の場合は使用しない)
            if(load.kumi_priolity_1 != 99 || load.kumi_priolity_2 != 99) {
                qsort(com, BK_CNT, sizeof(combiData_t), comp2 );
                // for (int j=0;j<BK_CNT -1;j++){
                //      pc.printf("%d  %4f  %2d  \r\n",com[j].kumi, com[j].total,com[j].priolity);
                //   }
            }

            //res(組合せ重量)を更新
            if(com[BK_CNT-1].total != DAMMY_MAX) {
                //res = com[BK_CNT-1].total;
            }
        }
    }//loop終了

    //ボタン操作で準最適値モードON後、
    //一つも組合せなければ「組合せがない」ことを通知する
    if( (checkBtnJunsaiteki == true) && (res == KINJITI_MAX_WHIGHT) ) {
        v.output_audio(MSG_NO_JUNSAITEKI);
        v.wait_echo();
    }

    delete[] weight;
    return;
}
//指定された組み合わせ条件が成立しているか確認
//条件を満たしていなければfalseを返す
bool check_combi_condition(int bits, const loadcell_t *load)
{
    //サーボ動作中のものが組合せにあれば即終了
    //printf("moving1  %d\r\n",moving1);
    if(((bits & moving1)>0) && servo_connection==true && load->spMode == SPEED_DAMP){ return false;  }
    //本数指定時の処理
    if(load->kumi_flag == KUMI_NONE) {
        //制限無しの場合、無条件で組み合わせ条件成立
        if(load->combiflug==0){
            return true;
        }else if(load->combiflug==1  &&(((bits|  UP_HEAD)^  UP_HEAD)==0||
                                 ((bits| LOW_HEAD)^  LOW_HEAD)==0||
                                 ((bits|TOP1_HEAD)^ TOP1_HEAD)==0||
                                 ((bits|TOP2_HEAD)^ TOP2_HEAD)==0||
                                 ((bits|BOT2_HEAD)^ BOT2_HEAD)==0||
                                 ((bits|BOT1_HEAD)^ BOT1_HEAD)==0  ) ){
            return true;
        /*}else if(load->combiflug==2  && (((((bits|  UP_HEAD)^  UP_HEAD )==0) && combi_chk==0)  ||
                                         ((((bits| LOW_HEAD)^  LOW_HEAD)==0) && combi_chk==1))){
        }else {
            return false;
        }*/
        }else if(load->combiflug==2){
            if(((bits|  UP_HEAD)^  UP_HEAD )==0){
                if(combi_chk==0){
                    return true;
                }else {
                    combi_OK = 1;
                    return false;
                }
            }else if(((bits| LOW_HEAD)^  LOW_HEAD)==0){
                if(combi_chk==1){
                    return true;
                }else {
                    combi_OK = 1;
                    return false;
                }
            }
            return false;
        }else {
            return false;
        }
        //制限有り 本数指定内かを確認
    /*} else if(load->kumi_flag == KUMI_COMBI) {
        if(load->combiflug==0){
            return true;
        }else if(load->combiflug==1  &&(((bits|  UP_HEAD)^  UP_HEAD)==0||
                                 ((bits| LOW_HEAD)^  LOW_HEAD)==0||
                                 ((bits|TOP1_HEAD)^ TOP1_HEAD)==0||
                                 ((bits|TOP2_HEAD)^ TOP2_HEAD)==0||
                                 ((bits|BOT2_HEAD)^ BOT2_HEAD)==0||
                                 ((bits|BOT1_HEAD)^ BOT1_HEAD)==0  ) ){
            return true;
        }else if(load->combiflug==2  &&(((bits|  UP_HEAD)^  UP_HEAD)==0||
                                       ((bits| LOW_HEAD)^  LOW_HEAD)==0) ){
            return true;*/
        
    } else if(load->kumi_flag == KUMI_LIMIT || load->kumi_flag == KUMI_KAIKYU || (load->kumi_flag == KUMI_KAISU1 || load->kumi_flag == KUMI_KAISU2)) {
        //指定本数内じゃなかったら総重量ゼロ
        //pc.printf("kumi_min%3d  kumi_max%3d\r\n",load->kumi_min+1 ,load->kumi_max+1);
        if((load->kumi_flag == KUMI_KAISU1 || load->kumi_flag == KUMI_KAISU2 )) {//親子計量と違い、親の所と組合せが有ったら間違いになる
            if((bits & gOyaHead) > 0){
                return false;
            }
        }
        if(bit_count(bits) < load->kumi_min+1 || bit_count(bits) > load->kumi_max+1) {
            return false;
        }else if(load->combiflug==0){
            return true;
        }else if(load->combiflug==1  &&(((bits|  UP_HEAD)^  UP_HEAD)==0||
                                 ((bits| LOW_HEAD)^  LOW_HEAD)==0||
                                 ((bits|TOP1_HEAD)^ TOP1_HEAD)==0||
                                 ((bits|TOP2_HEAD)^ TOP2_HEAD)==0||
                                 ((bits|BOT2_HEAD)^ BOT2_HEAD)==0||
                                 ((bits|BOT1_HEAD)^ BOT1_HEAD)==0  ) ){
            return true;
        /*}else if(load->combiflug==2  &&((((((bits|  UP_HEAD)^  UP_HEAD)==0) && combi_chk==0)||
                                         (((bits| LOW_HEAD)^  LOW_HEAD)==0) && combi_chk==1))){
            return true;*/
        }else if(load->combiflug==2){
            if(((bits|  UP_HEAD)^  UP_HEAD )==0){
                if(combi_chk==0){
                    return true;
                }else {
                    combi_OK = 1;
                    return false;
                }
            }else if(((bits| LOW_HEAD)^  LOW_HEAD)==0){
                if(combi_chk==1){
                    return true;
                }else {
                    combi_OK = 1;
                    return false;
                }
            }
            return false;
        }else {
            return false;
        }
    
        //親子計量(2:10)
    } else if(load->kumi_flag == KUMI_OYAKO || load->kumi_flag == KUMI_DENSO) {
        //親の位置に計量物無い、親同士が組み合わせ対象の場合処理抜け
        if(((bits & gOyaHead) == 0) || ( bit_count((bits & gOyaHead)) == 2) || (bit_count(bits & gKoHead) < 1) || (bit_count(bits & gKoHead) < load->kumi_min+1) || bit_count(bits & gKoHead) > load->kumi_max+1) {
            return false;
        }
        //親子計量(6:6)
    } else if(load->kumi_flag == KUMI_OYAKO_HALF) {
        //if( bit_count(bits & LOW_HEAD) < load->kumi_min+1 || bit_count(bits & LOW_HEAD) > load->kumi_max+1){  //17/10/18変更。これだけだと親だけで成立する
        //if( bit_count(bits & LOW_HEAD) < load->kumi_min+1 || bit_count(bits & LOW_HEAD) > load->kumi_max+1 || (bit_count(bits & UP_HEAD) < 1)) {
        if( bit_count(bits & LOW_HEAD) < load->kumi_min+1 || bit_count(bits & LOW_HEAD) > load->kumi_max+1 || 
            bit_count(bits & UP_HEAD ) < load->kumi_priolity_1+1 || bit_count(bits & UP_HEAD ) > load->kumi_priolity_2+1 || (bit_count(bits & UP_HEAD) < 1)) {
            return false;
        }
    }

    return true;
}
void drive_remeas() //ゆっくりモード中での「成立後での準最適値成立個数達成による再計量」時に使う処理
{
    int secondCnt = 0;                        //一定数に達したら準最適値処理開始
    int *stay_cnt = new int[product.head];      //滞留警告判断のためのカウント変数
    //float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得

    //cntData_t cnt={0,0,0};                      //カウント機能の値保持用
    combiData_t orderCombi[BK_CNT] = {0,0.0,0};   //組み合わせ候補のデータ(配列)

    //cnt.nowMode = load.spMode;       //処理モード取得
    //lcd.settings(&load);            //設定内容を表示
    for(int i = 0; i < product.head; i++) {
        setHeadVolt(i, param.KURI);
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            if(cel.getWeight(i) >= param.CUT && cel.getWeight(i) < mweight.u02.rank_min) {
                iro[i] = C_2S;
            } else if(cel.getWeight(i) >= mweight.u02.rank_min && cel.getWeight(i) < mweight.u02.rank_max) {
                iro[i] = C_S;
            } else if(cel.getWeight(i) >= mweight.u03.rank_min && cel.getWeight(i) < mweight.u03.rank_max) {
                iro[i] = C_M;
            } else if(cel.getWeight(i) >= mweight.u04.rank_min && cel.getWeight(i) < mweight.u04.rank_max+1) {
                iro[i] = C_L;
            } else if(cel.getWeight(i) >= mweight.u04.rank_max+1) {
                iro[i] = C_2L;
            } else {
                iro[i] = C_NULL;
            }
        }

        //デバッグ用ここから
        if( product.productType >= STANDARD_D || product.productType == JUNIOR_D || product.productType == BIG_D) {
            //temp = cel.temperature();   //温度を取得する
            //for(int i = 0; i < product.head; i++){
            cel.set_head(i);
            pc.printf("%d %6.3f %6.3f   %0.8f\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.getAin());
            //}
            if(i == product.head - 1) {
                //printf("%d %f C\r\n",14, temp);

                pc.printf("========================\r\n");
                pc.printf("========================\r\n");
                pc.printf("========================\r\n");
            }
        }
        //デバッグ用ここまで

    }//for

    //重量確認(適正でない重量があるため処理を飛ばす)
    if(check_max_weight(cel.over_weight()) == false) {
        //continue;
    }

    if(load.kumi_flag == KUMI_KAIKYU) {
        get_combi_location_multi(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
    } else {
        get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
    }
    //resultCombi2 = orderCombi[0];                    //最小組み合わせ結果を取得
    resultCombi2.kumi = orderCombi[0].kumi;
    resultCombi2.total = orderCombi[0].total;
    resultCombi2.priolity = orderCombi[0].priolity;

    //振動検知機能がONだったら
    if(param.vivration >= 1) {
        //繰り返し確認する
        for(int i = 0; i < BK_CNT; i++) {
            //組み合わせ候補が安定しているか
            if(checkWeightTaget(&orderCombi[i]) == true) {
                resultCombi2.kumi = orderCombi[i].kumi;
                resultCombi2.total = orderCombi[i].total;
                resultCombi2.priolity = orderCombi[i].priolity;
                break;
            }
        }
    }
    if(resultCombi2.kumi > 0) {
    }
    //printf("new:%6d\r",resultCombi2.kumi);

}
//ゆっくりモード時の処理を行う
void driveSlowly(combiData_t *com)
//void driveSlowly(combiData_t *com, cntData_t *cnt)
{
    char multiweight[SIZE] = {0xD7,0xDD,0xB8,0xCD,0xDE,0xC2,' ' ,0xB8,0xD0,0xB1,0xDC,0xBE,0x00};
    int missPoint = 0;  //ゆっくりモード時に取り間違いした位置
    int inNow=0, inOld = 0;
    int checkCom,ledOff;
    int disp_wait=0;    //ディスプレイ表示更新頻度調整用//川
    int loopcnt=0;      //ループ回数を確認
    int inChk=0;        //重量増加チェックビット
    int first_chk_flg=0;
    int match_kumi=0;
    float kumi_MINI_target=0;   //最小評価物の、さらに組数分割ったもの
    double recheck_total=0;
    int recheck_kumi =0;
    int atonose_limit =0;          //後乗せ防止が反応してから再計量に移るまでのリミット
    //int recheck_flg =0;         //値を更新したのかを判別するためのフラグ
    int reset_flg=0;
    int btnset = 0;
    
    no_combi_v_flg = false;
    //第一上限値までの最大重量を取得
    //この値を超えていたら第二上限値を含めた重量となるため
    //目視でわかるようにその場合にはLED点滅を行う。
    //float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    match_kumi = com->kumi; //成立した組合せをループ中保存
    kumi_MINI_target = param.MINI_target / bit_count(match_kumi);                     //成立した組合せ個数を用意
    //初期化(ヘッド数分)
    cel.setAinSpeedDummy(gNowHeadBit);
    //printf("AAB\r");
    //se.ck01();   //3.17で機能予定
    inNow |= com->kumi;
    cel.reset_cel_status(product.head);
    for(int i=0; i < product.head; i++) {
        //計量物が載っているか取得
        if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
            cel.set_head(i);
            cel.setAin_zero(i);
        } else if(cel.checkIn2(i,inNow)) {
            inNow = inNow | oneBit[i];
            //printf("%2d  OK\r",i);
        }
    }//for
    if(bit_count(inNow)>= (bscData.startKinji-bit_count(stop_cells))) { //170816 成立した時点で、すでに準最適値の個数以上だったら
        re_meas2nd=1;
    }
    //成立した時点の重量を取得する(組合せ分のみ)
    inChk=1;
    for(int k=0; k < product.head; k++) {
        if((inChk & com->kumi) > 0) {
            oldweightVale[k] = cel.getWeight(k);
            if(load.kumi_flag == KUMI_KAIKYU) {
                if(oldweightVale[k] > param.CUT && oldweightVale[k] < mweight.u01.rank_min - 1) {
                    load.target = 9999;
                    load.jougen = mweight.u01.jougen;
                    load.kumi_min = mweight.u01.kumi_min;
                    load.kumi_max = mweight.u01.kumi_max;
                } else if(oldweightVale[k] > mweight.u01.rank_min && oldweightVale[k] < mweight.u01.rank_max) {
                    load.target = mweight.u01.target;
                    load.jougen = mweight.u01.jougen;
                    load.kumi_min = mweight.u01.kumi_min;
                    load.kumi_max = mweight.u01.kumi_max;
                } else if(oldweightVale[k] > mweight.u02.rank_min && oldweightVale[k] < mweight.u02.rank_max) {
                    load.target = mweight.u02.target;
                    load.jougen = mweight.u02.jougen;
                    load.kumi_min = mweight.u02.kumi_min;
                    load.kumi_max = mweight.u02.kumi_max;
                } else if(oldweightVale[k] > mweight.u03.rank_min && oldweightVale[k] < mweight.u03.rank_max) {
                    load.target = mweight.u03.target;
                    load.jougen = mweight.u03.jougen;
                    load.kumi_min = mweight.u03.kumi_min;
                    load.kumi_max = mweight.u03.kumi_max;
                } else if(oldweightVale[k] > mweight.u04.rank_min && oldweightVale[k] < mweight.u04.rank_max) {
                    load.target = mweight.u04.target;
                    load.jougen = mweight.u04.jougen;
                    load.kumi_min = mweight.u04.kumi_min;
                    load.kumi_max = mweight.u04.kumi_max;
                } else if(oldweightVale[k] > mweight.u04.rank_max+1) {
                    load.target = mweight.u05.target;
                    load.jougen = mweight.u05.jougen;
                    load.kumi_min = mweight.u05.kumi_min;
                    load.kumi_max = mweight.u05.kumi_max;
                }
                res = load.jougen;    //組み合わせ重量の上限値を取得
                load.jougen -= load.target;   //制限ｱﾘ等と同じ計算式に合わせるための計算
            }
        }
        inChk = inChk <<1;
        nowweightVale4[k] = nowweightVale[k];
    }
    //組合せ成立後の無限ループ
    //load.secondOnOff=0;
    show_results(com);     //組み合わせに応じたLED点灯,総重量表示
    comp_kumi = com->kumi;
    while(com->kumi) {
        missPoint = 0;  //初期化
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            comp_kumi = com->kumi;
        } else {
            led.REGIS(com->kumi);
        }
        //******* 川4/17追加  ディスプレイ表示更新
        disp_wait = disp_wait+1;
        if(disp_wait==5) {
            if((int(com->total) <= res)) {
                lcd.settings(&load);            //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
            }
            //load.secondOnOff=0;
            show_results(com);     //組み合わせに応じたLED点灯,総重量表示
            disp_wait=0;
        }
        //*******ディスプレイ表示更新ここまで

        //上ボタンが離されたら、操作ロック解除
        if( btn.down_press() == false || btn.esc()) {
            btn.operation_unlock();
        }
        //下ボタンが押されたら、ｹｲﾘｮｳorｶｳﾝﾄ/LED点灯時間
        if( btn.up_press() == true){
        //if(btn.esc()) {
        //    btn.operation_unlock();
        //    btn.old_btn();       //スイッチ更新
        //}
            se.chime_01();
            while(btn.any_press()==true){
                if(demo_flg==true&&btn.down_press() == true&&file.get_preset()==product.head*2){
                    demo_continue=2;
                    btn.force_ESC();
                    se.Touch_01();
                    continue;
                }
                wait(0.1);
            }
            if(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2){
                    demo_continue=2;
                    btn.force_ESC();
                    continue;
            }
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            
            if(load.kumi_flag == KUMI_OYAKO || load.kumi_flag == KUMI_OYAKO_HALF) {
                btnset = Chenge_vaule_btnD(0);
                if(btnset==0) {
                    auto_zero_off = true;
                    start_keiryo(0);
                    auto_zero_off = false;
                } else if(btnset==1) {
                    set_preset2(file.get_preset(),TL_SPMODE);
                }else if(btnset==2){
                    set_option_setting(OP_BEGINNER);
                }else if(btnset==3){
                    set_option_setting(OP_KIKAKUGAI_V);
                }else if(btnset==4){
                    set_option_setting(OP_KOKUTI);
                }else if(btnset==5){
                    set_option_setting(OP_SEL_BEEP);
                }else if(btnset==6){
                    set_sercret_parameter_one(P_buffer,1); //保存指令付
                }else if(btnset==7){
                    set_option_setting(OP_NO_COMBI_V);
                } 
            }
            //ゆっくりモード時には、
            else {
                //ゆっくりモード時には、
                btnset = Chenge_vaule_btnD(98);//ｶｳﾝﾄ以外orｹｲﾘｮｳﾓｰﾄﾞ
                /*if(btnset==0) {
                    set_count(&cnt);
                } else*/ if(btnset==1) {
                    auto_zero_off = true;
                    start_keiryo(0);
                    auto_zero_off = false;
                }else if(btnset==2){
                    set_preset2(file.get_preset(),TL_SPMODE);
                }else if(btnset==3){
                    set_preset2(file.get_preset(),TL_KUMI_FLAG);
                }else if(btnset==4){
                    set_option_setting(OP_BEGINNER);
                }else if(btnset==5){
                    set_option_setting(OP_KIKAKUGAI_V);
                }else if(btnset==6){
                    set_option_setting(OP_KOKUTI);
                }else if(btnset==7){
                    set_option_setting(OP_SEL_BEEP);
                }else if(btnset==8){
                    set_sercret_parameter_one(P_buffer,1); //保存指令付
                }else if(btnset==9){
                    set_option_setting(OP_NO_COMBI_V);
                }
            }
            reset_flg = 1;
            //se.chime_01();
            //btn.old_btn();       //スイッチ更新
            //auto_zero_off = true;
            //start_keiryo(0);
            //auto_zero_off = false;
            lcd.cls();
            //1行目LCD表示
            if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                lcd.locate(0,0);
                lcd.printf(multiweight);
                lcd.blank(LOW);
                lcd.printf(":----.-g       ");
            } else {
                lcd.settings(&load);      //設定内容を表示
            }
            //show_results(com);     //組み合わせに応じたLED点灯,総重量表示
            btn.operation_unlock();
        }
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
        } else {
            //準最適値ONの場合、重量計る前はLED消灯
            if(load.secondOnOff==1 && (int(com->total) > res)) {
                wait(0.05);
                led.REGIS(0);
                wait(0.03);
            }
        }
        //printf("03A_%2d\r",bit_count(inNow));
//再計量部
        //***********************全載せ再計算***********************
        //成立皿がまだ1皿も取られていない&準最適値がON&規定数量以上の皿に物が乗った&再計量をしていない
        if(option.reck_zen==true) { //全載せ再計算のON/OFF
            //if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT || load.kumi_flag == KUMI_COMBI) && first_chk_flg == 0 && (bit_count(inNow)>= bscData.startKinji) && re_meas2nd==0) { //オプションでON/OFF出来る様になったため、準最適値でのON/OFFは関係なくなったので消去
            if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT) && first_chk_flg == 0 && (bit_count(inNow)>= (bscData.startKinji-bit_count(stop_cells))) && re_meas2nd==0) { //オプションでON/OFF出来る様になったため、準最適値でのON/OFFは関係なくなったので消去
                //if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT) && first_chk_flg == 0 && load.secondOnOff==1 && (bit_count(inNow)>= bscData.startKinji) && re_meas2nd==0){
                //if(load.secondOnOff==1 && (bit_count(inNow)>= bscData.startKinji) && (re_meas2nd != re_meas3rd || re_meas2nd < bit_count(inNow))){               //準最適値がON&規定数量以上の皿に物が乗った&再計量をしていない
                re_meas2nd=1;
                drive_remeas();
                if(com->kumi != resultCombi2.kumi && ( resultCombi2.total > load.target && com->total - 3 > resultCombi2.total )) { //成立したものが最適だった場合
                    se.Touch_01();
                    com->kumi = resultCombi2.kumi;
                    com->total = resultCombi2.total;
                    com->priolity = resultCombi2.priolity;
                    //return;
                    if((int(resultCombi2.total) < res)) {
                        lcd.settings(&load);            //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
                    }
                    show_results(com);     //組み合わせに応じたLED点灯,総重量表示
                    for(int m=0; m < product.head; m++) {
                        kumiawase_er[m]=0;
                    }
                    atonose_limit=0;
                    first_chk_flg=0;
                    resultCombi2.kumi = 0;
                    resultCombi2.total = 0;
                    resultCombi2.priolity = 0;
                    match_kumi = com->kumi; //成立した組合せをループ中保存
                    kumi_MINI_target = param.MINI_target / bit_count(match_kumi);                     //成立した組合せ個数を用意
                    //btn.operation_lock();
                    inNow=0;
                    inChk=1;
                    for(int k=0; k < product.head; k++) {
                        if((inChk & com->kumi) > 0) {
                            oldweightVale[k] = nowweightVale[k];
                        } else if(nowweightVale[k] > param.CUT) {
                            inNow += oneBit[k];
                        }
                        inChk = inChk <<1;
                    }
                } else {
                    se.Touch_01();
                    resultCombi2.kumi = 0;
                    resultCombi2.total = 0;
                    resultCombi2.priolity = 0;
                    //printf("5B_\r\n");
                }
            }
        }
        //***********************全載せ再計算end***********************

        recheck_total=0;
        inNow |= com->kumi;  //組み合わせ成立箇所は現在物が載っている(強制)
        inOld = inNow;  //皿の状態を保存

        //計量皿に物が載っているか確認

        inChk=1;            //重量チェックするためのビット1ヘッド目から開始
        for(int i=0; i < product.head; i++) {
            if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
                cel.set_head(i);
                cel.setAin_zero(i);
            } else {
                //if((inChk & com->kumi && (first_chk_flg==0 || atonose_limit > 0 )) > 0){
                //    inNow = infoNow2(i, inNow);
                //setHeadVolt(i, param.KURI);
                //}else{
                //printf("%02dh ||",i);
                //if(i==2){printf("07innow:%2d:inOld:%2d\r\n",inNow,inOld);}
                inNow = infoNow(i, inNow);
                
                //if(i==2){printf("08innow:%2d:inOld:%2d\r\n",inNow,inOld);}
                //}
                //printf("03E_%2d\r",bit_count(inNow));
            }
            //デバッグ用ここから
            if( product.productType >= STANDARD_D || product.productType == JUNIOR_D || product.productType == BIG_D) {
                //for(int i = 0; i < product.head; i++){
                cel.set_head(i);
                printf("%02dh %f %f   %f\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.getAin());
                //}
                if(i == product.head - 1) {
                    printf("\r\n");
                    printf("========================\r\n");
                    printf("========================\r\n");
                    printf("========================\r\n");
                }
            }
            //デバッグここまで

            //201704/26 //川 成立皿に追加の重量が載った場合(NGなので)ループを脱出する
            //201706/29 //川 ジュニア専用だったのを撤去
            //計量皿に物が載っているか確認
            //if( product.productType == JUNIOR || product.productType == JUNIOR_D){
            //inChk=1;            //重量チェックするためのビット1ヘッド目から開始
            //for(int k=0; k < product.head; k++) {
            //cel.get_All_weight();
            float sabun=0;
            if (loopcnt>0) {
                if((inChk & com->kumi && (first_chk_flg==0 || atonose_limit > 0 )) > 0) {
                    //3.17以降適用if((inChk & com->kumi) > 0){
                    //setHeadVolt(k, param.KURI);
                    //setHeadVolt(k, 1200);
                    //printf("===%d %f %f %d %d %f\r\n", k+1,oldweightVale[k], nowweightVale[k],inChk , inNow,param.MINI_target);
                    sabun = nowweightVale[i] - nowweightVale2[i];
                    nowweightVale2[i]=nowweightVale[i];

                    //printf("01  %d %6.2f %6.2f %6.2f %4.1f %d\r\n", i+1,oldweightVale[i], nowweightVale[i], nowweightVale2[i],param.BARATSUKI , inNow);
                    //測定重量が安定していなかったら、フラグを0
                    if(loopcnt>3 && sabun >= param.BARATSUKI || sabun <= -param.BARATSUKI) {
                        //nowweightVale[k]=0;
                    } else if (nowweightVale[i]!=0) {
                        recheck_total += nowweightVale[i];
                        recheck_kumi += 1;
                    }
                    if(option.reck_ato == true) { //後載せ防止機能ON/OFF
                        //Jr以外では[変数の値]以上だったら再計量、Jrでは[変数をビットで割った分の値]以上だったら再計量
                        //if(((product.productType != JUNIOR && product.productType != JUNIOR_D)&&(nowweightVale[i] >= param.MINI_target && ((oldweightVale[i]+0.2+param.MINI_target) < nowweightVale[i] || (oldweightVale[i]-0.2-param.MINI_target) > nowweightVale[i])))
                        if(((product.productType != JUNIOR && product.productType != JUNIOR_D)&&(nowweightVale[i] >= param.MINI_target && ((oldweightVale[i]+0.2+param.MINI_target) < nowweightVale[i])))
                                || ((product.productType == JUNIOR || product.productType == JUNIOR_D)&&(nowweightVale[i] >= param.MINI_target && ((oldweightVale[i]+0.2+kumi_MINI_target) < nowweightVale[i] || (oldweightVale[i]-0.2-kumi_MINI_target) > nowweightVale[i])))) {
                            setHeadVolt(i, param.KURI); //量りなおす
                            //それでもだめだったら
                            if(((product.productType != JUNIOR && product.productType != JUNIOR_D)&&(nowweightVale[i] >= param.MINI_target && ((oldweightVale[i]+0.2+param.MINI_target) < nowweightVale[i])))
                                    || ((product.productType == JUNIOR || product.productType == JUNIOR_D)&&(nowweightVale[i] >= param.MINI_target && ((oldweightVale[i]+0.2+kumi_MINI_target) < nowweightVale[i] || (oldweightVale[i]-0.2-kumi_MINI_target) > nowweightVale[i])))) {
                                //printf("========================\r\n");
                                //printf("========================\r\n");
                                //printf("========================\r\n");
                                //printf("===%d %f %f %d %d %f\r\n", i+1,oldweightVale[i], nowweightVale[i],inChk , inNow,param.MINI_target);
                                //ledOff = inNow ^ inChk;
                                //checkCom = (inOld ^ inNow) & inOld;
                                //ledOff = checkCom & com->kumi;       //組み合わせ成立していた場所の取った場所を取得
                                kumiawase_er[i] +=1;
                                atonose_limit+=1;
                                int er_limit = 6;
                                /*if(kumiawase_er[0]== 8 || kumiawase_er[1]== 8 ||
                                     kumiawase_er[2]== 8 || kumiawase_er[3]== 8 ||
                                     kumiawase_er[4]== 8 || kumiawase_er[5]== 8 ||
                                     kumiawase_er[6]== 8 || kumiawase_er[7]== 8 ||
                                     kumiawase_er[8]== 8 || kumiawase_er[9]== 8 ||
                                     kumiawase_er[10]== 8 || kumiawase_er[11]== 8 ){
                                    missPoint = inChk;  //位置を取得
                                    led.Flash(missPoint);
                                    se.recheck();    //警告音
                                //}else if(atonose_limit>4){
                                }else*/
                                if(kumiawase_er[0]>= er_limit || kumiawase_er[1]>= er_limit ||
                                        kumiawase_er[2]>= er_limit || kumiawase_er[3]>= er_limit ||
                                        kumiawase_er[4]>= er_limit || kumiawase_er[5]>= er_limit ||
                                        kumiawase_er[6]>= er_limit || kumiawase_er[7]>= er_limit ||
                                        kumiawase_er[8]>= er_limit || kumiawase_er[9]>= er_limit ||
                                        kumiawase_er[10]>= er_limit || kumiawase_er[11]>= er_limit ) {
                                    for(int m=0; m < product.head; m++) {
                                        kumiawase_er[m]=0;
                                    }
                                    re_meas2nd=0;
                                    atonose_limit = 0;
                                    missPoint = inChk;  //位置を取得
                                    btn.operation_unlock();
                                    //led.Flash(missPoint);
                                    led.Flash(match_kumi);
                                    se.recheck();    //警告音
                                    //led.Flash(match_kumi);
                                    first_chk_flg=0;
                                    btn.operation_lock();
                                    comp_kumi=0;
                                    return;
                                }
                            } else {
                                if(kumiawase_er[i] >0) {
                                    kumiawase_er[i] -=1;
                                }
                            }
                        }
                    }
                }
            }
            if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                //if(sabun >= (0.5)){//差が0.5以上
                if((comp_kumi & oneBit[i]) > 0) {               //成立しているものは(else以降に入れてるので「していないものは」)
                    //無視
                } else {
                    if((nowweightVale[i]!=0 && ((nowweightVale[i] -nowweightVale3[i]) <= -0.3)) || ((nowweightVale[i] -nowweightVale3[i]) >= 5)) {
                        nowweightVale3[i] = nowweightVale[i];
                    }
                }
                //if(nowweightVale3[i] >= param.CUT && nowweightVale3[i] < mweight.u02.rank_min){
                if(nowweightVale3[i] >= param.CUT && nowweightVale3[i] < mweight.u01.rank_min &&
                   nowweightVale4[i] >= param.CUT && nowweightVale4[i] < mweight.u01.rank_min   ) {
                    iro[i] = C_BATU;
                    //}else if(nowweightVale3[i] >= mweight.u02.rank_min && nowweightVale3[i] < mweight.u02.rank_max){
                } else if(nowweightVale3[i] >= mweight.u01.rank_min && nowweightVale3[i] < mweight.u01.rank_max &&
                          nowweightVale4[i] >= mweight.u01.rank_min && nowweightVale4[i] < mweight.u01.rank_max) {
                    iro[i] = C_BATU;
                } else if(nowweightVale3[i] >= mweight.u02.rank_min && nowweightVale3[i] < mweight.u02.rank_max &&
                          nowweightVale4[i] >= mweight.u02.rank_min && nowweightVale4[i] < mweight.u02.rank_max) {
                    iro[i] = C_S;
                    //}else if(nowweightVale3[i] >= mweight.u03.rank_min && nowweightVale3[i] < mweight.u03.rank_max){
                } else if(nowweightVale3[i] >= mweight.u03.rank_min && nowweightVale3[i] < mweight.u03.rank_max &&
                          nowweightVale4[i] >= mweight.u03.rank_min && nowweightVale4[i] < mweight.u03.rank_max) {
                    iro[i] = C_M;
                    //}else if(nowweightVale3[i] >= mweight.u04.rank_min && nowweightVale3[i] < mweight.u04.rank_max){
                } else if(nowweightVale3[i] >= mweight.u04.rank_min && nowweightVale3[i] < mweight.u04.rank_max &&
                          nowweightVale4[i] >= mweight.u04.rank_min && nowweightVale4[i] < mweight.u04.rank_max) {
                    iro[i] = C_L;
                    //}else if(nowweightVale3[i] >= mweight.u04.rank_max+1){
                } else if(nowweightVale3[i] >= mweight.u04.rank_max+1 &&
                          nowweightVale4[i] >= mweight.u04.rank_max+1) {
                    iro[i] = C_BATU;
                } else if(nowweightVale3[i] != 0 && nowweightVale3[i] <= param.CUT &&
                          nowweightVale4[i] != 0 && nowweightVale4[i] <= param.CUT ) {
                    iro[i] = C_NULL;
                }
                nowweightVale4[i] = nowweightVale[i];
            }
            /*
            if(cel.getWeight(i)<param.CUT_Y) {
                //wait(0.1);
                if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
                    cel.set_head(i);
                    cel.setAin_zero(i);
                } else {
                    inNow = infoNow(i, inNow);
                }
            }*/

            if(cel.getWeight(i)<param.CUT_Y) {
                kumiawase_er[i]=0;
            }
            //printf("10  %d %6.2f %6.2f %6.2f %4.1f %d\r\n", k+1,oldweightVale[k], nowweightVale[k], nowweightVale2[k],param.BARATSUKI , inNow);
            //oldweightVale[k] = nowweightVale[k];
            //inChk = inChk <<1;
            //}//for
            /*
            //if(int(com->total) > res){//準最適値がONだったらここは絶対NGなので、準最適値がON時は使用しない
            //}else{
                //if (loopcnt <= 10){
                if (loopcnt >= 1 && first_chk_flg==0){
                    //準最適値がOFFで、成立後に再チェックした重量が設定外の場合はループを抜ける
                    if (load.secondOnOff==0 && recheck_kumi == bit_count(match_kumi)&& int(recheck_total) != 0 && (int(recheck_total) <= (load.target-1) || int(recheck_total) > (load.target + load.jougen+1))) {
                        //btn.operation_unlock();
                            //wait(0.5);
                            //lcd.disSums(recheck_total);
                            //wait(0.5);
                        //se.recheck2();    //警告音
                        //btn.operation_lock();
                        //return;
                    }else if (com->total > param.CUT && first_chk_flg == 0 && recheck_total > param.CUT && ((com->total + 0.8 < recheck_total &&  com->total + 5 > recheck_total) || (com->total - 0.8 > recheck_total && com->total - 5 < recheck_total))){//if
                    //成立後に値が±0.8g以上±3g以下の範囲で違っていたら表示を更新する(0.8だとジュニアでは意味がないんじゃ・・・)
                        com->total = recheck_total;
                        // 組み合わせ成立有りの処理を実行
                        lcd.locate(0,1);
                        if( product.productType == BIG || product.productType == BIG_D){
                            //OneWeight=cel.getWeight(3);//川
                            lcd.disSums(com->total);
                            //lcd.disSums(com->total,OneWeight);
                        }else{
                            lcd.disSum(com->total);
                        }
                    }
                }//if
            }//else*/
            //}
            //201704/26 //川 成立皿に追加の重量が載った場合(NGなので)ループを脱出する



            inChk = inChk <<1;
        }//for
        //OldとNowで物体有無の差を取得(差があれば物が置かれたか、もしくは取られた)
        //更にoldで重量があった場所のみを抽出する。(取り間違いか、組み合わせ成立箇所を取った場所を取得できる)
        
        checkCom = (inOld ^ inNow) & inOld;
        ledOff = checkCom & com->kumi;       //組み合わせ成立していた場所の取った場所を取得
        com->kumi -= (ledOff & com->kumi);   //残りの組み合わせ成立場所を取得
        missPoint = checkCom - ledOff;       //取り間違い位置を取得
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            comp_kumi = com->kumi;
        } else {
            led.REGIS(com->kumi);
        }
        //成立した皿に変化があったら(皿が取られたら)フラグを1にする(成立した皿を取り出しはじめたと認識させる)
        if(((inNow & match_kumi) ^ match_kumi )> 0) {
            //後乗せしてそうな皿があったら
            /*if(kumiawase_er[0]>= 5 || kumiawase_er[1]>= 5 || kumiawase_er[2]>= 5 || kumiawase_er[3]>= 5 ||
               kumiawase_er[4]>= 5 || kumiawase_er[5]>= 5 || kumiawase_er[6]>= 5 || kumiawase_er[7]>= 5 ||
               kumiawase_er[8]>= 5 || kumiawase_er[9]>= 5 || kumiawase_er[10]>= 5 || kumiawase_er[11]>= 5 ){

                re_meas2nd=0;
                atonose_limit=0;
                first_chk_flg=0;
                se.mistake();    //警告音
                led.Flash(match_kumi);
                se.mistake();    //警告音
                led.Flash(match_kumi);
                se.mistake();    //警告音
                led.Flash(match_kumi);
                for(int m=0; m < product.head; m++){
                    kumiawase_er[m]=0;
                }
                btn.operation_lock();
                return;
            }else{*/
            first_chk_flg=1;
            atonose_limit=0;
            //printf("===%d %4f %4f %d %d %f\r\n", 4,oldweightVale[3], nowweightVale[3],inChk , inNow,param.MINI_target);
            //printf("===%d %4f %4f %d %d %f\r\n", 5,oldweightVale[4], nowweightVale[4],inChk , inNow,param.MINI_target);
            //se.mistake();
            /*}*/
        }

        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            //取り間違いがあったら警告
            if(missPoint > 0 && com->kumi > 0) {
                //printf("missbit%5d  point%5d\r\n",missPoint,oneLocation[missPoint]);
                cnt_miss_and_kikakugai += 1;
                if(option.beginner==1){
                    if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                        led.REGIS_BATU(missPoint);
                    }else{
                        led.RedOn();
                        led.REGIS(missPoint);
                    }
                    se.mistake3(option.selbeep);    //警告音
                    if(option.kikakugai_v==1){
                        v.mistake2();
                        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                            led.REGIS_BATU(missPoint);
                            /*led.Flash_BATU(missPoint);
                            led.Flash_BATU(missPoint);
                            led.Flash_BATU(missPoint);
                            led.Flash_BATU(missPoint);
                            led.Flash_BATU(missPoint);
                            led.Flash_BATU(missPoint);*/
                        }else{
                            led.Flash(missPoint);
                            led.Flash(missPoint);
                            led.Flash(missPoint);
                            led.Flash(missPoint);
                            led.Flash(missPoint);
                            led.Flash(missPoint);
                        }
                        v.wait_echo();
                    }else{
                        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                            led.REGIS_BATU(missPoint);
                            wait(0.5);
                            //led.Flash_BATU(missPoint);
                        }else{
                            led.Flash(missPoint);
                        }
                    }
                    btn.force_DOWN();
                    reset_flg = 1;
                    btn.operation_unlock();
                }else{
                    if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                        led.REGIS_BATU(missPoint);
                    }else{
                        led.RedOn();
                        led.REGIS(missPoint);
                    }
                    if(option.kikakugai_v==1){
                        se.mistake3(option.selbeep);    //警告音
                        v.mistake();
                        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                            led.REGIS_BATU(missPoint);
                            wait(0.5);
                            //led.Flash_BATU(missPoint);
                        }else{
                            led.Flash(missPoint);
                        }
                        v.wait_echo();
                    }else{
                        se.mistake3(option.selbeep);    //警告音
                        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                            led.REGIS_BATU(missPoint);
                            wait(0.5);
                            //led.Flash_BATU(missPoint);
                        }else{
                            led.Flash(missPoint);
                        }
                        //se.mistake();    //警告音
                    }
                }
                //printf("%3d %3d %3d\r\n",bit_count(inNow),bit_count(missPoint),bscData.startKinji);
                if(bit_count(inNow)< (bscData.startKinji-bit_count(stop_cells))) {
                    re_meas2nd=0;
                }
                //se.mistake();    //警告音
                for(int m=0; m < product.head; m++) {
                    kumiawase_er[m]=0;
                }
            }else if((bit_count(inOld) > bit_count(inNow)) && option.kokuti==3){          
            //成立したものが取られた(載ってる数が変化した＆数が減った&初心者ﾓｰﾄﾞ)
                se.puter(bit_count(com->kumi));
                if(bit_count(com->kumi)==0){
                    //se.chime_01();
                }
            }
        } else {
            //取り間違いがあったら警告
            if(missPoint > 0 && com->kumi > 0) {
                cnt_miss_and_kikakugai += 1;
                if(option.beginner==1){
                    if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                        led.REGIS_BATU(missPoint);
                    }else{
                        led.RedOn();
                        led.REGIS(missPoint);
                    }
                    se.mistake3(option.selbeep);    //警告音
                    if(option.kikakugai_v==1){
                        v.mistake2();
                        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                            led.REGIS_BATU(missPoint);
                            /*led.Flash_BATU(missPoint);
                            led.Flash_BATU(missPoint);
                            led.Flash_BATU(missPoint);
                            led.Flash_BATU(missPoint);
                            led.Flash_BATU(missPoint);
                            led.Flash_BATU(missPoint);*/
                        }else{
                            led.Flash(missPoint);
                            led.Flash(missPoint);
                            led.Flash(missPoint);
                            led.Flash(missPoint);
                            led.Flash(missPoint);
                            led.Flash(missPoint);
                        }
                        v.wait_echo();
                    }else{
                        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                            led.REGIS_BATU(missPoint);
                            wait(0.5);
                            //led.Flash_BATU(missPoint);
                        }else{
                            led.Flash(missPoint);
                        }
                    }
                    btn.force_DOWN();
                    reset_flg = 1;
                    btn.operation_unlock();
                }else{
                    if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                        led.REGIS_BATU(missPoint);
                    }else{
                        led.RedOn();
                        led.REGIS(missPoint);
                    }
                    if(option.kikakugai_v==1){
                        se.mistake3(option.selbeep);    //警告音
                        v.mistake();
                        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                            led.REGIS_BATU(missPoint);
                            wait(0.5);
                            //led.Flash_BATU(missPoint);
                        }else{
                            led.Flash(missPoint);
                        }
                        v.wait_echo();
                    }else{
                        se.mistake3(option.selbeep);    //警告音
                        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                            led.REGIS_BATU(missPoint);
                            wait(0.5);
                            //led.Flash_BATU(missPoint);
                        }else{
                            led.Flash(missPoint);
                        }
                        //se.mistake();    //警告音
                    }
                }
                //printf("%3d %3d %3d\r\n",bit_count(inNow),bit_count(missPoint),bscData.startKinji);
                if(bit_count(inNow)< (bscData.startKinji-bit_count(stop_cells))) {
                    re_meas2nd=0;
                }
                //se.mistake();    //警告音
                for(int m=0; m < product.head; m++) {
                    kumiawase_er[m]=0;
                }
            //}else if(inOld != inNow && (bit_count(inOld) > bit_count(inNow)) && option.beginner==1){       
            }else if((bit_count(inOld) > bit_count(inNow)) && option.kokuti==3){          
            //成立したものが取られた(載ってる数が変化した＆数が減った&初心者ﾓｰﾄﾞ)
                se.puter(bit_count(com->kumi));
                if(bit_count(com->kumi)==0){
                    //se.chime_01(); //場合によっては失敗するので、ループ脱出条件(取り終わった認識時)に移行
                }
            }
        }

        //ループ脱出条件(成立皿を取り終わる,上:再計算キーを押す)
        if(com->kumi == 0 || ((btn.down_press() == true || btn.esc_press() == true) && btn.get_lock_flg() == false) || 
           (reset_flg == 1 && btn.get_lock_flg() == false) ||
           (demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)) {
            re_meas2nd=0;
            atonose_limit=0;
            first_chk_flg=0;
            //re_meas3rd=0;
            for(int m=0; m < product.head; m++) {
                kumiawase_er[m]=0;
            }
            for(int iro_c=0; iro_c<product.head; iro_c++) {
                iro[iro_c] = C_NULL;
            }
            
            if(com->kumi == 0 && option.kokuti==3){
                se.chime_01();
            }
            //*********書き込み用データ収集************
            //if(btn.down_press() == false && sd_en==0){
            //if(btn.down_press() == false){
            if(com->kumi == 0){
                nowlap = coulap;
                coulap = 0;
                total_kumi_cnt += 1;
                kumi_value_data = float(int(com->total*10))/10;
                average_kumi_value = (((average_kumi_value * (total_kumi_cnt -1)) + kumi_value_data) / total_kumi_cnt);
                bit_num = bit_count(match_kumi);    //成立した個数の保存
                bit_place =match_kumi ;
                if(product.use_usb==1 && option.usb_EN==true) {
                    SD_update();
                }
                //se.Touch_02();
                //}
                //if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT || load.kumi_flag == KUMI_COMBI)&& total_kumi_cnt>0) {
                if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT)&& total_kumi_cnt>0) {
                    seconds = time(NULL);
                    tm *dated4 = localtime(&seconds);
                    //printf("ss%d:%d:%d\r\n",dated4->tm_hour,dated4->tm_min,dated4->tm_sec);
                }
            }
            //*****************************************
            btn.operation_lock();
            if(option.kokuti==0||option.kokuti==2||option.kokuti==3) {
                se.Touch_02();
            }
            comp_kumi=0;
            return;
        }//if
        /**/

        if(loopcnt<5000) {
            loopcnt++;
        }
        btn.old_btn();
    }//while無限ループ
}
void SD_update()
{
    //mkdir("/sd/example", 0777);
//統計データ更新
    FILE *fp = fopen("/local/TOTALQ.txt", "w");
    if(fp == NULL) {
        error("Could not open file for writeTOTALQTY\n");
    }
    //fprintf(fp, "total  :%6d times\r\n",total_kumi_cnt);
    fprintf(fp, "%d",total_kumi_cnt);

    //fprintf(fp, "now_KAWABE writing!!");
    fclose(fp);
    fp = NULL;
//平均データ更新
    FILE *fp2 = fopen("/local/AVERAG.txt", "w");
    if(fp2 == NULL) {
        error("Could not open file for writeAVERAGE_WEIGHT\n");
    }
    //fprintf(fp2, "average:%6.1f g\r\n\r\n",average_kumi_value);
    fprintf(fp2, "%0.1f",average_kumi_value);

    //fprintf(fp2, "now_KAWABE writing!!");
    fclose(fp2);
    fp2 = NULL;
//個別データ追加書き込み
    FILE *fp3 = fopen("/local/00INDI.TXT", "a");
    //FILE *fp3 = fopen("/usb/00INDI.TXT", "a");
    if(fp3 == NULL) {
        error("Could not open file for writeINDIVIDUAL\n");
    }
    //fprintf(fp3, "%6d\t  %2d\t% 6.1f\r\n",bit_place,bit_num,kumi_value_data);
    fprintf(fp3, "%2d\t %4d\t %6.1f\t %6.1f\r\n",bit_num,bit_place,kumi_value_data,float(float(nowlap)/10));
    fclose(fp3);
    fp3 = NULL;
}

void SD_initialise()
{
//個別データ追加書き込み
    FILE *fp3 = fopen("/local/00INDI.TXT", "w");
    //FILE *fp3 = fopen("/usb/00INDI.TXT", "w");
    if(fp3 == NULL) {
        error("Could not open file for write\n");
    }
    //fprintf(fp3, " place\t num\t value\r\n");
    fprintf(fp3, "");
    fclose(fp3);
    fp3 = NULL;
}

//組み合わせ成立候補の計量が安定しているかを確認する
bool checkWeightTaget(combiData_t *com)
{
    float min = float(load.target) + param.buffer;  //最小値(バッファー有り)
    float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    float checkTotal=0;
    int bk = com->kumi;           //組み合わせ箇所を取得

    //重量がゼロなら処理終了
    if(com->kumi == 0 || com->total <= 0) {
        com->kumi = 0;  //組み合わせ無し初期化
        com->total = 0.0;
        return false;
    }

    //目標重量から+5g以上だったら処理と飛ばす
    //振動で計量重量ずれていても5g以上は計量がずれるとは考えにくいため
    if(param.vivration == 2 && (com->total > (load.target + com->kumi * param.buffer2 + param.buffer + LOW_LIMIT_START))) {
        return true;
    }

    //組み合わせが準最適値での組み合わせなので、計量重量に精度は不要
    if( com->total > res) {
        return true;
    }

    //ビット1になっている箇所の重量を取得していく
    for(int bit = 0; bk; bk >>= 1) {
        if (bk & 0x01) {
            setHeadVolt(bit, param.KURI);   //重量再取得

            //精度が出ていたら
            if(cel.ArrayCheck()) {
                //重量を取得していく
                checkTotal += cel.getArrayWeight(WHT_CNT-1) + op_n_hosei + hosei_f[i] - MINUS;
                //重量が安定していないため、現在の組み合わせは解除して処理終了
            } else {
                com->kumi = 0; //組み合わせ解除
                com->total = 0.0;
                return false; //処理終了
            }
        }//if
        bit++;
    }

    //ターゲット重量内かを確認
    if( (checkTotal >= min + com->kumi * param.buffer2) && int(checkTotal) <= int(res)) {
        com->total = checkTotal; //安定確実を確認した総重量を取得(
        return true;
    }

    //範囲内の重量ではない。ここまで来たらNG
    com->kumi = 0;
    com->total = 0.0;

    return false;
}

/*****************************************************************************
規定の下限、上限重量を超えていたら警告
*****************************************************************************/
bool check_max_weight(int overHead)
{
    float overWeight = 0;

    //一番最初の皿の既定重量が重い場所を取得
    for(int i = 0; i < product.head; i++) {
        if(overHead & oneBit[i]) {
            //表示する値を取得する
            overWeight = cel.anteiWeight(i);
            break;
        }
    }

    //重量に異常があれば
    if(overHead > 0) {
        lcd.bigNumber(overWeight);
        se.mistake();                   //警告音
        cnt_miss_and_kikakugai += 1;
        if(option.kikakugai_v==1) {
            v.output_audio(MSG_NO_RANK);    //音声出力
        }
        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
            led.REGIS_BATU(overHead);
            wait(0.8);
            //led.Flash_BATU(overHead);
        }else{
            led.Flash(overHead);            //LED点滅
        }
        if(option.kikakugai_v==1) {
            v.wait_echo();
        }
        wait(0.3);                      //2017-02-14 (2→ 0.5に変更)
        if(product.head <= 8) {           //2017-07-25 (2→ 0.5に変更)
            wait(0.5);                  //6ヘッドだと音声合成が鳴りやむ前にエラー判定が続いて、警告音がおかしくなる為
        }
        return false;
    }

    //適正重量のみ
    return true;
}
void miri_count(){
    miri_end=1;
}

//最適重量組合せ結果表示関数(通常の組み合わせ)
void show_results(const combiData_t *com){
    float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    combiData_t orderCombi[BK_CNT] = {0,0.0,0};   //組み合わせ候補のデータ(配列)
    combiData_t resultCombi;
    const char kumiawase[]={0xB8,0xD0,0xB1,0xDC,0xBE,0x00};      //ｸﾐｱﾜｾ

    int on_cell =0; //皿に何か載っているか確認する変数
    //int hyper_kumi =0;  //準最適値時に最初に入力した値を保存する変数
    //int hyper_total =0; //準最適値時に最初に入力した値を保存する変数
    //int loop_cel =0;      //ユックリモード時のループ回数

    //組み合わせ内容表示分をクリア
    //lcd.blank(LOW);
    //組み合わせなければ処理終了
    if(com->kumi == 0) {
        //if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)){
        if((product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            noCombiNews();
        } else {
            noCombiNews();
        }
        if(load.kumi_flag <= KUMI_LIMIT){//組合せの場合は下段に「ｸﾐｱﾜｾ」を表示する
            lcd.locate(9,1);
            if(cnt.max == 0){
                lcd.printf(kumiawase);
            }
            lcd.locate(0,1);
        }
        return;
    }
    //loop_cel=0;
    on_cell =0;
    for(int on_c=0; on_c < product.head; on_c++) {
        on_cell += oneBit[on_c];
    }
    //準最適値表示の場合は大文字表示
    if(int(com->total) > res) {
        lcd.bigNumber(com->total); //大文字で重量を表示
        //}

        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            if(load.spMode == SPEED_HIGH){
                for(int i = 0; i <= product.head ; i++){
                    if( (com->kumi & 1<<i)>0){
                        if((com->total - res)<99){
                            iro[i] = CR_01 + com->total - res -1;
                        }else{
                            iro[i] = CR_99;
                        }
                    }else{
                        iro[i] = C_NULL;
                    }
                }
            }else{
                led.REGIS3(com->kumi);
            }
        } else {
            led.REGIS(com->kumi);
        }

        //第二上限値内の組み合わせ成立だったらLED点滅させる
        /*if(load.spMode == SPEED_LOW &&(load.secondOnOff == 1 ||btn.get_lock_flg() == true) && (int(com->total) > res)) {
            junsaiteki_loop = true;
            se.Touch_02();
            int btn_flg = 0;
            int end_flg_j = 0;
            int loop_i =0;
            resultCombi.kumi  = com->kumi ;                    //最小組み合わせ結果を取得
            resultCombi.total = com->total;
            while(end_flg_j == 0) {
                loop_cel = (loop_i*2)%product.head;
                if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                    led.REGIS3(resultCombi.kumi);
                } else {
                    led.REGIS(resultCombi.kumi);
                    wait(0.02);
                    led.REGIS(0);
                }
                setHeadVolt(loop_cel, param.KURI/3*2);
                if((cel.getWeight(loop_cel)>param.CUT) &&
                        ((on_cell & oneBit[loop_cel])==0)) {
                    on_cell = on_cell | oneBit[loop_cel];
                } else if((cel.getWeight(loop_cel)<param.CUT) &&
                          ((on_cell & oneBit[loop_cel])!=0)) {
                    on_cell = on_cell ^ oneBit[loop_cel];
                } else {
                }
                loop_cel += 1;
                setHeadVolt(loop_cel, param.KURI/3*2);
                if((cel.getWeight(loop_cel)>param.CUT) &&
                        ((on_cell & oneBit[loop_cel])==0)) {
                    on_cell = on_cell | oneBit[loop_cel];
                } else if((cel.getWeight(loop_cel)<param.CUT) &&
                          ((on_cell & oneBit[loop_cel])!=0)) {
                    on_cell = on_cell ^ oneBit[loop_cel];
                } else {
                }
                int secondCnt = 0;
                get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
                resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得
                if(int(resultCombi.total) >= load.target && int(resultCombi.total) < res) {
                    setHeadVolt((loop_i*2)%product.head  , param.KURI);
                    setHeadVolt((loop_i*2)%product.head+1, param.KURI);
                    get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
                    resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得
                    if(int(resultCombi.total) >= load.target && int(resultCombi.total) < res) {
                        end_flg_j = 1;
                        se.Touch_02();
                    }
                    lcd.bigNumber(resultCombi.total); //大文字で重量を表示
                }
                lcd.bigNumber(resultCombi.total); //大文字で重量を表示
                //wait(0.05);
                //再計算ボタンが押されたら、待機時間を待たずに終了する。
                //ただし、ボタンが離されたことを確認後にボタンを押されたらとする
                //(ボタンを押すことでここに入ってきてるかもしれないので)
                if((btn_flg == 1 && btn.down_press() == true) || on_cell <= 0) {
                    se.Touch_02();
                    junsaiteki_loop = false;
                    end_flg_j = 1;
                    led.REGIS(0);
                } else if(btn.down_press() == false) {
                    btn_flg = 1;
                }
                loop_i += 1;
                if(loop_i >= product.head/2) {
                    loop_i =0;
                }
            }
        }*/
        //ゆっくりモード,上ボタン押下で、準最適値モードだったら音鳴らして終了
        //(ここの時点ではSlolyに行く動作か、ユックリモード内のループ中か判断できない)
        if(btn.get_lock_flg() == true && load.spMode == SPEED_LOW/* && product.limit_on != WIDE_LIMIT*/){
            se.Touch_02();
            //return;
        }else if(load.spMode == SPEED_LOW /*&& product.limit_on != WIDE_LIMIT*/){
        //ゆっくりモードだったら終了
            //return;
        }
/* 組み合わせ成立有りの処理を実行 */
    } else {
        lcd.blank(LOW);
        if(load.kumi_flag <= KUMI_LIMIT){//組合せの場合は下段に「ｸﾐｱﾜｾ」を表示する
            lcd.locate(9,1);
            if(cnt.max == 0){
                lcd.printf(kumiawase);
                lcd.locate(0,1);
            }
        }
        if( product.productType == BIG || product.productType == BIG_D) {
            //OneWeight=cel.getWeight(3);//川
            lcd.disSums(com->total);
            //lcd.disSums(com->total,OneWeight);
        } else {
            lcd.disSum(com->total);
        }
    }
    if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) && int(com->total) > load.target && int(com->total) < res) {
        comp_kumi = com->kumi;
    } else {
        
        //設定時間LED表示(高速モード時のみ)
        if(load.spMode == SPEED_HIGH  || load.spMode == SPEED_DAMP) {
            if(option.kokuti==1||option.kokuti==2||option.kokuti==3){
                se.Touch_02();
            }
            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                for(int i = 0; i <= product.head ; i++){
                    if( (com->kumi & 1<<i)>0){
                        if((int(com->total) > res)) {
                            if((com->total - res)<99){
                                iro[i] = CR_01 + com->total - res -1;
                            }else{
                                iro[i] = CR_99;
                            }
                        }else{
                            iro[i] = CG_01 + bit_count(com->kumi)-1;
                        }
                    }else{
                        iro[i] = C_NULL;
                    }
                }
            }else{
                led.REGIS(com->kumi);
            }
            //第二上限値内の組み合わせ成立だったらLED点滅させる
            if((load.secondOnOff == 1 || btn.get_lock_flg() == true) && (int(com->total) > res)) {
                se.Touch_02();
                int btn_flg = 0;
                for(int i = 0; i <= load.lightTime; i += 1) {
                    if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                        for(int i = 0; i <= product.head ; i++){
                            if( (com->kumi & 1<<i)>0){
                                if((com->total - res)<99){
                                    iro[i] = CR_01 + com->total - res -1;
                                }else{
                                    iro[i] = CR_99;
                                }
                            }else{
                                iro[i] = C_NULL;
                            }
                        }
                    }else{
                        led.REGIS(com->kumi);
                    }
                    wait(0.05);
                    led.REGIS(0);
                    wait(0.05);
                    //再計算ボタンが押されたら、待機時間を待たずに終了する。
                    //ただし、ボタンが離されたことを確認後にボタンを押されたらとする
                    //(ボタンを押すことでここに入ってきてるかもしれないので)
                    if(btn_flg == 1 && btn.down_press() == true) {
                        se.Touch_02();
                        i = load.lightTime + 1;
                        
                        /*if(servo_connection==true && load.spMode == SPEED_DAMP){
                            moving2=99;
                            send_i(EJECT_RESET, 0, 0);
                            loop_a = 0;
                            while((check_signal()==false&&loop_a<=10) ||moving2!=0){
                            //while(check_signal()==false&&loop_a<=10){
                                loop_a+=1;
                                wait(0.1);
                                printf("ASA\r\n");
                            }
                        }*/
                        
                        
                        if(servo_connection==true && load.spMode == SPEED_DAMP){
                            //moving2=99;
                            //send_i(EJECT_RESET, 0, 0);
                            loop_a = 0;
                            while((check_signal()==false&&loop_a<=10) /*|| moving2!=0*/){
                            //while(check_signal()==false&&loop_a<=10){
                                send_i(ON_CHECK,0,0);  //動作してる皿が無いか確認
                                loop_a+=1;
                                wait(0.1);
                            }
                            while(btn.down_press() == true) {wait(0.1);}
                        }
                        
                    } else if(btn.down_press() == false) {
                        btn_flg = 1;
                    }
                }
                no_combi_v_flg = false;
            } else {
                if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                    for(int i = 0; i <= product.head ; i++){
                        if( (com->kumi & 1<<i)>0){
                            iro[i] = CG_01 + bit_count(com->kumi)-1;
                        }else{
                            iro[i] = C_NULL;
                        }
                    }
                }else{
                    led.REGIS(com->kumi);
                }
                if(servo_connection==true && load.spMode == SPEED_DAMP){
                    signal_flg=false;
                    signal_loop =0;
                    send_i(ON_CHECK,0,0);
                    while(check_signal()==false&&signal_loop<=10){
                        signal_loop+=1;
                        wait(0.1);
                    }
                    //if(moving2==0){
                        send_i(SERVO_ON,com->kumi,0);while(check_signal()==false){wait(0.1);}
                        if(go_flager==1){
                            send_i(FAKE_GO,0,0);while(check_signal()==false){wait(0.1);}
                        }
                        
                        
                        nowlap = coulap;
                        coulap = 0;
                        total_kumi_cnt += 1;
                        kumi_value_data = float(int(com->total*10))/10;
                        average_kumi_value = (((average_kumi_value * (total_kumi_cnt -1)) + kumi_value_data) / total_kumi_cnt);
                        //bit_num = bit_count(com->kumi);    //成立した個数の保存
                    //}
                }
                //wait(load.lightTime*0.1);
                for(int i = 0; i <= load.lightTime; i += 1) {
                    //send_i(ON_CHECK,0,0);
                    wait(0.1);
                    //再計算ボタンが押されたら、待機時間を待たずに終了する。
                    if(btn.down_press() == true) {
                        se.Touch_02();
                        i = load.lightTime + 1;
                       /* if(servo_connection==true && load.spMode == SPEED_DAMP){
                            moving2=99;
                            send_i(EJECT_RESET, 0, 0);
                            loop_a = 0;
                            while((check_signal()==false&&loop_a<=10) ||moving2!=0){
                                loop_a+=1;
                                wait(0.1);
                                printf("ASA\r\n");
                            }
                        }*/
                        
                        if(servo_connection==true && load.spMode == SPEED_DAMP){
                            //moving2=99;
                            //send_i(EJECT_RESET, 0, 0);
                            loop_a = 0;
                            while((check_signal()==false&&loop_a<=10)/* || moving2!=0*/){
                            //while(check_signal()==false&&loop_a<=10){
                                send_i(ON_CHECK,0,0);  //動作してる皿が無いか確認
                                loop_a+=1;
                                wait(0.1);
                            }
                            while(btn.down_press() == true) {wait(0.1);}
                        }
                    }
                    if(servo_connection==true && check_signal()==true && moving1 ==0 && load.spMode == SPEED_DAMP) {
                        i = load.lightTime + 1;
                    }
                }
                no_combi_v_flg = false;
                
                signal_flg=false;
                signal_loop =0;
                send_i(ON_CHECK,0,0);
                while(check_signal()==false&&signal_loop<=10){
                    signal_loop+=1;
                    wait(0.1);
                }
            }
        }
        //設定時間LED表示(ハイパーモード時のみ)
        else if(load.spMode == SPEED_HYPER){
            if(option.kokuti==1||option.kokuti==2||option.kokuti==3){
                se.Touch_02();
            }
            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                if(load.spMode == SPEED_HIGH){
                    for(int i = 0; i <= product.head ; i++){
                        if( (com->kumi & 1<<i)>0){
                            iro[i] = CG_01 + bit_count(com->kumi)-1;
                        }else{
                            iro[i] = C_NULL;
                        }
                    }
                }else{
                    led.REGIS3(com->kumi);
                }
            }else{
                led.REGIS(com->kumi);
            }
            //第二上限値内の組み合わせ成立だったらLED点滅させる
            if((load.secondOnOff == 1 ||btn.get_lock_flg() == true) && (int(com->total) > res)){
                junsaiteki_loop = 1;
                se.Touch_02();
                int btn_flg = 0;
                resultCombi.kumi  = com->kumi ;                    //最小組み合わせ結果を取得
                resultCombi.total = com->total;
                for(int i = 0; i <= load.lightTime; i += 1){
                    if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                        if(load.spMode == SPEED_HIGH){
                            for(int i = 0; i <= product.head ; i++){
                                if( (com->kumi & 1<<i)>0){
                                    iro[i] = CG_01 + bit_count(com->kumi)-1;
                                }else{
                                    iro[i] = C_NULL;
                                }
                            }
                        }else{
                            led.REGIS3(com->kumi);
                        }
                        wait(0.02);
                    }else{
                        led.REGIS(resultCombi.kumi);
                        wait(0.02);
                        led.REGIS(0);
                    }
                    setHeadVolt((i*2)%product.head  , param.KURI/3*2);
                    setHeadVolt((i*2)%product.head+1, param.KURI/3*2);
                    int secondCnt = 0;
                    Compulsion_no_combi_v_flg = true;
                    get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
                    Compulsion_no_combi_v_flg = false;
                    //printf("o %8.1f:r %8.1f\r\n",orderCombi[0].total,resultCombi.total);
                    //組合せ確認で出てきた結果が[組合せ範囲内] or [前回より軽かった]場合は、resultCombiを更新する
                    if(int(orderCombi[0].total) >= load.target && int(orderCombi[0].total) < res/* || orderCombi[0].total < resultCombi.total*/){
                        resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得
                    }
                    if(int(resultCombi.total) >= load.target && int(resultCombi.total) < res){
                        setHeadVolt((i*2)%product.head  , param.KURI/3*2);
                        setHeadVolt((i*2)%product.head+1, param.KURI/3*2);
                        get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
                        resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得
                        //printf("o %8.1f:r %8.1f\r\n",orderCombi[0].total,resultCombi.total);
                        if(int(resultCombi.total) > load.target && int(resultCombi.total) < res){
                            junsaiteki_loop = 2;
                            i = load.lightTime + 1;
                            //return;
                        }
                        //com->kumi  = resultCombi.kumi;                    //最小組み合わせ結果を取得
                        //com->total = resultCombi.total;
                        //lcd.bigNumber(resultCombi.total); //大文字で重量を表示
                    }
                    else{
                        lcd.bigNumber(resultCombi.total); //大文字で重量を表示
                    }
                    //wait(0.05);
                    //再計算ボタンが押されたら、待機時間を待たずに終了する。
                    //ただし、ボタンが離されたことを確認後にボタンを押されたらとする
                    //(ボタンを押すことでここに入ってきてるかもしれないので)
                    if(btn_flg == 1 && btn.down_press() == true){
                        se.Touch_02();
                        junsaiteki_loop = 0;
                        i = load.lightTime + 1;
                        led.REGIS(0);
                    }else if(btn.down_press() == false){
                        btn_flg = 1;
                    }
                    lcd.bigNumber(resultCombi.total); //大文字で重量を表示
                }
                no_combi_v_flg = false;
            }else{
                led.REGIS(com->kumi);
                //wait(load.lightTime*0.1);
                for(int i = 0; i <= load.lightTime; i += 1){
                    wait(0.1);
                    //再計算ボタンが押されたら、待機時間を待たずに終了する。
                    if(btn.down_press() == true){
                        se.Touch_02();
                        i = load.lightTime + 1;
                    }
                }
                no_combi_v_flg = false;
            }
        }//設定時間LED表示(ユックリの準最適値のみ)
        else if(load.spMode == SPEED_LOW /*&& product.limit_on == WIDE_LIMIT*/ &&
                (load.secondOnOff == 1 ||btn.get_lock_flg() == true) && (int(com->total) > res)){
            //if(option.kokuti==1||option.kokuti==2){
                se.Touch_02();
            //}
            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                //led.REGIS3(com->kumi);
                led.REGIS(com->kumi);
            }else{
                led.REGIS(com->kumi);
            }
            //第二上限値内の組み合わせ成立だったらLED点滅させる
            if((load.secondOnOff == 1 ||btn.get_lock_flg() == true) && (int(com->total) > res)){
                cnt_junsaiteki += 1;
                junsaiteki_loop = 1;
                se.Touch_02();
                int btn_flg = 0;
                resultCombi.kumi  = com->kumi ;                    //最小組み合わせ結果を取得
                resultCombi.total = com->total;
                miri_end=0;
                //miri_sec.attach( &miri_count, 0.1);
                miri_sec.attach( &miri_count, float(load.lightTime/10));
                for(int i=0;i <= load.lightTime; i += 1){ //指定時間まで光らせる(成立すれば終了)
                    /*if(product.matrix_mode == MATRIX_ON){
                        if(i == 0){
                            //led.REGIS3(resultCombi.kumi);
                            led.REGIS(resultCombi.kumi);
                            lcd.bigNumber(resultCombi.total); //大文字で重量を表示
                        }
                    }else{*/
                        led.REGIS(resultCombi.kumi);
                        wait(0.02);
                        led.REGIS(0);
                    //}
                    setHeadVolt((i*2)%product.head  , param.KURI/3);  setHeadVolt((i*2)%product.head+1, param.KURI/3);
                    if(product.matrix_mode == MATRIX_ON){
                        setHeadVolt((i*2)%product.head+2, param.KURI/3);  setHeadVolt((i*2)%product.head+3, param.KURI/3);
                        setHeadVolt((i*2)%product.head+4, param.KURI/3);  setHeadVolt((i*2)%product.head+5, param.KURI/3);
                        if(product.head==8){
                            setHeadVolt((i*2)%product.head+6, param.KURI/3);  setHeadVolt((i*2)%product.head+7, param.KURI/3);
                        }
                    }
                    int secondCnt = 0;
                    Compulsion_no_combi_v_flg = true;
                    get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
                    Compulsion_no_combi_v_flg = false;
                    //printf("o %8.1f:r %8.1f\r\n",orderCombi[0].total,resultCombi.total);
                    //組合せ確認で出てきた結果が[組合せ範囲内] or [前回より軽かった]場合は、resultCombiを更新する
                    if(int(orderCombi[0].total) >= load.target && int(orderCombi[0].total) < res/* || orderCombi[0].total < resultCombi.total*/){
                        resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得
                    }
                    if(int(orderCombi[0].total) >= load.target && int(orderCombi[0].total) < res){
                        setHeadVolt((i*2)%product.head  , param.KURI/3);  setHeadVolt((i*2)%product.head+1, param.KURI/3);
                        if(product.matrix_mode == MATRIX_ON){
                            setHeadVolt((i*2)%product.head+2, param.KURI/3);  setHeadVolt((i*2)%product.head+3, param.KURI/3);
                            setHeadVolt((i*2)%product.head+4, param.KURI/3);  setHeadVolt((i*2)%product.head+5, param.KURI/3);
                            if(product.head==8){
                                setHeadVolt((i*2)%product.head+6, param.KURI/3);  setHeadVolt((i*2)%product.head+7, param.KURI/3);
                            }
                        }
                        /*orderCombi[0].total=0;
                        for(int j =0;j<=product.head;j++){
                            if(((1<<j)& orderCombi[0].kumi)>0){
                                setHeadVolt(j, param.KURI/3);
                                orderCombi[0].total += cel.get_now_weight();
                            }
                        }*/
                        get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
                        resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得
                        //printf("o %8.1f:r %8.1f\r\n",orderCombi[0].total,resultCombi.total);
                        if(int(orderCombi[0].total) > load.target && int(orderCombi[0].total) < res){
                            junsaiteki_loop = 2;
                            i = load.lightTime + 1;
                            resultCombi = orderCombi[0]; 
                            //return;
                            miri_end=1;
                        }
                        //com->kumi  = resultCombi.kumi;                    //最小組み合わせ結果を取得
                        //com->total = resultCombi.total;
                        //lcd.bigNumber(resultCombi.total); //大文字で重量を表示
                    }
                    else{
                        lcd.bigNumber(resultCombi.total); //大文字で重量を表示
                    }
                    //wait(0.05);
                    //再計算ボタンが押されたら、待機時間を待たずに終了する。
                    //ただし、ボタンが離されたことを確認後にボタンを押されたらとする
                    //(ボタンを押すことでここに入ってきてるかもしれないので)
                    if(btn_flg == 1 && btn.down_press() == true){
                        se.Touch_02();
                        junsaiteki_loop = 0;
                        i = load.lightTime + 1;
                        led.REGIS(0);
                    }else if(btn.down_press() == false){
                        btn_flg = 1;
                    }
                    if(product.matrix_mode == MATRIX_ON){
                        //i = load.lightTime + 1;
                    }else{
                        lcd.bigNumber(resultCombi.total); //大文字で重量を表示
                    }
                    if(miri_end==1){
                        miri_sec.detach();
                        i = load.lightTime;
                        miri_end=0;
                    }
                }
            }
            no_combi_v_flg = false;
        }
    }
    if(int(com->total) > res) {
        lcd.cls();
    }
}




//最適重量組合せ結果表示関数(通常の組み合わせ)
void show_rank(const combiData_t *com)
{
    lcd.settings(&load);            //設定内容を更新
    //組み合わせ内容表示分をクリア
    lcd.blank(LOW);

    //組み合わせなければ処理終了
    if(com->kumi == 0) {
        noCombiNews();
        return;
    }

    /* 組み合わせ成立有りの処理を実行 */
    lcd.disSum(com->total);

    led.REGIS(com->kumi);
    wait(load.lightTime*0.1);
}
//最適重量組合せ結果表示関数(通常の組み合わせ)
void show_rank2(const combiData_t *com)
{
    if(load.kumi_flag == KUMI_KAIKYU) {
        lcd.settings(&load);            //設定内容を更新
    } else if(load.kumi_flag == KUMI_RANK) {
        lcd.settings_rank2(&load);            //設定内容を表示
    }

    //組み合わせ内容表示分をクリア
    lcd.blank(LOW);
    if(load.kumi_flag == KUMI_RANK) {
        int presetNo = 0;
        presetNo = file.get_preset();
        //メッセッジ表示
        lcd.printf("%02d:",presetNo+1);
        printname(presetNo);

    } else {
        //組み合わせあれば
        if(com->kumi != 0) {
            /* 組み合わせ成立有りの処理を実行 */
            lcd.disSum(com->total);
        }
        //組み合わせなければ赤LED点灯
        else {
            lcd.blank(LOW);
            lcd.printf(":----.-g");
            //noCombiNews();
        }

        //led.REGIS(com->kumi);
        //wait(load.lightTime*0.1);
    }
    wait(0.1);
}
void show_check(const combiData_t *com)
{
    if(load.kumi_flag == KUMI_KAIKYU) {
        lcd.settings(&load);            //設定内容を更新
    } else if(load.kumi_flag == KUMI_RANK) {
        lcd.settings_rank2(&load);            //設定内容を表示
    }

    //組み合わせ内容表示分をクリア
    lcd.blank(LOW);
    lcd.printf("%dg %c %dg",load.target,0x7e,load.target+load.jougen);
    wait(0.1);
}

void show_check_juuryo(const combiData_t *com)
{
    if(load.kumi_flag == KUMI_KAIKYU) {
        lcd.settings(&load);            //設定内容を更新
    } else if(load.kumi_flag == KUMI_RANK) {
        lcd.settings_rank2(&load);            //設定内容を表示
    }else{
        lcd.settings_rank2(&load);            //設定内容を表示
    }

    //組み合わせ内容表示分をクリア
    lcd.blank(LOW);
    lcd.printf("%dg %c %dg",load.target,0x7e,load.target+load.jougen);
    wait(0.1);
}
//カウント状態を表示する
void show_count(const countData_t *cnt)
{
    //ゆっくりモード時にはカウント表示
    if(cnt->nowMode >=1) {
        if(cnt->max > 0) lcd.count(cnt->num, cnt->max);
    }
}

//組み合わせ不成立時の各種表示
void noCombiNews()
{
    if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
    } else {
        led.RedOn();    //RED LED[組み合わせ] 未成立:点灯 成立:消灯
        if(moving1 == 0){
        }else{
            led.REGIS(moving1);
        }
    }
    if(gLevel == 0){
        lcd.blank(LOW);
        lcd.printf(":----.-g");
    }
    wait(SPEED);
}

//最適重量組合せ結果表示関数(対面型の組み合わせに使用)
void show_results(const int com,const stateWeight_t stateW)
{
    //組み合わせ内容表示行をクリア
    lcd.blank(LOW);

    //組み合わせなければ処理終了
    if(com == 0) {
        noCombiNews();
        return;
    }

    //重量表示
    lcd.disSum(stateW.sumOldNum);
    lcd.disSum(stateW.sumYoungNum);


    //設定時間LED表示(対面はゆっくりモードで固定)
    led.REGIS(com);
    wait(0.03);
}

//滞留場所を通知(アナウンス)
void announceRetention(int com, int stay_cnt[])
{
    int location = 0;   //滞留している場所取得
    if(param.tairyu <= 0) return;   //滞留閾値が0なら処理終了

    //重量無しの計量皿は滞留カウント初期化
    for(int i=0; i< product.head; i++) {
        if(cel.anteiWeight(i) <= 0) {
            stay_cnt[i]=0;
        }
    }//for

    if(com ==0) return;
    for(int i=0; i<product.head; i++) {
        //組み合わせ有 && 重量有 && 組み合わせ不参加 の計量皿
        if((cel.anteiWeight(i) > 0)&& (com & 0x01) == 0 ) {
            stay_cnt[i]++;
            //組合せ場所は「0」にする。
        } else {
            stay_cnt[i]=0;
        }//if

        com >>= 1;  //調べる計量皿の重量有無を1bit目に移動

        //もしもstay_cnt[]がparam.tairyuよりおおきくなれば・・・
        if(stay_cnt[i] >= param.tairyu) {
            location += oneBit[i];
            stay_cnt[i] = 0;              //滞留カウント数初期化
        }
    }//for

    //閾値以上の滞留箇所があればアナウンス
    if(location > 0) {
        se.mistake();               //警告音
        v.output_audio(ERR_TAIRYU); //音声出力
        v.wait_echo();
        led.Flash(location);       //滞留位置LED点灯
        wait(2.5);
        led.ledAllOff();
    }//if
}

//対面型の組み合わせ計量
void drive_face_to_face()
{
    stateWeight_t stateW = {0},stateIniW = {0};
    statePosition_t stateP = {0}, stateIniP = {0};   //各状態の位置を取得

    lcd.settings(TOP, load);  //設定内容を表示

    while(1) {
        led.ledAllOff();        // LED消灯
        btn.new_btn();          //スイッチ(New)取得
        stateP.miss = 0;        //取り間違い位置を初期化

        //esc_swを押すと、Menu画面に戻る
        if(btn.esc()) {
            se.Touch_01();
            return;
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            auto_zero_point();
            //lcd.settings(TOP, load);  //設定内容を表示
        }

        //ループ脱出条件(成立皿を取り終わる,下キーを押す)
        if(stateP.combi > 0 && btn.down_press()) {
            se.Touch_02();
            stateP = stateIniP;
            stateW = stateIniW;
        }//if

        btn.old_btn();       //スイッチ更新

        //組み合わせ若番側
        if( (stateP.combi & LOW_HEAD) == 0) {
            get_head_mask_volt(LOW_HEAD);           //マスク箇所の電圧を取得
            get_combi_half(load, &stateP, LOW_HEAD);
            stateW.sumYoungNum = cel.getSum(stateP.combi & LOW_HEAD);
        } else {
            get_taimen_miss( &stateP, LOW_HEAD );
        }

        //組み合わせ後番側
        if( (stateP.combi & UP_HEAD) == 0) {
            get_head_mask_volt(UP_HEAD);           //マスク箇所の電圧を取得
            get_combi_half(load, &stateP, UP_HEAD);
            stateW.sumOldNum = cel.getSum(stateP.combi & UP_HEAD);
        } else {
            get_taimen_miss( &stateP, UP_HEAD );
        }

        //組み合わせがなければ
        if(stateP.combi == 0) {
            stateP.over = stateIniP.over;                     //位置情報を初期化
            stateP.over = cel.over_weight_taimen();
            check_max_weight(stateP.over);  //適正重量外だったらエラー
        }

        //取り間違いを通知
        if(stateP.miss > 0 && stateP.combi > 0) {
            led.Flash(stateP.miss);
            se.mistake();    //警告音
        }

        //設定内容を表示(大文字表示で内容が非表示となる)
        lcd.settings(TOP, load);

        //組み合わせに応じたLED点灯,総重量表示
        show_results(stateP.combi, stateW);
    }//while
}

//片側の組み合わせ状態を取得
void get_combi_half(loadcell_t val, statePosition_t *stateP, int mask)
{
    statePosition_t stateCopyP = {0};   //状態をバックアップ

    stateCopyP.combi = COMBI(val, mask);    //組み合わせ取得

    //ゆっくりモード以降前準備
    if(stateCopyP.combi > 0) {
        cel.setAinSpeedDummy(mask);
        for(int i = mask, bit = 0; i; i >>= 1) {
            //計量物が載っているか取得
            if(cel.checkIn(bit)) {
                stateCopyP.on += oneBit[bit];
            }
            bit++;
        }
    }

    //取得した値を更新
    stateP->combi &= ~mask;            //マスク箇所を0クリア
    stateP->combi |= stateCopyP.combi;    //マスク箇所更新して計量物の有無をビットで取得

    stateP->on &= ~mask;            //マスク箇所を0クリア
    stateP->on |= stateCopyP.on;    //マスク箇所更新して計量物の有無をビットで取得
}

//成立箇所のmiss位置を取得する
void get_taimen_miss(statePosition_t *stateP, const int mask)
{
    //対面のため、片面のみ規定回数分、計量皿に重量物があるか確認する
    for(int i = 0; i < BK_CNT; i++) {
        stateP->on |= stateP->combi;  //組み合わせ成立箇所は現在物が載っている(強制)
        stateP->onOld = stateP->on;  //皿の状態を保存

        //計量皿に物が載っているか確認
        for(int i = mask, bit = 0; i; i >>= 1) {
            if(i & 0x01) stateP->on = infoNow(bit, stateP->on);
            bit++;
        }

        //取った位置、取り間違い位置を取得
        get_miss_position(stateP);
    }
}

//取り間違い位置、取った位置を取得
void get_miss_position(statePosition_t *stateP)
{
    int onDiff = 0; //前回と今回の計量物が置かれている場所の差
    int ledOff = 0; //LEDを消灯する場所

    onDiff = (stateP->onOld ^ stateP->on) & stateP->onOld;    //置き場所の差分を抽出
    ledOff = onDiff & stateP->combi;       //組み合わせ成立していた場所の取った場所を取得
    stateP->combi -= ledOff;                 //残りの組み合わせ成立場所を取得
    stateP->miss |= onDiff - ledOff;  //取り間違い位置を取得
}

//対面モードで使用する組み合わせ関数
int COMBI(loadcell_t val, int mask)
{
    int bit,bk;
    int in = 0; //物が載っている場所を取得
    int com=0;  //組み合わせ成立箇所
    double sum=0; //組み合わせ候補箇所の重量を合算していく
    float target, res;
    if(mask == UP_HEAD) {
        res = val.target + val.jougen;    //組み合わせ重量の上限値を取得
        target =val.target;
    } else {
        res = val.taimenTarget + val.taimenJougen;    //組み合わせ重量の上限値を取得
        target =val.taimenTarget;
    }

    int kasan = 0;

    in = cel.getAnteiIn();  //計量物の数を取得
    if(in == 0) return com; //計量物ないなら即終了

    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)
    float *weight = new float[product.head];

    for(int i = 0; i < product.head; i++) {
        weight[i] = cel.anteiWeight(i);
    }

    in &= mask;
    if( mask == UP_HEAD) {
        in >>= 6;   //1-6ヘッドまで不要なのでシフト演算で削除
        kasan = 6;  //6-11ヘッド位置の重量を取得する
    }

    //計量物がある分だけ組み合わせ確認を行う
    for(int i=in; i; i--) {
        //組み合わせ候補に重量ゼロがあれば次の組み合わせ候補へ
        if( (in & i) != i) continue;

        //ビット1になっている箇所の重量を取得していく
        //bkはbitが立っている場所の重量値を取得するために使用
        //sumは取得する重量値の合計重量を保持
        sum = 0;
        bk = i;
        for(bit = 0; bk; bk >>= 1) {
            if (bk & 0x01) sum += weight[bit+kasan] - MINUS; //総重量取得
            bit++;
        }

        //一番最適な重量を取得する
        if((sum >= target) && (int(sum) < int(res))) {
            res = sum;          //resを更新する。これをしないと毎回読みに行く。
            com = i << kasan;   //一番最小の組み合わせを取得
        }
    }//loop終了

    delete[] weight;
    return com;
}

//計量皿に物が載っているか確認
int infoNow(int head, const int now)
{
    int res = now;
    cel.set_head(head);
    cel.setAinSpeed(param.KURI_Y);
    //i番目のbitが立っているか(重量があるか確認)
    if(now & oneBit[head]) {
        if(cel.checkOut())   res -= oneBit[head];    //重量があったが取られた
    } else {
        //if(head==4){
        //    if(cel.checkIn(head)){   res |= oneBit[head];    }  //重量無かったが置かれた
        //}else{
            if(cel.checkIn2(head,res)){   res |= oneBit[head];    }  //重量無かったが置かれた
        //}
    }
    /*printf("kumi=%2d\r\n",res);
    if(res==5){
        wait(3);   
    }*/
    return res;
}
int auto_span_ck()
{
    //           o     -   to   su   ha  maru  nn     si   ma   su   ka
    char ck1[]= {0xB5,0x2D,0xC4,0xBD,0xCA,0xDF,0xDD,' ',0xBC,0xCF,0xBD,0xB6,0x00};
    lcd.blank(0);
    lcd.locate(0,0);
    lcd.printf(ck1);
    lcd.locate(0,1);
    lcd.blank(1);
    if (span_auto_f == YES) {
        lcd.blank(1);
        lcd.printf("YES");
    } else {
        lcd.blank(1);
        lcd.printf("NO");
        span_auto_f = NO;
    }
    wait(1);
    btn.new_btn();              //スイッチ(New)取得
    while(btn.get_btn() != BTN_ENT) {
        if(btn.get_btn() == BTN_UP || btn.get_btn() == BTN_DOWN) {
            if (span_auto_f == YES) {
                lcd.blank(1);
                lcd.printf("NO");
                span_auto_f = NO;
            } else {
                lcd.blank(1);
                lcd.printf("YES");
                span_auto_f = YES;
            }
            se.Touch_01();
        } else if(btn.get_btn() == BTN_ESC) {
            btn.old_btn();   //スイッチ更新
            return 99;
        }
        btn.old_btn();   //スイッチ更新
        wait(0.05);
        btn.new_btn();              //スイッチ(New)取得
    }//while
    se.Touch_01();
    return span_auto_f;
}
int span_item_ck()
{
    char ck1[]= {0xC1,0xAE,0xB3,0xBE,0xB2,0xBD,0xD9,' ',0xBB,0xD7,0xA6,' ',0xBE,0xDD,0xC0,0xB8,0x00}; //ﾁｮｳｾｲｽﾙ ｻﾗｦ ｾﾝﾀｸ
    char ck2[]= {'N','o','.'};  //No.
    //char ck2[]={0xBB,0xD7,0xD2,0x00};   //ｻﾗﾒ
    file.read_stop_cells();
    lcd.blank(0);
    lcd.locate(0,0);
    lcd.printf(ck1);
    if(span_item_f > product.head) {
        span_item_f = 0;
    }
    lcd.locate(0,1);
    lcd.blank(1);
    if (span_item_f == 0) {
        lcd.blank(1);
        lcd.printf("ALL");
    } else {
        lcd.blank(1);
        lcd.locate(0,1);
        lcd.printf(ck2);
        lcd.printf("%2d",span_item_f);
    }
    wait(1);
    while(btn.any_press()) {wait(0.1);}
    btn.new_btn();              //スイッチ(New)取得
    while(btn.get_btn() != BTN_ENT) {
        if(btn.get_btn() == BTN_DOWN) {
            span_item_f += 1;
            if(gLevel==0 && (oneBit[span_item_f-1] & stop_cells) > 0) {
                span_item_f += 1;
            }
            if(span_item_f > product.head) {
                span_item_f = 0;
            }
            se.Touch_01();
        } else if(btn.get_btn() == BTN_UP) {
            if(span_item_f <= 0) {
                span_item_f = product.head;
            } else {
                span_item_f -= 1;
            }
            if(gLevel==0 && (oneBit[span_item_f-1] & stop_cells) > 0) {
                span_item_f -= 1;
            }
            se.Touch_01();
        } else if(btn.get_btn() == BTN_ESC) {
            btn.old_btn();   //スイッチ更新
            return 99;
        }
        //表示設定
        if (span_item_f == 0) {
            lcd.blank(1);
            lcd.printf("ALL");
        } else {
            lcd.blank(1);
            lcd.locate(0,1);
            lcd.printf(ck2);
            lcd.printf("%2d",span_item_f);
        }
        if (span_item_f>0) {
            led.REGIS(oneBit[span_item_f -1]);
        } else {
            if (gLevel==0) {
                led.REGIS((((oneBit[product.head-1])*2)-1)^stop_cells);
            } else {
                led.REGIS((((oneBit[product.head-1])*2)-1));
            }
        }
        btn.old_btn();   //スイッチ更新

        //LED消灯
        //wait(0.03);
        //led.REGIS(0);
        wait(0.03);
        btn.new_btn();              //スイッチ(New)取得
    }//while
    se.Touch_01();
    return span_item_f;
}

//各ロードセルのスパン値を取得
void set_span()
{
    
    char manual_mode[]= {0xBC,0xAD,0xC4,0xDE,0xB3,' ' ,0xD3,0xB0,0xC4,0xDE,0x00};   //ｼｭﾄﾞｳ ﾓｰﾄﾞ
    const char tests[]={0xC1,0xAE,0xB3,0xBE,0xB2,0xC1,0xAD,0xB3,0x00};      //ﾁｮｳｾｲﾁｭｳ
    span_t *changeAin = new span_t[product.head];
    float zero_voltage=0;
    int temp = bscData.temperature;
    //float fain_data,sain_data;
    int back_flg=0; //戻るときに2個前の値がおかしくなるのを防ぐフラグ
    int wait_next=0;
    //t defo_hundo = param.hundo;
    float w_ave = 0;
    int f_s_flag = 0;
    int LCD_mojisu = 16;
    int between_span_hundo = 0;
    float befour_ain = 0.0;
    bool one_head_endf = false;
    bool manual_mode_f = false;
    //*********起動後5分経つまでスパンを行わさせないプログラム******************
    //*********escを押し続けると回避可能*************************************
    const char par[]= {0x25,0x00};
    seconds = time(NULL);
    tm *dated = localtime(&seconds);
    
    file.read_span();                   //各スパン値
    cel.set_span(file.get_span());      //スパン値を取得
    file.read_span_hundo();

    cel.setIniHWeight();    //風袋込重量を強制取得
    
    if ((w_date.tm_min+5 <= dated->tm_min || w_date.tm_hour < dated->tm_hour || w_date.tm_mday < dated->tm_mday) ||
            (w_date.tm_min > dated->tm_min && ((w_date.tm_min+5)-(dated->tm_min+60))<1)) {
    } else {
        se.mistake();
        if (w_date.tm_min  - dated->tm_min >= 6) {
            v.output_span_wait((w_date.tm_min + 5) - (dated->tm_min + 60));
        } else {
            v.output_span_wait(w_date.tm_min + 5 - dated->tm_min);
        }
        btn.ini();                      //ボタン内容を初期化
        wait(1);
        btn.new_btn();              //スイッチ(New)取得
        //printf("A00Time as a string = %d/%d/%d/%d/%d/%d/\r\n",
        //    dated->tm_year + 1900,dated->tm_mon + 1,dated->tm_mday,
        //    dated->tm_hour,dated->tm_min,dated->tm_sec);
        //printf("A01Time as a string = %s\r\n", ctime(&seconds));
        //printf("%d\r\n", w_date.tm_min);
        //printf("%d\r\n", dated->tm_min);
        //printf("%d\r\n", w_date.tm_min+5 - dated->tm_min);

        if(btn.get_btn() != BTN_ESC) {
            delete[] changeAin;
            return;  //スパンキャンセル
        }
        btn.end_force();
        se.Touch_01();
        //while(btn.any_press()){wait(0.2);}
        while(btn.esc_press()){wait(0.2);}
            
            
        
        se.Touch_01();
        wait(0.5);
        //se.Touch_01();
        //se.Touch_01();
        //se.Touch_01();
    }
    //*******************************************************
    span_auto_f = YES;
    span_item_f = 0;
    if (gLevel==0) {                            //パスワード入力していない場合
        span_auto_f = YES;
        //span_item_f = 1;
    } else {
        stop_cells = 0;                         //未使用セル設定
        span_auto_f = auto_span_ck();           //オートスパンで行うかどうかの選択
        //span_auto_f =NO;                  //表示について意見がはいったので、ひとまず強制的にOFFにする

        if (span_auto_f==99) {
            delete[] changeAin;
            return;
        }
    }
    if(product.head>1){
        span_item_f = span_item_ck();
    }else{
        span_item_f = 1;
    }
    if (span_item_f==99) {
        delete[] changeAin;
        return;
    }


    wait(1);
    lcd.blank(0);
    //lcd.locate(0,0);
    //lcd.printf("                ");
    for(int i = 0; i < product.head; i++) {
        changeAin[i].fain=0;
        changeAin[i].sain=0;
        changeAin[i].sain2=0;
        changeAin[i].sain3=0;
        changeAin[i].sain4=0;
        changeAin[i].sain5=0;
        changeAin[i].sain6=0;
        changeAin[i].sain7=0;
        changeAin[i].sain8=0;
        changeAin[i].sain9=0;
        changeAin[i].sain10=0;
    }
    
    //se.Touch_02();
    btn.ini();                      //ボタン内容を初期化
    //wait(1.5);
    if(span_auto_f == NO){
        manual_mode_f = true;
    }

           
    //cel.setIniHWeight();    //風袋込重量を強制取得
    cel.set_ini(); 
    //全ヘッドのスパン調整
    for(int i = 0; i < product.head; i++) {
        printf("Z,%5d\r\n",i);
        if (gLevel==0 && ((oneBit[i] & stop_cells) > 0)) {                            //通常モードのスパン&不使用セルは無視(セーブのため、値は元データを取得する)
            cel.set_now_span_data(file.get_span(i));//指定位置のデータをset
            changeAin[i].fain   = cel.get_now_fain();
            changeAin[i].sain   = cel.get_now_sain(1);
            changeAin[i].sain2  = cel.get_now_sain(2);  if(0 > changeAin[i].sain2  || changeAin[i].sain2  > 1){changeAin[i].sain2  = changeAin[i].sain  + 0.005; }
            changeAin[i].sain3  = cel.get_now_sain(3);  if(0 > changeAin[i].sain3  || changeAin[i].sain3  > 1){changeAin[i].sain3  = changeAin[i].sain2 + 0.005; }
            changeAin[i].sain4  = cel.get_now_sain(4);  if(0 > changeAin[i].sain4  || changeAin[i].sain4  > 1){changeAin[i].sain4  = changeAin[i].sain3 + 0.005; }
            changeAin[i].sain5  = cel.get_now_sain(5);  if(0 > changeAin[i].sain5  || changeAin[i].sain5  > 1){changeAin[i].sain5  = changeAin[i].sain4 + 0.005; }
            changeAin[i].sain6  = cel.get_now_sain(6);  if(0 > changeAin[i].sain6  || changeAin[i].sain6  > 1){changeAin[i].sain6  = changeAin[i].sain5 + 0.005; }
            changeAin[i].sain7  = cel.get_now_sain(7);  if(0 > changeAin[i].sain7  || changeAin[i].sain7  > 1){changeAin[i].sain7  = changeAin[i].sain6 + 0.005; }
            changeAin[i].sain8  = cel.get_now_sain(8);  if(0 > changeAin[i].sain8  || changeAin[i].sain8  > 1){changeAin[i].sain8  = changeAin[i].sain7 + 0.005; }
            changeAin[i].sain9  = cel.get_now_sain(9);  if(0 > changeAin[i].sain9  || changeAin[i].sain9  > 1){changeAin[i].sain9  = changeAin[i].sain8 + 0.005; }
            changeAin[i].sain10 = cel.get_now_sain(10); if(0 > changeAin[i].sain10 || changeAin[i].sain10 > 1){changeAin[i].sain10 = changeAin[i].sain9 + 0.005; }

            
        } else {
            if( i < 0) {
                delete[] changeAin;
                return;  //スパンキャンセル
            }

            if(span_item_f>0) { //全体計量以外＝単体計量だったら
                i=span_item_f-1;
            }
            //printf("1 i %5d %5d  \r\n",i,param.hundo );

            //fain_data=0;
            //sain_data=0;
            led.ledAllOff();                // LED消灯
            btn.ini();                      //ボタン内容を初期化
            cel.set_now_span_data(file.get_span(i));//指定位置のデータをset
            //fain_data= cel.get_now_fain();
            //sain_data= cel.get_now_sain();
            //スパン値の設定中、決定を押してから戻ってきたとき、決定した値を保持する
            if((changeAin[i].fain >= 0.0001) && (changeAin[i].fain != cel.get_now_fain())) {
                cel.set_fainV(changeAin[i].fain);      //現在のロードセルのFAIN値を取得
            }
            if((changeAin[i].sain >= 0.0001) && (changeAin[i].sain != cel.get_now_sain(1))) {
                cel.set_sainV(changeAin[i].sain);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain2 >= 0.0001) && (changeAin[i].sain2 != cel.get_now_sain(2))) {
                cel.set_sainVs(changeAin[i].sain2,2);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain3 >= 0.0001) && (changeAin[i].sain3 != cel.get_now_sain(3))) {
                cel.set_sainVs(changeAin[i].sain3,3);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain4 >= 0.0001) && (changeAin[i].sain4 != cel.get_now_sain(4))) {
                cel.set_sainVs(changeAin[i].sain4,4);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain5 >= 0.0001) && (changeAin[i].sain5 != cel.get_now_sain(5))) {
                cel.set_sainVs(changeAin[i].sain5,5);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain6 >= 0.0001) && (changeAin[i].sain6 != cel.get_now_sain(6))) {
                cel.set_sainVs(changeAin[i].sain6,6);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain7 >= 0.0001) && (changeAin[i].sain7 != cel.get_now_sain(7))) {
                cel.set_sainVs(changeAin[i].sain7,7);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain8 >= 0.0001) && (changeAin[i].sain8 != cel.get_now_sain(8))) {
                cel.set_sainVs(changeAin[i].sain8,8);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain9 >= 0.0001) && (changeAin[i].sain9 != cel.get_now_sain(9))) {
                cel.set_sainVs(changeAin[i].sain9,9);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain10 >= 0.0001) && (changeAin[i].sain10 != cel.get_now_sain(10))) {
                cel.set_sainVs(changeAin[i].sain10,10);      //現在のロードセルのSAIN値を取得
            }


            float defo_span_ave_limit = 0.00005;
            float defo_span_vol_dif   = 0.00005;
            if(param.hundo<=90){
                defo_span_ave_limit = 0.0001;
                defo_span_vol_dif = 0.0001;
            }
            if(product.Rating==RATING_BIG){
                defo_span_ave_limit = 0.0001;
                defo_span_vol_dif = 0.0001;
            }
            int span_all_kuri=0;
            int span_ann_kuri=0;
            int span_mistake=0;
            float now_voltage=0;
            float old_voltage=0;
            float span_average=0;
            float span_ave_limit = defo_span_ave_limit;
            float span_vol_dif   = defo_span_vol_dif;
            f_s_flag = 0;
            int progress = 0;
            //セルの繰返し計測値を読み取り開始するプログラム**************************

            //******************************************************************
            
            
            
            btn.end_force();
            btn.new_btn();              //スイッチ(New)取得
            //Enterが押されるまで繰り返すone_head_endf
            //while(btn.get_btn() != BTN_ENT) {
            while(btn.any_press() == true){wait(0.2);}
            one_head_endf = false;
            while(one_head_endf ==false && f_s_flag <= sphun_count) {
                //printf("Y,%5d\r\n",f_s_flag);
                if(f_s_flag ==2  && (0 > changeAin[i].sain2  || changeAin[i].sain2  > 1)){changeAin[i].sain2  = changeAin[i].sain  + 0.005; }
                if(f_s_flag ==3  && (0 > changeAin[i].sain3  || changeAin[i].sain3  > 1)){changeAin[i].sain3  = changeAin[i].sain2 + 0.005; }
                if(f_s_flag ==4  && (0 > changeAin[i].sain4  || changeAin[i].sain4  > 1)){changeAin[i].sain4  = changeAin[i].sain3 + 0.005; }
                if(f_s_flag ==5  && (0 > changeAin[i].sain5  || changeAin[i].sain5  > 1)){changeAin[i].sain5  = changeAin[i].sain4 + 0.005; }
                if(f_s_flag ==6  && (0 > changeAin[i].sain6  || changeAin[i].sain6  > 1)){changeAin[i].sain6  = changeAin[i].sain5 + 0.005; }
                if(f_s_flag ==7  && (0 > changeAin[i].sain7  || changeAin[i].sain7  > 1)){changeAin[i].sain7  = changeAin[i].sain6 + 0.005; }
                if(f_s_flag ==8  && (0 > changeAin[i].sain8  || changeAin[i].sain8  > 1)){changeAin[i].sain8  = changeAin[i].sain7 + 0.005; }
                if(f_s_flag ==9  && (0 > changeAin[i].sain9  || changeAin[i].sain9  > 1)){changeAin[i].sain9  = changeAin[i].sain8 + 0.005; }
                if(f_s_flag ==10 && (0 > changeAin[i].sain10 || changeAin[i].sain10 > 1)){changeAin[i].sain10 = changeAin[i].sain9 + 0.005; }
                //wait(0.1);
                if(product.Rating==RATING_BIG){
                    setHeadVolt(i, param.KURI*2);    //電圧値を取得
                }else{
                    setHeadVolt(i, param.KURI);    //電圧値を取得
                }
                btn.new_btn();              //スイッチ(New)取得

                if(btn.get_btn() == BTN_UP) {
                    cel.set_fain();      //現在のロードセルのFAIN値を取得
                    //if( i == 0){
                    //温度センサーの値を取得する
                    //    temp = cel.get_temperature(TEMP_POSITION,10);    //電圧値を取得
                    //}
                    se.Touch_01();
                    span_ann_kuri=0;
                    manual_mode_f = true;
                    //f_s_flag=99;
                } else if(btn.get_btn() == BTN_DOWN) {
                    if(f_s_flag == 0 || f_s_flag == 1){
                        cel.set_sain();      //現在のロードセルのSAIN値を取得
                    }else if(f_s_flag == 2){
                        cel.set_sain_2();
                    }else if(f_s_flag == 3){
                        cel.set_sain_3();
                    }else if(f_s_flag == 4){
                        cel.set_sain_4();
                    }else if(f_s_flag == 5){
                        cel.set_sain_5();
                    }else if(f_s_flag == 6){
                        cel.set_sain_6();
                    }else if(f_s_flag == 7){
                        cel.set_sain_7();
                    }else if(f_s_flag == 8){
                        cel.set_sain_8();
                    }else if(f_s_flag == 9){
                        cel.set_sain_9();
                    }else if(f_s_flag == 10){
                        cel.set_sain_10();
                    }
                    se.Touch_01();
                    span_ann_kuri=0;
                    manual_mode_f = true;
                    //f_s_flag=99;
                } else if(btn.get_btn() == BTN_KEEP_ESC_UP) {
                    cel.plus_fain();     //FAIN値を強制的に加算していく
                } else if(btn.get_btn() == BTN_KEEP_ESC_DOWN) {
                    cel.minus_fain();    //FAIN値を強制的に減算していく
                } else if(btn.get_btn() == BTN_ESC) {
                    if(span_item_f>0) { //全体計量以外＝単体計量だったら
                        changeAin[i].fain  = cel.get_now_fain();
                        changeAin[i].sain  = cel.get_now_sain(1);
                        changeAin[i].sain2 = cel.get_now_sain(2);
                        changeAin[i].sain3 = cel.get_now_sain(3);
                        changeAin[i].sain4 = cel.get_now_sain(4);
                        changeAin[i].sain5 = cel.get_now_sain(5);
                        changeAin[i].sain6 = cel.get_now_sain(6);
                        changeAin[i].sain7 = cel.get_now_sain(7);
                        changeAin[i].sain8 = cel.get_now_sain(8);
                        changeAin[i].sain9 = cel.get_now_sain(9);
                        changeAin[i].sain10 = cel.get_now_sain(10);
                        file.read_stop_cells();             //未使用セル設定
                        set_stopcell();                     //不使用セル(＋12Headの対面点灯を組み合わせる)
                        i = -2; //終了させる
                        btn.old_btn();   //スイッチ更新
                        delete[] changeAin;
                        span_auto_f = YES;
                        span_item_f = 0;
                        return;  //スパンキャンセル
                    } else {
                        changeAin[i].fain = cel.get_now_fain();
                        changeAin[i].sain  = cel.get_now_sain(1);
                        changeAin[i].sain2 = cel.get_now_sain(2);
                        changeAin[i].sain3 = cel.get_now_sain(3);
                        changeAin[i].sain4 = cel.get_now_sain(4);
                        changeAin[i].sain5 = cel.get_now_sain(5);
                        changeAin[i].sain6 = cel.get_now_sain(6);
                        changeAin[i].sain7 = cel.get_now_sain(7);
                        changeAin[i].sain8 = cel.get_now_sain(8);
                        changeAin[i].sain9 = cel.get_now_sain(9);
                        changeAin[i].sain10 = cel.get_now_sain(10);
                        back_flg=1;
                        i -= 2; //一つ前に戻る
                        f_s_flag = 0;
                        
                        if( gLevel== 0 && (oneBit[i+1] & stop_cells) > 0) {
                            i -= 1; //一つ前に戻る
                            if( i < -1) {
                                span_auto_f = YES;
                                span_item_f = 0;
                                delete[] changeAin;
                                return;  //スパンキャンセル
                            }
                        }
                        btn.old_btn();   //スイッチ更新
                        break;
                    }
                } else if(btn.get_btn() == BTN_ESC) {
                    if(span_item_f>0) { //全体計量以外＝単体計量だったら
                        i = product.head; //最後まで飛ぶ
                    }
                } else if(btn.get_btn() == BTN_ENT) {
                    //for(int i = f_s_flag ; i<= sphun_count;i++){
                    //    cel.set_sainVs(0.0, i);
                    //}
                    
                    defo_span_ave_limit = 0.00003;
                    defo_span_vol_dif   = 0.00002;
                    //if(param.hundo<=90){
                    if(span_hundo[f_s_flag]<=90){
                        defo_span_ave_limit = 0.0001;
                        defo_span_vol_dif = 0.0001;
                    }
                    if(product.Rating==RATING_BIG){
                        //defo_span_ave_limit = 0.0001;
                        //defo_span_vol_dif = 0.0001;
                        defo_span_ave_limit = 0.02;
                        defo_span_vol_dif = 0.02;
                    }
                    progress = 0;
                    span_all_kuri=0;
                    span_ann_kuri=0;
                    span_mistake=0;
                    now_voltage=0;
                    old_voltage=0;
                    span_average=0;
                    span_ave_limit = defo_span_ave_limit;
                    span_vol_dif   = defo_span_vol_dif;
                    span_all_kuri=0;
                    
                    if(f_s_flag >= sphun_count){
                        
                        if(span_hundo[f_s_flag-1] <= 0){
                            f_s_flag = sphun_count+1;
                        }
                        f_s_flag = 1001;
                        one_head_endf = true;
                        btn.force_ENTER();
                    }else{
                        f_s_flag += 1;
                        if(span_hundo[f_s_flag-1] <= 0){
                            f_s_flag = sphun_count+1;
                        }
                    }
                    if(span_auto_f == YES){
                        manual_mode_f = false;
                    }
                    se.Touch_01();
                    while(btn.any_press() == true){wait(0.2);}
                    
                    continue;
                }

                //各取得値を表示
                if(one_head_endf ==false){
                    if (gLevel==0) {                            //パスワード入力していない場合
                        lcd.locate(0,0);
                        lcd.printf("                ");
                        lcd.locate(0,0);
                        if( product.productType == BIG || product.productType == BIG_D) {   //川
                            //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                            newdata.total=cel.get_now_weight_span(f_s_flag);
                            lcd.printf("[%2d]%6.1f g",i+1,newdata.total);    //川
                        } else {
                            lcd.printf("[%2d]%6.1f g",i+1,cel.get_now_weight_span(f_s_flag));
                        }
                        if(f_s_flag == 0){
                            lcd.locate(13,0);
                            lcd.printf("   ");
                        }else if(f_s_flag > 0 && param.hundo>0){
                            lcd.locate(13,0);
                            lcd.printf("%3d",span_hundo[f_s_flag-1]/10);
                        }
                        lcd.locate(0,1);
                        lcd.printf("                ");
                        lcd.locate(0,1);
                        if((f_s_flag >= 0 && f_s_flag <= sphun_count)) {    //通常モード、オートスパン時は矢印表示
                            if (int(float(span_ann_kuri)/(float(span_OK_kaisu)/float(LCD_mojisu))) > progress) {
                                progress=int(float(span_ann_kuri)/(float(span_OK_kaisu)/float(LCD_mojisu)));
                            }
                            //printf("%2d:%2d:%2d\r\n",span_ann_kuri,progress,int(span_ann_kuri/(span_OK_kaisu/LCD_mojisu)));
                            for(int j=0; j < LCD_mojisu; j++) {
                                if(progress - j > 0) {
                                    lcd.printf(">");
                                } else {
                                    lcd.printf(" ");
                                }
                            }
                            //wait(2);
                        } else {
                            if(manual_mode_f == true){
                                lcd.printf(manual_mode);
                            }
                        }
                    } else {
                        //lcd.cls();
                        param.hundo = span_hundo[f_s_flag-1];
                        if(f_s_flag > 1 && (param.hundo <= 0)){
                            for(int i = f_s_flag ; i<= sphun_count;i++){
                                cel.set_sainVs(0.0, i);
                            }
                            f_s_flag = 1002;
                            btn.force_ENTER();
                        }
                        
                        lcd.locate(0,0);
                        if(f_s_flag <= sphun_count){
                            if(product.Rating==RATING_BIG){
                                if(f_s_flag==0){
                                    lcd.printf("0=%4.0f %d=%4.0f",cel.get_now_fain()*10000,int(span_hundo[0]/100),cel.get_now_sain(1)*10000);
                                }else{
                                    lcd.printf("0=%4.0f %d=%4.0f",cel.get_now_fain()*10000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*10000);
                                }
                            }else{
                                if(f_s_flag==0){
                                    lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(span_hundo[0]/100),cel.get_now_sain(1)*1000);
                                }else{
                                    lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*1000);
                                }
                            }
                            if(f_s_flag == 0){
                                lcd.locate(13,0);
                                lcd.printf("   ");
                            }else if(f_s_flag > 0 && span_hundo[f_s_flag-1]>0){
                                lcd.locate(13,0);
                                lcd.printf("%3d",span_hundo[f_s_flag-1]/10);
                            }
                            
                            lcd.locate(0,1);
                            lcd.printf("                ");
                            lcd.locate(0,1);
                            if( product.productType == BIG || product.productType == BIG_D) {   //川
                                //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                                newdata.total=cel.get_now_weight_span(f_s_flag);
                                lcd.printf("[%2d]%6.1f g ",i+1,newdata.total);    //川
                            } else {
                                lcd.printf("[%2d]%6.1f g ",i+1,cel.get_now_weight_span(f_s_flag));
                            }
                        }
                    }   
                }
                //温度表示(1皿目のみ)
                //if( i == 0){
                //    lcd.locate(12,1);   //温度を表示する
                //    if(temp >= TEMP_MIN && (temp <= TEMP_MAX) ){
                //        lcd.printf(" %2d",temp);  //既定内の気温
                //    }else{
                //        lcd.printf("ERR");  //既定範囲外の気温
                //    }
                //}

                //スパン取得するロードセル位置のLED点灯
                if(product.matrix_mode == MATRIX_OFF){
                    led.REGIS(oneBit[i]);
                }else{
                    for(int k = 0; k < product.head; k++) {
                        if(i==k){
                            iro[k] = PENTA_R;
                        }else{
                            iro[k] = C_NUSE;
                        }
                    }
                }
                btn.old_btn();   //スイッチ更新

                //LED消灯
                wait(0.03);
                if(product.matrix_mode == MATRIX_OFF){
                    led.REGIS(0);
                }
                wait(0.03);
                if (span_auto_f == YES) {
                    param.hundo = span_hundo[f_s_flag-1];
                    //printf(" %d\r\n",param.hundo);
                    if(f_s_flag > 1 && (param.hundo <= 0)){
                        for(int i = f_s_flag ; i<= sphun_count;i++){
                            cel.set_sainVs(0.0, i);
                        }
                        f_s_flag = 1003;
                        btn.force_ENTER();
                        continue;
                    }
                    //*********************オートスパンプログラム********************************************************
                    if((f_s_flag >= 0 && f_s_flag <= sphun_count) && manual_mode_f == false) {
                        now_voltage=cel.getAin_now(i);
                        old_voltage=cel.getAin_old(i);
                        span_all_kuri += 1;
                        span_ann_kuri += 1;
                        if(span_all_kuri==1) {
                            if(f_s_flag == 0) {
                                if(i == 0) {
                                    if(product.matrix_mode == MATRIX_OFF){
                                        led.REGIS(oneBit[i]);
                                    }else{
                                        for(int k = 0; k < product.head; k++) {
                                            if(i==k){
                                                iro[k] = PENTA_R;
                                            }else{
                                                iro[k] = C_NUSE;
                                            }
                                        }
                                    }
                                    if( span_mistake >= 100) {
                                        span_mistake = 0;
                                    } else {
                                        v.output_span_next();
                                        //wait(2);
                                    }
                                    //LED消灯
                                    wait(0.03);
                                    if(product.matrix_mode == MATRIX_OFF){
                                        led.REGIS(0);
                                    }
                                    wait(0.2);
                                }
                                if(product.matrix_mode == MATRIX_OFF){
                                }else{
                                    for(int k = 0; k < product.head; k++) {
                                        if(i==k){
                                            iro[k] = PENTA_R;
                                        }else{
                                            iro[k] = C_NUSE;
                                        }
                                    }
                                }
                                if( i > 0 && i < product.head) {
                                    if(product.matrix_mode == MATRIX_OFF){
                                        led.REGIS(oneBit[i]);
                                    }
                                    if( span_mistake >= 100) {
                                        span_mistake = 0;
                                    } else {
                                        v.output_span_next();
                                        //wait(6);
                                    }
                                    //LED消灯
                                    wait(0.03);
                                    if(product.matrix_mode == MATRIX_OFF){
                                        led.REGIS(0);
                                    }
                                    wait(0.2);
                                }
                            }
                            if((f_s_flag >= 1 && f_s_flag <= sphun_count)) {
                                if(product.matrix_mode == MATRIX_OFF){
                                    led.REGIS(oneBit[i]);
                                }else{
                                    for(int k = 0; k < product.head; k++) {
                                        if(i==k){
                                            iro[k] = PENTA_R;
                                        }else{
                                            iro[k] = C_NUSE;
                                        }
                                    }
                                }
                                if( span_mistake >= 100) {
                                    span_mistake = 0;
                                } else {
                                    
                                    param.hundo = span_hundo[f_s_flag-1];
                                    if(param.hundo>0){
                                        //v.gram_voice(param.hundo);
                                        v.output_span_hundo(param.hundo);
                                        //wait(2);
                                    }
                                }
                                //LED消灯
                                wait(0.03);
                                if(product.matrix_mode == MATRIX_OFF){
                                    led.REGIS(0);
                                }
                                wait(0.2);
                            }
                        }
                        befour_ain = 0.0;
                        if(f_s_flag == 1){     befour_ain = cel.get_now_fain();}
                        else             {     befour_ain = cel.get_now_sain(f_s_flag-1);}
                        if(f_s_flag >= 1){
                            between_span_hundo = span_hundo[f_s_flag]-span_hundo[f_s_flag-1];
                        }
                        if(f_s_flag == 0 && span_mistake <= 20 ) {
                            span_ann_kuri = 0;
                            span_ann_kuri = 0;
                            span_average  = 0;
                            span_mistake += 1;
                            //以下の条件のどれかでも当てはまればNGとする
                        } else if((now_voltage >= span_average + span_ave_limit || now_voltage <= span_average - span_ave_limit) ||          //平均値より一定以上違ってたらNG
                                  (now_voltage >= old_voltage + span_vol_dif    || now_voltage <= old_voltage  - span_vol_dif) ||            //一個前の値より一定以上違ってたらNG
                                  //(f_s_flag == 1 &&(() cel.get_now_fain()+0.02 >= now_voltage)) ||
                                                                                                                                                                                                                                                                 
                                  ((f_s_flag >= 1 && f_s_flag <= sphun_count) &&((product.productType == JUNIOR     || product.productType == JUNIOR_D ) && befour_ain +0.002 >= now_voltage)) ||
                                  ((f_s_flag >= 1 && f_s_flag <= sphun_count) &&((product.productType == STANDARD   || product.productType == STANDARD_D ||
                                                     product.productType%100 == MATRIX || product.matrix_mode == MATRIX_ON   ||
                                                     product.productType == BIG        || product.productType == BIG_D ) &&
                                                     (
                                                      (between_span_hundo<= 90 &&                            befour_ain+0.0007 >= now_voltage)||
                                                      (between_span_hundo>  90 && between_span_hundo<=300 && befour_ain+0.0015 >= now_voltage)||
                                                      (between_span_hundo> 300 && between_span_hundo<=400 && befour_ain+0.005  >= now_voltage)||
                                                      (between_span_hundo>=400 &&                            befour_ain+0.005  >= now_voltage)

                                                        /*
                                                      (param.hundo<= 90 && befour_ain+0.0007 >= now_voltage)||
                                                      (param.hundo>  90 && param.hundo<=300 && befour_ain+0.0015 >= now_voltage)||
                                                      (param.hundo> 300 && param.hundo<=400 && befour_ain+0.005 >= now_voltage)||
                                                      (param.hundo>=400 &&                     befour_ain+0.005 >= now_voltage)
                                                      */
                                                      
                                                      
                                                      
                                                      )))
                                 ) {                                              //スパン2個目(ノーマルなら500g)の時、規定値(ひとまず20g相当)より上でなければNGとする

                            if(f_s_flag == 0 && span_mistake <= 55 ) {                                                                      //スパン1個目(0g)の時ミスが規定以下の場合は許容差を変更しない
                                span_ann_kuri = 0;
                                span_average  = 0;
                                span_mistake += 1;
                                //lcd.locate(15,1);
                                //lcd.printf("Y",i+1);
                                //printf("flag:%d |||now_vol:%f|||ave:%f\r\n",f_s_flag,now_voltage,span_average);                           //スパン1個目(0g)の時ミスが規定以上の場合は許容差を変更(緩める)
                            } else if(f_s_flag == 0 && span_mistake > 55) {
                                span_ann_kuri = 0;
                                span_average  = 0;
                                span_mistake += 1;
                                if(product.Rating==RATING_BIG){
                                        //span_ave_limit += 0.00006;
                                        //span_vol_dif   += 0.00006;
                                        //span_ave_limit += 0.0001;
                                        //span_vol_dif   += 0.0001;
                                        span_ave_limit += 0.02;
                                        span_vol_dif   += 0.02;
                                        
                                }else{
                                        //span_ave_limit += 0.00005;
                                        //span_vol_dif   += 0.00005;
                                        span_ave_limit += 0.05;
                                        span_vol_dif   += 0.05;
                                }
                                if (gLevel==0) {                            //パスワード入力していない場合
                                    //lcd.locate(10,0);
                                    //lcd.printf("NOIZE",i+1);
                                }
                                //lcd.locate(14,1);
                                //lcd.printf("YY",i+1);
                            } else if((f_s_flag >= 1 && f_s_flag <= sphun_count)  && span_mistake <= 45) {                                                                 //スパン2個目(500g)の時ミスが規定以下の場合は許容差を変更しない
                                span_ann_kuri = 0;
                                span_average = 0;
                                span_mistake += 1;
                                //lcd.locate(15,1);
                                //lcd.printf("X",i+1);
                            } else if((f_s_flag >= 1 && f_s_flag <= sphun_count)  && span_mistake > 45) {                                                                 //スパン2個目(500g)の時ミスが規定以上の場合は許容差を変更(緩める)
                                span_ann_kuri = 0;
                                span_average = 0;
                                span_mistake += 1;
                                if(product.Rating==RATING_BIG){
                                    //span_ave_limit += 0.00006;
                                    //span_vol_dif   += 0.00006;
                                    //span_ave_limit += 0.0001;
                                    //span_vol_dif   += 0.0001;
                                    span_ave_limit += 0.02;
                                    span_vol_dif   += 0.02;
                                }else{
                                    span_ave_limit += 0.00005;
                                    span_vol_dif   += 0.00005;
                                }
                                //lcd.locate(14,1);
                                //lcd.printf("XX",i+1);
                            }

                        } else if(f_s_flag >= 1 && f_s_flag <= sphun_count) {
                            //printf("1 %3d   %3d   %3d   %3d %4.5f\r\n",f_s_flag,span_mistake, span_ann_kuri,span_all_kuri,span_average );
                            //if(f_s_flag == 0 ){
                            //se.Touch_01();
                            //}else{
                            //se.Touch_02();
                            //}
                            if(span_ann_kuri > 1) {
                                span_average=(span_average*(span_ann_kuri-1)+cel.getAin_now(i))/span_ann_kuri;
                                //lcd.locate(15,1);
                                //lcd.printf(" ",i+1);
                            }
                        }
                        //printf("2 %3d   %3d   %3d   %3d %4.5f\r\n",f_s_flag,span_mistake, span_ann_kuri,span_all_kuri,span_average );
                        if (gLevel>0) {                            //パスワード入力している場合
                            if (int(float(span_ann_kuri)/float(span_OK_kaisu)*100) > progress) {
                                progress = int(float(span_ann_kuri)/float(span_OK_kaisu)*100);
                            }
                            lcd.locate(12,1);
                            if(f_s_flag >= 0 && f_s_flag <= sphun_count) {
                                //printf("%3d : %6.2f\r\n",progress,(float(span_ann_kuri)/float(span_OK_kaisu)*100));
                                lcd.printf("%3d%s",progress,par);
                                //lcd.printf("%3d",progress);
                            } else {
                                lcd.printf("    ");
                            }
                        }
                        if(span_ann_kuri>=span_OK_kaisu) {
                            if(f_s_flag==0) {
                                cel.set_fainV(span_average);
                                se.Touch_01();
                                //f_s_flag=1;
                            } else if(f_s_flag >= 1 && f_s_flag <= sphun_count) {
                                zero_voltage = cel.get_now_fain();
                                //(例：ノーマルTCの場合)実際の500gの値より0.4g程度高い値をスパン値として設定する。結果500gを載せると499.7位になる。
                                if( product.productType == STANDARD || product.productType == STANDARD_D || product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                                    if(param.hundo<=150){
                                        span_average = (((span_average - zero_voltage)/param.hundo)*(param.hundo + 0.2)) + zero_voltage;
                                    }else{
                                        span_average = (((span_average - zero_voltage)/param.hundo)*(param.hundo + 0.4)) + zero_voltage;
                                    }
                                } else if( product.productType == JUNIOR   || product.productType == JUNIOR_D  ) {
                                    span_average = (((span_average - zero_voltage)/param.hundo)*(param.hundo + 0.1)) + zero_voltage;
                                } else if( product.productType == BIG      || product.productType == BIG_D     ) {
                                    span_average = (((span_average - zero_voltage)/param.hundo)*(param.hundo + 2.5)) + zero_voltage;
                                }
                                cel.set_sainVs(span_average, f_s_flag);
                                if(gLevel != 1){
                                    se.Touch_01();
                                }
                                //wait(3);
                                //se.Touch_02();
                                if(gLevel == 1){//スパン値調整部。重量が特定の値にあわなければ、スパン値をちょっと削って
                                    w_ave=0;
                                    for(int w_lp=0;w_lp<5;w_lp++){
                                        setHeadVolt(i, param.KURI);    //電圧値を取得
                                        w_ave += cel.get_now_weight_span(f_s_flag);
                                    }
                                    w_ave = w_ave/5;
                                    if(param.hundo> 150){
                                        while((((param.hundo-0.2) < w_ave) || ( w_ave <  (param.hundo-0.4))) && btn.any_press()==false){
                                             if((param.hundo+1.3) < w_ave){
                                                span_average = span_average+((span_average-zero_voltage)/(param.hundo));
                                            }else if((param.hundo-0.2) < w_ave){
                                                span_average = span_average+((span_average-zero_voltage)/(param.hundo*10));
                                            }else if(w_ave < (param.hundo-1.5)){
                                                span_average = span_average-((span_average-zero_voltage)/(param.hundo));
                                            }else if(w_ave < (param.hundo-0.4)){
                                                span_average = span_average-((span_average-zero_voltage)/(param.hundo*10));
                                            }                                                  
                                            cel.set_sainVs(span_average, f_s_flag);
                                            w_ave=0;
                                            for(int w_lp=0;w_lp<5;w_lp++){
                                                setHeadVolt(i, param.KURI);    //電圧値を取得
                                                w_ave += cel.get_now_weight_span(f_s_flag);
                                                //printf("2:%6.1f g  %6.1f g\r\n",cel.get_now_weight_span(f_s_flag),w_ave);
                                            }
    
                                            w_ave = w_ave/5;
                                            lcd.locate(0,0);  
                                            lcd.blank(0);  
                                            lcd.printf(tests);
                                            lcd.locate(0,1);
                                            lcd.printf("                ");
                                            lcd.locate(0,1);
                                            if( product.productType == BIG || product.productType == BIG_D) {   //川
                                                //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                                                newdata.total=cel.get_now_weight_span(f_s_flag);
                                                lcd.printf("[%2d]%6.1f g",i+1,newdata.total);    //川
                                            } else {
                                                lcd.printf("[%2d]%6.1f g",i+1,cel.get_now_weight_span(f_s_flag));
                                            }
                                        }
                                    }
                                    else if(param.hundo<=150){
                                        
                                    //printf("2 %d\r\n",param.hundo);
                                        while((((param.hundo-0.1) < w_ave) || ( w_ave <  (param.hundo-0.2))) && btn.any_press()==false){
                                            if(      (param.hundo+1.3) < w_ave){
                                                span_average = span_average+((span_average-zero_voltage)/(param.hundo));
                                            }else if((param.hundo-0.0) < w_ave){
                                                span_average = span_average+((span_average-zero_voltage)/(param.hundo*10));
                                            }else if(w_ave < (param.hundo-1.5)){
                                                span_average = span_average-((span_average-zero_voltage)/(param.hundo));
                                            }else if(w_ave < (param.hundo-0.2)){
                                                span_average = span_average-((span_average-zero_voltage)/(param.hundo*10));
                                            }                                    
                                            cel.set_sainVs(span_average, f_s_flag);
                                            w_ave=0;
                                            for(int w_lp=0;w_lp<5;w_lp++){
                                                setHeadVolt(i, param.KURI);    //電圧値を取得
                                                w_ave += cel.get_now_weight_span(f_s_flag);
                                                //printf("2:%6.1f g  %6.1f g\r\n",cel.get_now_weight_span(f_s_flag),w_ave);
                                            }
    
                                            w_ave = w_ave/5;
                                            lcd.locate(0,0);  
                                            lcd.blank(0);  
                                            lcd.printf(tests);
                                            lcd.locate(0,1);
                                            lcd.printf("                ");
                                            lcd.locate(0,1);
                                            if( product.productType == BIG || product.productType == BIG_D) {   //川
                                                //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                                                newdata.total=cel.get_now_weight_span(f_s_flag);
                                                lcd.printf("[%2d]%6.1f g",i+1,newdata.total);    //川
                                            } else {
                                                lcd.printf("[%2d]%6.1f g",i+1,cel.get_now_weight_span(f_s_flag));
                                            }
                                        }
                                    }
                                    if(btn.any_press()==true){se.Touch_01();}
                                    while(btn.any_press()==true){
                                        wait(0.1);
                                    }                      
                                    
                                    lcd.locate(0,0);  
                                    
                                    if(product.Rating==RATING_BIG){
                                        if(f_s_flag==0){
                                            lcd.printf("0=%4.0f %d=%4.0f",cel.get_now_fain()*10000,int(param.hundo/100),cel.get_now_sain(1)*10000);
                                        }else{
                                            lcd.printf("0=%4.0f %d=%4.0f",cel.get_now_fain()*10000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*10000);
                                        }
                                    }else{
                                        if(f_s_flag==0){
                                            lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(param.hundo/100),cel.get_now_sain(1)*1000);
                                        }else{
                                            lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*1000);
                                        }
                                    }
                                }
                                se.Touch_01();
                            }
                            span_ann_kuri=0;
                            span_average=0;
                            span_all_kuri=0;
                            span_mistake = 0;
                            span_ave_limit = defo_span_ave_limit;
                            span_vol_dif   = defo_span_vol_dif;
                            progress = 0;
                            //変更されたスパン値を保存
                            changeAin[i].fain  = cel.get_now_fain();
                            changeAin[i].sain  = cel.get_now_sain(1);
                            changeAin[i].sain2 = cel.get_now_sain(2);
                            changeAin[i].sain3 = cel.get_now_sain(3);
                            changeAin[i].sain4 = cel.get_now_sain(4);
                            changeAin[i].sain5 = cel.get_now_sain(5);
                            changeAin[i].sain6 = cel.get_now_sain(6);
                            changeAin[i].sain7 = cel.get_now_sain(7);
                            changeAin[i].sain8 = cel.get_now_sain(8);
                            changeAin[i].sain9 = cel.get_now_sain(9);
                            changeAin[i].sain10 = cel.get_now_sain(10);
                            wait(0.1);
                            if(f_s_flag == 0 || f_s_flag == 0) {
                                if(gLevel==0) {
                                    wait_next=1;
                                } else {
                                    wait_next=span_OK_kaisu;
                                }
                            } else {
                                if(gLevel==0) {
                                    wait_next=1;
                                } else {
                                    wait_next=10;
                                }
                            }
                            for(int k=0; k < wait_next; k++) {
                                setHeadVolt(i, param.KURI);    //電圧値を取得

                                if (gLevel==0) {                            //パスワード入力していない場合
                                    lcd.locate(0,0);
                                    lcd.printf("                ");
                                    lcd.locate(0,0);
                                    //if( product.productType == BIG || product.productType == BIG_D){    //川
                                    //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                                    //    newdata.total=cel.get_now_weight_span(f_s_flag);
                                    //    lcd.printf("[%2d]%6.1f g",i+1,newdata.total);    //川
                                    //}else{
                                    //    lcd.printf("[%2d]%6.1f g",i+1,cel.get_now_weight_span(f_s_flag));
                                    //}
                                    lcd.locate(0,0);
                                    lcd.printf("       OK       ");
                                    lcd.locate(0,1);
                                    lcd.printf("                ");
                                } else {
                                    //lcd.cls();
                                    lcd.locate(0,0);
                                    
                                    if(product.Rating==RATING_BIG){
                                        if(f_s_flag==0){
                                            lcd.printf("0=%4.0f %d=%4.0f",cel.get_now_fain()*10000,int(span_hundo[0]/100),cel.get_now_sain(1)*10000);
                                        }else{
                                            lcd.printf("0=%4.f %d=%4.0f",cel.get_now_fain()*10000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*10000);
                                        }
                                    }else{
                                        if(f_s_flag==0){
                                            lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(span_hundo[0]/100),cel.get_now_sain(1)*1000);
                                        }else{
                                            lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*1000);
                                        }
                                    }
                                    lcd.locate(0,1);
                                    lcd.printf("                ");
                                    lcd.locate(0,1);
                                    if( product.productType == BIG || product.productType == BIG_D) {   //川
                                        //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                                        newdata.total=cel.get_now_weight_span(f_s_flag);
                                        lcd.printf("[%2d]%6.1f g",i+1,newdata.total);    //川
                                    } else {
                                        lcd.printf("[%2d]%6.1f g",i+1,cel.get_now_weight_span(f_s_flag));
                                    }
                                }
                                wait(0.1);
                            }
                            if(f_s_flag >= sphun_count){
                                printf("%6d : %6d\r\n",f_s_flag,sphun_count);
                                se.Touch_01();
                                se.Touch_01();
                                f_s_flag = 1004;
                                one_head_endf = true;
                                btn.force_ENTER();
                            }else{
                                printf("%6d : %6d\r\n",f_s_flag,sphun_count);
                                if(f_s_flag == 0 && manual_mode_f == true){
                                    f_s_flag = 2;
                                }else{
                                    f_s_flag += 1;
                                }
                            }
                            se.Touch_01();
                        }
                        if(span_ann_kuri <=1) {                                             //スパン値のｓ最初の値設定
                            span_average = now_voltage;
                        }
                        if(span_all_kuri >= 300 || span_mistake >= 130) {                   //オートスパン失敗
                            //span_mistake = 0;
                            span_mistake = 200;
                            v.output_span_ng();
                            //wait(4);
                            span_all_kuri=0;
                        }
                    }
                    //************************************************************************************************
                }

            }//while

            se.Touch_01();  //Enter押下した音
            if(back_flg==0) {
                //変更されたスパン値を保存
                changeAin[i].fain  = cel.get_now_fain();
                changeAin[i].sain  = cel.get_now_sain(1);
                changeAin[i].sain2 = cel.get_now_sain(2);
                changeAin[i].sain3 = cel.get_now_sain(3);
                changeAin[i].sain4 = cel.get_now_sain(4);
                changeAin[i].sain5 = cel.get_now_sain(5);
                changeAin[i].sain6 = cel.get_now_sain(6);
                changeAin[i].sain7 = cel.get_now_sain(7);
                changeAin[i].sain8 = cel.get_now_sain(8);
                changeAin[i].sain9 = cel.get_now_sain(9);
                changeAin[i].sain10 = cel.get_now_sain(10);
            } else {
                back_flg=0;
            }

            if(span_item_f>0) { //全体計量以外＝単体計量だったら
                i = product.head; //最後まで飛ぶ
            }
        }
    }//for
    //printf("%6d : %6d\r\n",f_s_flag,sphun_count);

    if(span_item_f>0) { //全体計量以外＝単体計量だったら
        file.set_span2(changeAin,span_item_f-1);
        file.save_span();                       //各スパン値を上書き保存
        file.read_span();                       //各スパン値
    } else {
        file.set_span(changeAin);
        file.save_span();                   //各スパン値を上書き保存
        file.read_span();                       //各スパン値
    }
    bscData.temperature = temp;         //変更した温度取得
    file.save_basic_setting(bscData);   //取得した温度を上書き保存
    cel.set_span(file.get_span());      //スパン値を取得
    file.read_stop_cells();             //未使用セル設定
    set_stopcell();                     //不使用セル(＋12Headの対面点灯を組み合わせる)
    span_auto_f = YES;
    span_item_f = 0;
    for(int i = 0; i < product.head; i++) {
        changeAin[i].fain =0;
        changeAin[i].sain =0;
    }
    delete[] changeAin;
}

//各ロードセルのスパン値を取得
void set_span_preset(int preset_head)
{
    
    char manual_mode[]= {0xBC,0xAD,0xC4,0xDE,0xB3,' ' ,0xD3,0xB0,0xC4,0xDE,0x00};   //ｼｭﾄﾞｳ ﾓｰﾄﾞ
    const char tests[]={0xC1,0xAE,0xB3,0xBE,0xB2,0xC1,0xAD,0xB3,0x00};      //ﾁｮｳｾｲﾁｭｳ
    span_t *changeAin = new span_t[product.head];
    data_t limitData;
    float zero_voltage=0;
    int temp = bscData.temperature;
    //float fain_data,sain_data;
    int back_flg=0; //戻るときに2個前の値がおかしくなるのを防ぐフラグ
    int wait_next=0;
    //t defo_hundo = param.hundo;
    float w_ave = 0;
    int f_s_flag = 0;
    int LCD_mojisu = 16;
    int between_span_hundo = 0;
    float befour_ain = 0.0;
    bool one_head_endf = false;
    bool manual_mode_f = false;
    const char set_juuryo[]={0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,0xBE,0xAF,0xC3,0xB2,0x00};      //ｼﾞｭｳﾘｮｳ ｾｯﾃｲ
    //int 
    //*********起動後5分経つまでスパンを行わさせないプログラム******************
    //*********escを押し続けると回避可能*************************************
    const char par[]= {0x25,0x00};
    //seconds = time(NULL);
    //*******************************************************
    span_auto_f = YES;
    span_item_f = 0;
    if (gLevel==0) {                            //パスワード入力していない場合
        span_auto_f = YES;
        //span_item_f = 1;
    } else {
        stop_cells = 0;                         //未使用セル設定
        span_auto_f = auto_span_ck();           //オートスパンで行うかどうかの選択
        //span_auto_f =NO;                  //表示について意見がはいったので、ひとまず強制的にOFFにする

        if (span_auto_f==99) {
            delete[] changeAin;
            return;
        }
    }
    /*
    if(product.head>1){
        span_item_f = 0;
    }else{
        span_item_f = 1;
    }
    if (span_item_f==99) {
        delete[] changeAin;
        return;
    }*/


    wait(1);
    lcd.blank(0);
    //lcd.locate(0,0);
    //lcd.printf("                ");
    for(int i = 0; i < product.head; i++) {
        changeAin[i].fain=0;
        changeAin[i].sain=0;
        changeAin[i].sain2=0;
        changeAin[i].sain3=0;
        changeAin[i].sain4=0;
        changeAin[i].sain5=0;
        changeAin[i].sain6=0;
        changeAin[i].sain7=0;
        changeAin[i].sain8=0;
        changeAin[i].sain9=0;
        changeAin[i].sain10=0;
    }
    
    //se.Touch_02();
    btn.ini();                      //ボタン内容を初期化
    //wait(1.5);
    if(span_auto_f == NO){
        manual_mode_f = true;
    }

    file.read_span_preset(preset_head);
    cel.set_span_preset();

/*
    limitData.min = 10;
    limitData.max = 1000;
    lcd.cls();
    lcd.printf(set_juuryo);
    lcd.locate(0,1);
    Chenge_vaule( &span_hundo_preset, limitData);
*/

    for(int i = -1; i < product.head; i++) {
        printf("Z,%5d\r\n",i);
        if( i < 0) {
            limitData.lcd = 1;
            limitData.min = 10;
            limitData.max = 3000;
            if(span_hundo_preset < limitData.min){
                span_hundo_preset = limitData.min;
            }
            if(span_hundo_preset > limitData.max){
                span_hundo_preset = limitData.max;
            }
            lcd.cls();
            lcd.printf(set_juuryo);
            lcd.locate(0,1);
            
            printf("B01\r\n");
            Chenge_vaule( &span_hundo_preset, limitData);
            printf("B02\r\n");

            if(btn.esc_press()==true){
                delete[] changeAin;
                return;  //スパンキャンセル
            }
            cel.set_span_preset();
            continue;
        }
        //cel.setIniHWeight();    //風袋込重量を強制取得
        cel.set_ini(); 
        //全ヘッドのスパン調整
        cel.set_fainV(span_preset[i][0]);      //現在のロードセルのFAIN値を取得
        cel.set_sainV(span_preset[i][1]);      //現在のロードセルのSAIN値を取得
        cel.set_sainVs(0.0           ,2);      //現在のロードセルのSAIN値を取得
        cel.set_sainVs(0.0           ,3);      //現在のロードセルのSAIN値を取得
        cel.set_sainVs(0.0           ,4);      //現在のロードセルのSAIN値を取得
        cel.set_sainVs(0.0           ,5);      //現在のロードセルのSAIN値を取得
        cel.set_sainVs(0.0           ,6);      //現在のロードセルのSAIN値を取得
        cel.set_sainVs(0.0           ,7);      //現在のロードセルのSAIN値を取得
        cel.set_sainVs(0.0           ,8);      //現在のロードセルのSAIN値を取得
        cel.set_sainVs(0.0           ,9);      //現在のロードセルのSAIN値を取得
        cel.set_sainVs(0.0           ,10);      //現在のロードセルのSAIN値を取得
        changeAin[i].fain   = cel.get_now_fain();
        changeAin[i].sain   = cel.get_now_sain(1);
        changeAin[i].sain2  = 0.0;
        changeAin[i].sain3  = 0.0;
        changeAin[i].sain4  = 0.0;
        changeAin[i].sain5  = 0.0;
        changeAin[i].sain6  = 0.0;
        changeAin[i].sain7  = 0.0;
        changeAin[i].sain8  = 0.0;
        changeAin[i].sain9  = 0.0;
        changeAin[i].sain10 = 0.0;
        if (gLevel==0 && ((oneBit[i] & stop_cells) > 0)) {                            //通常モードのスパン&不使用セルは無視(セーブのため、値は元データを取得する)
            cel.set_now_span_data(file.get_span(i));//指定位置のデータをset
            changeAin[i].fain   = cel.get_now_fain();
            changeAin[i].sain   = cel.get_now_sain(1);
            changeAin[i].sain2  = 0.0;
            changeAin[i].sain3  = 0.0;
            changeAin[i].sain4  = 0.0;
            changeAin[i].sain5  = 0.0;
            changeAin[i].sain6  = 0.0;
            changeAin[i].sain7  = 0.0;
            changeAin[i].sain8  = 0.0;
            changeAin[i].sain9  = 0.0;
            changeAin[i].sain10 = 0.0;
        } else {

            if(span_item_f>0) { //全体計量以外＝単体計量だったら
                i=span_item_f-1;
            }
            //printf("1 i %5d %5d  \r\n",i,param.hundo );

            //fain_data=0;
            //sain_data=0;
            led.ledAllOff();                // LED消灯
            btn.ini();                      //ボタン内容を初期化
            //cel.set_now_span_data(file.get_span(i));//指定位置のデータをset
            //fain_data= cel.get_now_fain();
            //sain_data= cel.get_now_sain();
            //スパン値の設定中、決定を押してから戻ってきたとき、決定した値を保持する
            if((changeAin[i].fain >= 0.0001) && (changeAin[i].fain != cel.get_now_fain())) {
                cel.set_fainV(changeAin[i].fain);      //現在のロードセルのFAIN値を取得
            }
            if((changeAin[i].sain >= 0.0001) && (changeAin[i].sain != cel.get_now_sain(1))) {
                cel.set_sainV(changeAin[i].sain);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain2 >= 0.0001) && (changeAin[i].sain2 != cel.get_now_sain(2))) {
                cel.set_sainVs(changeAin[i].sain2,2);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain3 >= 0.0001) && (changeAin[i].sain3 != cel.get_now_sain(3))) {
                cel.set_sainVs(changeAin[i].sain3,3);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain4 >= 0.0001) && (changeAin[i].sain4 != cel.get_now_sain(4))) {
                cel.set_sainVs(changeAin[i].sain4,4);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain5 >= 0.0001) && (changeAin[i].sain5 != cel.get_now_sain(5))) {
                cel.set_sainVs(changeAin[i].sain5,5);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain6 >= 0.0001) && (changeAin[i].sain6 != cel.get_now_sain(6))) {
                cel.set_sainVs(changeAin[i].sain6,6);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain7 >= 0.0001) && (changeAin[i].sain7 != cel.get_now_sain(7))) {
                cel.set_sainVs(changeAin[i].sain7,7);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain8 >= 0.0001) && (changeAin[i].sain8 != cel.get_now_sain(8))) {
                cel.set_sainVs(changeAin[i].sain8,8);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain9 >= 0.0001) && (changeAin[i].sain9 != cel.get_now_sain(9))) {
                cel.set_sainVs(changeAin[i].sain9,9);      //現在のロードセルのSAIN値を取得
            }
            if((changeAin[i].sain10 >= 0.0001) && (changeAin[i].sain10 != cel.get_now_sain(10))) {
                cel.set_sainVs(changeAin[i].sain10,10);      //現在のロードセルのSAIN値を取得
            }


            float defo_span_ave_limit = 0.00005;
            float defo_span_vol_dif   = 0.00005;
            if(param.hundo<=90){
                defo_span_ave_limit = 0.0001;
                defo_span_vol_dif = 0.0001;
            }
            if(product.Rating==RATING_BIG){
                defo_span_ave_limit = 0.0001;
                defo_span_vol_dif = 0.0001;
            }
            int span_all_kuri=0;
            int span_ann_kuri=0;
            int span_mistake=0;
            float now_voltage=0;
            float old_voltage=0;
            float span_average=0;
            float span_ave_limit = defo_span_ave_limit;
            float span_vol_dif   = defo_span_vol_dif;
            f_s_flag = 0;
            int progress = 0;
            //セルの繰返し計測値を読み取り開始するプログラム**************************

            //******************************************************************
            
            
            
            btn.end_force();
            btn.new_btn();              //スイッチ(New)取得
            //Enterが押されるまで繰り返すone_head_endf
            //while(btn.get_btn() != BTN_ENT) {
            while(btn.any_press() == true){wait(0.2);}
            one_head_endf = false;
            while(one_head_endf ==false && f_s_flag <= sphun_count) {
                //printf("Y,%5d\r\n",f_s_flag);
                if(f_s_flag ==2  && (0 > changeAin[i].sain2  || changeAin[i].sain2  > 1)){changeAin[i].sain2  = changeAin[i].sain  + 0.005; }
                if(f_s_flag ==3  && (0 > changeAin[i].sain3  || changeAin[i].sain3  > 1)){changeAin[i].sain3  = changeAin[i].sain2 + 0.005; }
                if(f_s_flag ==4  && (0 > changeAin[i].sain4  || changeAin[i].sain4  > 1)){changeAin[i].sain4  = changeAin[i].sain3 + 0.005; }
                if(f_s_flag ==5  && (0 > changeAin[i].sain5  || changeAin[i].sain5  > 1)){changeAin[i].sain5  = changeAin[i].sain4 + 0.005; }
                if(f_s_flag ==6  && (0 > changeAin[i].sain6  || changeAin[i].sain6  > 1)){changeAin[i].sain6  = changeAin[i].sain5 + 0.005; }
                if(f_s_flag ==7  && (0 > changeAin[i].sain7  || changeAin[i].sain7  > 1)){changeAin[i].sain7  = changeAin[i].sain6 + 0.005; }
                if(f_s_flag ==8  && (0 > changeAin[i].sain8  || changeAin[i].sain8  > 1)){changeAin[i].sain8  = changeAin[i].sain7 + 0.005; }
                if(f_s_flag ==9  && (0 > changeAin[i].sain9  || changeAin[i].sain9  > 1)){changeAin[i].sain9  = changeAin[i].sain8 + 0.005; }
                if(f_s_flag ==10 && (0 > changeAin[i].sain10 || changeAin[i].sain10 > 1)){changeAin[i].sain10 = changeAin[i].sain9 + 0.005; }
                //wait(0.1);
                if(product.Rating==RATING_BIG){
                    setHeadVolt(i, param.KURI*2);    //電圧値を取得
                }else{
                    setHeadVolt(i, param.KURI);    //電圧値を取得
                }
                btn.new_btn();              //スイッチ(New)取得

                if(btn.get_btn() == BTN_UP) {
                    cel.set_fain();      //現在のロードセルのFAIN値を取得
                    //if( i == 0){
                    //温度センサーの値を取得する
                    //    temp = cel.get_temperature(TEMP_POSITION,10);    //電圧値を取得
                    //}
                    se.Touch_01();
                    span_ann_kuri=0;
                    manual_mode_f = true;
                    //f_s_flag=99;
                } else if(btn.get_btn() == BTN_DOWN) {
                    if(f_s_flag == 0 || f_s_flag == 1){
                        cel.set_sain();      //現在のロードセルのSAIN値を取得
                    }else if(f_s_flag == 2){
                        cel.set_sain_2();
                    }else if(f_s_flag == 3){
                        cel.set_sain_3();
                    }else if(f_s_flag == 4){
                        cel.set_sain_4();
                    }else if(f_s_flag == 5){
                        cel.set_sain_5();
                    }else if(f_s_flag == 6){
                        cel.set_sain_6();
                    }else if(f_s_flag == 7){
                        cel.set_sain_7();
                    }else if(f_s_flag == 8){
                        cel.set_sain_8();
                    }else if(f_s_flag == 9){
                        cel.set_sain_9();
                    }else if(f_s_flag == 10){
                        cel.set_sain_10();
                    }
                    se.Touch_01();
                    span_ann_kuri=0;
                    manual_mode_f = true;
                    //f_s_flag=99;
                } else if(btn.get_btn() == BTN_KEEP_ESC_UP) {
                    cel.plus_fain();     //FAIN値を強制的に加算していく
                } else if(btn.get_btn() == BTN_KEEP_ESC_DOWN) {
                    cel.minus_fain();    //FAIN値を強制的に減算していく
                } else if(btn.get_btn() == BTN_ESC) {
                    if(span_item_f>0) { //全体計量以外＝単体計量だったら
                        changeAin[i].fain  = cel.get_now_fain();
                        changeAin[i].sain  = cel.get_now_sain(1);
                        changeAin[i].sain2 = cel.get_now_sain(2);
                        changeAin[i].sain3 = cel.get_now_sain(3);
                        changeAin[i].sain4 = cel.get_now_sain(4);
                        changeAin[i].sain5 = cel.get_now_sain(5);
                        changeAin[i].sain6 = cel.get_now_sain(6);
                        changeAin[i].sain7 = cel.get_now_sain(7);
                        changeAin[i].sain8 = cel.get_now_sain(8);
                        changeAin[i].sain9 = cel.get_now_sain(9);
                        changeAin[i].sain10 = cel.get_now_sain(10);
                        file.read_stop_cells();             //未使用セル設定
                        set_stopcell();                     //不使用セル(＋12Headの対面点灯を組み合わせる)
                        i = -2; //終了させる
                        btn.old_btn();   //スイッチ更新
                        delete[] changeAin;
                        span_auto_f = YES;
                        span_item_f = 0;
                        return;  //スパンキャンセル
                    } else {
                        changeAin[i].fain = cel.get_now_fain();
                        changeAin[i].sain  = cel.get_now_sain(1);
                        changeAin[i].sain2 = cel.get_now_sain(2);
                        changeAin[i].sain3 = cel.get_now_sain(3);
                        changeAin[i].sain4 = cel.get_now_sain(4);
                        changeAin[i].sain5 = cel.get_now_sain(5);
                        changeAin[i].sain6 = cel.get_now_sain(6);
                        changeAin[i].sain7 = cel.get_now_sain(7);
                        changeAin[i].sain8 = cel.get_now_sain(8);
                        changeAin[i].sain9 = cel.get_now_sain(9);
                        changeAin[i].sain10 = cel.get_now_sain(10);
                        back_flg=1;
                        i -= 2; //一つ前に戻る
                        f_s_flag = 0;
                        
                        if( gLevel== 0 && (oneBit[i+1] & stop_cells) > 0) {
                            i -= 1; //一つ前に戻る
                            if( i < -1) {
                                span_auto_f = YES;
                                span_item_f = 0;
                                delete[] changeAin;
                                return;  //スパンキャンセル
                            }
                        }
                        btn.old_btn();   //スイッチ更新
                        break;
                    }
                } else if(btn.get_btn() == BTN_ESC) {
                    if(span_item_f>0) { //全体計量以外＝単体計量だったら
                        i = product.head; //最後まで飛ぶ
                    }
                } else if(btn.get_btn() == BTN_ENT) {
                    //for(int i = f_s_flag ; i<= sphun_count;i++){
                    //    cel.set_sainVs(0.0, i);
                    //}
                    
                    defo_span_ave_limit = 0.00003;
                    defo_span_vol_dif   = 0.00002;
                    //if(param.hundo<=90){
                    if(span_hundo[f_s_flag]<=90){
                        defo_span_ave_limit = 0.0001;
                        defo_span_vol_dif = 0.0001;
                    }
                    if(product.Rating==RATING_BIG){
                        //defo_span_ave_limit = 0.0001;
                        //defo_span_vol_dif = 0.0001;
                        defo_span_ave_limit = 0.02;
                        defo_span_vol_dif = 0.02;
                    }
                    progress = 0;
                    span_all_kuri=0;
                    span_ann_kuri=0;
                    span_mistake=0;
                    now_voltage=0;
                    old_voltage=0;
                    span_average=0;
                    span_ave_limit = defo_span_ave_limit;
                    span_vol_dif   = defo_span_vol_dif;
                    span_all_kuri=0;
                    
                    if(f_s_flag >= sphun_count){
                        
                        if(span_hundo[f_s_flag-1] <= 0){
                            f_s_flag = sphun_count+1;
                        }
                        f_s_flag = 1001;
                        one_head_endf = true;
                        btn.force_ENTER();
                    }else{
                        f_s_flag += 1;
                        if(span_hundo[f_s_flag-1] <= 0){
                            f_s_flag = sphun_count+1;
                        }
                    }
                    if(span_auto_f == YES){
                        manual_mode_f = false;
                    }
                    se.Touch_01();
                    while(btn.any_press() == true){wait(0.2);}
                    
                    continue;
                }

                //各取得値を表示
                if(one_head_endf ==false){
                    if (gLevel==0) {                            //パスワード入力していない場合
                        lcd.locate(0,0);
                        lcd.printf("                ");
                        lcd.locate(0,0);
                        if( product.productType == BIG || product.productType == BIG_D) {   //川
                            //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                            newdata.total=cel.get_now_weight_span(f_s_flag);
                            lcd.printf("[%2d]%6.1f g",i+1,newdata.total);    //川
                        } else {
                            lcd.printf("[%2d]%6.1f g",i+1,cel.get_now_weight_span(f_s_flag));
                        }
                        if(f_s_flag == 0){
                            lcd.locate(13,0);
                            lcd.printf("   ");
                        }else if(f_s_flag > 0 && param.hundo>0){
                            lcd.locate(13,0);
                            lcd.printf("%3d",span_hundo[f_s_flag-1]/10);
                        }
                        lcd.locate(0,1);
                        lcd.printf("                ");
                        lcd.locate(0,1);
                        if((f_s_flag >= 0 && f_s_flag <= sphun_count)) {    //通常モード、オートスパン時は矢印表示
                            if (int(float(span_ann_kuri)/(float(span_OK_kaisu)/float(LCD_mojisu))) > progress) {
                                progress=int(float(span_ann_kuri)/(float(span_OK_kaisu)/float(LCD_mojisu)));
                            }
                            //printf("%2d:%2d:%2d\r\n",span_ann_kuri,progress,int(span_ann_kuri/(span_OK_kaisu/LCD_mojisu)));
                            for(int j=0; j < LCD_mojisu; j++) {
                                if(progress - j > 0) {
                                    lcd.printf(">");
                                } else {
                                    lcd.printf(" ");
                                }
                            }
                            //wait(2);
                        } else {
                            if(manual_mode_f == true){
                                lcd.printf(manual_mode);
                            }
                        }
                    } else {
                        //lcd.cls();
                        param.hundo = span_hundo[f_s_flag-1];
                        if(f_s_flag > 1 && (param.hundo <= 0)){
                            for(int i = f_s_flag ; i<= sphun_count;i++){
                                cel.set_sainVs(0.0, i);
                            }
                            f_s_flag = 1002;
                            btn.force_ENTER();
                        }
                        
                        lcd.locate(0,0);
                        if(f_s_flag <= sphun_count){
                            if(product.Rating==RATING_BIG){
                                if(f_s_flag==0){
                                    lcd.printf("0=%4.0f %d=%4.0f",cel.get_now_fain()*10000,int(span_hundo[0]/100),cel.get_now_sain(1)*10000);
                                }else{
                                    lcd.printf("0=%4.0f %d=%4.0f",cel.get_now_fain()*10000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*10000);
                                }
                            }else{
                                if(f_s_flag==0){
                                    lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(span_hundo[0]/100),cel.get_now_sain(1)*1000);
                                }else{
                                    lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*1000);
                                }
                            }
                            if(f_s_flag == 0){
                                lcd.locate(13,0);
                                lcd.printf("   ");
                            }else if(f_s_flag > 0 && span_hundo[f_s_flag-1]>0){
                                lcd.locate(13,0);
                                lcd.printf("%3d",span_hundo[f_s_flag-1]);
                            }
                            
                            lcd.locate(0,1);
                            lcd.printf("                ");
                            lcd.locate(0,1);
                            if( product.productType == BIG || product.productType == BIG_D) {   //川
                                //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                                newdata.total=cel.get_now_weight_span(f_s_flag);
                                lcd.printf("[%2d]%6.1f g ",i+1,newdata.total);    //川
                            } else {
                                lcd.printf("[%2d]%6.1f g ",i+1,cel.get_now_weight_span(f_s_flag));
                            }
                        }
                    }   
                }
                //温度表示(1皿目のみ)
                //if( i == 0){
                //    lcd.locate(12,1);   //温度を表示する
                //    if(temp >= TEMP_MIN && (temp <= TEMP_MAX) ){
                //        lcd.printf(" %2d",temp);  //既定内の気温
                //    }else{
                //        lcd.printf("ERR");  //既定範囲外の気温
                //    }
                //}

                //スパン取得するロードセル位置のLED点灯
                if(product.matrix_mode == MATRIX_OFF){
                    led.REGIS(oneBit[i]);
                }else{
                    for(int k = 0; k < product.head; k++) {
                        if(i==k){
                            iro[k] = PENTA_R;
                        }else{
                            iro[k] = C_NUSE;
                        }
                    }
                }
                btn.old_btn();   //スイッチ更新

                //LED消灯
                wait(0.03);
                if(product.matrix_mode == MATRIX_OFF){
                    led.REGIS(0);
                }
                wait(0.03);
                if (span_auto_f == YES) {
                    param.hundo = span_hundo[f_s_flag-1];
                    //printf(" %d\r\n",param.hundo);
                    if(f_s_flag > 1 && (param.hundo <= 0)){
                        for(int i = f_s_flag ; i<= sphun_count;i++){
                            cel.set_sainVs(0.0, i);
                        }
                        f_s_flag = 1003;
                        btn.force_ENTER();
                        continue;
                    }
                    //*********************オートスパンプログラム********************************************************
                    if((f_s_flag >= 0 && f_s_flag <= sphun_count) && manual_mode_f == false) {
                        now_voltage=cel.getAin_now(i);
                        old_voltage=cel.getAin_old(i);
                        span_all_kuri += 1;
                        span_ann_kuri += 1;
                        if(span_all_kuri==1) {
                            if(f_s_flag == 0) {
                                if(i == 0) {
                                    if(product.matrix_mode == MATRIX_OFF){
                                        led.REGIS(oneBit[i]);
                                    }else{
                                        for(int k = 0; k < product.head; k++) {
                                            if(i==k){
                                                iro[k] = PENTA_R;
                                            }else{
                                                iro[k] = C_NUSE;
                                            }
                                        }
                                    }
                                    if(product.matrix_mode == MATRIX_OFF){
                                        led.REGIS(oneBit[i]);
                                    }else{
                                    }
                                    if( span_mistake >= 100) {
                                        span_mistake = 0;
                                    } else {
                                        v.output_span_next();
                                        //wait(2);
                                    }
                                    //LED消灯
                                    wait(0.03);
                                    if(product.matrix_mode == MATRIX_OFF){
                                        led.REGIS(0);
                                    }
                                    wait(0.2);
                                }
                                if(product.matrix_mode == MATRIX_OFF){
                                }else{
                                    for(int k = 0; k < product.head; k++) {
                                        if(i==k){
                                            iro[k] = PENTA_R;
                                        }else{
                                            iro[k] = C_NUSE;
                                        }
                                    }
                                }
                                if( i > 0 && i < product.head) {
                                    //led.REGIS(oneBit[i]);
                                    if( span_mistake >= 100) {
                                        span_mistake = 0;
                                    } else {
                                        v.output_span_next();
                                        //wait(6);
                                    }
                                    //LED消灯
                                    wait(0.03);
                                    if(product.matrix_mode == MATRIX_OFF){
                                        led.REGIS(0);
                                    }
                                    wait(0.2);
                                }
                            }
                            if((f_s_flag >= 1 && f_s_flag <= sphun_count)) {
                                if(product.matrix_mode == MATRIX_OFF){
                                    led.REGIS(oneBit[i]);
                                }else{
                                    for(int k = 0; k < product.head; k++) {
                                        if(i==k){
                                            iro[k] = PENTA_R;
                                        }else{
                                            iro[k] = C_NUSE;
                                        }
                                    }
                                }
                                if( span_mistake >= 100) {
                                    span_mistake = 0;
                                } else {
                                    
                                    param.hundo = span_hundo[f_s_flag-1];
                                    if(param.hundo>0){
                                        //v.gram_voice(param.hundo);
                                        v.output_span_mononose();
                                        //v.output_span_hundo(param.hundo);
                                        //wait(2);
                                    }
                                }
                                //LED消灯
                                wait(0.03);
                                if(product.matrix_mode == MATRIX_OFF){
                                    led.REGIS(0);
                                }
                                wait(0.2);
                            }
                        }
                        befour_ain = 0.0;
                        if(f_s_flag == 1){     befour_ain = cel.get_now_fain();}
                        else             {     befour_ain = cel.get_now_sain(f_s_flag-1);}
                        if(f_s_flag >= 1){
                            between_span_hundo = span_hundo[f_s_flag]-span_hundo[f_s_flag-1];
                        }
                        if(f_s_flag == 0 && span_mistake <= 20 ) {
                            span_ann_kuri = 0;
                            span_ann_kuri = 0;
                            span_average  = 0;
                            span_mistake += 1;
                            //以下の条件のどれかでも当てはまればNGとする
                        } else if((now_voltage >= span_average + span_ave_limit || now_voltage <= span_average - span_ave_limit) ||          //平均値より一定以上違ってたらNG
                                  (now_voltage >= old_voltage + span_vol_dif    || now_voltage <= old_voltage  - span_vol_dif) ||            //一個前の値より一定以上違ってたらNG
                                  //(f_s_flag == 1 &&(() cel.get_now_fain()+0.02 >= now_voltage)) ||
                                                                                                                                                                                                                                                                 
                                  ((f_s_flag >= 1 && f_s_flag <= sphun_count) &&((product.productType == JUNIOR     || product.productType == JUNIOR_D ) && befour_ain +0.002 >= now_voltage)) ||
                                  ((f_s_flag >= 1 && f_s_flag <= sphun_count) &&((product.productType == STANDARD   || product.productType == STANDARD_D ||
                                                     product.productType%100 == MATRIX || product.matrix_mode == MATRIX_ON   ||
                                                     product.productType == BIG        || product.productType == BIG_D ) &&
                                                     (
                                                      (between_span_hundo<= 90 &&                            befour_ain+0.0007 >= now_voltage)||
                                                      (between_span_hundo>  90 && between_span_hundo<=300 && befour_ain+0.0015 >= now_voltage)||
                                                      (between_span_hundo> 300 && between_span_hundo<=400 && befour_ain+0.005  >= now_voltage)||
                                                      (between_span_hundo>=400 &&                            befour_ain+0.005  >= now_voltage)

                                                        /*
                                                      (param.hundo<= 90 && befour_ain+0.0007 >= now_voltage)||
                                                      (param.hundo>  90 && param.hundo<=300 && befour_ain+0.0015 >= now_voltage)||
                                                      (param.hundo> 300 && param.hundo<=400 && befour_ain+0.005 >= now_voltage)||
                                                      (param.hundo>=400 &&                     befour_ain+0.005 >= now_voltage)
                                                      */
                                                      
                                                      
                                                      
                                                      )))
                                 ) {                                              //スパン2個目(ノーマルなら500g)の時、規定値(ひとまず20g相当)より上でなければNGとする

                            if(f_s_flag == 0 && span_mistake <= 55 ) {                                                                      //スパン1個目(0g)の時ミスが規定以下の場合は許容差を変更しない
                                span_ann_kuri = 0;
                                span_average  = 0;
                                span_mistake += 1;
                                //lcd.locate(15,1);
                                //lcd.printf("Y",i+1);
                                //printf("flag:%d |||now_vol:%f|||ave:%f\r\n",f_s_flag,now_voltage,span_average);                           //スパン1個目(0g)の時ミスが規定以上の場合は許容差を変更(緩める)
                            } else if(f_s_flag == 0 && span_mistake > 55) {
                                span_ann_kuri = 0;
                                span_average  = 0;
                                span_mistake += 1;
                                if(product.Rating==RATING_BIG){
                                        //span_ave_limit += 0.00006;
                                        //span_vol_dif   += 0.00006;
                                        //span_ave_limit += 0.0001;
                                        //span_vol_dif   += 0.0001;
                                        span_ave_limit += 0.02;
                                        span_vol_dif   += 0.02;
                                        
                                }else{
                                        //span_ave_limit += 0.00005;
                                        //span_vol_dif   += 0.00005;
                                        span_ave_limit += 0.05;
                                        span_vol_dif   += 0.05;
                                }
                                if (gLevel==0) {                            //パスワード入力していない場合
                                    //lcd.locate(10,0);
                                    //lcd.printf("NOIZE",i+1);
                                }
                                //lcd.locate(14,1);
                                //lcd.printf("YY",i+1);
                            } else if((f_s_flag >= 1 && f_s_flag <= sphun_count)  && span_mistake <= 45) {                                                                 //スパン2個目(500g)の時ミスが規定以下の場合は許容差を変更しない
                                span_ann_kuri = 0;
                                span_average = 0;
                                span_mistake += 1;
                                //lcd.locate(15,1);
                                //lcd.printf("X",i+1);
                            } else if((f_s_flag >= 1 && f_s_flag <= sphun_count)  && span_mistake > 45) {                                                                 //スパン2個目(500g)の時ミスが規定以上の場合は許容差を変更(緩める)
                                span_ann_kuri = 0;
                                span_average = 0;
                                span_mistake += 1;
                                if(product.Rating==RATING_BIG){
                                    //span_ave_limit += 0.00006;
                                    //span_vol_dif   += 0.00006;
                                    //span_ave_limit += 0.0001;
                                    //span_vol_dif   += 0.0001;
                                    span_ave_limit += 0.02;
                                    span_vol_dif   += 0.02;
                                }else{
                                    span_ave_limit += 0.00005;
                                    span_vol_dif   += 0.00005;
                                }
                                //lcd.locate(14,1);
                                //lcd.printf("XX",i+1);
                            }

                        } else if(f_s_flag >= 1 && f_s_flag <= sphun_count) {
                            //printf("1 %3d   %3d   %3d   %3d %4.5f\r\n",f_s_flag,span_mistake, span_ann_kuri,span_all_kuri,span_average );
                            //if(f_s_flag == 0 ){
                            //se.Touch_01();
                            //}else{
                            //se.Touch_02();
                            //}
                            if(span_ann_kuri > 1) {
                                span_average=(span_average*(span_ann_kuri-1)+cel.getAin_now(i))/span_ann_kuri;
                                //lcd.locate(15,1);
                                //lcd.printf(" ",i+1);
                            }
                        }
                        //printf("2 %3d   %3d   %3d   %3d %4.5f\r\n",f_s_flag,span_mistake, span_ann_kuri,span_all_kuri,span_average );
                        if (gLevel>0) {                            //パスワード入力している場合
                            if (int(float(span_ann_kuri)/float(span_OK_kaisu)*100) > progress) {
                                progress = int(float(span_ann_kuri)/float(span_OK_kaisu)*100);
                            }
                            lcd.locate(12,1);
                            if(f_s_flag >= 0 && f_s_flag <= sphun_count) {
                                //printf("%3d : %6.2f\r\n",progress,(float(span_ann_kuri)/float(span_OK_kaisu)*100));
                                lcd.printf("%3d%s",progress,par);
                                //lcd.printf("%3d",progress);
                            } else {
                                lcd.printf("    ");
                            }
                        }
                        if(span_ann_kuri>=span_OK_kaisu) {
                            if(f_s_flag==0) {
                                cel.set_fainV(span_average);
                                se.Touch_01();
                                //f_s_flag=1;
                            } else if(f_s_flag >= 1 && f_s_flag <= sphun_count) {
                                zero_voltage = cel.get_now_fain();
                                //(例：ノーマルTCの場合)実際の500gの値より0.4g程度高い値をスパン値として設定する。結果500gを載せると499.7位になる。
                                if( product.productType == STANDARD || product.productType == STANDARD_D || product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                                    if(param.hundo<=150){
                                        span_average = (((span_average - zero_voltage)/param.hundo)*(param.hundo + 0.2)) + zero_voltage;
                                    }else{
                                        span_average = (((span_average - zero_voltage)/param.hundo)*(param.hundo + 0.4)) + zero_voltage;
                                    }
                                } else if( product.productType == JUNIOR   || product.productType == JUNIOR_D  ) {
                                    span_average = (((span_average - zero_voltage)/param.hundo)*(param.hundo + 0.1)) + zero_voltage;
                                } else if( product.productType == BIG      || product.productType == BIG_D     ) {
                                    span_average = (((span_average - zero_voltage)/param.hundo)*(param.hundo + 2.5)) + zero_voltage;
                                }
                                cel.set_sainVs(span_average, f_s_flag);
                                if(gLevel != 1){
                                    se.Touch_01();
                                }
                                //wait(3);
                                //se.Touch_02();
                                if(gLevel == 1){//スパン値調整部。重量が特定の値にあわなければ、スパン値をちょっと削って
                                    w_ave=0;
                                    for(int w_lp=0;w_lp<5;w_lp++){
                                        setHeadVolt(i, param.KURI);    //電圧値を取得
                                        w_ave += cel.get_now_weight_span(f_s_flag);
                                    }
                                    w_ave = w_ave/5;
                                    if(param.hundo> 150){
                                        while((((param.hundo-0.2) < w_ave) || ( w_ave <  (param.hundo-0.4))) && btn.any_press()==false){
                                             if((param.hundo+1.3) < w_ave){
                                                span_average = span_average+((span_average-zero_voltage)/(param.hundo));
                                            }else if((param.hundo-0.2) < w_ave){
                                                span_average = span_average+((span_average-zero_voltage)/(param.hundo*10));
                                            }else if(w_ave < (param.hundo-1.5)){
                                                span_average = span_average-((span_average-zero_voltage)/(param.hundo));
                                            }else if(w_ave < (param.hundo-0.4)){
                                                span_average = span_average-((span_average-zero_voltage)/(param.hundo*10));
                                            }                                                  
                                            cel.set_sainVs(span_average, f_s_flag);
                                            w_ave=0;
                                            for(int w_lp=0;w_lp<5;w_lp++){
                                                setHeadVolt(i, param.KURI);    //電圧値を取得
                                                w_ave += cel.get_now_weight_span(f_s_flag);
                                                //printf("2:%6.1f g  %6.1f g\r\n",cel.get_now_weight_span(f_s_flag),w_ave);
                                            }
    
                                            w_ave = w_ave/5;
                                            lcd.locate(0,0);  
                                            lcd.blank(0);  
                                            lcd.printf(tests);
                                            lcd.locate(0,1);
                                            lcd.printf("                ");
                                            lcd.locate(0,1);
                                            if( product.productType == BIG || product.productType == BIG_D) {   //川
                                                //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                                                newdata.total=cel.get_now_weight_span(f_s_flag);
                                                lcd.printf("[%2d]%6.1f g",i+1,newdata.total);    //川
                                            } else {
                                                lcd.printf("[%2d]%6.1f g",i+1,cel.get_now_weight_span(f_s_flag));
                                            }
                                        }
                                    }
                                    else if(param.hundo<=150){
                                        
                                    //printf("2 %d\r\n",param.hundo);
                                        while((((param.hundo-0.1) < w_ave) || ( w_ave <  (param.hundo-0.2))) && btn.any_press()==false){
                                            if(      (param.hundo+1.3) < w_ave){
                                                span_average = span_average+((span_average-zero_voltage)/(param.hundo));
                                            }else if((param.hundo-0.0) < w_ave){
                                                span_average = span_average+((span_average-zero_voltage)/(param.hundo*10));
                                            }else if(w_ave < (param.hundo-1.5)){
                                                span_average = span_average-((span_average-zero_voltage)/(param.hundo));
                                            }else if(w_ave < (param.hundo-0.2)){
                                                span_average = span_average-((span_average-zero_voltage)/(param.hundo*10));
                                            }                                    
                                            cel.set_sainVs(span_average, f_s_flag);
                                            w_ave=0;
                                            for(int w_lp=0;w_lp<5;w_lp++){
                                                setHeadVolt(i, param.KURI);    //電圧値を取得
                                                w_ave += cel.get_now_weight_span(f_s_flag);
                                                //printf("2:%6.1f g  %6.1f g\r\n",cel.get_now_weight_span(f_s_flag),w_ave);
                                            }
    
                                            w_ave = w_ave/5;
                                            lcd.locate(0,0);  
                                            lcd.blank(0);  
                                            lcd.printf(tests);
                                            lcd.locate(0,1);
                                            lcd.printf("                ");
                                            lcd.locate(0,1);
                                            if( product.productType == BIG || product.productType == BIG_D) {   //川
                                                //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                                                newdata.total=cel.get_now_weight_span(f_s_flag);
                                                lcd.printf("[%2d]%6.1f g",i+1,newdata.total);    //川
                                            } else {
                                                lcd.printf("[%2d]%6.1f g",i+1,cel.get_now_weight_span(f_s_flag));
                                            }
                                        }
                                    }
                                    if(btn.any_press()==true){se.Touch_01();}
                                    while(btn.any_press()==true){
                                        wait(0.1);
                                    }                      
                                    
                                    lcd.locate(0,0);  
                                    
                                    if(product.Rating==RATING_BIG){
                                        if(f_s_flag==0){
                                            lcd.printf("0=%4.0f %d=%4.0f",cel.get_now_fain()*10000,int(param.hundo/100),cel.get_now_sain(1)*10000);
                                        }else{
                                            lcd.printf("0=%4.0f %d=%4.0f",cel.get_now_fain()*10000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*10000);
                                        }
                                    }else{
                                        if(f_s_flag==0){
                                            lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(param.hundo/100),cel.get_now_sain(1)*1000);
                                        }else{
                                            lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*1000);
                                        }
                                    }
                                }
                                se.Touch_01();
                            }
                            span_ann_kuri=0;
                            span_average=0;
                            span_all_kuri=0;
                            span_mistake = 0;
                            span_ave_limit = defo_span_ave_limit;
                            span_vol_dif   = defo_span_vol_dif;
                            progress = 0;
                            //変更されたスパン値を保存
                            changeAin[i].fain  = cel.get_now_fain();
                            changeAin[i].sain  = cel.get_now_sain(1);
                            changeAin[i].sain2 = cel.get_now_sain(2);
                            changeAin[i].sain3 = cel.get_now_sain(3);
                            changeAin[i].sain4 = cel.get_now_sain(4);
                            changeAin[i].sain5 = cel.get_now_sain(5);
                            changeAin[i].sain6 = cel.get_now_sain(6);
                            changeAin[i].sain7 = cel.get_now_sain(7);
                            changeAin[i].sain8 = cel.get_now_sain(8);
                            changeAin[i].sain9 = cel.get_now_sain(9);
                            changeAin[i].sain10 = cel.get_now_sain(10);
                            wait(0.1);
                            if(f_s_flag == 0 || f_s_flag == 0) {
                                if(gLevel==0) {
                                    wait_next=1;
                                } else {
                                    wait_next=span_OK_kaisu;
                                }
                            } else {
                                if(gLevel==0) {
                                    wait_next=1;
                                } else {
                                    wait_next=10;
                                }
                            }
                            for(int k=0; k < wait_next; k++) {
                                setHeadVolt(i, param.KURI);    //電圧値を取得

                                if (gLevel==0) {                            //パスワード入力していない場合
                                    lcd.locate(0,0);
                                    lcd.printf("                ");
                                    lcd.locate(0,0);
                                    //if( product.productType == BIG || product.productType == BIG_D){    //川
                                    //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                                    //    newdata.total=cel.get_now_weight_span(f_s_flag);
                                    //    lcd.printf("[%2d]%6.1f g",i+1,newdata.total);    //川
                                    //}else{
                                    //    lcd.printf("[%2d]%6.1f g",i+1,cel.get_now_weight_span(f_s_flag));
                                    //}
                                    lcd.locate(0,0);
                                    lcd.printf("       OK       ");
                                    lcd.locate(0,1);
                                    lcd.printf("                ");
                                } else {
                                    //lcd.cls();
                                    lcd.locate(0,0);
                                    
                                    if(product.Rating==RATING_BIG){
                                        if(f_s_flag==0){
                                            lcd.printf("0=%4.0f %d=%4.0f",cel.get_now_fain()*10000,int(span_hundo[0]/100),cel.get_now_sain(1)*10000);
                                        }else{
                                            lcd.printf("0=%4.f %d=%4.0f",cel.get_now_fain()*10000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*10000);
                                        }
                                    }else{
                                        if(f_s_flag==0){
                                            lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(span_hundo[0]/100),cel.get_now_sain(1)*1000);
                                        }else{
                                            lcd.printf("0=%3.0f %d=%3.0f",cel.get_now_fain()*1000,int(param.hundo/100),cel.get_now_sain(f_s_flag)*1000);
                                        }
                                    }
                                    lcd.locate(0,1);
                                    lcd.printf("                ");
                                    lcd.locate(0,1);
                                    if( product.productType == BIG || product.productType == BIG_D) {   //川
                                        //newdata.total=int((cel.get_now_weight_span(f_s_flag)/5))*5;    //川
                                        newdata.total=cel.get_now_weight_span(f_s_flag);
                                        lcd.printf("[%2d]%6.1f g",i+1,newdata.total);    //川
                                    } else {
                                        lcd.printf("[%2d]%6.1f g",i+1,cel.get_now_weight_span(f_s_flag));
                                    }
                                }
                                wait(0.1);
                            }
                            if(f_s_flag >= sphun_count){
                                printf("%6d : %6d\r\n",f_s_flag,sphun_count);
                                se.Touch_01();
                                se.Touch_01();
                                f_s_flag = 1004;
                                one_head_endf = true;
                                btn.force_ENTER();
                            }else{
                                printf("%6d : %6d\r\n",f_s_flag,sphun_count);
                                if(f_s_flag == 0 && manual_mode_f == true){
                                    f_s_flag = 2;
                                }else{
                                    f_s_flag += 1;
                                }
                            }
                            se.Touch_01();
                        }
                        if(span_ann_kuri <=1) {                                             //スパン値のｓ最初の値設定
                            span_average = now_voltage;
                        }
                        if(span_all_kuri >= 300 || span_mistake >= 130) {                   //オートスパン失敗
                            //span_mistake = 0;
                            span_mistake = 200;
                            v.output_span_ng();
                            //wait(4);
                            span_all_kuri=0;
                        }
                    }
                    //************************************************************************************************
                }

            }//while

            se.Touch_01();  //Enter押下した音
            if(back_flg==0) {
                //変更されたスパン値を保存
                changeAin[i].fain  = cel.get_now_fain();
                changeAin[i].sain  = cel.get_now_sain(1);
                changeAin[i].sain2 = cel.get_now_sain(2);
                changeAin[i].sain3 = cel.get_now_sain(3);
                changeAin[i].sain4 = cel.get_now_sain(4);
                changeAin[i].sain5 = cel.get_now_sain(5);
                changeAin[i].sain6 = cel.get_now_sain(6);
                changeAin[i].sain7 = cel.get_now_sain(7);
                changeAin[i].sain8 = cel.get_now_sain(8);
                changeAin[i].sain9 = cel.get_now_sain(9);
                changeAin[i].sain10 = cel.get_now_sain(10);
                span_preset[i][0] = changeAin[i].fain;
                span_preset[i][1] = changeAin[i].sain;
            } else {
                back_flg=0;
            }

            if(span_item_f>0) { //全体計量以外＝単体計量だったら
                i = product.head; //最後まで飛ぶ
            }
        }
    }//for
    //printf("%6d : %6d\r\n",f_s_flag,sphun_count);

    if(span_item_f>0) { //全体計量以外＝単体計量だったら
        //file.set_span2(changeAin,span_item_f-1);
        file.save_span_preset(file.get_preset());     //各スパン値を上書き保存
        file.read_span_preset(file.get_preset());     //各スパン値
    } else {
        file.set_span(changeAin);
        file.save_span_preset(file.get_preset());     //各スパン値を上書き保存
        file.read_span_preset(file.get_preset());     //各スパン値
    }
    bscData.temperature = temp;         //変更した温度取得
    file.save_basic_setting(bscData);   //取得した温度を上書き保存
    cel.set_span(file.get_span());      //スパン値を取得
    file.read_stop_cells();             //未使用セル設定
    set_stopcell();                     //不使用セル(＋12Headの対面点灯を組み合わせる)
    span_auto_f = YES;
    span_item_f = 0;
    for(int i = 0; i < product.head; i++) {
        changeAin[i].fain =0;
        changeAin[i].sain =0;
    }
    delete[] changeAin;
}

/*****************************************************************************
    計量皿に物が載っていないことが前提
    風袋込の重量が0gとなっているか確認を行い、ゼロ点が取れていなかったら
    ゼロ点の取得を行う。
*****************************************************************************/
void set_zero_span()
{
    int i=0;
    int zeroFlg = 0;        //0gが確認できた回数を保持。
    int setNum = 3;         //0gが取得できたことを確認する回数を設定
    float checkWH = param.ZEROTOLERANCE ;  //1.0;    //ゼロ点が安定しているかを判断する範囲
    float zero_average[10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    float ave_sum = 0.0;
    int no_set_count = 0;
    //計量皿(HEAD)の各重量、ゼロ点調整を行う。
    while(i < product.head) {
        lcd.locate(14,1);
        lcd.printf("%2d",i+1);

        //初期化
        //led.REGIS(0);       // LED消灯
        if(product.matrix_mode == MATRIX_OFF){
            //led.REGIS(0);       // LED消灯
        }else{
            for(int k = 0; k < product.head; k++) {
                if(i==k){
                    iro[k] = PENTA_R;
                }else{
                    iro[k] = C_NUSE;
                }
            }
        }
        zeroFlg = 0;
        cel.set_head(i);
        cel.getZero();      //ゼロ点取得用に電圧取得。spanも実行

        for (int x = 0; x < 10; x++){ 
            zero_average[x] = 0.0;
        }
        no_set_count = 0;
        ave_sum = 0.0;
        zero_between_kyousei[i] = servo_n_histry[0][i] + servo_n_histry[1][i];
        //フラグがtrueになるまで(ゼロ点がとれるまで)
        while(zeroFlg < setNum) {
            //不使用セルだった場合は次の処理へ
            if((oneBit[i] & stop_cells) > 0) {
                wait(0.01);
                if(product.matrix_mode == MATRIX_OFF){
                    //led.REGIS(0);
                }
                zeroFlg = setNum;   //現在の計量皿は確認終了
            }else{
                if(no_set_count < 10 /*|| gLevel == 0*/ || forced_zero == false){
                    //Flg = 0 のとき取得中のロードセル位置をLEDを点灯
                    if(zeroFlg == 0) {
                        if(product.matrix_mode == MATRIX_OFF){
                            //led.REGIS(oneBit[i]);
                        }else{
                        }
                        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                            wait(0.05);
                        }
                        wait(0.05);
                    }
                    if(product.matrix_mode == MATRIX_OFF){
                        //led.REGIS(0);
                    }
                    cel.getZero();      //ゼロ点取得用に電圧取得。spanも実行
        
                    //規定重量以内かを確認
                    //printf("%2d:%5.1f|\r\n",i,cel.getWeight(i));
                    if(cel.getWeight(i) <= checkWH && cel.getWeight(i) >= -checkWH) {
                        //誤差が1g以内だったら次の計量皿を確認する
                        
                        AD_bas_min[i] = AD_now_min[i];
                        AD_bas_max[i] = AD_now_max[i];
                        AD_bas_dif[i] = AD_bas_max[i]- AD_bas_min[i];
                        
                        zeroFlg++;
                    } else {
                        cel.setIniHWeight_one(i);
                        zeroFlg = 0;    //規定の重量範囲外だったため
                    }
                }else{
                    //iro[i] = C_QUES;
                    ave_sum = 0.0;
                    for (int x = 0; x < 10; x++){ 
                        setHeadVolt(i, param.KURI);
                        //average[x] = getAin(i);
                        ave_sum += cel.getAin();
                        //led.REGIS(oneBit[i]);
                        iro[i] = PENTA_R;
                        wait(0.1);
                        //iro[i] = C_NULL;
                        wait(0.05);
                    }
                    ave_sum = ave_sum / 10 ;
                    printf("%2d|%8.6f\r\n",i+1,ave_sum);
                    cel.setZero(ave_sum);
                    zeroFlg = setNum;   //現在の計量皿は確認終了
                    if(AD_now_min[i] == 0.0){
                        AD_bas_min[i] = AD_now_min[i];
                    }
                    if(AD_now_max[i] == 0.0){
                        AD_bas_max[i] = AD_now_max[i];
                    }
                    iro[i] = C_NULL;
                }//no_set_count
                no_set_count ++;
                    
                //何らかの理由でゼロ点がとれないなど。ボタン押下で次の処理へ
                if(btn.any_press() == true && btn.ent_press() == false) {
                    wait(0.5);
                    if(product.matrix_mode == MATRIX_OFF){
                        //led.REGIS(0);
                    }
                    zeroFlg = setNum;   //現在の計量皿は確認終了
                    
                    if(AD_now_min[i] == 0.0){
                        AD_bas_min[i] = AD_now_min[i];
                    }
                    if(AD_now_max[i] == 0.0){
                        AD_bas_max[i] = AD_now_max[i];
                    }
                }
            }
        }//while

        i++;    //次の計量皿へ
    }
    led.REGIS(0);  // LED消灯
    lcd.blank(1);
}


void set_zero_span_onehead(int i){

    int zeroFlg   = 0;        //0gが確認できた回数を保持。
    int setNum    = param.zerocount;         //0gが取得できたことを確認する回数を設定
    float checkWH = param.ZEROTOLERANCE;    //ゼロ点が安定しているかを判断する範囲
    float get_diff = 0.0;
    int set_zero_loop = 0;
    int zero_loop_limit = 7;
 //初期化
    wait(1);
    cel.set_head(i);
    cel.setInitial_onehead(i);
    setHeadVolt(i, param.KURI);
    setHeadVolt(i, param.KURI);

    lcd.locate(0,0);
    lcd.printf("wait_stable");
    get_diff = cel.diff_gram(/*param.KURI*/);
    while(get_diff <-param.ZEROTOLERANCE || 
                param.ZEROTOLERANCE < get_diff){
        wait(0.2);
        setHeadVolt(i, param.KURI);
        get_diff = cel.diff_gram(/*param.KURI*/);
        wait(0.1);
        if(gLevel == 1){
            lcd.blank(1);
            lcd.printf("%2d:%6.1fg",i+1,cel.getWeight(i));
            lcd.locate(10,1);
            lcd.printf("%4.1fg",cel.get_HWeight(i));
        }else{
            wait(0.05);
        }
    }
    cel.setIniH_one();

    lcd.blank(0);
    lcd.locate(0,0);
    lcd.printf("ZERO_SET");
    
    if(now_TC_mode >= 1){
        iro[i] = C_NULL;
    }else{
        led.REGIS(0);       // LED消灯
    }
    zeroFlg = 0;
    cel.getZero();      //ゼロ点取得用に電圧取得。spanも実行
    cel.getZero();      //ゼロ点取得用に電圧取得。spanも実行
    wait(0.1);
    set_zero_loop = 0;
//フラグがtrueになるまで(ゼロ点がとれるまで)
    while(zeroFlg < setNum) {
        
        //不使用セルだった場合は次の処理へ
        if((oneBit[i] & stop_cells) > 0) {
            wait(0.2);
            if(now_TC_mode >= 1){
                iro[i] = C_NULL;
            }else{
                led.REGIS(0);       // LED消灯
            }
            zeroFlg = setNum;   //現在の計量皿は確認終了
        }else{
            //Flg = 0 のとき取得中のロードセル位置をLEDを点灯
            if(zeroFlg == 0) {
                if(now_TC_mode >= 1){
                    iro[i] = PENTA_G;
                }else{
                    led.REGIS(oneBit[i]);
                }
                if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                    //wait(0.03);
                }
                wait(0.03);
            }

            if(now_TC_mode >= 1){
                iro[i] = C_NULL;
            }else{
                led.REGIS(0);         // LED消灯
            }
            
            wait(0.1);
            cel.getZero();      //ゼロ点取得用に電圧取得。spanも実行
            /*
            for(int k=0;k<product.head;k++){
                setHeadVolt(k, param.KURI);
            }
            cel.set_head(i);
            */
            wait(0.2);
            setHeadVolt(i, param.KURI);
            if(gLevel == 1){
                lcd.locate(0,0);
                lcd.printf("0_SET");
                lcd.locate(6,0);
                lcd.printf("%5.1f",cel.diff_gram(/*param.KURI*/));
                lcd.locate(12,0);
                lcd.printf("%3d",set_zero_loop);
                lcd.blank(1);
                lcd.printf("%2d:%6.1fg",i+1,cel.getWeight(i));
                lcd.locate(10,1);
                lcd.printf("%4.1fg",cel.get_HWeight(i));
            }else{
                wait(0.05);
            }

            //規定重量以内かを確認
            if(cel.getWeight(i) <= checkWH && cel.getWeight(i) >= -checkWH) {
                //誤差が1g以内だったら次の計量皿を確認する
                
                AD_bas_min[i] = AD_now_min[i];
                AD_bas_max[i] = AD_now_max[i];
                AD_bas_dif[i] = AD_bas_max[i]- AD_bas_min[i];
                zeroFlg++;
                set_zero_loop = 0;
            } else {
                zeroFlg = 0;    //規定の重量範囲外だったため
                set_zero_loop++;
                if(set_zero_loop >= zero_loop_limit){
                    cel.setIniH_one();
                    set_zero_loop = 0;
                }
            }
            //何らかの理由でゼロ点がとれないなど。ボタン押下で次の処理へ
            if(btn.any_press() == true && btn.ent_press() == false) {
                wait(0.5);
                if(now_TC_mode >= 1){
                    iro[i] = C_NULL;
                }else{
                    led.REGIS(0);         // LED消灯
                }
                zeroFlg = setNum;   //現在の計量皿は確認終了
            }
        }
    }//while

    if(now_TC_mode >= 1){
        iro[i] = C_NULL;
    }else{
        led.REGIS(0);         // LED消灯
    }
}












/*****************************************************************************
    立ち上がりゼロ点確認
    現在のAin値とFain値の差が規定値以上だったらエラーとする
*****************************************************************************/
void check_zero_point(void)
{
    float errCheckAin=0;    //Ain値に異常がないか確認
    int err_cnt = 0;
    int gramBuffer = 0;
    int temp = 0;

    //現在の温度を取得
    temp = cel.get_temperature(TEMP_POSITION,10);

    //温度が一定の範囲内か
    if(temp >= TEMP_MIN && (temp <= TEMP_MAX) ) {

        //温度差による差分重量を取得する(gramBufferがマイナス値にならないように)
        if(bscData.temperature >= temp) {
            gramBuffer = (bscData.temperature - temp) * TEMP_GRAM;
        } else {
            gramBuffer = (temp - bscData.temperature) * TEMP_GRAM;
        }
    }

    for(int i=0; i < product.head; i++) {
        //printf("%d   %d  ",oneBit[i] & stop_cells,stop_cells);
        setHeadVolt(i, param.KURI);

        //printf("%f        ",cel.getAin());
        //起動時の風袋無し重量を取得
        errCheckAin = cel.ask_H_Weight(cel.getAin());
        //printf("%f\r\n",errCheckAin);
        //既定値以上の差があれば0確認場所とする
        if( errCheckAin > (param.zeroErr + gramBuffer) || errCheckAin < (-param.zeroErr - gramBuffer)) {
            err_cnt += oneBit[i];
        }
    }//for

    //ゼロ点エラーがあればメッセージ表示
    if(err_cnt > 0) {
        lcd.messege(TOP, DIS_CHE_HEAD, CLS);
        wait(1.0);

        led.REGIS(err_cnt);         //LED点灯
        led.RedOn();
        if (matrix_mode==1) {       //赤点灯で緑色が消えるので
            led.REGIS(err_cnt);         //LED点灯
        }
        se.mistake();               //警告音
        v.output_audio(ERR_ZERO);   //音声出力
        slideMessage();             //ErrMessageを表示
        lcd.cls();                  //ErrMessageを非表示
        v.wait_echo();
    }
}

/*****************************************************************************
    立ち上がりゼロ点確認
    現在のAin値とFain値の差が規定値以上だったらエラーとする
*****************************************************************************/
/*
//引き継ぎ優先のためゼロ確認処理をコメントアウトし
//ゼロ点エラー処理は昔の状態へ変更2017-03-27

void check_zero_point(void){
    float errCheckAin=0;    //Ain値に異常がないか確認
    int err_cnt = 0;
    int nomalBit = 0;
    int gramBuffer = 0;
    int temp = 0;

    //現在の温度を取得
    temp = cel.get_temperature(TEMP_POSITION,10);

    //温度が一定の範囲内か
    if(temp >= TEMP_MIN && (temp <= TEMP_MAX) ){

        //温度差による差分重量を取得する(gramBufferがマイナス値にならないように)
        if(bscData.temperature >= temp){
            gramBuffer = (bscData.temperature - temp) * TEMP_GRAM;
        }else{
            gramBuffer = (temp - bscData.temperature) * TEMP_GRAM;
        }
    }

    for(int i=0; i < product.head; i++) {
        setHeadVolt(i, param.KURI);

        //起動時の風袋無し重量を取得
        errCheckAin = cel.ask_H_Weight(cel.getAin());

        //既定値以上の差があれば0確認場所とする
        if( errCheckAin > (param.zeroErr + gramBuffer) || errCheckAin < (-param.zeroErr - gramBuffer)){
            err_cnt += oneBit[i];
        }
    }//for

    //異常皿は無く正常終了
    if( err_cnt == 0){
        return;
    }

    //正常な皿位置を一箇所取得
    nomalBit = get_nomal_position(err_cnt);

    //正常皿がないため処理終了。問い合わせをしてください。
    if( nomalBit == 0){
        //ゼロ点の確認がとれません。
        //お問い合わせをお願い致します。
        zeroErrInquire(err_cnt);   //エラーのため問い合わせが必要
        return;
    }

    //0点確認する必要があるので音声でお知らせ
    v.output_audio(MSG_LED_OITE);   //音声出力
    v.wait_echo();

    //異常皿と正常皿の認識重量が同じが確認する
    int errorBit = check_weight(err_cnt, nomalBit);

    //エラーヘッドがあれば通知
    if( errorBit != 0){
        zeroErrInquire(errorBit);   //問い合わせ
    }
}

//ゼロ点の確認ができなかったため、問い合わせを促す表示を行う
void zeroErrInquire(int ledOn){
    lcd.messege(TOP, DIS_ZERO_ERR, CLS);
    lcd.messege(LOW, DIS_CHE_HEAD);

    wait(1.0);

    led.REGIS(ledOn);         //LED点灯
    led.RedOn();

    se.mistake();               //警告音
    v.output_audio(MSG_MAINTENANCE);   //音声出力
    slideMessage();             //ErrMessageを表示
    lcd.cls();                  //ErrMessageを非表示
    v.wait_echo();
}

//正常な場所の皿位置を取得
int get_nomal_position(int errBit){

    //皿数繰り返し
    for(int i = 0; i < product.head; i++){

        //0エラー箇所以外の正常皿の位置を返す
        if( !(errBit & oneBit[i])){
            return oneBit[i];
        }
    }

    //正常皿はない
    return 0;
}

//正常皿と異常皿の重量確認を行う
int check_weight(int errBit, int nomalBit){
    int ledOn = 0;
    float weight, nomalWeight;
    ledOn = nomalBit + errBit;  //正常皿、異常皿位置を取得

    float *weightCheck = new float[product.head]; //各点灯皿の重量格納用

    //LED点灯している皿の重量を取得するまで
    while(ledOn){

        btn.new_btn();               //スイッチ(New)取得

        //皿数繰り返し
        for(int i = 0; i < product.head; i++){
            if (ledOn & oneBit[i]) {
                setHeadVolt(i, param.KURI);   //重量再取得

                //精度が出ていたら
                if(cel.ArrayCheck()){
                    weight = 0; //初期化

                    //重量を取得していく
                    weight = cel.getArrayWeight(WHT_CNT-1);

                    //重量範囲を確認
                    if(weight >= gZeroMinGram && weight <= gZeroMaxGram){

                        //正常な皿の重量値を取得
                        if(nomalBit == oneBit[i]){
                            nomalWeight = weight;
                        }

                        weightCheck[i] = weight;
                        ledOn -= oneBit[i]; //取得した皿位置のLEDを消灯
                    }
                }
            }//if
        }//for

        led.REGIS(ledOn);       //調整する場所のLED点灯
        btn.old_btn();              //スイッチ更新

        //LED消灯
        wait(0.03);
        led.REGIS(0);
        wait(0.03);

        //処理途中でユーザーが強制終了
        if(btn.get_btn() == BTN_ESC){
            delete[] weightCheck;
            return 0;
        }
    }//while

    ledOn = nomalBit + errBit;  //正常皿、異常皿位置を再取得

    //結果格納
    int result;
    result = is_zero_err(ledOn, nomalWeight, weightCheck);
    delete[] weightCheck; //処理終了前にメモリ解放
    return result;
}

//ゼロ点が正常か確認
int is_zero_err(int ledOn, const float nomal, const float *checkWeight){
    float diff;
    int errBit = 0;

    for(int i = 0; i < product.head; i++){

        //LED点灯箇所のみ確認する
        if (ledOn & oneBit[i]){
            diff = nomal - checkWeight[i]; //通常皿との重量差分を取得する
            if( (diff > param.zeroRange) || (diff < -param.zeroRange) ){
                //既定値以上の重量差がある。異常と判断。
                errBit += oneBit[i];
            }
        }
    }

    //重量差が範囲内 = 正常
    return errBit;
}
*/

bool select_menu_use(int m){
    switch(m) {
        case(MENU_START)        :return true;
        case(MENU_CHANGE_VALUE) :return true;
        case(MENU_OPTION)       :return true;
        case(MENU_SPAN)         :return true;
        case(MENU_KEIRYO)       :return true;
        case(MENU_KEIRYO2)      :if(product.matrix_mode!=MATRIX_ON)   {return false;}
                                 else                                 {return true; }
        case(MENU_KEIRYO3)      :if(product.matrix_mode!=MATRIX_ON)   {return false;}
                                 else                                 {return true; }
        case(MENU_PASSWORD)     :return true;
        case(MENU_DEMO)         :return false;
        case(MENU_END)          :return false;
        default                 :return false;
    }
}

//メニューを選択
int selectMenu()
{
    int m=0;                        //選択しているメニュー番号
    int disMenu = MENU_KEIRYO;     //メニュー数

    set_stopcell();                     //不使用セル(＋12Headの対面点灯を組み合わせる)
    //Lowレベルの場合表示メニュー数を変更
    if(gLevel == 0) {
        /*
        if( product.productType == STANDARD || product.productType == JUNIOR || product.productType == BIG || product.productType == STANDARD_D || product.productType == JUNIOR_D || product.productType == BIG_D){
            disMenu = MENU_TRAINIG;
        }else{
            //ジュニア/ビッグ用の設定の場合トレーニングは非表示とする
            disMenu = MENU_OPTION;
        }
        */
        disMenu = MENU_KEIRYO;
        led.REGIS(ALL_HEAD^stop_cells2);
        led.RedOff();
    }else if (gLevel == 1){
        disMenu = MENU_PASSWORD;
        led.REGIS(ALL_HEAD^stop_cells2);
        led.RedOff();
    }else if (DEBUG_MODE != 0){
        disMenu = MENU_KEIRYO;
    } else {
        led.REGIS(0);
        led.REGIS(ALL_HEAD^stop_cells2);
        led.RedOn();
    }

    btn.ini();
    lcd.messege(0, DIS_MENU, CLS);    //LCD1行目、cls()実行

    while(1) {
        //lcd表示変更する必要があるか判断
        if(btn.get_one_push()) {
            lcd.menu(m);
        }//if LCD

        btn.new_btn();              //スイッチ(New)取得
        btn.next_or_back(&m,0, disMenu);
        if(select_menu_use(m)==false){
            while(select_menu_use(m)==false){
                if(btn.get_btn()!= BTN_UP && btn.get_btn()!= BTN_DOWN){
                    m += 1;
                    if(m > disMenu)     {m = 0;}
                    if(m < 0)           {m = disMenu;}
                }else{
                    btn.next_or_back(&m,0, disMenu);
                }
            }
        }

        if(btn.push_sound() && !btn.esc()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(go_password == true){
            password();
            go_password = false;
        }

        //Enterが押されたら移行するモード値を取得する
        if(btn.get_btn() == BTN_ENT) {
            btn.old_btn();  //スイッチ更新
            return m;    //選択されたメニューを返す
        }
        btn.old_btn();  //スイッチ更新
    }//while
}

bool select_option_use(int m){
    //printf("TC_mode_%d\r\n",now_TC_mode);
    if(now_TC_mode >= 1){
        if(gLevel == 1)                                                         {return true; }
        else{
            return select_driveoption_use(m);
        }
    }
    if(now_TC_mode == 0){
        //printf("000%d\r\n",m);
        if(gLevel == 1)                                                         {return true; }
        else{
            switch(m) {
                case(OP_2ND)            : 
                        if(product.limit_on==SELECTOR_LIMIT)  {return false;}
                        else                                  {return true; }
                case(OP_ZENNOSE)        : 
                        if(product.limit_on==SELECTOR_LIMIT)  {return false;}
                        else                                  {return false; }
                case(OP_ATONOSE)        : 
                        if(product.limit_on==SELECTOR_LIMIT)  {return false;}
                        else                                  {return true; }
                case(OP_STOP_CELLS)     :                                         return true;
                case(OP_USB_EN)         : 
                        if(product.use_usb==1)                {return true;}
                        else                                  {return false;}
                case(OP_CHAR_ROTATE)    : 
                        if(product.matrix_mode!=MATRIX_ON)    {return false;}
                        else                                  {return false; }
                case(OP_RANK_OVER)      : 
                        if(product.matrix_mode==MATRIX_ON)    {return false;}
                        else if(product.limit_on==WIDE_LIMIT) {return false;}
                        else if(product.limit_on==SELECTOR_LIMIT) {return false;}
                        else                                  {return false; }
                case(OP_KOKUTI)         : 
                        if(product.limit_on==SELECTOR_LIMIT)  {return false;}
                        else                                  {return false; }
                case(OP_KIKAKUGAI_V)    : 
                        if(product.matrix_mode==MATRIX_ON)    {return false;}
                        else                                  {return false; }
                case(OP_RANKSPEED)      : 
                        if(product.matrix_mode==MATRIX_ON)    {return false;}
                        else                                  {return false; }
                case(OP_BTN_SETTING)    : 
                        if(product.limit_on==SELECTOR_LIMIT)  {return true;}
                        else                                  {return true; }
                case(OP_BEGINNER)       : 
                        if(product.limit_on==SELECTOR_LIMIT)  {return false;}
                        else                                  {return false; }
                case(OP_SEL_BEEP)       : 
                        if(product.limit_on==SELECTOR_LIMIT)  {return false;}
                        else                                  {return false; }
                case(OP_BUFFER)         : 
                        if(product.limit_on==SELECTOR_LIMIT)  {return false;}
                        else                                  {return true; }
                case(OP_TIMER)          : 
                        if(product.use_usb==1)                {return true;}
                        else                                  {return false;}
                case(OP_NO_COMBI_V )    :                      return true;
                case(OP_SECONDMBI_V )   : 
                        if(product.limit_on==SELECTOR_LIMIT)  {return false;}
                        else                                  {return false; }
                case(OP_NUMBER_COLOR )  : 
                        if(product.limit_on==SELECTOR_LIMIT)  {return false;}
                        else                                  {return false; }
                case(OP_O_count_lim )   : 
                        if(product.matrix_mode==MATRIX_ON)    {return true;}
                        else                                  {return false; }
                case(OP_O_lit_count_lim): 
                        if(product.matrix_mode==MATRIX_ON)    {return true;}
                        else                                  {return false; }
                case(OP_NAGAMONO_HOSEI):                     return true;
                case(OP_NAGAMONO_HOSEI2):                    return false;
                case(OP_SERVO1):                             return true;
                case(OP_SERVO2):                             return true;
                case(OP_SERVO3):                             return true;
                case(OP_SERVO4):                             return true;
                case(OP_ZERO_CHECK):                         return true;
                case(OP_WAITING_LIMMIT):                     return true;
                case(OP_SELAUTOZERO):                        return true;
                case(OP_KEKKA_HYOUJI):                       return true;
                case(OP_ALART_AUTOZERO):                     return true;
                case(OP_FIX_VALUE):                          return true;
                default:                                                        return false;
            }
        }
    }
    return false;
}
//メニューを選択
int selectMenu_option(int mode_op)         //17/09/21
{
    //int m=0;                        //選択しているメニュー番号
    int m=mode_op;                        //選択しているメニュー番号
    int disMenu = OP_END - 1;     //メニュー数
    const char name[]= {0xB5,0xCC,0xDF,0xBC,0xAE,0xDD,0x00};   //ｵﾌﾟｼｮﾝ
    int jump_num=0;         //不使用でジャンプした場合に数字部の数を減らす
    //S+以外(USBｼｭﾂﾘｮｸは今のところS+のみ)はメニューを減らす(裏ﾓｰﾄﾞでは変更可能)
    //if(product.productType == MATRIX || product.productType == MATRIX_D || gLevel == 1){
    //}else{
    //    disMenu = OP_USB_EN - 1;     //メニュー数
    //}
    //Lowレベルの場合表示メニュー数を変更
    if(gLevel == 0) {
        led.REGIS(0xfff^stop_cells2);
        led.RedOff();
    } else {
        led.REGIS(0);
        led.REGIS(0xfff^stop_cells2);
        led.RedOn();
    }
    /*if(product.limit_on == SELECTOR_LIMIT && gLevel == 0) {       //selecter
        if(m == OP_STOP_CELLS || m == OP_CHAR_ROTATE || m == OP_RANKSPEED|| m == OP_BTN_SETTING ) {
        } else {
            m = OP_STOP_CELLS;//とりあえず表示する候補
        }
    }
    if(product.head == 1 && gLevel == 0) {       //onetouch
        if(m == OP_USB_EN  || m == OP_CHAR_ROTATE || m == OP_RANKSPEED|| m == OP_BTN_SETTING) {
        } else {
            m = OP_USB_EN ;//とりあえず表示する候補
        }
    }*/
    while(select_option_use(m)==false){
        m+=1;
        if(m>=OP_END){
            m=0;
        }
    }



    btn.ini();
    lcd.blank(TOP);                     //組み合わせ内容表示分をクリア
    lcd.printf(name);
    //lcd.messege(0, DIS_MENU, CLS);    //LCD1行目、cls()実行
    //printf("%d\r\n",disMenu);
    jump_num=0;
    for(int j=0;j<=m;j++){
        if(select_option_use(j)==false){
            jump_num +=1;
        }
    }

/*
    if(gLevel == 1) {
        if(product.head <= 8) {       //6head
        }
    } else if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {

        if(product.head == 1) {       //onetouch
            if(m ==  OP_USB_EN) {
                jump_num = 4;
            } else if(m == OP_CHAR_ROTATE) {
                jump_num = 4;
            } else if(m == OP_RANKSPEED) {
                jump_num = 7;
            } else if(m == OP_BTN_SETTING) {
                jump_num = 7;
            } else {
                jump_num = 0;
            }

        } else if(product.limit_on == SELECTOR_LIMIT) {       //selecter
            if(m == OP_STOP_CELLS) {
                jump_num = 3;
            } else if(m == OP_CHAR_ROTATE) {
                jump_num = 4;
            } else if(m == OP_RANKSPEED) {
                jump_num = 7;
            } else if(m == OP_BTN_SETTING) {
                jump_num = 7;
            } else {
                jump_num = 0;
            }

        } else if(product.head <= 8) {       //6head
            if(m >= OP_RANK_OVER) {
                jump_num = 1;
            } else {
                jump_num = 0;
            }
        } else {                    //12head
            if(m >= OP_RANK_OVER) {
                jump_num = 1;
            } else {
                jump_num = 0;
            }
        }
    } else {//ﾏﾄﾘｸｽじゃない
        if(product.head <= 8) {       //6head
            if(m >= OP_RANKSPEED) {
                jump_num = 2;
            } else if(m >= OP_USB_EN) {
                jump_num = 1;
            } else {
                jump_num = 0;
            }
        } else {                   //12head
            if(m >= OP_RANKSPEED) {
                jump_num = 2;
            } else if(m >= OP_USB_EN) {
                jump_num = 1;
        //printf("%d\r\n",m);
            } else {
                jump_num = 0;
            }
        }
    }*/
    while(1) {
        //lcd表示変更する必要があるか判断
        if(btn.get_one_push()) {
            jump_num=0;
            for(int j=0;j<=m;j++){
                if(select_option_use(j)==false){
                    jump_num +=1;
                }
            }
            lcd.blank(1);           //2行目をクリア
            lcd.locate(0,1);
            lcd.printf("%d",(m - jump_num)+1);
            lcd.Option(m);
        }//if LCD

        btn.new_btn();              //スイッチ(New)取得
        //esc押下なら処理終了
        if(btn.esc()) {
            se.Touch_01();
            btn.old_btn();  //スイッチ更新
            return 99;
        }
        btn.next_or_back(&m,0, disMenu);
        /*while(((m == OP_USB_EN || m == OP_CHAR_ROTATE || m == OP_RANKSPEED )&& product.productType != MATRIX && product.productType != MATRIX_D && product.matrix_mode != MATRIX_ON&& gLevel != 1)||
                (m == OP_RANK_OVER && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)&& gLevel != 1)||
                (product.limit_on == SELECTOR_LIMIT && (m != OP_STOP_CELLS && m != OP_CHAR_ROTATE && m != OP_RANKSPEED) && gLevel != 1)||
                (product.head == 1                  && (m != OP_USB_EN     && m != OP_CHAR_ROTATE && m != OP_RANKSPEED) && gLevel != 1)
             ) {*/
        while(select_option_use(m)==false) {
            btn.next_or_back(&m,0, disMenu);
            //btn.next_or_back(&jump_num,0, disMenu);
            /*
            if(m <= 0) {
                jump_num = 0;
            }*/
        }
        /*
        if(gLevel == 1) { //とりあえず、裏ﾓｰﾄﾞは全部表示させる
            if(product.head <= 8) {       //
            }
        } else if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            if(product.head == 1) {       //onetouch
                if(m ==  OP_USB_EN) {
                    jump_num = 4;
                } else if(m == OP_CHAR_ROTATE) {
                    jump_num = 4;
                } else if(m == OP_RANKSPEED) {
                    jump_num = 7;
                } else if(m == OP_BTN_SETTING) {
                    jump_num = 7;
                } else {
                    jump_num = 0;
                }

            } else if(product.limit_on == SELECTOR_LIMIT) {       //selecter
                if(m == OP_STOP_CELLS) {
                    jump_num = 3;
                } else if(m == OP_CHAR_ROTATE) {
                    jump_num = 4;
                } else if(m == OP_RANKSPEED) {
                    jump_num = 7;
                } else if(m == OP_BTN_SETTING) {
                    jump_num = 7;
                } else {
                    jump_num = 0;
                }


            } else  if(product.head <= 8) {       //6head
                if(m >= OP_RANKSPEED && product.limit_on==2) {
                    jump_num = 2;
                } else if(m >= OP_RANK_OVER) {
                    jump_num = 1;
                } else {
                    jump_num = 0;
                }
            } else {                    //12head
                if(m >= OP_RANKSPEED && product.limit_on==2) {
                    jump_num = 2;
                } else if(m >= OP_RANK_OVER) {
                    jump_num = 1;
                } else {
                    jump_num = 0;
                }
            }
        } else {//ﾏﾄﾘｸｽじゃない
            if(product.head <= 8) {       //6head
                if(m >= OP_RANKSPEED) {
                    jump_num = 2;
                } else if(m >= OP_USB_EN) {
                    jump_num = 1;
                } else {
                    jump_num = 0;
                }
            } else {                   //12head
                if(m >= OP_RANKSPEED) {
                    jump_num = 2;
                } else if(m >= OP_USB_EN) {
                    jump_num = 1;
                    //printf("%d\r\n",m);
                } else {
                    jump_num = 0;
                }
            }
        }*/

        /*        if((m == OP_KATAMEN && product.head <= 8) ||
                   (m == OP_USB_EN && product.productType != MATRIX && product.productType != MATRIX_D && gLevel != 1)){

                    btn.next_or_back(&m,0, disMenu);
                    btn.next_or_back(&jump_num,0, disMenu);

                    if(m <= 0){
                        jump_num = 0;
                    }//else{
                     //   jump_num += 1;
                    //}
                }*/
        /*
        if(m <= 0) {
            jump_num = 0;
        }*/
        jump_num=0;
        for(int j=0;j<=m;j++){
            if(select_option_use(j)==false){
                jump_num +=1;
            }
        }
        if(btn.push_sound() && !btn.esc()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        //Enterが押されたら移行するモード値を取得する
        if(btn.get_btn() == BTN_ENT) {
            btn.old_btn();  //スイッチ更新
            return m;    //選択されたメニューを返す
        }
        btn.old_btn();  //スイッチ更新
    }//while
}
//プリセット選択(画面表示を行う)
//disNo モードを渡して表示するタイトルを決定する
void displaySelect(int disNo)
{
    //int max_head = product.head;
    int max_head = 12;
    int presetNo = 0;
    //組み合わせ目標選択だったら
    if(disNo == MENU_START) {
        v.output_audio(MSG_SELECT); //最初だけ音声出力
        wait(0.5);
        v.wait_echo();
    }
    if(disNo == MENU_CHANGE_VALUE && not(btn.esc())) {
        if(esc_flg == false){
            v.output_audio2(); //音声出力
            wait(0.5);
        }
    }
    //メッセッジ表示
    lcd.messege(TOP, DIS_KANTAN_NO,CLS);

    presetNo = file.get_preset();
    if(demo_flg==false&&presetNo>(max_head*2-1)){
        presetNo=(max_head*2-1);
    }
    select_preset(&presetNo);       //プリセット選択。ESCでキャンセルならメニュー画面へ
    file.set_preset(presetNo);      //
    esc_flg = false;
}

/* ******************************
    ボタン操作にてプリセットを選択
    headPosition:現在のヘッド位置
****************************** */
void select_preset(int *headPosition)
{
    btn.ini();
    //int max_head = product.head;
    int max_head = 12;
    
    const char checker[]= //{0x28,0xC1,0xAA,0xAF,0xB6,0xB0,0x29,0x00};     //(ﾁｪｯｶｰ)
                            {0x28,0xBA,0xBD,0xB3,0x29,' ' ,' ' ,0x00};      //(ｺｽｳ)
    const char checker2[]=  {0x28,0xB8,0xDE,0xD7,0xD1,0x29,' ' ,0x00};      //(ｸﾞﾗﾑ)
    //const char irisu[]   =  {0x28,0xB2,0xD8,0xBD,0xB3,0x29,' ' ,0x00};      //(ｲﾘｽｳ)
    const char ranker[]   = {0x28,0xD7,0xDD,0xB8,0x29,0x00};      //(ﾗﾝｸ)
    const char nodata[]   = {'N' ,'o' ,' ' ,'D' ,'a' ,'t' ,'a' ,0x00};      //(nodata)

    //const char Select[][SIZE]={
    //    {0xD7,0xDD,0xB8,0xBE,0xDD,0xCD,0xDE,0xC2,0x00}       //ﾗﾝｸｾﾝﾍﾞﾂ
    // };
    /**********************************************
    エラー対策
    ヘッド数を変更した場合にありえないプリセットを
    見ることがあるので、設定ヘッド数*2以上のプリセット番号は
    初期化する
    **********************************************/
    
    if((*headPosition >= max_head*2 && max_head > 1) || (product.head == 1 && *headPosition > 3))    *headPosition = 0;
    
    led.RedOff();
    led.REGIS(0);
    //Enter押されるまで
    while(btn.get_btn() != BTN_ENT) {
        //ボタン操作あれば画面更新
        if(btn.get_btn()) {
            load = file.get_load(*headPosition);   //データ取得

            lcd.easy_view(load);
            lcd.blank(1);
            if(set_mokuhyo_no(load.kumi_flag) == 0){
                lcd.printf("%02d:",*headPosition+1);
                lcd.printf(nodata);
            }
            //対面だったら表示を変更する
            else if(load.kumi_flag == KUMI_TAIMEN) {
                lcd.printf("%02d:L%dg R%dg",*headPosition+1,load.target,load.taimenTarget);
                
            } 
            //文字を表示させる。
            else if((load.kumi_flag ==  KUMI_RANK || load.kumi_flag ==  KUMI_KAIKYU || load.kumi_flag ==  KUMI_KAISU1 || load.kumi_flag ==  KUMI_KAISU2 || load.kumi_flag ==  KUMI_PACK || load.kumi_flag ==  KUMI_PACK2) && 
                    (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                lcd.printf("%02d:",*headPosition+1);
                printname(*headPosition);
                if(load.kumi_flag ==  KUMI_RANK/* || load.kumi_flag ==  KUMI_HIKIZAN*/){
                    lcd.locate(11,1);
                    lcd.printf(ranker);
                }                //lcd.printf(Select[0]);
            }
            //残数表示[番号]：[重量]g(ｺｽｳ)
            else if((load.kumi_flag ==  KUMI_KOSUU)&& (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                lcd.printf("%02d:%4dg",*headPosition+1,load.target);
                lcd.printf(checker);
            } 
            //残量表示[番号]：[重量]g(ｸﾞﾗﾑ)
            else if((load.kumi_flag ==  KUMI_JUURYO)&& (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                lcd.printf("%02d:%4dg",*headPosition+1,load.target);
                lcd.printf(checker2);
            } 
            else {
                lcd.printf("%02d:%5dg%c%5dg",*headPosition+1,load.target, 0x7e, load.target+load.jougen);
                //lcd.printf("%02d:%dg%c%c%c%dg",*headPosition+1,load.target, 0x20, 0x7e, 0x20, load.target+load.jougen);
            }
        }

        //ボタン操作時の効果音
        if(btn.push_sound()) {
            se.Touch_01();
        }

        btn.new_btn();                   //スイッチ(New)取得

        //esc押下なら処理終了
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return;
        }

        //プリセット変更
        btn.puls_or_minus(headPosition);
        if(demo_flg==true){
            if(*headPosition >= max_head*2&&max_head>1||*headPosition >= 4 &&product.head==1)    *headPosition = 0;
            if(*headPosition < 0&&max_head>1)           *headPosition = max_head*2;
            else if(*headPosition < 0&&max_head==1)           *headPosition = 4-1;
        }else{
            if(*headPosition >= max_head*2&&max_head>1||*headPosition >= 4 &&product.head==1)    *headPosition = 0;
            if(*headPosition < 0&&max_head>1)           *headPosition = max_head*2-1;
            else if(*headPosition < 0&&max_head==1)           *headPosition = 4-1;
        }
        //現在選択している計量皿位置をLED点灯
        led.REGIS(0);
        led.RedOff();
        /*
        if(*headPosition >= 0 && *headPosition < product.head) {
            led.RedOff();
            led.REGIS(oneBit[*headPosition]);
        } else if(*headPosition >= product.head && *headPosition < product.head*2) {
            led.RedOn();
            led.REGIS(oneBit[*headPosition-product.head]);
        } else if(*headPosition >= product.head*2 && *headPosition < product.head*3) {
            led.RedOff();
            led.REGIS(0);
            //led.REGIS(oneBit[*headPosition-product.head*2]);
        } else if(*headPosition >= product.head*3 && *headPosition < product.head*4) {
            led.RedOff();
            led.REGIS(0);
            //led.REGIS(oneBit[*headPosition-product.head*3]);
        } else {
            led.RedOn();
            led.REGIS(0);
        }
        */
        ////変更スピードが速いため少し待つ
        if( btn.get_btn() == BTN_KEEP_UP || btn.get_btn() == BTN_KEEP_DOWN) {
            wait(0.1);
        }
        btn.old_btn();  //スイッチ更新
    }//for
}

int multi_weight_set3(int headPosition, int data_KUMI_FLAG){
#define u_num 10            //一応項目は10ﾗﾝｸまでの予定
#define koumoku 8         //1ランク当たりの項目数

    const char rank_color[] = {0xD7,0xDD,0xB8,0xC9,0xB2,0xDB,0x00};      //ﾗﾝｸﾉｲﾛ
    //const char fast_rank[] = {0xBB,0xB2,0xBC,0xAE,0xB3,' ' ,0xD7,0xDD,0xB8,0x00};      //ｻｲｼｮｳ ﾗﾝｸ
    const char select_rank[] = {0xCB,0xAE,0xB3,0xBC,0xDE,' ' ,0xD7,0xDD,0xB8,0x00};      //ﾋｮｳｼﾞ ﾗﾝｸ
    data_t limitData;
    int mode = 0;                           //項目の値をset_presetと合わせるための変数
    //int old_i;                            //繰返しが進んだか戻ったか判定
    //bool esc_flug=false;                    //エスケープが
    //bool end_flug=false;                    //これ以上のﾗﾝｸ設定は不要の場合(RANK_EN=0の時)
    //unitWeight_t unit[u_num];
    multiWeight_t mweight;
    mweight = file.get_lweight(headPosition); //現在プリセット選択されているデータを取得
    int data2[koumoku];
    int data_f= mweight.Rcolor; 
    //int data_p= mweight.priority;
    int data_r= mweight.Rsort;
    int data_s= mweight.Rselect;
    int data[u_num][koumoku]= {{mweight.u01.rank_en ,mweight.u01.rank_min ,mweight.u01.rank_max ,mweight.u01.kumi_min ,mweight.u01.kumi_max ,mweight.u01.target ,mweight.u01.jougen}
        ,{mweight.u02.rank_en ,mweight.u02.rank_min ,mweight.u02.rank_max ,mweight.u02.kumi_min ,mweight.u02.kumi_max ,mweight.u02.target ,mweight.u02.jougen}
        ,{mweight.u03.rank_en ,mweight.u03.rank_min ,mweight.u03.rank_max ,mweight.u03.kumi_min ,mweight.u03.kumi_max ,mweight.u03.target ,mweight.u03.jougen}
        ,{mweight.u04.rank_en ,mweight.u04.rank_min ,mweight.u04.rank_max ,mweight.u04.kumi_min ,mweight.u04.kumi_max ,mweight.u04.target ,mweight.u04.jougen}
        ,{mweight.u05.rank_en ,mweight.u05.rank_min ,mweight.u05.rank_max ,mweight.u05.kumi_min ,mweight.u05.kumi_max ,mweight.u05.target ,mweight.u05.jougen}
        ,{mweight.u06.rank_en ,mweight.u06.rank_min ,mweight.u06.rank_max ,mweight.u06.kumi_min ,mweight.u06.kumi_max ,mweight.u06.target ,mweight.u06.jougen}
        ,{mweight.u07.rank_en ,mweight.u07.rank_min ,mweight.u07.rank_max ,mweight.u07.kumi_min ,mweight.u07.kumi_max ,mweight.u07.target ,mweight.u07.jougen}
        ,{mweight.u08.rank_en ,mweight.u08.rank_min ,mweight.u08.rank_max ,mweight.u08.kumi_min ,mweight.u08.kumi_max ,mweight.u08.target ,mweight.u08.jougen}
        ,{mweight.u09.rank_en ,mweight.u09.rank_min ,mweight.u09.rank_max ,mweight.u09.kumi_min ,mweight.u09.kumi_max ,mweight.u09.target ,mweight.u09.jougen}
        ,{mweight.u10.rank_en ,mweight.u10.rank_min ,mweight.u10.rank_max ,mweight.u10.kumi_min ,mweight.u10.kumi_max ,mweight.u10.target ,mweight.u10.jougen}
    };
    int data_f_end =0;
    int data_s_end =0;
    while(data_f_end == 0) {      //各項目の後に、データの整理と名前入力の項目が入るので、u_numより1個多い
        //btn.end_force();
        if(data_s_end == 0){
            limitData.min = 0;
            limitData.max = 2;
            limitData.lcd = 1;
            mode = TL2_END+5;
            lcd.locate(0,0);
            lcd.printf("                ");
            lcd.locate(0,0);
            lcd.printf(select_rank);
            wait(0.1);
            SetteiMokuhyo2(&data_s, limitData,mode,KUMI_RANK);
            if(btn.esc()) {
                return 1;
            }
            if (data_s!=0){
                data_f=0;
                data_f_end = 1;
            }
            data_s_end = 1;
        }else{
            data_s_end = 0;
        }
        if(data_f_end == 0){//色を設定
            limitData.min = 0;
            limitData.max = 1;
            limitData.lcd = 1;
            mode = TL2_END+6;
            lcd.locate(0,0);
            lcd.printf("                ");
            lcd.locate(0,0);
            lcd.printf(rank_color);
            wait(0.1);
            SetteiMokuhyo2(&data_f, limitData,mode,KUMI_RANK);
            if(btn.esc()) {
                data_s_end = 0;
            }else{
                data_f_end = 1;
            }
        }
    }
    for(int i = 0; i < product.head; i++) {
        iro[i]=C_NULL;
    }
    for (int i = 0 ; i < u_num ; i++ ) {
        for(int j = 0 ; j < koumoku ; j++ ) {
            data2[j] = data[i][j];
        }
        file.set_lweight(data2, headPosition, i,data_f, data_r, data_s);
    }
    //file.set_lweight(data, headPosition);
    file.save_unitweight();      //変更されたデータを受け渡し

    return 0;
}
//複数重量範囲の設定
int multi_weight_set2(int headPosition, int data_KUMI_FLAG)
{
#define u_num 10            //一応項目は10ﾗﾝｸまでの予定
#define koumoku 8         //1ランク当たりの項目数

    const char rank_color[] = {0xB6,0xD7,0xB0,0x00};      //ﾗﾝｸ ｶﾗｰ
    //const char kumiyu1[]={0xBE,0xB2,0xD8,0xC2,0xD5,0xB3,0xBE,0xDD,' ' ,0xCE,0xB3,0xCE,0xB3,0x00};    //ｾｲﾘﾂﾕｳｾﾝ ﾎｳﾎｳ
    const char kumir1[]= {0xC3,0xDE,0xB0,0xC0,0xC9,' ' ,0xDC,0xB9,0xB6,0xC0,0x00};   //ﾃﾞｰﾀﾉ ﾜｹｶﾀ
    //const char fast_rank[] = {0xBB,0xB2,0xBC,0xAE,0xB3,' ' ,0xD7,0xDD,0xB8,0x00};      //ｻｲｼｮｳ ﾗﾝｸ

    //onst char select_rank[] = {0xCB,0xAE,0xB3,0xBC,0xDE,' ' ,0xD7,0xDD,0xB8,0x00};      //ﾋｮｳｼﾞ ﾗﾝｸ


    const int dammy = 20;   //関数切り替えに利用するダミーデータ
    data_t limitData;
    int i=0;
    int j=0;
    int mode = 0;                           //項目の値をset_presetと合わせるための変数
    int set_en=0;                           //項目で使用するかどうかのフラグ 0:使用しない_1:使用する_2:
    //int old_i;                            //繰返しが進んだか戻ったか判定
    //bool esc_flug=false;                    //エスケープが
    //bool end_flug=false;                    //これ以上のﾗﾝｸ設定は不要の場合(RANK_EN=0の時)
    //unitWeight_t unit[u_num];
    multiWeight_t mweight;
    mweight = file.get_lweight(headPosition); //現在プリセット選択されているデータを取得
    int data2[koumoku];
    int data_f= mweight.Rcolor; 
    //int data_p= mweight.priority;
    int data_r= mweight.Rsort;
    int data_s= mweight.Rselect;
    int data[u_num][koumoku]= {{mweight.u01.rank_en ,mweight.u01.rank_no ,mweight.u01.rank_min ,mweight.u01.rank_max ,mweight.u01.kumi_min ,mweight.u01.kumi_max ,mweight.u01.target ,mweight.u01.jougen}
                              ,{mweight.u02.rank_en ,mweight.u02.rank_no ,mweight.u02.rank_min ,mweight.u02.rank_max ,mweight.u02.kumi_min ,mweight.u02.kumi_max ,mweight.u02.target ,mweight.u02.jougen}
                              ,{mweight.u03.rank_en ,mweight.u03.rank_no ,mweight.u03.rank_min ,mweight.u03.rank_max ,mweight.u03.kumi_min ,mweight.u03.kumi_max ,mweight.u03.target ,mweight.u03.jougen}
                              ,{mweight.u04.rank_en ,mweight.u04.rank_no ,mweight.u04.rank_min ,mweight.u04.rank_max ,mweight.u04.kumi_min ,mweight.u04.kumi_max ,mweight.u04.target ,mweight.u04.jougen}
                              ,{mweight.u05.rank_en ,mweight.u05.rank_no ,mweight.u05.rank_min ,mweight.u05.rank_max ,mweight.u05.kumi_min ,mweight.u05.kumi_max ,mweight.u05.target ,mweight.u05.jougen}
                              ,{mweight.u06.rank_en ,mweight.u06.rank_no ,mweight.u06.rank_min ,mweight.u06.rank_max ,mweight.u06.kumi_min ,mweight.u06.kumi_max ,mweight.u06.target ,mweight.u06.jougen}
                              ,{mweight.u07.rank_en ,mweight.u07.rank_no ,mweight.u07.rank_min ,mweight.u07.rank_max ,mweight.u07.kumi_min ,mweight.u07.kumi_max ,mweight.u07.target ,mweight.u07.jougen}
                              ,{mweight.u08.rank_en ,mweight.u08.rank_no ,mweight.u08.kumi_min ,mweight.u08.kumi_max ,mweight.u08.kumi_min ,mweight.u08.kumi_max ,mweight.u08.target ,mweight.u08.jougen}
                              ,{mweight.u09.rank_en ,mweight.u09.rank_no ,mweight.u09.kumi_min ,mweight.u09.kumi_max ,mweight.u09.kumi_min ,mweight.u09.kumi_max ,mweight.u09.target ,mweight.u09.jougen}
                              ,{mweight.u10.rank_en ,mweight.u10.rank_no ,mweight.u10.kumi_min ,mweight.u10.kumi_max ,mweight.u10.kumi_min ,mweight.u10.kumi_max ,mweight.u10.target ,mweight.u10.jougen}
                            };
    int matrix_RG_SA = CG_RANK_START-C_RANK_START; //ﾏﾄﾘｸｽで緑と赤のenum差を埋めるための数
    
    /*      int data[]={mweight.u01.rank_min ,mweight.u01.rank_max ,mweight.u01.kumi_min ,mweight.u01.kumi_max ,mweight.u01.target ,mweight.u01.jougen
                  ,mweight.u02.rank_min ,mweight.u02.rank_max ,mweight.u02.kumi_min ,mweight.u02.kumi_max ,mweight.u02.target ,mweight.u02.jougen
                  ,mweight.u03.rank_min ,mweight.u03.rank_max ,mweight.u03.kumi_min ,mweight.u03.kumi_max ,mweight.u03.target ,mweight.u03.jougen
                  ,mweight.u04.rank_min ,mweight.u04.rank_max ,mweight.u04.kumi_min ,mweight.u04.kumi_max ,mweight.u04.target ,mweight.u04.jougen
                  ,mweight.u05.rank_min ,mweight.u05.rank_max ,mweight.u05.kumi_min ,mweight.u05.kumi_max ,mweight.u05.target ,mweight.u05.jougen};
    */
//    mweight = data[];

    int data_f_end =0;
    int data_s_end =0;
    rank_RG = 0;
    while(i < u_num+2) {      //各項目の後に、データの整理と名前入力の項目が入るので、u_numより1個多い
        pc.printf("%5d\r\n",i);
        //btn.end_force();
        /*if(i==0 && j==0 && data_KUMI_FLAG == KUMI_RANK && data_s_end == 0){
            limitData.min = 0;
            limitData.max = 2;
            limitData.lcd = 1;
            mode = TL2_END+5;
            lcd.locate(0,0);
            lcd.printf("                ");
            lcd.locate(0,0);
            lcd.printf(select_rank);
            wait(0.1);
            SetteiMokuhyo2(&data_s, limitData,mode,data_KUMI_FLAG);
            if(btn.esc()) {
                return 1;
            }
            if (data_s!=0){
                data_f=0;
                data_f_end = 1;
            }
            data_s_end = 1;
        }else{
            data_s_end = 0;
        }*/
        /*if(i==0 && j==0 && data_KUMI_FLAG == KUMI_RANK && data_f_end == 0){//色を設定
            limitData.min = 0;
            limitData.max = 1;
            limitData.lcd = 1;
            mode = TL2_END+6;
            lcd.locate(0,0);
            lcd.printf("                ");
            lcd.locate(0,0);
            lcd.printf(rank_color);
            wait(0.1);
            SetteiMokuhyo2(&data_f, limitData,mode,data_KUMI_FLAG);
            if(btn.esc()) {
                return 1;
            }
            data_f_end = 1;
        }else{
            data_f_end = 0;
        }*/
        while(j < koumoku) {
            //******************************************************************************************
            //******状況による各種設定の変更 設定しない内容なら set_en=0にしていく
            //******************************************************************************************
            if(data_KUMI_FLAG == KUMI_KAIKYU) { //ランク別選択
                if(i == 2 && j == TL2_RANK_EN) { //ランク[M]は無いと全部OFFにもできるので、絶対ENにする
                    data[i][TL2_RANK_EN] = 1;
                    set_en=3;//設定値は強制
                } else if(i >0 && i < 4 && j == TL2_RANK_EN) {
                    set_en=1;
                } else if(i == 2 && j == TL2_RANK_MIN && data[1][TL2_RANK_EN]==0 && data[2][TL2_RANK_EN]==1) { //M以下で、1個前がOFF、今はONなら、min発動
                    //printf("00\r\n");
                    data[1][TL2_RANK_MIN]=param.CUT+1;
                    data[1][TL2_RANK_MAX]=param.CUT+1;
                    data[0][TL2_RANK_MIN]=param.CUT+1;
                    data[0][TL2_RANK_MAX]=param.CUT+1;
                    set_en=1;
                } else if(i == 2 && j == TL2_RANK_MAX && data[1][TL2_RANK_EN]==0 && data[2][TL2_RANK_EN]==1) { //M以下で、1個前がOFF、今はONなら、1個前のMAXデータを修正する
                    data[1][TL2_RANK_MAX]=data[2][TL2_RANK_MIN];
                    data[1][TL2_RANK_MIN]=data[1][TL2_RANK_MAX];
                    data[0][TL2_RANK_MAX]=data[1][TL2_RANK_MIN];
                    set_en=1;
                } else if(i == 1 && j == TL2_RANK_MAX && data[1][TL2_RANK_EN]==1) { //M以下で、1個前がOFF、今はONなら、1個前のMAXデータを修正する
                    data[0][TL2_RANK_MAX]=data[1][TL2_RANK_MIN];
                    set_en=1;
                } else if(j == TL2_RANK_NO && data[i][TL2_RANK_EN]==1){
                    set_en=0;
                    data[0][TL2_RANK_NO]=C_S;
                    data[1][TL2_RANK_NO]=C_M;
                    data[2][TL2_RANK_NO]=C_L;
                        
                    data[i][TL2_RANK_NO]=data[i-1][TL2_RANK_NO]+1;
                } else if((i != 0 && i < 4 && j != TL2_RANK_MIN ) || (i == 1 && j == TL2_RANK_MIN)) {               //mweight.u05ﾗﾝｸﾍﾞﾂ組合せは5種(うち2Sと2Lは表示のみ)
                    set_en=1;
                } else if(i == u_num || i == u_num + 1) {
                    set_en=2;
                } else {
                    set_en=0;
                }
            }else if(data_KUMI_FLAG == KUMI_KAISU2 && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)){
                if(i == 0) { //2L以上のｾｯﾃｲ時に、2Lもしくはその1個前がOFFだったらすっ飛ばす
                    set_en=0;
                } else if((i == 1 || i == 2)&& j == TL2_RANK_EN) { //ランク[M]は無いと全部OFFにもできるので、絶対ENにする
                    data[i][TL2_RANK_EN] = 1;
                    set_en=3;//設定値は強制
                } else if(i == 3 && j == TL2_RANK_EN ){
                    set_en=1;
                } else if(i == 4 && data[3][TL2_RANK_EN]==1 && j == TL2_RANK_EN ){//大きいﾗﾝｸ側を使用するなら、2段目表記もEN
                    data[i][TL2_RANK_EN] = 1;
                    set_en=3;//設定値は強制
                } else if(j == TL2_KUMI_MIN  && data[i][TL2_RANK_EN]==1){
                    set_en=1;
                } else if(j == TL2_TARGET  && data[i][TL2_RANK_EN]==1){
                    set_en=1;
                } else if(1 <= i && i <= 4 && j == TL2_JOUGEN  && data[i][TL2_RANK_EN]==1){
                    set_en=0;
                    if(i== 1 || i== 3){
                        data[i][TL2_JOUGEN] = int((float(data[i][TL2_TARGET])*1.05+4.0)/5)*5;
                    }
                    if(i== 2 || i== 4){
                        data[i][TL2_JOUGEN] = int((float(data[i][TL2_TARGET])*1.05+9.0)/5)*5;
                    }
                } else if((i==2||i==4) && j == TL2_RANK_NO && data[i][TL2_RANK_EN]==1){
                    data[i][TL2_RANK_NO] = data[i-1][TL2_RANK_NO];
                    set_en=3;
                } else if(j == TL2_RANK_NO && data[i][TL2_RANK_EN]==1){
                    set_en=1;
                    if(i>0){
                        if(data[i][TL2_RANK_NO]<CG_RANK_START && data[i-1][TL2_RANK_NO]<CG_RANK_START){    //両方と赤だった場合
                            if(data[i][TL2_RANK_NO] < C_9L && data[i-1][TL2_RANK_NO] < C_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO]){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO] = data[i-1][TL2_RANK_NO]+1;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == C_SA && data[i][TL2_RANK_NO] < C_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = C_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == C_MA && data[i][TL2_RANK_NO] < C_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = C_L;              //[L]にする
                            }
                        }else if(data[i][TL2_RANK_NO]>CG_RANK_START && data[i-1][TL2_RANK_NO]>CG_RANK_START){  //両方と緑だった場合
                            if(data[i][TL2_RANK_NO]<CG_9L && data[i-1][TL2_RANK_NO]<CG_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO]){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO]=data[i-1][TL2_RANK_NO]+1;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == CG_SA && data[i][TL2_RANK_NO] < CG_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = CG_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == CG_MA && data[i][TL2_RANK_NO] < CG_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = CG_L;              //[L]にする
                            }
                        }else if(data[i][TL2_RANK_NO]<CG_RANK_START && data[i-1][TL2_RANK_NO]>CG_RANK_START){  //1個前が緑で今は赤の場合
                            if(data[i][TL2_RANK_NO]<C_9L && data[i-1][TL2_RANK_NO]<CG_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO] - matrix_RG_SA){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO]=data[i-1][TL2_RANK_NO]+1 - matrix_RG_SA;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == CG_SA && data[i][TL2_RANK_NO] < C_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = C_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == CG_MA && data[i][TL2_RANK_NO] < C_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = C_L;              //[L]にする
                            }
                        }else if(data[i][TL2_RANK_NO]>CG_RANK_START && data[i-1][TL2_RANK_NO]<CG_RANK_START) {  //1個前が赤で今は緑の場合
                            if(data[i][TL2_RANK_NO]<CG_9L && data[i-1][TL2_RANK_NO]<C_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO] + matrix_RG_SA){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO]=data[i-1][TL2_RANK_NO]+1 + matrix_RG_SA;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == C_SA && data[i][TL2_RANK_NO] < CG_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = CG_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == C_MA && data[i][TL2_RANK_NO] < CG_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = CG_L;              //[L]にする
                            }
                            //pc.printf("b0:%d\r\n",data[i][TL2_RANK_NO]);
                        }
                    }
                } else if(i == u_num || i == u_num + 1) {
                    set_en=2;
                } else {
                    set_en=0;
                }
            } else if((data_KUMI_FLAG == KUMI_KAISU1) && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) { //マトリクスのランククミアワセだった場合
                if(i == 0) { //2L以上のｾｯﾃｲ時に、2Lもしくはその1個前がOFFだったらすっ飛ばす
                    set_en=0;
                } else if(i == 1 && j == TL2_RANK_EN) { //ランク[M]は無いと全部OFFにもできるので、絶対ENにする
                    data[i][TL2_RANK_EN] = 1;
                    set_en=3;//設定値は強制
                } else if(i > 2 && i < u_num && (data[2][TL2_RANK_EN]==0 || data[i-1][TL2_RANK_EN]==0 )) { //2個目以降のｾｯﾃｲ時に、2個目もしくはｋ今回の1個前がOFFだったらすっ飛ばす
                    set_en=0;
                } else if(i <= 2 && (j == TL2_RANK_EN)) { //この項目だったら設定する
                    set_en=1;
                } else if(i==1   && j == TL2_RANK_MIN && data[i-1][TL2_RANK_EN]==0 && data[i][TL2_RANK_EN]==1) { //M以下で、1個前がOFF、今はONなら、min発動
                    set_en=0;
                } else if(i==1   && j == TL2_RANK_MAX) { //M以下で、1個前がOFF、今はONなら、1個前のMAXデータを修正する
                    set_en=0;
                } else if(j == TL2_RANK_NO && data[i][TL2_RANK_EN]==1){
                    set_en=1;
                    if(i>0){
                        if(data[i][TL2_RANK_NO]<CG_RANK_START && data[i-1][TL2_RANK_NO]<CG_RANK_START){    //両方と赤だった場合
                            if(data[i][TL2_RANK_NO] < C_9L && data[i-1][TL2_RANK_NO] < C_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO]){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO] = data[i-1][TL2_RANK_NO]+1;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == C_SA && data[i][TL2_RANK_NO] < C_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = C_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == C_MA && data[i][TL2_RANK_NO] < C_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = C_L;              //[L]にする
                            }
                        }else if(data[i][TL2_RANK_NO]>CG_RANK_START && data[i-1][TL2_RANK_NO]>CG_RANK_START){  //両方と緑だった場合
                            if(data[i][TL2_RANK_NO]<CG_9L && data[i-1][TL2_RANK_NO]<CG_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO]){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO]=data[i-1][TL2_RANK_NO]+1;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == CG_SA && data[i][TL2_RANK_NO] < CG_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = CG_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == CG_MA && data[i][TL2_RANK_NO] < CG_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = CG_L;              //[L]にする
                            }
                        }else if(data[i][TL2_RANK_NO]<CG_RANK_START && data[i-1][TL2_RANK_NO]>CG_RANK_START){  //1個前が緑で今は赤の場合
                            if(data[i][TL2_RANK_NO]<C_9L && data[i-1][TL2_RANK_NO]<CG_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO] - matrix_RG_SA){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO]=data[i-1][TL2_RANK_NO]+1 - matrix_RG_SA;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == CG_SA && data[i][TL2_RANK_NO] < C_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = C_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == CG_MA && data[i][TL2_RANK_NO] < C_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = C_L;              //[L]にする
                            }
                        }else if(data[i][TL2_RANK_NO]>CG_RANK_START && data[i-1][TL2_RANK_NO]<CG_RANK_START) {  //1個前が赤で今は緑の場合
                            if(data[i][TL2_RANK_NO]<CG_9L && data[i-1][TL2_RANK_NO]<C_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO] + matrix_RG_SA){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO]=data[i-1][TL2_RANK_NO]+1 + matrix_RG_SA;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == C_SA && data[i][TL2_RANK_NO] < CG_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = CG_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == C_MA && data[i][TL2_RANK_NO] < CG_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = CG_L;              //[L]にする
                            }
                            //pc.printf("b0:%d\r\n",data[i][TL2_RANK_NO]);
                        
                        }
                    }
                } else if(j == TL2_KUMI_MIN  && data[i][TL2_RANK_EN]==1){
                    set_en=1;
                } else if(j == TL2_TARGET  && data[i][TL2_RANK_EN]==1){
                    set_en=1;
                } else if(1 <= i && i <= 2 && TL2_JOUGEN  && data[i][TL2_RANK_EN]==1){
                    set_en=0;
                    //if(data[i][TL2_TARGET] > data[i][TL2_JOUGEN]){
                       // data[i][TL2_JOUGEN] = data[i][TL2_TARGET]+20;
                        data[i][TL2_JOUGEN] = int((float(data[i][TL2_TARGET])*1.05+4.0)/5)*5;
                    //}.33
                } else if(i == u_num || i == u_num + 1) {
                    set_en=2;
                } else {
                    set_en=0;
                }
            } else if(data_KUMI_FLAG == KUMI_RANK && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) { //マトリクスのランククミアワセだった場合
                if(i == 0) { //2L以上のｾｯﾃｲ時に、2Lもしくはその1個前がOFFだったらすっ飛ばす
                    set_en=0;
                } else if(i == 1 && j == TL2_RANK_EN) { //ランク[M]は無いと全部OFFにもできるので、絶対ENにする
                    data[i][TL2_RANK_EN] = 1;
                    set_en=3;//設定値は強制
                } else if(i > 2 && i < u_num && (data[2][TL2_RANK_EN]==0 || data[i-1][TL2_RANK_EN]==0 )) { //2個目以降のｾｯﾃｲ時に、2個目もしくはｋ今回の1個前がOFFだったらすっ飛ばす
                    set_en=0;
                } else if(i==1 && j == TL2_RANK_MIN && data[i-1][TL2_RANK_EN]==0 && data[i][TL2_RANK_EN]==1) { //M以下で、1個前がOFF、今はONなら、min発動
                    data[0][TL2_RANK_MIN]=param.CUT+1;
                    //data[i-1][TL2_RANK_MIN]=param.CUT+1;
                    //data[i-1][TL2_RANK_MAX]=param.CUT+1;
                    if( data[i-1][TL2_RANK_MIN]==data[i][TL2_RANK_MIN]) {
                        data[i][TL2_RANK_MIN]=data[i][TL2_RANK_MIN]+1;
                    }
                    set_en=1;
                } else if(i==1 && j == TL2_RANK_MAX) { //M以下で、1個前がOFF、今はONなら、1個前のMAXデータを修正する
                    data[0][TL2_RANK_MIN]=param.CUT+1;
                    data[0][TL2_RANK_MAX]=data[1][TL2_RANK_MIN];
                    set_en=1;
                } else if(j == TL2_RANK_NO && data[i][TL2_RANK_EN]==1){
                    set_en=1;
                    if(i>0){
                        if(data[i][TL2_RANK_NO]<CG_RANK_START && data[i-1][TL2_RANK_NO]<CG_RANK_START){    //両方と赤だった場合
                            if(data[i][TL2_RANK_NO] < C_9L && data[i-1][TL2_RANK_NO] < C_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO]){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO] = data[i-1][TL2_RANK_NO]+1;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == C_SA && data[i][TL2_RANK_NO] < C_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = C_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == C_MA && data[i][TL2_RANK_NO] < C_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = C_L;              //[L]にする
                            }
                        }else if(data[i][TL2_RANK_NO]>CG_RANK_START && data[i-1][TL2_RANK_NO]>CG_RANK_START){  //両方と緑だった場合
                            if(data[i][TL2_RANK_NO]<CG_9L && data[i-1][TL2_RANK_NO]<CG_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO]){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO]=data[i-1][TL2_RANK_NO]+1;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == CG_SA && data[i][TL2_RANK_NO] < CG_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = CG_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == CG_MA && data[i][TL2_RANK_NO] < CG_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = CG_L;              //[L]にする
                            }
                        }else if(data[i][TL2_RANK_NO]<CG_RANK_START && data[i-1][TL2_RANK_NO]>CG_RANK_START){  //1個前が緑で今は赤の場合
                            if(data[i][TL2_RANK_NO]<C_9L && data[i-1][TL2_RANK_NO]<CG_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO] - matrix_RG_SA){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO]=data[i-1][TL2_RANK_NO]+1 - matrix_RG_SA;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == CG_SA && data[i][TL2_RANK_NO] < C_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = C_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == CG_MA && data[i][TL2_RANK_NO] < C_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = C_L;              //[L]にする
                            }
                        }else if(data[i][TL2_RANK_NO]>CG_RANK_START && data[i-1][TL2_RANK_NO]<CG_RANK_START) {  //1個前が赤で今は緑の場合
                            if(data[i][TL2_RANK_NO]<CG_9L && data[i-1][TL2_RANK_NO]<C_9L && data[i][TL2_RANK_NO] < data[i-1][TL2_RANK_NO] + matrix_RG_SA){    //両方ともランク(9S~9L）内且つ、1個前のランクより低かったら
                                data[i][TL2_RANK_NO]=data[i-1][TL2_RANK_NO]+1 + matrix_RG_SA;              //1個前より1個上のランクにする
                            }else if(data[i-1][TL2_RANK_NO] == C_SA && data[i][TL2_RANK_NO] < CG_S){  //1個前が[SA]で、今が[S]以下だったら
                                data[i][TL2_RANK_NO] = CG_M;              //[S]にする
                            }else if(data[i-1][TL2_RANK_NO] == C_MA && data[i][TL2_RANK_NO] < CG_M){  //1個前が[MA]で、今が[ML]以下だったら
                                data[i][TL2_RANK_NO] = CG_L;              //[L]にする
                            }
                            //pc.printf("b0:%d\r\n",data[i][TL2_RANK_NO]);
                        
                        }
                    }
                
                } else if(i <= 9 && (j == TL2_RANK_EN || j == TL2_RANK_MAX)) { //この項目だったら設定する
                    //if(i <= 7 && (j == TL2_RANK_EN || (i == 1 && j == TL2_RANK_MIN) || j == TL2_RANK_MAX)){//この項目だったら設定する
                    set_en=1;
                } else if(i == u_num || i == u_num + 1) {
                    set_en=2;
                } else {
                    set_en=0;
                }
            }
            //******************************************************************************************
            //******************************************************************************************
            //******************************************************************************************
            //******不使用な内容の場合の、初期値を入れておく部分
            //******************************************************************************************
            if(set_en==0&&i < u_num) { //設定しない内容なら飛ばす
                if(j == TL2_RANK_EN) { //0=不使用
                    data[i][TL2_RANK_EN] = 0;
                    if(i>0) {
                        data[i][TL2_RANK_MIN] = data[i-1][TL2_RANK_MAX];
                        data[i][TL2_RANK_MAX] = data[i][TL2_RANK_MIN];
                    } else {
                        data[i][TL2_RANK_MIN] = param.CUT+1;
                        data[i][TL2_RANK_MAX] = param.CUT+1;
                    }
                    data[i][TL2_KUMI_MIN] = 99;
                    data[i][TL2_KUMI_MAX] = 99;
                    data[i][TL2_TARGET] = 99999;
                    data[i][TL2_JOUGEN] = 99999;
                } else if(j == TL2_RANK_MIN) { //0=不使用
                    if(i>0) {
                        data[i][TL2_RANK_MIN] = data[i-1][TL2_RANK_MAX];
                    } else {
                        data[i][TL2_RANK_MIN] = param.CUT+1;
                    }
                    if(data[i][TL2_RANK_MIN]>data[i][TL2_RANK_MAX]) {
                        data[i][TL2_RANK_MAX]=data[i][TL2_RANK_MIN];
                    }
                } else if(j == TL2_RANK_MAX) { //0=不使用
                    if(i>0) {
                        data[i][TL2_RANK_MAX] = data[i][TL2_RANK_MIN];
                    } else {
                        data[i][TL2_RANK_MAX] = param.CUT+1;
                    }
                } else if(j == TL2_JOUGEN) { //0=不使用
                    if((data_KUMI_FLAG == KUMI_KAISU1 || data_KUMI_FLAG == KUMI_KAISU2)){
                        if(data[i][TL2_RANK_EN] == 1){
                            //data[i][TL2_JOUGEN] = data[i][TL2_TARGET] + 20;
                            data[i][TL2_JOUGEN] = int((float(data[i][TL2_TARGET])*1.05+4.0)/5)*5;
                        }else{
                             data[i][TL2_JOUGEN] = 0;
                        }
                    }
                }else{
                    data[i][j]=0;
                }
                //printf("%2d:%2d: %4d  %4d\r\n",i,j,data[i][TL2_RANK_MIN],data[i][TL2_RANK_MAX]);
            } else if(set_en>=1/* && i < u_num*/) {
                //******************************************************************************************
                //******************************************************************************************
                //******************************************************************************************
                //******設定上限データ指定
                //******************************************************************************************
                if(i < u_num){
                    btn.old_btn();  //スイッチ更新(ここ以外で入れると不使用項目のスキップができない)
                    if(j == TL2_RANK_EN) {
                        limitData.min = 0;
                        limitData.max = 1;
                        limitData.lcd = 1;
                        mode = TL_RANK_ON_OFF;
                    }
                    if(j == TL2_RANK_NO) {
                        //pc.printf("bNo:%d\r\n",data[i][j]);
                        if(data[i][j] >= CG_9S){
                            rank_RG=1;
                            data[i][j] = data[i][j] - matrix_RG_SA;
                        }else{
                            rank_RG=0;
                        }
                        //pc.printf("b:%d\r\n",rank_RG);
                        limitData.min = C_9S;
                        limitData.max = CR_99;
                        limitData.lcd = 1;
                        mode = TL_END;
                        if(data[i][TL2_RANK_NO] < limitData.min){
                            data[i][TL2_RANK_NO] = limitData.min;
                        }else if(limitData.max < data[i][TL2_RANK_NO]){
                            data[i][TL2_RANK_NO] = limitData.max;
                        }
                    }
                    if(j >= TL2_RANK_MIN && data[i][TL2_RANK_EN] == 0) {
                    } else {
                        switch(j) {
                            case(TL2_RANK_MIN):
                                limitData = file.get_limit_data(TL_RANK_MIN);                     //ﾗﾝｸの重量
                                if(i>0) {
                                    //limitData.min = data[i-1][TL2_RANK_MAX]+1;
                                    limitData.min = data[i-1][TL2_RANK_MAX];
                                }
                                mode = TL_RANK_MIN;
                                break;
                            case(TL2_RANK_MAX):
                                limitData = file.get_limit_data(TL_RANK_MAX);                     //ﾗﾝｸの重量
                                limitData.min = data[i][TL2_RANK_MIN]+1;
                                mode = TL_RANK_MAX;
                                break;
                            case(TL2_KUMI_MIN):
                                if(data_KUMI_FLAG == KUMI_KAIKYU){
                                    limitData = file.get_limit_data(TL_KUMI_MIN);                   //組合せ数量
                                    if(data[i][TL2_KUMI_MIN] >= product.head) {
                                        data[i][TL2_KUMI_MIN] = 0;
                                    }
                                }else if((data_KUMI_FLAG == KUMI_KAISU1 || data_KUMI_FLAG == KUMI_KAISU2)){
                                    limitData.min = 1;
                                    limitData.max = 50;
                                    if(data_KUMI_FLAG == KUMI_KAISU2){ 
                                        if(i==1 || i==3){
                                            limitData.min = 2;
                                        }else if(i==2 || i==4){
                                            limitData.max = data[i-1][TL2_KUMI_MIN]-1;
                                        }
                                    }
                                }
                                mode = TL_KUMI_MIN;
                                break;
                            case(TL2_KUMI_MAX):
                                limitData = file.get_limit_data(TL_KUMI_MAX);
                                limitData.min = data[i][TL2_KUMI_MIN];
                                if(data[i][TL2_KUMI_MAX] >= product.head) {
                                    data[i][TL2_KUMI_MAX] = 0;
                                }
                                mode = TL_KUMI_MAX;
                                break;
                            case(TL2_TARGET):
                                if(data_KUMI_FLAG == KUMI_KAIKYU){
                                    limitData = file.get_limit_data(TL_TARGET);                   //組合せｊ重量
                                    limitData.min = data[i][TL2_RANK_MIN] * (data[i][TL2_KUMI_MIN]+1)+1;
                                    limitData.max = data[i][TL2_RANK_MAX] * (data[i][TL2_KUMI_MAX]+1)-1;
                                    if(limitData.min < data[i][TL2_RANK_MIN]*(data[i][TL2_KUMI_MIN]+1)) { //最小の値(最小重量×最小組数)より最小組合わせ重量が小さい＝ありえない
                                        limitData.min = data[i][TL2_RANK_MIN]*(data[i][TL2_KUMI_MIN]+1);
                                    }
                                    if(data[i][TL2_TARGET]>limitData.max || data[i][TL2_TARGET]<limitData.min) {
                                        data[i][TL2_TARGET] = limitData.min;
                                    }
                                }else if((data_KUMI_FLAG == KUMI_KAISU1 || data_KUMI_FLAG == KUMI_KAISU2)){
                                    limitData.min = param.CUT * data[i][TL2_KUMI_MIN];
                                    limitData.max = 600;
                                }
                                mode = TL_TARGET;
                                break;
                            case(TL2_JOUGEN):
                                if(data_KUMI_FLAG == KUMI_KAIKYU){
                                    limitData = file.get_limit_data(TL_JOUGEN);                   //組合せｊ重量
                                    limitData.min = data[i][TL2_TARGET];
                                    limitData.max = data[i][TL2_RANK_MAX] * (data[i][TL2_KUMI_MAX]+1);
                                    if(data[i][TL2_KUMI_MIN] == 0 && data[i][TL2_TARGET]<data[i][TL2_RANK_MAX]) {                 //ランク内の最大重量より小さい値が最小組合せ重量になっている＝上限が０だとランク内で組み合わせが入らない状態になる
                                        limitData.min = data[i][TL2_TARGET];
                                        limitData.max = data[i][TL2_RANK_MAX] * (data[i][TL2_KUMI_MAX]+1);
                                    }
                                    if(data[i][TL2_JOUGEN]>limitData.max || data[i][TL2_JOUGEN]<limitData.min) {
                                        data[i][TL2_JOUGEN] = limitData.max;
                                    }
                                }else if((data_KUMI_FLAG == KUMI_KAISU1 || data_KUMI_FLAG == KUMI_KAISU2)){
                                    limitData.min = data[i][TL2_TARGET];
                                    limitData.max = 800;
                                }
                                mode = TL_JOUGEN;
                                break;
                        }//switch
    
                        //dammy データだったら目標設定項目(最大・最小の設定値反映等)
                        if(limitData.lcd == dammy) {
                            //エラー対策
                            //6ヘッドだった場合には、制限有りのmax値= head数までとなる
                            if(j == TL2_KUMI_MIN || j == TL2_KUMI_MAX) {
                                if(limitData.max >= product.head) {
                                    limitData.max = product.head -1;
                                }
                                //if(limitData.max < data[i][j]){
                                //    data[i][j] = limitData.max
                                //}
                                //組み合わせ方法(最大)のとき設定範囲(最小値)を変更
                                if(j == TL2_KUMI_MAX) {
                                    limitData.min = data[i][TL2_KUMI_MIN];
                                }
                            }
                        }//if dammy
                    }//data[i][TL2_RANK_EN] != 0
                }
                //******************************************************************************************
                //******************************************************************************************
                //******************************************************************************************
                //******不使用な内容の場合の、初期値を入れておく部分
                //******************************************************************************************
                if(set_en==1 && j >= TL2_RANK_MIN && data[i][TL2_RANK_EN] == 0 && i < u_num) {
                    //*****ENが0＝現在のランクは使用しない
                    //*****不要なデータの初期化
                    switch(j) {
                        case(TL2_RANK_MIN):
                            if(data[i][TL2_RANK_MIN] <= param.CUT) {
                                data[i][TL2_RANK_MIN] = param.CUT+1;
                            }
                            mode = TL2_RANK_MIN;
                            break;
                        case(TL2_RANK_MAX):
                            data[i][TL2_RANK_MAX] = data[i][TL2_RANK_MIN];
                            if(data[i][TL2_RANK_MAX] <= param.CUT) {
                                data[i][TL2_RANK_MAX] = param.CUT+1;
                            }
                            mode = TL2_RANK_MAX;
                            break;
                        case(TL2_KUMI_MIN):
                            data[i][TL2_KUMI_MIN] = 99;
                            mode = TL2_KUMI_MIN;
                            break;
                        case(TL2_KUMI_MAX):
                            data[i][TL2_KUMI_MAX] = 99;
                            mode = TL2_KUMI_MAX;
                            break;
                        case(TL2_TARGET):
                            data[i][TL2_TARGET] = 99999;
                            mode = TL2_TARGET;
                            break;
                        case(TL2_JOUGEN):
                            data[i][TL2_JOUGEN] = 99999;
                            mode = TL2_JOUGEN;
                            break;
                    }//switch
                    if(i>0) {
                        data[i][TL2_RANK_MIN] = data[i-1][TL2_RANK_MAX];
                    }
                    //printf("%2d:%2d: %4d  %4d\r\n",i,j,data[i][TL2_RANK_MIN],data[i][TL2_RANK_MAX]);
                } else /*if(i < u_num )*/ {
                    //******************************************************************************************
                    //******************************************************************************************
                    //******************************************************************************************
                    //******使用する場合のLCD表示設定(項目によってはﾗﾝｸ表記が変わるため)
                    //******************************************************************************************
                    if(set_en==1&&i < u_num ) {
                        //************************表示**********************************************************
                        lcd.locate(0,0);
                        lcd.printf("                ");
                        lcd.locate(0,0);
                        if(data_KUMI_FLAG == KUMI_KAIKYU) {
                            switch(i) {
                                case(0):
                                    lcd.printf("[2S]");
                                    break;
                                case(1):
                                    lcd.printf("[ S]");
                                    break;
                                case(2):
                                    lcd.printf("[ M]");
                                    break;
                                case(3):
                                    lcd.printf("[ L]");
                                    break;
                                case(4):
                                    lcd.printf("[2L]");
                                    break;
                                default:
                                    lcd.printf("[--]");
                                    break;
                            }
                        } else if((/*j == TL2_RANK_EN || */j == TL2_RANK_MIN || j == TL2_RANK_MAX /*|| j >= TL2_RANK_MAX*/) && 
                                  (data_KUMI_FLAG == KUMI_RANK || (data_KUMI_FLAG == KUMI_KAISU1 || data_KUMI_FLAG == KUMI_KAISU2))&& 
                                  (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                            if(data[i][TL2_RANK_NO] >= CG_9S){
                                rank_RG=1;
                            }else{
                                rank_RG=0;
                            }
                            if(CR_01<=(data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)&&(data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)<=CR_99){
                                lcd.printf("[%2d]",(data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)-(CR_01-1));
                            }else if(C_9S<=(data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)&&(data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)<=C_MA){
                                if(C_9S<=(data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)&&(data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)<=C_2S){
                                    lcd.printf("[%dS]",C_S-(data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)+1);
                                }else if(C_2L<=(data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)&&(data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)<=C_9L){
                                    lcd.printf("[%dL]",(data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)-C_L+1);
                                }else{
                                    switch((data[i][TL2_RANK_NO] - rank_RG*matrix_RG_SA)) {
                                        case(C_S):
                                            lcd.printf("[ S]");
                                            break;
                                        case(C_M):
                                            lcd.printf("[ M]");
                                            break;
                                        case(C_L):
                                            lcd.printf("[ L]");
                                            break;
                                        case(C_SA):
                                            lcd.printf("[SA]");
                                            break;
                                        case(C_MA):
                                            lcd.printf("[MA]");
                                            break;
                                    }
                                }
                            }
                        }
                        //if((data_KUMI_FLAG == KUMI_KAISU || data_KUMI_FLAG == KUMI_KAISU2 || load->kumi_flag == KUMI_KAISU3) && j == TL2_KUMI_MIN){
                        //    
                        //}else{
                        if(data_KUMI_FLAG == KUMI_KAISU2){
                            lcd.setTitle_kaisu3(i ,j);
                        }else{
                            lcd.setTitle2(j);                        //設定項目表示
                        }
                        wait(0.1);
                        //lcd.locate(11,0);
                        //printf("01 %2d  : %2d\r\n",i,j);
                        if(j == TL2_RANK_EN){
                            Settei_suru_sinai(&data[i][j], limitData,mode,data_KUMI_FLAG);
                        }else if(/*j == TL2_RANK_EN || */j == TL2_KUMI_MIN || j == TL2_KUMI_MAX) {
                            //printf("010 %2d  : %2d\r\n",i,j);
                            if(data_KUMI_FLAG == KUMI_KAIKYU){
                                SetteiMokuhyo2(&data[i][j], limitData,mode,data_KUMI_FLAG);
                            }else if((data_KUMI_FLAG == KUMI_KAISU1 || data_KUMI_FLAG == KUMI_KAISU2)){
                                lcd.blank(1);
                                Chenge_vaule( &data[i][j], limitData);
                            }else{
                                SetteiMokuhyo2(&data[i][j], limitData,mode,data_KUMI_FLAG);
                            }
                            //printf("011 %2d  : %2d\r\n",i,j);
                        } else if(j == TL2_RANK_NO) {
                            lcd.locate(0,0);
                            lcd.printf("                ");
                            lcd.locate(0,0);
                            lcd.setTitle2(j);                        //設定項目表示
                            Setteirank(&data[i][j], limitData,mode,data_KUMI_FLAG,i,data[1][j],data[2][j],data[3][j],data[4][j],data[5][j],data[6][j],data[7][j],data[8][j],data[9][j]);

                            if(btn.get_btn() == BTN_ESC){
                                if(rank_RG == 1){
                                    data[i][j] = data[i][j] + matrix_RG_SA;
                                }
                            }else{
                            //pc.printf("a:%d\r\n",data[i][j]);
                            //if(data[i][TL2_RANK_EN] == 1){
                                
                                if(data[i][j] >= CG_9S){
                                    data[i][j] = data[i][j] - matrix_RG_SA;
                                    rank_RG = 1;
                                }
                                if( data_KUMI_FLAG != KUMI_KAISU1 && data_KUMI_FLAG != KUMI_KAISU2){
                                    lcd.locate(0,0);
                                    lcd.printf("                ");
                                    lcd.locate(0,0);
                                    lcd.printf(rank_color);
                                    rank_RG = RorG_result(rank_RG , data[i][j]);//次のランクの色を赤or緑に変更する
                                    data[i][j] = data[i][j] + rank_RG * matrix_RG_SA;
                                }
                            //}
                            }
                        } else {
                            lcd.locate(0,1);
                            lcd.printf("                ");
                            lcd.locate(0,1);

                            //単位を表示(g secなど)
                            lcd.locate(8,1);
                            //目標重量
                            if(j == TL2_RANK_MIN || j == TL2_RANK_MAX || j == TL2_JOUGEN  || j == TL2_TARGET) {
                                lcd.msgPrintf(DIS_G);
                                if(j == TL2_RANK_MAX) {
                                    lcd.locate(6,1);
                                    //lcd.printf(".9");
                                    lcd.locate(0,1);
                                } else if(j == TL2_RANK_MIN) {
                                    lcd.locate(6,1);
                                    //lcd.printf(".0");
                                    lcd.locate(0,1);
                                }
                            }
                            Chenge_vaule( &data[i][j], limitData);
                            lcd.locate(0,1);
                        }
                        //printf("%2d:%2d: %4d  %4d\r\n",i,j,data[i][TL2_RANK_MIN],data[i][TL2_RANK_MAX]);
                    } else if(set_en==2) {
                        //************************全体総括設定*********************************************************
                        //昇順ソートか降順ソートか
                        //今は未完(というか載せた時に成立するからソートも何もない)
                        /*
                        if(i==u_num && j==koumoku-1 && data_KUMI_FLAG == KUMI_KAIKYU) {
                            limitData.min = 0;
                            limitData.max = 2;
                            limitData.lcd = 1;
                            mode = TL2_END;
                            lcd.locate(0,0);
                            lcd.printf("                ");
                            lcd.locate(0,0);
                            lcd.printf(kumiyu1);
                            wait(0.1);
                            SetteiMokuhyo2(&data_p, limitData,mode,data_KUMI_FLAG);
                            if(btn.esc()) {
                                //i= 3;
                                //j= TL2_JOUGEN;
                                //btn.force_ESC();
                            }
                        } else {
                            data_p = 0;
                        }*/
                        //出力データの分け方
                        if(i==u_num && j==koumoku-1 && data_KUMI_FLAG == KUMI_RANK && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                            limitData.min = 0;
                            if(product.head <= 6) {
                                limitData.max = 2;
                            } else {
                                limitData.max = 5;
                            }
                            /*if(product.limit_on==SELECTOR_LIMIT){
                                data_r = 0;
                                if(btn.esc()) {
                                    i= 99;
                                    j= 99;
                                }else{
                                    i++;
                                }
                            }else{*/
                            limitData.lcd = 1;
                            mode = TL2_END;
                            lcd.locate(0,0);
                            lcd.printf("                ");
                            lcd.locate(0,0);
                            lcd.printf(kumir1);
                            wait(0.1);
                            
                            if(product.use_usb==1){
                                SetteiMokuhyo2(&data_r, limitData,mode,data_KUMI_FLAG);
                            }
                            if(btn.esc()) {
                                //i=6;
                                //j= TL2_JOUGEN;
                                //btn.force_ESC();
                            }
                            /*}*/
                        } else {
                            //data_p = 0;
                        }

                        if(i==u_num+1) {
                            //名前を入力。戻るを押されてたら上の項目に戻る
                            if(set_name(headPosition)!=true) {
                                /*
                                if( data_KUMI_FLAG == KUMI_KAIKYU){
                                    i= 3;
                                    //j= TL2_JOUGEN;
                                }else if(data_KUMI_FLAG == KUMI_RANK && (product.productType == MATRIX || product.productType == MATRIX_D || product.productType == ITIGO || product.productType == ITIGO_D)){
                                    i=6;
                                    //j= TL2_JOUGEN;
                                    //i -= 1;
                                    //j= TL2_JOUGEN-1;
                                    //btn.force_ESC();
                                }*/
                            } else {     //名前入力で設定値は全て変更済みなはずなので、項目類を全て終わらせる
                                i= 99;
                                j= 99;
                            }
                        }
                        set_en=0;
                    }
                    //printf("11  %d  %d  %d\r\n",i,j,set_en);
                }//data[i][TL2_RANK_EN] != 0

                //printf("12  %d  %d  %d\r\n",i,j,set_en);
            }//各項目のEN信号
            //******************************************************************************************
            //******************************************************************************************
            //******************************************************************************************
            //******戻る/決定ボタンが押された場合,項目を変更する
            //******************************************************************************************
            //old_i=i;
            if(i !=99 &&(btn.esc()|| btn.ent())) {
                int ch_value=0;     //項目を進めるか戻るか指示する値(1 or -1)
                if(btn.ent()) {
                    ch_value = 1;
                } else if(btn.esc()) {
                    ch_value = -1;
                }
                j = j + ch_value;
                //btn.puls_or_minus(j)
                if(j < 0) {
                    i -= 1;
                    j = koumoku -1;
                    if(i<0) {
                        if(data_KUMI_FLAG == KUMI_RANK && data_f_end == 1){
                            i = 0;  j = 0; data_f_end = 0; data_s_end = 0;
                        }else if(data_KUMI_FLAG == KUMI_RANK && data_s_end == 1){
                            i = 0;  j = 0; data_f_end = 0; data_s_end = 0;
                        }else{
                            return 1;
                        }
                    }
                } else if(j >= koumoku) {
                    i += 1;
                    j = 0;
                    if(i>u_num+1) {
                        i= 99;
                        j= 99;
                    }
                }
            }//btn.esc()|| btn.ent()
            //printf("13  %d  %d  %d\r\n",i,j,set_en);
        }//for(j)
        pc.printf("14  %d  %d  %d\r\n",i,j,set_en);
    }//for(i)
    //    pc.printf("zz \r\n");
    for(int i = 0; i < product.head; i++) {
        iro[i]=C_NULL;
    }
    for (int i = 0 ; i < u_num ; i++ ) {
        for(int j = 0 ; j < koumoku ; j++ ) {
            data2[j] = data[i][j];
        }
        file.set_lweight(data2, headPosition, i,data_f, data_r, data_s);
    }
    //file.set_lweight(data, headPosition);
    file.save_unitweight();      //変更されたデータを受け渡し

    return 0;
}
void preset_default(int i,int data[],int headPosition)
{
    switch(i) {
        case(TL_KATAMEN_HYOUJI):
            data[TL_KATAMEN_HYOUJI]  = 0;
            break;
        case(TL_COMBI_FLAG)://組合せ形態
            data[TL_COMBI_FLAG]      = 0;
            break;
        case(TL_KUMI_MIN)://組合せ数量(最小)
            data[TL_KUMI_MIN]    = 0;
            break;
        case(TL_KUMI_MAX)://組合せ数量(最大)
            if(data[TL_KUMI_FLAG] == KUMI_DENSO){   data[TL_KUMI_MAX]    = 1;}
            else{                                   data[TL_KUMI_MAX]    = 0;}
            break;
        case(TL_KUMI_PRIORITY_1)://組合せ優先1
            data[TL_KUMI_PRIORITY_1] = 99;
            break;
        case(TL_KUMI_PRIORITY_2)://組合せ優先2
            data[TL_KUMI_PRIORITY_2] = 99;
            break;
        case(TL_TARGET):
            data[TL_TARGET]          = 100+5;
            break;
        case(TL_JOUGEN):
            data[TL_JOUGEN]          = 30;
            break;
        case(TL_TAIMEN_TARGET):
            data[TL_TAIMEN_TARGET]   = 0;
            break;
        case(TL_TAIMEN_JOUGEN):
            data[TL_TAIMEN_JOUGEN]   = 0;
            break;
        case(TL_FUZOROI_BOUSHI):
            data[TL_FUZOROI_BOUSHI]  = 0;
            break;
        case(TL_SECOND_ON_OFF):
            if(product.limit_on==KUMI_ZANCOM || product.limit_on==KUMI_ZANCOM2){
                data[TL_SECOND_ON_OFF]   = 1;
                break;
            }else{
                data[TL_SECOND_ON_OFF]   = 0;
                break;
            }
        case(TL_SPMODE):
            if(data[TL_KUMI_FLAG] == KUMI_DENSO){   
                    data[TL_SPMODE]          = 0;}
            else if(data[TL_KUMI_FLAG] == KUMI_ZANCOM ||
                    data[TL_KUMI_FLAG] == KUMI_ZANCOM2){   
                    data[TL_SPMODE]          = 0;}
            //else if(data[TL_KUMI_FLAG] == KUMI_LIMIT ||
            //      data[TL_KUMI_FLAG] == KUMI_NONE    ){   
            //        data[TL_SPMODE]          = SPEED_DAMP;}
            else{                                   
                    data[TL_SPMODE]          = 1;}
            break;
        case(TL_HYOUJI):
            data[TL_HYOUJI]          = 30;
            break;
        case(TL_RANK_ON_OFF):
            data[TL_RANK_ON_OFF]     = 0;
            break;
        case(TL_RANK_MIN):
            data[TL_RANK_MIN]        = 0;
            break;
        case(TL_RANK_MAX):
            data[TL_RANK_MAX]        = 0;
            break;
        case(TL_CHECK_HYOUUJI):
            data[TL_CHECK_HYOUUJI]   = 8;
            break;
        case(TL_KURI_FLAG):
            data[TL_KURI_FLAG]       = 0;
            break;
        default:
            data[i]                  = 0;
            break;
    }
}
bool preset_use(int i,int data[]) //項目をすべて書き出し、使用する項目はtrue、他はfalseとする
{
    if(i==0) { //kumi_flugの設定時は無条件にtrue
        return true;
    }
    switch(data[TL_KUMI_FLAG]) {
        case(KUMI_NONE):
            if(product.limit_on==WIDE_LIMIT){
                switch(i) {
                    case(TL_TARGET):            return true;  //組合せ重量(min)
                    case(TL_JOUGEN):            return true;  //組合せ重量(max)
                    case(TL_SECOND_ON_OFF):     return true;
                    case(TL_SPMODE):            return true;
                    case(TL_HYOUJI):            return true;
                    case(TL_KURI_FLAG):         return true;
                    case(TL_TAIMEN_JOUGEN):
                            if(option.second_target == 1){ return true; }
                            else                         { return false;}
                    case(TL_TAIMEN_TARGET):
                    //case(TL_TAIMEN_JOUGEN):
                    case(TL_COMBI_FLAG):
                    case(TL_TANJU):
                    case(TL_KATAMEN_HYOUJI):
                    case(TL_KUMI_MIN)://組合せ数量(最小)
                    case(TL_KUMI_MAX)://組合せ数量(最大)
                    case(TL_KUMI_PRIORITY_1)://組合せ優先1
                    case(TL_KUMI_PRIORITY_2)://組合せ優先2
                    case(TL_FUZOROI_BOUSHI):
                    case(TL_RANK_ON_OFF):
                    case(TL_RANK_MIN):
                    case(TL_RANK_MAX):
                    case(TL_CHECK_HYOUUJI):
                    default:
                    return false;
                }
            }else{
                switch(i) {
                    case(TL_COMBI_FLAG):
                        if(product.Array==0) {
                            //return true;
                            return false;
                        } else {
                            return false;
                            //return true;
                        }
                    case(TL_KATAMEN_HYOUJI):
                        //if(product.Array!=0) {
                        if(product.Array==0) {
                            //return true;
                            return false;
                        } else {
                            return false;
                            //return true;
                        }
                    case(TL_TARGET):            return true;
                    case(TL_JOUGEN):            return true;
                    //case(TL_FUZOROI_BOUSHI):    return true;
                    case(TL_FUZOROI_BOUSHI):
                            if(TOP_USER  == true){    return true;  //不揃い防止
                            }else{                    return false; }
                    case(TL_SECOND_ON_OFF):     return true;
                    case(TL_SPMODE):            return true;
                    case(TL_KURI_FLAG):
                        if(data[TL_SPMODE]==SPEED_HIGH || data[TL_SPMODE]==SPEED_HYPER || data[TL_SPMODE]==SPEED_DAMP) {
                            return true;
                        } else {
                            return true;
                        }
                    case(TL_HYOUJI):
                        if(data[TL_SPMODE]==SPEED_HIGH || data[TL_SPMODE]==SPEED_HYPER || data[TL_SPMODE]==SPEED_DAMP) {
                            return true;
                        } else {
                            return true;
                        }
                    case(TL_RANK_ON_OFF):       return true;
                    case(TL_RANK_MIN):
                        if(data[TL_RANK_ON_OFF]==1 || data[TL_RANK_ON_OFF]==3) {
                            return true;
                        } else {
                            return false;
                        }
                    case(TL_RANK_MAX):
                        if(data[TL_RANK_ON_OFF]==2 || data[TL_RANK_ON_OFF]==3) {
                            return true;
                        } else {
                            return false;
                        }
    
                    case(TL_TAIMEN_JOUGEN):
                            if(option.second_target == 1){ return true; }
                            else                         { return false;}
                    
                    case(TL_TANJU):
                    case(TL_KUMI_MIN)://組合せ数量(最小)
                    case(TL_KUMI_MAX)://組合せ数量(最大)
                    case(TL_KUMI_PRIORITY_1)://組合せ優先1
                    case(TL_KUMI_PRIORITY_2)://組合せ優先2
                    case(TL_TAIMEN_TARGET):
                    //case(TL_TAIMEN_JOUGEN):
                    default:
                        return false;
                }
            }
        /*case(KUMI_COMBI):
            switch(i) {
                case(TL_COMBI_FLAG):        return true;
                case(TL_TARGET):            return true;
                case(TL_JOUGEN):            return true;
                case(TL_FUZOROI_BOUSHI):    return true;
                case(TL_SECOND_ON_OFF):     return true;
                case(TL_SPMODE):            return true;
                case(TL_HYOUJI):
                    if(data[TL_SPMODE]==SPEED_HIGH || data[TL_SPMODE]==SPEED_HYPER) {
                        return true;
                    } else {
                        return false;
                    }
                case(TL_RANK_ON_OFF):       return true;
                case(TL_RANK_MIN):
                    if(data[TL_RANK_ON_OFF]==1 || data[TL_RANK_ON_OFF]==3) {
                        return true;
                    } else {
                        return false;
                    }
                case(TL_RANK_MAX):
                    if(data[TL_RANK_ON_OFF]==2 || data[TL_RANK_ON_OFF]==3) {
                        return true;
                    } else {
                        return false;
                    }

                case(TL_KUMI_MIN)://組合せ数量(最小)
                case(TL_KUMI_MAX)://組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1)://組合せ優先1
                case(TL_KUMI_PRIORITY_2)://組合せ優先2
                case(TL_KATAMEN_HYOUJI):
                case(TL_TANJU):
                case(TL_TAIMEN_TARGET):
                case(TL_TAIMEN_JOUGEN):
                case(TL_CHECK_HYOUUJI):
                default:
                    return false;
            }*/
        case(KUMI_LIMIT):
            if(product.limit_on==WIDE_LIMIT){
                switch(i) {
                    case(TL_TARGET):            return true;  //組合せ重量(min)
                    case(TL_JOUGEN):            return true;  //組合せ重量(max)
                    case(TL_SECOND_ON_OFF):     return true;
                    case(TL_SPMODE):            return true;
                    case(TL_HYOUJI):            return true;
                    case(TL_KUMI_MIN):          return true;  //組合せ数量(最小)
                    case(TL_KUMI_MAX):          return true;  //組合せ数量(最大)
                    case(TL_KURI_FLAG):         return true;
                    case(TL_FUZOROI_BOUSHI):    return true;
                    case(TL_TAIMEN_JOUGEN):
                            if(option.second_target == 1){ return true; }
                            else                         { return false;}
                    case(TL_KUMI_PRIORITY_1)://組合せ優先1
                    case(TL_KUMI_PRIORITY_2)://組合せ優先2
                    case(TL_TAIMEN_TARGET):
                    //case(TL_TAIMEN_JOUGEN):
                    case(TL_COMBI_FLAG):
                    case(TL_TANJU):
                    case(TL_KATAMEN_HYOUJI):
                    ///////case(TL_FUZOROI_BOUSHI):
                    case(TL_RANK_ON_OFF):
                    case(TL_RANK_MIN):
                    case(TL_RANK_MAX):
                    case(TL_CHECK_HYOUUJI):
                    default:
                    return false;
                }
            }else{
                switch(i) {
                    case(TL_COMBI_FLAG):
                        if(product.head<12) {
                            return false;   //ｶﾀﾒﾝﾋｮｳｼﾞ
                        } else {
                            return true;
                        }
                    case(TL_KUMI_MIN):          return true;  //組合せ数量(最小)
                    case(TL_KUMI_MAX):          return true;  //組合せ数量(最大)
                    case(TL_KURI_FLAG):         return true;
                    case(TL_KUMI_PRIORITY_1):   return false;              //組合せ優先1
                        /*if(data[TL_KUMI_MIN] == data[TL_KUMI_MAX]) {
                            return false;
                        } else {
                            return true;
                        }*/
                    case(TL_KUMI_PRIORITY_2):   return false;                        //組合せ優先2
                        /*if(data[TL_KUMI_PRIORITY_1]==99) {
                            return false;
                        } else {
                            return true;
                        }*/
                    case(TL_FUZOROI_BOUSHI):    return true;
                    case(TL_KATAMEN_HYOUJI):
                        //if(product.Array!=0) {
                        if(product.Array==0) {
                            //return true;
                            return false;
                        } else {
                            return false;
                            //return true;
                        }
                    case(TL_TARGET):            return true;  //組合せ重量(min)
                    case(TL_JOUGEN):            return true;  //組合せ重量(max)
                    
                    //case(TL_FUZOROI_BOUSHI):
                    //        if(TOP_USER  == true){    return true;  //不揃い防止
                    //        }else{                    return false; }
                    
                    case(TL_SECOND_ON_OFF):     return true;  //準最適値
                    case(TL_SPMODE):            return true;
                    case(TL_HYOUJI):                                    //LED点灯時間
                        if(data[TL_SPMODE]==SPEED_HIGH || data[TL_SPMODE]==SPEED_HYPER) {
                            return true;
                        } else {
                            return true;
                        }
                    case(TL_RANK_ON_OFF):       return true;  //規格外設定(OFF,最大、最小、両方)
                    case(TL_RANK_MIN):                                  //規格外設定(最小)
                        if(data[TL_RANK_ON_OFF]==1 || data[TL_RANK_ON_OFF]==3) {
                            return true;
                        } else {
                            return false;
                        }
                    case(TL_RANK_MAX):                                  //規格外設定(最大)
                        if(data[TL_RANK_ON_OFF]==2 || data[TL_RANK_ON_OFF]==3) {
                            return true;
                        } else {
                            return false;
                        }
                    //case(TL_COMBI_FLAG):
                    case(TL_TANJU):
                    case(TL_TAIMEN_TARGET):
                    case(TL_TAIMEN_JOUGEN):
                    case(TL_CHECK_HYOUUJI):
                    default:
                        return false;
    
                }
            }
        case(KUMI_RANK):  //S+のﾗﾝｸｾﾝﾍﾞﾂは別プログラムなので放置(というか、ここに来ないはず) ※ﾃﾞﾓﾓｰﾄﾞ時にここに来る
            if(i> TL_KUMI_FLAG &&((data[TL_KUMI_FLAG] == KUMI_KAIKYU) ||
                                  (data[TL_KUMI_FLAG] == KUMI_RANK && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON || product.productType == ITIGO || product.productType == ITIGO_D)))) {
                if(demo_flg==false || product.head*2 < file.get_preset() ){ //デモモード且つﾃﾞﾓﾓｰﾄﾞのプリセット番号の場合はそのまま先に進んでほしいのでスル―(スル―しない条件を
                    if(i<=TL_KUMI_FLAG+1) { //FLAGを決定した次の項目でmultiに行くので、その分までtrueにする
                            return true;
                    } else {
                        return false;
                    }
                }else{
                    if(i==TL_KUMI_FLAG+1) { //FLAGを決定した次の項目でmultiに行くので、その分までtrueにする
                        return true;
                    }
                }
                    
            }
            switch(i) {
                case(TL_TARGET):            return true;  //ﾗﾝｸ内重量(min)
                case(TL_JOUGEN):            return true;  //ﾗﾝｸ内重量(max)
                case(TL_KURI_FLAG):         return true;
                //case(TL_COMBI_FLAG):
                case(TL_KATAMEN_HYOUJI):
                case(TL_KUMI_MIN)://組合せ数量(最小)
                case(TL_KUMI_MAX)://組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1)://組合せ優先1
                case(TL_KUMI_PRIORITY_2)://組合せ優先2
                case(TL_TAIMEN_TARGET):
                case(TL_TAIMEN_JOUGEN):
                case(TL_FUZOROI_BOUSHI):
                case(TL_SECOND_ON_OFF):
                case(TL_SPMODE):
                case(TL_HYOUJI):
                case(TL_RANK_ON_OFF):
                case(TL_RANK_MIN):
                case(TL_RANK_MAX):
                case(TL_TANJU):
                case(TL_CHECK_HYOUUJI):
                default:
                    return false;
            }
        case(KUMI_KAIKYU)://ﾗﾝｸﾍﾞﾂｸﾐｱﾜｾは別プログラムなので、すべて使用しない(というか、ここに来ないはず)
            if(i> TL_KUMI_FLAG &&((data[TL_KUMI_FLAG] == KUMI_KAIKYU) ||
                                  (data[TL_KUMI_FLAG] == KUMI_RANK && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON || product.productType == ITIGO || product.productType == ITIGO_D)))) {
                if(i==TL_KUMI_FLAG+1) { //FLAGを決定した次の項目でmultiに行くので、その分までtrueにする
                    return true;
                } else {
                    return false;
                }
            }
            switch(i) {
                case(TL_KURI_FLAG):         return true;
                case(TL_KUMI_MIN)://組合せ数量(最小)
                case(TL_KUMI_MAX)://組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1)://組合せ優先1
                case(TL_KUMI_PRIORITY_2)://組合せ優先2
                //case(TL_COMBI_FLAG):
                case(TL_KATAMEN_HYOUJI):
                case(TL_TANJU):
                case(TL_TARGET):
                case(TL_JOUGEN):
                case(TL_TAIMEN_TARGET):
                case(TL_TAIMEN_JOUGEN):
                case(TL_FUZOROI_BOUSHI):
                case(TL_SECOND_ON_OFF):
                case(TL_SPMODE):
                case(TL_HYOUJI):
                case(TL_RANK_ON_OFF):
                case(TL_RANK_MIN):
                case(TL_RANK_MAX):
                case(TL_CHECK_HYOUUJI):
                default:
                    return false;
            }
        case(KUMI_OYAKO):
            switch(i) {
                case(TL_KATAMEN_HYOUJI):
                    //if(product.Array!=0) {
                    if(product.Array==0) {
                        //return true;
                        return false;
                    } else {
                        return false;
                        //return true;
                    }
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_FUZOROI_BOUSHI):    return true;  //不揃い防止
                case(TL_SECOND_ON_OFF):     return true;  //準最適値
                case(TL_SPMODE):            return true;  //高速・ﾕｯｸﾘ
                case(TL_KURI_FLAG):         return true;
                case(TL_HYOUJI):                                    //LED点灯時間
                    if(data[TL_SPMODE]==SPEED_HIGH || data[TL_SPMODE]==SPEED_HYPER || data[TL_SPMODE]==SPEED_DAMP) {
                        return true;
                    } else {
                        return true;
                    }
                case(TL_RANK_ON_OFF):       return true;  //規格外設定(OFF,最大、最小、両方)
                case(TL_RANK_MIN):                                  //規格外設定(最小)
                    if(data[TL_RANK_ON_OFF]==1 || data[TL_RANK_ON_OFF]==3) {
                        return true;
                    } else {
                        return false;
                    }
                case(TL_RANK_MAX):                                  //規格外設定(最大)
                    if(data[TL_RANK_ON_OFF]==2 || data[TL_RANK_ON_OFF]==3) {
                        return true;
                    } else {
                        return false;
                    }
                case(TL_KUMI_MIN):    return true;  //組合せ数量(最小)    return true;  
                case(TL_KUMI_MAX):    return true;  //組合せ数量(最大)    return true;  
                case(TL_KUMI_PRIORITY_1):   return false;  //組合せ優先1    return true;  
                case(TL_KUMI_PRIORITY_2):    return false;  //組合せ優先2    return true;  
                case(TL_COMBI_FLAG):
                case(TL_TANJU):
                case(TL_TAIMEN_TARGET):
                case(TL_TAIMEN_JOUGEN):
                case(TL_CHECK_HYOUUJI):
                default:
                    return false;
            }
        case(KUMI_OYAKO_HALF):
            switch(i) {
                case(TL_KUMI_MIN):          return true;  //組合せ数量(最小)
                case(TL_KUMI_MAX):          return true;  //組合せ数量(最大)
                case(TL_KATAMEN_HYOUJI):    return false;   //ｶﾀﾒﾝﾋｮｳｼﾞ
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_FUZOROI_BOUSHI):    return true;  //不揃い防止
                case(TL_SECOND_ON_OFF):     return true;  //準最適値
                case(TL_SPMODE):            return true;  //高速・ﾕｯｸﾘ
                case(TL_KUMI_PRIORITY_1):    return true;  //組合せ優先1
                case(TL_KUMI_PRIORITY_2):    return true;  //組合せ優先2
                case(TL_KURI_FLAG):         return true;
                case(TL_HYOUJI):                                    //LED点灯時間
                    if(data[TL_SPMODE]==SPEED_HIGH || data[TL_SPMODE]==SPEED_HYPER || data[TL_SPMODE]==SPEED_DAMP) {
                        return true;
                    } else {
                        return true;
                    }
                case(TL_RANK_ON_OFF):       return true;  //規格外設定(OFF,最大、最小、両方)
                case(TL_RANK_MIN):                                  //規格外設定(最小)
                    if(data[TL_RANK_ON_OFF]==1 || data[TL_RANK_ON_OFF]==3) {
                        return true;
                    } else {
                        return false;
                    }
                case(TL_RANK_MAX):                                  //規格外設定(最大)
                    if(data[TL_RANK_ON_OFF]==2 || data[TL_RANK_ON_OFF]==3) {
                        return true;
                    } else {
                        return false;
                    }
                case(TL_COMBI_FLAG):
                case(TL_TANJU):
                case(TL_TAIMEN_TARGET):
                case(TL_TAIMEN_JOUGEN):
                case(TL_CHECK_HYOUUJI):
                default:
                    return false;
            }
        case(KUMI_TAIMEN ):
            switch(i) {
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_TAIMEN_TARGET):     return true;
                case(TL_TAIMEN_JOUGEN):     return true;
                case(TL_KURI_FLAG):         return true;
                case(TL_COMBI_FLAG):
                case(TL_TANJU):
                case(TL_KATAMEN_HYOUJI):
                case(TL_KUMI_MIN)://組合せ数量(最小)
                case(TL_KUMI_MAX)://組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1)://組合せ優先1
                case(TL_KUMI_PRIORITY_2)://組合せ優先2
                case(TL_FUZOROI_BOUSHI):
                case(TL_SECOND_ON_OFF):
                case(TL_SPMODE):
                case(TL_HYOUJI):
                case(TL_RANK_ON_OFF):
                case(TL_RANK_MIN):
                case(TL_RANK_MAX):
                case(TL_CHECK_HYOUUJI):
                default:
                    return false;
            }
        case(KUMI_KOSUU):
            switch(i) {
                case(TL_KUMI_MIN):          return false;//組合せ数量(最小)
                case(TL_KUMI_MAX):          return false;//組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1):   return false;//組合せ優先1
                case(TL_KUMI_PRIORITY_2):   return false;//組合せ優先2
                case(TL_COMBI_FLAG):        return false;
                case(TL_KATAMEN_HYOUJI):    return false;
                case(TL_TANJU):             return true;
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_TAIMEN_TARGET):     return false;
                case(TL_TAIMEN_JOUGEN):     return false;
                case(TL_FUZOROI_BOUSHI):    return false;
                case(TL_SECOND_ON_OFF):     return false;
                case(TL_SPMODE):            return false;
                case(TL_HYOUJI):            return false;
                case(TL_RANK_ON_OFF):       return false;
                case(TL_RANK_MIN):          return false;
                case(TL_RANK_MAX):          return false;
                case(TL_CHECK_HYOUUJI):     return false;
                case(TL_KURI_FLAG):         return false;
                default:
                    return false;
            }
        case(KUMI_JUURYO):
            switch(i){
                case(TL_KATAMEN_HYOUJI):    return false;
                case(TL_KUMI_MIN):          return false;//組合せ数量(最小)
                case(TL_KUMI_MAX):          return false;//組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1):   return false;//組合せ優先1
                case(TL_KUMI_PRIORITY_2):   return false;//組合せ優先2
                case(TL_TANJU):             return false;
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_TAIMEN_TARGET):     return false;
                case(TL_TAIMEN_JOUGEN):     return false;
                case(TL_FUZOROI_BOUSHI):    return false;
                case(TL_SECOND_ON_OFF):     return false;
                case(TL_SPMODE):            return false;
                case(TL_HYOUJI):            return false;
                case(TL_RANK_ON_OFF):       return false;
                case(TL_RANK_MIN):          return false;
                case(TL_RANK_MAX):          return false;
                case(TL_CHECK_HYOUUJI):     return false;
                case(TL_KURI_FLAG):         return false;
                default:
                    return false;
            }
        case(KUMI_KAISU1)://ﾗﾝｸﾍﾞﾂｸﾐｱﾜｾは別プログラムなので、すべて使用しない(というか、ここに来ないはず)
        case(KUMI_KAISU2)://ﾗﾝｸﾍﾞﾂｸﾐｱﾜｾは別プログラムなので、すべて使用しない(というか、ここに来ないはず)
        
            if(i> TL_KUMI_FLAG &&((data[TL_KUMI_FLAG] == KUMI_KAISU1 || data[TL_KUMI_FLAG] == KUMI_KAISU2) ||
                                  (data[TL_KUMI_FLAG] == KUMI_RANK && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON || product.productType == ITIGO || product.productType == ITIGO_D)))) {
                if(i==TL_KUMI_FLAG+1) { //FLAGを決定した次の項目でmultiに行くので、その分までtrueにする
                    return true;
                } else if (i==TL_FUZOROI_BOUSHI){
                    return true;
                } else if (i==TL_TAIMEN_TARGET){
                    return true;
                } else if (i==TL_SECOND_ON_OFF){
                    return true;
                } else if (i==TL_HYOUJI && data[TL_SECOND_ON_OFF]==1){
                    return true;
                } else {
                    return false;
                }
            }
            switch(i) {
                case(TL_FUZOROI_BOUSHI):
                case(TL_TAIMEN_TARGET):
                    return true;
                case(TL_KUMI_MIN)://組合せ数量(最小)
                case(TL_KUMI_MAX)://組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1)://組合せ優先1
                case(TL_KUMI_PRIORITY_2)://組合せ優先2
                //case(TL_COMBI_FLAG):
                case(TL_KATAMEN_HYOUJI):
                case(TL_TANJU):
                case(TL_TARGET):
                case(TL_JOUGEN):
                case(TL_TAIMEN_JOUGEN):
                case(TL_SECOND_ON_OFF):
                case(TL_SPMODE):
                case(TL_HYOUJI):
                case(TL_RANK_ON_OFF):
                case(TL_RANK_MIN):
                case(TL_RANK_MAX):
                case(TL_CHECK_HYOUUJI):
                case(TL_KURI_FLAG):         return false;
                default:
                    return false;
            }
        case(KUMI_PACK):
            switch(i){
                case(TL_KATAMEN_HYOUJI):    return false;
                case(TL_KUMI_MIN):          return true;//組合せ数量(最小)
                case(TL_KUMI_MAX):          return true;//組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1):   return false;//組合せ優先1
                case(TL_KUMI_PRIORITY_2):   return false;//組合せ優先2
                case(TL_TANJU):             return false;
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_TAIMEN_TARGET):     return false;
                case(TL_TAIMEN_JOUGEN):     return false;
                case(TL_FUZOROI_BOUSHI):    return false;
                case(TL_SECOND_ON_OFF):     return false;
                case(TL_SPMODE):            return false;
                case(TL_HYOUJI):            return false;
                case(TL_RANK_ON_OFF):       return false;
                case(TL_RANK_MIN):          return false;
                case(TL_RANK_MAX):          return false;
                case(TL_CHECK_HYOUUJI):     return false;
                case(TL_KURI_FLAG):         return false;
                default:
                    return false;
            }
        case(KUMI_PACK2):
            switch(i){
                case(TL_KATAMEN_HYOUJI):    return false;
                case(TL_KUMI_MIN):          return false;//組合せ数量(最小)
                case(TL_KUMI_MAX):          return true;//組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1):   return false;//組合せ優先1
                case(TL_KUMI_PRIORITY_2):   return false;//組合せ優先2
                case(TL_TANJU):             return false;
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_TAIMEN_TARGET):     return false;
                case(TL_TAIMEN_JOUGEN):     return false;
                case(TL_FUZOROI_BOUSHI):    return false;
                case(TL_SECOND_ON_OFF):     return false;
                case(TL_SPMODE):            return false;
                case(TL_HYOUJI):            return false;
                case(TL_RANK_ON_OFF):       return false;
                case(TL_RANK_MIN):          return false;
                case(TL_RANK_MAX):          return false;
                case(TL_CHECK_HYOUUJI):     return false;
                case(TL_KURI_FLAG):         return false;
                default:
                    return false;
            }
        case(KUMI_DENSO):
            switch(i){
                case(TL_KATAMEN_HYOUJI):    return false;
                case(TL_KUMI_MIN):          return false;//組合せ数量(最小)
                case(TL_KUMI_MAX):          return false;//組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1):   return false;//組合せ優先1
                case(TL_KUMI_PRIORITY_2):   return false;//組合せ優先2
                case(TL_TANJU):             return false;
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_TAIMEN_TARGET):     return false;
                case(TL_TAIMEN_JOUGEN):     return false;
                case(TL_FUZOROI_BOUSHI):    return false;
                case(TL_SECOND_ON_OFF):     return false;
                case(TL_SPMODE):            return false;
                case(TL_HYOUJI):            return false;
                case(TL_RANK_ON_OFF):       return false;
                case(TL_RANK_MIN):          return false;
                case(TL_RANK_MAX):          return false;
                case(TL_CHECK_HYOUUJI):     return false;
                case(TL_KURI_FLAG):         return false;
                default:
                    return false;
            }
        case(KUMI_ZANCOM):
            switch(i){
                case(TL_KATAMEN_HYOUJI):    return false;
                case(TL_KUMI_MIN):          return false;//組合せ数量(最小)
                case(TL_KUMI_MAX):          return false;//組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1):   return false;//組合せ優先1
                case(TL_KUMI_PRIORITY_2):   return false;//組合せ優先2
                case(TL_TANJU):             return false;
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_TAIMEN_TARGET):     return false;
                case(TL_TAIMEN_JOUGEN):     return false;
                case(TL_FUZOROI_BOUSHI):    return false;
                case(TL_SECOND_ON_OFF):     return false;
                case(TL_SPMODE):            return false;
                case(TL_HYOUJI):            return false;
                case(TL_RANK_ON_OFF):       return false;
                case(TL_RANK_MIN):          return false;
                case(TL_RANK_MAX):          return false;
                case(TL_CHECK_HYOUUJI):     return false;
                case(TL_KURI_FLAG):         return false;
                default:
                    return false;
            }
        case(KUMI_ZANCOM2):
            switch(i){
                case(TL_KATAMEN_HYOUJI):    return false;
                case(TL_KUMI_MIN):          return false;//組合せ数量(最小)
                case(TL_KUMI_MAX):          return false;//組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1):   return false;//組合せ優先1
                case(TL_KUMI_PRIORITY_2):   return false;//組合せ優先2
                case(TL_TANJU):             return false;
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_TAIMEN_TARGET):     return false;
                case(TL_TAIMEN_JOUGEN):     return false;
                case(TL_FUZOROI_BOUSHI):    return false;
                case(TL_SECOND_ON_OFF):     return false;
                case(TL_SPMODE):            return false;
                case(TL_HYOUJI):            return false;
                case(TL_RANK_ON_OFF):       return false;
                case(TL_RANK_MIN):          return false;
                case(TL_RANK_MAX):          return false;
                case(TL_CHECK_HYOUUJI):     return false;
                case(TL_KURI_FLAG):         return false;
                default:
                    return false;
            }
        /*//項目増えた時用のサンプル。これをもとに不要なものをfalseにしていく
        case(KUMI_KOSUU):
            switch(i){
                case(TL_KATAMEN_HYOUJI):    return true;
                case(TL_KUMI_MIN):          return true;//組合せ数量(最小)
                case(TL_KUMI_MAX):          return true;//組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1):   return true;//組合せ優先1
                case(TL_KUMI_PRIORITY_2):   return true;//組合せ優先2
                case(TL_TANJU):             return true;
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_TAIMEN_TARGET):     return true;
                case(TL_TAIMEN_JOUGEN):     return true;
                case(TL_FUZOROI_BOUSHI):    return true;
                case(TL_SECOND_ON_OFF):     return true;
                case(TL_SPMODE):            return true;
                case(TL_HYOUJI):            return true;
                case(TL_RANK_ON_OFF):       return true;
                case(TL_RANK_MIN):          return true;
                case(TL_RANK_MAX):          return true;
                case(TL_CHECK_HYOUUJI):     return true;
                default:
                    return true;
                    return false;
            }*/
        default:
            return false;
    }
}

void set_preset(int headPosition)
{
    const char pack[]=          {0xCA,0xDF,0xAF,0xB8,' ' ,0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,' ' ,' ' ,' ' ,0x00};      //ﾊﾟｯｸ ｼﾞｭｳﾘｮｳ
    const char secondtarget[]=  {0xD3,0xB8,0xCB,0xAE,0xB3,0xC1,' ' ,'2' ,0xBA,0xD2,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,0x00};      //ﾓｸﾋｮｳﾁ 2ｺﾒ ｾｯﾃｲ
   
    const char pack_kumi[2][SIZE]= {{0xC1,0xB4,0xAF,0xB8,0xBD,0xD9,' ' ,0xCA,0xDF,0xAF,0xB8,0xBD,0xB3,' ' ,' ' ,' ' ,0x00}       //ﾁｪｯｸｽﾙ ﾊﾟｯｸｽｳ  
                                   ,{0xCA,0xDF,0xAF,0xB8,0xBD,0xB3,' ' ,0xBA,0xDE,0xB3,0xB9,0xB2,' ' ,' ' ,' ' ,' ' ,0x00}};     //ﾊﾟｯｸｽｳ ｺﾞｳｹｲ
    const char total_weight_pack[]= {0xBA,0xDE,0xB3,0xB9,0xB2,' ' ,0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,0x00};      //ｺﾞｳｹｲ ｼﾞｭｳﾘｮｳ
    const char pack_weight[]=       {0xCA,0xDF,0xAF,0xB8,' ' ,0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,0x00};      //ﾊﾟｯｸ ｼﾞｭｳﾘｮｳ
    const char hoseihennkou[]={0xCE,0xBE,0xB2,0xC1,' ' ,0xCD,0xDD,0xBA,0xB3,0x00};      //ﾎｾｲﾁ ﾍﾝｺｳ
    const char spanhennkou[] ={0x34,0x3A,0xBD,0xCA,0xDF,0xDD,' ' ,0xC1,0xAE,0xB3,0xBE,0xB2,0x00};          //ｽﾊﾟﾝﾁｮｳｾｲ
    const char spansakujo[]  ={0xBD,0xCA,0xDF,0xDD,' ' ,0xBB,0xB8,0xBC,0xDE,0xD6,' ' ,' ' ,0x00};      //ｽﾊﾟﾝ ｻｸｼﾞｮ
    
    
    const int dammy = 20;   //関数切り替えに利用するダミーデータ
    data_t limitData;
    load = file.get_load(headPosition); //現在プリセット選択されているデータを取得

    /*int data[] = {load.kumi_flag   ,load.kumi_min    ,load.kumi_max    ,load.kumi_priolity_1    ,load.kumi_priolity_2
                  ,load.combiflug  , load.katamen    ,load.tanjuu      ,load.target
                  ,load.jougen     ,load.taimenTarget,load.taimenJougen,load.fuzoroiBoushi
                  ,load.secondOnOff,load.spMode      ,load.lightTime   ,load.rankOnOff
                  ,load.rankMin    ,load.rankMak     ,load.check_hyouji
                 };*/
    int data[] = {load.kumi_flag   ,load.combiflug   ,load.kumi_min    ,load.kumi_max    ,load.kumi_priolity_1    ,load.kumi_priolity_2
                  ,load.katamen   ,load.tanjuu      ,load.target
                  ,load.jougen     ,load.taimenTarget,load.taimenJougen,load.fuzoroiBoushi
                  ,load.secondOnOff,load.spMode      ,load.lightTime   ,load.rankOnOff
                  ,load.rankMin    ,load.rankMak     ,load.check_hyouji,load.kuri_flag
                 };
    //目標設定を行う
    //for(int i = 0; i < TL_END; i++){
    int i = 0;
    bool no_yes_result = false;
    while( i < TL_END+1) {
        
        
        
        if(i < 0) {
            //lcd.blank(0);
            //lcd.blank(1);
            //lcd.locate(0,0);
            //lcd.printf(ch_esc[0]);
            //wait(1);
            //lcd.blank(0);
            //lcd.blank(1);
            esc_flg = true;
            return; //ESCキー押下したらﾄｳﾛｸメニューに戻る
        }
        if(i== TL_KUMI_FLAG && product.matrix_mode == MATRIX_ON && 
           demo_flg==true && headPosition==product.head*2){//デモﾓｰﾄﾞで最後(通常より+1個追加した番号)だったら、強制的にランク選別にする
            data[TL_KUMI_FLAG]=KUMI_RANK;
            i+=1;
            continue;
        }
        //if(i == TL_END && data[TL_KUMI_FLAG] <= KUMI_LIMIT){
        if(i == TL_HOSEI && data[TL_KUMI_FLAG] <= KUMI_LIMIT){
            lcd.cls();
            lcd.printf(hoseihennkou);
            if(off_on_no_yes_result(2,0)==1){
                hosei_for_preset(file.get_preset());
            }
            //if(btn.esc_press()) {
            //    i-=1;
            //}
        //}else if(i == TL_END && data[TL_KUMI_FLAG] == KUMI_JUURYO && product.limit_on == NIRA_LIMIT){
        }else if(i == TL_SPAN && product.limit_on == NIRA_LIMIT && 
                    (data[TL_KUMI_FLAG] == KUMI_JUURYO/* || data[TL_KUMI_FLAG] <= KUMI_LIMIT*/)){
            /*
            file.read_span_preset(headPosition);
            if(span_preset[0][0]==0){
                no_yes_result = false;
            }else{
                no_yes_result = true;
            }*/
            lcd.cls();
            lcd.printf(spanhennkou);
            if(off_on_no_yes_result(2,0)==1){
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf(omatikudasai);
                set_span_preset(headPosition);
            }
            else{
                if(data[TL_KUMI_FLAG] <= KUMI_LIMIT){
                    lcd.cls();
                    lcd.printf(spansakujo);
                    if(off_on_no_yes_result(2,0)==1){
                        lcd.cls();
                        lcd.locate(0,0);
                        lcd.printf(omatikudasai);
                        for(int l = 0; l < product.head; l++){
                            span_preset[l][0] = 0.0;
                            span_preset[l][1] = 0.0;
                        }
                        file.save_span_preset(headPosition);     //各スパン値を上書き保存
                    }
                }
            }

        }else  if(product.limit_on==SELECTOR_LIMIT && i==TL_KUMI_FLAG) { //ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂはﾗﾝｸｾﾝﾍﾞﾂしかできないので、FLAGの登録は表示させずにすっ飛ばす
            //printf("b3\r\n");
            data[TL_KUMI_FLAG] = KUMI_RANK;
            i += 1;
            continue;
        //}else  if(product.limit_on==NIRA_LIMIT && i==TL_KUMI_FLAG) { //ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂはﾗﾝｸｾﾝﾍﾞﾂしかできないので、FLAGの登録は表示させずにすっ飛ばす
        //    //printf("b3\r\n");
        //    data[TL_KUMI_FLAG] = KUMI_JUURYO;
        //    i += 1;
        //    continue;
        }else{
            //printf("a0\r\n");
            if(preset_use(i,data)==true) { //項目を使用するか否か判定する部分
                //printf("a1\r\n");
                limitData = file.get_limit_data(i); //制限値でデータ取得
                if(i==TL_KUMI_FLAG) {
                    kumi_flug=true; //今登録中の値(KUMI_FLUG判定用)
                } else {
                    kumi_flug=false; //今登録中の値(KUMI_FLUG判定用)
                }
    
                //複数重量範囲(CELLM)がある案件についてはこのプログラム内だけで行う
                //if(i> TL_KUMI_FLAG &&(data[TL_KUMI_FLAG] == KUMI_KAIKYU || data[TL_KUMI_FLAG] == mitei2 || data[TL_KUMI_FLAG] == mitei3)){
                if(i> TL_KUMI_FLAG &&((data[TL_KUMI_FLAG] == KUMI_KAIKYU) || (data[TL_KUMI_FLAG] == KUMI_KAISU1) || (data[TL_KUMI_FLAG] == KUMI_KAISU2) ||
                                      (data[TL_KUMI_FLAG] == KUMI_RANK && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON || product.productType == ITIGO || product.productType == ITIGO_D)))) {
                    //printf("aa\r\n");
                    if(demo_flg==false||demo_flg==true&&headPosition<product.head*2){
                        data[TL_KATAMEN_HYOUJI]        = 0;
                        data[TL_KUMI_MIN]        = 0;
                        data[TL_KUMI_MAX]        = 0;
                        data[TL_KUMI_PRIORITY_1] = 99;
                        data[TL_KUMI_PRIORITY_2] = 99;
                        data[TL_TARGET]          = 100+5;
                        data[TL_JOUGEN]          = 30;
                        if(data[TL_KUMI_FLAG]!=KUMI_KAISU1 && data[TL_KUMI_FLAG]!=KUMI_KAISU2){
                            data[TL_TAIMEN_TARGET]   = 0;
                        }
                        if(data[TL_KUMI_FLAG] > KUMI_LIMIT || option.second_target == 0){
                            data[TL_TAIMEN_JOUGEN]   = 0;
                        }
                        if(data[TL_KUMI_FLAG]!=KUMI_KAISU1 && data[TL_KUMI_FLAG]!=KUMI_KAISU2){
                            data[TL_FUZOROI_BOUSHI]  = 0;
                        }
                        if(data[TL_KUMI_FLAG]!=KUMI_KAISU1 && data[TL_KUMI_FLAG]!=KUMI_KAISU2){
                        data[TL_SECOND_ON_OFF]   = 1;
                        }
                        data[TL_SPMODE]          = 1;
                        data[TL_HYOUJI]          = 25;
                        data[TL_RANK_ON_OFF]     = 0;
                        data[TL_RANK_MIN]        = 0;
                        data[TL_RANK_MAX]        = 0;
                        data[TL_TANJU]           = 0;
                        data[TL_CHECK_HYOUUJI]   = 8;
                        data[TL_COMBI_FLAG]      = 0;
                    }
                    if(i==TL_KUMI_FLAG+1){
                        if(multi_weight_set2(headPosition,data[TL_KUMI_FLAG])==0) {
                            if(demo_flg==false||(demo_flg==true && headPosition<product.head*2)){
                                if(data[TL_KUMI_FLAG]==KUMI_KAISU1 || (data[TL_KUMI_FLAG] == KUMI_KAISU2)){
                                    i = TL_TARGET;
                                    //v.speak_numbers(i);
                                    continue;
                                }else{
                                    i = TL_END;
                                    continue;
                                }
                            }else{
                                i = TL_KUMI_FLAG +2;
                                data[TL_KUMI_FLAG]=KUMI_NONE;
                                if(demo_flg==true && headPosition==product.head*2){
                                    data[TL_KUMI_FLAG]=KUMI_RANK;
                                }
                                continue;
                            }
                        } else {
                            if(product.limit_on==SELECTOR_LIMIT) {
                                return;
                            } else {
                                i = -1;
                                continue;
                            }
                        }
                    } else if(data[TL_KUMI_FLAG]==KUMI_KAISU1 || data[TL_KUMI_FLAG]==KUMI_KAISU2 || (demo_flg==true && headPosition == product.head*2)){
                        if(i == TL_TAIMEN_TARGET && (data[TL_KUMI_FLAG]==KUMI_KAISU1 || data[TL_KUMI_FLAG]==KUMI_KAISU2)){
                            lcd.cls();
                            lcd.printf(pack);
                            wait(0.1);
                        }else{
                            lcd.setTitle(i);
                            wait(0.1);
                        }
                    }
                //複数重量範囲(CELLM)以外
                } else {
                    if(i == TL_SPMODE && product.limit_on==NUMBER_LIMIT) { //回数制限の場合はﾕｯｸﾘﾓｰﾄﾞのみ
                        data[TL_SPMODE] = 1;
                        i += 1;
                        continue;
                    }else if(data[TL_KUMI_FLAG]== KUMI_OYAKO_HALF && (i == TL_KUMI_PRIORITY_1 || i == TL_KUMI_PRIORITY_2)){
                        
                    }else if((data[TL_KUMI_FLAG]== KUMI_NONE || data[TL_KUMI_FLAG]== KUMI_LIMIT ) && i == TL_TAIMEN_JOUGEN && option.second_target == 1){
                        lcd.cls();
                        lcd.printf(secondtarget);
                        wait(0.1);
                            int befoure_yes = 1;
                        if(data[TL_TAIMEN_JOUGEN]>0){
                            befoure_yes = 1;
                        }else{
                            befoure_yes = 0;
                        }
                            
                        if(off_on_no_yes_result(2,befoure_yes) == 0){
                            i += 1;
                            data[TL_TAIMEN_TARGET] = 0;
                            data[TL_TAIMEN_JOUGEN] = 0;
                            continue;
                        }
                        data[TL_TAIMEN_TARGET] = data[TL_TARGET];
                        if(data[TL_TAIMEN_JOUGEN] < data[TL_JOUGEN]){
                            data[TL_TAIMEN_JOUGEN] = data[TL_JOUGEN]+ 1;
                        }
                        limitData.min = data[TL_TAIMEN_JOUGEN] + 1;
                        lcd.setTitle(i);
                        wait(0.1);
                    }else if(data[TL_KUMI_FLAG]== KUMI_PACK){
                        lcd.cls();
                        if(i == TL_KUMI_MIN){
                            lcd.printf(pack_kumi[0]);
                        }
                        else if(i == TL_KUMI_MAX){
                            lcd.printf(pack_kumi[1]);
                        }
                        else if(i == TL_TARGET){
                            lcd.printf(total_weight_pack);
                        }
                    
                    }else if(data[TL_KUMI_FLAG]== KUMI_PACK2){
                        lcd.cls();
                        if(i == TL_KUMI_MAX){
                            lcd.printf(pack_kumi[1]);
                        }else if(i == TL_TARGET){
                            lcd.printf(pack_weight);
                        }
                        
                        
                    } else {
                        lcd.setTitle(i);
                        wait(0.1);
                    }
                }
                //dammy データだったら目標設定項目(最大・最小の設定値反映等)
                if(limitData.lcd == dammy) {
                    if(i== TL_COMBI_FLAG){
                           limitData.max = 2;
                           limitData.min = 0;
                    } 
                    //printf("b1\r\n");
                    if(i == TL_KUMI_FLAG) {
                        limitData.max = KUMI_END-1;
                        if(product.limit_on==WIDE_LIMIT){
                            limitData.max = KUMI_LIMIT;
                        }else{
                            //printf("b2\r\n");
                            if(product.head <= 8) {
                                //6ヘッドの場合親子6:6,対面モードは表示しない
                                //limitData.max = KUMI_OYAKO;
                                limitData.max = KUMI_END;
                            }
                            if(product.limit_on==NUMBER_LIMIT || product.limit_on==WIDE_LIMIT) {
                                //回数制限の場合はｾｲｹﾞﾝｱﾘ･ﾅｼのみ
                                limitData.max = KUMI_LIMIT;
                                if(data[TL_KUMI_FLAG] > KUMI_LIMIT) {
                                    data[TL_KUMI_FLAG]= KUMI_NONE;
                                }
                            }
                        }
                    }
                    //エラー対策
                    //6ヘッドだった場合には、制限有りのmax値= head数までとなる
                    else if(i == TL_KUMI_MIN || i == TL_KUMI_MAX) {
                        if(limitData.max >= product.head -1) {
                            limitData.max = product.head -1;
                        }
                        if( (data[TL_KUMI_FLAG] <= KUMI_LIMIT) ) { 
                            //組み合わせ方法(最大)のとき設定範囲(最小値)を変更
                            if(i == TL_KUMI_MAX) {
                                limitData.min = data[TL_KUMI_MIN];
                            }
                            //エラー対策
                            //皿指定数のmax値の変更を行う。組合せ方法が親子計量(6:6)の場合、
                            //親皿は6個しかないため親皿指定数のmax値を5とする
                            //つまり範囲は(0から～5まで)
                        }
                        if( (data[TL_KUMI_FLAG] == KUMI_OYAKO_HALF)) { 
                            //&& (i == TL_KUMI_MIN || i == TL_KUMI_MAX)) {
                            limitData.max = (product.head / 2)  -1;
                            //エラー対策
                            //親皿は5皿までのため、5より大きい数値は5で初期化
                            if(data[TL_KUMI_MIN] > limitData.max) {
                                data[TL_KUMI_MIN] = limitData.max;
                            }
                            //エラー対策
                            //親皿は5のため、5より大きい数値は5で初期化
                            if(i == TL_KUMI_MAX && data[TL_KUMI_MAX] > 5) {
                                data[TL_KUMI_MAX] = 5;
                            }
                        }
                        //エラー対策
                        //皿指定数のmax値の変更を行う。組合せ方法が皿数制限で、組合せ方法がOFF以外の場合、
                        //親成立出来る皿数は6個しかないためmax値を5とする
                        if( (data[TL_COMBI_FLAG] >0 && data[TL_KUMI_FLAG]==KUMI_LIMIT) && (i == TL_KUMI_MIN || i == TL_KUMI_MAX)) {
                            limitData.max = 5;
                            //エラー対策
                            //5より大きい数値は5で初期化
                            if(data[TL_KUMI_MIN] > 5) {
                                data[TL_KUMI_MIN] = 5;
                            }
                            //エラー対策
                            //5より大きい数値は5で初期化
                            if(i == TL_KUMI_MAX && data[TL_KUMI_MAX] > 5) {
                                data[TL_KUMI_MAX] = 5;
                            }
                        }
                        //親子計量で子皿数の上限を12→10にする
                        if( (data[TL_KUMI_FLAG] == KUMI_OYAKO) && (i == TL_KUMI_MIN || i == TL_KUMI_MAX)) {
                            if(product.Array == 0) {
                                limitData.max = product.head - 3;
                            }else{
                                limitData.max = product.head - 2;
                            }
                        }
                        
                        if(data[TL_KUMI_FLAG] == KUMI_PACK || data[TL_KUMI_FLAG] == KUMI_PACK2) {
                            if(i == TL_KUMI_MIN){
                                limitData.max = 20;
                            }else if(i == TL_KUMI_MAX){
                                limitData.min = data[TL_KUMI_MIN];
                                limitData.max = 120;
                            }
                        }
                    }
    
    
                    //優先組み合わせ方法のとき設定範囲(最大・最小値)を変更
                    else if(i == TL_KUMI_PRIORITY_1 || i == TL_KUMI_PRIORITY_2) {
                        if(data[TL_KUMI_FLAG]== KUMI_OYAKO_HALF){
                            limitData.min = 0;
                            limitData.max = 5;
                            if(i == TL_KUMI_PRIORITY_2){
                                limitData.min = data[TL_KUMI_PRIORITY_1];
                            }
                        }else{
                            limitData.min = data[TL_KUMI_MIN];
                            limitData.max = data[TL_KUMI_MAX] +1;
                            //ﾕｳｾﾝｸﾐｱﾜｾで決定した値が 99(OFF)だった時に値を変更できる値へ一時的に戻す
                            if(data[i] == 99) {
                                data[i] = limitData.max;
                            }
                        }
                    }
                    else if(i == TL_KURI_FLAG) {
                        limitData.min = 0;
                        limitData.max = 3;
                        limitData.lcd = 1;
                    }
                    //優先組み合わせ方法のとき設定範囲(最大・最小値)を変更
                    if(i == TL_CHECK_HYOUUJI) {
                        if(limitData.max > (int(data[TL_TARGET]/data[TL_TANJU])-2)) {
                            limitData.max=int(data[TL_TARGET]/data[TL_TANJU])-2;
                        }
                    }
                    //値変更************************
                    //if(i==TL_KATAMEN_HYOUJI){
                    //    limitData.lcd = OP_KATAMEN;//ON/OFFChenge_vaule表示
                    //    Chenge_vaule2(&data[i],limitData);
                    //}else{
                    SetteiMokuhyo(&data[i], limitData,i,data);
                    //}
                    //値変更************************
                    //値変更後の設定************************
                    //ﾕｳｾﾝｸﾐｱﾜｾで決定した値が limitData.max(すでに組合せ数より1にしている)だった時に値を絶対ない組合せ数にする(ﾕｳｾﾝが消える)
                    if((i == TL_KUMI_PRIORITY_1 || i == TL_KUMI_PRIORITY_2) && data[i] == limitData.max) {
                        data[i] = 99;
                    }
                } else {
                    //単位を表示(g secなど)
                    if(i == TL_FUZOROI_BOUSHI) {
                        lcd.locate(10,1);
                    }else{
                        lcd.locate(7,1);
                    }
    
                    //目標重量
                    if(i >= TL_TARGET && i <= TL_FUZOROI_BOUSHI) {
                        lcd.msgPrintf(DIS_G);
                    }
                    if(i == TL_TARGET) {
                        if(data[TL_KUMI_FLAG]==KUMI_KOSUU) {
                            limitData.min = data[TL_TANJU];
                        }
                    }
                    if(i == TL_JOUGEN && data[TL_KUMI_FLAG]==KUMI_KOSUU) {
                        limitData.min = data[TL_TANJU];
                    }else if(i == TL_JOUGEN && data[TL_KUMI_FLAG]==KUMI_LIMIT) {
                        limitData.min = int(float(data[TL_KUMI_MIN])*param.buffer2);
                    }
                    if(i == TL_HYOUJI) {
                        lcd.msgPrintf(DIS_MSEC);
                    }
                    if(i == TL_RANK_MIN) {
                        lcd.msgPrintf(DIS_IJO);
                    }
                    if(i == TL_RANK_MAX) {
                        lcd.msgPrintf(DIS_IKA);
                    }
    
                    //ランク管理、上限下限どちらもある場合
                    //上限値は下限値を下回らない
                    if(i == TL_RANK_MAX && data[TL_RANK_ON_OFF] == 3) {
                        //Min値を取得
                        limitData.min = data[TL_RANK_MIN]+1;
                    }
                    if((data[TL_KUMI_FLAG]==KUMI_KAISU1 || data[TL_KUMI_FLAG]==KUMI_KAISU2) && i == TL_TAIMEN_TARGET){
                        limitData.min = 0;
                    }
                    //LED点灯時間のみ小数点となるため、型変換を行う。
                    if(i == TL_HYOUJI) {
                        float sec = float(data[i]) / float(10); //(例)1 → 0.1へ
                        Chenge_vaule( &sec, limitData);
                        data[i] = int(sec * float(10)+0.2);  //データ型をint型へ戻す(例)0.1 → 1へ
    
                        //邪道なエラー対策だが0値だとLEDが光らないので必ず1以上とする
                        if(data[i] == 0) {
                            data[i] = 1;
                        }
                    } else if(i == TL_JOUGEN || i == TL_TAIMEN_JOUGEN) {
                        Chenge_vaule_JOUGEN( &data[i], limitData, data[i-1]);
                    } else if((data[TL_KUMI_FLAG]==KUMI_KAISU1 || data[TL_KUMI_FLAG]==KUMI_KAISU2) && i == TL_TAIMEN_TARGET){
                        Chenge_Measured_value( &data[i], limitData);
                        
                    } else {
                        Chenge_vaule( &data[i], limitData);
                    }
                }
    
            } else { //preset_use()
                //使用しない項目＝初期値を入れておく
                preset_default(i, data, headPosition);
            }
            //次の設定項目を取得
            //今登録中の値(KUMI_FLUG判定用)
            kumi_flug=false;
        }
        if(btn.get_btn() == BTN_ENT) {
            i+=1;
        } else if(btn.get_btn() == BTN_ESC) {
            i-=1;
            //if(product.limit_on==NIRA_LIMIT && i==TL_KUMI_FLAG) {
            //    return;
            //}

        }



    }
    /*lcd.cls();
    lcd.printf(hoseihennkou);
    if(off_on_no_yes_result(2,0)==1){
        hosei_for_preset(file.get_preset());
    }*/
    
    lcd.blank(0);
    lcd.blank(1);
    lcd.locate(0,1);
    lcd.printf("OK!");
    se.OK();
    /*
    lcd.locate(0,0);
    lcd.printf(ch_end[0]);
    lcd.locate(0,1);
    lcd.printf(ch_end[1]);
    */
    file.set_load(data, headPosition);      //変更されたデータを受け渡し
    file.save_loadcell();                   //各パラメータを保存
    //wait(1);
    lcd.blank(0);
    lcd.blank(1);
}
//プリセットの設定を行う(1個の機能のみ)
void set_preset2(int headPosition ,int set)
{
    const char pack[]=          {0xCA,0xDF,0xAF,0xB8,' ' ,0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,' ' ,' ' ,' ' ,0x00};      //ﾊﾟｯｸ ｼﾞｭｳﾘｮｳ
    const char secondtarget[]=  {0xD3,0xB8,0xCB,0xAE,0xB3,0xC1,' ' ,'2' ,0xBA,0xD2,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,0x00};      //ﾓｸﾋｮｳﾁ 2ｺﾒ ｾｯﾃｲ
   
    const char pack_kumi[2][SIZE]= {{0xC1,0xB4,0xAF,0xB8,0xBD,0xD9,' ' ,0xCA,0xDF,0xAF,0xB8,0xBD,0xB3,' ' ,' ' ,' ' ,0x00}       //ﾁｪｯｸｽﾙ ﾊﾟｯｸｽｳ  
                                   ,{0xCA,0xDF,0xAF,0xB8,0xBD,0xB3,' ' ,0xBA,0xDE,0xB3,0xB9,0xB2,' ' ,' ' ,' ' ,' ' ,0x00}};     //ﾊﾟｯｸｽｳ ｺﾞｳｹｲ
    const char total_weight_pack[]= {0xBA,0xDE,0xB3,0xB9,0xB2,' ' ,0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,0x00};      //ｺﾞｳｹｲ ｼﾞｭｳﾘｮｳ
    const char pack_weight[]=       {0xCA,0xDF,0xAF,0xB8,' ' ,0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,0x00};      //ﾊﾟｯｸ ｼﾞｭｳﾘｮｳ
    
    const int dammy = 20;   //関数切り替えに利用するダミーデータ
    data_t limitData;
    load = file.get_load(headPosition); //現在プリセット選択されているデータを取得

    /*int data[] = {load.kumi_flag ,load.kumi_min, load.kumi_max , load.kumi_priolity_1, load.kumi_priolity_2, load.target
            , load.jougen,load.taimenTarget,load.taimenJougen,load.fuzoroiBoushi
            ,load.secondOnOff,load.spMode,load.lightTime,load.rankOnOff
            ,load.rankMin,load.rankMak};
    */
    /*int data[] = {load.kumi_flag   ,load.kumi_min    ,load.kumi_max    ,load.kumi_priolity_1    ,load.kumi_priolity_2
                  ,load.combiflug  , load.katamen    ,load.tanjuu      ,load.target
                  ,load.jougen     ,load.taimenTarget,load.taimenJougen,load.fuzoroiBoushi
                  ,load.secondOnOff,load.spMode      ,load.lightTime   ,load.rankOnOff
                  ,load.rankMin    ,load.rankMak     ,load.check_hyouji
                 };*/
    int data[] = {load.kumi_flag   ,load.combiflug   ,load.kumi_min    ,load.kumi_max    ,load.kumi_priolity_1    ,load.kumi_priolity_2
                  ,load.katamen   ,load.tanjuu      ,load.target
                  ,load.jougen     ,load.taimenTarget,load.taimenJougen,load.fuzoroiBoushi
                  ,load.secondOnOff,load.spMode      ,load.lightTime   ,load.rankOnOff
                  ,load.rankMin    ,load.rankMak     ,load.check_hyouji,load.kuri_flag
                 };
    //目標設定を行う
    for(int i = 0; i < TL_END; i++) {
        if(i < 0) {
            //lcd.blank(0);
            //lcd.blank(1);
            //lcd.locate(0,0);
            //lcd.printf(ch_esc[0]);
            //wait(1);
            //lcd.blank(0);
            //lcd.blank(1);
            return; //ESCキー押下したらﾄｳﾛｸメニューに戻る
        }
        if(i== TL_KUMI_FLAG && product.matrix_mode == MATRIX_ON && 
           demo_flg==true && headPosition==product.head*2){//デモﾓｰﾄﾞで最後(通常より+1個追加した番号)だったら、強制的にランク選別にする
            data[TL_KUMI_FLAG]=KUMI_RANK;
            i+=1;
            continue;
        }
        //printf("a0\r\n");
        if(i == set) {
            //printf("a1\r\n");
            limitData = file.get_limit_data(i); //制限値でデータ取得
            if(i==TL_KUMI_FLAG) {
                kumi_flug=true; //今登録中の値(KUMI_FLUG判定用)
            } else {
                kumi_flug=false; //今登録中の値(KUMI_FLUG判定用)
            }
            //複数重量範囲がある案件についてはこのプログラム内だけで行う
            //if(i> TL_KUMI_FLAG &&(data[TL_KUMI_FLAG] == KUMI_KAIKYU || data[TL_KUMI_FLAG] == mitei2 || data[TL_KUMI_FLAG] == mitei3)){
            if(i> TL_KUMI_FLAG &&((data[TL_KUMI_FLAG] == KUMI_KAIKYU) || (data[TL_KUMI_FLAG] == KUMI_KAISU1) || (data[TL_KUMI_FLAG] == KUMI_KAISU2) ||
                                  (data[TL_KUMI_FLAG] == KUMI_RANK && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON || product.productType == ITIGO || product.productType == ITIGO_D)))) {

                //printf("aa\r\n");
                if(demo_flg==false||demo_flg==true&&headPosition<product.head*2){
                    data[TL_KATAMEN_HYOUJI]        = 0;
                    data[TL_KUMI_MIN]        = 0;
                    data[TL_KUMI_MAX]        = 0;
                    data[TL_KUMI_PRIORITY_1] = 99;
                    data[TL_KUMI_PRIORITY_2] = 99;
                    data[TL_TARGET]          = 100+5;
                    data[TL_JOUGEN]          = 30;
                    if(data[TL_KUMI_FLAG]!=KUMI_KAISU1 && data[TL_KUMI_FLAG]!=KUMI_KAISU2){
                        data[TL_TAIMEN_TARGET]   = 0;
                    }
                    if(data[TL_KUMI_FLAG] > KUMI_LIMIT || option.second_target == 0){
                        data[TL_TAIMEN_JOUGEN]   = 0;
                    }
                    if(data[TL_KUMI_FLAG]!=KUMI_KAISU1 && data[TL_KUMI_FLAG]!=KUMI_KAISU2){
                        data[TL_FUZOROI_BOUSHI]  = 0;
                    }
                    if(data[TL_KUMI_FLAG]!=KUMI_KAISU1 && data[TL_KUMI_FLAG]!=KUMI_KAISU2){
                    data[TL_SECOND_ON_OFF]   = 1;
                    }
                    data[TL_SPMODE]          = 1;
                    data[TL_HYOUJI]          = 25;
                    data[TL_RANK_ON_OFF]     = 0;
                    data[TL_RANK_MIN]        = 0;
                    data[TL_RANK_MAX]        = 0;
                    data[TL_TANJU]           = 0;
                    data[TL_CHECK_HYOUUJI]   = 8;
                    data[TL_COMBI_FLAG]      = 0;
                    if(multi_weight_set2(headPosition,data[TL_KUMI_FLAG])==0) {
                        i = TL_END;
                        continue;
                    } else {
                        i = -1;
                        continue;
                    }
               }
                if(i==TL_KUMI_FLAG+1){
                    if(multi_weight_set2(headPosition,data[TL_KUMI_FLAG])==0) {
                        if(demo_flg==false||(demo_flg==true && headPosition<product.head*2)){
                            if(data[TL_KUMI_FLAG]==KUMI_KAISU1 || (data[TL_KUMI_FLAG] == KUMI_KAISU2)){
                                i = TL_TARGET;
                                //v.speak_numbers(i);
                                continue;
                            }else{
                                i = TL_END;
                                continue;
                            }
                        }else{
                            i = TL_KUMI_FLAG +2;
                            data[TL_KUMI_FLAG]=KUMI_NONE;
                            if(demo_flg==true && headPosition==product.head*2){
                                data[TL_KUMI_FLAG]=KUMI_RANK;
                            }
                            continue;
                        }
                    } else {
                        if(product.limit_on==SELECTOR_LIMIT) {
                            return;
                        } else {
                            i = -1;
                            continue;
                        }
                    }
                } else if(data[TL_KUMI_FLAG]==KUMI_KAISU1 || data[TL_KUMI_FLAG]==KUMI_KAISU2 || (demo_flg==true && headPosition == product.head*2)){
                    if(i == TL_TAIMEN_TARGET && (data[TL_KUMI_FLAG]==KUMI_KAISU1 || data[TL_KUMI_FLAG]==KUMI_KAISU2)){
                        lcd.cls();
                        lcd.printf(pack);
                        wait(0.1);
                    }else{
                        lcd.setTitle(i);
                        wait(0.1);
                    }
                }
            //複数重量範囲(CELLM)以外
            } else {
                if(i == TL_SPMODE && product.limit_on==NUMBER_LIMIT) { //回数制限の場合はﾕｯｸﾘﾓｰﾄﾞのみ
                    data[TL_SPMODE] = 1;
                    i += 1;
                    continue;
                }else if(data[TL_KUMI_FLAG]== KUMI_OYAKO_HALF && (i == TL_KUMI_PRIORITY_1 || i == TL_KUMI_PRIORITY_2)){
                    
                }else if((data[TL_KUMI_FLAG]== KUMI_NONE || data[TL_KUMI_FLAG]== KUMI_LIMIT ) && i == TL_TAIMEN_JOUGEN && option.second_target == 1){
                    lcd.cls();
                    lcd.printf(secondtarget);
                    wait(0.1);
                        int befoure_yes = 1;
                    if(data[TL_TAIMEN_JOUGEN]>0){
                        befoure_yes = 1;
                    }else{
                        befoure_yes = 0;
                    }
                        
                    if(off_on_no_yes_result(2,befoure_yes) == 0){
                        i += 1;
                        data[TL_TAIMEN_TARGET] = 0;
                        data[TL_TAIMEN_JOUGEN] = 0;
                        continue;
                    }
                    data[TL_TAIMEN_TARGET] = data[TL_TARGET];
                    if(data[TL_TAIMEN_JOUGEN] < data[TL_JOUGEN]){
                        data[TL_TAIMEN_JOUGEN] = data[TL_JOUGEN]+ 1;
                    }
                    limitData.min = data[TL_TAIMEN_JOUGEN] + 1;
                    lcd.setTitle(i);
                    wait(0.1);
                }else if(data[TL_KUMI_FLAG]== KUMI_PACK){
                    lcd.cls();
                    if(i == TL_KUMI_MIN){
                        lcd.printf(pack_kumi[0]);
                    }
                    else if(i == TL_KUMI_MAX){
                        lcd.printf(pack_kumi[1]);
                    }
                    else if(i == TL_TARGET){
                        lcd.printf(total_weight_pack);
                    }
                
                }else if(data[TL_KUMI_FLAG]== KUMI_PACK2){
                    lcd.cls();
                    if(i == TL_KUMI_MAX){
                        lcd.printf(pack_kumi[1]);
                    }else if(i == TL_TARGET){
                        lcd.printf(pack_weight);
                    }
                } else {
                    lcd.setTitle(i);
                    wait(0.1);
                }
            }
            //dammy データだったら目標設定項目(最大・最小の設定値反映等)
            if(limitData.lcd == dammy) {
                if(i== TL_COMBI_FLAG){
                       limitData.max = 2;
                       limitData.min = 0;
                } 
                //printf("b1\r\n");
                if(i == TL_KUMI_FLAG) {
                    limitData.max = KUMI_END-1;
                    if(product.limit_on==WIDE_LIMIT){
                        limitData.max = KUMI_LIMIT;
                    }else if(now_TC_mode >= 1){
                        limitData.max = 1;
                    }else{
                        //printf("b2\r\n");
                        if(product.head <= 8) {
                            //6ヘッドの場合親子6:6,対面モードは表示しない
                            //limitData.max = KUMI_OYAKO;
                            limitData.max = KUMI_END;
                        }
                        if(product.limit_on==NUMBER_LIMIT || product.limit_on==WIDE_LIMIT) {
                            //回数制限の場合はｾｲｹﾞﾝｱﾘ･ﾅｼのみ
                            limitData.max = KUMI_LIMIT;
                            if(data[TL_KUMI_FLAG] > KUMI_LIMIT) {
                                data[TL_KUMI_FLAG]= KUMI_NONE;
                            }
                        } else  if(product.limit_on==SELECTOR_LIMIT && i==TL_KUMI_FLAG) { //ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂはﾗﾝｸｾﾝﾍﾞﾂしかできないので、FLAGの登録は表示させずにすっ飛ばす
                            //printf("b3\r\n");
                            data[TL_KUMI_FLAG] = KUMI_RANK;
                            i += 1;
                            continue;
                        }
                    }
                }
                //エラー対策
                //6ヘッドだった場合には、制限有りのmax値= head数までとなる
                else if(i == TL_KUMI_MIN || i == TL_KUMI_MAX) {
                    if(limitData.max >= product.head) {
                        limitData.max = product.head -1;
                    }
                    //組み合わせ方法(最大)のとき設定範囲(最小値)を変更
                    if(i == TL_KUMI_MAX) {
                        limitData.min = data[TL_KUMI_MIN];
                    }
                    //エラー対策
                    //皿指定数のmax値の変更を行う。組合せ方法が親子計量(6:6)の場合、
                    //親皿は6個しかないため親皿指定数のmax値を5とする
                    //つまり範囲は(0から～5まで)
                    if( (data[TL_KUMI_FLAG] == KUMI_OYAKO_HALF) && (i == TL_KUMI_MIN || i == TL_KUMI_MAX)) {
                        limitData.max = 5;
                        //エラー対策
                        //親皿は5皿までのため、5より大きい数値は5で初期化
                        if(data[TL_KUMI_MIN] > 5) {
                            data[TL_KUMI_MIN] = 5;
                        }
                        //エラー対策
                        //親皿は5のため、5より大きい数値は5で初期化
                        if(i == TL_KUMI_MAX && data[TL_KUMI_MAX] > 5) {
                            data[TL_KUMI_MAX] = 5;
                        }
                    }
                    //エラー対策
                    //皿指定数のmax値の変更を行う。組合せ方法が皿数制限で、組合せ方法がOFF以外の場合、
                    //親成立出来る皿数は6個しかないためmax値を5とする
                    if( (data[TL_COMBI_FLAG] >0 && data[TL_KUMI_FLAG]==KUMI_LIMIT) && (i == TL_KUMI_MIN || i == TL_KUMI_MAX)) {
                        limitData.max = 5;
                        //エラー対策
                        //5より大きい数値は5で初期化
                        if(data[TL_KUMI_MIN] > 5) {
                            data[TL_KUMI_MIN] = 5;
                        }
                        //エラー対策
                        //5より大きい数値は5で初期化
                        if(i == TL_KUMI_MAX && data[TL_KUMI_MAX] > 5) {
                            data[TL_KUMI_MAX] = 5;
                        }
                    }
                    //親子計量で子皿数の上限を12→10にする
                    if( (data[TL_KUMI_FLAG] == KUMI_OYAKO) && (i == TL_KUMI_MIN || i == TL_KUMI_MAX)) {
                        if(product.Array == 0) {
                            limitData.max = product.head - 3;
                        }else{
                            limitData.max = product.head - 2;
                        }
                    }
                    
                    if(data[TL_KUMI_FLAG] == KUMI_PACK || data[TL_KUMI_FLAG] == KUMI_PACK2) {
                        if(i == TL_KUMI_MIN){
                            limitData.max = 20;
                        }else if(i == TL_KUMI_MAX){
                            limitData.min = data[TL_KUMI_MIN];
                            limitData.max = 120;
                        }
                    }
                }


                //優先組み合わせ方法のとき設定範囲(最大・最小値)を変更
                else if(i == TL_KUMI_PRIORITY_1 || i == TL_KUMI_PRIORITY_2) {
                    if(data[TL_KUMI_FLAG]== KUMI_OYAKO_HALF){
                        limitData.min = 0;
                        limitData.max = 5;
                        if(i == TL_KUMI_PRIORITY_2){
                            limitData.min = data[TL_KUMI_PRIORITY_1];
                        }
                    }else{
                        limitData.min = data[TL_KUMI_MIN];
                        limitData.max = data[TL_KUMI_MAX] +1;
                        //ﾕｳｾﾝｸﾐｱﾜｾで決定した値が 99(OFF)だった時に値を変更できる値へ一時的に戻す
                        if(data[i] == 99) {
                            data[i] = limitData.max;
                        }
                    }
                }
                else if(i == TL_KURI_FLAG) {
                    limitData.min = 0;
                    limitData.max = 3;
                    limitData.lcd = 1;
                }
                //優先組み合わせ方法のとき設定範囲(最大・最小値)を変更
                if(i == TL_CHECK_HYOUUJI) {

                    if(limitData.max > (int(data[TL_TARGET]/data[TL_TANJU])-2)) {
                        limitData.max=int(data[TL_TARGET]/data[TL_TANJU])-2;
                    }
                }
                //値変更************************
                //if(i==TL_KATAMEN_HYOUJI){
                //    limitData.lcd = OP_KATAMEN;//ON/OFFChenge_vaule表示
                //    Chenge_vaule2(&data[i],limitData);
                //}else{
                SetteiMokuhyo(&data[i], limitData,i,data);
                //}
                //値変更************************
                //値変更後の設定************************
                //ﾕｳｾﾝｸﾐｱﾜｾで決定した値が limitData.max(すでに組合せ数より1にしている)だった時に値を絶対ない組合せ数にする(ﾕｳｾﾝが消える)
                if((i == TL_KUMI_PRIORITY_1 || i == TL_KUMI_PRIORITY_2) && data[i] == limitData.max) {
                    data[i] = 99;
                }
            } else {
                //単位を表示(g secなど)
                if(i == TL_FUZOROI_BOUSHI) {
                    lcd.locate(10,1);
                }else{
                    lcd.locate(7,1);
                }

                //目標重量
                if(i >= TL_TARGET && i <= TL_FUZOROI_BOUSHI) {
                    lcd.msgPrintf(DIS_G);
                }
                if(i == TL_TARGET) {
                    if(data[TL_KUMI_FLAG]==KUMI_KOSUU) {
                        limitData.min = data[TL_TANJU];
                    }
                }
                if(i == TL_JOUGEN && data[TL_KUMI_FLAG]==KUMI_KOSUU) {
                    limitData.min = data[TL_TANJU];
                }else if(i == TL_JOUGEN && data[TL_KUMI_FLAG]==KUMI_LIMIT) {
                    limitData.min = int(float(data[TL_KUMI_MIN])*param.buffer2);
                }
                if(i == TL_HYOUJI) {
                    lcd.msgPrintf(DIS_MSEC);
                }
                if(i == TL_RANK_MIN) {
                    lcd.msgPrintf(DIS_IJO);
                }
                if(i == TL_RANK_MAX) {
                    lcd.msgPrintf(DIS_IKA);
                }

                //ランク管理、上限下限どちらもある場合
                //上限値は下限値を下回らない
                if(i == TL_RANK_MAX && data[TL_RANK_ON_OFF] == 3) {
                    //Min値を取得
                    limitData.min = data[TL_RANK_MIN]+1;
                }
                if((data[TL_KUMI_FLAG]==KUMI_KAISU1 || data[TL_KUMI_FLAG]==KUMI_KAISU2) && i == TL_TAIMEN_TARGET){
                    limitData.min = 0;
                }
                //LED点灯時間のみ小数点となるため、型変換を行う。
                if(i == TL_HYOUJI) {
                    float sec = float(data[i]) / float(10); //(例)1 → 0.1へ
                    Chenge_vaule( &sec, limitData);
                    data[i] = int(sec * float(10)+0.2);  //データ型をint型へ戻す(例)0.1 → 1へ

                    //邪道なエラー対策だが0値だとLEDが光らないので必ず1以上とする
                    if(data[i] == 0) {
                        data[i] = 1;
                    }
                } else if(i == TL_JOUGEN || i == TL_TAIMEN_JOUGEN) {
                    Chenge_vaule_JOUGEN( &data[i], limitData, data[i-1]);
                } else if((data[TL_KUMI_FLAG]==KUMI_KAISU1 || data[TL_KUMI_FLAG]==KUMI_KAISU2) && i == TL_TAIMEN_TARGET){
                    Chenge_Measured_value( &data[i], limitData);
                    
                } else {
                    Chenge_vaule( &data[i], limitData);
                }
            }

            //esc押下なら処理終了
            if(btn.esc()) {
                i = -2;
                continue;

            }
            //次の設定項目を取得
            switch(i){
                case TL_KUMI_FLAG :
                    if(data[TL_KUMI_FLAG] == KUMI_LIMIT){
                        if(product.head<12){
                             set = TL_KUMI_MIN;
                        }else{  
                            set = TL_COMBI_FLAG;
                        }
                    }else if(data[TL_KUMI_FLAG] == KUMI_NONE){
                        set = TL_COMBI_FLAG;
                    /*}else if(data[TL_KUMI_FLAG] == KUMI_COMBI){  
                        set = TL_COMBI_FLAG;*/
                    }else{i = TL_END;kumi_flug=false;}
                    break;
                case TL_COMBI_FLAG :
                    if(data[TL_KUMI_FLAG] == KUMI_LIMIT){ 
                        set = TL_KUMI_MIN;
                    }else{i = TL_END;kumi_flug=false;}
                    break;
                case TL_KUMI_MIN : set = TL_KUMI_MAX; break;
                case TL_KUMI_MAX :
                    if(data[TL_KUMI_MIN] != data[TL_KUMI_MAX]){
                        set = TL_KUMI_PRIORITY_1;}
                    else{i = TL_END;kumi_flug=false;}
                    break;
                case TL_KUMI_PRIORITY_1 :
                    if(data[TL_KUMI_PRIORITY_1] != 99){
                        set = TL_KUMI_PRIORITY_2;}
                    else{i = TL_END;kumi_flug=false;}
                    break;
                case TL_TARGET :            set = TL_JOUGEN; break;
                case TL_KUMI_PRIORITY_2 :
                case TL_JOUGEN :
                case TL_TAIMEN_TARGET :
                case TL_TAIMEN_JOUGEN :
                case TL_FUZOROI_BOUSHI :
                case TL_SECOND_ON_OFF :     i = TL_END;kumi_flug=false; break;
                case TL_SPMODE :            set = TL_HYOUJI; break;
                    //if(data[TL_SPMODE] != SPEED_LOW || product.limit_on == WIDE_LIMIT){set = TL_HYOUJI;}
                    //else{i = TL_END;kumi_flug=false;}
                    //break;
                case TL_HYOUJI :
                case TL_RANK_ON_OFF :
                case TL_RANK_MIN :
                case TL_RANK_MAX :
                default :
                //次の設定項目を取得
                    i = TL_END;
                    //今登録中の値(KUMI_FLUG判定用)
                    kumi_flug=false;
                    break;
            }
        }
    }//for


    lcd.blank(0);
    lcd.blank(1);
    lcd.locate(0,1);
    lcd.printf("OK!");
    se.OK();
    /*
    lcd.locate(0,0);
    lcd.printf(ch_end[0]);
    lcd.locate(0,1);
    lcd.printf(ch_end[1]);
    */
    file.set_load(data, headPosition);      //変更されたデータを受け渡し
    file.save_loadcell();                   //各パラメータを保存
    load = file.get_load(headPosition); //現在プリセット選択されているデータを取得
    cel.set_now_loadcell_data(&load);   //現在選択されているプリセット(ロードセル)をset
    //wait(1);
    lcd.blank(0);
    lcd.blank(1);
}

//目標設定のesc押下時の次の設定項目を取得
int get_title_push_esc(int iEsc, const int *data)
{
    //組み合わせ数設定の場合、組み合わせ方法まで戻る
    //2017,05,08 優先組数が追加されたので、変更前の値条件を分割

    if((data[TL_KUMI_FLAG] == KUMI_OYAKO_HALF) && (iEsc == TL_TARGET)) {
        //se.OK();
        iEsc =  TL_KUMI_MIN;
        return iEsc;
    }
    if((data[TL_KUMI_FLAG] == KUMI_LIMIT) && (iEsc == TL_TARGET)) {
        //se.OK();
        iEsc =  TL_KUMI_MIN;
        return iEsc;
    }
    if(iEsc == TL_KUMI_MIN || iEsc == TL_TARGET) {
        iEsc = -1;
        return iEsc;
    }
    if(iEsc == TL_KUMI_MIN || iEsc == TL_TARGET) {
        iEsc = -1;
        return iEsc;
    }

    //不揃い防止設定だったら
    if(iEsc == TL_FUZOROI_BOUSHI && data[TL_KUMI_FLAG] != KUMI_TAIMEN) {
        iEsc = TL_TARGET;
        return iEsc;
    }
    //ゆっくりモード設定の場合は、上限値設定まで戻る
    if(iEsc == TL_SPMODE) {
        iEsc = TL_FUZOROI_BOUSHI;   //通常は対面の上限重量値にもどる
        return iEsc;
    }

    //ランク管理設定の場合、管理設定まで戻る
    if(iEsc == TL_RANK_ON_OFF) {
        iEsc = TL_SECOND_ON_OFF;
        return iEsc;
    }
    //ランク管理設定の場合、管理設定まで戻る
    if(iEsc == TL_RANK_MIN || iEsc == TL_RANK_MAX) {
        iEsc = TL_HYOUJI;
        return iEsc;
    }
    iEsc -= 2;
    return iEsc;
}

//目標設定の次の設定項目を取得
//※ここのあとに値が+1(次へ)になるので、1個前の値を指示すること
int get_next_title(int i,int* data)
{

    //組み合わせ方法選択が制限有り以外っだら目標設定へ
    //追加2016/08 親子計量(6:6)以外の場合も目標設定を行う
    if( (i == TL_KUMI_FLAG) && (data[i] != KUMI_LIMIT && data[0] != KUMI_OYAKO_HALF)) {
        data[TL_KUMI_MIN] = 0;
        data[TL_KUMI_MAX] = 0;
        data[TL_KUMI_PRIORITY_1] = 99;
        data[TL_KUMI_PRIORITY_2] = 99;
        i = TL_KUMI_PRIORITY_2;  //計量目標設定へ
        //i = TL_KUMI_MAX;  //計量目標設定へ
    } else if((i == TL_KUMI_MAX) && data[TL_KUMI_MIN] == data[TL_KUMI_MAX]) {
        //se.Touch_01();
        //se.Touch_01();
        //se.Touch_01();
        data[TL_KUMI_PRIORITY_1] = 99;
        data[TL_KUMI_PRIORITY_2] = 99;
        i = TL_KUMI_PRIORITY_2;  //計量目標設定へ

        //追加 2017.06.10 組合せ優先をOFFにした場合のジャンプを追加
    } else if((i == TL_KUMI_PRIORITY_1) && (data[TL_KUMI_PRIORITY_1] < data[TL_KUMI_MIN]- 1 || data[TL_KUMI_PRIORITY_1] > data[TL_KUMI_MAX]  || data[TL_KUMI_MIN] == data[TL_KUMI_MAX])) {
        //se.Touch_01();
        data[TL_KUMI_PRIORITY_2] = 99;
        i = TL_KUMI_PRIORITY_2;  //計量目標設定へ
        //追加 2016.08.18 ランク選別機能を追加。
        //ランク選別の場合、重量設定を行ったら設定終了とする
        //各設定値は固定とする。高速モードに固定、点灯時間固定とする。
    } else if((i == TL_JOUGEN) && (data[TL_KUMI_FLAG] == KUMI_RANK)) {
        data[TL_SPMODE] = 0;     //高速モード固定
        data[TL_HYOUJI] = RANK_LED_TIME;     //LEDの点灯時間
        i = TL_END;
        //対面計量以外だったら、対面時の重量設定を表示しない
    } else if(i == TL_JOUGEN && data[TL_KUMI_FLAG] != KUMI_TAIMEN) {
        i = TL_TAIMEN_JOUGEN;
        //対面計量時は、準最適値機能は使用できない
    } else if(i == TL_TAIMEN_JOUGEN && data[TL_KUMI_FLAG] == KUMI_TAIMEN) {
        //ここに各初期値を代入
        data[TL_SECOND_ON_OFF] = 0;  //準最適値OFF
        data[TL_SPMODE] = 1;     //ゆっくりモード固定
        data[TL_HYOUJI] = 1;     //0.1秒
        data[TL_RANK_ON_OFF] = 0;  //ランク管理OFF
        data[TL_RANK_MIN] = 20;
        data[TL_RANK_MAX] = 30;

        i = TL_END;

        //ゆっくモード設定が高速モード以外だったら
    } else if(i == TL_SPMODE && (data[i] == SPEED_LOW )) {
        i = TL_HYOUJI;        //LED表示時間を設定しない
    } else if(i == TL_RANK_ON_OFF && data[i] == 0) {
        i = TL_END;     //ランク管理ナシならば重量設定しない
    } else if(i == TL_RANK_ON_OFF && data[i] == 2) {
        i = TL_RANK_MIN;     //ランク管理有り,上限のみならば
    } else if(i == TL_RANK_MIN && data[TL_RANK_ON_OFF] == 1) {
        i = TL_END;     //ランク管理有り,上限のみならば
    } else if( (data[TL_KUMI_FLAG] == KUMI_OYAKO_HALF) && (i == TL_KUMI_MAX)) {
        data[TL_KUMI_PRIORITY_1] = 99;  //対面親子計量の場合、ﾕｳｾﾝ組合せは使用しない
        data[TL_KUMI_PRIORITY_2] = 99;
        i = TL_KUMI_PRIORITY_2 ;     //計量目標設定へ
    }

    return i;
}
int set_mokuhyo_no(int Num){
    //printf("NUM%d  M %d ar %d\r\n",Num,product.matrix_mode,product.Array);
    //マトリクス以外はランク別組合せができないので、項目の番号をいじる

    if(product.matrix_mode == MATRIX_ON){
        if(product.limit_on == NIRA_LIMIT){
            switch(Num){//6H以上の LED
                case KUMI_NONE:         return 1;
                case KUMI_LIMIT:        return 2;
                case KUMI_RANK:         return 0;
                //case KUMI_OYAKO:        return 3;
                case KUMI_KAIKYU:       return 0;
                case KUMI_KOSUU:        return 0;
                case KUMI_JUURYO:       return 4;
                case KUMI_OYAKO:        return 5;
                case KUMI_OYAKO_HALF:   return 6;
                default:                return 0;
            }
        }else{
            switch(Num){//6H以上の LED
                case KUMI_NONE:         return 1;
                case KUMI_LIMIT:        return 2;
                case KUMI_RANK:         return 0;
                case KUMI_KAIKYU:       return 0;
                case KUMI_KOSUU:        return 0;
                case KUMI_JUURYO:       return 3;
                case KUMI_OYAKO:        return 4;
                case KUMI_OYAKO_HALF:   return 5;
                case KUMI_TAIMEN:       return 0;
                case KUMI_KAISU1:       return 0;
                case KUMI_KAISU2:       return 0;
                case KUMI_DENSO:        return 0;
                case KUMI_ZANCOM:       return 0;
                default:                return 0;
            }
        }
    }else{
        if(product.limit_on == NIRA_LIMIT){
            switch(Num){//6H以上の LED
                case KUMI_NONE:         return 1;
                case KUMI_LIMIT:        return 2;
                case KUMI_RANK:         return 0;
                //case KUMI_OYAKO:        return 3;
                case KUMI_KAIKYU:       return 0;
                case KUMI_KOSUU:        return 0;
                case KUMI_JUURYO:       return 0;
                case KUMI_OYAKO:        return 4;
                case KUMI_OYAKO_HALF:   return 5;
                default:                return 0;
            }
        }else{
            switch(Num){//6H以上の LED
                case KUMI_NONE:         return 1;
                case KUMI_LIMIT:        return 2;
                case KUMI_RANK:         return 0;
                case KUMI_KAIKYU:       return 0;
                case KUMI_KOSUU:        return 0;
                case KUMI_JUURYO:       return 0;
                case KUMI_OYAKO:        return 3;
                case KUMI_OYAKO_HALF:   return 4;
                case KUMI_TAIMEN:       return 0;
                case KUMI_KAISU1:       return 0;
                case KUMI_KAISU2:       return 0;
                case KUMI_DENSO:        return 0;
                case KUMI_ZANCOM:       return 0;
                default:                return 0;
            }
        }
    }









    
    /*
     if((product.matrix_mode != MATRIX_ON)&&product.head!=1) {
        if(product.Array == 2){
            switch(Num){//6H以上の LED
            
                case KUMI_NONE:         return 1;
                case KUMI_LIMIT:        return 2;
                case KUMI_RANK:         return 3;
                case KUMI_KAIKYU:       return 0;
                case KUMI_KOSUU:        return 0;
                case KUMI_JUURYO:       return 0;
                case KUMI_OYAKO:        return 4;
                case KUMI_OYAKO_HALF:   return 5;
                case KUMI_TAIMEN:       return 6;
                case KUMI_KAISU1:       return 7;
                case KUMI_KAISU2:       return 8;
                case KUMI_DENSO:        return 0;
                case KUMI_ZANCOM:       return 0;
                default:                return 0;
            }
        }else{
            switch(Num){//6H以上の LED
                case KUMI_NONE:         return 1;
                case KUMI_LIMIT:        return 2;
                case KUMI_RANK:         return 3;
                case KUMI_KAIKYU:       return 0;
                case KUMI_KOSUU:        return 0;
                case KUMI_JUURYO:       return 0;
                case KUMI_OYAKO:        return 4;
                case KUMI_OYAKO_HALF:   return 0;
                case KUMI_TAIMEN:       return 0;
                case KUMI_KAISU1:       return 5;
                case KUMI_KAISU2:       return 6;
                case KUMI_DENSO:        return 0;
                case KUMI_ZANCOM:       return 0;
                default:                return 0;
            }
        }
        if(product.head>6){
            switch(Num){//6H以上の LED
                case KUMI_NONE:         return 1;
                case KUMI_LIMIT:        return 2;
                //case KUMI_COMBI:        return 3;
                case KUMI_RANK:         return 3;
                case KUMI_KAIKYU:       return 0;
                case KUMI_KOSUU:        return 0;
                case KUMI_JUURYO:       return 0;
                case KUMI_OYAKO:        return 4;
                case KUMI_OYAKO_HALF:
                    if(product.Array == 2){
                                        return 5;
                    }else{              return 0;
                    }               
                case KUMI_TAIMEN:
                    if(product.Array == 2){
                                        return 6;
                    }else{              return 0;
                    }               
                case KUMI_KAISU1:       return 7;
                case KUMI_KAISU2:       return 8;
                case KUMI_DENSO:        return 9;
                case KUMI_ZANCOM:       return 0;
                default:                return 0;
            }
        }else{  //6H LED
            switch(Num){
                case KUMI_NONE:         return 1;
                case KUMI_LIMIT:        return 2;
                //case KUMI_COMBI:        return 0;
                case KUMI_RANK:         return 3;
                case KUMI_KAIKYU:       return 0;
                case KUMI_KOSUU:        return 0;
                case KUMI_JUURYO:       return 0;
                case KUMI_OYAKO:        return 4;
                case KUMI_OYAKO_HALF:   return 0;
                case KUMI_TAIMEN:       return 0;
                case KUMI_KAISU1:       return 5;
                case KUMI_KAISU2:       return 6;
                case KUMI_DENSO:        return 7;
                case KUMI_ZANCOM:       return 8;
                default:                return 0;
            }
        }
    } else if(product.head==1){//ワンタッチャブルではﾗﾝｸｾﾝﾍﾞﾂ、チェッカ―しかないので、項目の番号をいじる
        switch(Num){
            case KUMI_NONE:         return 0;
            case KUMI_LIMIT:        return 0;
            case KUMI_RANK:         return 1;
            case KUMI_KAIKYU:       return 0;
            case KUMI_KOSUU:        return 2;
            case KUMI_JUURYO:       return 3;
            case KUMI_OYAKO:        return 0;
            case KUMI_OYAKO_HALF:   return 0;
            case KUMI_TAIMEN:       return 0;
            case KUMI_KAISU1:       return 0;
            case KUMI_KAISU2:       return 0;
            case KUMI_PACK:         return 0;
            case KUMI_PACK2:        return 0;
            default:                return 0;
        }
    } else if(product.Array == 1){//ワンタッチャブルではﾗﾝｸｾﾝﾍﾞﾂ、チェッカ―しかないので、項目の番号をいじる
        switch(Num){
            case KUMI_NONE:         return 1;
            case KUMI_LIMIT:        return 2;
            case KUMI_RANK:         return 3;
            case KUMI_KAIKYU:       return 0;
            case KUMI_KOSUU:        return 4;
            case KUMI_JUURYO:       return 5;
            case KUMI_OYAKO:        return 6;
            case KUMI_OYAKO_HALF:   return 0;
            case KUMI_TAIMEN:       return 0;
            case KUMI_KAISU1:       return 0;
            case KUMI_KAISU2:       return 0;
            case KUMI_PACK:         return 0;
            case KUMI_PACK2:        return 0;
            case KUMI_DENSO:        return 0;
            case KUMI_ZANCOM:       return 0;
            case KUMI_ZANCOM2:      return 0;
            case KUMI_COMZAN:       return 0;
            default:                return 0;
        }
    } else if(product.Array == 2){
        switch(Num){
            case KUMI_NONE:         return 1;
            case KUMI_LIMIT:        return 2;
            case KUMI_RANK:         return 3;
            case KUMI_KAIKYU:       return 4;
            case KUMI_KOSUU:        return 5;
            case KUMI_JUURYO:       return 6;
            case KUMI_OYAKO:        return 7;
            case KUMI_OYAKO_HALF:   return 8;
            case KUMI_TAIMEN:       return 9;
            case KUMI_KAISU1:       return 10;
            case KUMI_KAISU2:       return 11;
            case KUMI_DENSO:        return 0;
            case KUMI_ZANCOM:       return 12;
            case KUMI_ZANCOM2:      return 0;
            default:                return 0;
        }
    }else{
        switch(Num){
            case KUMI_NONE:         return 1;
            case KUMI_LIMIT:        return 2;
            case KUMI_RANK:         return 3;
            case KUMI_KAIKYU:       return 4;
            case KUMI_KOSUU:        return 5;
            case KUMI_JUURYO:       return 6;
            case KUMI_OYAKO:        return 7;
            case KUMI_OYAKO_HALF:   return 0;
            case KUMI_TAIMEN:       return 0;
            case KUMI_KAISU1:       return 8;
            case KUMI_KAISU2:       return 9;
            case KUMI_DENSO:        return 10;
            case KUMI_ZANCOM:       return 11;
            case KUMI_ZANCOM2:      return 12;
            default:                return 0;
        }
    }*/
}
/*****************************************************************************
    目標設定を行うときの使用する関数
*****************************************************************************/
void SetteiMokuhyo(int *xNum,data_t limitData,int mode,const int *data)
{
    int Num = *xNum;
    btn.ini();
// 2017.09.29 ver04.02_01  -ーーー  KUMI_KAIKYUの順番が変わってる。注意!!!!
    //組み合わせ方法を選択
    /*char Select[][SIZE]={
        {0x31,0x2E,0xBE,0xB2,0xB9,0xDE,0xDD,0xC5,0xBC,0x00}             //ｾｲｹﾞﾝﾅｼ
        ,{0x32,0x2E,0xBE,0xB2,0xB9,0xDE,0xDD,0xB1,0xD8,0x00}            //ｾｲｹﾞﾝｱﾘ
        ,{0x33,0x2E,0xD7,0xDD,0xB8,0xBE,0xDD,0xCD,0xDE,0xC2,0x00}       //ﾗﾝｸｾﾝﾍﾞﾂ
        ,{0x34,0x2E,0xD7,0xDD,0xB8,0xCD,0xDE,0xC2,' ' ,0xB8,0xD0,0xB1,0xDC,0xBE,0x00}    //ﾗﾝｸﾍﾞﾂ ｸﾐｱﾜｾ
        ,{0x35,0x2E,0xB5,0xD4,0xBA,0xB9,0xB2,0xD8,0xAE,0xB3,0x00}       //ｵﾔｺｹｲﾘｮｳ
        ,{0x36,0x2E,0xC0,0xB2,0xD2,0xDD,' ',0xB5,0xD4,0xBA,0xB9,0xB2,0xD8,0xAE,0xB3,0x00}       //ﾀｲﾒﾝ ｵﾔｺｹｲﾘｮｳ
        ,{0x37,0x2E,0xC0,0xB2,0xD2,0xDD,0x00}                           //ﾀｲﾒﾝ
    };*/
    char Select[][SIZE]= {
         {0x2E,0xBE,0xB2,0xB9,0xDE,0xDD,0xC5,0xBC,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｾｲｹﾞﾝﾅｼ
        ,{0x2E,0xBB,0xD7,0xBD,0xB3,' ' ,0xBE,0xB2,0xB9,0xDE,0xDD,' ' ,' ' ,' ' ,0x00}      //ｻﾗｽｳ ｾｲｹﾞﾝ
        //,{0x2E,0xB8,0xD0,0xB1,0xDC,0xBE,' ' ,0xBE,0xB2,0xB9,0xDE,0xDD,' ' ,' ' ,0x00}      //ｸﾐｱﾜｾ ｾｲｹﾞﾝ
        ,{0x2E,0xD7,0xDD,0xB8,0xBE,0xDD,0xCD,0xDE,0xC2,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾗﾝｸｾﾝﾍﾞﾂ
        ,{0x2E,0xD7,0xDD,0xB8,0xCD,0xDE,0xC2,' ' ,0xB8,0xD0,0xB1,0xDC,0xBE,' ' ,0x00}      //ﾗﾝｸﾍﾞﾂ ｸﾐｱﾜｾ
        //,{0x2E,0xC1,0xAA,0xAF,0xB6,0xB0,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾁｪｯｶｰ
        ,{0x2E,0xBA,0xBD,0xB3,' ' ,0xCB,0xAE,0xB3,0xBC,0xDE,' ' ,' ' ,' ' ,' ' ,0x00}      //ｺｽｳ ﾋｮｳｼﾞ
        ,{0x2E,0xB8,0xDE,0xD7,0xD1,' ' ,0xCB,0xAE,0xB3,0xBC,0xDE,' ' ,' ' ,' ' ,0x00}      //ｸﾞﾗﾑ ﾋｮｳｼﾞ

        ,{0x2E,0xB5,0xD4,0xBA,0xB9,0xB2,0xD8,0xAE,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｵﾔｺｹｲﾘｮｳ
        ,{0x2E,0xC0,0xB2,0xD2,0xDD,' ' ,0xB5,0xD4,0xBA,0xB9,0xB2,0xD8,0xAE,0xB3,0x00}      //ﾀｲﾒﾝ ｵﾔｺｹｲﾘｮｳ
        ,{0x2E,0xC0,0xB2,0xD2,0xDD,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾀｲﾒﾝ
        ,{0x2E,0xB6,0xB2,0xBD,0xB3,' ' ,0xB9,0xB2,0xD8,0xAE,0xB3,' ' ,' ' ,' ' ,0x00}      //ｶｲｽｳ ｹｲﾘｮｳ
        ,{0x2E,0xB6,0xB2,0xBD,0xB3,' ' ,0xB9,0xB2,0xD8,0xAE,0xB3,'2' ,' ' ,' ' ,0x00}      //ｶｲｽｳ ｹｲﾘｮｳ2
        ,{0x2E,0xCA,0xDF,0xAF,0xB8,' ' ,0xC1,0xB4,0xAF,0xB6,0xB0,' ' ,' ' ,' ' ,0x00}      //ﾊﾟｯｸ ﾁｪｯｶｰ
        ,{0x2E,0xCA,0xDF,0xAF,0xB8,' ' ,0xC1,0xB4,0xAF,0xB6,0xB0,'2' ,' ' ,' ' ,0x00}      //ﾊﾟｯｸ ﾁｪｯｶｰ2
        ,{0x2E,0xB5,0xD4,0xBA,0xB9,0xB2,0xD8,0xAE,0xB3,'2' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｵﾔｺｹｲﾘｮｳ2
        ,{0x2E,0xBB,0xDE,0xDD,0xD8,0xAE,0xB3,0xB8,0xD0,0xB1,0xDC,0xBE,' ' ,' ' ,0x00}      //ｻﾞﾝﾘｮｳｸﾐｱﾜｾ
        ,{0x2E,0xBB,0xDE,0xDD,0xD8,0xAE,0xB3,0xB8,0xD0,0xB1,0xDC,0xBE,'2' ,' ' ,0x00}      //ｻﾞﾝﾘｮｳｸﾐｱﾜｾ2
    };
    //組み合わせ指定有りの場合の選択肢
    const char cHead[]= {' ',0xBB,0xD7,0x00};       //ｻﾗ
    
    const char ytitle2[][8]= {                      //モード設定
         {'K',':',0xBA,0xB3,0xBF,0xB8,' ' ,0x00}         //ｺｳｿｸ
        ,{'Y',':',0xD5,0xAF,0xB8,0xD8,' ' ,0x00}         //ﾕｯｸﾘ
        ,{'H',':',0xCA,0xB2,0xBC,0xD5,0xC2,0x00}      //ﾊｲｼｭﾂ
        //,{'D',':',0xC0,0xDE,0xDD,0xCC,0xDF,0x00}         //ﾀﾞﾝﾌﾟ
        //,{'H',':',0xCA,0xB2,0xCA,0xDF,0xB0,0x00}         //ﾊｲﾊﾟｰ
    };
    const char karaMade[2][SIZE]= {
         {' ' ,0xB6,0xD7,' ' ,' ' ,0x00}         //ｶﾗ
        ,{' ' ,0xCF,0xC3,0xDE,' ' ,0x00}   //ﾏﾃﾞ
    };
    const char kokara[2][SIZE]= {{0xC9,0xBA,0xD8,0x00}     //ﾉｺﾘ
        ,{0xBA,0xB6,0xD7,' ' ,0xCB,0xAE,0xB3,0xBC,0xDE,0x00}
    };      //ｺｶﾗ ﾋｮｳｼﾞ
    const char stTitle[][4]= {"OFF","ON"};          //準最適値
    const char combi[][SIZE]= {
         {"OFF"}
        ,{'1' ,0xDA,0xC2,0xC4,' ' ,'2' ,0x2A,'3' ,0xC3,0xDE,' ' ,0xBE,0xB2,0xD8,0xC2,0x00}      //1ﾚﾂﾄ 2*3ﾃﾞ ｾｲﾘﾂ
         //{'2' ,0x2A,'3' ,0xC3,0xDE,' ' ,0xBE,0xB2,0xD8,0xC2,0x00}      //2*3ﾃﾞ ｾｲﾘﾂ
        ,{'1' ,0xDA,0xC2,0xC3,0xDE,' ' ,0xBE,0xB2,0xD8,0xC2,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //1ﾚﾂﾃﾞ ｾｲﾘﾂ
    };

    //ランクカンリ有無を選択
    const char Select1[][SIZE]= {
        {"OFF"}                                         //OFF
        ,{'O','N',' ',0xBB,0xB2,0xBC,0xAE,0xB3,0x00}    //ｻｲｼｮｳ
        ,{'O','N',' ',0xBB,0xB2,0xC0,0xDE,0xB2,0x00}    //ｻｲﾀﾞｲ
        ,{'O','N',' ',0xBB,0xB2,0xC0,0xDE,0xB2,'/',0xBB,0xB2,0xBC,0xAE,0xB3,0x00} //ON
    };

    const char yuusenn[]= {' ' ,0xA6,' ' ,0xD5,0xB3,0xBE,0xDD,0x00};       // ｦ ﾕｳｾﾝ

    const char katamenn[][SIZE]= {{"OFF "}  //off
        ,{0x7F,0x7F,'L' ,'E' ,'D' ,' ' ,' ' ,' ' ,0xBC,0xD6,0xB3,0xBB,0xDE,0xD7,0x7E,0x7E,0x00}    //←←LED   ｼﾖｳｻﾞﾗ→→
        ,{0x7F,0x7F,0xBC,0xD6,0xB3,0xBB,0xDE,0xD7,' ' ,' ' ,' ' ,'L' ,'E' ,'D' ,0x7E,0x7E,0x00}    //←←ｼﾖｳｻﾞﾗ   LED→→
    };
    const char taimenoyako[][SIZE]={{0xB5,0xD4,0xB6,0xDE,0xDC,' ' ,0xBB,0xD7,0xBD,0xB3,0x00}      //ｵﾔｶﾞﾜ ｻﾗｽｳ
                                   ,{0xBA,0xB6,0xDE,0xDC,' ' ,0xBB,0xD7,0xBD,0xB3,0x00}      //ｺｶﾞﾜ ｻﾗｽｳ
    };
    const char LowMidHi[][SIZE]={{"DEFAULT"}      //
                                ,{"LOW    "}      //
                                ,{"MID    "}      //
                                ,{"HIGH   "}      //
    };

    //最小値より低ければ,値を初期化
    if(Num < limitData.min) Num = limitData.min;
    if(Num > limitData.max) Num = limitData.max;
    if(product.head==1 && mode == TL_KUMI_FLAG && Num != KUMI_RANK && Num != KUMI_KOSUU && Num != KUMI_JUURYO && Num != KUMI_PACK && Num != KUMI_PACK2) {
        Num = KUMI_RANK;
    }
    if(mode == TL_KATAMEN_HYOUJI) {
        lcd.locate(0,1);
        lcd.printf(katamenn[Num]);
        katamen = Num;
        load.katamen = Num;
        set_stopcell();
        led.REGIS(0xfff^stop_cellsd);
    }
//printf("%d%d\r\n",Num,mode);
    while(btn.get_btn() != BTN_ENT) {
        if(btn.get_one_push()) {
            lcd.blank(1);
            //組み合わせ方法選択
            if(mode == TL_KUMI_FLAG) {
                //マトリクス以外はランク別組合せができないので、項目の番号をいじる
                lcd.printf("%d",set_mokuhyo_no(Num));
                /*
                if((Num >= KUMI_KAIKYU || Num >= KUMI_KOSUU) && (product.productType %100 != MATRIX && product.matrix_mode != MATRIX_ON)&&product.head!=1) {
                    lcd.printf("%d",Num+1-2);
                } else if(Num >= KUMI_KAIKYU || Num >= KUMI_KOSUU)
                } else if(product.head==1){//ワンタッチャブルではﾗﾝｸｾﾝﾍﾞﾂ、チェッカ―しかないので、項目の番号をいじる
                    if(Num==KUMI_RANK){
                        lcd.printf("%d",1);
                    }else if(Num==KUMI_KOSUU){
                        lcd.printf("%d",2);
                    }
                }else{
                    lcd.printf("%d",Num+1);
                }*/
                lcd.printf(Select[Num]);
            }

            //組み合わせ方法表示
            else if(mode == TL_KUMI_MIN || mode == TL_KUMI_MAX) {
                if(data[TL_KUMI_FLAG]==KUMI_PACK || data[TL_KUMI_FLAG]==KUMI_PACK2){
                    lcd.blank(1);
                    lcd.printf("%02d",Num);
                }else {
                    if(data[TL_KUMI_FLAG]==KUMI_OYAKO_HALF){
                        lcd.blank(0);
                        lcd.printf(taimenoyako[0]);
                    }
                    if(data[TL_KUMI_FLAG]==KUMI_OYAKO){
                        lcd.blank(0);
                        lcd.printf(taimenoyako[1]);
                    }
                    lcd.blank(1);
                    lcd.printf("%02d%s%s",Num+1,cHead,karaMade[mode-2]);
                }
            }
            //優先組み合わせ方法表示
            else if(mode == TL_KUMI_PRIORITY_1 || mode == TL_KUMI_PRIORITY_2) {
                if(data[TL_KUMI_FLAG]==KUMI_OYAKO_HALF||data[TL_KUMI_FLAG]==KUMI_OYAKO){
                    lcd.blank(0);
                    lcd.printf(taimenoyako[1]);
                    lcd.blank(1);
                    lcd.printf("%02d%s%s",Num+1,cHead,karaMade[mode-4]);
                }else{
                    if  (Num == limitData.max) {
                        lcd.blank(1);
                        lcd.printf("OFF");
                        //Num=999;
                    } else {
                        lcd.blank(1);
                        lcd.printf("%02d%s%s",Num+1,cHead,yuusenn);
                    }
                }
            } else if(mode == TL_SECOND_ON_OFF) {
                lcd.printf(stTitle[Num]);   //準最適値
            } else if(mode == TL_SPMODE) {
                lcd.printf(ytitle2[Num]);   //ゆっくモード設定
            } else if(mode == TL_RANK_ON_OFF) {
                lcd.printf(Select1[Num]);   //ランクカンリ設定
            } else if(mode == TL_KATAMEN_HYOUJI) {
                lcd.blank(1);
                lcd.printf(katamenn[Num]);
                katamen = Num;
                load.katamen = Num;
                set_stopcell();
                led.REGIS(0xfff^stop_cellsd);//ｶﾀﾒﾝｼﾖｳ
            }else if(mode == TL_COMBI_FLAG) {
                lcd.printf(combi[Num]);   //クミアワセ設定

            //組み合わせ方法表示
            }else if(mode == TL_CHECK_HYOUUJI) {
                lcd.printf("%s%02d%s",kokara[0],Num+1,kokara[1]);
            }else if(mode == TL_KURI_FLAG) {
                lcd.printf(LowMidHi[Num]);
            }
            
        }

        btn.new_btn();              //スイッチ(New)取得
        btn.next_or_back(&Num, limitData.min, limitData.max);
        //組合せ優先で、ﾕｳｾﾝ1と同じ値になったときに(無駄なので)一個飛ばす
        if(mode == TL_KUMI_PRIORITY_2 && Num == data[TL_KUMI_PRIORITY_1]&&data[TL_KUMI_FLAG]!=KUMI_OYAKO_HALF) {
            btn.next_or_back(&Num, limitData.min, limitData.max);
        }
/*
        //マトリクス以外はランク別組合せができないので、項目をすっ飛ばす
        if(product.productType %100 != MATRIX && product.matrix_mode != MATRIX_ON) {
            //if((btn.get_btn() == BTN_DOWN && Num == KUMI_KAIKYU - 1 )||(btn.get_btn() == BTN_UP && Num == KUMI_KAIKYU + 1 )){
            if(mode==TL_KUMI_FLAG && (Num == KUMI_KAIKYU || Num == KUMI_KOSUU)) {
                btn.next_or_back(&Num, limitData.min, limitData.max);
            }
            if(mode==TL_KUMI_FLAG && (Num == KUMI_KAIKYU || Num == KUMI_KOSUU)) {
                btn.next_or_back(&Num, limitData.min, limitData.max);
            }
        }
        //チェッカーは2項目以外をすっ飛ばす
        while(product.head==1 && mode==TL_KUMI_FLAG && Num != KUMI_RANK && Num != KUMI_KOSUU){
            btn.next_or_back(&Num, limitData.min, limitData.max);
        }*/
        while(mode == TL_KUMI_FLAG && set_mokuhyo_no(Num)==0){
            btn.next_or_back(&Num, limitData.min, limitData.max);
        }
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return;         //esc押下なら処理終了
        }

        btn.old_btn();  //スイッチ更新
    }//while
    *xNum = Num;
}

/*****************************************************************************
    目標設定を行うときの使用する関数 (ランククミアワセ以降の各設定値)
*****************************************************************************/
void SetteiMokuhyo2(int *xNum,data_t limitData,int mode,int kumi_flug)
{
    int Num = *xNum;
    btn.ini();
    const char kumiyu[3][9] = {{"OFF"}
        ,{0xB5,0xB5,0xB7,0xB2,' ' ,0xCE,0xB3,0x00}    //ｵｵｷｲ ﾎｳ
        ,{0xC1,0xB2,0xBB,0xB2,' ' ,0xCE,0xB3,0x00}
    };    //ﾁｲｻｲ ﾎｳ
    const char kumir[7][SIZE] = {{"OFF"}
        ,{0xB1,0xB6,0x3A,'A' ,' ' ,0xD0,0xC4,0xDE,0x3A,'B' ,0x00}    //ｱｶ:A ﾐﾄﾞ:B
        ,{0xB1,0xB6,0x3A,'A' ,' ' ,0xD0,0xC4,0xDE,0x3A,'B' ,0x00}    //ｱｶ:A ﾐﾄﾞ:B
        ,{0xB1,0xB6,0x3A,'A' ,' ' ,0xD0,0xC4,0xDE,0x3A,'B' ,' ' ,0xB7,0xB2,0xDB,0x3A,'C' ,0x00}    //ｱｶ:A ﾐﾄﾞ:B ｷｲﾛ:C
        ,{0xB1,0xB6,0x3A,'A' ,' ' ,0xD0,0xC4,0xDE,0x3A,'B' ,' ' ,0xB7,0xB2,0xDB,0x3A,'C' ,0x00}    //ｱｶ:A ﾐﾄﾞ:B ｷｲﾛ:C
        ,{0xB1,0x3A,'A' ,' ' ,0xD0,0x3A,'B' ,' ' ,0xB7,0x3A,'Y' ,' ' ,0xC5,0xBC,0x3A,'D' ,0x00}    //ｱ:A ﾐ:B ｷ:Y ﾅｼ:D
        ,{0xB1,0x3A,'A' ,' ' ,0xD0,0x3A,'B' ,' ' ,0xB7,0x3A,'Y' ,' ' ,0xC5,0xBC,0x3A,'D' ,0x00}    //ｱ:A ﾐ:B ｷ:Y ﾅｼ:D
    };
    const char rankcolor[2][SIZE] = {
          {0xB1,0xB6,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}    //ｱｶ
         ,{0xD0,0xC4,0xDE,0xD8,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}    //ﾐﾄﾞﾘ
    };
    /*
    const char fastrank[9][SIZE] = {
          {'3' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'2' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{' ' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{' ' ,'M' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{' ' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'2' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'3' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}
         ,{'4' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}
         ,{'5' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}}; 
    */
    const char selrank[3][SIZE] = {
          {0xC2,0xB3,0xBC,0xDE,0xAE,0xB3,' ' ,0x28,'M' ,',' ,'L' ,0x2E,0x2E,0x2E,0x29,0x00}      //ﾂｳｼﾞｮｳ (M,L...)
         ,{'5' ,'k' ,'g' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //5kg
         ,{'1' ,'0' ,'k' ,'g' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}};    //10kg

    //組み合わせ指定有りの場合の選択肢
    const char cHead[]= {' ',0xBB,0xD7,0x00};       //ｻﾗ

    const char karaMade[][SIZE]= {
        {' ',0xB6,0xD7}         //ｶﾗ
        ,{' ',0xCF,0xC3,0xDE}   //ﾏﾃﾞ
    };
    const char stTitle[][4]= {"OFF","ON"};
    //最小値より低ければ,値を初期化
    if(Num < limitData.min) Num = limitData.min;
    while(btn.get_btn() != BTN_ENT) {
        if(btn.get_one_push()) {
            lcd.blank(1);

            //組み合わせ方法表示
            if(mode == TL_KUMI_MIN || mode == TL_KUMI_MAX) {
                lcd.printf("%02d%s%s",Num+1,cHead,karaMade[mode-2]);
            } else if(mode == TL_RANK_ON_OFF) {
                lcd.printf(stTitle[Num]);
            }//ランクカンリ設定
        } else if(mode == TL2_END+5 && kumi_flug==KUMI_RANK) {
            lcd.locate(0,1);
            lcd.printf(selrank[Num]);//最小ﾗﾝｸ設定    
        } else if(mode == TL2_END+6 && kumi_flug==KUMI_RANK) {
            lcd.locate(0,1);
            lcd.printf(rankcolor[Num]);//最小ﾗﾝｸ設定  
        } else if(mode == TL2_END && kumi_flug==KUMI_KAIKYU) {
            lcd.locate(0,1);
            lcd.printf(kumiyu[Num]);//ランクカンリ設定
        } else if(mode == TL2_END && kumi_flug== KUMI_RANK && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON || product.productType == ITIGO || product.productType == ITIGO_D)) {
            lcd.locate(0,1);
            lcd.printf(kumir[Num]);//ランクカンリ設定
            switch(Num) {
                case 0:
                    for(int i=0; i<product.head; i++) {
                        iro[i]=C_NULL;
                    }
                    break;
                case 1:
                    for(int i=0; i<product.head; i++) {
                        if(product.head>6) {
                            if(i<6) {
                                iro[i]=PENTA_R;
                            } else {
                                iro[i]=PENTA_G;
                            }
                        } else {
                            if(i<3) {
                                iro[i]=PENTA_R;
                            } else {
                                iro[i]=PENTA_G;
                            }
                        }
                    }
                    break;
                case 2:
                    for(int i=0; i<product.head; i++) {
                        if(product.head>6) {
                            if(i<6) {
                                iro[i]=PENTA_G;
                            } else {
                                iro[i]=PENTA_R;
                            }
                        } else {
                            if(i<3) {
                                iro[i]=PENTA_G;
                            } else {
                                iro[i]=PENTA_R;
                            }
                        }

                    }
                    break;
                case 3:
                    for(int i=0; i<product.head; i++) {
                        if(i<6) {
                            iro[i]=PENTA_R;
                        } else if(i<9) {
                            iro[i]=PENTA_G;
                        } else {
                            iro[i]=PENTA_Y;
                        }
                    }
                    break;
                case 4:
                    for(int i=0; i<product.head; i++) {
                        if(i<3) {
                            iro[i]=PENTA_G;
                        } else if(i<6) {
                            iro[i]=PENTA_Y;
                        } else {
                            iro[i]=PENTA_R;
                        }
                    }
                    break;
                case 5:
                    for(int i=0; i<product.head; i++) {
                        if(i<3) {
                            iro[i]=PENTA_R;
                        } else if(i<6) {
                            iro[i]=PENTA_G;
                        } else if(i<9) {
                            iro[i]=PENTA_Y;
                        } else {
                            iro[i]=C_NULL;
                        }
                    }
                    break;
                case 6:
                    for(int i=0; i<product.head; i++) {
                        if(i<3) {
                            iro[i]=C_NULL;
                        } else if(i<6) {
                            iro[i]=PENTA_Y;
                        } else if(i<9) {
                            iro[i]=PENTA_G;
                        } else {
                            iro[i]=PENTA_R;
                        }
                    }
                    break;

            }
        }

        btn.new_btn();              //スイッチ(New)取得
        btn.next_or_back(&Num, limitData.min, limitData.max);

        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if( btn.get_btn() == BTN_KEEP_UP || btn.get_btn() == BTN_KEEP_DOWN) {
            wait(0.1);
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return;         //esc押下なら処理終了
        }

        btn.old_btn();  //スイッチ更新
    }//while
    *xNum = Num;
}

void Settei_suru_sinai(int *xNum,data_t limitData,int mode,int kumi_flug)
{
    int Num = *xNum;
    btn.ini();
    const char stTitle[][4]= {{0xBC,0xC5,0xB2,0x00},{0xBD,0xD9,' ',0x00}};  //ｽﾙ //ｼﾅｲ
    //最小値より低ければ,値を初期化
    if(Num < limitData.min) Num = limitData.min;
    if(Num > limitData.max) Num = limitData.max;
    
    while(btn.get_btn() != BTN_ENT) {
        if(btn.get_one_push()) {
            lcd.blank(1);
            if(mode == TL_RANK_ON_OFF) {
                lcd.printf(stTitle[Num]);
            }//ランクカンリ設定
        } 

        btn.new_btn();              //スイッチ(New)取得
        btn.next_or_back(&Num, limitData.min, limitData.max);

        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            Num = 99;
            *xNum = Num;
            btn.old_btn();  //スイッチ更新
            return;         //esc押下なら処理終了
        }

        btn.old_btn();  //スイッチ更新
    }//while
    *xNum = Num;
}
void Setteirank(int *xNum,data_t limitData,int mode,int kumi_flug ,int i ,int en1 ,int en2 ,int en3 ,int en4 ,int en5 ,int en6 ,int en7 ,int en8 ,int en9)
{
    int Num = *xNum;
    //int bNum=0;
    btn.ini();
    int matrix_RG_SA = CG_RANK_START-C_RANK_START;
    const char fastrank[21][SIZE] = {
          {'9' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'8' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'7' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'6' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'5' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'4' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'3' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'2' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{' ' ,'S' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{' ' ,'M' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{' ' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'2' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00} 
         ,{'3' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}
         ,{'4' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}
         ,{'5' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}
         ,{'6' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}
         ,{'7' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}
         ,{'8' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}
         ,{'9' ,'L' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}
         ,{'S' ,'A' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}
         ,{'M' ,'A' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}}; 
    //最小値より低ければ,値を初期化
    if(Num < limitData.min) Num = limitData.min;
    if(Num > limitData.max) Num = limitData.max;
    if(en1>CG_RANK_START){   //他のランクは赤/緑を認識できないので、ここで調整する(調整しないと同じランクが発生する)
        en1 = en1 - matrix_RG_SA;
    }else if(en2>CG_RANK_START){
        en2 = en2 - matrix_RG_SA;
    }else if(en3>CG_RANK_START){
        en3 = en3 - matrix_RG_SA;
    }else if(en4>CG_RANK_START){
        en4 = en4 - matrix_RG_SA;
    }else if(en5>CG_RANK_START){
        en5 = en5 - matrix_RG_SA;
    }else if(en6>CG_RANK_START){
        en6 = en6 - matrix_RG_SA;
    }else if(en7>CG_RANK_START){
        en7 = en7 - matrix_RG_SA;
    }else if(en8>CG_RANK_START){
        en8 = en8 - matrix_RG_SA;
    }else if(en9>CG_RANK_START){
        en9 = en9 - matrix_RG_SA;
    }
    
    lcd.blank(LOW);
    lcd.locate(0,1);
    while(btn.get_btn() != BTN_ENT) {
        lcd.locate(0,1);
        if(C_9S<=Num && Num<=C_MA){
            lcd.printf(fastrank[Num-C_9S]);//最小ﾗﾝｸ設定 
            iro[0]=Num + rank_RG * matrix_RG_SA;
        }else{
            lcd.printf("%2d",Num-(CR_01-1));
            iro[0]=Num + rank_RG * matrix_RG_SA;
        }
        //bNum = Num;
        btn.new_btn();              //スイッチ(New)取得
        //btn.next_or_back(&Num, limitData.min, limitData.max);
        btn.puls_or_minus(&Num,1,1);
        if(Num < limitData.min) Num = limitData.max;
        if(Num > limitData.max) Num = limitData.min;
        while((i>=2 && Num== en1) || (i>=3 && Num== en2) || (i>=4 && Num== en3) || (i>=5 && Num== en4) ||
              (i>=6 && Num== en5) || (i>=7 && Num== en6) || (i>=8 && Num== en7) || (i>=9 && Num== en8) ||
              (i>=2 && Num== en1+matrix_RG_SA) || (i>=3 && Num== en2+matrix_RG_SA) || (i>=4 && Num== en3+matrix_RG_SA) || (i>=5 && Num== en4+matrix_RG_SA) ||
              (i>=6 && Num== en5+matrix_RG_SA) || (i>=7 && Num== en6+matrix_RG_SA) || (i>=8 && Num== en7+matrix_RG_SA) || (i>=9 && Num== en8+matrix_RG_SA) ||
              (i>=2 && Num== en1-matrix_RG_SA) || (i>=3 && Num== en2-matrix_RG_SA) || (i>=4 && Num== en3-matrix_RG_SA) || (i>=5 && Num== en4-matrix_RG_SA) ||
              (i>=6 && Num== en5-matrix_RG_SA) || (i>=7 && Num== en6-matrix_RG_SA) || (i>=8 && Num== en7-matrix_RG_SA) || (i>=9 && Num== en8-matrix_RG_SA)  
              ){
            btn.puls_or_minus(&Num,1,1);
            if(btn.get_btn() != BTN_UP && btn.get_btn() != BTN_DOWN && btn.get_btn() != BTN_KEEP_UP && btn.get_btn() != BTN_KEEP_DOWN){
                Num++;
            }
            if(Num < limitData.min) Num = limitData.max;
            if(Num > limitData.max) Num = limitData.min;
        }
        
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return;         //esc押下なら処理終了
        }
        if(btn.get_btn() == BTN_KEEP_UP || btn.get_btn() == BTN_KEEP_DOWN){
            wait(0.1);
        }
        if(btn.any_press()==true && btn.get_btn()!=BTN_NONE ){
            lcd.blank(LOW);
            lcd.locate(0,1);
        }
        btn.old_btn();  //スイッチ更新
    }//while
    *xNum = Num + rank_RG * matrix_RG_SA;
    //pc.printf("No:%4d\r\n",Num + rank_RG * matrix_RG_SA);
}


//トレーニングモードを選択
void selectTrainingMode()
{
    int i = 0;
    btn.ini();  //ボタン初期化

    while(1) {
        //操作に対応した文字表示
        if(btn.get_one_push()) {
            lcd.messege( TOP, DIS_MODE_SELECT, CLS);
            lcd.messege( LOW, DIS_NORMAL + i);  //ノーマルorサウンド表示
        }

        btn.new_btn();              //スイッチ(New)取得
        btn.next_or_back(&i, 0, 1);

        btn.old_btn();  //スイッチ更新

        //モード選択されたらトレーニングスタート
        if(btn.get_btn() == BTN_ENT) {
            se.Touch_01();
            startTraining(i);
            btn.ini();
        }

        //escなら処理終了
        if( btn.esc() ) return;
    }
}

//トレーニング開始
void startTraining(int treMode)
{
    int Training_cnt=0;     //トレーニング時の点灯回数
    const int timeSecond = 30;    //トレーニング時間
    const int ledNum = 4;         //点灯数
    int ledOn = 0,ledOff=0; //点灯,消灯させる場所
    int inNow=0,inOld=0;    //現在、前回の押された場所を保持
    int treSondCnt = 0;     //トレーニングサウンドの音声出力間隔
    char mode[][SIZE]= {
        {' ',0xC9,0x2D,0xCF,0xD9,0x00}          //→ﾉｰﾏﾙ
        ,{' ',0xBB,0xB3,0xDD,0xC4,0xDE,0x00}    //→ｻｳﾝﾄﾞ
    };

    btn.ini();
    led.ledAllOff();
    lcd.messege(TOP, DIS_TRAINING, CLS);
    lcd.printf(mode[treMode]);              //(選択されたモード)

    t.reset();
    t.start();

    while(1) {
        btn.new_btn();              //スイッチ(New)取得

        //esc_swを押すと、Menu画面に戻る
        if(btn.esc()) {
            se.Touch_01();
            t.stop();
            btn.old_btn();       //スイッチ更新
            led.ledAllOff();    //LED消灯
            return;
        }//if
        btn.old_btn();  //スイッチ更新

        //規定時間の間はLED点灯
        if(t.read() < timeSecond) {
            inNow = 0;
            //LED点灯無くなったら、再点灯
            if(!ledOn) {
                for(int i = 0; i < ledNum; i++) {
                    ledOn |= oneBit[rand() % (product.head)]; //LED光らせる場所を取得
                }

                if(treMode) {
                    se.combiSoundAll(ledOn);    //組み合わせ場所を効果音で通知
                }
            }//if

            //サウンドモード時に一定の間隔で音を鳴らす
            if(treMode && treSondCnt == 0) {
                se.combiSoundOne(ledOn);
            }

            //計量皿に物が載っているか確認
            for(int i=0; i < product.head; i++) {
                //重量が載っていたら
                setHeadVolt(i, KURI_TRE);
                if(cel.getWeight(i) > TRE_WHT) inNow += oneBit[i];
            }//for

            //消灯場所取得(点灯場所と押された場所が一致している)
            ledOff = ledOn & inNow;

            //新旧の押下場所を取得して、更に新たに押された場所のみ取得
            //LED点灯箇所以外を押下していたら警告音
            if(((inNow | inOld) ^ inOld) - ledOff) {
                se.mistake();
            }
            inOld = inNow;
            ledOn -= ledOff;                    //消灯されるLEDがあれば適用
            treSondCnt++;                       //カウントアップ

            //規定回数以上だったら初期化
            if(treSondCnt >= 5) {
                treSondCnt = 0; //次はサウンドを鳴らす
            }

            led.REGIS(ledOn);   //LED点灯

            //消灯したら音出力
            if(!ledOn) {
                Training_cnt++;
                lcd.hit(Training_cnt,(int)t.read() % timeSecond);
                se.Touch_01();
            }
        }//if Training_cnt
        //トレーニング終了したら
        else {
            lcd.hit(Training_cnt, timeSecond);
            led.REGIS(0);
            led.RedOn();
            t.stop();
        }
    }//while
}

bool parameter_select(int Pass, int i){
    
    if(Pass == PASS_DEFAULT_PARAMETER ) {
        return true;
    }else if(Pass == PASS_VIEW_PARAM || Pass == PASS_VIEW_PARAM2) {
        switch(i){
            case P_hundo:
            case P_KURI:
            case P_KURI_Y:
            case P_KURI_J:
            case P_CUT:
            case P_CUT_Y:
            case P_sabun:
            
            case P_zerocount:
            case P_ZEROTOLERANCE:
            case P_ZEROAREA:
            case P_BARATSUKI:
            case P_buffer:
            case P_BARATSUKI_R:
            case P_BARATSUKI_GRAM:
            case P_buffer2:
            case P_bufferT:
                return true;
            default :
                return false;
        }
    }
    return false;
}

void change_parameter(int Pass){
    bool use_check=false;
    sentaku_back_flg=false;
    //パラメーターが変更されたら上書き
    file.read_all_file();               //FileRead
    for(int i =0;i<P_END;i++){
        if(i<0){return;}//0以下は終了
            use_check = parameter_select(Pass,i);
            if(use_check == true){
                set_sercret_parameter_one(i,0);
            }
        if(btn.get_btn() == BTN_ESC){
            i -= 2;
            sentaku_back_flg = true;
            //intf("back_flg%d",sentaku_back_flg);
        }else{
            sentaku_back_flg = false;
        }
    }
    set_sercret_parameter_one(P_END,1); //保存指令付
    
}



//パスワード入力
void password()
{
    int Pass = 0;
    int lim_1000=0;
    int lim_0100=0;
    int lim_0010=0;
    int lim_0001=0;
    int lim_min=0;
    int lim_max=9999;
    sentaku_back_flg=false;
    lcd.messege( TOP, DIS_PASS_INPUT, CLS);
    bool testmode_change = false;
    
    for(int i=0;i<4;i++){
        if(i==0){
            lim_min=0       *1000+lim_0100*100+lim_0010*10+lim_0001;
            lim_max=9       *1000+lim_0100*100+lim_0010*10+lim_0001;
            //printf("%8d %8d :%2d %2d%2d %2d\r\n",lim_min,lim_max,lim_1000,lim_0100,lim_0010,lim_0001);
            data_t limitData= {lim_min, lim_max, 1000}; //limit値
            lcd.locate(9,1);
            lcd.printf("^   ");
            Chenge_vaule_digit(&Pass, limitData,4,0,9);
            lim_1000= (Pass-(Pass%1000))/1000;
        }else if(i==1){
            lim_min=lim_1000*1000+0       *100+lim_0010*10+lim_0001;
            lim_max=lim_1000*1000+9       *100+lim_0010*10+lim_0001;
            data_t limitData= {lim_min, lim_max,  100}; //limit値
            lcd.locate(9,1);
            lcd.printf(" ^  ");
            Chenge_vaule_digit(&Pass, limitData,4,0,9);
            lim_0100= (Pass-lim_1000*1000-(Pass%100))/100;
        }else if(i==2){
            lim_min=lim_1000*1000+lim_0100*100+0       *10+lim_0001;
            lim_max=lim_1000*1000+lim_0100*100+9       *10+lim_0001;
            data_t limitData= {lim_min, lim_max,  10}; //limit値
            lcd.locate(9,1);
            lcd.printf("  ^ ");
            Chenge_vaule_digit(&Pass, limitData,4,0,9);
            lim_0010= (Pass-lim_1000*1000-lim_0100*100-(Pass%10))/10;
        }else if(i==3){
            lim_min=lim_1000*1000+lim_0100*100+lim_0010*10+0;
            lim_max=lim_1000*1000+lim_0100*100+lim_0010*10+9;
            data_t limitData= {lim_min, lim_max,  1}; //limit値
            lcd.locate(9,1);
            lcd.printf("   ^");
            Chenge_vaule_digit(&Pass, limitData,4,0,9);
            lim_0001= (Pass%10);
        }
        if(btn.esc()) {
            i=i-2;
            if(i==-2) {
                lcd.cls();
                return;
            }
        }
    }
    //data_t limitData= {0, 9999, 1}; //limit値
    //Chenge_vaule(&Pass, limitData);

    //esc押されたのでMENUへ
    
    if(btn.esc()) {
        return;
    }
    if(Pass == TEST_MODE1) {
        lcd.locate(0,1);
        if(testmode1 == false){  
            testmode1 = true;
            lcd.printf("test1_on ");
        }else{
            testmode1 = false;
            lcd.printf("test1_off");
        }
        testmode_change = true;
    }else if(Pass == TEST_MODE2) {
        lcd.locate(0,1);
        if(testmode2 == false){  
            testmode2 = true;
            lcd.printf("test2_on ");
        }else{
            testmode2 = false;
            lcd.printf("test2_off");
        }
        testmode_change = true;
    }else if(Pass == TEST_MODE3) {
        lcd.locate(0,1);
        if(testmode3 == false){  
            testmode3 = true;
            lcd.printf("test3_on ");
        }else{
            testmode3 = false;
            lcd.printf("test3_off");
        }
        testmode_change = true;
    }else{
        //testmode1 = false;
        //testmode2 = false;
        //testmode3 = false;
    }
    

    if(Pass == PRINT_ON) {
        lcd.locate(0,1);
        if(print_flg == false){  
            print_flg = true;
            lcd.printf("print_on ");
        }else{
            print_flg = false;
            lcd.printf("print_off");
        }
    }

    
    if(Pass == PASS_TOP_USER) {
        TOP_USER = true ;
    }else{
        TOP_USER = false;
    }
        
    
    
    
    if(Pass == PASS_ADMIN || Pass == PASS_ADMIN2) {
        gLevel = 1;
    } else if(Pass == PASS_SECRET) {
        gLevel = 0;
        //パラメーターが変更されたら上書き
        set_sercret_product();
    } else if(Pass == PASS_SECRET2) {
        gLevel = 0;
        set_sercret_parameter();
    } else if(Pass == PASS_DEFAULT_PARAMETER){
        gLevel = 0;
        change_parameter(Pass);
    } else if(Pass == PASS_DEFAULT_PARAMETER || Pass == PASS_DEFAULT_PARAMETER ){
        gLevel = 0;
        change_parameter(Pass);
        
        
        
    }else if(Pass == SERVO_CHECK){
        result_LCD_servo2();
        
        
        
    } else if(Pass == PASS_VIEW_PARAM || Pass == PASS_VIEW_PARAM2) {
        gLevel = 0;
        change_parameter(Pass);
    } else if(Pass == PASS_LIMITAD) {
        gLevel = 0;
        set_limit_count();
    } else if(Pass == PASS_MATRIX || Pass == PASS_MATRIX) {  //マトリクステストモード
        if(now_TC_mode == -1){
            gLevel = 99;
        }else if(now_TC_mode == 0){
            matrix_test();
        }
        //matrix_test();
    } else if(Pass == PASS_DEMO) {
        demo_flg = true;
        gLevel=0;
                                //状態を初期表示に
    /*} else if(Pass == TEST_Ver1T  ) {    
        DEBUG_MODE = TEST_Ver1T;
        gLevel = 1;
    } else if(Pass == TEST_Ver2T  ) {      
        DEBUG_MODE = TEST_Ver2T;
        gLevel = 1;
    } else if(Pass == TEST_Ver3T  ) {      
        DEBUG_MODE = TEST_Ver3T;
        gLevel = 1;*/
    } else if(DEBUG_MODE != 0 && Pass == TEST_CHANGE){
        lcd.cls();
        lcd.printf("SPAN_KURIKAESI");
        change_data(&span_OK_kaisu ,1 ,30);
        lcd.cls();
        lcd.printf("KEIRYO_KURIKAESI");
        change_data(&keiryou_kurikaesi ,1 ,30);
        lcd.cls();
        
    }else if(testmode_change == true){wait(2.0);
    } else {        //不一致
        se.mistake();                   //警告音
        lcd.messege(TOP, DIS_PASS_ERR, CLS);
        wait(2.0);
        gLevel=0;                        //状態を初期表示に
    }
    lcd.cls();
    return;
}

void change_data(int *xNum ,int limit_min ,int limit_max){
    int Num = *xNum;
    btn.ini();

    if(Num < limit_min) Num = limit_min;
    if(Num > limit_max) Num = limit_max;
    
    while(btn.get_btn() != BTN_ENT) {
        if(btn.get_one_push()) {
            lcd.blank(1);
            lcd.locate(2 ,1);
            lcd.printf("%3d",Num);

            
        }

        btn.new_btn();              //スイッチ(New)取得
        btn.next_or_back(&Num, limit_min, limit_max);

        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if( btn.get_btn() == BTN_KEEP_UP || btn.get_btn() == BTN_KEEP_DOWN) {
            wait(0.1);
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return;         //esc押下なら処理終了
            //return xNum;         //esc押下なら処理終了
        }

        btn.old_btn();  //スイッチ更新
    }//while
    *xNum = Num;
    //return Num;
}
    
    
    
    
    


void set_limit_count()
{
    file.read_limit1();
    file.read_limit2();
    data_t limitData;
    int data[] = {KUMI_limit,1};
    for(int i= 0; i <2; i++) {
        //規定回数以上Escボタン押下されたのでメニューに戻る
        if(i < 0) {
            return;
        }
        if(i==0) {
            lcd.locate(0,0);
            lcd.printf("                ");
            lcd.locate(0,1);
            lcd.printf("                ");
            lcd.locate(0,0);
            lcd.printf("limit change");
            limitData.min = 1;
            limitData.max = 9999;
            limitData.lcd = 1;
        } else if(i==1) {
            lcd.locate(0,0);
            lcd.printf("                ");
            lcd.locate(0,1);
            lcd.printf("     now:       ");
            lcd.locate(0,1);
            lcd.locate(0,0);
            lcd.printf("count reset?");
            limitData.min = KUMI_now;
            limitData.max = 1;
            limitData.lcd = 90;
        }

        Chenge_vaule2(&data[i],limitData);
        if(btn.esc()) {
            i -= 2;
            return;
        }
    }
    if(data[1]!=0) {
        KUMI_now =0;
    }
    file.save_limit1(data[0]);
    file.save_limit2(KUMI_now);
    file.read_limit1();
    file.read_limit2();
    return;
}


int off_on_no_yes_result(int off_no,int now){
    const char stTitle[][3][5]= {{"OFF","ON"},{"NO","YES"},{{0xBC,0xC5,0xB2,0x00},{0xBD,0xD9,0x00}}};          //ｼﾅｲ//ｽﾙ 
    
    if(now < 0) now = 0;
    if(now > 1) now = 1;
    
    lcd.blank(1);
    lcd.printf(stTitle[off_no][now]);
    btn.new_btn();              //スイッチ(New)取得
    btn.old_btn();  //スイッチ更新

    while(btn.get_btn() != BTN_ENT) {
        if(btn.get_one_push()) {
            lcd.blank(1);
            lcd.printf(stTitle[off_no][now]);
        }
        btn.new_btn();              //スイッチ(New)取得
        btn.next_or_back(&now, 0, 1);
        
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return now;         //esc押下なら処理終了
        }
        btn.old_btn();  //スイッチ更新
    }
    return now;
}

int RorG_result(int now ,int rank_no){
    pc.printf("%d\r\n",now);
    int matrix_RG_SA = CG_RANK_START-C_RANK_START; //ﾏﾄﾘｸｽで緑と赤のenum差を埋めるための数
    const char stTitle[][5]= {{0xB1,0xB6,0x00},{0xD0,0xC4,0xDE,0xD8,0x00}};          //アカ、ミドリ
    if(now < 0) now = 0;
    if(now > 1) now = 1;
    if(rank_no >= CG_9S){
        rank_no = rank_no - matrix_RG_SA;
    }
        
    lcd.blank(1);
    lcd.printf(stTitle[now]);
    btn.new_btn();              //スイッチ(New)取得
    btn.old_btn();  //スイッチ更新
    for(int i=0;i<product.head;i++){
        iro[i]=C_NULL;
    }
    iro[0]= rank_no + now * matrix_RG_SA;
    while(btn.get_btn() != BTN_ENT) {
        if(btn.get_one_push()) {
            lcd.blank(1);
            lcd.printf(stTitle[now]);
            iro[0]= rank_no + now * matrix_RG_SA;
            printf("00:%d\r\n",now);
        }
        btn.new_btn();              //スイッチ(New)取得
        btn.next_or_back(&now, 0, 1);
        
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return now;         //esc押下なら処理終了
        }
        btn.old_btn();  //スイッチ更新
    }
    return now;
}


/*****************************************************************************
    1ヘッドずつの重量を量る
*****************************************************************************/
void start_keiryo(int head)
{
    const char antei[]= {0xA1,0x00};          //(〇表示)
    int i = 0;
    int keiryo_demo = 0; //疑似的に組合せ計量と同じような待機時間を作る
    btn.ini();
    led.ledAllOff();       // LED消灯
    int servo_test = 0;
    int servo_onoff = 0;
    int syousuuten = 0;
    int autozeroONOFF = 0;

    file.read_span_preset(file.get_preset());
    //if(now_TC_mode >= 1 && product.limit_on == NIRA_LIMIT && 
    //  (load.kumi_flag == KUMI_JUURYO/* || load.kumi_flag <= KUMI_LIMIT*/) &&
    //  span_preset[0][0] != 0.0
    //  ){
    ////if(0){
    //    cel.set_span_preset();
    //    cel.set_fainV(span_preset[i][0]);      //現在のロードセルのFAIN値を取得
    //    cel.set_sainV(span_preset[i][1]);      //現在のロードセルのSAIN値を取得
    //    cel.set_sainVs(0.0           ,2);      //現在のロードセルのSAIN値を取得
    //    cel.set_sainVs(0.0           ,3);      //現在のロードセルのSAIN値を取得
    //    cel.set_sainVs(0.0           ,4);      //現在のロードセルのSAIN値を取得
    //    cel.set_sainVs(0.0           ,5);      //現在のロードセルのSAIN値を取得
    //    cel.set_sainVs(0.0           ,6);      //現在のロードセルのSAIN値を取得
    //    cel.set_sainVs(0.0           ,7);      //現在のロードセルのSAIN値を取得
    //    cel.set_sainVs(0.0           ,8);      //現在のロードセルのSAIN値を取得
    //    cel.set_sainVs(0.0           ,9);      //現在のロードセルのSAIN値を取得
    //    cel.set_sainVs(0.0           ,10);      //現在のロードセルのSAIN値を取得
    //}else{
        file.read_span();                   //各スパン値
        cel.set_span(file.get_span());      //スパン値を取得
        file.read_span_hundo();
    //}

    lcd.cls();
    lcd.printf("servo_test?");
    lcd.locate(0,1);
    servo_test = off_on_no_yes_result(2,servo_test);
    if(servo_test == 1){
        send_i(TEST_SET,0,0);
    }else{
        send_i(TEST_END,0,0);
    }
    lcd.cls();
    lcd.printf("AUTO_ZERO_ON?");
    if(auto_zero_off == false){
        autozeroONOFF = 1;
    }else{
        autozeroONOFF = 0;
    }
    lcd.locate(0,1);
    if(off_on_no_yes_result(0,1)){
        autozeroONOFF = 1;
        auto_zero_off = false;
    }else{
        autozeroONOFF = 0;
        auto_zero_off = true;
    }
    lcd.cls();
    lcd.printf("syousuutenn ON?");
    lcd.locate(0,1);
    if(off_on_no_yes_result(0,syousuuten)){
        syousuuten = 1;
    }else{
        syousuuten = 0;
    }

    if(now_TC_mode == 0){
        auto_zero_point();
    }

    if(auto_zero_off == false) {
    } else {
        i = 0;
        if(head>0){
            i = head;
        }
    } 
    while((stop_cells&(1<<i))>0) {
        btn.force_DOWN();
        btn.next_or_back(&i,0, product.head-1);
    }
    btn.end_force();
    lcd.messege( TOP, DIS_START_WHT, CLS);
    //Escが押されるまで繰り返す
    while(btn.get_btn() != BTN_ESC) {

        btn.new_btn();               //スイッチ(New)取得
        if(servo_test == 1){
            if(btn.get_btn() == BTN_UP) {
                if(servo_onoff == 0){
                    send_i(SERVO_1 ,i,servo_span[i][1]); wait_res();
                    servo_onoff = 1;
                }else{
                    send_i(SERVO_1 ,i,servo_span[i][0]); wait_res();
                    servo_onoff = 0;
                }
                while(btn.any_press()==true){
                    wait(0.2);
                }
            }else{
                btn.next_or_back(&i,0, product.head-1);
            }
        }else{
            btn.next_or_back(&i,0, product.head-1);
        }
        while((stop_cells&(1<<i))>0) {
            btn.next_or_back(&i,0, product.head-1);
        }
        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            
            while(btn.any_press()==true){
                wait(0.2);
            }
            btn.new_btn();               //スイッチ(New)取得
            lcd.cls();
            lcd.printf("servo_test?");
            lcd.locate(0,1);
            servo_test = off_on_no_yes_result(2,servo_test);
            if(servo_test == 1){
                send_i(TEST_SET,0,0);
            }else{
                send_i(TEST_END,0,0);
            }
            lcd.cls();
            lcd.printf("AUTO_ZERO_OFF?");
            //if(auto_zero_off == false){
            //    autozeroONOFF = 1;
            //}else{
            //    autozeroONOFF = 0;
            //}
            lcd.locate(0,1);
            if(off_on_no_yes_result(0,autozeroONOFF)){
                autozeroONOFF = 1;
                auto_zero_off = false;
            }else{
                autozeroONOFF = 0;
                auto_zero_off = true;
            }
            lcd.locate(0,1);
            lcd.cls();
            lcd.printf("syousuutenn ON?");
            lcd.locate(0,1);
            if(off_on_no_yes_result(0,syousuuten)){
                syousuuten = 1;
            }else{
                syousuuten = 0;
            }

            if(gLevel == 1 || gLevel == 0) {
                se.Touch_01();   //ボタン操作時の効果音
                if(gLevel == 1){
                    while(btn.ent_press()){wait(0.2);}
                    for(int k =0;k<P_END;k++){
                        //if(k<0){k =P_END;}//0以下は終了
                        switch(k){
                            case P_zerocount:
                            case P_ZEROTOLERANCE:
                            case P_ZEROAREA:
                                set_sercret_parameter_one(k,0);
                                break;
                        }
                        if(btn.get_btn() == BTN_ESC){
                            k -= 2;
                        }
                        if(k<0){k =P_END;}//0以下は終了
                    }
                    set_sercret_parameter_one(P_END,1); //保存指令付
                    cel.set_param(file.get_param());    //パラメータをロードセルクラスへ
                    while(btn.ent_press()){wait(0.2);}
                    lcd.blank(0);
                    lcd.printf("ADD_delay");
                    keiryo_demo = off_on_no_yes_result(0,keiryo_demo);
                    lcd.blank(0);
                    lcd.blank(1);
                }
                
            }
            auto_zero_point();
            lcd.messege( TOP, DIS_START_WHT, CLS);
            btn.new_btn();               //スイッチ(New)取得
        }
        if(keiryo_demo==1){
            for(int m = 0; m < product.head; m++) {
                setHeadVolt(m, param.KURI);
            }
        }else{
            setHeadVolt(i, KURI2nd); //調整を行うロードセルを指定
        }
        if( product.productType >= STANDARD_D) {
            //cel.set_head(i);
            pc.printf("%2d %08.2f\r\n", i+1, cel.getWeight(i));
        }
        //////////////////////////////////////////////////////////
            lcd.locate(0,0);
            if(cel.anteiWeight(i) == 0){
                //lcd.printf("%2d:%6.1fg",i+1,cel.anteiWeight(i));
                lcd.printf("%3.1f:%6.1fg",op_n_hosei + hosei_f[i],cel.anteiWeight(i));
            }else{
                //lcd.printf("%2d:%6.1fg:%3.1fg",i+1,cel.anteiWeight(i) + op_n_hosei,op_n_hosei);
                lcd.printf("%3.1f:%6.1fg",op_n_hosei + hosei_f[i],cel.anteiWeight(i) + op_n_hosei + hosei_f[i]);
            }
        /////////////////////////////////////////////////////////
        lcd.locate(12,0);
        lcd.printf("%3d",use_span);
        lcd.blank(1);
        if(syousuuten == 1) {
            //17/08/11 裏モードは小数点表示
            lcd.printf("%2d:%6.1fg",i+1,cel.getWeight(i));
            lcd.locate(10,1);
            lcd.printf("%4.1fg",cel.get_HWeight(i));
            
            //lcd.locate(10,1); 
            //lcd.printf("%6.1fg",cel.getWeight2(i));
        } else {
            //17/08/11 通常モードは小数点無し
            if(product.productType==BIG) {
                lcd.printf("%2d:%4.0fg %4.0fg",i+1,float(int(cel.getWeight(i)/5)*5),cel.getWeight(i));
                //pc.printf("%2d:%4.0fg %6.1fg\r\n",i+1,float(int(cel.getWeight(i)/5)*5),cel.getWeight(i));

            } else
                if(product.Rating==RATING_BIG){
                    lcd.printf("%2d:%4.0fg",i+1,float(int(cel.getWeight(i)/2)*2));
                }else{
                    lcd.printf("%2d:%4.0fg",i+1,cel.getWeight(i));
                }
        }
        if(cel.getWeight(i) == cel.anteiWeight(i)){
            lcd.locate(10,1);
            lcd.printf(antei);
        }
        
        
        if(product.matrix_mode == MATRIX_OFF){
            led.REGIS(oneBit[i]);
        }else{
            for(int k = 0; k < product.head; k++) {
                if(i==k){
                    iro[k] = PENTA_R;
                }else{
                    iro[k] = C_NUSE;
                }
            }
        }
        btn.old_btn();              //スイッチ更新

        //LED消灯
        wait(0.03);
        if(product.matrix_mode == MATRIX_OFF){
            led.REGIS(0);
        }
        wait(0.03);
    }//while
    auto_zero_off = false;
    if(servo_connection==true){
        send_i(TEST_END,0,0); servo_off_wait();
    }
}

/*****************************************************************************
    1ヘッドずつの重量を量るサーボ動作付き
*****************************************************************************/
void start_keiryo2(int head)
{
    data_t limitData;
    const char antei[]= {0xA1,0x00};          //(〇表示)
    int i = 0;
    int keiryo_demo = 0; //疑似的に組合せ計量と同じような待機時間を作る
    int no_weighter=0;
    int wait_count01 = 20;
    int test_head =0;
    int open_wait = waiter_t;
    int b_waiter_t = waiter_t;
    //int b_landing_w = landing_w;
    btn.ini();
    led.ledAllOff();       // LED消灯
    /*
    limitData.lcd = 1;
    limitData.min = 0;
    limitData.max = 100;
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Open Wait time");
    lcd.locate(1,1);
    Chenge_vaule(&open_wait, limitData);
    waiter_t = open_wait;
    //send_para(PARA_SET3,  open_wait,      0, waiter_t, 0);
    servo_set();
*/
    lcd.cls();
    send_i(EJECT_RESET, 11+1, 0);
    lcd.cls();
    if(auto_zero_off == false) {
        auto_zero_point();
    } else {
        i = 0;
        if(head>0){
            i = head;
        }
    }
    //send_i(KEIRYO_ST,0,0); wait(0.2);
    
    while((stop_cells&(1<<i))>0) {
        btn.force_DOWN();
        btn.next_or_back(&i,0, product.head-1);
    }
    btn.end_force();
    lcd.messege( TOP, DIS_START_WHT, CLS);
    //Escが押されるまで繰り返す
    while(btn.get_btn() != BTN_ESC) {

        btn.new_btn();               //スイッチ(New)取得
        btn.next_or_back(&i,0, product.head-1);
        while((stop_cells&(1<<i))>0) {
            btn.next_or_back(&i,0, product.head-1);
        }
        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            if(gLevel == 1 || gLevel == 0) {
                se.Touch_01();   //ボタン操作時の効果音
                if(gLevel == 1){
                    while(btn.ent_press()){wait(0.2);}
                    for(int k =0;k<P_END;k++){
                        //if(k<0){k =P_END;}//0以下は終了
                        switch(k){
                            case P_zerocount:
                            case P_ZEROTOLERANCE:
                            case P_ZEROAREA:
                                set_sercret_parameter_one(k,0);
                                break;
                        }
                        if(btn.get_btn() == BTN_ESC){
                            k -= 2;
                        }
                        if(k<0){k =P_END;}//0以下は終了
                    }
                    set_sercret_parameter_one(P_END,1); //保存指令付
                    cel.set_param(file.get_param());    //パラメータをロードセルクラスへ
                    while(btn.ent_press()){wait(0.2);}
                    lcd.blank(0);
                    lcd.printf("ADD_delay");
                    keiryo_demo = off_on_no_yes_result(0,keiryo_demo);
                    lcd.blank(0);
                    lcd.blank(1);
                }
                
            }
            auto_zero_point();
            lcd.messege( TOP, DIS_START_WHT, CLS);
        }
        no_weighter=0;
        //if(keiryo_demo==1){
            //wait(0.3+((0.3*product.head)/12));
    
            //if(antei_flug[i] == 0){
                //setHeadVolt(i, param.KURI);
            //}else{
            //    setHeadVolt(i, param.KURI_Y);
            //}
            //if(wait_count01 <= 1 || keiryo_demo==1){
                for(int m = 0; m < product.head; m++) {
                    if(m == i){
                        //if(antei_flug[m] == 0){
                            setHeadVolt(m, param.KURI);
                        //}else{
                        //    setHeadVolt(m, param.KURI_Y);
                        //}
                        //setHeadVolt(m, param.KURI);
                    }
                    if(cel.getWeight(m)<param.CUT){
                        no_weighter = no_weighter | (1<<m);
                    }
                    //if(gLevel == 1){    
                    //    printf("%2d|min:%8.6f |max:%8.6f\r\n",m,AD_now_min[m],AD_now_max[m]);
                    //    printf("  |min:%8.6f |max:%8.6f\r\n"   ,AD_bas_min[m],AD_bas_max[m]);
                    //}
                }
            //}
        //}else{
        //    setHeadVolt(i, param.KURI); //調整を行うロードセルを指定
        //}
        //if( product.productType >= STANDARD_D) {
            //cel.set_head(i);
            //pc.printf("%2d %08.2f\r\n", i+1, cel.getWeight(i));
            //pc.printf("%02d: get%6.3f %6.3f %6.3f   %0.8f\r\n", i+1,cel.getWeight(i),cel.anteiWeight(i), cel.get_HWeight(i),cel.getAin());
            pc.printf("%02d: w%9.2f h%9.2f A%0.8f\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.getAin());
        //}
        //////////////////////////////////////////////////////////
        lcd.locate(0,0);
        if(cel.anteiWeight(i) == 0){
            //lcd.printf("%2d:%6.1fg",i+1,cel.anteiWeight(i));
            lcd.printf("%3.1f:%6.1fg",op_n_hosei + hosei_f[i],cel.anteiWeight(i));
        }else{
            //lcd.printf("%2d:%6.1fg:%3.1fg",i+1,cel.anteiWeight(i) + op_n_hosei,op_n_hosei);
            lcd.printf("%3.1f:%6.1fg",op_n_hosei + hosei_f[i],cel.anteiWeight(i) + op_n_hosei + hosei_f[i]);
        }
        /////////////////////////////////////////////////////////
        lcd.locate(12,0);
        lcd.printf("%3d",use_span);
        lcd.blank(1);
        if(gLevel == 1) {
            //17/08/11 裏モードは小数点表示
            lcd.printf("%2d:%6.1fg",i+1,cel.getWeight(i));
            lcd.locate(10,1);
            lcd.printf("%4.1fg",cel.get_HWeight(i));
            
            //lcd.locate(10,1); 
            //lcd.printf("%6.1fg",cel.getWeight2(i));
        } else {
            //17/08/11 通常モードは小数点無し
            if(product.productType==BIG) {
                lcd.printf("%2d:%4.0fg %4.0fg",i+1,float(int(cel.getWeight(i)/5)*5),cel.getWeight(i));
                //pc.printf("%2d:%4.0fg %6.1fg\r\n",i+1,float(int(cel.getWeight(i)/5)*5),cel.getWeight(i));

            } else
                if(product.Rating==RATING_BIG){
                    lcd.printf("%2d:%4.0fg",i+1,float(int(cel.getWeight(i)/2)*2));
                }else{
                    lcd.printf("%2d:%4.0fg",i+1,cel.getWeight(i));
                }
        }
        if(cel.getWeight(i) == cel.anteiWeight(i)){
            lcd.locate(10,1);
            lcd.printf(antei);
        }
        
        
        led.REGIS(oneBit[i]);       //調整する場所のLED点灯
        btn.old_btn();              //スイッチ更新
        
        wait_count01-=1;
        
        
        if(wait_count01 <= 0){
            no_weighter = no_weighter ^ (no_weighter & close_wait_flg);
            
            close_wait_flg = close_wait_flg | (1 << test_head);
            
            test_head +=1;
            if(test_head>5){test_head=0;}
            
            send_i(ON_CHECK,0,close_wait_flg);
            wait_signal();
            
            if(moving1 == 0){
                //send_i(SERVO_ON,no_weighter,close_wait_flg);wait_signal();
                send_i(SERVO_ON,1 << i,close_wait_flg);wait_signal();
                send_i(FAKE_GO,0,0);wait_signal();
                //wait(50);
            }
            wait_count01 =50;
            for(int a = 0; a < product.head; a++) {
                if((close_wait_flg & (1 << a)) > 0 && (moving2 & (1 << a)) >0){
                    lcd.cls();
                    lcd.locate(0,0);
                    lcd.printf("ZERO_SET");
                    se.Touch_01();   //ボタン操作時の効果音
                    set_zero_span_onehead(a);
                    //v.speak_numbers(a+1);
                    close_wait_flg = close_wait_flg ^ (close_wait_flg & (1 << a));
                }
            }
            send_i(ON_CHECK,0,close_wait_flg);
            wait_signal();
            
        }
        
        /*
        for(int a = 0; a < product.head; a++) {
            if((close_wait_flg & (1 << a)) > 0 && (moving2 & (1 << a)) >0){
                set_zero_span_onehead(a);
                close_wait_flg = close_wait_flg ^ (close_wait_flg & (1 << a));
            }
        }*/


        //LED消灯
        wait(0.03);
        led.REGIS(0);
        wait(0.03);
        
        
    }//while
    
    send_i(EJECT_RESET, 0, 0);
    seconds = time(NULL);
    tm *dated4 = localtime(&seconds);
    //printf("en%d:%d:%d\r\n",dated4->tm_hour,dated4->tm_min,dated4->tm_sec);
    close_wait_flg = 0;
    close_wait_flg2 = 0;
    test_head = 0;
    int ji=0;
    int hun=0;
    ji = dated4->tm_hour - run_date.tm_hour;
    if(ji<0) {
        ji += 24;
    }
    hun = dated4->tm_min - run_date.tm_min;
    if(hun<0) {
        hun += 60;
        ji -= 1;
    }
    
    //b_waiter_t = waiter_t;
    //servo_set();
    lcd.blank(1);
    lcd.blank(0);
    lcd.locate(0,1);
    lcd.printf("                ");
    lcd.locate(0,1);
    for(int a=0 ; a < product.head ; a++){                                
        if( servo_n_histry[1][a] == 1){
            servo_n_histry[0][a] += 1;
            servo_n_histry[1][a] =  0;
        }
    }
    servo_histry += total_kumi_cnt;
    for(int l=0; l<product.head; l++) {
        servo_b_histry[l] = servo_b_histry[l] + servo_n_histry[0][l] + servo_n_histry[1][l];
        servo_n_histry[0][l] = 0;
        servo_n_histry[1][l] = 0;
    }
    servo_h_time[1] += hun;
    
    if(servo_h_time[1]>=60){
        servo_h_time[1] -= 60;
        servo_h_time[0] +=  1;
    }
    servo_h_time[0] += ji;
    lcd.blank(1);
    lcd.blank(0);
    lcd.locate(0,1);
    lcd.printf("                ");
    file.save_hservo();
}
bool set_stop_cells()
{
    int now_cell=0;
    int i = 0;
    char ck1[]= {0xCD,0xDD,0xBA,0xB3,0xBD,0xD9,' ',0xBB,0xD7,0xA6,' ',0xBE,0xDD,0xC0,0xB8,0x00};  //ﾍﾝｺｳｽﾙ ｻﾗｦ ｾﾝﾀｸ
    char ck2[]= {'N','o','.'};  //No.
    char ck3[]= {'N','o','.',' ',' ',0x3A,0xBC,0xD6,0xB3,0x00};      //No.__:ｼﾖｳ
    //char ck2[]={0xBB,0xD7,0xD2,0x00};       //ｻﾗﾒ
    //char ck3[]={0xBB,0xD7,0xD2,0x3A,0xBC,0xD6,0xB3,0x00};       //ｻﾗﾒ:ｼﾖｳ
    char ckOK[]= {0xBD,0xD9,' ',0x00};                         //ｽﾙ
    char ckNG[]= {0xBC,0xC5,0xB2,0x00};                        //ｼﾅｲ
    lcd.blank(0);
    lcd.locate(0,0);
    lcd.printf(ck1);
    btn.ini();
    file.read_stop_cells();
    set_stopcell();                     //不使用セル(＋12Headの対面点灯を組み合わせる)
    //ファイルデータから表示選択
    lcd.blank(1);
    lcd.printf(ck2);
    lcd.locate(3,1);
    lcd.printf("%2d",i + 1);
    now_cell=stop_cells2 & (1<<i);
    if(now_cell > 0) {
        //lcd.printf(ckNG);
        now_cell =1;
    } else {
        //lcd.printf(ckOK);
        now_cell =0;
    }
    btn.ini();
    while(btn.get_btn() != BTN_ENT) {
        btn.new_btn();               //スイッチ(New)取得
        if(btn.get_btn() == BTN_UP) {
            i -= 1;
            if(i < 0) {
                i = product.head-1;
            }
            lcd.locate(3,1);
            lcd.printf("%2d",i + 1);
            se.Touch_01();
        } else if(btn.get_btn() == BTN_DOWN) {
            i += 1;
            if(i > product.head-1) {
                i = 0;
            }
            lcd.locate(3,1);
            lcd.printf("%2d",i + 1);
            se.Touch_01();
        } else if(btn.get_btn() == BTN_ESC) {
            se.Touch_01();
            file.read_stop_cells();
            set_stopcell();                     //不使用セル(＋12Headの対面点灯を組み合わせる)
            return false;
        }
        led.REGIS(~(stop_cells2 | (1<<i)));       //調整する場所のLED消灯_使用するセルは点灯
        btn.old_btn();              //スイッチ更新
        wait(0.03);
        led.REGIS(~stop_cells2 | (1<<i));        //調整する場所のLED点灯
        //wait(0.03);
        //led.REGIS(0);       //調整する場所のLED点灯
        wait(0.05);
    }//while
    se.Touch_01();
    lcd.blank(1);
    lcd.printf(ck3);
    lcd.locate(3,1);
    lcd.printf("%2d",i + 1);
    now_cell=stop_cells2 & (1<<i);
    lcd.locate(9,1);
    if(now_cell > 0) {
        lcd.printf(ckNG);
        now_cell =1;
    } else {
        lcd.printf(ckOK);
        now_cell =0;
    }
    btn.ini();
    while(btn.get_btn() != BTN_ENT) {
        btn.new_btn();               //スイッチ(New)取得
        if(btn.get_btn() == BTN_UP || btn.get_btn() == BTN_DOWN) {
            lcd.locate(9,1);
            if(now_cell==0) {
                lcd.printf(ckNG);       //(ボタン押されてるので次の値)
                now_cell =1;
            } else {
                lcd.printf(ckOK);
                now_cell=0;
            }
            se.Touch_01();
        } else if(btn.get_btn() == BTN_ESC) {
            se.Touch_01();
            file.read_stop_cells();
            set_stopcell();                     //不使用セル(＋12Headの対面点灯を組み合わせる)
            return false;
        }
        led.REGIS(~(stop_cells2 | (1<<i)));       //調整する場所のLED消灯_使用するセルは点灯
        btn.old_btn();              //スイッチ更新
        wait(0.03);
        led.REGIS(~stop_cells2 | (1<<i));        //調整する場所のLED点灯
        //wait(0.03);
        //led.REGIS(0);       //調整する場所のLED点灯
        wait(0.05);
    }//while
    se.Touch_01();
    stop_cells2 = (stop_cells2 & (~(1<<i))) | (now_cell<<i);

    file.save_stop_cells(stop_cells2);
    return true;
}

/*****************************************************************************
    namae設定を行う
*****************************************************************************/
bool set_name(int ph)
{
    const int set = 10;    //設定を行う数
    data_t limitData= {-16, 26, 1};
    const char namec[]= {0xC5,0xCF,0xB4,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00};   //ﾅﾏｴ ﾍﾝｺｳ
    const char named[]= {0xC5,0xCF,0xB4,' ' ,0xC6,0xAD,0xB3,0xD8,0xAE,0xB8,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00};   //ﾅﾏｴ ﾆｭｳﾘｮｸ
    const char saving[]= {0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,0x00}; //ﾎｿﾞﾝﾁｭｳ
    char ckOK[]= {0xBD,0xD9,' ',0x00};                         //ｽﾙ
    char ckNG[]= {0xBC,0xC5,0xB2,0x00};                        //ｼﾅｲ
    int change_EN=0;    //名前変更するかの 1:ｽﾙ / 0:ｼﾅｲ
    //準最適値を行うヘッド数は、HEAD数より
    //大きい値はありえない(エラー対策)
    if(bscData.startKinji > product.head) {
        bscData.startKinji = product.head;
    }

    lcd.locate(0,0);
    lcd.printf("                ");
    lcd.locate(0,1);
    lcd.printf("                ");
    lcd.locate(0,0);
    lcd.printf(namec);
    //*****名前変更するか確認
    lcd.locate(0,1);
    if(change_EN > 0) {
        lcd.printf(ckOK);
        change_EN =1;
    } else {
        lcd.printf(ckNG);
        change_EN =0;
    }
    btn.ini();
    while(btn.get_btn() != BTN_ENT) {
        btn.new_btn();               //スイッチ(New)取得
        if(btn.get_btn() == BTN_UP || btn.get_btn() == BTN_DOWN) {
            lcd.locate(0,1);
            if(change_EN==0) {
                lcd.printf(ckOK);       //(ボタン押されてるので次の値)
                change_EN =1;
            } else {
                lcd.printf(ckNG);
                change_EN=0;
            }
            se.Touch_01();
        } else if(btn.get_btn() == BTN_ESC) {
            se.Touch_01();
            return false;
        }
        btn.old_btn();              //スイッチ更新
        wait(0.03);
    }//while
    //********************


    if(change_EN!=1) {      //名前変更しないで終了
        return true;
    } else {                 //名前変更(して終了)
        for(int i= 0; i < set; i++) {
            //規定回数以上Escボタン押下されたのでメニューに戻る
            if(i < 0) {
                return false;
            }
            lcd.locate(0,0);
            lcd.printf("                ");
            lcd.locate(0,0);
            lcd.printf(named);
            wait(0.4);
            for(int k=0; k<10; k++) {
                if(name[ph][k]==0) {
                    name[ph][k]=-40;
                }
            }
            lcd.locate(0,1);
            printname(ph);

            //設定データが整数か浮動小数かで処理を分ける
            Chenge_name_vaule(&name[ph][i], limitData ,ph,i);
            if(name[ph][i]==0) {
                for(; i < set; i++) {
                    name[ph][i]=-40;
                }
            }
            if(btn.esc()) {
                i -= 2;
            }
        }//for
        lcd.blank(1);
        lcd.locate(0,1);
        lcd.printf("                ");
        lcd.locate(0,1);
        lcd.printf(saving);
        pc.printf("end");
        file.save_name();
        file.read_name();
    }
    return true;
}
//********************************************************
//       オプション設定部
//********************************************************

bool set_option_setting(int modeop)
{
    int bk_speed = option.rank_speed;//ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂ一時保存()
    int bk_c_rotate = option.c_rotate;//ﾓｼﾞ回転一時保存(赤ボタンで)
    option = file.get_option();
    const int set = OP_END;
    //const int change = OP_KIKAKUGAI_V-2;
    data_t limitData;
    option.rank_speed = bk_speed;
    option.c_rotate = bk_c_rotate;
    //各データを設定
    //int data[]= {option.reck_zen, option.reck_ato, option.usb_EN, option.c_rotate, option.rank_over, option.katamen};
    int data[]= {   0                   , 
                    option.reck_zen     , 
                    option.reck_ato     , 
                    0                   ,
                    option.usb_EN       , 
                    option.c_rotate     , 
                    option.rank_over    , 
                    option.kokuti       ,
                    option.kikakugai_v  , 
                    option.rank_speed   , 
                    option.btn_sett     , 
                    option.beginner     ,
                    option.selbeep      , 
                    0                   , 
                    0                   , 
                    option.no_combi_v   , 
                    option.second_target, 
                    option.number_color, 
                    option.O_count_lim , 
                    option.O_lit_count_lim,
                    option.nagamono_hosei,
                    0                   , 
                    0                   , 
                    0                   , 
                    0                   , 
                    0                   , 
                    0                   , 
                    option.waiting_limit, 
                    option.selautozero, 
                    option.kekka_hyouji, 
                    option.alart_autozero, 
                    option.fix_value
                };

    for(int i= 0; i < set; i++) {
        //規定回数以上Escボタン押下されたのでメニューに戻る
        if(i==modeop) {

            //*****設定上限値指定*****
            if(modeop==OP_ZENNOSE) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = 0;//ON/OFFChenge_vaule表示
            } else if(modeop==OP_ATONOSE) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = 0;//ON/OFFChenge_vaule表示
            } else if(modeop==OP_USB_EN) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = 0;//ON/OFFChenge_vaule表示
            } else if(modeop==OP_CHAR_ROTATE) {
                limitData.min = 0;
                //if(product.head >= 8) {
                if(product.Array == 0) {
                    //limitData.max = 2;
                    limitData.max = 1;
                } else {
                    limitData.max = 1;
                }
                limitData.lcd = OP_CHAR_ROTATE;//ON/OFFChenge_vaule表示
            } else if(modeop==OP_RANK_OVER) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = 0;//ON/OFFChenge_vaule表示
            } else if(modeop==OP_KOKUTI) {
                limitData.min = 0;
                limitData.max = 3;
                limitData.lcd = OP_KOKUTI;//ON/OFFChenge_vaule表示
            } else if(modeop==OP_KIKAKUGAI_V) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = 0;//ON/OFFChenge_vaule表示
            } else if(modeop==OP_RANKSPEED) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = 0;//ON/OFFChenge_vaule表示
            } else if(modeop==OP_BTN_SETTING) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = OP_BTN_SETTING;//ON/OFFChenge_vaule表示
            } else if(modeop==OP_BEGINNER) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = OP_BEGINNER;    //ON/OFFChenge_vaule表示
            } else if(modeop==OP_SEL_BEEP) {
                limitData.min = 1;
                limitData.max = 8;
                limitData.lcd = OP_SEL_BEEP;    //ON/OFFChenge_vaule表示
            } else if(modeop==OP_NO_COMBI_V) {
                limitData.min = 0;
                limitData.max = 2;
                limitData.lcd = OP_NO_COMBI_V;    //ON/OFFChenge_vaule表示
            } else if(modeop==OP_SECONDMBI_V) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = OP_SECONDMBI_V;    //ON/OFFChenge_vaule表示
            } else if(modeop==OP_NUMBER_COLOR) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = OP_NUMBER_COLOR;    //ON/OFFChenge_vaule表示
            } else if(modeop==OP_O_count_lim) {
                limitData.min = 1;
                limitData.max = 5;
                limitData.lcd = OP_O_count_lim;    //ON/OFFChenge_vaule表示
            } else if(modeop==OP_O_lit_count_lim) {
                limitData.min = 1;
                limitData.max = 10;
                limitData.lcd = OP_O_lit_count_lim;    //ON/OFFChenge_vaule表示
            } else if(modeop==OP_NAGAMONO_HOSEI) {
                limitData.min = -50;
                limitData.max = 1000;
                limitData.lcd = OP_NAGAMONO_HOSEI;    //ON/OFFChenge_vaule表示
            } else if(modeop==OP_WAITING_LIMMIT) {
                limitData.min = 0;
                limitData.max = 480;
                limitData.lcd = OP_WAITING_LIMMIT;    //ON/OFFChenge_vaule表示
            } else if(modeop==OP_SELAUTOZERO) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = OP_SELAUTOZERO;       //ON/OFFChenge_vaule表示
            } else if(modeop==OP_KEKKA_HYOUJI) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = OP_KEKKA_HYOUJI;       //ON/OFFChenge_vaule表示
            } else if(modeop==OP_ALART_AUTOZERO) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = OP_ALART_AUTOZERO;       //ON/OFFChenge_vaule表示
            } else if(modeop==OP_FIX_VALUE) {
                limitData.min = 0;
                limitData.max = 1;
                limitData.lcd = OP_FIX_VALUE;       //ON/OFFChenge_vaule表示
            }
            //**********************
            lcd.set_option(i);
            Chenge_vaule2(&data[i],limitData);
        }
        if(btn.esc()) {
            option = file.get_option();           //パラメータ(設定情報)を取得
            katamen = load.katamen;
            set_stopcell();
            led.REGIS(0xfff^stop_cells2);
            i = -2;
            return false;
        }
    }
    //OPTIONの保存
    set_stopcell();
    op_n_hosei = float(data[OP_NAGAMONO_HOSEI])/float(10);
    file.set_option(data);    //変更データ受け渡し
    file.save_option();
    option = file.get_option();           //パラメータ(設定情報)を取得
    cel.set_option(file.get_option());    //別ファイルへデータ受け渡し
    return true;
}
/*****************************************************************************
    基本設定の各種設定を行う
*****************************************************************************/
bool set_basic_setting()
{
    const int set = 1;    //設定を行う数
    data_t limitData= {1, product.head, 1};

    char subMsg[][SIZE]= {
        {' ',0xBB,0xD7,0x00}          //ｻﾗ
    };

    //準最適値を行うヘッド数は、HEAD数より
    //大きい値はありえない(エラー対策)
    if(bscData.startKinji > product.head) {
        bscData.startKinji = product.head;
    }

    //各データを設定(設定値整数)
    int data[] = { bscData.startKinji};     //準最適値を開始するヘッド数

    for(int i= 0; i < set; i++) {
        //規定回数以上Escボタン押下されたのでメニューに戻る
        if(i < 0) {
            return false;
        }

        lcd.set_basic(i);
        wait(0.4);

        lcd.locate(5,1);    //単位などの表示位置へ移動
        if( i == 0) {
            lcd.printf(subMsg[i]);
        }

        //設定データが整数か浮動小数かで処理を分ける
        Chenge_vaule(&data[i], limitData);

        if(btn.esc()) {
            i -= 2;
        }
    }//for

    bscData.startKinji   = data[0];
    file.save_basic_setting(bscData);
    return true;
}
void multi_hundo_set(bool back_flg){
    const char settei[]={0xCC,0xDD,0xC4,0xDE,0xB3,' ' ,' ' ,0xBE,0xAF,0xC3,0xB2,0x00};      //ﾌﾝﾄﾞｳ ｾｯﾃｲ
    const char hundou[]={0xCC,0xDD,0xC4,0xDE,0xB3,' ' ,' ' ,0xC2,0xB2,0xB6,0x00};      //ﾌﾝﾄﾞｳ ﾂｲｶ
    int answer = 1;
    data_t limitData;
    //int num;
    int a=0;
    printf("back_flg%d",back_flg);
    if(back_flg == true){
        for(int i = 0 ; i < sphun_count ;i++){
            if(span_hundo[i] > 0){
                a = i;
                printf("a%d",a);
            }
        }
    }
    
    for(int i = 0 ; i < sphun_count ;i++){
        if(a > 0){
            i = a;
            a = 0;
        }
        printf("i%d\r\n",i);
        printf("%2d\r\n",i);
        if(span_hundo[i] == 0){
            answer = 0;
        }
        if(i == 0){
            answer = 1;
        }
        if(i>=1 && span_hundo[i-1] > 0){
            lcd.blank(0);
            lcd.printf(hundou);
            lcd.locate(5,0);
            lcd.printf("%d", i+1);
            limitData.max = 1;
            limitData.min = 0;
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            Settei_suru_sinai(&answer,limitData,TL_RANK_ON_OFF,TL_RANK_ON_OFF);
            //printf("answer%2d\r\n",answer);
        }
        if(answer == 1){
            //printf("answer == %2d\r\n",answer);
            lcd.blank(0);
            lcd.printf(settei);
            lcd.locate(5,0);
            lcd.printf("%d", i+1);
            
            
            limitData = file.get_limit_sdata(P_hundo);
            
            while(btn.any_press() == true){wait(0.2);}
            if(span_hundo[i] == 0 || (i >0 && span_hundo[i] <= span_hundo[i-1])){
                span_hundo[i] = span_hundo[i-1]+10;
            }
            lcd.locate(0,1);
            lcd.printf("%4d", span_hundo[i]);
            
            lcd.locate(5,0);
            lcd.printf("%d", i+1);
            
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            
            if(i>0){
                limitData.min = span_hundo[i-1]+10;
            }
            while(btn.get_btn() != BTN_ENT && btn.get_btn() != BTN_ESC) {
                if(btn.get_btn()) {
                    lcd.blank(1);
                    lcd.printf("%4d", span_hundo[i]);
                    wait(0.1);
                }
                btn.new_btn();              //スイッチ(New)取得
                //ボタン操作時の効果音
                if(btn.push_sound()) {
                    se.Touch_01();
                    wait(0.1);
                }
                    //printf("MIN:%5d:MAX:%5d : %2d\r\n",limitData.min,limitData.max,btn.get_btn());
                btn.puls_or_minus(&span_hundo[i]);
        
                if(span_hundo[i] > limitData.max) {
                    span_hundo[i]  = limitData.min;
                } else if(span_hundo[i] < limitData.min)       {
                    span_hundo[i]  = limitData.max;
                }
                
                if(btn.esc()){
                    //シート数の超過
                    if(i>0){
                        //i=0;
                        i -= 2;
                        //printf("answer%2d %2d btn.esc1\r\n",answer,i);
                        while(btn.any_press() == true){wait(0.2);}
                        continue ;  
                    }else{
                        //printf("answer%2d %2d end0\r\n",answer,i);
                        return;       //処理終了。値の変更なし
                    }
                }
        
                btn.old_btn();  //スイッチ更新
            }
            
            
        
        }else if(answer == 0){
        printf("answer == %2d\r\n",answer);
            span_hundo[i] = 0;
        }else if(answer == 99){
                
        printf("answer == %2d\r\n",answer);
                printf("answer%2d %2d \r\n",answer,i);
            if(i>=1){
                //i=0;
                i -= 2;
                printf("answer%2d %2d answer == 99\r\n",answer,i);
                
                while(btn.any_press() == true){wait(0.2);}
                btn.old_btn();       //スイッチ更新
                btn.new_btn();       //スイッチ更新
            }else{
                printf("answer%2d %2d end0\r\n",answer,i);
                return;       //処理終了。値の変更なし
            }
        }
    }
    file.save_span_hundo();
    
    
}      
/*****************************************************************************
    シークレットモードの各種設定を行う
*****************************************************************************/
void set_sercret_product()
{

    file.read_all_file();               //FileRead
    product = file.get_product();           //パラメータ(設定情報)を取得

    const int set = file.get_count_param();
    const int fNum = file.get_start_fparam();  //浮動小数の設定開始位置
    data_t limitData;
    product_t product0;
    
    //各データを設定(設定値整数)
    int data[7]= {
        /*product.productType,
        product.limit_on,
        product.Array,
        product.Rating,
        product.matrix_mode,
        product.use_usb,
        product.head,*/
        
    };
    data[0] = product.productType;
    data[1] = product.limit_on;
    data[2] = product.Array;
    data[3] = product.Rating;
    data[4] = product.matrix_mode;
    data[5] = product.use_usb;
    data[6] = product.head;

    //各データを設定(設定値浮動少数)
 
    for(int i= 0; i < PR_END; i++) {
        //規定回数以上Escボタン押下されたのでメニューに戻る
        if(i < 0) {
            return;
        }
        limitData.lcd =  1;
        limitData.min =  0;
        limitData.max = 12;
        if(i==PR_productType){
            limitData.lcd = i + 100;
            limitData.min = 1;
            limitData.max = (PRODUCT_END-1) * 2;
            if(data[PR_productType] > PRODUCT_END){
                data[PR_productType] = data[PR_productType] - (STANDARD_D - PRODUCT_END);
            }
        } else if(i==PR_limit_on){
            limitData.lcd = i + 100;
            limitData.min = 0;
            limitData.max = LIMIT_END-1;
        } else if(i==PR_Array){
            limitData.min = 0;
            limitData.max = 3;
        } else if(i==PR_Rating){
            limitData.min = 0;
            limitData.max = 3;
        } else if(i==PR_matrix_mode){
            limitData.min = 0;
            limitData.max = 1;
        } else if(i==PR_Use_USB){
            limitData.min = 0;
            limitData.max = 1;
        }else if(i==RP_head){
            limitData.min = 1;
            limitData.max = 12;
        }
        
        lcd.set_secret_product(i);

        //設定データが整数か浮動小数かで処理を分ける
        Chenge_vaule(&data[i],limitData);

        if(btn.esc()) {
            i -= 2;
        }
    }//for
    if(i==PR_productType){
        if(data[PR_productType] >= PRODUCT_END){
            data[PR_productType] = data[PR_productType] + (STANDARD_D - PRODUCT_END);
        }
    }
    product0.productType     =data[0];
    product0.limit_on        =data[1];
    product0.Array           =data[2];
    product0.Rating          =data[3];
    product0.matrix_mode     =data[4];
    product0.use_usb         =data[5];
    product0.head            =data[6];
    
    
    file.set_product(product0);    //変更データ受け渡し
    file.save_product();
    product = file.get_product();           //パラメータ(設定情報)を取得

    //製品種別が変更されている可能性があるので再読込
    file.read_data_by_product(product.productType);   //製品別のデータ読み込みを行う
    return;
}

void set_sercret_parameter()
{

    file.read_all_file();               //FileRead
    param = file.get_param();           //パラメータ(設定情報)を取得

    const int set = file.get_count_param();
    const int fNum = file.get_start_fparam();  //浮動小数の設定開始位置
    data_t limitData;
    int back_flg;

    //各データを設定(設定値整数)
    int data[]= {
        /*    param.productType,param.head,param.hundo,param.KURI,param.KURI_Y,param.CUT
            ,param.CUT_Y,param.sabun,param.liteLedOff,param.tairyu,param.vivration
            ,param.zeroErr,param.zeroRange
        };*/
        /*param.productType,
        param.limit_on,
        param.Array,
        param.Rating,
        param.matrix_mode,
        param.use_usb,
        param.head,*/
        
        param.hundo,
        param.KURI,
        param.KURI_Y,
        param.KURI_J,
        //ram.AUTOCUT,
        param.CUT,
        param.CUT_Y,
        param.sabun,
        param.BARA_KAKUNIN,
        param.liteLedOff,
        param.tairyu,
        param.vivration,
        param.zeroErr,
        param.zeroRange,
        param.zerocount
    };

    //各データを設定(設定値浮動少数)
    float fdata[] = {
        param.ZEROTOLERANCE,
        param.ZEROAREA,
        param.ZEROAREA2,
        param.BARATSUKI,
        param.buffer,
        param.MINI_target,
        param.BARATSUKI_R,
        param.BARATSUKI_GRAM,
        param.buffer2,
        param.bufferT,
        param.buffer_kaisu,
        param.BARATSUKI_SA
        };

    for(int i= 0; i < set; i++) {
        //規定回数以上Escボタン押下されたのでメニューに戻る
        if(i < 0) {
            return;
        }
        
            
        if(i==P_BARATSUKI_R || i==P_BARATSUKI_GRAM) {
            limitData = file.get_limit_sdata(P_BARATSUKI);
        } else if(i==P_buffer2) {
            limitData = file.get_limit_sdata(P_buffer);
        } else if(i==P_bufferT) {
            limitData = file.get_limit_sdata(P_buffer);   
            limitData.min = -5;
            limitData.max =  5;
        } else if(i==P_BARA_KAKUNIN) {
            limitData = file.get_limit_sdata(P_buffer);            
            limitData.min = 1;
            limitData.max =  10;
        }else{
            limitData = file.get_limit_sdata(i);
            if(limitData.min < - 10){
                limitData.min =0;
            }
            if(limitData.max > 10000){
                limitData.max =10000;
            }
        }
        if(i==P_MINI_target) {
            limitData.min = 0;
            limitData.max = 500;
            limitData.lcd = 1;
        } else
            pc.printf("%d\r\n",i);
        lcd.set_secret(i);

        if(i== P_hundo){
            multi_hundo_set(back_flg);
        }else{
            //設定データが整数か浮動小数かで処理を分ける
            if(i < fNum) Chenge_vaule(&data[i],limitData);
            else      Chenge_vaule(&fdata[i-fNum], limitData);
        }

        if(btn.esc()) {
            i -= 2;
            back_flg = true;
            
    printf("back_flg0%d",back_flg);
        }else{
            back_flg = false;
        }
    }//for
    file.set_param(data, fdata);    //変更データ受け渡し
    file.save_param();
    param = file.get_param();           //パラメータ(設定情報)を取得
    cel.set_param(file.get_param());    //パラメータをロードセルクラスへ
    
    //製品種別が変更されている可能性があるので再読込
    file.read_data_by_product(product.productType);   //製品別のデータ読み込みを行う
    return;
}

/*****************************************************************************
    シークレットモードの各種設定を行う(どれか一か所のみ)
    (変更する場所、値は関数からのｓ指定)
*****************************************************************************/
void set_sercret_parameter_one(int set_memo , int endflg){
    
    //file.read_all_file();               //FileRead
    //file.read_param();           //パラメータ(設定情報)を取得
    param = file.get_param();           //パラメータ(設定情報)を取得
    
    const int set = file.get_count_param();
    const int fNum = file.get_start_fparam();  //浮動小数の設定開始位置
    data_t limitData;
    
    
    //各データを設定(設定値整数)
    int data[]= {
        /*    param.productType,param.head,param.hundo,param.KURI,param.KURI_Y,param.CUT
            ,param.CUT_Y,param.sabun,param.liteLedOff,param.tairyu,param.vivration
            ,param.zeroErr,param.zeroRange
        };*/
        /*param.productType,
        param.limit_on,
        param.Array,
        param.Rating,
        param.matrix_mode,
        param.use_usb,
        param.head,*/
        
        param.hundo,
        param.KURI,
        param.KURI_Y,
        param.KURI_J,
        //ram.AUTOCUT,
        param.CUT,
        param.CUT_Y,
        param.sabun,
        param.BARA_KAKUNIN,
        param.liteLedOff,
        param.tairyu,
        param.vivration,
        param.zeroErr,
        param.zeroRange,
        param.zerocount
    };

    //各データを設定(設定値浮動少数)
    float fdata[] = {
        param.ZEROTOLERANCE,
        param.ZEROAREA,
        param.ZEROAREA2,
        param.BARATSUKI,
        param.buffer,
        param.MINI_target,
        param.BARATSUKI_R,
        param.BARATSUKI_GRAM,
        param.buffer2,
        param.bufferT,
        param.buffer_kaisu,
        param.BARATSUKI_SA
        };
    for(int i= 0; i < set; i++){
        //規定回数以上Escボタン押下されたのでメニューに戻る
        if(i < 0){
            return;
        }
        if(i==  set_memo){
            if(i==P_BARATSUKI_R){
                limitData = file.get_limit_sdata(P_BARATSUKI);
            }else if(i==P_MINI_target){
                limitData.min = 0;
                limitData.max = 500;
                limitData.lcd = 1;
            } else if(i==P_buffer2) {
                limitData = file.get_limit_sdata(P_buffer);
            } else if(i==P_bufferT) {
                limitData = file.get_limit_sdata(P_buffer);            
                limitData.min = -5;
                limitData.max =  5;
            } else if(i==P_BARA_KAKUNIN) {
                limitData = file.get_limit_sdata(P_buffer);            
                limitData.min = 1;
                limitData.max =  10;
            }else{
                limitData = file.get_limit_sdata(i);
                if(limitData.min < - 10){
                    limitData.min =0;
                }
                if(limitData.max > 10000){
                    limitData.max =10000;
                }
            }
            //printf("%d\r\n",i);
            lcd.set_secret(i);
            
            
            if(i== P_hundo){
                multi_hundo_set(sentaku_back_flg);
            }else{
                //設定データが整数か浮動小数かで処理を分ける
                if(i < fNum) Chenge_vaule(&data[i],limitData);
                else      Chenge_vaule(&fdata[i-fNum], limitData);
                /*
                if(i < fNum){
                    data[i]=value_i;
                }else{
                    fdata[i-fNum]=value_f;
                }*/
            }
        }
    }//for
    file.set_param(data, fdata);    //変更データ受け渡し
    if (endflg==1){
        file.save_param();
    }
    param = file.get_param();           //パラメータ(設定情報)を取得
    cel.set_param(file.get_param());    //パラメータをロードセルクラスへ
    //製品種別が変更されている可能性があるので再読込
    if (endflg==1){
        file.read_data_by_product(product.productType);   //製品別のデータ読み込みを行う
    }
    return;
}


/*****************************************************************************
    組み合わせカウントの設定を行う
*****************************************************************************/
void set_count(cntData_t *cnt)
{
    int iBk = cnt->max;  //処理開始時の設定カウント値を退避

    btn.ini();
    lcd.messege(TOP, DIS_CNT_SET, CLS);
    wait(0.4);

    while(btn.get_btn() != BTN_ENT) {

        if(btn.get_btn()) {
            lcd.locate(0,1);
            lcd.printf("MAX COUNT: %0004d", iBk);
        }

        btn.new_btn();              //スイッチ(New)取得
        //ボタン操作時の効果音
        if(btn.push_sound()) {
            se.Touch_01();
            wait(0.1);
        }
        if(btn.esc()) return;       //処理終了。値の変更なし

        btn.puls_or_minus(&iBk, 1, 10);

        if(iBk > COMBI_MAX_CNT) {
            iBk = 0;
        } else if(iBk < 0)       {
            iBk = COMBI_MAX_CNT;
        }

        btn.old_btn();  //スイッチ更新
    }

    //ここまできたらカウントを設定、初期化
    cnt->num = 0;       //カウント回数を初期化
    cnt->max = iBk;     //maxカウント回数を設定
}

/*****************************************************************************
    組み合わせカウントの設定を行う(ライトユーザー用)
*****************************************************************************/
void set_count_force(cntData_t *cnt)
{
    cnt->num = 0;       //カウント回数を初期化
    cnt->max = KUMI_limit-KUMI_now;     //maxカウント回数を設定
}

/*****************************************************************************
    設定値変更用関数
    pnum    変更する値のポインタを取得
    min     最小値
    max     最大値
    setLcd  値を表示するlcdの位置

    戻り値: 0(値の変更をキャンセル),1(値の変更を行った)

    注意)変数num値が0だったら、ゼロ表示ではなくオフ(カタカナ)表示を行うように変更(2016.11.29)
        変数num値が0値だったら、numが0値のときｵﾌ表示を行う(2016.12.06)
        変数num値が0値だったら、numが0値でOFF,0以外でON表示を行うよう変更(2017.01.10)
*****************************************************************************/
template <class T>
bool Chenge_vaule(T *pnum, data_t limit)
{
    T num = *pnum;
    const char off[] = {"OFF "} ; //off
    const char on[] =  {"ON  "} ; //on
    btn.ini();  //保持ボタンを初期化

    //最小値より低ければ,値を初期化
    if(float(int(num*10))/10 < limit.min) num = limit.min;
    if(float(int(num*10))/10 > limit.max) num = limit.max;

    while( btn.get_btn() != BTN_ENT ) {
        //何かキーが押されたら表示更新(もしくは一回目)
        if(btn.get_btn()) {
            if(limit.lcd < 100) {
                //行を空白で埋める
                lcd.locate(limit.lcd,1);
                lcd.printf("     ");
                lcd.locate(limit.lcd,1);
            }

            //0値以外は数値。0だったらオフ(文字)の表示を行う
            if(limit.lcd != 0 && limit.lcd < 16) {
                lcd.displayNum(num, limit.max);
            } else if(limit.lcd - 100 == PR_productType) {
                lcd.locate(0,1);
                lcd.set_product_list(num);
            } else if(limit.lcd - 100 == PR_limit_on) {
                lcd.locate(0,1);
                lcd.set_limit_list(num);
            } else if(num != 0 && limit.lcd == 0) {
                lcd.printf(on);
                lcd.displayNum(num, limit.max);
            } else {
                lcd.printf(off);
                lcd.printf("  ---");
                //lcd.displayNum(num, limit.max);
                //lcd.printf("        ");
            }
            wait(0.05);
        }

        btn.new_btn();                           //スイッチ(New)取得
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return false;
        }   //esc押下なら処理終了

        btn.puls_or_minus(&num);

        //値が範囲を超えたら修正
        if(float(int(num*10))/10 > float(limit.max))  num = limit.min;
        if(float(int(num*10))/10 < float(limit.min))  num = limit.max;
        //printf("%9.7f:%9.7f:%9.7f:%9.7f\r\n",float(int(num*10))/10,float(num),float(limit.max),2.0);
        btn.old_btn();  //スイッチ更新
    }//while

    //値を更新
    *pnum = num;
    return true;
}
template <class T>
bool Chenge_vaule_JOUGEN(T *pnum, data_t limit, int minimum)
{
    T num = *pnum;
    //const char gram[] =  {" g "} ; //on
    
    btn.ini();  //保持ボタンを初期化

    //最小値より低ければ,値を初期化
    if(int(num) < limit.min) num = limit.min;
    if(int(num) > limit.max) num = limit.max;
    
    lcd.locate(2,1);
    lcd.printf("%4d g~ ",minimum);
    lcd.locate(10,1);
    lcd.printf("%4d g",minimum + num);
    

    while( btn.get_btn() != BTN_ENT ) {
        //何かキーが押されたら表示更新(もしくは一回目)
        if(btn.get_btn()) {

            //行を空白で埋める
            lcd.locate(8,0);
            lcd.printf("(");
            lcd.locate(9,0);
            lcd.printf("     ");
            lcd.locate(9,0);
            lcd.displayNum(num, limit.max);
            lcd.printf(")");
            lcd.locate(10,1);
            lcd.printf("%4d g",minimum + num);
            wait(0.05);
        }

        btn.new_btn();                           //スイッチ(New)取得
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return false;
        }   //esc押下なら処理終了

        btn.puls_or_minus(&num);

        //値が範囲を超えたら修正
        if(num > limit.max)  num = limit.min;
        if(num < limit.min)  num = limit.max;

        btn.old_btn();  //スイッチ更新
    }//while

    //値を更新
    *pnum = num;
    return true;
}

template <class T>
bool Chenge_Measured_value(T *pnum, data_t limit)
{
    const char keisokutyu[]={0xB9,0xB2,0xBF,0xB8,0xC1,0xAD,0xB3,0xA5,0xA5,0xA5,0x00};      //ｹｲｿｸﾁｭｳ･･･
    T num = *pnum;
    btn.ini();  //保持ボタンを初期化
    bool next_go = false;
    int b_weight[5] = {0,0,0,0,0};
    int befour_num = num;
    //最小値より低ければ,値を初期化
    if(int(num) < limit.min) num = limit.min;
    if(int(num) > limit.max) num = limit.max;
    lcd.blank(1);
    set_zero_span_onehead(5);
    led.REGIS(oneBit[5]);
    v.output_case();
    while( next_go == false ) {
        if(btn.esc_press()) {//esc押下なら処理終了
            se.Touch_01();
            btn.old_btn();  //スイッチ更新
            num = befour_num;
            break;
        }
        else if(btn.any_press()) {
            se.Touch_01();
            next_go = true;
            
        }
        setHeadVolt(5, param.KURI);
        num = int(cel.anteiWeight(5)+0.5);
        b_weight[4] = b_weight[3];
        b_weight[3] = b_weight[2];
        b_weight[2] = b_weight[1];
        b_weight[1] = b_weight[0];
        b_weight[0] = num;
        
        pc.printf("%5.1f\r\n",cel.anteiWeight(5));
        lcd.locate(limit.lcd,1);
        if(num>1){
            lcd.blank(1);
            lcd.printf("%3d",num);
            lcd.printf("  ");
            lcd.printf(keisokutyu);
            //lcd.displayNum(num, 10000);
        }else{
            lcd.blank(1);
            lcd.printf(keisokutyu);
        }
        if(num>3 && b_weight[0] == b_weight[1] && b_weight[1] == b_weight[2] && b_weight[2] == b_weight[3] && b_weight[3] == b_weight[4]){
            next_go = true;
            lcd.blank(1);
            lcd.printf("%3d",num);
            lcd.printf("  ");
            se.chime_01();
            //v.output_case2();
        }
        wait(0.5);
    }
    
    if(next_go == false) {//esc押下なら処理終了
        num = befour_num;
    }
    while(btn.any_press()){wait(0.3);}
    lcd.blank(1);
    lcd.locate(4,1);
    lcd.msgPrintf(DIS_G);
    lcd.locate(0,1);
    lcd.printf("   ");
    lcd.locate(0,1);
    lcd.printf("%3d",num);
    //lcd.displayNum(num, 10000);
    while( btn.get_btn() != BTN_ENT ) {
        //何かキーが押されたら表示更新(もしくは一回目)
        if(btn.get_btn()) {
            //v.stop_voice();
            //行を空白で埋める
            lcd.locate(0,1);
            lcd.printf("   ");
            lcd.locate(0,1);

            //0値以外は数値。0だったらオフ(文字)の表示を行う
            lcd.printf("%3d",num);
            //lcd.displayNum(num, 10000);
            wait(0.05);
        }

        btn.new_btn();                           //スイッチ(New)取得
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            //v.wait_echo2();
            return false;
        }   //esc押下なら処理終了

        btn.puls_or_minus(&num);

        //値が範囲を超えたら修正
        if(num > limit.max)  num = limit.min;
        if(num < limit.min)  num = limit.max;

        btn.old_btn();  //スイッチ更新
    }//while

    //値を更新
    *pnum = num;
    
    //v.wait_echo2();
    return true;
}


bool digirap_flg=false;
int  digirap_cnt=0;
void countup_digirap(){
    digirap_cnt += 1;
    if(digirap_cnt > 5){
        if(digirap_flg==false)  {digirap_flg =true;}
        else                    {digirap_flg =false;}
    }
}
template <class T>
bool Chenge_vaule_digit(T *pnum, data_t limit,int digi, int row, int col) //
{
    T num = *pnum;
    btn.ini();  //保持ボタンを初期化
    Ticker digit_rap;                         //川  ラップ取得用
    int  digi2=0;
    bool rap_flg2=false;
    //最小値より低ければ,値を初期化
    if(int(num) < limit.min) num = limit.min;
    digit_rap.attach( &countup_digirap, 0.1);
    while( btn.get_btn() != BTN_ENT ) {
        //何かキーが押されたら表示更新(もしくは一回目)
        if(btn.get_btn()) {
            lcd.locate(col,row);
            if(digi==5){
                lcd.printf("%05d",num);
            }else if(digi==4){
                lcd.printf("%04d",num);
            }else if(digi==3){
                lcd.printf("%03d",num);
            }
            digirap_cnt=0;
            digirap_flg=false;
            wait(0.05);
        }
        if(digirap_flg==false){
            if(rap_flg2==true){
                lcd.locate(col,row);
                if(digi==5){
                    lcd.printf("%05d",num);
                }else if(digi==4){
                    lcd.printf("%04d",num);
                }else if(digi==3){
                    lcd.printf("%03d",num);
                }
                digirap_cnt=0;
                rap_flg2=false;
                wait(0.05);
            }
        }else if(digirap_flg==true){
            if(rap_flg2==false){
                if(limit.lcd>=10000){digi2=5;}
                else if(limit.lcd>=1000){digi2=4;}
                else if(limit.lcd>=100){digi2=3;}
                else if(limit.lcd>=10){digi2=2;}
                else if(limit.lcd>=1){digi2=1;}
                else                {digi2=1;}
                lcd.locate(col+(digi-digi2),row);
                lcd.printf("%c",0xFF);
                digirap_cnt=0;
                rap_flg2=true;
                wait(0.05);
            }
        }

        btn.new_btn();                           //スイッチ(New)取得
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            digit_rap.detach();
            return false;
        }   //esc押下なら処理終了

        btn.puls_or_minus(&num,limit.lcd,limit.lcd);

        //値が範囲を超えたら修正
        if(num > limit.max)  num = limit.min;
        if(num < limit.min)  num = limit.max;

        btn.old_btn();  //スイッチ更新
    }//while
    digit_rap.detach();
    //値を更新
    *pnum = num;
    return true;
}
//*****************************************************************************
template <class T>
bool Chenge_vaule_time(T *pnum, data_t limit) //limit.lcd＝90は、回数制限の数量を変更するためのもの
{
    T num = *pnum;
    btn.ini();              //保持ボタンを初期化
    //int lcd_mode=0;
    //最小値より低ければ,値を初期化
    if(int(num) < limit.min) num = limit.min;
    if(int(num) > limit.max) num = limit.max;
    //lcd_mode = limit.lcd;
    while( btn.get_btn() != BTN_ENT ) {
        //何かキーが押されたら表示更新(もしくは一回目)
        if(btn.get_btn()) {
            /*if(lcd_mode == 0) { //year
                //行を空白で埋める
                lcd.locate(0,0);
            }else if(lcd_mode == 1) { //mon
                lcd.locate(5,0);
            }else if(lcd_mode == 2) { //day
                lcd.locate(8,0);
            }else if(lcd_mode == 3) { //hour
                lcd.locate(1,1);
            }else if(lcd_mode == 4) { //min
                lcd.locate(4,1);
            }*/
            lcd.locate(0,0);
            //printf(" %2d  %d %d %d\r\n",limit.lcd,limit.lcd%10,limit.lcd/10,limit.max);
            lcd.locate(limit.lcd%100-1,limit.lcd/100);
            lcd.printf("%c",0x7E);
            lcd.locate(limit.lcd%100,limit.lcd/100);
            lcd.displayNum2(num, limit.max);
            wait(0.05);
        }

        btn.new_btn();                           //スイッチ(New)取得
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return false;
        }   //esc押下なら処理終了

        btn.puls_or_minus(&num);

        //値が範囲を超えたら修正
        if(num > limit.max)  num = limit.min;
        if(num < limit.min)  num = limit.max;

        btn.old_btn();  //スイッチ更新
    }//while
    
    lcd.locate(limit.lcd%100-1,limit.lcd/100);
    lcd.printf(" ");
    //値を更新
    *pnum = num;
    return true;
    
    
}
//*****************************************************************************
template <class T>
bool Chenge_vaule2(T *pnum, data_t limit) //limit.lcd＝90は、回数制限の数量を変更するためのもの
{
    T num = *pnum;
    const char off[] = {"OFF "} ; //off
    const char on[] =  {"ON  "} ; //on
    //const char amen[]= {'A' ,0xD2,0xDD,' ' ,0x28,'1' ,'-' ,'6' ,0xBB,0xD7,' ' ,0xBC,0xD6,0xB3,0x29,0x00};         //Aﾒﾝ (1-6ｻﾗ ｼﾖｳ)
    //const char bmen[]= {'B' ,0xD2,0xDD,' ' ,0x28,'7' ,'-' ,'1' ,'2' ,0xBB,0xD7,' ' ,0xBC,0xD6,0xB3,0x29,0x00};    //Bﾒﾝ (7-12ｻﾗ ｼﾖｳ)
    const char kokuti[4][SIZE]= {{0xC4,0xD8,0xB5,0xDC,0xD8,' ' ,0xC9,0xD0,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}     //ﾄﾘｵﾜﾘ ﾉﾐ
        ,   {0xBE,0xB2,0xD8,0xC2,0xBC,0xDE,' ' ,0xC9,0xD0,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｾｲﾘﾂｼﾞ ﾉﾐ
        ,   {0xBE,0xB2,0xD8,0xC2,'/' ,0xC4,0xD8,0xB5,0xDC,0xD8,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｾｲﾘﾂ/ﾄﾘｵﾜﾘ
        ,   {0xD2,0xDB,0xC3,0xDE,0xA8,0xB0,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾒﾛﾃﾞｨｰ

    };
    const char button[2][SIZE]= { {0xB6,0xB3,0xDD,0xC4,' ' ,0xBE,0xAF,0xC3,0xB2,0x00}     //ｶｳﾝﾄ ｾｯﾃｲ
        ,{0xB9,0xB2,0xD8,0xAE,0xB3,' ' ,0xD3,0xB0,0xC4,0xDE,0x00}            //ｹｲﾘｮｳ ﾓｰﾄﾞ
    };
    const char rotate[4][SIZE]= { {'O' ,'F' ,'F' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}    
        , {0xCA,0xDF,0xC0,0xB0,0xDD,'1' ,' ' ,' ' ,' ' ,0x00}      //ﾊﾟﾀｰﾝ1
        , {0xCA,0xDF,0xC0,0xB0,0xDD,'2' ,' ' ,' ' ,' ' ,0x00}      //ﾊﾟﾀｰﾝ2
    };
    const char not_do[2][SIZE]= {{0xBC,0xC5,0xB2,0x00},{0xBD,0xD9,' ' ,0x00}};          //ｼﾅｲ//ｽﾙ 
    
    const char tarinai[2][SIZE]= {{0xC0,0xD8,0xC5,0xB2,0xC4,0xB7,' ' ,0xB1,0xB6,' ' ,' ' ,0x00}      //ﾀﾘﾅｲﾎｳ ｱｶ
                                 ,{0xC0,0xD8,0xC5,0xB2,0xC4,0xB7,' ' ,0xD0,0xC4,0xDE,0xD8,0x00}};      //ﾀﾘﾅｲﾄｷ ﾐﾄﾞﾘ


    const char no_combi_v[3][SIZE]= {  {'O' ,'F' ,'F' ,' ' ,' ' ,0x00} 
                                      ,{0xB5,0xDD,0xBE,0xB2,' ' ,0x00}      //ｵﾝｾｲ
                                      ,{0xB5,0xC4,' ' ,' ' ,' ' ,0x00}};    //ｵﾄ
    const char n_minits[]={0xCC,0xDD,0x00};      //ﾌﾝ

    
//{0xD0,0xC4,0xDE,0xD8,0xCE,0xDE,0xC0,0xDD,' ' ,0xBE,0xAF,0xC3,0xB2,0x00}      //ﾐﾄﾞﾘﾎﾞﾀﾝ ｾｯﾃｲ

    btn.ini();              //保持ボタンを初期化
    int lcd_mode=0;
    //最小値より低ければ,値を初期化
    if(int(num) < limit.min) num = limit.min;
    lcd_mode = limit.lcd;
    int dumy_num=limit.min;         //表示用数量保存場所(例)回数制限の数を見せたいとき
    if(lcd_mode == 90) {//回数制限の数量を変更
        limit.min = 0;
        limit.max = 1;
        num=0;
        lcd.locate(9,1);
        lcd.displayNum(dumy_num, dumy_num);
    }
    if(limit.lcd>1) {
        limit.lcd=1;
    }
    if(lcd_mode == OP_CHAR_ROTATE) {
        for(int l=0; l<product.head; l++) {
            iro[l]=C_M;
        }
    }
    
    while( btn.get_btn() != BTN_ENT ) {
        //何かキーが押されたら表示更新(もしくは一回目)
        if(btn.get_btn()) {
            if(lcd_mode == OP_KOKUTI) {
                //行を空白で埋める
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                if(num==0) {
                    lcd.printf(kokuti[num]);
                } else if(num==1) {
                    lcd.printf(kokuti[num]);
                } else {
                    lcd.printf(kokuti[num]);
                }
            } else if(lcd_mode == OP_BTN_SETTING) {
                //行を空白で埋める
                lcd.locate(limit.lcd,1);
                lcd.printf("    ");
                lcd.locate(limit.lcd,1);
                if(num != 0) {
                    lcd.printf("YES");
                    //lcd.displayNum(num, limit.max);
                } else {
                    lcd.printf("NO");
                    //lcd.displayNum(num, limit.max);
                }
            } else if(lcd_mode == OP_CHAR_ROTATE) {
                //行を空白で埋める
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                if(num==0) {
                    lcd.printf(rotate[num]);
                } else if(num==1) {
                    lcd.printf(rotate[num]);
                } else {
                    lcd.printf(rotate[num]);
                }
                option.c_rotate = num;
            } else if(lcd_mode == OP_BEGINNER) {
                //行を空白で埋める
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                if(num != 0) {
                    lcd.printf(on);
                    //lcd.displayNum(num, limit.max);
                } else {
                    lcd.printf(off);
                    //lcd.displayNum(num, limit.max);
                }
            } else if(lcd_mode == OP_SEL_BEEP) {
                //行を空白で埋める
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                lcd.printf("%02d",num);
                se.mistake3(num);
            } else if(lcd_mode == OP_NO_COMBI_V) {
                //行を空白で埋める
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                lcd.printf(no_combi_v[num]);
                wait(0.05);
            } else if(lcd_mode == OP_SECONDMBI_V) {
                //行を空白で埋める
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                lcd.printf(not_do[num]);
            } else if(lcd_mode == OP_NUMBER_COLOR) {
                //行を空白で埋める
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                //lcd.printf(not_do[num]);
                lcd.printf(tarinai[num]);
                
            } else if(lcd_mode == OP_O_count_lim) {
                //行を空白で埋める
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                //lcd.printf(not_do[num]);
                lcd.printf("%4d",num);
                
            } else if(lcd_mode == OP_O_lit_count_lim) {
                //行を空白で埋める
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                //lcd.printf(not_do[num]);
                lcd.printf("%4d",num);
                
            } else if(lcd_mode == OP_NAGAMONO_HOSEI) {
                //行を空白で埋める
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                //lcd.printf(not_do[num]);
                if(num == 0) {lcd.printf(off);}
                else{
                    lcd.printf("%5.1f",float(num)/10);
                }
            } else if(lcd_mode == OP_NAGAMONO_HOSEI2) {
                //行を空白で埋める
                set_lastHeadVolt(param.KURI);
                lcd.locate(7,1);
                lcd.printf("%5.0f",cel.get_lastheadWeight() + float(num)/10);
                lcd.locate(0,1);
                lcd.printf("      ");
                lcd.locate(0,1);
                //lcd.printf(not_do[num]);
                
                if(num == 0) {lcd.printf(off);}
                else{
                    lcd.printf("%5.1f",float(num)/10);
                    //lcd.printf("%5d",num);
                }
                iro[cel.get_lasthead()] = CR_01 + cel.get_lastheadWeight()+ float(num)/10 -1;
            } else if(lcd_mode == OP_WAITING_LIMMIT) {
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                if(num == 0) {lcd.printf(off);}
                else{
                    lcd.printf("%5d",num);
                    lcd.printf(n_minits);
                }
            } else if(lcd_mode == OP_SELAUTOZERO) {
                //行を空白で埋める
                lcd.locate(0,1);
                lcd.printf("                ");
                lcd.locate(0,1);
                if(num != 0) {
                    lcd.printf(on);
                    auto_zero_off = false;
                    //lcd.displayNum(num, limit.max);
                } else {
                    lcd.printf(off);
                    auto_zero_off = true;
                    //lcd.displayNum(num, limit.max);
                }
            } else if(lcd_mode == OP_KEKKA_HYOUJI) {
                lcd.blank(1);
                if(num != 0) {
                    lcd.printf(on);
                    //lcd.displayNum(num, limit.max);
                } else {
                    lcd.printf(off);
                    //lcd.displayNum(num, limit.max);
                }
            } else if(lcd_mode == OP_ALART_AUTOZERO) {
                lcd.blank(1);
                if(num != 0) {
                    lcd.printf(on);
                    //lcd.displayNum(num, limit.max);
                } else {
                    lcd.printf(off);
                    //lcd.displayNum(num, limit.max);
                }
            } else if(lcd_mode == OP_FIX_VALUE) {
                lcd.blank(1);
                if(num != 0) {
                    lcd.printf(on);
                    //lcd.displayNum(num, limit.max);
                } else {
                    lcd.printf(off);
                    //lcd.displayNum(num, limit.max);
                }

                
            } else if(lcd_mode == 90) {//回数制限の数量を変更
                //行を空白で埋める
                lcd.locate(limit.lcd,1);
                lcd.printf("    ");
                lcd.locate(limit.lcd,1);
                if(num != 0) {
                    lcd.printf("YES");
                    //lcd.displayNum(num, limit.max);
                } else {
                    lcd.printf("NO");
                    //lcd.displayNum(num, limit.max);
                }
                wait(0.05);
            } else {
                //行を空白で埋める
                lcd.locate(limit.lcd,1);
                lcd.printf("      ");
                lcd.locate(limit.lcd,1);
                //lcd.locate(limit.lcd,0);
                //0値以外は数値。0だったらオフ(文字)の表示を行う
                if(limit.lcd != 0) {
                    lcd.displayNum(num, limit.max);
                } else if(num != 0 && limit.lcd == 0) {
                    lcd.printf(on);
                    //lcd.displayNum(num, limit.max);
                } else {
                    lcd.printf(off);
                    //lcd.displayNum(num, limit.max);
                }
                wait(0.05);
            }
        }
        
        if(btn.any_press() == false && lcd_mode == OP_NAGAMONO_HOSEI2) {
            //行を空白で埋める
            set_lastHeadVolt(param.KURI);
            lcd.locate(7,1);
            //lcd.printf("%5.0f",cel.get_lastheadWeight() + num);
            lcd.printf("%5.0f",cel.get_lastheadWeight() + float(num)/10);
            iro[cel.get_lasthead()] = CR_01 + cel.get_lastheadWeight()+ float(num)/10 -1;
            //lcd.printf(not_do[num]);
        }
        

        btn.new_btn();                           //スイッチ(New)取得
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return false;
        }   //esc押下なら処理終了

        btn.puls_or_minus(&num);

        //値が範囲を超えたら修正
        if(num > limit.max)  num = limit.min;
        if(num < limit.min)  num = limit.max;

        btn.old_btn();  //スイッチ更新
    }//while

    if(lcd_mode == OP_CHAR_ROTATE) {
        for(int l=0; l<product.head; l++) {
            iro[l]=C_NULL;
        }
    }
    //値を更新
    *pnum = num;
    return true;
}
//*****************************************************************************
template <class T>
bool Chenge_name_vaule(T *pnum, data_t limit,int paramhead,int col)
{
    T num = *pnum;
    btn.ini();  //保持ボタンを初期化
    int nowloop=0;  //表示切替のループｶｳﾝﾄ
    int b_num=0;  //変化がある前の値
    //最小値より低ければ,値を初期化
    if(num!=-40 && int(num) < limit.min) num = limit.min;
    lcd.locate(col,1);
    lcd.printf("%c",0xFF);
    if(num==-40||num==0) {           //[END?]より手前
        num=0;//[END?]
        lcd.locate(col,1);
        lcd.printf("END?");
        lcd.locate(col,1);
    }
    wait(0.3);
    while( btn.get_btn() != BTN_ENT ) {
        nowloop+=1;
        //printf("lll%d\r\n",nowloop);
        //表示の更新(文字と白塗りつぶし)
        if(nowloop>90) {
            if(num!=0) {
                lcd.locate(col,1);
                lcd.printf("%c",0x40+num);
            }
        }
        if(nowloop>250) {
            if(num!=0) {
                lcd.locate(col,1);
                lcd.printf("%c",0xFF);
            }
            nowloop=0;
            //printf("SS\r\n");
        }
        //何かキーが押されたら表示更新(もしくは一回目)
        if(btn.get_btn()) {

            //行を空白で埋める
            lcd.locate(col,1);
            lcd.printf(" ");
            lcd.locate(col,1);
            if(num==0) {
                lcd.locate(col,1);
                lcd.printf("END?");
                lcd.locate(col,1);
            } else {
                if(b_num==0) {
                    lcd.locate(0,1);
                    lcd.printf("                ");
                    lcd.locate(0,1);
                    printname(paramhead);
                }
                lcd.locate(col,1);
                lcd.printf("%c",0x40+num);
                wait(0.1);;
                nowloop=0;
            }
        }

        btn.new_btn();                           //スイッチ(New)取得
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return false;
        }   //esc押下なら処理終了
        b_num = num;
        btn.puls_or_minus(&num);
        if(num==-1 && b_num==0) {           //[END?]より手前
            num=-7;                   //数値9表示
        } else if(num==-6) {               //[9]の先
            num=0;                      //[END?]表示
        } else if(num==27) {                 //[ﾟ]の先
            num=-16;                    //[ｦ]表示
        } else if(num==-17) {                //[ｦ]より手前
            num=26;                     //[ﾟ]表示
        }
        //値が範囲を超えたら修正
        if(num > limit.max)  num = limit.min;
        if(num < limit.min)  num = limit.max;

        btn.old_btn();  //スイッチ更新
    }//while

    //値を更新
    *pnum = num;
    return true;
}

int Chenge_vaule_btnD(int option){
    const char kokuti[]={0xBB,0xB7,0xDE,0xAE,0xB3,' ' ,0xBE,0xDD,0xC0,0xB8,0x00};      //ｻｷﾞｮｳ ｾﾝﾀｸ

    const char button_j[5][SIZE]={
                                  {}
                                 ,{0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,0xC1,0xAA,0xAF,0xB8,' ' ,' ' ,' ' ,' ' ,0x00}      //ｼﾞｭｳﾘｮｳ ﾁｪｯｸ
                                 ,{0xBD,0xCB,0xDF,0xB0,0xC4,0xDE,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｽﾋﾟｰﾄﾞ ﾍﾝｺｳ
                                 ,{0xBC,0xDE,0xC4,0xDE,0xB3,0xCC,0xB3,0xC0,0xB2,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,0x00}      //ｼﾞﾄﾞｳﾌｳﾀｲ ﾍﾝｺｳ
                                 ,{0xCE,0xBE,0xB2,0xC1,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾎｾｲﾁ ﾍﾝｺｳ

                                };
    const char button_k[8][SIZE]={
                                  {'L' ,'E' ,'D' ,0xC3,0xDD,0xC4,0xB3,0xBC,0xDE,0xB6,0xDD,' ' ,0xCD,0xDD,0xBA,0xB3,0x00}      //LEDﾃﾝﾄｳｼﾞｶﾝ ﾍﾝｺｳ
                                 ,{0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,0xC1,0xAA,0xAF,0xB8,' ' ,' ' ,' ' ,' ' ,0x00}      //ｼﾞｭｳﾘｮｳ ﾁｪｯｸ
                                 ,{'L' ,'E' ,'D' ,0xC3,0xDD,0xC4,0xB3,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,' ' ,' ' ,' ' ,0x00}      //LEDﾃﾝﾄｳ ｾｯﾃｲ
                                 ,{0xBE,0xB2,0xB9,0xDE,0xDD,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｾｲｹﾞﾝ ﾍﾝｺｳ
                                 ,{0xCE,0xBE,0xB2,0xC1,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾎｾｲﾁ ﾍﾝｺｳ　　
                                 ,{0xCC,0xBE,0xB2,0xD8,0xC2,' ' ,0xBA,0xB8,0xC1,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾌｾｲﾘﾂ ｺｸﾁ
                                 ,{0xBA,0xB8,0xC1,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｺｸﾁ ｾｯﾃｲ
                                 ,{0xBB,0xB0,0xCE,0xDE,' ' ,0xCD,0xDD,0xBA,0xB3,'2' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｻｰﾎﾞ ﾍﾝｺｳ2
                                 /*{'L','E','D',0xC3,0xDD,0xC4,0xB3,' ',0xBE,0xAF,0xC3,0xB2,0x00}    //LEDﾃﾝﾄｳ ｾｯﾃｲ
                                 ,{0xB9,0xB2,0xD8,0xAE,0xB3,' ' ,0xD3,0xB0,0xC4,0xDE,0x00}            //ｹｲﾘｮｳ ﾓｰﾄﾞ
                                 ,{0xD3,0xB0,0xC4,0xDE,' ' ,0xCD,0xDD,0xBA,0xB3,0x00}                 //ﾓｰﾄﾞ ﾍﾝｺｳ
                                 ,{0xBE,0xB2,0xB9,0xDE,0xDD,' ' ,0xCD,0xDD,0xBA,0xB3,0x00}*/      //ｾｲｹﾞﾝ ﾍﾝｺｳ
                                };
    const char button_y[10][SIZE]={
                                  {0xB6,0xB3,0xDD,0xC4,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｶｳﾝﾄ ｾｯﾃｲ
                                 ,{0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,0xC1,0xAA,0xAF,0xB8,' ' ,' ' ,' ' ,' ' ,0x00}      //ｼﾞｭｳﾘｮｳ ﾁｪｯｸ
                                 ,{'L' ,'E' ,'D' ,0xC3,0xDD,0xC4,0xB3,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,' ' ,' ' ,' ' ,0x00}      //LEDﾃﾝﾄｳ ｾｯﾃｲ
                                 ,{0xBE,0xB2,0xB9,0xDE,0xDD,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｾｲｹﾞﾝ ﾍﾝｺｳ
                                 ,{0xB4,0xD7,0xB0,' ' ,0xBB,0xB2,0xB9,0xB2,0xBB,0xDD,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｴﾗｰ ｻｲｹｲｻﾝ
                                 ,{0xB4,0xD7,0xB0,0xB5,0xDD,0xBE,0xB2,' ' ,0xBC,0xAD,0xC2,0xD8,0xAE,0xB8,' ' ,' ' ,0x00}      //ｴﾗｰｵﾝｾｲ ｼｭﾂﾘｮｸ
                                 ,{0xBA,0xB8,0xC1,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｺｸﾁ ｾｯﾃｲ
                                 ,{0xB4,0xD7,0xB0,0xB5,0xDD,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｴﾗｰｵﾝ ﾍﾝｺｳ
                                 ,{0xCE,0xBE,0xB2,0xC1,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾎｾｲﾁ ﾍﾝｺｳ
                                 //,{0xB8,0xD0,0xB1,0xDC,0xBE,0xC5,0xBC,' ' ,0xBA,0xB8,0xC1,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｸﾐｱﾜｾﾅｼ ｺｸﾁ
                                 ,{0xCC,0xBE,0xB2,0xD8,0xC2,' ' ,0xBA,0xB8,0xC1,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾌｾｲﾘﾂ ｺｸﾁ
                                  /*{0xB6,0xB3,0xDD,0xC4,' ' ,0xBE,0xAF,0xC3,0xB2,0x00}     //ｶｳﾝﾄ ｾｯﾃｲ
                                 ,{0xB9,0xB2,0xD8,0xAE,0xB3,' ' ,0xD3,0xB0,0xC4,0xDE,0x00}     //ｹｲﾘｮｳ ﾓｰﾄﾞ
                                 ,{0xD3,0xB0,0xC4,0xDE,' ' ,0xCD,0xDD,0xBA,0xB3,0x00}                 //ﾓｰﾄﾞ ﾍﾝｺｳ
                                 ,{0xBE,0xB2,0xB9,0xDE,0xDD,' ' ,0xCD,0xDD,0xBA,0xB3,0x00}*/      //ｾｲｹﾞﾝ ﾍﾝｺｳ
                               }; 
    const char button_o[8][SIZE]={//親子計量時のオプション
                                  {0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,0xC1,0xAA,0xAF,0xB8,' ' ,' ' ,' ' ,' ' ,0x00}      //ｼﾞｭｳﾘｮｳ ﾁｪｯｸ
                                 ,{'L' ,'E' ,'D' ,0xC3,0xDD,0xC4,0xB3,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,' ' ,' ' ,' ' ,0x00}      //LEDﾃﾝﾄｳ ｾｯﾃｲ
                                 ,{0xB4,0xD7,0xB0,' ' ,0xBB,0xB2,0xB9,0xB2,0xBB,0xDD,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｴﾗｰ ｻｲｹｲｻﾝ
                                 ,{0xB4,0xD7,0xB0,0xB5,0xDD,0xBE,0xB2,' ' ,0xBC,0xAD,0xC2,0xD8,0xAE,0xB8,' ' ,' ' ,0x00}      //ｴﾗｰｵﾝｾｲ ｼｭﾂﾘｮｸ
                                 ,{0xBA,0xB8,0xC1,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｺｸﾁ ｾｯﾃｲ
                                 ,{0xB4,0xD7,0xB0,0xB5,0xDD,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｴﾗｰｵﾝ ﾍﾝｺｳ
                                 ,{0xCE,0xBE,0xB2,0xC1,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾎｾｲﾁ ﾍﾝｺｳ
                                 //,{0xB8,0xD0,0xB1,0xDC,0xBE,0xC5,0xBC,' ' ,0xBA,0xB8,0xC1,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｸﾐｱﾜｾﾅｼ ｺｸﾁ
                                 ,{0xCC,0xBE,0xB2,0xD8,0xC2,' ' ,0xBA,0xB8,0xC1,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾌｾｲﾘﾂ ｺｸﾁ
                                  /*{0xB6,0xB3,0xDD,0xC4,' ' ,0xBE,0xAF,0xC3,0xB2,0x00}     //ｶｳﾝﾄ ｾｯﾃｲ
                                 ,{0xB9,0xB2,0xD8,0xAE,0xB3,' ' ,0xD3,0xB0,0xC4,0xDE,0x00}     //ｹｲﾘｮｳ ﾓｰﾄﾞ
                                 ,{0xD3,0xB0,0xC4,0xDE,' ' ,0xCD,0xDD,0xBA,0xB3,0x00}                 //ﾓｰﾄﾞ ﾍﾝｺｳ
                                 ,{0xBE,0xB2,0xB9,0xDE,0xDD,' ' ,0xCD,0xDD,0xBA,0xB3,0x00}*/      //ｾｲｹﾞﾝ ﾍﾝｺｳ
                               }; 
    const char button_o2[2][SIZE]={//DENSO計量時のオプション
                                  {0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,0xC1,0xAA,0xAF,0xB8,' ' ,' ' ,' ' ,' ' ,0x00}      //ｼﾞｭｳﾘｮｳ ﾁｪｯｸ
                                 //,{0xBA,0xB8,0xC1,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｺｸﾁ ｾｯﾃｲ
                                 ,{0xBD,0xB3,0xBC,0xDE,0xC9,' ' ,0xB2,0xDB,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｽｳｼﾞﾉ ｲﾛ
                               }; 
    const char button_kaisu[9][SIZE]={//回数計量時のオプション
                                  {0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,0xC1,0xAA,0xAF,0xB8,' ' ,' ' ,' ' ,' ' ,0x00}      //ｼﾞｭｳﾘｮｳ ﾁｪｯｸ
                                 ,{'L' ,'E' ,'D' ,0xC3,0xDD,0xC4,0xB3,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,' ' ,' ' ,' ' ,0x00}      //LEDﾃﾝﾄｳ ｾｯﾃｲ
                                 ,{0xB4,0xD7,0xB0,' ' ,0xBB,0xB2,0xB9,0xB2,0xBB,0xDD,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｴﾗｰ ｻｲｹｲｻﾝ
                                 ,{0xB4,0xD7,0xB0,0xB5,0xDD,0xBE,0xB2,' ' ,0xBC,0xAD,0xC2,0xD8,0xAE,0xB8,' ' ,' ' ,0x00}      //ｴﾗｰｵﾝｾｲ ｼｭﾂﾘｮｸ
                                 ,{0xBA,0xB8,0xC1,' ' ,0xBE,0xAF,0xC3,0xB2,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｺｸﾁ ｾｯﾃｲ
                                 ,{0xB4,0xD7,0xB0,0xB5,0xDD,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｴﾗｰｵﾝ ﾍﾝｺｳ
                                 ,{0xCE,0xBE,0xB2,0xC1,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾎｾｲﾁ ﾍﾝｺｳ
                                 //,{0xB8,0xD0,0xB1,0xDC,0xBE,0xC5,0xBC,' ' ,0xBA,0xB8,0xC1,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｸﾐｱﾜｾﾅｼ ｺｸﾁ
                                 ,{0xCC,0xBE,0xB2,0xD8,0xC2,' ' ,0xBA,0xB8,0xC1,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾌｾｲﾘﾂ ｺｸﾁ
                                 ,{0xD3,0xBC,0xDE,' ' ,0xB6,0xB2,0xC3,0xDD,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾓｼﾞ ｶｲﾃﾝ
                               }; 
                               
    const char button_rank[2][SIZE]={
                                  {0xCB,0xAE,0xB3,0xBC,0xDE,' ' ,0xCD,0xDD,0xBA,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾋｮｳｼﾞ ﾍﾝｺｳ
                                 ,{0xBC,0xDE,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,0xC1,0xAA,0xAF,0xB8,' ' ,' ' ,' ' ,' ' ,0x00}      //ｼﾞｭｳﾘｮｳ ﾁｪｯｸ
                                };
                               
    data_t limit;
    int num = 0;
    lcd.cls();                  //表示をクリア
    btn.ini();              //保持ボタンを初期化
    limit.min = 0;
    limit.max = 5;
    if(option == 99 ){
        limit.min = 1; //小数点に対応していないので保留
        limit.max = 1;
    }else if(load.kumi_flag == KUMI_JUURYO) {
        if(gLevel != 1){
            return 1;
        }
        limit.min = 1;
        limit.max = 4;
    }else if(load.kumi_flag == KUMI_OYAKO || load.kumi_flag == KUMI_OYAKO_HALF){
        limit.min = 0;
        limit.max = 7;
    }else if(load.kumi_flag == KUMI_DENSO){
        limit.min = 0;
        limit.max = 1;
    }else if(load.kumi_flag == KUMI_KAISU1 || load.kumi_flag == KUMI_KAISU2){
        limit.min = 0;
        limit.max = 8;
    }else if(load.spMode == SPEED_LOW) {
        limit.max = 9;
        if(option == 98 ){
            limit.min = 1;
        }
    }else if(load.spMode == SPEED_HIGH || load.spMode == SPEED_HYPER || load.spMode==SPEED_DAMP) {
        limit.min = 0;
        limit.max = 6;
        if(load.spMode==SPEED_DAMP) {
            limit.max = 7;
        }
    }
    limit.lcd = 0;
    num=0;
    if(option == 1) {
        num=1;
    }
    if(num < limit.min) num = limit.min;
    if(num > limit.max) num = limit.max;
    lcd.locate(0,0);
    lcd.printf("                ");
    lcd.locate(0,0);
    lcd.printf(kokuti);


    if(limit.lcd>1) {
        limit.lcd=1;
    }
    while( btn.get_btn() != BTN_ENT ) {
        //何かキーが押されたら表示更新(もしくは一回目)
        if(btn.get_btn()) {
            //行を空白で埋める
            lcd.locate(limit.lcd,1);
            lcd.printf("             ");
            lcd.locate(limit.lcd,1);
            if(option == 99 ){
                lcd.printf(button_rank[num]);
            }else if(load.kumi_flag == KUMI_JUURYO) {
                lcd.printf(button_j[num]);
            }else if(load.kumi_flag == KUMI_OYAKO || load.kumi_flag == KUMI_OYAKO_HALF){
                lcd.printf(button_o[num]);
            }else if(load.kumi_flag == KUMI_DENSO){
                lcd.printf(button_o2[num]);
            }else if(load.kumi_flag == KUMI_KAISU1 || load.kumi_flag == KUMI_KAISU2){
                lcd.printf(button_kaisu[num]);
            }else if(load.spMode == SPEED_LOW) {
                lcd.printf(button_y[num]);
            }else if(load.spMode == SPEED_HIGH || load.spMode ==SPEED_DAMP) {
                lcd.printf(button_k[num]);
            } else {
                lcd.printf(button_k[num]);
            }
            wait(0.05);
        }
        btn.new_btn();                           //スイッチ(New)取得
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            return 99;
        }   //esc押下なら処理終了

        btn.puls_or_minus(&num);

        //値が範囲を超えたら修正
        if(num > limit.max)  num = limit.min;
        if(num < limit.min)  num = limit.max;

        btn.old_btn();  //スイッチ更新
    }//while
    lcd.locate(0,0);
    lcd.printf("                ");
    lcd.locate(0,0);

    //値を更新
    return num;
}

void printname(int headPosition)
{
    for(int k=0; k<10; k++) {
        if(name[headPosition][k]==0) {
            name[headPosition][k]=-40;
        }
    }
    lcd.printf("%c%c%c%c%c%c%c%c%c%c",0x40+name[headPosition][0],0x40+name[headPosition][1],0x40+name[headPosition][2],0x40+name[headPosition][3],0x40+name[headPosition][4],0x40+name[headPosition][5],0x40+name[headPosition][6],0x40+name[headPosition][7],0x40+name[headPosition][8],0x40+name[headPosition][9]);
}
//ロードセル電圧を取得
//取得するヘッド位置と、平均移動回数を指定
void setHeadVolt(int head, int repert)
{
    /*if(((close_wait_flg & (1 << head)) > 0) && (moving2 & (1 << head)) >0){
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("ZERO_SET");
        se.Touch_01();   //ボタン操作時の効果音
        set_zero_span_onehead(head);
        v.speak_numbers(head+1);
        close_wait_flg = close_wait_flg ^ (close_wait_flg & (1 << head));
        lcd.cls();
    }*/
    //printf("%2d  %d  %d  %d\r\n",head,span_mode_f,oneBit[head] & stop_cells,stop_cells);
    if(span_mode_f == 0 && (oneBit[head] & stop_cells) > 0) { //スパン中でない&不使用セルだったら0gを返す
        cel.set_head(head);
        cel.setAin_zero(head);
    } else if(option.rank_speed == RANK_FAST && repert < 300) {
        cel.set_head(head);
        cel.setAinSpeed(repert);
    } else {
        cel.set_head(head);
        cel.setAin(repert);
    }
}

void set_lastHeadVolt(int repert)//ヘッドをしよう
{
    cel.setAin(repert);
}

//マスク箇所のbit位置の電圧を取得する
void get_head_mask_volt(const int mask)
{
    //必要な分だけ重量取得
    for(int i = mask,bit = 0; i; i >>= 1) {
        if(i & 0x01) setHeadVolt(bit, param.KURI);
        bit++;
    }
}

//配列を初期化(初期値はダミー値をいれる)
void ArrayIni(combiData_t *ikk, int num)
{
    for(int i = 0; i < num; i++) {
        ikk->kumi = 0;
        ikk->total = DAMMY_MAX;
        ikk->priolity = 999;
        ikk++;
    }
}

/* 比較関数 クイックソート */
int comp( const void *c1, const void *c2 )
{
    combiData_t test1 = *(combiData_t *)c1;
    combiData_t test2 = *(combiData_t *)c2;

    int tmp1 = test1.total;   /* total を基準とする */
    int tmp2 = test2.total;

    return tmp1 - tmp2;
}
/* 比較関数2 クイックソート */
int comp2( const void *c1, const void *c2 )
{
    combiData_t test1 = *(combiData_t *)c1;
    combiData_t test2 = *(combiData_t *)c2;

    int tmp1 = test1.priolity;   /* total を基準とする */
    int tmp2 = test2.priolity;

    return tmp1 - tmp2;
}
void USB_check()
{
    char SAVEER[2][16]= {{0xB4,0xD7,0xB0,0x21,0x21,0x00}   //ｴﾗｰ!!
        ,{'U' ,'S' ,'B' ,0xB6,0xDE,0xD0,0xC2,0xB6,0xD8,0xCF,0xBE,0xDD,0x00}
    };    //USBｶﾞﾐﾂｶﾘﾏｾﾝ
    char SAVEFILE[300];  //選択されているプリセット位置
    sprintf(SAVEFILE,"/usb/%02dTOTALR.txt", file.get_preset()+1);
    if(msc.disk_initialize()!=1) {

    } else {
        lcd.blank(0);
        lcd.blank(1);
        lcd.locate(0,0);
        lcd.printf(SAVEER[0]);
        lcd.locate(0,1);
        lcd.printf(SAVEER[1]);
        se.chime_01();    //警告音
        wait(2);
        lcd.blank(0);
        lcd.blank(1);
    }
}


void matrix_test(){
    char multiweight[SIZE] = {0xD7,0xDD,0xB8,0xCD,0xDE,0xC2,' ' ,0xB8,0xD0,0xB1,0xDC,0xBE,0x00};
    char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
                ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼﾀ
    int secondCnt = 0;                        //一定数に達したら準最適値処理開始
    int *stay_cnt = new int[product.head];      //滞留警告判断のためのカウント変数
    //float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    //cntData_t cnt= {0,0,0};                     //カウント機能の値保持用
    combiData_t orderCombi[BK_CNT] = {0,0.0,0};   //組み合わせ候補のデータ(配列)
    combiData_t resultCombi;                    //組み合わせ結果(最小値取得用)
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    cnt.nowMode = load.spMode;       //処理モード取得
    cnt.num     = 0;                 //処理モード取得
    cnt.max     = 0;                 //処理モード取得
    cnt_miss_and_kikakugai=0;      //取り間違いと規格外が発生した回数
    cnt_junsaiteki=0;               //準最適値が発生した回数
    int btnset =0;//計量中の下ボタン押しで出てくる値保存
    int SerialNumber = 0; //USBを連続で使用する際の連番値
    int first_target = load.target;
    int first_jougen = load.jougen;
    int scond_target = load.taimenTarget;
    int scond_jougen = load.taimenJougen;
    
    befoure_check = 0;      //一個前に確認した、排出待ちのビット
    now_____check = 0;      //一個前に確認した、排出待ちのビット
    check_same_count = 0;      //排出待ちのビットが同一だった時のカウント
    check_same_max = 0;

    int iro_ck =C_S;        //マトリクス表示内容
    int ck_lp =0;           //繰返し回数
    int ck_end_f=0;
    int ck_all_end_f=0;
    int wait_lp = 0;

    if(off_on_no_yes_result(2,0)==1){

        load.kumi_flag       = 0;
        load.combiflug       = 0;
        load.kumi_min        = 0;
        load.kumi_max        = 0;
        load.kumi_priolity_1 = 99;
        load.kumi_priolity_2 = 99;
        load.katamen         = 0;
        load.tanjuu          = 0;
        load.target          = 9999;
        load.jougen          = 0;
        load.taimenTarget    = 0;
        load.taimenJougen    = 0;
        load.fuzoroiBoushi   = 0;
        load.secondOnOff     = 0;
        load.spMode          = 2;
        load.lightTime       = 1;
        load.rankOnOff       = 0;
        load.rankMin         = 0;
        load.rankMak         = 0;
        load.check_hyouji    = 0;
        load.kuri_flag       = 0;
        int matrix_loop = -1;
        int matrix_loop_limit = 5;

        bool iro_stop_flg = false;
        int now_iro_no = 1;
        int now_iro_set = ALL_G;

        file.read_hosei(file.get_preset());
        no_combi_v_flg = false;
        set_stopcell();
        if(servo_connection==true && load.spMode == SPEED_DAMP){
            send_i(EJECT_RESET, 11+load.kumi_flag, 0);
        }
        //1行目LCD表示
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            lcd.locate(0,0);
            lcd.printf(multiweight);
            lcd.blank(LOW);
            lcd.printf(":----.-g       ");
        } else {
            lcd.settings(&load);      //設定内容を表示
        }
        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            for(int l=0;l<product.head;l++){
                iro[l]=C_NULL;
            }
        }

        //作業開始時間記録
        seconds = time(NULL);
        tm *dated3 = localtime(&seconds);
        run_date.tm_year = dated3->tm_year+1900;
        run_date.tm_sec  = dated3->tm_sec;
        run_date.tm_min  = dated3->tm_min;
        run_date.tm_hour = dated3->tm_hour;
        run_date.tm_mday = dated3->tm_mday;
        run_date.tm_mon  = dated3->tm_mon+1;
        run_date.tm_year = dated3->tm_year+1900;
        //
        //printf("st%d:%d:%d\r\n",run_date.tm_hour,run_date.tm_min,run_date.tm_sec);

        initial_drive_function();
        /*for(int l; l<product.head; l++) {
            nowweightVale3[l]=0.0;
            servo_b_histry[l]   =0;
            servo_n_histry[0][l]=0;
            servo_n_histry[1][l]=0;
        }*/
        for(int i = 0; i < product.head; i++) {
            no_zero_counter[i] = 0;
        }
        close_wait_flg = 0;

        if(product.use_usb==1 && option.usb_EN==true){
            //SerialNumber = USB_filecheck();
            SerialNumber = USB_filecheck2(run_date.tm_year,run_date.tm_mon,run_date.tm_mday,run_date.tm_hour,run_date.tm_min);
            if(SerialNumber<999){
                /*lcd.blank(0);
                lcd.blank(1);
                lcd.locate(0,1);
                lcd.printf(SAVEEROK[1]);
                se.OK();    //警告音
                //wait(3);
                lcd.blank(0);
                lcd.blank(1);*/
            }
        }

        //準最適値を行うヘッド数は、HEAD数より
        //大きい値はありえない(エラー対策)
        if(bscData.startKinji > product.head) {
            bscData.startKinji = product.head;
        }
        tm *dated4 = localtime(&seconds);
        if(product.limit_on==NUMBER_LIMIT) {
            set_count_force(&cnt);
        }
        meas_rap.attach( &rap_count, 0.1);
        //meas_rap2.attach( &measure_1head, 0.15);
        //testmode3=true;
        while(1) {


            //se.chime_01();
            
            se.Touch_01();
            
            if(iro_stop_flg == false){
                
                if(matrix_loop < (matrix_loop_limit / 2)){
                    option.c_rotate = 0;
                }else{
                    option.c_rotate = 1;
                }
                matrix_loop++;
                
                if(matrix_loop >= matrix_loop_limit){
                    matrix_loop = 0;
                    now_iro_no++;
                }
            }
            if(btn.down_press() == true){
                se.Touch_02();    
                if(iro_stop_flg == true){
                    iro_stop_flg = false;
                }else{
                    iro_stop_flg = true;
                }
                while(btn.down_press() == true){    wait(0.2);}
            }
            if(btn.up_press() == true){
                se.Touch_02();
                matrix_loop = 0;
                now_iro_no = now_iro_no - 2;
                if(now_iro_no < 0){now_iro_no = 0;}
                //while(btn.up_press() == true){      wait(0.2);}
            }
            int iro_s_rank  = C_S;
            int iro_e_rank  = C_MA;
            int iro_s_CR_xx = CR_00;
            int iro_e_CR_xx = CR_99;
            int iro_s_CG_xx = CG_01;
            int iro_e_CG_xx = CG_99;

            int iro_single_e = 10;
            int iro_rank_end = iro_single_e + (iro_e_rank  - iro_s_rank)+1;
            int iro_r_00_end = iro_rank_end + (iro_e_CR_xx - iro_s_CR_xx)+1;
            int iro_g_00_end = iro_r_00_end + (iro_e_CG_xx - iro_s_CG_xx)+1;

            //単独マトリクス表示
            if(now_iro_no <= iro_single_e){
                if(now_iro_no == 1){
                    now_iro_set = ALL_G;
                }else if(now_iro_no == 2){
                    now_iro_set = ALL_R;
                }else if(now_iro_no == 3){
                    now_iro_set = ALL_Y;
                }else if(now_iro_no == 4){
                    now_iro_set = PENTA_G;
                }else if(now_iro_no == 5){
                    now_iro_set = PENTA_R;
                }else if(now_iro_no == 6){
                    now_iro_set = PENTA_Y;
                }else if(now_iro_no == 7){
                    now_iro_set = C_NUSE;
                }else if(now_iro_no == 8){
                    now_iro_set = C_BUI_G;
                }else if(now_iro_no == 9){
                    now_iro_set = C_BUI_R;
                }else if(now_iro_no == 10){
                    now_iro_set = C_BUI_Y;
                }
                matrix_loop_limit = 2;
            }
            //ランク
            if(iro_single_e < now_iro_no && now_iro_no <= iro_rank_end){
                now_iro_set = iro_s_rank + (now_iro_no - iro_single_e)-1;
                matrix_loop_limit = 2;
            }
            //数字(赤)
            if(iro_rank_end < now_iro_no && now_iro_no <= iro_r_00_end){
                now_iro_set = iro_s_CR_xx + (now_iro_no - iro_rank_end)-1;
                matrix_loop_limit = 2;
            }
            //数字(緑)
            if(iro_r_00_end < now_iro_no && now_iro_no <= iro_g_00_end){
                now_iro_set = iro_s_CG_xx + (now_iro_no - iro_r_00_end)-1;
                matrix_loop_limit = 2;
            }
            if(iro_g_00_end < now_iro_no){
                btn.force_ESC();
            }
            for(int i=0;i<product.head;i++){
                iro[i] = now_iro_set;
            }
            printf("%5d:%5d:%5d:%5d:%5d:%5d:%5d:%5d:%5d:%5d\r\n",matrix_loop,now_iro_no,now_iro_set  ,iro_single_e  ,iro_rank_end , iro_r_00_end , iro_g_00_end,C_9S,CR_00,CG_00);
            //for(int i=0;i<product.head;i++){
            //    printf("%3d:",iro[i]);
            //}
            //printf("\r\n");
            wait(0.1);




            //se.Touch_01();   //ボタン操作時の効果音
            //se.Touch_01();   //ボタン操作時の効果音
            if(testmode3==true){se.puter(6);}
            //se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();
            
                //printf("A:moving1 %5d  || moving2 %5d\r\n",moving1,moving2);
            if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            } else {
            }
            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            
            }else{
            }
            btn.new_keep_btn();         //スイッチ(New)取得
            if(product.limit_on==NUMBER_LIMIT) {
                if(kumi_en_check()==false) {
                    se.mistake();                   //警告音
                    v.over_limit();
                    btn.force_ESC();
                } else {
                    /*lcd.cls();
                    lcd.locate(0,1);
                    lcd.printf("%d",KUMI_limit-KUMI_now);
                    v.last_limit(KUMI_limit-KUMI_now);
                    KUMI_now =KUMI_now +1;
                    file.save_limit2(KUMI_now);*/
                }
            }
            if(option.waiting_limit > 0 && no_combi_rap > (option.waiting_limit * 60 * 10)){
                se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();
                btn.force_ESC();
                no_combi_rap = 0;
            }
            /*    //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
            if(btn.up_press() == true) {
                se.chime_01();
                while(btn.any_press()==true){
                    if(demo_flg==true&&btn.down_press() == true&&file.get_preset()==product.head*2){
                        demo_continue=2;
            printf("a%d  %d\r\n",file.get_preset(),demo_continue);
                        btn.force_ESC();
                        continue;
                    }
                    wait(0.1);
                }
                if(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2){
                        demo_continue=2;
            printf("b%d  %d\r\n",file.get_preset(),demo_continue);
                        btn.force_ESC();
                        continue;
                }
            printf("c%d  %d\r\n",file.get_preset(),demo_continue);
                //ゆっくりモード時には、
                if(load.spMode == SPEED_LOW) {
                    btnset = Chenge_vaule_btnD(option.btn_sett);//ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
                    if(btnset==0) {
                        set_count(&cnt);
                    } else if(btnset==1) {
                        auto_zero_off = true;
                        start_keiryo(0);
                        auto_zero_off = false;
                    }else if(btnset==2){
                        set_preset2(file.get_preset(),TL_SPMODE);
                    }else if(btnset==3){
                        set_preset2(file.get_preset(),TL_KUMI_FLAG);
                    }else if(btnset==4){
                        set_option_setting(OP_BEGINNER);
                    }else if(btnset==5){
                        set_option_setting(OP_KIKAKUGAI_V);
                    }else if(btnset==6){
                        set_option_setting(OP_KOKUTI);
                    }else if(btnset==7){
                        set_option_setting(OP_SEL_BEEP);
                    }else if(btnset==8){
                        set_sercret_parameter_one(P_buffer,1); //保存指令付
                    }
                } else if(load.spMode == SPEED_HIGH || load.spMode == SPEED_HYPER) {
                    btnset = Chenge_vaule_btnD(0);
                    if(btnset==0) {
                        set_preset2(file.get_preset(),TL_HYOUJI);
                    } else if(btnset==1) {
                        auto_zero_off = true;
                        start_keiryo(0);
                        auto_zero_off = false;
                    }else if(btnset==2){
                        set_preset2(file.get_preset(),TL_SPMODE);
                    }else if(btnset==3){
                        set_preset2(file.get_preset(),TL_KUMI_FLAG);
                    }else if(btnset==4){
                        set_sercret_parameter_one(P_buffer,1); //保存指令付
                    }
                }
                //1行目LCD表示
                if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                    lcd.locate(0,0);
                    lcd.printf(multiweight);
                    lcd.blank(LOW);
                    lcd.printf(":----.-g");
                } else {
                    lcd.settings(&load);      //設定内容を表示
                }
            }
            printf("%d  %d\r\n",file.get_preset(),demo_continue);*/
            //esc_swを押すと、Menu画面に戻る
            if(btn.esc_press()||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)) {
                meas_rap.detach();
                meas_rap2.detach();
                btn.end_force();
                se.Touch_01();
                //total_kumi_cnt=10;
                //if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT || load.kumi_flag == KUMI_COMBI )&& total_kumi_cnt>0) {
                
                if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                    for(int l=0;l<product.head;l++){
                        iro[l]=C_NULL;
                    }
                }
                if(servo_connection==true && load.spMode == SPEED_DAMP){
                    se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();
                    for(int header=0; header<product.head; header++){
                        send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
                    }
                    wait(0.5);
                    for(int header=0; header<product.head; header++){
                        send_i(SERVO_1 ,header,servo_span[header][0]); wait_res();
                    }

                }
                
                if(servo_connection==true && load.spMode == SPEED_DAMP){
                    send_i(EJECT_RESET, 0, 0);
                    wait_signal();
                }
                if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT)&& total_kumi_cnt>0) {
                    seconds = time(NULL);
                    tm *dated4 = localtime(&seconds);
                    //printf("en%d:%d:%d\r\n",dated4->tm_hour,dated4->tm_min,dated4->tm_sec);
                    int ji=0;
                    int hun=0;
                    ji = dated4->tm_hour - run_date.tm_hour;
                    if(ji<0) {
                        ji += 24;
                    }
                    hun = dated4->tm_min - run_date.tm_min;
                    if(hun<0) {
                        hun += 60;
                        ji -= 1;
                    }
                    lcd.blank(1);
                    lcd.blank(0);
                    lcd.locate(0,1);
                    lcd.printf("                ");
                    lcd.locate(0,1);
                    if(product.use_usb==1 && option.usb_EN==true) {
                        lcd.printf(saving[0]);
                    }
                    if(product.use_usb==1 && option.usb_EN==true) {
                        file.save_SAGYOU(ji,hun,file.get_preset()+1,load.target,total_kumi_cnt,average_kumi_value);
                    }        
                        
                        
                    
                    printf("total:%4d\r",total_kumi_cnt);
                    printf("ave  :%6.1f\r",average_kumi_value);
                    printf("time:%02dh%02dm\r",ji,hun);
                    if((ji*60)+hun==0){
                        printf("1min:%03d\r",total_kumi_cnt);
                    }else{
                        printf("1min:%03d\r",int(total_kumi_cnt/(ji*60)+hun));
                    }
                    printf("err :%4d(%4.1f%%)\r",cnt_miss_and_kikakugai,float(cnt_miss_and_kikakugai)/float(total_kumi_cnt));
                    printf("jst :%4d(%4.1f%%)\r",cnt_junsaiteki,float(cnt_junsaiteki)/float(total_kumi_cnt));
                    printf("\r\n");
                    //printf("err :%4d%s%04.1f%s%s\r",cnt_miss_and_kikakugai,0x28,float(cnt_miss_and_kikakugai)/float(total_kumi_cnt),0x25,0x29);
                    //printf("jst :%4d%s%04.1f%s%s\r",cnt_junsaiteki,0x28,float(cnt_junsaiteki)/float(total_kumi_cnt),0x25,0x29);
                    int num=0;
                    printf("%02dH:%5d:%6d\r",num*2+1,servo_n_histry[0][num*2  ],servo_b_histry[num*2  ] + servo_n_histry[0][num*2  ]);
                    printf("%02dH:%5d:%6d\r",num*2+2,servo_n_histry[0][num*2+1],servo_b_histry[num*2+1] + servo_n_histry[0][num*2+1]);
                    num=1;
                    printf("%02dH:%5d:%6d\r",num*2+1,servo_n_histry[0][num*2  ],servo_b_histry[num*2  ] + servo_n_histry[0][num*2  ]);
                    printf("%02dH:%5d:%6d\r",num*2+2,servo_n_histry[0][num*2+1],servo_b_histry[num*2+1] + servo_n_histry[0][num*2+1]);
                    num=2;
                    printf("%02dH:%5d:%6d\r",num*2+1,servo_n_histry[0][num*2  ],servo_b_histry[num*2  ] + servo_n_histry[0][num*2  ]);
                    printf("%02dH:%5d:%6d\r",num*2+2,servo_n_histry[0][num*2+1],servo_b_histry[num*2+1] + servo_n_histry[0][num*2+1]);
                    
                    for(int a=0 ; a < product.head ; a++){                                
                        if( servo_n_histry[1][a] == 1){
                            servo_n_histry[0][a] += 1;
                            servo_n_histry[1][a] =  0;
                        }
                    }
                    servo_histry += total_kumi_cnt;
                    
                    file.save_hservo();
                    if(option.kekka_hyouji == 1){
                        result_LCD(ji,hun);
                        result_LCD_servo(ji,hun);
                    }
                    
                    
                    for(int l=0; l<product.head; l++) {
                        servo_b_histry[l] = servo_b_histry[l] + servo_n_histry[0][l] + servo_n_histry[1][l];
                        servo_n_histry[0][l] = 0;
                        servo_n_histry[1][l] = 0;
                    }
                    servo_h_time[1] += hun;
                    
                    if(servo_h_time[1]>=60){
                        servo_h_time[1] -= 60;
                        servo_h_time[0] +=  1;
                    }
                    servo_h_time[0] += ji;
                    
                    
                    lcd.blank(1);
                    lcd.blank(0);
                    lcd.locate(0,1);
                    lcd.printf("                ");
                    lcd.printf(saving[1]);
                    //file.save_hservo();
                    
                    //save_sagyou_file(total_kumi_cnt,average_kumi_value,load.target,load.jougen);
                    if(product.use_usb==1 && option.usb_EN==true) {
                        //save_sagyou_file2(total_kumi_cnt,average_kumi_value,load.target,load.jougen,ji,hun,SerialNumber);
                        save_sagyou_file2_date(total_kumi_cnt,average_kumi_value,load.target,load.jougen,ji,hun,SerialNumber,run_date.tm_year,run_date.tm_mon,run_date.tm_mday,run_date.tm_hour,run_date.tm_min);
                    }
                    //save_sagyou_file3(total_kumi_cnt,average_kumi_value,load.target,load.jougen,ji,hun,SerialNumber);
                    if(product.use_usb==1 && option.usb_EN==true) {
                        SD_initialise();
                    }
                    if(product.use_usb==1 && option.usb_EN==true) {
                        lcd.blank(1);
                        lcd.blank(0);
                        lcd.locate(0,1);
                        lcd.printf("                ");
                        lcd.printf(saving[1]);
                    }
                }
                delete[] stay_cnt;
                total_kumi_cnt = 0;
                average_kumi_value = 0;
                btn.old_btn();       //スイッチ更新
                btn.new_btn();       //スイッチ更新
                wait(0.2);
                if(servo_connection==true && load.spMode == SPEED_DAMP){
                    send_i(KEIRYO_EN,0,0);wait(0.2);
                    //moving2=99;
                        
                    send_i(EJECT_RESET, 0, 0);
                    wait_signal();
                }
                return;
            }


            //ent_swを押すと、ゼロ点調整（風袋引き）する
            if(btn.get_btn() == BTN_ENT) {
                now_TC_mode = 3;
                for(int header=0; header<product.head; header++){
                    send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
                }
                lcd.messege(TOP,DIS_WAIT,CLS);
                lcd.messege(LOW,DIS_ZERO);
                wait(1);
                //auto_zero_point();                  //ゼロ点取得
                for(int header=0; header<product.head; header++){
                    send_i(SERVO_1 ,header,servo_span[header][0]); wait_res();
                }
                wait(1);
                auto_zero_point();                  //ゼロ点取得
                
                now_TC_mode = 1;
                //1行目LCD表示
                if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                    lcd.locate(0,0);
                    lcd.printf(multiweight);
                } else {
                    lcd.settings(&load);      //設定内容を表示
                }
                no_combi_rap = 0;
                
            }

            btn.old_btn();       //スイッチ更新
            
            if(testmode3==true){se.puter(5);}

            if(servo_connection==true && load.spMode == SPEED_DAMP){
                send_i(ON_CHECK,0,close_wait_flg);
                wait_signal();
                /*for(int a = 0; a < product.head; a++) {
                    if(((movingb1 & (1<<a)) > 0) && ((movingb1 & (1<<a)) == 0)){
                        cel.break_antei(a);
                        antei_flug[a]=0;
                    }
                }*/
            }else{
                moving1=0;
                moving2=0;
            }
            
            if(testmode3==true){se.puter(5);}
            //se.puter(5);
            for(int i = 0; i < product.head; i++) {
                //if(antei_flug[i] == 0){
                    setHeadVolt(i, KURI2nd);
                //}else{
                //    setHeadVolt(i, param.KURI_Y);
                //}
            }
            //se.puter(1);
            if(testmode3==true){se.puter(5);}
            bool error_atonose = false;
            for(int i = 0; i < product.head; i++) {
                if(cel.anteiWeight(i) > param.CUT){
                    antei_flug[i] = 1;
                }else{
                    antei_flug[i] = 0.0;
                }
                if((moving1 & (1<<i)) > 0 && befour_anteiw[i] > 0){
                    //if (((befour_anteiw[i] - cel.anteiWeight(i)) <-5) || befour_anteiw[i] - cel.anteiWeight(i) > 5){
                    if (cel.getWeight(i) > param.CUT && (((befour_anteiw[i] - cel.getWeight(i)) < -param.MINI_target) || ((befour_anteiw[i] - cel.getWeight(i)) > param.MINI_target))){
                        
                        if(option.reck_ato == true) {
                            iro[i] = C_R_EXCLA;
                            error_atonose = true;
                        }
                    }
                }
            }
            if(testmode3==true){se.puter(5);}
            if(error_atonose == true){
                se.mistake3(6);
                error_atonose = false;
            }

            bool force_zero_flg = false;
            int force_zero_sec = 30;
            /**/
            for(int i = 0; i < product.head; i++) {
                if(no_zero_counter[i] > (force_zero_sec * 10)){
                    if(force_zero_flg == false){
                        force_zero_flg = true;
                        close_wait_flg = close_wait_flg | (1 << i);
                    }else{
                        no_zero_counter[i] = ((force_zero_sec - 20) * 10);
                    }
                }
            }
            /**/
            /*
            //if(no_zero_counter_zero == 0){
                for(int i = 0; i < product.head; i++) {
                    //pc.printf(" %02dH: %5d  ",i+1,no_zero_counter[i]);
                    if((i+1) %3 == 0){
                        pc.printf("\r\n");
                    }
                    if(i == product.head-1){
                    // pc.printf("flg: %5d\r\n",close_wait_flg);
                    }
                }
            //}
            */
            /*
            for(int i = 0; i < product.head; i++) {
                if(antei_flug[i] == 0){
                    setHeadVolt(i, param.KURI);
                }else{
                    setHeadVolt(i, param.KURI_Y);
                }
                if(cel.anteiWeight(i) > param.CUT){
                    antei_flug[i] = 1;
                }else{
                    antei_flug[i] = 0;
                }
            }//for
            */
            //デバッグ用ここから
            
            if(print_flg == true){  
                for(int i = 0; i < product.head; i++) {
                    if(i==product.head-1){
                        cel.set_head(i);
                        //send_data(i+1,cel.getWeight(i),cel.get_HWeight(i),1);
                        send_data(i+1,cel.getWeight(i),cel.getAin(),1);
                    }else{
                        cel.set_head(i);
                        //send_data(i+1,cel.getWeight(i),cel.get_HWeight(i),0);
                        send_data(i+1,cel.getWeight(i),cel.getAin(),0);
                    }
                }
            }
            
            if( product.productType >= STANDARD_D) {
                for(int i = 0; i < product.head; i++) {
                    /*
                    cel.set_head(i);
                    pc.printf("%02d: %6.3f %6.3f %6.3f   %0.8f\r\n",
                    i+1,cel.getWeight(i),cel.anteiWeight(i), cel.get_HWeight(i),cel.getAin());
                    if(i == product.head - 1) {
                        //printf("%d %f C\r\n",14, temp);
                        pc.printf("========================\r\n");
                        pc.printf("========================\r\n");
                        pc.printf("========================\r\n");
                    }
                    */
                    
                    cel.set_head(i);
                    pc.printf("%02d: %4d %4d\r\n",
                    i+1,int(cel.getWeight(i)),int(cel.anteiWeight(i)));
                    if(i == product.head - 1) {
                        //printf("%d %f C\r\n",14, temp);
                        pc.printf("========================\r\n");
                    }
                }
                /*for(int l=0;l<product.head;l++){
                    if(int(cel.anteiWeight(l)) > 0){
                        v.speak_numbers(l+1);
                        v.gram_voice(int(cel.anteiWeight(l)));
                        wait(0.5);
                    }
                }*/
            }
            //se.Touch_01();se.Touch_02();se.Touch_01();se.Touch_02();se.Touch_01();se.Touch_02();se.Touch_01();se.Touch_02();
            
                //printf("B:moving1 %5d  || moving2 %5d\r\n",moving1,moving2);
            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            }else{
            }
            
            if(testmode3==true){se.puter(4);}
            //デバッグ用ここまで
            
            //第2上限値適用するか否か
            /*
            if(option.second_target == 1){
                antei_checker=0;
                for(int i = 0; i < product.head; i++) {
                    if(cel.anteiWeight(i)>0){
                        antei_checker = antei_checker | (1 << i);
                    }
                }
                if( load.kumi_flag <= KUMI_LIMIT && scond_target >= 0 && (bit_count(antei_checker) >= (bscData.startKinji-bit_count(stop_cells)))){
                    load.target = scond_target;
                    load.jougen = scond_jougen;
                }else{
                    load.target = first_target;
                    load.jougen = first_jougen;
                }
            }//第2上限値ここまで
            */
            
            //1行目LCD表示
            if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                lcd.locate(0,0);
                lcd.printf(multiweight);
                lcd.blank(LOW);
                lcd.printf(":----.-g       ");
            } else {
                lcd.settings(&load);      //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
            }

            if(load.kumi_flag == KUMI_KAIKYU) {
                //if(product.productType == MATRIX || product.productType == MATRIX_D || product.productType == ITIGO || product.productType == ITIGO_D){
            } else {
                //重量確認(適正でない重量があるため処理を飛ばす)
                //if(check_max_weight(cel.over_weight()) == false) {
                //    continue;
                //}
            }
            
            if(testmode3==true){se.puter(3);}
            if(servo_connection==true && load.spMode == SPEED_DAMP){
                send_i(ON_CHECK,0,close_wait_flg);
                wait_signal();
            }
            if(check_same_count > 50){
                se.mistake3(2);
                btn.force_ESC();
                continue;
            }




    //////////////////////皿を動かすか確認     
            //if(moving2==0 || servo_connection==false){
            total_combi_count = 0;
            //if(1){
                
            for(int k=0;k<3;k++){
                orderCombi[k].kumi     = 0;
                orderCombi[k].total    = 0;
                orderCombi[k].priolity = 0;
            }
            if(combi_chk==0){
                combi_chk=1;
            }else {
                combi_chk=0;
            }
            if(load.kumi_flag == KUMI_KAIKYU) {
                //if(product.productType == MATRIX || product.productType == MATRIX_D || product.productType == ITIGO || product.productType == ITIGO_D){
                get_combi_location_multi(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
            } else {
                for(int j=0;j<5;j++){
                    Confilcombi[j].kumi     = 0;
                    Confilcombi[j].total    = 0;
                    Confilcombi[j].priolity = 0;
                }
                //if(testmode1 == false){
                //if(testmode1 == false && load.spMode == SPEED_DAMP){
                    get_combi_location_AUTO(orderCombi, &secondCnt,0);     //組み合わせ確認,組み合わせ候補を抽出
                    //get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
                //}else{
                    //get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
                    //get_combi_location_AUTO(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
                //}
            }
            //resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得


            //if(gLevel == 1 && orderCombi[0].total > 0 && orderCombi[0].total<10000){
            //    v.speak_numbers(int(orderCombi[0].total));
            //}



            if(testmode3==true){se.puter(2);}
            //printf("A3\r\n");
            
            //振動検知機能がONだったら
            if(param.vivration >= 1) {
                //繰り返し確認する
                for(int i = 0; i < BK_CNT; i++) {
                    //組み合わせ候補が安定しているか
                    if(checkWeightTaget(&orderCombi[i]) == true) {
                        resultCombi = orderCombi[i];
                        break;
                    }
                }
            }
            total_combi_count = 0;
            Confilcombi[0] = resultCombi;                    //最小組み合わせ結果を取得
            //printf("%2d | %4d | to %4d\r\n",total_combi_count , Confilcombi[0].kumi , int(Confilcombi[0].total));
            //
            if(Confilcombi[0].kumi > 0 && 
                Confilcombi[0].total >= (load.target) && 
                Confilcombi[0].total <= (load.target + load.jougen) && 
                load.kumi_flag <= KUMI_LIMIT && load.spMode == SPEED_DAMP && option.btn_sett != 0){
                for(int j = 1;j < 5; j++){
                    //Confilcombi[j].kumi     = 0;
                    //Confilcombi[j].total    = 0;
                    //Confilcombi[j].priolity = 0;
                    for(int k=0;k<3;k++){
                        orderCombi[k].kumi     = 0;
                        orderCombi[k].total    = 0;
                        orderCombi[k].priolity = 0;
                    }
                    get_combi_location_AUTO(orderCombi, &secondCnt, Confilcombi[0].kumi | 
                                                                    Confilcombi[1].kumi | 
                                                                    Confilcombi[2].kumi | 
                                                                    Confilcombi[3].kumi | 
                                                                    Confilcombi[4].kumi  );     //組み合わせ確認,組み合わせ候補を抽出
                    resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得
                    if(param.vivration >= 1) {
                        //繰り返し確認する
                        for(int i = 0; i < BK_CNT; i++) {
                            //組み合わせ候補が安定しているか
                            if(checkWeightTaget(&orderCombi[i]) == true) {
                                resultCombi = orderCombi[i];
                                break;
                            }
                        }
                    }
                    Confilcombi[j] = resultCombi;                    //最小組み合わせ結果を取得
                    printf("%2d | %4d | to %4d\r\n", j , Confilcombi[j].kumi , int(Confilcombi[j].total));
                    if(Confilcombi[j].kumi > 0 && 
                        Confilcombi[j].total >= (load.target) && 
                        Confilcombi[j].total <= (load.target + load.jougen)){
                    }else{
                        total_combi_count = j;
                        j = 6;
                    }
                }
            }
            if(load.kumi_flag <= KUMI_LIMIT){
                resultCombi   = Confilcombi[0];
                orderCombi[1] = Confilcombi[1];
                orderCombi[2] = Confilcombi[2];
            }
            //1行目LCD表示
            if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                lcd.locate(0,0);
                lcd.printf(multiweight);
                lcd.blank(LOW);
                lcd.printf(":----.-g       ");
            } else {
                lcd.settings(&load);      //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
                if((product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)&& resultCombi.kumi > 0) {
                    //for(int l=0;l<product.head;l++){
                    //    iro[l]=C_NULL;
                    //}
                }
            }
            int inChk=1;
            if(load.kumi_flag == KUMI_KAIKYU) {
                for(int k=0; k < product.head; k++) {
                    if((inChk & resultCombi.kumi) > 0) {
                        oldweightVale[k] = cel.getWeight(k);
                        if(oldweightVale[k] > param.CUT && oldweightVale[k] < mweight.u02.rank_min - 1) {
                            load.target = mweight.u01.target;
                            load.jougen = mweight.u01.jougen;
                            load.kumi_min = mweight.u01.kumi_min;
                            load.kumi_max = mweight.u01.kumi_max;
                        } else if(oldweightVale[k] > mweight.u02.rank_min && oldweightVale[k] < mweight.u02.rank_max) {
                            load.target = mweight.u02.target;
                            load.jougen = mweight.u02.jougen;
                            load.kumi_min = mweight.u02.kumi_min;
                            load.kumi_max = mweight.u02.kumi_max;
                        } else if(oldweightVale[k] > mweight.u03.rank_min && oldweightVale[k] < mweight.u03.rank_max) {
                            load.target = mweight.u03.target;
                            load.jougen = mweight.u03.jougen;
                            load.kumi_min = mweight.u03.kumi_min;
                            load.kumi_max = mweight.u03.kumi_max;
                        } else if(oldweightVale[k] > mweight.u04.rank_min && oldweightVale[k] < mweight.u04.rank_max) {
                            load.target = mweight.u04.target;
                            load.jougen = mweight.u04.jougen;
                            load.kumi_min = mweight.u04.kumi_min;
                            load.kumi_max = mweight.u04.kumi_max;
                        } else if(oldweightVale[k] > mweight.u04.rank_max+1) {
                            load.target = mweight.u05.target;
                            load.jougen = mweight.u05.jougen;
                            load.kumi_min = mweight.u05.kumi_min;
                            load.kumi_max = mweight.u05.kumi_max;
                        }
                        res = load.jougen;    //組み合わせ重量の上限値を取得
                        load.jougen -= load.target;   //制限ｱﾘ等と同じ計算式に合わせるための計算
                    }
                    inChk = inChk <<1;
                }
            }
            
            if(testmode3==true){se.puter(1);}
            //printf("A4\r\n");
                    //printf("b0\r\n");
        /*
            if(cel.anteiWeight(0) == 0 || resultCombi.kumi == 0 ){
                se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();se.Touch_02();
            }
    */

            for(int j=0;j<5;j++){
                if(Confilcombi[j].kumi > 0 && 
                Confilcombi[j].total >= (load.target) && 
                Confilcombi[j].total <= (load.target + load.jougen)){
                }else{
                    Confilcombi[j].kumi     = 0;
                    Confilcombi[j].total    = 0;
                    Confilcombi[j].priolity = 0;
                }
            }

            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            }else{
                if(moving1 == 0){
                    led.ledAllOff();            // LED消灯
                }else{
                    led.REGIS(moving1);
                }
            }
            show_count(&cnt);               //カウント表示
            if(testmode3==true){se.puter(0);}
            //printf("A5\r\n");
                    // printf("b1\r\n");
                    //printf("%d/%d :%d/%d :lp %d\r\n",load.kumi_flag,KUMI_RANK,load.spMode,SPEED_HYPER,junsaiteki_loop);
    //*********準最適値だった場合、もう一度組み合わせてみる**************************
            // if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT)  && 
            //     ((load.spMode == SPEED_HYPER) || (load.spMode == SPEED_LOW  ))&&
            //     junsaiteki_loop == 2){
            //     
            //     get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
            //     resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得
            //     if(int(resultCombi.total) > res){
            //     //*********再度準最適値だった場合は無視
            //     }else{
            //         lcd.settings(&load);      //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
            //         show_results_auto(&resultCombi,&orderCombi[1],&orderCombi[2]);     //組み合わせに応じたLED点灯,総重量表示
            //         show_count(&cnt);               //カウント表示
            //     }
            // }
    //*********準最適値だった場合**************************
            //printf("%7.2f\r\n", float(load.target) + (float(bit_count(resultCombi.kumi))*param.buffer2));

            if(load.spMode == SPEED_HIGH || load.spMode == SPEED_HYPER || load.spMode == SPEED_DAMP) {
                announceRetention(resultCombi.kumi, stay_cnt);   //滞留アナウンス
            }
            //if(resultCombi.kumi > 0 ){
            //    printf("%8.1f \r\n",resultCombi.total);
            // }
            
            //printf("A6\r\n");
            /*
            //ゆっくりモードかつ組み合わせ成立していれば
            if( (load.spMode == SPEED_LOW && resultCombi.kumi > 0 && int(resultCombi.total) <= res) //||
                    ){ 
                
                //printf("AA\r\n");
                if(option.kokuti==1||option.kokuti==2||option.kokuti==3) {
                    se.Touch_02();
                }
                //se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();
                driveSlowly(&resultCombi);
                //driveSlowly(&resultCombi,&cnt);
                comp_kumi=0;
                //se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();se.Touch_01();
                if(product.limit_on==NUMBER_LIMIT && resultCombi.kumi == 0) {
                    KUMI_now =KUMI_now +1;
                    file.save_limit2(KUMI_now);
                    if(KUMI_limit-KUMI_now>0 && KUMI_limit-KUMI_now < 10) {
                        v.last_limit(KUMI_limit-KUMI_now);
                    }
                }

                //組み合わせ分を取り終わっていたら
                if(resultCombi.kumi == 0 && cnt.max > 0) {
                    if(cnt.num < cnt.max) {
                        cnt.num++;
                        //残りカウント数に合わせて音声お知らせ
                        v.output_audio_Count(cnt.max,cnt.num);
                    }
                }//if
            }//ifゆっくりモード終了
            */
            junsaiteki_loop = 0;
            //ﾗﾝｸﾍﾞﾂ組合せのみ1行目LCD表示
            if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                lcd.locate(0,0);
                lcd.printf(multiweight);
                lcd.blank(LOW);
                lcd.printf(":----.-g       ");
            }
            //se.Touch_01();se.Touch_01();se.Touch_01();
            if(servo_connection==true && load.spMode == SPEED_DAMP){
                send_i(ON_CHECK,0,close_wait_flg);
                wait_signal();
            }

            
            //printf("C:moving1 %5d  || moving2 %5d\r\n",moving1,moving2);
            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {

            }else{
                if(moving1 == 0){
                    led.ledAllOff();            // LED消灯
                }else{
                    led.REGIS(moving1);
                }
            }
            //}
            //printf("A1end\r\n");
        }//while


        for(int k=0;k<3;k++){
            for(int i=0;i<product.head;i++){
                iro[i] = ALL_G + k;
                if(btn.down_press() == true){
                    se.Touch_01();
                    wait_lp=1;
                }
                wait(0.2);
            }
            for(int j=0;j<20;j++){
                wait(0.1);
                if(btn.down_press() == true || wait_lp==1){
                    if(btn.down_press() == true){
                        se.Touch_02();
                    }
                    while(btn.down_press() == true){    wait(0.2);  }
                    while(btn.down_press() == false){
                        if(btn.up_press() == true){
                            se.Touch_02();
                            k -= 1;
                            if(k < 0){k = 0;}
                            for(int i=0;i<product.head;i++){
                                iro[i] = ALL_G + k;
                            }
                            while(btn.up_press() == true){   wait(0.1);  }
                        }
                        wait(0.1);  
                    }
                    se.Touch_02();
                    wait(0.7);
                    j=20;
                    wait_lp=0;
                }
            }
        }
    }else{
        
        while(ck_end_f==0){
            option.c_rotate = 0;
            for(int i=0;i<product.head;i++){
                iro[i] = iro_ck + ck_lp;
            }
            se.Touch_01();
            wait(0.5);
            wait_lp=0;
            while( (btn.down_press() == true || wait_lp<10) && btn.up_press() == false && btn.esc_press() == false) {
                if(btn.down_press() == true){
                    wait(0.2);
                    se.Touch_02();
                    while(btn.down_press() == true){  wait(0.2);  }
                    while(btn.down_press() == false){
                        if(btn.up_press() == true){
                            se.Touch_02();
                            ck_lp -= 1;
                            if(ck_lp < 0){ck_lp = 0;}
                            for(int i=0;i<product.head;i++){
                                iro[i] = iro_ck + ck_lp;
                            }
                            while(btn.up_press() == true){   wait(0.1);  }
                        }
                        wait(0.1);  
                    }
                    se.Touch_02();
                }
                while(btn.down_press() == true){    wait(0.2);  }
                wait_lp++;
                wait(0.1);
            }
            while(btn.down_press() == true){    wait(0.2);  }
            //while(btn.down_press() == false){
            //    wait(0.1);
            //}
            if(btn.up_press() == false && btn.esc_press() == false){
                option.c_rotate = 1;
                se.Touch_01();
                wait_lp=0;
                while( (btn.down_press() == true || wait_lp<15) && btn.up_press() == false && btn.esc_press() == false) {
                    if(btn.down_press() == true){
                        wait(0.2);
                        se.Touch_02();
                        while(btn.down_press() == true){  wait(0.2);  }
                        while(btn.down_press() == false){
                            if(btn.up_press() == true){
                                se.Touch_02();
                                ck_lp -= 1;
                                if(ck_lp < 0){ck_lp = 0;}
                                for(int i=0;i<product.head;i++){
                                    iro[i] = iro_ck + ck_lp;
                                }
                                while(btn.up_press() == true){   wait(0.1);  }
                            }
                            wait(0.1);  
                        }
                        se.Touch_02();
                    }
                    while(btn.down_press() == true){    wait(0.2);  }
                    wait_lp++;
                    wait(0.1);
                }
                while(btn.down_press() == true){    wait(0.2);  }
                if(product.Array == 0 && btn.up_press() == false && btn.esc_press() == false){
                    option.c_rotate = 2;
                    se.Touch_01();
                    wait_lp=0;
                    while( (btn.down_press() == true || wait_lp<15) && btn.up_press() == false && btn.esc_press() == false) {
                        if(btn.down_press() == true){
                            wait(0.2);
                            se.Touch_02();
                            while(btn.down_press() == true){  wait(0.2);  }
                            while(btn.down_press() == false){
                                if(btn.up_press() == true){
                                    se.Touch_02();
                                    ck_lp -= 1;
                                    if(ck_lp < 0){ck_lp = 0;}
                                    for(int i=0;i<product.head;i++){
                                        iro[i] = iro_ck + ck_lp;
                                    }
                                    while(btn.up_press() == true){   wait(0.1);  }
                                }
                                wait(0.1);  
                            }
                            se.Touch_02();
                        }

                        while(btn.down_press() == true){    wait(0.2);  }
                        wait_lp++;
                        wait(0.1);
                    }
                    while(btn.down_press() == true){    wait(0.2);  }
                }
            }
            if(btn.esc_press() == true){
                se.Touch_01();
                ck_all_end_f = 1;
                ck_lp=CR_01-iro_ck-1;
                file.read_option();
                option = file.get_option();
                for(int i=0;i<product.head;i++){
                    iro[i] = C_NULL;
                }
                return;
            }
            /*
            switch(iro_ck + ck_lp+1){
                case C_MA+1:
                    iro_ck =C_NULL; 
                    ck_end_f=1;
                    ck_lp += 1;
                    break;
                default:ck_lp += 1; break;
            }*/
            if((iro_ck + ck_lp + 1) > C_MA){
                iro_ck =C_NULL; 
                ck_end_f=1;
            }else{
                ck_lp += 1;
            }
            
        }
        iro_ck =CG_01;        //マトリクス表示内容
        ck_lp =0;           //繰返し回数
        ck_end_f=0;
        while(ck_end_f==0&&ck_all_end_f==0){
            option.c_rotate = 0;
            for(int i=0;i<product.head;i++){
                iro[i] = iro_ck + ck_lp;
            }
            se.Touch_01();
            wait(0.5);
            wait_lp=0;
            while( (btn.down_press() == true || wait_lp<10) && btn.up_press() == false && btn.esc_press() == false) {
                if(btn.down_press() == true){
                    se.Touch_02();
                    wait(0.2);
                    while(btn.down_press() == true){  wait(0.2);  }
                    while(btn.down_press() == false){
                        if(btn.up_press() == true){
                            se.Touch_02();
                            ck_lp -= 1;
                            if(ck_lp < 0){ck_lp = 0;}
                            for(int i=0;i<product.head;i++){
                                iro[i] = iro_ck + ck_lp;
                            }
                            while(btn.up_press() == true){   wait(0.1);  }
                        }
                        wait(0.1);  
                    }
                    se.Touch_02();
                }
                while(btn.down_press() == true){    wait(0.2);  }
                wait_lp++;
                wait(0.1);
            }
            while(btn.down_press() == true){    wait(0.2);  }
            //while(btn.down_press() == false){
            //    wait(0.1);
            //}
            if(btn.up_press() == false && btn.esc_press() == false){
                option.c_rotate = 1;
                se.Touch_01();
                wait_lp=0;
                while( (btn.down_press() == true || wait_lp<15) && btn.up_press() == false && btn.esc_press() == false) {
                    if(btn.down_press() == true){
                        se.Touch_02();
                        wait(0.2);
                        while(btn.down_press() == true){  wait(0.2);  }
                        while(btn.down_press() == false){
                            if(btn.up_press() == true){
                                se.Touch_02();
                                ck_lp -= 1;
                                if(ck_lp < 0){ck_lp = 0;}
                                for(int i=0;i<product.head;i++){
                                    iro[i] = iro_ck + ck_lp;
                                }
                                while(btn.up_press() == true){   wait(0.1);  }
                            }
                            wait(0.1);  
                        }
                        se.Touch_02();
                    }
                    while(btn.down_press() == true){    wait(0.2);  }
                    wait_lp++;
                    wait(0.1);
                }
                while(btn.down_press() == true){    wait(0.2);  }
                if(product.Array == 0 && btn.up_press() == false && btn.esc_press() == false){
                    option.c_rotate = 2;
                    se.Touch_01();
                    wait_lp=0;
                    while( (btn.down_press() == true || wait_lp<15) && btn.up_press() == false && btn.esc_press() == false) {
                        if(btn.down_press() == true){
                            se.Touch_02();
                            wait(0.2);
                            while(btn.down_press() == true){  wait(0.2);  }
                            while(btn.down_press() == false){
                                if(btn.up_press() == true){
                                    se.Touch_02();
                                    ck_lp -= 1;
                                    if(ck_lp < 0){ck_lp = 0;}
                                    for(int i=0;i<product.head;i++){
                                        iro[i] = iro_ck + ck_lp;
                                    }
                                    while(btn.up_press() == true){   wait(0.1);  }
                                }
                                wait(0.1);  
                            }
                            se.Touch_02();
                        }
                        while(btn.down_press() == true){    wait(0.2);  }
                        wait_lp++;
                        wait(0.1);
                    }
                    while(btn.down_press() == true){    wait(0.2);  }
                }
            }
            if(btn.esc_press() == true){
                se.Touch_01();
                ck_all_end_f = 1;
                ck_lp=CR_00-iro_ck;
                file.read_option();
                option = file.get_option();
                for(int i=0;i<product.head;i++){
                    iro[i] = C_NULL;
                }
                return;
            }
            /*switch(iro_ck + ck_lp){
                case CG_20:
                    iro_ck =C_NULL; 
                    ck_end_f=1;
                    ck_lp += 1;
                    break;
                default:ck_lp += 1; break;
            }*/
            if((iro_ck + ck_lp + 1) > CG_20){
                iro_ck =C_NULL; 
                ck_end_f=1;
            }else{
                ck_lp += 1;
            }
        }
        iro_ck =CG_21;        //マトリクス表示内容
        ck_lp =0;           //繰返し回数
        ck_end_f=0;
        while(ck_end_f==0&&ck_all_end_f==0){
            option.c_rotate = 0;
            for(int i=0;i<product.head;i++){
                iro[i] = iro_ck + ck_lp;
            }
            se.Touch_01();
            wait(0.05);
            wait_lp=0;
            while( (btn.down_press() == true || wait_lp<2) && btn.up_press() == false && btn.esc_press() == false) {
                if(btn.down_press() == true){
                    wait(0.2);
                    se.Touch_02();
                    while(btn.down_press() == true){  wait(0.2);  }
                    while(btn.down_press() == false){
                        if(btn.up_press() == true){
                            se.Touch_02();
                            ck_lp -= 1;
                            if(ck_lp < 0){ck_lp = 0;}
                            for(int i=0;i<product.head;i++){
                                iro[i] = iro_ck + ck_lp;
                            }
                            while(btn.up_press() == true){   wait(0.1);  }
                        }
                        wait(0.1);  
                    }
                    se.Touch_02();
                }
                while(btn.down_press() == true){    wait(0.2);  }
                wait_lp++;
                wait(0.1);
            }
            while(btn.down_press() == true){    wait(0.2);  }
            //while(btn.down_press() == false){
            //    wait(0.1);
            //}
            if(btn.up_press() == false && btn.esc_press() == false){
                option.c_rotate = 1;
                se.Touch_01();
                wait_lp=0;
                while( (btn.down_press() == true || wait_lp<2) && btn.up_press() == false && btn.esc_press() == false) {
                    if(btn.down_press() == true){
                        wait(0.2);
                        se.Touch_02();
                        while(btn.down_press() == true){  wait(0.2);  }
                        while(btn.down_press() == false){
                            if(btn.up_press() == true){
                                se.Touch_02();
                                ck_lp -= 1;
                                if(ck_lp < 0){ck_lp = 0;}
                                for(int i=0;i<product.head;i++){
                                    iro[i] = iro_ck + ck_lp;
                                }
                                while(btn.up_press() == true){   wait(0.1);  }
                            }
                            wait(0.1);  
                        }
                        se.Touch_02();
                    }
                    while(btn.down_press() == true){    wait(0.2);  }
                    wait_lp++;
                    wait(0.1);
                }
                while(btn.down_press() == true){    wait(0.2);  }
                if(product.Array == 0 && btn.up_press() == false && btn.esc_press() == false){
                    option.c_rotate = 2;
                    se.Touch_01();
                    wait_lp=0;
                    while( (btn.down_press() == true || wait_lp<2) && btn.up_press() == false && btn.esc_press() == false) {
                        if(btn.down_press() == true){
                            wait(0.2);
                            se.Touch_02();
                            while(btn.down_press() == true){  wait(0.2);  }
                            while(btn.down_press() == false){
                                if(btn.up_press() == true){
                                    se.Touch_02();
                                    ck_lp -= 1;
                                    if(ck_lp < 0){ck_lp = 0;}
                                    for(int i=0;i<product.head;i++){
                                        iro[i] = iro_ck + ck_lp;
                                    }
                                    while(btn.up_press() == true){   wait(0.1);  }
                                }
                                wait(0.1);  
                            }
                            se.Touch_02();
                        }
                        while(btn.down_press() == true){    wait(0.2);  }
                        wait_lp++;
                        wait(0.1);
                    }
                    while(btn.down_press() == true){    wait(0.2);  }
                }
            }
            if(btn.esc_press() == true){
                se.Touch_01();
                ck_all_end_f = 1;
                ck_lp=CR_00-iro_ck;
                file.read_option();
                option = file.get_option();
                for(int i=0;i<product.head;i++){
                    iro[i] = C_NULL;
                }
                return;
            }
            /*switch(iro_ck + ck_lp){
                case CG_99:
                    iro_ck =C_NULL; 
                    ck_end_f=1;
                    ck_lp += 1;
                    break;
                default:ck_lp += 1; break;
            }*/
            if((iro_ck + ck_lp + 1) > CG_99){
                iro_ck =C_NULL; 
                ck_end_f=1;
            }else{
                ck_lp += 1;
            }
        }
    }

    file.read_option();
    option = file.get_option();
    for(int i=0;i<product.head;i++){
        iro[i] = C_NULL;
    }
}

//#include "demo3.h"
int cel_led_bit(int i,int char_rank_col ,int char_antei_col ,int col){
    int cel_led =0;
    if((comp_kumi & oneBit[i]) > 0) {           //ランク別組合せで成立した場合に、赤から緑に変更する
        //cel_led=( ~char_rank_col << 16 | char_rank_col << 8) | (1 << col);
        //cel_led=( char_antei_col << 16 | char_rank_col << 8) | (1 << col);
        cel_led=( char_rank_col << 16 | 0xFF << 8) | (1 << col);
    } else if((comp_rank & oneBit[i]) > 0) { cel_led=( char_antei_col << 16 | char_rank_col << 8) | (1 << col);
    } else if(led_r==false) {                cel_led=( char_rank_col << 16 | 0xFF << 8) | (1 << col);
    } else {                                 cel_led=( 0xFF << 16 | char_rank_col << 8) | (1 << col);
    }
    return cel_led;
}
int cel_led_bit_red(int i,int char_rank_col ,int char_antei_col ,int col){
    int cel_led =0;
    if((comp_kumi & oneBit[i]) > 0) {           //ランク別組合せで成立した場合に、赤から緑に変更する
        //cel_led=( ~char_rank_col << 16 | char_rank_col << 8) | (1 << col);
        //cel_led=( char_antei_col << 16 | char_rank_col << 8) | (1 << col);
        cel_led=( char_rank_col << 16 | 0xFF << 8) | (1 << col);
    } else if((comp_rank & oneBit[i]) > 0) { cel_led=( char_antei_col << 16 | char_rank_col << 8) | (1 << col);
    } else if(led_r==false) {                cel_led=( char_rank_col << 16 | 0xFF << 8) | (1 << col);
    } else {                                 cel_led=( 0xFF << 16 | char_rank_col << 8) | (1 << col);
    }
    return cel_led;
}
int cel_led_bit_green(int i,int char_rank_col ,int char_antei_col ,int col){
    int cel_led =0;
    if((comp_kumi & oneBit[i]) > 0) {           //ランク別組合せで成立した場合に、赤から緑に変更する
        //cel_led=( ~char_rank_col << 16 | char_rank_col << 8) | (1 << col);
        //cel_led=( char_antei_col << 16 | char_rank_col << 8) | (1 << col);
        cel_led=( 0xFF << 16 | char_rank_col << 8) | (1 << col);
    } else if((comp_rank & oneBit[i]) > 0) { cel_led=( char_rank_col << 16 | char_antei_col << 8) | (1 << col);
    } else if(led_r==false) {                cel_led=( 0xFF << 16 | char_rank_col << 8) | (1 << col);
    } else {                                 cel_led=( char_rank_col << 16 | 0xFF << 8) | (1 << col);
    }
    return cel_led;
}
/*****************************************************************************
    bitカウント関数
    値 bits を２進表記した場合に現れる 1(ビット) の数を返す。
*****************************************************************************/
int bit_count(int bits)
{
    bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
    bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
    bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
    bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
    return (bits & 0x0000ffff) + (bits >>16 & 0x0000ffff);
}

/*****************************************************************************
    メッセージを流し続ける(ゼロ点エラー時)
*****************************************************************************/
void slideMessage()
{
    lcd.messege(TOP, DIS_ERR, CLS);  //LCD表示
    lcd.locate(0,LOW);

    //何かボタンが押下されたら処理終了
    while(1) {
        lcd.ArrowMosion();
        if(btn.any_press()) break;
    }//while
}
    




