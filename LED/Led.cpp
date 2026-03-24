#include "Led.h"
#include "led_matrix.h"
extern int cel_led[12];
extern int iro[12]={999,999,999,0,0,0,0,0,0,0,0,0};
extern int matrix_mode;  
extern int katamen;              //0:no 1:1~6 2:7~12  12ﾍｯﾄﾞを片面だけ使用する(自分の皿でLEDが隠れる場合,6ﾍｯﾄﾞで使用するときの表示)
extern int para_head;
extern int para_array;

//***********基板の改造具合によって変更が必要**************************
    //***********デフォルト************************************************
        //Led::Led(PinName d0, PinName d1, PinName d2, PinName d3):_SI(d0),
         //_SCK(d1), _RCK(d2), _red(d3){

    //***********未改造の基板用************************************************
        //Led::Led(PinName d0, PinName d1, PinName d2, PinName d3, PinName d4, PinName d5, PinName d6):_SI(d0),
        //_SCK(d1), _RCK(d2), _RCK2(d3), _SI2(d4), _SCK2(d5), _RCK2(d6){
    //***********改造済の基板用************************************************
        Led::Led(PinName d0, PinName d1, PinName d2, PinName d3, PinName d4, PinName d5):_SI(d0),
        _SCK(d1),  _RCK(d2),   _RCK2(d3)   ,_SI2(d4), _SCK2(d5){
//***********************************************************
     _timeAnime = 0.1;  //LEDアニメの間隔
}

//点灯,操作させるLEDの数を指定
void Led::set(int s){
    _headNum = s;
}

//赤,青LEDを全て消灯
void Led::ledAllOff(){
    if (matrix_mode==1){
        for(int i = 0; i<12; i++) {
                iro[i]=C_NULL;        
        }
    }
    else{
        REGIS(0);
        RedOff();
    }
}

//赤LED点灯
void Led::RedOn(){
    if (matrix_mode==1){
        for(int i = 0; i<12; i++) {
            if(iro[i]==C_NULL){
                iro[i]=PENTA_R;
            }
            else if(iro[i]==PENTA_G){      
                iro[i]=C_NULL;
            }
        }
    }
    else{
        _RCK2 = 1;
    }
}

//赤LED点灯
void Led::RedOff(){
    if (matrix_mode==1){
        for(int i = 0; i<12; i++) {
            if(iro[i]==PENTA_R){
                iro[i]=C_NULL;
            }        
        }
    }
    else{
        _RCK2 = 0;
    }
}

