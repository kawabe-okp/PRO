#include "LoadCell.h"

extern int DEBUG_MODE = 0;
extern int use_span;

extern int gLevel; 
extern bool auto_zero_off;  //自動風袋引き解除フラグ
const int DEFALT_HEAD = 12; //既定のヘッド数を仮に12としておく
//const int KAKUNIN = 2;      //CUT以上の重量がある場合のフラグ値
const float SEIDO_UP = 1.0; //重量を確定する場合の正確性を上昇させるための許容重量
const int SPAN_KURI = 3000; //ゼロ点取るときの繰り返し回数
//const float SABUN = 1.2;    //auto_zero時の重量取得時の前後の許容差分値(g).許容範囲内ならスパンを取る
      float SABUN = 1.2;    //auto_zero時の重量取得時の前後の許容差分値(g).許容範囲内ならスパンを取る
//const int AUTOZERO = 2;     //ゼロ点調整の安定していると判断する確認回数を設定
      int AUTOZERO = 0;     //ゼロ点調整の安定していると判断する確認回数を設定
extern float oldweightVale[12];
extern float nowweightVale[12];     //現在重量
extern float image_weight[14];
extern int log_flag;
extern int stop_cells;
extern int span_hundo[10];
extern int keiryou_kurikaesi;
extern int oya_hundo;

extern int antei_flug[12];
extern int antei_flug2[12];
extern float span_preset[12][2];
extern int span_hundo_preset;

