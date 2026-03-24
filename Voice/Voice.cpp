#include "Voice.h"

Voice::Voice(PinName p0, PinName p1): _device(p0, p1){
    _device.baud(9600);  //音声出力デバイスの通信速度設定?
}

void Voice::stop_voice(){ //発声を止め、ICの反応があるまで待つ
    _device.printf("$");
    wait_echo();
    wait_echo2();
}

char buf3;
//音声合成からの返答（">"）待ち
void Voice::wait_echo(){
    
    buf3= _device.getc();
    buf3=0;
    wait(0.5);
}
void Voice::wait_echo2(){
    _device.printf("?");
    wait(0.5);
    for(int i=0 ; i < 200 ; i++){
        if(_device.readable()==1){
            //printf("C\r\n");
            buf3= _device.getc();
            //printf("%c\r\n",buf3);
        }
    }
    buf3=0;
    wait(0.5);
}


void Voice::change_vpitch(int a,int b){
    char voice[300];
    char voice2[300];
    _device.printf("?");
    wait(0.4);
    sprintf(voice,"#W00300\r");
    _device.printf(voice);
    wait(0.4);
    wait_echo();
    if(a==0){
        sprintf(voice2,"#W00264\r");printf("00\r\n");
    _device.printf(voice2);
    wait(0.4);
    wait_echo();
    }else if(a==1){
        sprintf(voice2,"#W00250\r");printf("01\r\n");
    _device.printf(voice2);
    wait(0.4);
    wait_echo();
    }else if(a==2){
        sprintf(voice2,"#W00235\r");printf("02\r\n");
    _device.printf(voice2);
    wait(0.4);
    wait_echo();
    }else{
        sprintf(voice2,"#W00264\r");printf("00\r\n");
    _device.printf(voice2);
    wait(0.4);
    wait_echo();
    }
    //wait_echo();
    _device.printf("?");
    wait_echo();
    wait_echo();
    wait_echo();
    output_audio2();
    wait(0.4);
    wait_echo();
    buf3=0;
}

//音声内容
const char msg_audio[][54] = {
    {"ki'do-chu'-/de_su.\r"}            //起動中です
    ,{"che'xtukuwo/okonaxtuteku'dasa'i+.\r"}  //チェックを行ってください
    ,{"zerogura'muwo,kakuni'nnsitekudasa'i.\r"}   //0g確認してください
    ,{"tairyu'-si'teima_su.\r"} //滞留しています
    ,{"machigai/ka'mo?\r"}      
    ,{"omosu'gi/de_su.\r"}      //重すぎです
    ,{"ra'nkuse'nbetu.\r"}      //ランクセンベツ
    ,{";sentakusitekudasa'i.\r"}//選択してください
    ,{"toripuru/e'su/de_su.\r"}
    ,{"esu/esu/de_su.\r"}
    ,{"esu/de_su.\r"}
    ,{"emu/de_su.\r"}
    ,{"eru/de_su.\r"}
    ,{"e'ru+e'ru+de_su \r"}
    ,{"kikaku'gai/de_su.\r"}            //規格外です
    ,{"erui-dhi-,tento-/kashoni onazimono'wo oite'kudasa'i.\r"} //LED点灯箇所に同じ物を置いてください
    ,{"kumiawasega,arimase'n.mo'tto'nosete'kudasa'i.\r"}        //組合せがありません
    ,{"kumiawasega,arimase'n.irekaetemitekudasai\r"}        //組合せがありません
    
    ,{"'mentenansuga,hitsuyo-desu.\r"}  //メンテナンスが必要です
};

