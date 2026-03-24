#include "SoundEffct.h"

//音階配列定数
const float SoundEffct::mm[] = {
    mC,mc,mD,md,mE,mF,mf,mG,mg,mA,ma,mB
};

//コンストラクタ:ピン設定
SoundEffct::SoundEffct(PinName sp): _sp(sp){
    _pitch = 0;  //デフォルトの音階
    _volume = 0.5f;     //音量(ボリューム)
    _deadening = 0.0f;  //消音する(値固定)
}

//出力する音階レベルをセットする
void SoundEffct::set_pitch(int p){
    //1:低い音程(通常)
    //2:中音の音程
    //3:高音の音程
    _pitch =  p;
}

//音量レベルをセットする
void SoundEffct::set_volume(int v){
    switch(v){
        case(1):
            _volume = 0.03; //小音
            break;
        case(2):
            _volume = 0.125; //中音
            break;
        case(3):
            _volume = 0.5;  //大音(これが標準)
            break;
        default:
            _volume = 0.5;
            break;
    }
}
 
 /////////////////////////////////////////////////////////
////サウンド関数
/////////////////////////////////////////////////////////
void SoundEffct::sound(float m,float time){
    _sp.period(1.0/m);
    //_sp.write(0.5);
    _sp.write(_volume);
    wait(time);//音の長さ
    _sp.write(_deadening);
}

/////////////////////////////////////////////////////////
////メロディ01関数//音階上昇
/////////////////////////////////////////////////////////
void SoundEffct::onkai_all(void){    //音階上昇
    int i;
    
    //音階を1音ずつ上げていく
    for(i=0; i<12; i++) {
        onkai(i);
    }
}

//音階出力 引数(音階)(秒数)
void SoundEffct::onkai(int out){    //音階出力
    _sp.period(1.0/mm[out]);
    _sp.write(0.5);
    wait(0.3);//音の長さ
    _sp.write(_deadening);
}

//音階出力 引数(音階)(秒数)
void SoundEffct::onkai(int out, float sty){    //音階出力
    _sp.period(1.0/mm[out]);
    _sp.write(0.5);
    wait(sty);//音の長さ
    _sp.write(_deadening);
}

//音階出力 引数(音階)(秒数)
void SoundEffct::onkaiUp(int out){    //オクターブ上げていく
    static int up= 1;

    for(up = 1; up < 4; up++){
        sound(mm[out]*up  ,0.1);
    }
}

//組み合わせ箇所を効果音で通知する(複数箇所でも最初の一箇所のみ)
void SoundEffct::combiSoundOne(int com){
    for( int bit = 0; com; com >>= 1){
        if(com & 0x01){
            onkaiUp(bit);
            return;
        }
        bit++;
    }
}

//組み合わせ箇所を効果音で通知する(全ての組み合わせ場所を連続で)
void SoundEffct::combiSoundAll(int com){
    for( int bit = 0; com; com >>= 1){
        if(com & 0x01){
            onkai(bit,0.2);
        }
        bit++;
    }
}

//計量物がない場所を音声で通知する
void SoundEffct::noneCombiSound(int kara){
    for( int bit = 0; kara; kara >>= 1){
        if(kara & 0x01){
            onkai(bit);
            return;
        }
        bit++;
    }
}

void SoundEffct::sound_out(void) {
    /*
    j=j+m1;           //jに100μSで動く角度/2を足します。
    if(j>180)j=j-180; //1周期を超えたら360/2を引く
    sp1.write(ms[(int)j]);
    */
    static float j1=0;
    static float j2=0;
    static float j3=0;
    j1=j1+m1;
    j2=j2+m2;
    j3=j3+m3;
    if (j1>180)j1=j1-180;
    if (j2>180)j2=j2-180;
    if (j3>180)j3=j3-180;
    _sp.write((ms[(int)j1]+ms[(int)j2]+ms[(int)j3])/3.0);
}