/*****************************************************************************
    シフトレジスタ関数
    xの値によって各LEDを点灯させる
    bit 1=点灯, 0=消灯;
    yはLEDの数になる
*****************************************************************************/
void Led::REGIS(int x){
    int bit_chk,bit_re;
    //設定に伴う点灯位置の反転
    char bit_led=0x00;
    for(int i=0;i<(para_head/2);i++){
        if(i>0){
            bit_led=bit_led<<1;
        }
        bit_led=bit_led | 1;
    }
    if(katamen==1){
        printf("%8d%8d%8d%8d\r\n",x,bit_led,((x & bit_led) << (para_head/2)),(x & (bit_led<<(para_head/2))));
        x = ((x & bit_led) << (para_head/2))| ((x & (bit_led<<(para_head/2)))  >> (para_head/2));//下位ビットを上位へ移動したものと上位ビットを下位へ移動したものの合算
    }else if(katamen==2){
        x = ((x & bit_led) << (para_head/2))| ((x & (bit_led<<(para_head/2)))  >> (para_head/2));//下位ビットを上位へ移動したものと上位ビットを下位へ移動したものの合算
    }

    
    if (matrix_mode==1){
        if(x==0){
            for(int i = 0; i<12; i++) {
                iro[i]=C_NULL;        
            }
        }
        else{
            bit_re = x;
            for(int i = 0; i<12; i++) {
                bit_chk= bit_re & 0x01;
                if(bit_chk==1){
                    iro[i]=PENTA_G;
                }
                else{
                    if(iro[i]!=PENTA_R){
                        iro[i]=C_NULL;
                    }
                }     
                bit_re = bit_re >>1;   
            }
            
        }
    }
    else{//マトリクスモード以外＝LED
        _SCK=0;  //シフトレジスタクロック初期化(念のため)
        _RCK=0;  //ラッチクロック(データ送信)開始
    
        //上位ビットから順にシフトレジスタへデータ送信
        for(int i = 11; i>=0; i--) {
    
            /*解説→(x & oneBit[i]) >> i
                xの論理積を求めて上位ビットから順にビット値を取り出す
            */
            //_SI = (x & Bit[i]) >> i;
            _SI = (x & (1<<i)) >> i;
            _SCK=1;  //シフトレジスタクロック(1bit送信)開始
            _SCK=0;  //シフトレジスタクロック(1bit送信)終了
        }
        _RCK=1;  //ラッチクロック(データ送信)終了
    }
}//REGIS
/**********************************************************************
マトリクスのみ、1セル毎に赤設定が可能なので、
指定セルだけ赤を点灯させる
**********************************************************************/
void Led::REGIS_RED(int x){
    int bit_chk,bit_re;
    //設定に伴う点灯位置の反転
    char bit_led=0x00;
    for(int i=0;i<(para_head/2);i++){
        if(i>0){
            bit_led=bit_led<<1;
        }
        bit_led=bit_led | 1;
    }
    if(katamen==1){
        x = ((x & bit_led) << para_head/2 )| ((x & bit_led<<para_head/2)  >> para_head/2);//下位ビットを上位へ移動したものと上位ビットを下位へ移動したものの合算
    }else if(katamen==2){
        x = ((x & bit_led) << para_head/2 )| ((x & bit_led<<para_head/2)  >> para_head/2);//下位ビットを上位へ移動したものと上位ビットを下位へ移動したものの合算
    }

    
    if (matrix_mode==1){
        if(x==0){
            for(int i = 0; i<12; i++) {
                iro[i]=C_NULL;        
            }
        }
        else{
            bit_re = x;
            for(int i = 0; i<12; i++) {
                bit_chk= bit_re & 0x01;
                if(bit_chk==1){
                    iro[i]=PENTA_R;
                }
                else{
                    //if(iro[i]!=PENTA_R){
                        iro[i]=C_NULL;
                    //}
                }     
                bit_re = bit_re >>1;   
            }
            
        }
    }
    else{
        _SCK=0;  //シフトレジスタクロック初期化(念のため)
        _RCK=0;  //ラッチクロック(データ送信)開始
    
        //上位ビットから順にシフトレジスタへデータ送信
        for(int i = 11; i>=0; i--) {
    
            /*解説→(x & oneBit[i]) >> i
                xの論理積を求めて上位ビットから順にビット値を取り出す
            */
            //_SI = (x & Bit[i]) >> i;
            _SI = (x & (1<<i)) >> i;
            _SCK=1;  //シフトレジスタクロック(1bit送信)開始
            _SCK=0;  //シフトレジスタクロック(1bit送信)終了
        }
        _RCK=1;  //ラッチクロック(データ送信)終了
    }
}//REGIS_RED

/**********************************************************************
マトリクスのみ、1セル毎に赤設定が可能なので、
指定セルだけ[×]を点灯させる
**********************************************************************/
void Led::REGIS_BATU(int x){
    int bit_chk,bit_re;
    //設定に伴う点灯位置の反転
    char bit_led=0x00;
    for(int i=0;i<(para_head/2);i++){
        if(i>0){
            bit_led=bit_led<<1;
        }
        bit_led=bit_led | 1;
    }
    if(katamen==1){
        x = ((x & bit_led) << para_head/2 )| ((x & bit_led<<para_head/2)  >> para_head/2);//下位ビットを上位へ移動したものと上位ビットを下位へ移動したものの合算
    }else if(katamen==2){
        x = ((x & bit_led) << para_head/2 )| ((x & bit_led<<para_head/2)  >> para_head/2);//下位ビットを上位へ移動したものと上位ビットを下位へ移動したものの合算
    }

    
    if (matrix_mode==1){
        if(x==0){
            for(int i = 0; i<12; i++) {
                iro[i]=C_NULL;        
            }
        }
        else{
            bit_re = x;
            for(int i = 0; i<12; i++) {
                bit_chk= bit_re & 0x01;
                if(bit_chk==1){
                    iro[i]=C_BATU;
                }
                else{
                    //if(iro[i]!=PENTA_R){
                        iro[i]=C_NULL;
                    //}
                }     
                bit_re = bit_re >>1;   
            }
            
        }
    }
    else{
        _SCK=0;  //シフトレジスタクロック初期化(念のため)
        _RCK=0;  //ラッチクロック(データ送信)開始
    
        //上位ビットから順にシフトレジスタへデータ送信
        for(int i = 11; i>=0; i--) {
    
            /*解説→(x & oneBit[i]) >> i
                xの論理積を求めて上位ビットから順にビット値を取り出す
            */
            //_SI = (x & Bit[i]) >> i;
            _SI = (x & (1<<i)) >> i;
            _SCK=1;  //シフトレジスタクロック(1bit送信)開始
            _SCK=0;  //シフトレジスタクロック(1bit送信)終了
        }
        _RCK=1;  //ラッチクロック(データ送信)終了
    }
}//REGIS_BATU