/*****************************************************************************
    指定された音声出力を行う
*****************************************************************************/
void Voice::output_audio(int msg_no){
    _device.printf("?");
    _device.printf(msg_audio[msg_no]);
    //wait_echo();
    if(msg_no == ERR_ZERO || msg_no == MSG_KIDO){
        while(_device.readable()==1){
            //printf("C\r\n");
            buf3= _device.getc();
            //printf("%c\r\n",buf3);
        }
    }
}
void Voice::output_audio2(){
    _device.printf("?");
    char voice[300];
    sprintf(voice,";to-rokusitekudasa'i.\r");
    _device.printf(voice);
    
    buf3= _device.getc();
}
void Voice::output_span_wait(int hunn){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"junnbityuunotame ato <NUMK VAL=%d COUNTER=funn> /omachikudasa+i\r", hunn);
    _device.printf(voice);
    wait_echo();
    buf3=0;
}
void Voice::output_span_000g(){
    _device.printf("?");
    //char voice[300];
    //sprintf(voice,"/hakarino'ue'ni /nanimo/oka'naidekudasai\r");
    //_device.printf(voice);
    _device.printf("/hakarino'ue'ni /nanimo/oka'naidekudasai\r");
    buf3= _device.getc();
}
void Voice::output_span_hundo(int weight){ //オモリを置いてください
    _device.printf("?");
    char voice[300];
    //sprintf(voice,"/;omoriwo /oitekudasa+i\r");
    sprintf(voice,"<NUMK VAL=%d>guramuwo /oitekudasa+i\r",weight);
    _device.printf(voice);
    buf3= _device.getc();
}
void Voice::output_span_next(){ //ランプ点灯箇所に何も置かないでください
    _device.printf("?");
    char voice[300];
    sprintf(voice,";rann+pu/tenntookasyo+ni nanimo/o;ka'na'ide'kudasa'i\r");
    _device.printf(voice);
    buf3= _device.getc();
}
void Voice::output_span_mononose(){ //ランプ点灯箇所に何も置かないでください
    _device.printf("?");
    char voice[300];
    sprintf(voice,";rann+pu/tenntookasyo+ni oitekudasa+i\r");
    _device.printf(voice);
    buf3= _device.getc();
}
void Voice::output_span_ng(){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"o-tosupa'nn dekimasende'sita syudoude okonattekudasa+i\r");
    _device.printf(voice);
    buf3= _device.getc();
}
void Voice::output_case(){ //ランプ点灯箇所にケースを置いてください
    _device.printf("?");
    char voice[300];
    sprintf(voice,";rann+pu/tenntookasyo+ni ke-suwo;/oitekudasa+i\r");
    _device.printf(voice);
    buf3= _device.getc();
}
void Voice::output_case2(){ //ランプ点灯箇所にケースを置いてください
    _device.printf("?");
    char voice[300];
    sprintf(voice,";keisokusimasita tugiesusumetekudasa+i\r");
    _device.printf(voice);
    buf3= _device.getc();
}
void Voice::output_rank_over(){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"o'-ba-shimashita\r");
    _device.printf(voice);
}
void Voice::over_limit(){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"kaisu-o'-ba-+shima+shi+ta\r");
    _device.printf(voice);
    wait_echo();
    sprintf(voice,"/hennkyakusitekudasa+i\r");
    _device.printf(voice);
    wait_echo();
    buf3=0;
}
void Voice::last_limit(int limit){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"ato <NUMK VAL=%d COUNTER=kai> /shiyoudekima+su\r",limit);
    _device.printf(voice);
    wait_echo();
    buf3=0;
}
void Voice::Unregister(){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"/tourokusareteimasenn\r");
    _device.printf(voice);
    wait_echo();
    buf3=0;
}

void Voice::gram_voice(int weight){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"<NUMK VAL=%d>guramu\r",weight);
    _device.printf(voice);
    wait_echo();
    buf3=0;
}

void Voice::nokori_Sara_suu(int sara){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"nokori<NUMK VAL=%d>sara\r",sara);
    _device.printf(voice);
    wait_echo();
    buf3=0;
}
void Voice::imano_Sara_suu(int sara){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"gennzai<NUMK VAL=%d>sara\r",sara);
    _device.printf(voice);
    wait_echo();
    buf3=0;
}
void Voice::gram_voice_hani(int bai, int weight1, int weight2){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"<NUMK VAL=%d>ten<NUMK VAL=%d>guramu kara <NUMK VAL=%d>ten<NUMK VAL=%d>guramu\r",int(weight1/bai),weight1 % bai,int(weight2/bai),weight2 % bai);
    _device.printf(voice);
    wait_echo();
    buf3=0;
}