////メロディ02関数//まちがい警告音
void SoundEffct::mistake(void){
    /*
    sound(mA/3,0.1);
    wait(0.05);
    sound(mA/3,0.1);
    wait(0.2);
    */
    int pitch;
    switch (_pitch){
        case 1:
            pitch = mA/3;
            break;
        case 2:
            pitch = mA;
            break;
        case 3:
            pitch = mA*4;
            break;
        default:
            pitch = mA / 3;
            break;
    }
    sound(pitch,0.1);
    wait(0.05);
    sound(pitch,0.1);
    wait(0.2);
}

////メロディ02関数//まちがい警告音
void SoundEffct::mistake2(void){
    /*
    sound(mA/3,0.1);
    wait(0.05);
    sound(mA/3,0.1);
    wait(0.2);
    */
    int pitch;
    switch (_pitch){
        case 1:
            pitch = mA/3;
            break;
        case 2:
            pitch = mA;
            break;
        case 3:
            pitch = mA*4;
            break;
        default:
            pitch = mA / 3;
            break;
    }
    sound(pitch,0.1);
    wait(0.05);
    sound(pitch,0.1);
    wait(0.05);
    sound(pitch,0.1);
    wait(0.05);
}
void SoundEffct::mistake3(int num){
    switch(num){
        case  1:     mistake();      break;
        case  2:     mistake2();     break;
        case  3:     Touch_03();Touch_03();Touch_03();Touch_03();Touch_03();Touch_03();
                                     break;
        case  4:     ck05();         break;
        case  5:     ck06();         break;
        case  6:     ck07();         break;
        case  7:     ck08();         break;
        case  8:     ck09();         break;
        case  9:     ck10();         break;
        
        
        default:    mistake();      break;
    }
}
//メロディ03関数//タッチ確認音
void SoundEffct::Touch_01(void){
    sound(mA,0.05);
    wait(0.05);
}

//メロディ09関数//タッチ確認音
void SoundEffct::Touch_02(void){
    sound(mA*2,0.05);
    wait(0.05);
}

//メロディ09関数//タッチ確認音
void SoundEffct::Touch_03(void){
    sound(mA*3,0.05);
    wait(0.05);
}
//メロディ//チャイム
void SoundEffct::chime_01(void){
    sound(lA*3,0.3);
    sound(lF*3,0.5);
    wait(0.05);
}
//メロディ//再計量
void SoundEffct::recheck(void){

    sound(mA,0.1);
    wait(0.03);
    sound(mA,0.1);
    wait(0.03);
    sound(mA,0.1);
    wait(0.2);
}
//メロディ//再計量
void SoundEffct::recheck2(void){

    sound(nC,0.05);
    wait(0.02);
    sound(nC,0.05);
    wait(0.02);
    sound(nC,0.05);
    wait(0.02);
    sound(nC,0.05);
    wait(0.02);
    sound(nC,0.05);
    wait(0.2);
}

//準最適値と同時の再計量
void SoundEffct::wait_recheck(void){    //プログラム確認用4
    sound(nE*2,0.1);
    sound(nC*2,0.1);
    sound(nE*2,0.1);
    sound(nC*2,0.2);
    wait(0.2);
}