float betwe_spa[12][10]=  {{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
                           {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
                           {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
                           {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0} };


float betwe_spa2[12][10]= {{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
                           {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
                           {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},
                           {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0},{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0} };

extern int auto_zero_check;


extern bool testmode1;                  //テストプログラムONフラグ
extern bool testmode2;                  //テストプログラムONフラグ
extern bool testmode3;                  //テストプログラムONフラグ
extern bool TOP_USER; 




extern float AD_now_max[12];
extern float AD_now_min[12];
extern float AD_now_dif[12];
extern float AD_bas_max[12];
extern float AD_bas_min[12];
extern float AD_bas_dif[12];

extern int   no_zero_counter[14];
extern bool  forced_zero_flg[14];

float raw_data_0;

//コンストラクタ:ピン設定
LoadCell::LoadCell(PinName s0, PinName s1, PinName s2, PinName s3, PinName cell): _s0(s0),
_s1(s1), _s2(s2) ,_s3(s3), _cell(cell){
    _numHead = DEFALT_HEAD;  //ヘッド数がないのはありえない。指定忘れ対策
}

/*****************************************************************************
    テーブルコンビのヘッドの数を指定
    set_num_head実行後は、必ずmake_new_pointerを実行すること。
*****************************************************************************/
void LoadCell::set_num_head(int num){
    _numHead = num;
}

/*****************************************************************************
    動的配列のnewを実行
*****************************************************************************/
void LoadCell::make_new_pointer(){
    _pSpan = new span_t[_numHead];
    _pVal = new Val[_numHead];
    //_pVal_span = new Val[_numHead];
    //_pVal = new Val[_numHead];
    _pZero = new int[_numHead];
    _pAutoCount = new int[_numHead];
    _pAnteiCount = new int [_numHead]; //170926 安定した重量がどれだけ繰り返されたか
}

/*****************************************************************************a
    どのロードセルを使うか指定
*****************************************************************************/
void LoadCell::set_head(int num){
    //実際のロードセルの数はHEAD数とイコールのため
    //ヘッド位置がHEADより多い場合にはエラーにならないように訂正
    if(num >= _numHead){
        num -= _numHead;
    }       
    _head = num;
    MULUTIPLEXER(_head);
}

/*****************************************************************************
    マルチプレクサ関数
    指定先のロードセル電圧を取得するように変更
*****************************************************************************/
void LoadCell::MULUTIPLEXER(int taget){
    _s3=0,_s2=0,_s1=0,_s0=0;
    if(taget==0)  _s3=0,_s2=0,_s1=0,_s0=0;
    if(taget==1)  _s3=0,_s2=0,_s1=0,_s0=1;
    if(taget==2)  _s3=0,_s2=0,_s1=1,_s0=0;
    if(taget==3)  _s3=0,_s2=0,_s1=1,_s0=1;
    if(taget==4)  _s3=0,_s2=1,_s1=0,_s0=0;
    if(taget==5)  _s3=0,_s2=1,_s1=0,_s0=1;
    if(taget==6)  _s3=0,_s2=1,_s1=1,_s0=0;
    if(taget==7)  _s3=0,_s2=1,_s1=1,_s0=1;
    if(taget==8)  _s3=1,_s2=0,_s1=0,_s0=0;
    if(taget==9)  _s3=1,_s2=0,_s1=0,_s0=1;
    if(taget==10) _s3=1,_s2=0,_s1=1,_s0=0;
    if(taget==11) _s3=1,_s2=0,_s1=1,_s0=1;
    if(taget==12) _s3=1,_s2=1,_s1=0,_s0=0;
    if(taget==13) _s3=1,_s2=1,_s1=0,_s0=1;
    if(taget==14) _s3=1,_s2=1,_s1=1,_s0=0;
    if(taget==15) _s3=1,_s2=1,_s1=1,_s0=1;
    wait_us(5);
}//MULUTIPLEXER




// メディアンフィルタリング関数
float LoadCell::median_filter(float data[]) {
    int i, j;
    float temp;
    float sorted_data[FILTER_SIZE];
  // ソート
    for (i = 0; i < FILTER_SIZE; i++) {
        sorted_data[i] = data[i];
    }
    for (i = 0; i < FILTER_SIZE - 1; i++) {
        for (j = i + 1; j < FILTER_SIZE; j++) {
            if (sorted_data[j] < sorted_data[i]) {
                temp = sorted_data[i];
                sorted_data[i] = sorted_data[j];
                sorted_data[j] = temp;
            }
        }
    }

  // 中央値を返す
  return sorted_data[FILTER_SIZE / 2];
}
float LoadCell::now_voit(int loop){
    //FILE *sfp;
    double sum=0;    //ロードセル電圧の合計値
    //double cell_value[3000]; //セルの値(デバッグ用)
    double cell_value=0; //1個前のﾍｯﾄﾞの影響がありそうなのでひたすら書き換えてフラットに戻すための変数
    //wait_us(200);
    int a=200;//値を無視するための変数

   // MULUTIPLEXER(15);
   // for(int i=300; i; i--){ cell_value= _cell; }
   // MULUTIPLEXER(_head);

    //平均(移動)取得用にロードセル電圧取得
    for(int i=loop+a; i; i--){//100回まで前のヘッドの影響がありそうなので削除
        //cell_value[i] = _cell;
        //sum += cell_value[i];   
        if(loop+a-i<a){
            cell_value= _cell;
            //if(_head == 2){
            //    printf(":%4.6f\r\n",cell_value);
            //}
            cell_value=cell_value;
        }else{
            sum += _cell;     //デバッグ以外は必須！！
        }
        //wait_us(10);
    }
    //ロードセルの平均移動値を返す
    float fSum = float(sum/loop);
    
    //printf("1:%4.6f   ",fSum);
    //ノイズを取ってるつもりのプログラム
    /*int sum_loop=0;
    double sum2 = fSum;    //ロードセル電圧の合計値
    for(int i=loop; i; i--){
        if(cell_value[i] <= fSum + 0.000367 && cell_value[i] >= fSum -0.000367){
            sum2 += cell_value[i];
            sum_loop += 1;
        }
    }
    fSum = float(sum2/(sum_loop+1));
    //printf("2:%4.6f\r\n",fSum);
    
    
    
    */
    //↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓デバッグ用
    /*if (log_flag==1){
            if ((sfp = fopen("/local/log01.TXT","a")) == NULL) {
                fprintf(stderr, "%s write Err.\n", "log");
                exit(EXIT_FAILURE);
            }
            fprintf(sfp,"iiiiiiiii\r\n");
            for(int i=loop; i; i--){
                fprintf(sfp,"%f\r\n",cell_value[i]);
            }
            fclose(sfp);
            sfp = NULL;
    }*/
    //↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑デバッグ用
            
  
    return fSum;
}
/* ------------ ------------ ------------　------------*/
/* ------------ START SPAN調整で使用 ------------ */
//現在の電圧をfainとして取得
void LoadCell::set_fain(){
    _nowSpan.fain = now_voit(SPAN_KURI);
}
//変更済みの電圧をfainとして取得
void LoadCell::set_fainV(float fain_volt){
    _nowSpan.fain = fain_volt;
}

//現在の電圧をsainとして取得
void LoadCell::set_sain(){
    _nowSpan.sain = now_voit(SPAN_KURI);
}
//変更済みの電圧をsainとして取得
void LoadCell::set_sainV(float sain_volt ){
    _nowSpan.sain = sain_volt;
}
//変更済みの電圧をsainとして取得
void LoadCell::set_sainVs(float sain_volt , int f_s_flag){
    if(f_s_flag == 1){  _nowSpan.sain = sain_volt;  }
    if(f_s_flag == 2){  _nowSpan.sain2 = sain_volt; }
    if(f_s_flag == 3){  _nowSpan.sain3 = sain_volt; }
    if(f_s_flag == 4){  _nowSpan.sain4 = sain_volt; }
    if(f_s_flag == 5){  _nowSpan.sain5 = sain_volt; }
    if(f_s_flag == 6){  _nowSpan.sain6 = sain_volt; }
    if(f_s_flag == 7){  _nowSpan.sain7 = sain_volt; }
    if(f_s_flag == 8){  _nowSpan.sain8 = sain_volt; }
    if(f_s_flag == 9){  _nowSpan.sain9 = sain_volt; }
    if(f_s_flag == 10){  _nowSpan.sain10 = sain_volt; }
}
//現在の電圧をsainとして取得
void LoadCell::set_sain_2(){
    _nowSpan.sain2 = now_voit(SPAN_KURI);
}
//現在の電圧をsainとして取得
void LoadCell::set_sain_3(){
    _nowSpan.sain3 = now_voit(SPAN_KURI);
}
//現在の電圧をsainとして取得
void LoadCell::set_sain_4(){
    _nowSpan.sain4 = now_voit(SPAN_KURI);
}
//現在の電圧をsainとして取得
void LoadCell::set_sain_5(){
    _nowSpan.sain5 = now_voit(SPAN_KURI);
}
//現在の電圧をsainとして取得
void LoadCell::set_sain_6(){
    _nowSpan.sain6 = now_voit(SPAN_KURI);
}
//現在の電圧をsainとして取得
void LoadCell::set_sain_7(){
    _nowSpan.sain7 = now_voit(SPAN_KURI);
}
//現在の電圧をsainとして取得
void LoadCell::set_sain_8(){
    _nowSpan.sain8 = now_voit(SPAN_KURI);
}
//現在の電圧をsainとして取得
void LoadCell::set_sain_9(){
    _nowSpan.sain9 = now_voit(SPAN_KURI);
}
//現在の電圧をsainとして取得
void LoadCell::set_sain_10(){
    _nowSpan.sain10 = now_voit(SPAN_KURI);
}




//sain値を加算していく(約0.1g)
void LoadCell::plus_fain(){
    _nowSpan.sain += 0.00001;
}
//sain値を減算していく(約0.1g)
void LoadCell::minus_fain(){
    _nowSpan.sain -= 0.00001;
}

//現在(nowLoad)のfain値を返す
float LoadCell::get_now_fain(){
    return _nowSpan.fain;
}

//現在(nowLoad)のfain値を返す
float LoadCell::get_now_sain(int i){
    if(i==1){      return _nowSpan.sain;}
    else if(i==2){ return _nowSpan.sain2;}
    else if(i==3){ return _nowSpan.sain3;}
    else if(i==4){ return _nowSpan.sain4;}
    else if(i==5){ return _nowSpan.sain5;}
    else if(i==6){ return _nowSpan.sain6;}
    else if(i==7){ return _nowSpan.sain7;}
    else if(i==8){ return _nowSpan.sain8;}
    else if(i==9){ return _nowSpan.sain9;}
    else if(i==10){ return _nowSpan.sain10;}
    else         { return _nowSpan.sain10;}
}

//重量を返す(現在(nowLoad)の重量取得用)
float LoadCell::get_now_weight_span(int f_s_flag){
    if(f_s_flag == 0 || f_s_flag == 1){
        _param.hundo = span_hundo[0];
        use_span = _param.hundo;
        return                ((_pVal[_head].spanvolt - _nowSpan.fain)/(_nowSpan.sain - _nowSpan.fain)*_param.hundo);
    }else if(f_s_flag == 2){
        _param.hundo = span_hundo[1];
        use_span = _param.hundo;
        return span_hundo[0] +((_pVal[_head].spanvolt - _nowSpan.sain)/(_nowSpan.sain2-_nowSpan.sain)*(_param.hundo-span_hundo[0]));
    }else if(f_s_flag == 3){
        _param.hundo = span_hundo[2];
        use_span = _param.hundo;
        return span_hundo[1] +((_pVal[_head].spanvolt - _nowSpan.sain2)/(_nowSpan.sain3-_nowSpan.sain2)*(_param.hundo-span_hundo[1]));
    }else if(f_s_flag == 4){
        _param.hundo = span_hundo[3];
        use_span = _param.hundo;
        return span_hundo[2] +((_pVal[_head].spanvolt - _nowSpan.sain3)/(_nowSpan.sain4-_nowSpan.sain3)*(_param.hundo-span_hundo[2]));
    }else if(f_s_flag == 5){
        _param.hundo = span_hundo[4];
        use_span = _param.hundo;
        return span_hundo[3] +((_pVal[_head].spanvolt - _nowSpan.sain4)/(_nowSpan.sain5-_nowSpan.sain4)*(_param.hundo-span_hundo[3]));
    }else if(f_s_flag == 6){
        _param.hundo = span_hundo[5];
        use_span = _param.hundo;
        return span_hundo[4] +((_pVal[_head].spanvolt - _nowSpan.sain5)/(_nowSpan.sain6-_nowSpan.sain5)*(_param.hundo-span_hundo[4]));
    }else if(f_s_flag == 7){
        _param.hundo = span_hundo[6];
        use_span = _param.hundo;
        return span_hundo[5] +((_pVal[_head].spanvolt - _nowSpan.sain6)/(_nowSpan.sain7-_nowSpan.sain6)*(_param.hundo-span_hundo[5]));
    }else if(f_s_flag == 8){
        _param.hundo = span_hundo[7];
        use_span = _param.hundo;
        return span_hundo[6] +((_pVal[_head].spanvolt - _nowSpan.sain7)/(_nowSpan.sain8-_nowSpan.sain7)*(_param.hundo-span_hundo[6]));
    }else if(f_s_flag == 9){
        _param.hundo = span_hundo[8];
        use_span = _param.hundo;
        return span_hundo[7] +((_pVal[_head].spanvolt - _nowSpan.sain8)/(_nowSpan.sain9-_nowSpan.sain8)*(_param.hundo-span_hundo[7]));
    }else if(f_s_flag == 10){
        _param.hundo = span_hundo[9];
        use_span = _param.hundo;
        return span_hundo[8] +((_pVal[_head].spanvolt - _nowSpan.sain9)/(_nowSpan.sain10-_nowSpan.sain9)*(_param.hundo-span_hundo[8]));
    }else{
       _param.hundo = span_hundo[9];
        use_span = _param.hundo;
        return span_hundo[8] +((_pVal[_head].spanvolt - _nowSpan.sain9)/(_nowSpan.sain10-_nowSpan.sain9)*(_param.hundo-span_hundo[8]));
     }    
}
/*
float LoadCell::get_now_weight_span(int f_s_flag){
    if(f_s_flag == 0 || f_s_flag == 1){
        _param.hundo = span_hundo[0];
        use_span = _param.hundo;
        return ((_pVal[_head].spanvolt - _nowSpan.fain)/(_nowSpan.sain - _nowSpan.fain)*_param.hundo);
    }else if(f_s_flag == 2){
        _param.hundo = span_hundo[1];
        use_span = _param.hundo;
        return span_hundo[0] +((_pVal[_head].spanvolt - _nowSpan.sain)/(_nowSpan.sain2-_nowSpan.sain)*(_param.hundo-span_hundo[0]));
    }else if(f_s_flag == 3){
        _param.hundo = span_hundo[2];
        use_span = _param.hundo;
        return span_hundo[1] +((_pVal[_head].spanvolt - _nowSpan.sain2)/(_nowSpan.sain3-_nowSpan.sain2)*(_param.hundo-span_hundo[1]));
    }else if(f_s_flag == 4){
        _param.hundo = span_hundo[3];
        use_span = _param.hundo;
        return span_hundo[2] +((_pVal[_head].spanvolt - _nowSpan.sain3)/(_nowSpan.sain4-_nowSpan.sain3)*(_param.hundo-span_hundo[2]));
    }else if(f_s_flag == 5){
        _param.hundo = span_hundo[4];
        use_span = _param.hundo;
        return span_hundo[3] +((_pVal[_head].spanvolt - _nowSpan.sain4)/(_nowSpan.sain5-_nowSpan.sain4)*(_param.hundo-span_hundo[3]));
    }else{
        _param.hundo = span_hundo[4];
        use_span = _param.hundo;
        return span_hundo[3] +((_pVal[_head].spanvolt - _nowSpan.sain4)/(_nowSpan.sain5-_nowSpan.sain4)*(_param.hundo-span_hundo[3]));
    }    
}*/

        
//重量を返す(現在(nowLoad)の重量取得用)
float LoadCell::get_now_weight(){
    /*if(_pVal[_head].volt <= (_nowSpan.sain + _nowSpan.sain/200) || _nowSpan.sain2 < _nowSpan.sain ){
        _param.hundo = span_hundo[0];
        use_span = _param.hundo;
        return (( _pVal[_head].volt- _nowSpan.fain)/( _nowSpan.sain- _nowSpan.fain)*_param.hundo);
    }else if(_pVal[_head].volt <= (_nowSpan.sain2 + _nowSpan.sain2/200) || _nowSpan.sain3 < _nowSpan.sain2 ){
        _param.hundo = span_hundo[1];
        use_span = _param.hundo;
        return (( _pVal[_head].volt- _nowSpan.fain)/( _nowSpan.sain2- _nowSpan.fain)*_param.hundo);
    }else if(_pVal[_head].volt <= (_nowSpan.sain3 + _nowSpan.sain3/200)|| _nowSpan.sain4 < _nowSpan.sain3 ){
        _param.hundo = span_hundo[2];
        use_span = _param.hundo;
        return (( _pVal[_head].volt- _nowSpan.fain)/( _nowSpan.sain3- _nowSpan.fain)*_param.hundo);
    }else if(_pVal[_head].volt <= (_nowSpan.sain4 + _nowSpan.sain4/200) || _nowSpan.sain5 < _nowSpan.sain4 ){
        _param.hundo = span_hundo[3];
        use_span = _param.hundo;
        return (( _pVal[_head].volt- _nowSpan.fain)/( _nowSpan.sain4- _nowSpan.fain)*_param.hundo);
    }else if(_pVal[_head].volt <= (_nowSpan.sain5 + _nowSpan.sain5/200)){
        _param.hundo = span_hundo[4];
        use_span = _param.hundo;
        return (( _pVal[_head].volt- _nowSpan.fain)/( _nowSpan.sain5- _nowSpan.fain)*_param.hundo);
    }else {
        _param.hundo = span_hundo[4];
        use_span = _param.hundo;
        return (( _pVal[_head].volt- _nowSpan.fain)/( _nowSpan.sain5- _nowSpan.fain)*_param.hundo);
    }*/
    
    //if(DEBUG_MODE == TEST_Ver3T || DEBUG_MODE == 0 || 1){
        if(      _pVal[_head].volt <= (betwe_spa[_head][0]) || _nowSpan.sain2 < _nowSpan.sain  || span_hundo[1] == 0){
            _param.hundo = span_hundo[0];
            use_span = _param.hundo;
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (betwe_spa[_head][1]) || _nowSpan.sain3 < _nowSpan.sain2 || span_hundo[2] == 0){
            _param.hundo = span_hundo[1];
            use_span = _param.hundo;
            return span_hundo[0] +((_pVal[_head].volt - _nowSpan.sain)/(_nowSpan.sain2-_nowSpan.sain)*(_param.hundo-span_hundo[0]));
        }else if(_pVal[_head].volt <= (betwe_spa[_head][2]) || _nowSpan.sain4 < _nowSpan.sain3 || span_hundo[3] == 0 ){
            _param.hundo = span_hundo[2];
            use_span = _param.hundo;
            return span_hundo[1] +((_pVal[_head].volt - _nowSpan.sain2)/(_nowSpan.sain3-_nowSpan.sain2)*(_param.hundo-span_hundo[1]));
        }else if(_pVal[_head].volt <= (betwe_spa[_head][3]) || _nowSpan.sain5 < _nowSpan.sain4 || span_hundo[4] == 0 ){
            _param.hundo = span_hundo[3];
            use_span = _param.hundo;
            return span_hundo[2] +((_pVal[_head].volt - _nowSpan.sain3)/(_nowSpan.sain4-_nowSpan.sain3)*(_param.hundo-span_hundo[2]));
        }else if(_pVal[_head].volt <= (betwe_spa[_head][4]) || _nowSpan.sain6 < _nowSpan.sain5 || span_hundo[5] == 0 ){
            _param.hundo = span_hundo[4];
            use_span = _param.hundo;
            return span_hundo[3] +((_pVal[_head].volt - _nowSpan.sain4)/(_nowSpan.sain5-_nowSpan.sain4)*(_param.hundo-span_hundo[3]));
        }else if(_pVal[_head].volt <= (betwe_spa[_head][5]) || _nowSpan.sain7 < _nowSpan.sain6 || span_hundo[6] == 0 ){
            _param.hundo = span_hundo[5];
            use_span = _param.hundo;
            return span_hundo[4] +((_pVal[_head].volt - _nowSpan.sain5)/(_nowSpan.sain6-_nowSpan.sain5)*(_param.hundo-span_hundo[4]));
        }else if(_pVal[_head].volt <= (betwe_spa[_head][6]) || _nowSpan.sain8 < _nowSpan.sain7 || span_hundo[7] == 0 ){
        //}else if(_pVal[_head].volt <= (_nowSpan.sain7)){
            _param.hundo = span_hundo[6];
            use_span = _param.hundo;
            return span_hundo[5] +((_pVal[_head].volt - _nowSpan.sain6)/(_nowSpan.sain7-_nowSpan.sain6)*(_param.hundo-span_hundo[5]));
        }else if(_pVal[_head].volt <= (betwe_spa[_head][7]) || _nowSpan.sain9 < _nowSpan.sain8 || span_hundo[8] == 0 ){
        //}else if(_pVal[_head].volt <= (_nowSpan.sain8)){
            _param.hundo = span_hundo[7];
            use_span = _param.hundo;
            return span_hundo[6] +((_pVal[_head].volt - _nowSpan.sain7)/(_nowSpan.sain8-_nowSpan.sain7)*(_param.hundo-span_hundo[6]));
        }else if(_pVal[_head].volt <= (betwe_spa[_head][8]) || _nowSpan.sain10 < _nowSpan.sain9 || span_hundo[9] == 0 ){
        //}else if(_pVal[_head].volt <= (_nowSpan.sain9)){
            _param.hundo = span_hundo[8];
            use_span = _param.hundo;
            return span_hundo[7] +((_pVal[_head].volt - _nowSpan.sain8)/(_nowSpan.sain9-_nowSpan.sain8)*(_param.hundo-span_hundo[7]));
        }else if(_pVal[_head].volt <= (_nowSpan.sain10)){
            _param.hundo = span_hundo[9];
            use_span = _param.hundo;
            return span_hundo[8] +((_pVal[_head].volt - _nowSpan.sain9)/(_nowSpan.sain10-_nowSpan.sain9)*(_param.hundo-span_hundo[8]));
        }else{
            _param.hundo = span_hundo[9];
            use_span = _param.hundo;
            return span_hundo[8] +((_pVal[_head].volt - _nowSpan.sain9)/(_nowSpan.sain10-_nowSpan.sain9)*(_param.hundo-span_hundo[8]));
        }/*
    }else if(DEBUG_MODE == TEST_Ver1T){
        if(_pVal[_head].volt <= (_nowSpan.sain +  ((_nowSpan.sain2 - _nowSpan.sain)/2)) || _nowSpan.sain2 < _nowSpan.sain ){
            _param.hundo = span_hundo[0];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (_nowSpan.sain2 + ((_nowSpan.sain3 - _nowSpan.sain2)/2)) || _nowSpan.sain3 < _nowSpan.sain2 ){
            _param.hundo = span_hundo[1];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain2 -_nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (_nowSpan.sain3 + ((_nowSpan.sain4 - _nowSpan.sain3)/2)) || _nowSpan.sain4 < _nowSpan.sain3 ){
            _param.hundo = span_hundo[2];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain3 - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (_nowSpan.sain4 + ((_nowSpan.sain5 - _nowSpan.sain4)/2)) || _nowSpan.sain5 < _nowSpan.sain4 ){
            _param.hundo = span_hundo[3];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain4 - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (_nowSpan.sain5)){
            _param.hundo = span_hundo[4];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain5 - _nowSpan.fain)*_param.hundo);
        }else{
            _param.hundo = span_hundo[4];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain5 - _nowSpan.fain)*_param.hundo);
        }
    }else if(DEBUG_MODE == TEST_Ver2T){
        if(_pVal[_head].volt <= (betwe_spa2[_head][0]) || _nowSpan.sain2 < _nowSpan.sain ){
            _param.hundo = span_hundo[0];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (betwe_spa2[_head][1]) || _nowSpan.sain3 < _nowSpan.sain2 ){
            _param.hundo = span_hundo[1];
            return ((_pVal[_head].volt  - _nowSpan.fain)/(_nowSpan.sain2 - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (betwe_spa2[_head][2]) || _nowSpan.sain4 < _nowSpan.sain3 ){
            _param.hundo = span_hundo[2];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain3 - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (betwe_spa2[_head][3]) || _nowSpan.sain5 < _nowSpan.sain4 ){
            _param.hundo = span_hundo[3];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain4 - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (_nowSpan.sain5)){
            _param.hundo = span_hundo[4];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain5 - _nowSpan.fain)*_param.hundo);
        }else{
            _param.hundo = span_hundo[4];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain5 - _nowSpan.fain)*_param.hundo);
        }
    }else if(DEBUG_MODE == TEST_Ver3T){
        if(_pVal[_head].volt <= (betwe_spa[_head][0]) || _nowSpan.sain2 < _nowSpan.sain ){
            _param.hundo = span_hundo[0];
            use_span = _param.hundo;
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (betwe_spa[_head][1]) || _nowSpan.sain3 < _nowSpan.sain2 ){
            _param.hundo = span_hundo[1];
            use_span = _param.hundo;
            return span_hundo[0] +((_pVal[_head].volt - _nowSpan.sain)/(_nowSpan.sain2-_nowSpan.sain)*(_param.hundo-span_hundo[0]));
        }else if(_pVal[_head].volt <= (betwe_spa[_head][2]) || _nowSpan.sain4 < _nowSpan.sain3 ){
            _param.hundo = span_hundo[2];
            use_span = _param.hundo;
            return span_hundo[1] +((_pVal[_head].volt - _nowSpan.sain2)/(_nowSpan.sain3-_nowSpan.sain2)*(_param.hundo-span_hundo[1]));
        }else if(_pVal[_head].volt <= (betwe_spa[_head][3]) || _nowSpan.sain5 < _nowSpan.sain4 ){
            _param.hundo = span_hundo[3];
            use_span = _param.hundo;
            return span_hundo[2] +((_pVal[_head].volt - _nowSpan.sain3)/(_nowSpan.sain4-_nowSpan.sain3)*(_param.hundo-span_hundo[2]));
        }else if(_pVal[_head].volt <= (_nowSpan.sain5)){
            _param.hundo = span_hundo[4];
            use_span = _param.hundo;
            return span_hundo[3] +((_pVal[_head].volt - _nowSpan.sain4)/(_nowSpan.sain5-_nowSpan.sain4)*(_param.hundo-span_hundo[3]));
        }else{
            _param.hundo = span_hundo[4];
            use_span = _param.hundo;
            return span_hundo[3] +((_pVal[_head].volt - _nowSpan.sain4)/(_nowSpan.sain5-_nowSpan.sain4)*(_param.hundo-span_hundo[3]));
        }
    }else{
        if(_pVal[_head].volt <= (betwe_spa[_head][0]) || _nowSpan.sain2 < _nowSpan.sain ){
            _param.hundo = span_hundo[0];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (betwe_spa[_head][1]) || _nowSpan.sain3 < _nowSpan.sain2 ){
            _param.hundo = span_hundo[1];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain2 - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (betwe_spa[_head][2]) || _nowSpan.sain4 < _nowSpan.sain3 ){
            _param.hundo = span_hundo[2];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain3 - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (betwe_spa[_head][3]) || _nowSpan.sain5 < _nowSpan.sain4 ){
            _param.hundo = span_hundo[3];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain4 - _nowSpan.fain)*_param.hundo);
        }else if(_pVal[_head].volt <= (betwe_spa[_head][4])){
            _param.hundo = span_hundo[4];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain5 - _nowSpan.fain)*_param.hundo);
        }else{
            _param.hundo = span_hundo[4];
            return ((_pVal[_head].volt - _nowSpan.fain)/(_nowSpan.sain5 - _nowSpan.fain)*_param.hundo);
        }
    }*/
    /*
    if(volt <= (_pSpan[_head].sain + _pSpan[_head].sain/200) || _pSpan[_head].sain2 < _pSpan[_head].sain ){
        _param.hundo = span_hundo[0];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain-_pSpan[_head].fain)*_param.hundo);
    }else if(volt <= (_pSpan[_head].sain2 + _pSpan[_head].sain2/200) || _pSpan[_head].sain3 < _pSpan[_head].sain2 ){
        _param.hundo = span_hundo[1];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain2-_pSpan[_head].fain)*_param.hundo);
    }else if(volt <= (_pSpan[_head].sain3 + _pSpan[_head].sain3/200) || _pSpan[_head].sain4 < _pSpan[_head].sain3 ){
        _param.hundo = span_hundo[2];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain3-_pSpan[_head].fain)*_param.hundo);
    }else if(volt <= (_pSpan[_head].sain4 + _pSpan[_head].sain4/200) || _pSpan[_head].sain5 < _pSpan[_head].sain4 ){
        _param.hundo = span_hundo[3];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain4-_pSpan[_head].fain)*_param.hundo);
    }else if(volt <= (_pSpan[_head].sain5 + _pSpan[_head].sain5/200)){
        _param.hundo = span_hundo[4];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain5-_pSpan[_head].fain)*_param.hundo);
    }else{
        _param.hundo = span_hundo[4];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain5-_pSpan[_head].fain)*_param.hundo);
    }*/
        
    
}

/* ------------ END SPAN調整で使用 ------------ */
/* ------------ ------------ ------------　------------*/

void LoadCell::set_imageweight(int head, float im_weight){
    head = head - 1;
    image_weight[head] = im_weight;
    /*
    _pVal[head].vol_old = 0.35;       //前回の値を取得
    _pVal[head].volt    = 0.35;       //ロードセル値を０gの値として取得
    _pVal[head].weight = im_weight;        //重量取得
    nowweightVale[head] = _pVal[head].weight;
    oldweightVale[head] = _pVal[head].weight;
    _pVal[head].aweight = _pVal[head].weight;
    _pZero[head] = _param.BARA_KAKUNIN + 1;
    _pAutoCount[head] = 0;
    */
}

//電圧の取得後、重量安定確認、オートゼロ点取得を実行。
void LoadCell::setAin(int repeat){
    setAinSpeed(repeat);
    checkAntei();       //検出重量安定しているか
    if(auto_zero_off==false){
        //printf("AAA%2d\r\n",_head);
        checkZeroRum();     //ゼロ点を取得
    }
}
//強制的に値を0にする
void LoadCell::setAin_zero(int _head){
    _pVal[_head].vol_old = 0;       //前回の値を取得
    //_pVal[_head].vol_old1 = 0;       //前回の値を取得
    //_pVal[_head].vol_old2 = 0;       //前回の値を取得
    //_pVal[_head].volt = 0;       //ロードセル値を取得
    _pVal[_head].volt = _pSpan[_head].fain;       //ロードセル値を０gの値として取得
    _pVal[_head].weight = 0;        //重量取得
    //_pVal[_head].weight_old1 = 0;        //重量取得
    //_pVal[_head].weight_old2 = 0;        //重量取得
    
    nowweightVale[_head] = 0;
    
    ArraySort(_pVal[_head].wVolt, WHT_CNT);       //配列の順番を入れ替え
    _pVal[_head].wVolt[0] = 0;      //平均計算用の配列に電圧取得
}
//repeatの回数ロードセル電圧取得。
//必要な変数に値を代入。
/*float VAL_vol_old  = 0.0;
float VAL_vol_now  = 0.0;
float VAL_weight =0.0;*/
float LoadCell::diff_gram(/*int repeat*/){
    //setAinSpeed(repeat);
    return ask_H_Weight(_pVal[_head].volt) - ask_H_Weight(_pVal[_head].vol_old1);
}

void LoadCell::setAinSpeed(int repeat){
    if(image_weight[_head] > 0){
        //set_imageweight(_head+1,image_weight[_head]);
        _pVal[_head].vol_old = _pVal[_head].volt;       //前回の値を取得
        _pVal[_head].volt    = 0.35;       //ロードセル値を０gの値として取得
        _pVal[_head].weight_old2 = _pVal[_head].weight_old1;
        _pVal[_head].weight_old1 = _pVal[_head].weight;
        _pVal[_head].weight = image_weight[_head];        //重量取得
        nowweightVale[_head] = _pVal[_head].weight;
        oldweightVale[_head] = _pVal[_head].weight;
        //_pVal[_head].aweight = _pVal[_head].weight;
        ArraySort(_pVal[_head].wVolt, WHT_CNT);       //配列の順番を入れ替え
        _pVal[_head].wVolt[0] = _pVal[_head].volt;      //平均計算用の配列に電

    }else{

        _pVal[_head].vol_old = _pVal[_head].volt;       //前回の値を取得
        _pVal[_head].volt = now_voit(repeat);       //ロードセル値を取得
        _pVal[_head].weight_old2 = _pVal[_head].weight_old1;
        _pVal[_head].weight_old1 = _pVal[_head].weight;
        _pVal[_head].weight  = askWeight(_pVal[_head].volt);//重量取得
        //printf("  %5.1f  ", _pVal[_head].weight );
        //nowweightVale[_head] = askWeight(_pVal[_head].volt);
        nowweightVale[_head] = _pVal[_head].weight;
        /*
        VAL_weight  = askWeight(_pVal[_head].volt);
        VAL_vol_old = VAL_vol_now;;
        VAL_vol_old = VAL_vol_now;
        VAL_vol_now = _pVal[_head].volt;
        */    
        /*
        if(_pVal[_head].weight > _param.ZEROAREA && 
            (-3.0 <= (askWeight(_pVal[_head].vol_old) - _pVal[_head].weight) && 
            (askWeight(_pVal[_head].vol_old) - _pVal[_head].weight) <= 3.0)){
            nowweightVale[_head] = (askWeight(_pVal[_head].vol_old) * keiryou_kurikaesi + nowweightVale[_head])/(keiryou_
            
            
            
            
            
            
            kurikaesi + 1);
            _pVal[_head].weight = nowweightVale[_head];
            _pVal[_head].volt = (_pVal[_head].vol_old * keiryou_kurikaesi + _pVal[_head].volt)/(keiryou_kurikaesi + 1);
        }*/

        ArraySort(_pVal[_head].wVolt, WHT_CNT);       //配列の順番を入れ替え
        _pVal[_head].wVolt[0] = _pVal[_head].volt;      //平均計算用の配列に電圧取得

        //printf("%2d %8.2f\r\n",_head,_param.BARATSUKI);
    }
}

//平均算出用の配列に初期値として現在のロードセル電圧を代入
//bitでビッチ1が立っている場所の処理を行う
void LoadCell::setAinSpeedDummy(int bit){
    for(int head = 0; bit; bit >>= 1){
        if(bit & 0x01){
            for(int i = 0; i < WHT_CNT; i++){
                _pVal[head].wVolt[i] = _pVal[head].volt;
            }
        }
        head++;
    }
}

/* ------------ ------------ ------------　------------*/
/* ------------ START 配列を使用した処理群 ------------ */
//順番を入れ替える(要素0に最新,最後に古いデータとなる)
void LoadCell::ArraySort(float f[], int num){
    //入れ替え
    for(int i = num; i >= 2; i--){
        f[i-1] = f[i-2];
    }
    //0番目は初期化しておく
    f[0] = 0.0;
}

//各配列の値が規定の差分内かを確認する
bool LoadCell::ArrayCheck(){
    float sabun;
    
    for(int i = 0; i < WHT_CNT-1; i++){
        sabun = _pVal[_head].weight - askWeight(_pVal[_head].wVolt[i]);
        
        if(sabun > SEIDO_UP || sabun < -SEIDO_UP){
            return false;
        }
    }
    return true;
}

//配列に格納されている値の平均値を返す
float LoadCell::ArrayHeikin(float f[], int num){
    float damy = 0; //一時取得用
    
    //配列の値を全て取得
    for(int i = 0; i < num; i++){
        damy += askWeight(f[i]);
    }
    
    return damy / num;
}

//配列に格納した平均電圧を取得。平均電圧から重量を取得する
float LoadCell::getArrayWeight(){
    return ArrayHeikin(_pVal[_head].wVolt, WHT_CNT);
}

//配列要素数を指定。指定した分の平均を取得
float LoadCell::getArrayWeight(int setNum){
    return ArrayHeikin(_pVal[_head].wVolt, setNum);
}

/* ------------ END 配列を使用した処理群 ------------ */
/* ------------ ------------ ------------　------------*/
void LoadCell::set_ini(){
    for(int k = 0; k < _numHead; k++){
        set_head(k);
        //念のため各取得電圧値を初期化
        //_pVal[_head].volt =0.0;
        //for(int i = 0; i < WHT_CNT; i++){
        //    _pVal[_head].wVolt[i] = 0.0;
        //}
        
        //風袋引き用電圧取得
        //_pVal[_head].hVolt = now_voit(SPAN_KURI);

        //取得した風袋重量を元に、平均風袋重量を取得する。
        _pVal[_head].hweight = ask_H_Weight(_pVal[_head].hVolt);
        _pZero[_head] = 0;
        
    }
    
}

/*****************************************************************************
    風袋用の値を強制的に取得
*****************************************************************************/
void LoadCell::setIniHWeight(){
    for(int k = 0; k < _numHead; k++){
        set_head(k);
        //念のため各取得電圧値を初期化
        _pVal[_head].volt =0.0;
        for(int i = 0; i < WHT_CNT; i++){
            _pVal[_head].wVolt[i] = 0.0;
        }
        
        _pZero[_head] = 0;
        //風袋引き用電圧取得
        _pVal[_head].hVolt = now_voit(SPAN_KURI);

        //取得した風袋重量を元に、平均風袋重量を取得する。
        _pVal[_head].hweight = ask_H_Weight(_pVal[_head].hVolt);
    }
}

void LoadCell::setIniHWeight_one(int a){
    set_head(a);
    _pVal[_head].volt =0.0;
    for(int i = 0; i < WHT_CNT; i++){
        _pVal[_head].wVolt[i] = 0.0;
    }
    _pZero[_head] = 0;
    //風袋引き用電圧取得
    _pVal[_head].hVolt = now_voit(SPAN_KURI);
    //取得した風袋重量を元に、平均風袋重量を取得する。
    _pVal[_head].hweight = ask_H_Weight(_pVal[_head].hVolt);

}
void LoadCell::setIniH_one(){
    _pVal[_head].hVolt = _pVal[_head].volt;
    _pVal[_head].hweight = ask_H_Weight(_pVal[_head].hVolt);
}

void LoadCell::setInitial_onehead(int k){
    set_head(k);
    _pZero[_head]            = 0;         //風袋引き用電圧取得
    _pVal[_head].volt        = 0.0;        //ロードセル値を取得
    _pVal[_head].vol_old1    = 0.0;        //前回の値を取得
    _pVal[_head].vol_old2    = 0.0;        //前前回の値を取得
    _pVal[_head].weight_old2 = 0.0;
    _pVal[_head].weight_old1 = 0.0;
    _pVal[_head].weight      = 0.0;        //重量取得
    for(int i = 0; i < WHT_CNT; i++){
        _pVal[_head].wVolt[i]    = 0.0;
    }
    _pVal[_head].hVolt       = 0.0;        //風袋引き電圧
    _pVal[_head].hweight     = 0.0;        //風袋引きの重量
    _pVal[_head].aweight     = 0.0;        //安定した重量
    _pVal[_head].spanvolt    = 0.0;        //ロードセルから取得した電圧値
    nowweightVale[_head]     = 0.0;
}

//Fainの値を取得
float LoadCell::get_fain(){
    return _pSpan[_head].fain;
}

//Sainの値を取得
float LoadCell::get_sain(){
    return _pSpan[_head].sain;
}

//取得しているロードセル電圧を返す
float LoadCell::getAin(){
    return _pVal[_head].volt;
}
//取得しているロードセル電圧を返す
float LoadCell::getAin_now(int head){
    return _pVal[head].volt;
}
float LoadCell::getAin_old(int head){
    return _pVal[head].vol_old1;
}

//受けとった電圧を元に重量を返す(風袋込)
float LoadCell::askWeight(float volt){
    /*
    printf("1|v=%4.3f|%4d|f=%4.3f|s=%4.3f|\r\n ",volt ,_param.hundo,_pSpan[_head].fain,_pSpan[_head].sain);
    printf("2|v=%4.3f|%4d|f=%4.3f|s=%4.3f|\r\n ",volt ,_param.hundo,_pSpan[_head].fain,_pSpan[_head].sain2);
    printf("3|v=%4.3f|%4d|f=%4.3f|s=%4.3f|\r\n ",volt ,_param.hundo,_pSpan[_head].fain,_pSpan[_head].sain3);
    printf("4|v=%4.3f|%4d|f=%4.3f|s=%4.3f|\r\n ",volt ,_param.hundo,_pSpan[_head].fain,_pSpan[_head].sain4);
    printf("5|v=%4.3f|%4d|f=%4.3f|s=%4.3f|\r\n ",volt ,_param.hundo,_pSpan[_head].fain,_pSpan[_head].sain5);
    printf("6|v=%4.3f|%4d|f=%4.3f|s=%4.3f|\r\n ",volt ,_param.hundo,_pSpan[_head].fain,_pSpan[_head].sain5);
    */
    if(volt <= (_pSpan[_head].sain) || _pSpan[_head].sain2 < _pSpan[_head].sain ){
        //printf("1\r\n");
        _param.hundo = span_hundo[0];
        use_span = _param.hundo;
        //printf("0 %d: %5.1f\r\n",_head,((volt - _pSpan[_head].fain)/(_pSpan[_head].sain-_pSpan[_head].fain)*_param.hundo)-_pVal[_head].hweight);
        //_pVal[_head].hweight =   ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain-_pSpan[_head].fain)*_param.hundo)-_pVal[_head].hweight;
        return                  ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain-_pSpan[_head].fain)*_param.hundo)-_pVal[_head].hweight;
                    
    }else if(volt <= (_pSpan[_head].sain2) || _pSpan[_head].sain3 < _pSpan[_head].sain2 ){
        //printf("2\r\n");
        _param.hundo = span_hundo[1];
        use_span = _param.hundo;
        //printf("1 %d: %5.1f\r\n",_head,span_hundo[0] +((volt - _pSpan[_head].sain)/(_pSpan[_head].sain2-_pSpan[_head].sain)*(_param.hundo-span_hundo[0]))-_pVal[_head].hweight);
        //_pVal[_head].hweight =   span_hundo[0] +((volt - _pSpan[_head].sain)/(_pSpan[_head].sain2-_pSpan[_head].sain)*(_param.hundo-span_hundo[0]))-_pVal[_head].hweight;
        return                  span_hundo[0] +((volt - _pSpan[_head].sain)/(_pSpan[_head].sain2-_pSpan[_head].sain)*(_param.hundo-span_hundo[0]))-_pVal[_head].hweight;
    }else if(volt <= (_pSpan[_head].sain3) || _pSpan[_head].sain4 < _pSpan[_head].sain3 ){
        //printf("3\r\n");
        _param.hundo = span_hundo[2];
        use_span = _param.hundo;
        //printf("2 %d: %5.1f\r\n",_head,span_hundo[1] +((volt - _pSpan[_head].sain2)/(_pSpan[_head].sain3-_pSpan[_head].sain2)*(_param.hundo-span_hundo[1]))-_pVal[_head].hweight);
        //_pVal[_head].hweight =   span_hundo[1] +((volt - _pSpan[_head].sain2)/(_pSpan[_head].sain3-_pSpan[_head].sain2)*(_param.hundo-span_hundo[1]))-_pVal[_head].hweight;
        return                  span_hundo[1] +((volt - _pSpan[_head].sain2)/(_pSpan[_head].sain3-_pSpan[_head].sain2)*(_param.hundo-span_hundo[1]))-_pVal[_head].hweight;
    }else if(volt <= (_pSpan[_head].sain4) || _pSpan[_head].sain5 < _pSpan[_head].sain4 ){
        //printf("4\r\n");
        _param.hundo = span_hundo[3];
        use_span = _param.hundo;
        //printf("3 %d: %5.1f\r\n",_head,span_hundo[2] +((volt - _pSpan[_head].sain3)/(_pSpan[_head].sain4-_pSpan[_head].sain3)*(_param.hundo-span_hundo[2]))-_pVal[_head].hweight);
        //_pVal[_head].hweight =   span_hundo[2] +((volt - _pSpan[_head].sain3)/(_pSpan[_head].sain4-_pSpan[_head].sain3)*(_param.hundo-span_hundo[2]))-_pVal[_head].hweight;
        return                  span_hundo[2] +((volt - _pSpan[_head].sain3)/(_pSpan[_head].sain4-_pSpan[_head].sain3)*(_param.hundo-span_hundo[2]))-_pVal[_head].hweight;
     }else if(volt <= (_pSpan[_head].sain5) || _pSpan[_head].sain6 < _pSpan[_head].sain5 ){
        //printf("5\r\n");
        _param.hundo = span_hundo[4];
        use_span = _param.hundo;
        //printf("4 %d: %5.1f\r\n",_head,span_hundo[3] +((volt - _pSpan[_head].sain4)/(_pSpan[_head].sain5-_pSpan[_head].sain4)*(_param.hundo-span_hundo[3]))-_pVal[_head].hweight);
        //_pVal[_head].hweight =   span_hundo[3] +((volt - _pSpan[_head].sain4)/(_pSpan[_head].sain5-_pSpan[_head].sain4)*(_param.hundo-span_hundo[3]))-_pVal[_head].hweight;
        return                  span_hundo[3] +((volt - _pSpan[_head].sain4)/(_pSpan[_head].sain5-_pSpan[_head].sain4)*(_param.hundo-span_hundo[3]))-_pVal[_head].hweight;
    }else if(volt <= (_pSpan[_head].sain6) || _pSpan[_head].sain7 < _pSpan[_head].sain6 ){
        _param.hundo = span_hundo[5];
        use_span = _param.hundo;
        return                  span_hundo[4] +((volt - _pSpan[_head].sain5)/(_pSpan[_head].sain6-_pSpan[_head].sain5)*(_param.hundo-span_hundo[4]))-_pVal[_head].hweight;
    }else if(volt <= (_pSpan[_head].sain7) || _pSpan[_head].sain8 < _pSpan[_head].sain7 ){
        _param.hundo = span_hundo[6];
        use_span = _param.hundo;
        return                  span_hundo[5] +((volt - _pSpan[_head].sain6)/(_pSpan[_head].sain7-_pSpan[_head].sain6)*(_param.hundo-span_hundo[5]))-_pVal[_head].hweight;
    }else if(volt <= (_pSpan[_head].sain8) || _pSpan[_head].sain9 < _pSpan[_head].sain8 ){
        _param.hundo = span_hundo[7];
        use_span = _param.hundo;
        return                  span_hundo[6] +((volt - _pSpan[_head].sain7)/(_pSpan[_head].sain8-_pSpan[_head].sain7)*(_param.hundo-span_hundo[6]))-_pVal[_head].hweight;
    }else if(volt <= (_pSpan[_head].sain9) || _pSpan[_head].sain10 < _pSpan[_head].sain9 ){
        _param.hundo = span_hundo[8];
        use_span = _param.hundo;
        return                  span_hundo[7] +((volt - _pSpan[_head].sain8)/(_pSpan[_head].sain9-_pSpan[_head].sain8)*(_param.hundo-span_hundo[7]))-_pVal[_head].hweight;
    }else if(volt <= (_pSpan[_head].sain10)){
        _param.hundo = span_hundo[9];
        use_span = _param.hundo;
        return                  span_hundo[8] +((volt - _pSpan[_head].sain9)/(_pSpan[_head].sain10-_pSpan[_head].sain9)*(_param.hundo-span_hundo[8]))-_pVal[_head].hweight;

    }else{
        _param.hundo = span_hundo[9];
        use_span = _param.hundo;
        return                  span_hundo[8] +((volt - _pSpan[_head].sain9)/(_pSpan[_head].sain10-_pSpan[_head].sain9)*(_param.hundo-span_hundo[8]))-_pVal[_head].hweight;


        //printf("6\r\n");
        //_param.hundo = span_hundo[4];
        //use_span = _param.hundo;
        //printf("5 %d: %5.1f\r\n",_head,span_hundo[3] +((volt - _pSpan[_head].sain4)/(_pSpan[_head].sain5-_pSpan[_head].sain4)*(_param.hundo-span_hundo[3]))-_pVal[_head].hweight);
      //_pVal[_head].hweight =   span_hundo[3] +((volt - _pSpan[_head].sain4)/(_pSpan[_head].sain5-_pSpan[_head].sain4)*(_param.hundo-span_hundo[3]))-_pVal[_head].hweight;
        //return                  span_hundo[3] +((volt - _pSpan[_head].sain4)/(_pSpan[_head].sain5-_pSpan[_head].sain4)*(_param.hundo-span_hundo[3]))-_pVal[_head].hweight;
    }
    
}
//受けとった電圧を元に重量を返す(風袋引き無し)
float LoadCell::ask_H_Weight(float volt){
    
    if(volt <= (_pSpan[_head].sain) || _pSpan[_head].sain2 < _pSpan[_head].sain ){
        //printf("1\r\n");
        _param.hundo = span_hundo[0];
        use_span = _param.hundo;
        return                  ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain-_pSpan[_head].fain)*_param.hundo);
                    
    }else if(volt <= (_pSpan[_head].sain2) || _pSpan[_head].sain3 < _pSpan[_head].sain2 ){
        //printf("2\r\n");
        _param.hundo = span_hundo[1];
        use_span = _param.hundo;
        return                  span_hundo[0] +((volt - _pSpan[_head].sain)/(_pSpan[_head].sain2-_pSpan[_head].sain)*(_param.hundo-span_hundo[0]));
    }else if(volt <= (_pSpan[_head].sain3) || _pSpan[_head].sain4 < _pSpan[_head].sain3 ){
        //printf("3\r\n");
        _param.hundo = span_hundo[2];
        use_span = _param.hundo;
        return                  span_hundo[1] +((volt - _pSpan[_head].sain2)/(_pSpan[_head].sain3-_pSpan[_head].sain2)*(_param.hundo-span_hundo[1]));
    }else if(volt <= (_pSpan[_head].sain4) || _pSpan[_head].sain5 < _pSpan[_head].sain4 ){
        //printf("4\r\n");
        _param.hundo = span_hundo[3];
        use_span = _param.hundo;
        return                  span_hundo[2] +((volt - _pSpan[_head].sain3)/(_pSpan[_head].sain4-_pSpan[_head].sain3)*(_param.hundo-span_hundo[2]));
    }else if(volt <= (_pSpan[_head].sain5) || _pSpan[_head].sain6 < _pSpan[_head].sain5 ){
        //printf("5\r\n");
        _param.hundo = span_hundo[4];
        use_span = _param.hundo;
        return                  span_hundo[3] +((volt - _pSpan[_head].sain4)/(_pSpan[_head].sain5-_pSpan[_head].sain4)*(_param.hundo-span_hundo[3]));

    }else if(volt <= (_pSpan[_head].sain6) || _pSpan[_head].sain7 < _pSpan[_head].sain6 ){
        //printf("5\r\n");
        _param.hundo = span_hundo[5];
        use_span = _param.hundo;
        return                  span_hundo[4] +((volt - _pSpan[_head].sain5)/(_pSpan[_head].sain6-_pSpan[_head].sain5)*(_param.hundo-span_hundo[4]));
    }else if(volt <= (_pSpan[_head].sain7) || _pSpan[_head].sain8 < _pSpan[_head].sain7 ){
        //printf("5\r\n");
        _param.hundo = span_hundo[6];
        use_span = _param.hundo;
        return                  span_hundo[5] +((volt - _pSpan[_head].sain6)/(_pSpan[_head].sain7-_pSpan[_head].sain6)*(_param.hundo-span_hundo[5]));
    }else if(volt <= (_pSpan[_head].sain8) || _pSpan[_head].sain9 < _pSpan[_head].sain8 ){
        //printf("5\r\n");
        _param.hundo = span_hundo[7];
        use_span = _param.hundo;
        return                  span_hundo[6] +((volt - _pSpan[_head].sain7)/(_pSpan[_head].sain8-_pSpan[_head].sain7)*(_param.hundo-span_hundo[6]));
    }else if(volt <= (_pSpan[_head].sain9) || _pSpan[_head].sain10 < _pSpan[_head].sain9 ){
        //printf("5\r\n");
        _param.hundo = span_hundo[8];
        use_span = _param.hundo;
        return                  span_hundo[7] +((volt - _pSpan[_head].sain8)/(_pSpan[_head].sain9-_pSpan[_head].sain8)*(_param.hundo-span_hundo[7]));
    }else if(volt <= (_pSpan[_head].sain10)){
        //printf("5\r\n");
        _param.hundo = span_hundo[9];
        use_span = _param.hundo;
        return                  span_hundo[8] +((volt - _pSpan[_head].sain9)/(_pSpan[_head].sain10-_pSpan[_head].sain9)*(_param.hundo-span_hundo[8]));
 
    }else{
       //printf("5\r\n");
        _param.hundo = span_hundo[9];
        use_span = _param.hundo;
        return                  span_hundo[8] +((volt - _pSpan[_head].sain9)/(_pSpan[_head].sain10-_pSpan[_head].sain9)*(_param.hundo-span_hundo[8]));
    }
    /*
    else if(DEBUG_MODE == TEST_Ver2T){
        if(volt <= (betwe_spa2[_head][0]) || _pSpan[_head].sain2 < _pSpan[_head].sain ){
            //printf("1\r\n");
            _param.hundo = span_hundo[0];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain-_pSpan[_head].fain)*_param.hundo);
        }else if(volt <= (betwe_spa2[_head][1]) || _pSpan[_head].sain3 < _pSpan[_head].sain2 ){
            //printf("2\r\n");
            _param.hundo = span_hundo[1];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain2-_pSpan[_head].fain)*_param.hundo);
        }else if(volt <= (betwe_spa2[_head][2]) || _pSpan[_head].sain4 < _pSpan[_head].sain3 ){
            //printf("3\r\n");
            _param.hundo = span_hundo[2];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain3-_pSpan[_head].fain)*_param.hundo);
        }else if(volt <= (betwe_spa2[_head][3]) || _pSpan[_head].sain5 < _pSpan[_head].sain4 ){
            //printf("4\r\n");
            _param.hundo = span_hundo[3];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain4-_pSpan[_head].fain)*_param.hundo);
        }else if(volt <= (betwe_spa2[_head][4])){
            //printf("5\r\n");
            _param.hundo = span_hundo[4];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain5-_pSpan[_head].fain)*_param.hundo);
        }else{//printf("6\r\n");
            _param.hundo = span_hundo[4];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain5-_pSpan[_head].fain)*_param.hundo);
        }
    }else if(DEBUG_MODE == TEST_Ver3T){
        if(volt <= (betwe_spa[_head][0]) || _pSpan[_head].sain2 < _pSpan[_head].sain ){
            //printf("1\r\n");
            _param.hundo = span_hundo[0];
            use_span = _param.hundo;
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain-_pSpan[_head].fain)*_param.hundo);
        }else if(volt <= (betwe_spa[_head][1]) || _pSpan[_head].sain3 < _pSpan[_head].sain2 ){
            //printf("2\r\n");
            _param.hundo = span_hundo[1];
            use_span = _param.hundo;
            return span_hundo[0] +((volt - _pSpan[_head].sain)/(_pSpan[_head].sain2-_pSpan[_head].sain)*(_param.hundo-span_hundo[0]));
        }else if(volt <= (betwe_spa[_head][2]) || _pSpan[_head].sain4 < _pSpan[_head].sain3 ){
            //printf("3\r\n");
            _param.hundo = span_hundo[2];
            use_span = _param.hundo;
            return span_hundo[1] +((volt - _pSpan[_head].sain2)/(_pSpan[_head].sain3-_pSpan[_head].sain2)*(_param.hundo-span_hundo[1]));
        }else if(volt <= (betwe_spa[_head][3]) || _pSpan[_head].sain5 < _pSpan[_head].sain4 ){
            //printf("4\r\n");
            _param.hundo = span_hundo[3];
            use_span = _param.hundo;
            return span_hundo[2] +((volt - _pSpan[_head].sain3)/(_pSpan[_head].sain4-_pSpan[_head].sain3)*(_param.hundo-span_hundo[2]));
        }else if(volt <= (betwe_spa[_head][4])){
            //printf("5\r\n");
            _param.hundo = span_hundo[4];
            use_span = _param.hundo;
            return span_hundo[3] +((volt - _pSpan[_head].sain4)/(_pSpan[_head].sain5-_pSpan[_head].sain4)*(_param.hundo-span_hundo[3]));
        }else{
            //printf("6\r\n");
            _param.hundo = span_hundo[4];
            use_span = _param.hundo;
            return span_hundo[3] +((volt - _pSpan[_head].sain4)/(_pSpan[_head].sain5-_pSpan[_head].sain4)*(_param.hundo-span_hundo[3]));
        }
    }else{
        //printf("%6.5f  %6.5f  %6.1f\r",_pSpan[_head].sain,_pSpan[_head].fain,_pVal[_head].hweight);_pSpan[_head].sain
        if(volt <= (betwe_spa[_head][0]) || _pSpan[_head].sain2 < _pSpan[_head].sain ){
            //printf("1\r\n");
            _param.hundo = span_hundo[0];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain-_pSpan[_head].fain)*_param.hundo);
        }else if(volt <= (betwe_spa[_head][1]) || _pSpan[_head].sain3 < _pSpan[_head].sain2 ){
            //printf("2\r\n");
            _param.hundo = span_hundo[1];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain2-_pSpan[_head].fain)*_param.hundo);
        }else if(volt <= (betwe_spa[_head][2]) || _pSpan[_head].sain4 < _pSpan[_head].sain3 ){
            //printf("3\r\n");
            _param.hundo = span_hundo[2];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain3-_pSpan[_head].fain)*_param.hundo);
        }else if(volt <= (betwe_spa[_head][3]) || _pSpan[_head].sain5 < _pSpan[_head].sain4 ){
            //printf("4\r\n");
            _param.hundo = span_hundo[3];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain4-_pSpan[_head].fain)*_param.hundo);
        }else if(volt <= (betwe_spa[_head][4])){
            //printf("5\r\n");
            _param.hundo = span_hundo[4];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain5-_pSpan[_head].fain)*_param.hundo);
        }else{//printf("6\r\n");
            _param.hundo = span_hundo[4];
            return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain5-_pSpan[_head].fain)*_param.hundo);
        }
    }*/
    /*
    if(volt <= (_pSpan[_head].sain + _pSpan[_head].sain/200) || _pSpan[_head].sain2 < _pSpan[_head].sain ){
        _param.hundo = span_hundo[0];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain-_pSpan[_head].fain)*_param.hundo);
    }else if(volt <= (_pSpan[_head].sain2 + _pSpan[_head].sain2/200) || _pSpan[_head].sain3 < _pSpan[_head].sain2 ){
        _param.hundo = span_hundo[1];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain2-_pSpan[_head].fain)*_param.hundo);
    }else if(volt <= (_pSpan[_head].sain3 + _pSpan[_head].sain3/200) || _pSpan[_head].sain4 < _pSpan[_head].sain3 ){
        _param.hundo = span_hundo[2];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain3-_pSpan[_head].fain)*_param.hundo);
    }else if(volt <= (_pSpan[_head].sain4 + _pSpan[_head].sain4/200) || _pSpan[_head].sain5 < _pSpan[_head].sain4 ){
        _param.hundo = span_hundo[3];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain4-_pSpan[_head].fain)*_param.hundo);
    }else if(volt <= (_pSpan[_head].sain5 + _pSpan[_head].sain5/200)){
        _param.hundo = span_hundo[4];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain5-_pSpan[_head].fain)*_param.hundo);
    }else{
        _param.hundo = span_hundo[4];
        return ((volt - _pSpan[_head].fain)/(_pSpan[_head].sain5-_pSpan[_head].fain)*_param.hundo);
    }*/
}
//風袋引きした重量を返す
float LoadCell::getWeight(int head){
    return _pVal[head].weight;
}
//風袋引きした重量を返す
float LoadCell::get_lastheadWeight(){
    return _pVal[_head].weight;
}

int LoadCell::get_lasthead(){
    return _head;
}
/*
float LoadCell::getWeight2(int head){
    return VAL_weight;
}*/
void LoadCell::break_antei(int head){
    _pZero[head] = 0;
}



//検出重量が安定している重量を返す
float LoadCell::anteiWeight(int head){
    if(_pZero[head] < _param.BARA_KAKUNIN){
        return 0.0;
    }
    else if(_pZero[head] >= _param.BARA_KAKUNIN){
        return _pVal[head].aweight;
    }
    return 0.0;
}

//検出重量が安定している重量を返す
float LoadCell::anteiWeight_rank(int head){
    if(_pZero[head] < 5){
        return 0.0;
    }
    else if(_pZero[head] >= 5){
        if(_pVal[head].weight > oldweightVale[head]){
            _pVal[head].weight = oldweightVale[head];
            nowweightVale[head] = oldweightVale[head];
        }
        return _pVal[head].weight;
    }
    return 0.0;
}

//検出重量が安定している重量を返す
float LoadCell::anteiWeight_rank3(int head){
    if(_pZero[head] < 3){
        return 0.0;
    }
    else if(_pZero[head] >= 3){
        if(_pVal[head].weight > oldweightVale[head]){
            _pVal[head].weight = oldweightVale[head];
            nowweightVale[head] = oldweightVale[head];
        }
        return _pVal[head].weight;
    }
    return 0.0;
}

//重量を返す(風袋のみ)
float LoadCell::get_HWeight(int head){
    //return _pVal[head].hweight-_pVal[head].weight
    return _pVal[head].hweight;
}


/*****************************************************************************
    前回取得重量と差がある場合
    規定重量(CUT)より軽い場合には
    重量をゼロで返す
*****************************************************************************/
void LoadCell::antei_reset(){
    _pZero[_head] = 0;
    _pVal[_head].aweight = 0;
}

void LoadCell::checkAntei(){
    //前回の重量と現在の重量の差
    //float sabun = _pVal[_head].weight - askWeight(_pVal[_head].vol_old);
    //float sabun = _pVal[_head].weight - askWeight(_pVal[_head].vol_old);
    float sabun = _pVal[_head].weight - _pVal[_head].weight_old1;
    float sabun2 = _pVal[_head].weight - _pVal[_head].weight_old2;
    float sabun3 = _pVal[_head].weight_old2 - _pVal[_head].weight_old1;
    /*if(_head < 2){
        printf("%02dH_sabun:%5.1f:%5.1f:%5.1f:%5.1f\r\n",_head,sabun,sabun2,sabun3,_pVal[_head].aweight);
        printf("%02dH_pZero:%2d\r\n",_head,_pZero[_head]);
    }*/

    //測定重量が安定していなかったら、フラグを0

    if(_pVal[_head].aweight == 0 || _option.fix_value == 0){
        if(-_param.BARATSUKI >= sabun && sabun >=  _param.BARATSUKI){
            //BARATSUKI以上だったらリセット
            printf("---Q1---");
            antei_reset();
            return;
        }else if(_pZero[_head] >= 1 &&
           (-_param.BARATSUKI >= sabun2 || 
            sabun2 >=  _param.BARATSUKI ||
            -_param.BARATSUKI >= sabun3 || 
            sabun3 >=  _param.BARATSUKI)){
            //BARATSUKI以上だったらリセット
            printf("---Q2---");
            antei_reset();
            return;
        }else if(int(_pVal[_head].weight) <= _param.CUT){
            //規定重量を超えていないため、フラグを0
            printf("---Q3---");
            antei_reset();
            return;
        }else{
            printf("---Q4---");
            //BARATSUKI以内でCUT以上なのでカウントをプラス
            if(_pZero[_head] <= 10){
                _pZero[_head] += 1;
             }
            if(_option.fix_value == 1){
            printf("---Q5---");
                if(_pZero[_head] >= _param.BARA_KAKUNIN && 
                   _pVal[_head].aweight == 0){  
            printf("---Q51--");
                       //param以上繰り返して安定値がまだ０だったら値を入れる
                       //(０じゃないならその値のままになる)
                    _pVal[_head].aweight = _pVal[_head].weight;
                }
            }else if(_option.fix_value == 0){
            printf("---Q6---");
                if(_pZero[_head] >= _param.BARA_KAKUNIN){
            printf("---Q61--");
                    _pVal[_head].aweight = _pVal[_head].weight;
                }
            }
        }

    }else if(_pVal[_head].aweight > 0 && _option.fix_value == 1){
        if(-_param.MINI_target < sabun && sabun < _param.MINI_target){
            printf("---Q7---");

        }else{
            if(sabun >= _param.BARATSUKI || sabun <= -_param.BARATSUKI){
            printf("---Q8---");
                antei_reset();
                return;
            }
        }
        if(int(_pVal[_head].weight) <= _param.CUT){
            printf("---Q9---");
            antei_reset();
            return;
        }
    }else{//ここに来るはずはないけど念の為
        antei_reset();
        return;
    }
    
/*
        //規定重量を超えていないため、フラグを0
    if(int(_pVal[_head].weight) <= _param.CUT){
        antei_reset();
    }else if(_pVal[_head].aweight > 0){
        if(-_param.MINI_target < sabun && sabun < _param.MINI_target){
            //差がCUT以下だったら変更しない

        }else{
            antei_reset();
        }



    }else{
        if(_pZero[_head] >= _param.BARA_KAKUNIN && _pVal[_head].aweight == 0){
            _pVal[_head].aweight = _pVal[_head].weight;
        }
        if(-_param.BARATSUKI <= sabun && sabun <=  _param.BARATSUKI ){
                if(_pZero[_head] == _param.BARA_KAKUNIN){
                    _pVal[_head].aweight = (askWeight(_pVal[_head].vol_old) + _pVal[_head].weight)/2;
                }else{
                    //_pVal[_head].aweight;

                }
        }else{
            _pVal[_head].aweight = 0;
        }
        //測定重量が安定していなかったら、フラグを0
    }
*/
    
}

/*****************************************************************************
   CUT以下だったら物が載っていないと判断
   false :計量物が載っている
   true  :計量物が載っていない
*****************************************************************************/
bool LoadCell::checkOut(){
    
    //ライト版のoff条件値が入力なければ通常処理
    if( _param.liteLedOff == 0){
        //平均重量がカット以下だったら物が載っていない
        if(getArrayWeight() <= _param.CUT_Y){
            return true;
        }
    }else{
        //ライト版のオフ条件値があれば、判断はこちら
        if(getArrayWeight() < _param.liteLedOff){
            return true;
        }
    }
    //ここまで来たら物体が載っていると判断
    return false;
}
/*****************************************************************************
    物が載っていると判断する為の値
*****************************************************************************/
int cel_status[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
void LoadCell::set_cel_status(int head,int now){
    for(int i=0;i< head;i++){
        if((now & (1<<i))>0){
            cel_status[i]=1;
        }else{
            cel_status[i]=0;
        }
    }
    
}
void LoadCell::reset_cel_status(int head){
    for(int i=0;i< head;i++){
        cel_status[i]=0;
    }
}
/*****************************************************************************
    一定以上の重量があり、一定の同じ重量が検出されているかを確認する
    条件を見たせば物が載っていると判断する
*****************************************************************************/
bool LoadCell::checkIn(int head){
    float sabun;
    _head = head;
    for(int i = 0; i < WHT_CNT; i++){
        sabun = _pVal[head].weight - askWeight(_pVal[head].wVolt[i]);
        
        //printf("%2d : %6.5f  %6.1f  %6.1f\r",head,_pVal[head].wVolt[i],_pVal[head].weight,askWeight(_pVal[head].wVolt[i]));
        //指定重量より重いか
        if(_param.CUT_Y >= askWeight(_pVal[head].wVolt[i])){
            return false;
        }
        
        //重量が安定しているか
        //ゆっくりモード成立時の物が載っているかなので
        //条件はかなり緩くなると思われる
        if((sabun > _param.sabun || sabun < -_param.sabun)){
            return false;
        }
    }
    //ここまで来たら物体が載っていると判断
    return true;
}

bool LoadCell::checkIn2(int head,int now){
    float sabun;
    _head = head;
    int put_on_condition[WHT_CNT];
    //bool cut_y_condition[WHT_CNT];
    int Each_condition[WHT_CNT];
    for(int i = 0; i < WHT_CNT; i++){
        sabun = _pVal[head].weight - askWeight(_pVal[head].wVolt[i]);//現在の重量-過去の重量(wVolt[0]は現在のなのでfor文から除外
        
        //printf("%2d : %6.5f  %6.1f  %6.1f\r",head,_pVal[head].wVolt[i],_pVal[head].weight,askWeight(_pVal[head].wVolt[i]));
        //指定重量より重いか
        put_on_condition[i] = 0;
        if(_param.CUT <= askWeight(_pVal[head].wVolt[i])){   //CUT以上であれば、載ったと判断する
            put_on_condition[i] = 1;
        }else if(_param.CUT_Y <= askWeight(_pVal[head].wVolt[i])){
            put_on_condition[i] = 2;
        }else{
            put_on_condition[i] = 3;
        }
        
        
        /*
        if(_param.CUT_Y >= askWeight(_pVal[head].wVolt[i])){
            cut_y_condition[i] = false;
        }else{
            cut_y_condition[i] = true;
        }*/
        
        //重量が安定しているか
        //ゆっくりモード成立時の物が載っているかなので
        //条件はかなり緩くなると思われる
        Each_condition[i] = 0;
        if(_param.sabun < sabun){       //前回より重くなった
            Each_condition[i] = 1;
        }else if(-_param.sabun < sabun){//前回と同じくらい
            Each_condition[i] = 2;
        }else{                          //前回より軽くなった
            Each_condition[i] = 3;
        }
        
        
        /*
        if(sabun > _param.sabun){           //前回より重くなった
            Each_condition[i] = 1;
        }else if(sabun < -_param.sabun){    //前回より軽くなった
            Each_condition[i] = 2;
        }else{
            Each_condition[i] = 3;
        }*/
    }
    //if(0<=head && head<=4){
    //    printf("%4d:%2d:%2d:%2d:%5.1f:%5.1f:%5.1f:%5.1f\r\n",head,put_on_condition[0],put_on_condition[1],put_on_condition[2], _pVal[head].weight,askWeight(_pVal[head].wVolt[0]),askWeight(_pVal[head].wVolt[1]),askWeight(_pVal[head].wVolt[2]));
    //}
    /*
    for(int i=0;i< 6;i++){
        printf("|%2d:%2d",i+1,cel_status[i]);
    }
    printf("|h%2d",head);
    printf("\r\n");
    */
    //if(head==2){
    //    printf("%2d||%2d:%2d:%2d||%2d:%2d:%2d||%6.1f\r\n",cel_status[head],put_on_condition[0],put_on_condition[1],put_on_condition[2],Each_condition[0],Each_condition[1],Each_condition[2],askWeight(_pVal[head].wVolt[0]));
        //wait(3);
    //}
    /*if(head==2 && put_on_condition[0] == 1 && put_on_condition[1] == 1 && put_on_condition[2] == 1 && Each_condition[0] == 2){
        if(cel_status[head]== 1){
            printf("ON\r\n");
        }
        printf("%2d:%2d:%2d\r\n",put_on_condition[0],put_on_condition[1],put_on_condition[2]);
        wait(3);
    }*/
    if(put_on_condition[0] == 1 && put_on_condition[1] == 1 && put_on_condition[2] == 1 && Each_condition[0] == 2 && Each_condition[1] == 2 && Each_condition[2] == 2){
        
        cel_status[head]=1;
        //if(head==2 ){printf("1stON  ");printf("%2d:%2d:%2d||%2d:%2d:%2d||%6.1f\r\n",put_on_condition[0],put_on_condition[1],put_on_condition[2],Each_condition[0],Each_condition[1],Each_condition[2], askWeight(_pVal[head].wVolt[0]));}
        return true;
    }else if(put_on_condition[0] == 2){
        if(cel_status[head]== 1){
            cel_status[head]=1;
            //if(head==2 ){printf("2ndON  ");}
            return true;
        }else{
            cel_status[head]=0;
            return false;
        }
    }else if(put_on_condition[0] == 3){
        cel_status[head]=0;
        return false;
    }else {
        return false;
    }
    
    /*
    if(put_on_condition[0] == 1 || put_on_condition[1] <= 2 || put_on_condition[2] <= 2){  //CUT以上だった= 確実に乗っている
        if(Each_condition[0] == 1 || Each_condition[0] == 3){
            return false;
        }else{
            return true;
        }
    }else if(put_on_condition[0] == 2 || put_on_condition[1] <= 2 || put_on_condition[2] <= 2){
        if((now & (1<<head))> 0){
            return true;
        }else{
            return false;
        }       
    }else if(put_on_condition[0] == 3 || put_on_condition[0] <= 2 || put_on_condition[0] <= 2){  //CUT以上だった= 確実に乗っている
        return false;
    }
    */
    
    /*
    
    if(cut_y_condition == false && (Each_condition[1] < 2 || Each_condition[2] < 2)){ //CUT-Y以下で前回より軽くなった
        return false;
    }else if(cut_y_condition == false && (Each_condition[1] == 1 || Each_condition[2] == 1)){   //CUT-Y以下で前回より重くなった
        return false;
    }else if(cut_y_condition == false && (Each_condition[1] == 1 || Each_condition[2] == 1)){
        return true;
    }*/
        
    
    
    
    //ここまで来たら物体が載っていると判断
    //return true;
}

/*****************************************************************************
    0g～約4g(設定により変更)の検出重量が安定していれば
    0点調整、風袋引き(Tare)重量を取得
*****************************************************************************/
void LoadCell::autoTare(float diff){

    //差分が規定範囲を超えていたらやり直し
    //if(diff < -SABUN || diff > SABUN){
    if((diff < -_param.ZEROTOLERANCE || diff > _param.ZEROTOLERANCE)
     /*&& _param.zerocount >= 0*/){
        _pAutoCount[_head] = 0;     //安定していないためやり直し
        //printf("NG %3.1f\r\n",diff);
        return;
    }
    
    _pAutoCount[_head] += 1;     //安定している回数カウント(重量が規定範囲内にある)
   
    //検出重量が規定回数安定していることを確認
    //if(_pAutoCount[_head] >= AUTOZERO){
    if(_pAutoCount[_head] >= _param.zerocount){
        //printf("go_zero %5.1f\r\n",_pVal[_head].hweight);
        _pVal[_head].hVolt = _pVal[_head].volt;
        _pVal[_head].hweight = ask_H_Weight(_pVal[_head].hVolt);    //風袋引き取得
        _pAutoCount[_head] = _param.zerocount;     //オーバーフローしないための処理
        auto_zero_check = auto_zero_check | (1<<_head);
    }//if
}

/*****************************************************************************
    一定重量内ならばautoZeroを実行
*****************************************************************************/
void LoadCell::checkZeroRum(){
    //重量が規定より多いならゼロ点取らない。規定マイナス値の場合も
    if(_pVal[_head].weight < -_param.ZEROAREA || _pVal[_head].weight > _param.ZEROAREA){
        _pAutoCount[_head] = 0;
        return;
    }
    
    //現在の電圧と、前回の電圧の差(風袋込)
    float diff= askWeight(_pVal[_head].vol_old1) - _pVal[_head].weight;
    autoTare(diff);
    _pVal[_head].hweight = ask_H_Weight(_pVal[_head].hVolt); 
    _pVal[_head].weight = askWeight(_pVal[_head].volt);//重量取得
    //nowweightVale[_head] = askWeight(_pVal[_head].volt);
    
}

/*****************************************************************************
    起動時、強制ゼロ点調整時に使用
    重量が何gでも強制的にゼロ点取得を行う
*****************************************************************************/
void LoadCell::getZero(){
    setAinSpeed(SPAN_KURI);//電圧取得
    //現在の電圧と、前回の電圧の差(風袋抜)
    float diff= ask_H_Weight(_pVal[_head].vol_old1) - ask_H_Weight(_pVal[_head].volt);
    //printf("%5.1f  %5.1f\r\n",ask_H_Weight(_pVal[_head].vol_old1),ask_H_Weight(_pVal[_head].volt));
    //printf("%2d:%8.1f:%5.1f:%8.1f\r\n",_head,ask_H_Weight(_pVal[_head].vol_old1),ask_H_Weight(_pVal[_head].volt),diff);
    //autoTare(diff);
    
    if((diff < -_param.ZEROTOLERANCE || diff > _param.ZEROTOLERANCE)){
        _pAutoCount[_head] = 0;     //安定していないためやり直し
        return;
    }else{
        //printf("go_zero %5.1f\r\n",_pVal[_head].hweight);
        _pVal[_head].hVolt = _pVal[_head].volt;
        _pVal[_head].hweight = ask_H_Weight(_pVal[_head].hVolt);    //風袋引き取得
        _pAutoCount[_head] = _param.zerocount;     //オーバーフローしないための処理
        auto_zero_check = auto_zero_check | (1<<_head);
    }
}

void LoadCell::setZero(float zerodata){
    setAinSpeed(SPAN_KURI);//電圧取得
    //現在の電圧と、前回の電圧の差(風袋抜)
    float diff= 0.0;//強制的に０とする
    _pAutoCount[_head] = _param.zerocount;//強制的にOKとさせる
    
    _pVal[_head].volt = zerodata;
    _pVal[_head].vol_old1 = zerodata;
    //printf("%5.1f  %5.1f\r\n",ask_H_Weight(_pVal[_head].vol_old1),ask_H_Weight(_pVal[_head].volt));
    //printf("%2d:%8.1f:%5.1f:%8.1f\r\n",_head,ask_H_Weight(_pVal[_head].vol_old1),ask_H_Weight(_pVal[_head].volt),diff);
    //autoTare(diff);
    //printf("go_zero %5.1f\r\n",_pVal[_head].hweight);
    _pVal[_head].hVolt = _pVal[_head].volt;
    _pVal[_head].hweight = ask_H_Weight(_pVal[_head].hVolt);    //風袋引き取得
    _pAutoCount[_head] = _param.zerocount;     //オーバーフローしないための処理
    auto_zero_check = auto_zero_check | (1<<_head);
}


/* ------------ START パラメーター、各種データ処理 ------------ */
//現在選択されているロードセルのデータを取得
void LoadCell::set_now_loadcell_data(const loadcell_t *l){
    _preset = *l;   //選択されているプリセットデータ取得
}

//現在選択されているロードセルのスパン値を取得
void LoadCell::set_now_span_data(const span_t *s){
    _nowSpan = *s;   //選択されているプリセットデータ取得
}

//設置パラメータを取得
void LoadCell::set_param(const parameter_t p){
    _param = p;
}
void LoadCell::set_product(const product_t p){
    _product = p;
}

void LoadCell::set_BARATSUKI(float baratuki3){
    _param.BARATSUKI = baratuki3;
}
void LoadCell::set_HUNDO(float hundo){
    _param.hundo = hundo;
}

void LoadCell::set_ZEROAREA(float zeroarea){
    _param.ZEROAREA = zeroarea;
}
void LoadCell::set_option(const option_t p){
    _option = p;
}
//スパン値を取得する
void LoadCell::set_span(const span_t* s){
    //ヘッド数分のスパンデータをセット
    for(int i = 0; i < _numHead; i++){
        _pSpan[i].fain = s[i].fain;
        _pSpan[i].sain = s[i].sain;
        _pSpan[i].sain2 = s[i].sain2;
        _pSpan[i].sain3 = s[i].sain3;
        _pSpan[i].sain4 = s[i].sain4;
        _pSpan[i].sain5 = s[i].sain5;
        _pSpan[i].sain6 = s[i].sain6;
        _pSpan[i].sain7 = s[i].sain7;
        _pSpan[i].sain8 = s[i].sain8;
        _pSpan[i].sain9 = s[i].sain9;
        _pSpan[i].sain10 = s[i].sain10;
    }
    
}
void LoadCell::set_span_preset(){
    //ヘッド数分のスパンデータをセット
    for(int i = 0; i < _numHead; i++){
        _pSpan[i].fain = span_preset[i][0];
        _pSpan[i].sain = span_preset[i][1];
        _pSpan[i].sain2 = 0.0;
        _pSpan[i].sain3 = 0.0;
        _pSpan[i].sain4 = 0.0;
        _pSpan[i].sain6 = 0.0;
        _pSpan[i].sain7 = 0.0;
        _pSpan[i].sain8 = 0.0;
        _pSpan[i].sain9 = 0.0;
        _pSpan[i].sain10 = 0.0;
    }
    span_hundo[0] = span_hundo_preset;
    span_hundo[1] = 0;
    span_hundo[2] = 0;
    span_hundo[3] = 0;
    span_hundo[4] = 0;
    span_hundo[5] = 0;
    span_hundo[6] = 0;
    span_hundo[7] = 0;
    span_hundo[8] = 0;
    span_hundo[9] = 0;

    
}

//規定値(下限値、上限値)重量を超えていたら該当HEAD位置を返す
//但し、親子計量で親のみで組み合わせ成立の場合は0値を返す
//親のみ組み合わせ優先して取ってもらうため。
int LoadCell::over_weight(){
    int cnt = 0;
    const int rMin = 0,rMax = 3200; //最小値、最大値の初期値 //川 最大値を変更(1200->3200)
    float res = _preset.target + _preset.jougen;    //設定されている最大目標値
    int rankMin=rMin,rankMax=rMax;  //各最小値、最大値を代入
    
    //ランク管理設定に応じて閾値(重量)を取得
    if(_preset.rankOnOff == 0){
        //何もしない
    }else if(_preset.rankOnOff <= 3){
        rankMin = _preset.rankMin;
        rankMax = _preset.rankMak;
        if(_preset.rankOnOff == 1){ //下限値のみなら上限値なし(但し、仕様として1kg以上測れない)
            rankMax = rMax;
        }
        if(_preset.rankOnOff == 2){ //上限値のみなら下限値なし
            rankMin = rMin;
        }
    }

    //親子計量の場合
    if(_preset.kumi_flag==2){
        //親が目標重量(上限まで)だったら処理なし
        if( (anteiWeight(5) < res) && (anteiWeight(5) >= _preset.target)){
            return 0;
        }
        
        //機体が12ヘッドの場合のみ確認
        if(_numHead == 12){
            if((anteiWeight(11) < res) && (anteiWeight(11) >= _preset.target)){
                return 0;   
            }
        }
        res = res*0.9; //重量値を少し軽減
    }

    //重量超過、規定重量外の場所を取得
    for(int i=0; i < _numHead; i++) {
        if( (int(anteiWeight(i)) > int(res))                       //目標重量よりも重い
            || (anteiWeight(i) < rankMin && anteiWeight(i) > 0)   //規定MIN重量より軽い
            || (anteiWeight(i) > rankMax)) {                 //規定MAX重量より重い
            cnt += (1 << i);
        }
    }//for

    return cnt;
}

//規定値(下限値、上限値)重量を超えていたら該当HEAD位置を返す
//但し、親子計量で親のみで組み合わせ成立の場合は0値を返す
//親のみ組み合わせ優先して取ってもらうため。
int LoadCell::over_weight_taimen(){
    int cnt = 0;
    
    //対面時、片面の最大重量を取得
    float res = _preset.taimenTarget + _preset.taimenJougen;
    
    //重量超過、規定重量外の場所を取得
    for(int i=0; i < 6; i++) {
        if(anteiWeight(i) >= res){                       //目標重量よりも重い
            cnt += (1 << i);
        }
    }//for
    
    //対面時、片面の最大重量を取得
    res = _preset.target + _preset.jougen;
    
    //重量超過、規定重量外の場所を取得
    for(int i=6; i < _numHead; i++) {
        if(anteiWeight(i) >= res){                       //目標重量よりも重い
            cnt += (1 << i);
        }
    }//for
    
    return cnt;
}

/*****************************************************************************
    計量物が載っている位置を返す(各Bitで1の場所が空とする)
*****************************************************************************/
int LoadCell::getInHead(){
    int in= 0;
    for(_head = 0; _head < _numHead; _head++){
        if(_pVal[_head].weight > _param.CUT){
           in += (1 << _head);
        }
    }
    return in;
}

/*****************************************************************************
    計量が安定している位置を返す(各Bitで1の場所が空とする)
*****************************************************************************/
int LoadCell::getAnteiIn(){
    int in = 0;
    for(_head = 0; _head < _numHead; _head++){
        if(anteiWeight(_head) > 0){
           in += (1 << _head);
        }
    }
    return in;
}

/*****************************************************************************
    空の計量皿の位置を返す(各Bitで1の場所が空とする)
*****************************************************************************/
int LoadCell::getKaraHead(){
    int kara= 0;
    for(_head = 0; _head < _numHead; _head++){
        if(_pVal[_head].weight <= _param.CUT){
           kara += (1 << _head);
        }
    }
    return kara;
}

/*****************************************************************************
    空の計量皿の位置を返す(各Bitで1の場所が空とする)
    検出重量が安定してなければ空と判断する
*****************************************************************************/
int LoadCell::getAnteiKara(){
    int kara= 0;
    for(_head = 0; _head < _numHead; _head++){
        if(anteiWeight(_head) <= 0){
           kara += (1 << _head);
        }
    }
    return kara;
}

//受け取った組み合わせ位置の合計重量を返す
float LoadCell::getSum(int com){
    float sum=0;
    for(int bit = 0; com; com >>= 1){
        if(com & 0x01) sum += _pVal[bit].weight;
        bit++;
    }
    return sum;
}

//デバッグ用温度センサーの値を返す
int LoadCell::get_temperature(int head, int loop){
    float num;
    MULUTIPLEXER(head);
    num = now_voit(loop);
    return int( ((num*3.3)-0.6)*100);
    
}
/* ------------ END パラメーター、各種データ処理 ------------ */