void Voice::rank_guide(int size ,int gram){
    _device.printf("?");
    char voice[300];
    printf("%3d\r\n",size);
    //if(C_RANK_START+1 > size || C_RANK_END2-1 < size){
    if(C_9S <= size && size < C_S){
        sprintf(voice,"<ALPHA VAL=%d%s>/wa",C_S-size+1,"S");
    }else if(C_L <size && size <= C_9L){
        sprintf(voice,"<ALPHA VAL=%d%s>/wa",size-C_L+1,"L");
    }else if(size == C_S){
        sprintf(voice,"<ALPHA VAL=%s>/wa","S");
    }else if(size == C_M){
        sprintf(voice,"<ALPHA VAL=%s>/wa","M");
    }else if(size == C_L){
        sprintf(voice,"<ALPHA VAL=%s>/wa","L");
    }else if(size == C_SA){
        sprintf(voice,"<ALPHA VAL=%s/wa>","SA");
    }else if(size == C_MA){
        sprintf(voice,"<ALPHA VAL=%s>/wa","MA");
    }else if(CR_01 <= size && size <= CR_99 ){
        sprintf(voice,"<NUMK VAL=%d>/wa",size-CR_01+1);
    }else{
        sprintf(voice,"kikaku'gai/de_su.");
    }
    
    sprintf(voice,"%s,<NUMK VAL=%d>guramuwo,erannde,oitekuda'sai\r",voice,gram);
    
    
    _device.printf(voice);
    wait_echo();
    buf3=0;
}

void Voice::demo_00(){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"konnnitiwa,;te-burukonnbi'de'su,,,kino'uno,setumeiwookonaima'_su\r");
    _device.printf(voice);
    //wait_echo();
    //sprintf(voice,"kinou'no,setumeiwookonaimasu\r");
    //_device.printf(voice);
    //wait_echo();
    buf3=0;
}
void Voice::demo_01(){
    demo_0101();
    demo_0102();
}
void Voice::demo_0101(){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"kowakesurunonibenn'rina,kumiawasekeiryo'ude'_su\r");
    _device.printf(voice);
    wait_echo();
    buf3=0;
}

void Voice::demo_0102(){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"yasaiwo oitekudasai\r");
    _device.printf(voice);
    wait_echo();
    buf3=0;
}
void Voice::demo_02(int weight,int target,int jogen){
    _device.printf("?");
    char voice[300];
    char voice2[300];
    char voice3[300];
    sprintf(voice,"setteiwa,<NUMK VAL=%d>gura'mukara,<NUMK VAL=%d>guramumadedesu\r",target,target+jogen);
    _device.printf(voice);
    wait_echo();
    sprintf(voice2,"konohanni'nihaittamonoga,hikarimasu\r");
    _device.printf(voice2);
    wait_echo();
    //sprintf(voice,"korekarahikarutokoroga,<NUMK VAL=%d>guramude'su\r",weight);
    //_device.printf(voice);
    //wait_echo();
    sprintf(voice3,"hikattatokorowo /subete /matometekudasai\r");
    _device.printf(voice3);
    wait_echo();
    buf3=0;
}
void Voice::demo_03(int weight){
    _device.printf("?");
    char voice[300];
    char voice2[300];
    char voice3[300];
    sprintf(voice,"ima/matometamonoga,<NUMK VAL=%d>guramude'su\r",weight);
    _device.printf(voice);
    wait_echo();
    sprintf(voice2,"atowa,hukuro'nitume'rudakede'su kanntanndesyo?\r");
    _device.printf(voice2);
    wait_echo();
    sprintf(voice3,"tugiwa,donndonnhikarimasu, doujinihikattamonowo/tottekudasa+i\r");
    _device.printf(voice3);
    wait_echo();
    buf3=0;
}
void Voice::demo_04(){
    _device.printf("?");
    char voice[300];
    char voice2[300];
    sprintf(voice,"tugiwa, sennbetusagyounibennrina, rannkusennbetude'su\r");
    _device.printf(voice);
    wait_echo();
    buf3=0;
    sprintf(voice2,"setteisitajuuryouniawasete, rannkugahyoujisaremasu\r");
    _device.printf(voice2);
    wait_echo();
}
void Voice::mistake(){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"torimatigaide'su\r");
    _device.printf(voice);
    //wait_echo();
    buf3=0;
}
void Voice::mistake2(){
    _device.printf("?");
    char voice[300];
    sprintf(voice,"torimatigaide'su saikeisannsima'su\r");
    _device.printf(voice);
    //wait_echo();
    buf3=0;
}