//OK、完了などの音
void SoundEffct::OK(void){    //openning
    sound(nB*2,0.3);
    sound(nG*2,0.5);
    wait(0.2);
}
//起動時 オープニング用
void SoundEffct::openning(void){    //openning
    sound(mC*2,0.05);
    sound(mD*2,0.1);
    wait(0.2);
}
//プログラム確認用1
void SoundEffct::ck01(void){    //マリオコイン  プログラム確認用1
    sound(nB*2,0.1);
    sound(oE*2,0.2);
    //wait(0.2);
}
//プログラム確認用2
void SoundEffct::ck02(void){    //1up   プログラム確認用2
    sound(nE*2,0.15);
    sound(nG*2,0.15);
    sound(oE*2,0.15);
    sound(oC*2,0.15);
    sound(oD*2,0.15);
    sound(oG*2,0.15);
    wait(0.2);
}
//プログラム確認用3
void SoundEffct::ck03(void){    //ドラクエLvupプログラム確認用3
    sound(mF*2,0.1);
    sound(mF*2,0.1);
    sound(mF*2,0.1);
    sound(mF*2,0.1);
    wait(0.1);
    sound(md*2,0.1);
    wait(0.1);
    sound(mG*2,0.1);
    wait(0.1);
    sound(mF*2,0.7);
    wait(0.2);
}
//プログラム確認用4
void SoundEffct::ck04(void){    //マリオ タイムプログラム確認用4
    sound(nE*2,0.1);
    sound(nC*2,0.1);
    sound(nE*2,0.1);
    sound(nC*2,0.2);
    wait(0.2);
}
//プログラム確認用5
void SoundEffct::ck05(void){    //プログラム確認用5
    sound(nB,0.5);
    sound(nG,0.5);
    sound(nB,0.5);
    sound(nG,0.5);
    wait(0.1);
}
//プログラム確認用6
void SoundEffct::ck06(void){    //プログラム確認用5
    sound(nG,0.5);
    sound(nB,0.5);
    sound(nG,0.5);
    sound(nB,0.5);
    wait(0.1);
}
//プログラム確認用7
void SoundEffct::ck07(void){    //プログラム確認用6
    sound(lC,0.02);
    sound(lD,0.02);
    sound(lE,0.02);
    sound(lF,0.02);
    sound(lG,0.02);
    sound(lA,0.02);
    sound(lB,0.02);
    sound(mC,0.02);
    sound(mD,0.02);
    sound(mE,0.02);
    sound(mF,0.02);
    sound(mG,0.02);
    sound(mA,0.02);
    sound(mB,0.02);
    sound(nC,0.02);
    sound(nD,0.02);
    sound(nE,0.02);
    sound(nF,0.02);
    sound(nG,0.02);
    sound(nA,0.02);
    sound(nB,0.02);
    sound(oC,0.02);
    sound(oD,0.02);
    sound(oE,0.02);
    sound(oF,0.02);
    sound(oG,0.02);
    sound(oA,0.02);
    sound(oB,0.02);
}
//プログラム確認用8
void SoundEffct::ck08(void){    //プログラム確認用7
    sound(oB,0.02);
    sound(oA,0.02);
    sound(oG,0.02);
    sound(oF,0.02);
    sound(oE,0.02);
    sound(oD,0.02);
    sound(oC,0.02);
    sound(nB,0.02);
    sound(nA,0.02);
    sound(nG,0.02);
    sound(nF,0.02);
    sound(nE,0.02);
    sound(nD,0.02);
    sound(nC,0.02);
    sound(mB,0.02);
    sound(mA,0.02);
    sound(mG,0.02);
    sound(mF,0.02);
    sound(mE,0.02);
    sound(mD,0.02);
    sound(lC,0.02);
    sound(lB,0.02);
    sound(lA,0.02);
    sound(lG,0.02);
    sound(lF,0.02);
    sound(lE,0.02);
}
//プログラム確認用9
void SoundEffct::ck09(void){    //プログラム確認用8
    sound(oE,0.3);
    wait(0.05);
    sound(oE,0.3);
    wait(0.05);
    sound(oE,0.3);
    wait(0.05);
}
//プログラム確認用10
void SoundEffct::ck10(void){    //プログラム確認用9
    sound(oE,0.3);
    wait(0.05);
    sound(oE,0.3);
    wait(0.05);
    sound(oE,0.3);
    wait(0.05);
}

void SoundEffct::puter(int num){    //段階音発生
    switch(num){
        case 0:     sound(oC*2,0.05);            break;
        case 1:     sound(nB*2,0.05);            break;
        case 2:     sound(nA*2,0.05);            break;
        case 3:     sound(nG*2,0.05);            break;
        case 4:     sound(nF*2,0.05);            break;
        case 5:     sound(nE*2,0.05);            break;
        case 6:     sound(nD*2,0.05);            break;
        default:    sound(nC*2,0.05);            break;
    }
    wait(0.2);
}