void Led::REGIS3(int x){
    int bit_chk,bit_re;
    //設定に伴う点灯位置の反転
    char bit_led=0x00;
    for(int i=0;i<(para_head/2);i++){
        if(i>0){
            bit_led=bit_led<<1;
        }
        bit_led=bit_led | 1;
    }
    if(katamen==1){
        x = ((x & bit_led) << para_head/2 )| ((x & bit_led<<para_head/2)  >> para_head/2);//下位ビットを上位へ移動したものと上位ビットを下位へ移動したものの合算
    }else if(katamen==2){
        x = ((x & bit_led) << para_head/2 )| ((x & bit_led<<para_head/2)  >> para_head/2);//下位ビットを上位へ移動したものと上位ビットを下位へ移動したものの合算
    }

    
    if (matrix_mode==1){
        if(x==0){
            for(int i = 0; i<12; i++) {
                iro[i]=C_NULL;        
            }
        }
        else{
            bit_re = x;
            for(int i = 0; i<12; i++) {
                bit_chk= bit_re & 0x01;
                if(bit_chk==1){
                    iro[i]=C_R_EXCLA;
                }
                else{
                    if(iro[i]!=PENTA_R){
                        iro[i]=C_NULL;
                    }
                }     
                bit_re = bit_re >>1;   
            }
            
        }
    }
    else{
        _SCK=0;  //シフトレジスタクロック初期化(念のため)
        _RCK=0;  //ラッチクロック(データ送信)開始
    
        //上位ビットから順にシフトレジスタへデータ送信
        for(int i = 11; i>=0; i--) {
    
            /*解説→(x & oneBit[i]) >> i
                xの論理積を求めて上位ビットから順にビット値を取り出す
            */
            //_SI = (x & Bit[i]) >> i;
            _SI = (x & (1<<i)) >> i;
            _SCK=1;  //シフトレジスタクロック(1bit送信)開始
            _SCK=0;  //シフトレジスタクロック(1bit送信)終了
        }
        _RCK=1;  //ラッチクロック(データ送信)終了
    }
}//REGIS3

