/*
 * 
 * mbed the Sound
 * Copyright (c) 2015 ito shogo
 * 
 */ 


#ifndef SOUND_EFFCT_H
#define SOUND_EFFCT_H

#include "mbed.h"

#define lC 130.813  //ド
#define lc 138.591  //ド#
#define lD 146.832  //レ
#define ld 155.563  //レ#
#define lE 164.814  //ミ
#define lF 174.614  //ファ
#define lf 184.997  //ファ#
#define lG 195.998  //ソ
#define lg 207.652  //ソ#
#define lA 220.000  //ラ
#define la 233.082  //ラ#
#define lB 246.942  //シ

#define mC 261.626  //ド
#define mc 277.183  //ド#
#define mD 293.665  //レ
#define md 311.127  //レ#
#define mE 329.628  //ミ
#define mF 349.228  //ファ
#define mf 369.994  //ファ#
#define mG 391.995  //ソ
#define mg 415.305  //ソ#
#define mA 440.000  //ラ
#define ma 466.164  //ラ#
#define mB 493.883  //シ

#define nC 523.251  //ド
#define nc 554.365  //ド#
#define nD 587.330  //レ
#define nd 622.254  //レ#
#define nE 659.255  //ミ
#define nF 698.456  //ファ
#define nf 739.989  //ファ#
#define nG 783.991  //ソ
#define ng 830.609  //ソ#
#define nA 880.000  //ラ
#define na 932.328  //ラ#
#define nB 987.767  //シ

#define oC 1046.502  //ド
#define oc 1108.731  //ド#
#define oD 1174.659 //レ
#define od 1244.508  //レ#
#define oE 1318.510  //ミ
#define oF 1396.913  //ファ
#define of 1479.978  //ファ#
#define oG 1567.982  //ソ
#define og 1661.219  //ソ#
#define oA 1760.000  //ラ
#define oa 1864.655  //ラ#
#define oB 1975.533  //シ

class SoundEffct
{
public:
    SoundEffct(PinName sp);
    void onkai_all();
    
    void onkai(int out);
    void onkai(int out, float sty);  //音出力の長さを指定できる
    void onkaiUp(int out);          //音階上昇
    
    void sound(float m,float time);
    
    void combiSoundOne(int com);//組み合わせ箇所を(最初だけ)効果音で通知する
    void combiSoundAll(int com);//組み合わせ箇所を(全て)効果音で通知する
    void noneCombiSound(int kara);//計量物がない場所を音声で通知する(用途が逆だがcombiSoundとほぼ一緒)
    void sound_out();
    void Touch_01();
    void Touch_02();
    void Touch_03();
    void chime_01();
    void recheck();
    void recheck2();
    void wait_recheck();
    void OK(void);
    void openning(void);    //起動時のオープニング音
    void mistake();         //警告音
    void mistake2();         //警告音
    void mistake3(int num);  //警告音
    void set_pitch(int p);  //音階を設定
    void set_volume(int v); //音量を設定(ボリューム)
    void ck01(void);    //プログラム確認用1
    void ck02(void);    //プログラム確認用2
    void ck03(void);    //プログラム確認用3
    void ck04(void);    //プログラム確認用4
    void ck05(void);    //プログラム確認用5
    void ck06(void);    //プログラム確認用6
    void ck07(void);    //プログラム確認用7
    void ck08(void);    //プログラム確認用8
    void ck09(void);    //プログラム確認用9
    void ck10(void);    //プログラム確認用9
    void puter(int num);//段階音発生
private:
    PwmOut _sp;             //メロディ
    float m1,m2,m3;
    float ms[180];
    int _pitch;             //音の高さ
    float _volume;          //音量変更
    float _deadening;       //消音
    static const float mm[];            //音階
};

#endif