//音声内容
const char losfile[][54] = {
    //,{"kumiawasega,arimasen.\r"}      //組合せがありません
    {"de-taga,kiemasita.kannrigaisyae,toiawasetekudasai\r"}          //データが消えました管理会社へ問い合わせてください
    ,{"parame-taga,kiemasita\r"}          //パラメータが消えました
    ,{"zennkainode-taga,kiemasita\r"}       //初期データが消えました
    //,{"supannde-taga,kiemasita,supanntyouseiwo,sitekudsai\r"}      //スパンデータが消えました
    ,{"supannde-taga,kiemasita,\r"}      //スパンデータが消えました
    ,{"tourokude-taga,kiemasita\r"}     //登録データが消えました
    ,{"opusyonde-taga,kiemasita\r"}     //オプションデータが消えました
    ,{"syokide-taga,kiemasita\r"}       //初期データが消えました
    ,{"tourokude-taga,kiemasita\r"}     //登録データが消えました
    ,{"namaede-taga,kiemasita\r"}       //名前データが消えました
    ,{"opusyonde-taga,kiemasita\r"}     //オプションデータが消えました
    ,{"seigennde-taga,kiemasita\r"}     //制限データが消えました
    ,{"seigennde-taga,kiemasita\r"}     //制限データが消えました
    ,{"seigennde-taga,kiemasita\r"}     //制限データが消えました
    ,{"jikannde-taga,kiemasita\r"}     //時間データが消えました
};

/*****************************************************************************
    指定された音声出力を行う
*****************************************************************************/
void Voice::output_losfile(int file_no){
    _device.printf("?");
    _device.printf(losfile[file_no]);
    wait_echo();
}
void Voice::output_defoult(){
    char voice[30];
    _device.printf("?");
    sprintf(voice,"deforutoni,modosimasu\r");
    _device.printf(voice);
    wait_echo();
}
void Voice::output_retastar_span(){
    char voice[30];
    _device.printf("?");
    sprintf(voice,"supanntyouseiwo,sitekudasai\r");
    _device.printf(voice);
    wait_echo();
}
void Voice::speak_numbers(int i){
    char voice[30];
    //wait_echo2();
    
    _device.printf("?");
    sprintf(voice,"<NUMK VAL=%d>,\r",i);
    _device.printf(voice);
    wait_echo();
}
/*****************************************************************************
    組み合わせの回数に応じて、指定された音声出力を行う
    setNum : 指定した回数
    num    : 組み合わせた回数
*****************************************************************************/
void Voice::output_audio_Count(int setNum, int num){
    char voice[30];
    int check = setNum - num;

    if(check < 0) {
        return;
    }
    if(setNum < 0) {
        return;
    }

    if(setNum > 0 && check > 0) {
        if(check == 10 || check == 50 || check % 100 == 0 || check % 500 == 0 || check % 1000 == 0) {
            //音声出力準備
            _device.printf("?");
            sprintf(voice,"ato<NUMK VAL=%d>/de_su.\r",check);
            _device.printf(voice);
            wait(1.0);
            buf3= _device.getc();
        }
    }
    if(check == 0) {
        //音声出力準備
        _device.printf("?");
        _device.printf("shu-u/ryo-/de_su.\r");
        wait(1.0);
        buf3= _device.getc();
    }
}
