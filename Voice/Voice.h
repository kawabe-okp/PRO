/*
 * 
 * mbed the Voice
 * Copyright (c) 2015 ito shogo
 * 
 */ 

#ifndef Voice_h
#define Voice_h

#include "mbed.h"
#include "led_matrix.h"

//音声出力の種類
typedef enum voiceType{
    MSG_KIDO,
    ERR_CHE,
    ERR_ZERO,
    ERR_TAIRYU,
    ERR_MATIGAI,
    ERR_OMOI,
    MSG_SENBETSU,
    MSG_SELECT,
    MSG_TORIPURU,
    MSG_SS,
    MSG_S,
    MSG_M,
    MSG_L,
    MSG_LL,
    MSG_NO_RANK,
    MSG_LED_OITE,
    MSG_NO_JUNSAITEKI,
    MSG_NO_JUNSAITEKI2,
    MSG_MAINTENANCE
}vType;

class Voice
{
public:
    Voice(PinName p0, PinName p1);
    void stop_voice();      //発声を止める
    void wait_echo();               //音声合成からの返答（">"）待ち
    void wait_echo2();               //音声合成からの返答残ってたら（">"）処理
    void output_audio(int msg_no);
    void output_audio2();
    void output_audio_Count(int setNum, int num);
    void output_losfile(int file_no);
    void output_defoult();
    void output_retastar_span();
    void output_span_wait(int hunn);
    void output_span_000g();
    void output_span_hundo(int weight);
    void output_span_next();
    void output_span_mononose();
    void output_case();
    void output_case2();
    void output_span_ng();
    void output_rank_over();
    void over_limit();
    void last_limit(int limit);
    void Unregister();//使用不可な登録内容だったら
    void gram_voice(int weight);
    void gram_voice_hani(int bai, int weight1, int weight2);
    void size_voice(int size);
    void rank_guide(int size ,int gram);
    void nokori_Sara_suu(int sara);
    void imano_Sara_suu(int sara);
    void change_vpitch(int a,int b);
    void demo_00();
    void demo_01();
    void demo_0101();
    void demo_0102();
    void demo_02(int weight,int target,int jogen);
    void demo_03(int weight);
    void demo_04();
    void mistake();
    void mistake2();
    void speak_numbers(int i);
private:
    Serial _device;
};

#endif