/*****************************************************************************/
void Led::regis2(){
//void Led::regis2(int c01,int c02,int c03,int c04,int c05,int c06){
    _SCK=0;  //シフトレジスタクロック初期化(念のため)
    _RCK=0;  //ラッチクロック(データ送信)開始
    _SCK2=0;  //シフトレジスタクロック初期化(念のため)
    _RCK2=0;  //ラッチクロック(データ送信)開始
    int head_end = 0;
    int harf_head =0;
    if(para_head==8 && (para_array==1 || para_array==2)){
        head_end = 7;
    //}else if(para_head==10 && para_array==0){
    }else if(para_array==0){
        head_end = para_head/2-1;
        harf_head =para_head/2;
    }else{
        //head_end = 5;
        //harf_head =6;
        head_end = para_head-1;
        harf_head = para_head;
    }

    for(int k = head_end; k>=0; k--) {
    //for(int k = 11; k>=0; k--) {
        for(int i = 23; i>=0; i--) {
    
            //解説→( & (1 << i)) >> i
                //xのi番目の論理積を求めて
                //上位ビットから順にビット値を取り出す
            _SCK=0;  //シフトレジスタクロック(1bit送信)終了
            _SCK2=0;  //シフトレジスタクロック(1bit送信)終了
            //wait_us(1);
            //if(para_head!=8){
                _SI2 = (cel_led[k+harf_head] & (1<<i)) >> i;
            //}
            _SI = (cel_led[k] & (1<<i)) >> i;
            //wait_us(1);
            _SCK=1;  //シフトレジスタクロック(1bit送信)開始
            _SCK2=1;  //シフトレジスタクロック(1bit送信)開始
            //wait_us(2);
            _SCK=0;  //シフトレジスタクロック(1bit送信)終了
            _SCK2=0;  //シフトレジスタクロック(1bit送信)終了
            
            _SI2 = 0;
            _SI =  0 
            ;
            //if(k>=5){
                //_SI2 = (cel_led[k+6] & (1<<i)) >> i;
                //_SCK2=1;  //シフトレジスタクロック(1bit送信)開始
                //_SCK2=0;  //シフトレジスタクロック(1bit送信)終了
            //}
        }
    }
    _RCK=1;  //ラッチクロック(データ送信)終了
    _RCK2=1;  //ラッチクロック(データ送信)終了
}//REGIS
/*****************************************************************************
    共通部分を関数化
    エラー時などにどの計量皿かLEDで知らせる
*****************************************************************************/
void Led::Flash(int k)
{
    wait(0.03);
    REGIS(k);

    wait(0.1);
    REGIS(0);

    wait(0.1);
    REGIS(k);
    
    wait(0.1);
    REGIS(0);
    
    wait(0.1);
    REGIS(k);
    
    wait(0.1);
    REGIS(0);
    RedOn();
    RedOn();
}
/*****************************************************************************
    共通部分を関数化
    エラー時などにどの計量皿かLEDで知らせる
    ※赤点灯Ver
*****************************************************************************/
void Led::Flash_RED(int k)
{
    wait(0.03);
    REGIS_RED(k);

    wait(0.1);
    REGIS_RED(0);

    wait(0.1);
    REGIS_RED(k);
    
    wait(0.1);
    REGIS_RED(0);
    
    wait(0.1);
    REGIS_RED(k);
    /*
    wait(0.1);
    REGIS_RED(0);
    
    wait(0.1);
    REGIS_RED(k);*/
}

/*****************************************************************************
    共通部分を関数化
    エラー時などにどの計量皿かLEDで知らせる
    ※[×]表示Ver
*****************************************************************************/
void Led::Flash_BATU(int k)
{
    wait(0.03);
    REGIS_BATU(k);

    wait(0.1);
    REGIS_BATU(0);

    wait(0.1);
    REGIS_BATU(k);
    
    wait(0.1);
    REGIS_BATU(0);
    
    wait(0.1);
    REGIS_BATU(k);
    /*
    wait(0.1);
    REGIS_BATU(0);
    
    wait(0.1);
    REGIS_BATU(k);*/
}
/*****************************************************************************
    共通部分を関数化
    エラー時などにどの計量皿かLEDで知らせる(青点灯のみ)
*****************************************************************************/
void Led::Flash2(int k)
{
    wait(0.03);
    REGIS(k);

    wait(0.1);
    REGIS(0);

    wait(0.1);
    REGIS(k);

    wait(0.1);
    REGIS(0);

    wait(0.1);
    REGIS(k);

    wait(0.1);
    REGIS(0);

    wait(0.1);
    REGIS(k);
    
    wait(0.1);
    REGIS(0);
    
    wait(0.1);
    REGIS(k);
    
    wait(0.1);
    REGIS(0);
    
    wait(0.1);
    REGIS(k);
    
    wait(0.5);
    REGIS(0);
}

/*****************************************************************************
    共通部分を関数化
    エラー時などにどの計量皿かLEDで知らせる
    ※ドットマトリクス専用の機能
*****************************************************************************/
void Led::Flash_r(int l)
{
    wait(0.03);
    iro[l]=PENTA_R;

    wait(0.1);
    REGIS(0);

    wait(0.1);
    iro[l] =PENTA_R;
    
    wait(0.1);
    REGIS(0);
    
    wait(0.1);
    iro[l] = PENTA_R;
    
    wait(0.1);
    REGIS(0);
    iro[l] = PENTA_R;
   
}
/////////////////////////////////////////////////////////
/////LEDアニメーション 2
/////////////////////////////////////////////////////////
void Led::ANIME(void){
    RedOn();
    for(int i = 0; i < _headNum; i++) {
        REGIS(pow((double)2,i));
        wait(_timeAnime);
    }
    for(int i = _headNum -1; i>=0; i--) {
        REGIS(pow((double)2,i));
        wait(_timeAnime);
    }
    ledAllOff();
}