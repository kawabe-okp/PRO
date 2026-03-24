#include "FileData.h" 

LocalFileSystem local("local");         //ローカルファイル名

//各ファイル名
const char NAME_PRODUCT[]="/local/PAR00.TXT"; //パラメータ
const char NAME_PRESET[] ="/local/PRE.TXT";  //選択されているプリセット位置
const char NAME_PARAM[]  ="/local/PARA2.TXT"; //パラメーター
const char NAME_PARAG[]  ="/local/PARAG.TXT"; //パラメーター
const char NAME_BASIC[]  ="/local/BASIC.TXT"; //基本設定
const char NAME_SPAN[]   ="/local/SPAN.TXT";   //各ヘッドのスパン値
const char NAME_SPAN2[]  ="/local/SPAN2.TXT";   //各ヘッドのスパン値
const char NAME_SPAN3[]  ="/local/SPAN3.TXT";   //各ヘッドのスパン値

const char NAME_SPANC[]  ="/local/SPANC.TXT";   //各ヘッドのスパン値
const char NAME_CELL[]   ="/local/CELL.TXT";   //計量設定(プリセット内容)
const char NAME_MCELL[]  ="/local/CELLM.TXT";   //計量設定(プリセット内容)
const char NAME_DATA[]   ="/local/DATA.TXT";   //設定範囲の制限値(通常、標準)
const char NAME_DATAL[]  ="/local/DATAL.TXT"; //設定範囲の制限値(ライト版)
const char NAME_DATAB[]  ="/local/DATAB.TXT"; //設定範囲の制限値(ビッグ版)
const char NAME_SDATA[]  ="/local/DATAP2.TXT"; //param設定値の設定範囲
const char NAME_TIME[]   ="/local/TIME.TXT"; //時間取得
const char NAME_STCEL[]  ="/local/STCEL.TXT"; //不使用セル取得
const char NAME_OPTIO[]  ="/local/OPTIO2.TXT";//特殊仕様設定 17/09/21
const char NAME_BUFFER[]   ="/local/BUFF.TXT";//ﾅｶﾞﾓﾉﾎｾｲ値 21/09/06
const char NAME_NAME[]   ="/local/NAME.TXT"; //入力するファイル名取得
const char NAME_LIMIT1[] ="/local/GENK.TXT"; //回数制限値。ファイル名は「限界」の意(limitだとバレバレなので。)
const char NAME_LIMIT2[] ="/local/GIRI.TXT"; //回数制限値。ファイル名は「ギリギリ」の意(limitだとバレバレなので。)
const char ERROR_LOG[]   ="/local/ERROR.TXT"; //入力するファイル名取得


#include "File_signal_set1.h" 
//const char NAME_PARAM_AZ_INT[] ="/local/PARAZI.TXT"; //オートゼロの設定値
//const char NAME_PARAM_AZ_FROAT[] ="/local/PARAZF.TXT"; //オートゼロの設定値

const int MAX_HEAD = 12;        //最大ヘッド数
const int NUM_DATA = TL_END;        //目標設定で設定するデータ数
const int NUM_SDATA = P_END;//22;       //パラメーターの項目数(PARAM)
const int NUM_SDATA_FLORT = P_ZEROTOLERANCE; //16; //パラメーターの浮動小数が始まる項目位置
extern int nowPreset;
struct tm date;
bool time_reset =false;        //日付がリセットされているか t=された
struct tm s_date;        //時刻設定ファイルの読み出し
extern time_t seconds;
extern int sphun_count;
extern int span_hundo[10];
extern int stop_cells;           //計量時に強制的に0にするセルを選択
extern int stop_cells2;           //計量時に強制的に0にするセルを選択(データで保存するところ)
extern int name[25][10];             //名前入力データ
extern int katamen;              //0:no 1:1~6 2:7~12  12ﾍｯﾄﾞを片面だけ使用する(自分の皿でLEDが隠れる場合,6ﾍｯﾄﾞで使用するときの表示)
extern int para_head;
extern int para_array;
int KUMI_limit =0;            //回数制限の場合の上限
int KUMI_now =0;            //回数制限の場合の現在値
extern int file_lost_sig;       //ファイル読み込みに失敗した際、出力するデータ
extern float betwe_spa[12][10]; 
extern float betwe_spa2[12][10];
bool new_pointer_created = false; //make_new_pointer();を複数動作させないための引数

extern int test_target[50];
extern float op_n_hosei;//OP_NAGAMONO_HOSEIの値をfloat型で保存する

extern int op_h_buffer1[12]; /*ヘッド毎のﾊﾞｯﾌｧ(ﾅｶﾞﾓﾉﾎｾｲ)*/
                        
extern float op_h_buffer2[12]; /*ヘッド毎のﾊﾞｯﾌｧ(ﾅｶﾞﾓﾉﾎｾｲ)*/
extern int hosei[12];
extern float hosei_f[12];
extern float span_preset[12][2];
extern int span_hundo_preset;

#include "File_signal.h" 


//全てのファイル
void FileData::read_all_file(){
    read_product();        //設定値(製品仕様)
    read_param();          //設定値
    if(new_pointer_created == false){
        new_pointer_created = true;
        make_new_pointer();    //必要な動的配列を確保
    }
    read_preset();         //プリセット番号
    read_span_hundo();           //スパン値
    read_span();           //スパン値
    read_loadcell();       //目標設定値
    read_basic_setting();  //基本設定値
    read_SData();          //PARAMの設定範囲
    read_unitweight();     //複数重量設定値
    read_name();
    read_option();          //オプションの設定値
    read_limit1();
    read_limit2();
    read_nagamono_buffer();
    //データの読み込み(各制限値)
    read_data_by_product(_product.productType);
}

//製品別のデータ読み込みを行う
void FileData::read_data_by_product(int type){
    switch (type){
        case(STANDARD):
            read_data(NAME_DATA);  //通常設定
            break;
        
        case(JUNIOR):
            read_data(NAME_DATAL);  //ジュニア用設定
            break;
            
        case(BIG):
            //read_paramg();          //設定値
            read_data(NAME_DATAB);  //ビッグ用設定
            
            break;
            
        case(MATRIX):
            read_data(NAME_DATA);  //通常設定
            break;
        
        case(STANDARD_D):
            read_data(NAME_DATA);  //通常設定(デバッグ)
            break;
        
        case(JUNIOR_D):
            read_data(NAME_DATAL);  //ジュニア用設定(デバッグ)
            break;
            
        case(BIG_D):
            read_data(NAME_DATAB);  //ビッグ用設定(デバッグ)
            break;
            
        case(MATRIX_D):
            read_data(NAME_DATA);  //通常設定
            break;
    }
}
/*****************************************************************************
    動的配列のnewを実行
*****************************************************************************/
void FileData::make_new_pointer(){
    _pCel = new loadcell_t[MAX_HEAD*2];
    _pSpan = new span_t[MAX_HEAD];
    _pData = new data_t[NUM_DATA];
    _pSData = new data_t[NUM_SDATA];
}
void FileData::make_new_pointer2(){
    _pMweight= new multiWeight_t[MAX_HEAD*2];
}
//プリセット番号を上書き
void FileData::save_preset(){
    FILE *sfp;

    if ((sfp = fopen(NAME_PRESET,"w")) == NULL) {
        fprintf(stderr, "%s write Err.\n", "PRE");
        exit(EXIT_FAILURE);
    }

    fprintf(sfp,"%d\r\n",_preset);
    fclose(sfp);
    sfp = NULL;
}

//プリセット番号を読み込み
void FileData::read_preset(){
    FILE *fp;

    if ((fp = fopen(NAME_PRESET,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "PRE");
        //exit(EXIT_FAILURE);
        file_lost_sig += 2;
        _preset=0;
        save_preset();
        //fclose(fp); //ファイルクローズ
        fp = NULL;
    }else{
        fscanf(fp, "%d", &_preset);
        fclose(fp); //ファイルクローズ
        fp = NULL;
    }
}

//前回選択されたプリセット番号を取得する
int FileData::get_preset(){
    return _preset;
}

//選択されたプリセット番号をセット
void FileData::set_preset(int preset){
    _preset = preset;
    nowPreset = preset;
}

/*****************************************************************************
    設定情報の上書き
*****************************************************************************/
void FileData::save_SAGYOU(int ji, int hun, int presetno,int target,int kumi_total,float average ){
    FILE *sfp = fopen("/local/SAGYOU.TXT", "a");
    if(sfp == NULL) {
        error("Could not open file for SAGYOU.TXT\n");
        exit(EXIT_FAILURE);
    }

    //fprintf(sfp,"%02d:%02d  %2d  %5dg  %5dcnt  ave:%7.1fg\r\n"
    fprintf(sfp,"%2d  %5dg  %5dcnt  ave:%7.1fg\r\n"
        //,ji
        //,hun
        ,presetno
        ,target
        ,kumi_total
        ,average
    );

    fclose(sfp);
    sfp = NULL;
}

/*****************************************************************************
    設定情報の上書き
*****************************************************************************/
void FileData::save_param(){
    FILE *sfp;

    if ((sfp = fopen(NAME_PARAM,"w")) == NULL) {
        fprintf(stderr, "%s write Err.\n", "PARAM");
        exit(EXIT_FAILURE);
    }

    //fprintf(sfp,"%d  %d  %d  %d  %d %d %2d %4d %4d %4d %4d %2d %2d  %d  %d  %d  %d  %d %d %d %f %f %f %f %f %f %f %f %f %f %f\r\n"
  //fprintf(sfp,"%4d %4d %4d %4d %2d %2d %2d  %d  %d  %d  %d  %d %d %d %f %f %f %f %f %f %f %f %f %f %f\r\n"
    fprintf(sfp,"%4d %4d %4d %2d %2d %2d  %2d  %3d  %2d  %2d  2%d  %2d %3d %2d %f %f %f %f %f %f %f %f %f %f %f %f\r\n"
        //,_product.productType
        //,_product.limit_on
        //,_product.Array
        //,_product.Rating
        //,_product.matrix_mode
        //,_product.use_usb      
        //,_product.head
        ,_param.hundo
        ,_param.KURI
        ,_param.KURI_Y
        ,_param.KURI_J
        //,_param.AUTOCUT
        ,_param.CUT
        ,_param.CUT_Y
        ,_param.sabun
        ,_param.BARA_KAKUNIN
        ,_param.liteLedOff
        ,_param.tairyu
        ,_param.vivration
        ,_param.zeroErr
        ,_param.zeroRange
        ,_param.zerocount
        ,_param.ZEROTOLERANCE
        ,_param.ZEROAREA
        ,_param.ZEROAREA2
        ,_param.BARATSUKI
        ,_param.buffer
        ,_param.MINI_target
        ,_param.BARATSUKI_R
        ,_param.BARATSUKI_GRAM
        ,_param.buffer2
        ,_param.bufferT
        ,_param.buffer_kaisu
        ,_param.BARATSUKI_SA
    );
    //fprintf(sfp,"p li ar ra ma u he  hun kuri kury kurj cu cy sa ll ta vi  zE zR zC ZEROTOR ZEROARE  ZEROAR2  BARATSUK buffer   MINI_tar BARATSU2 BARATSU3 buffer2  bufferT  buf_KAIS\r\n");
    fprintf(sfp," hun kuri kury kurj cu cy  sa bak ll ta vi  zE zR zC ZEROTOR ZEROARE  ZEROAR2  BARATSUK buffer   MINI_tar BARATSU2 BARATSU3 buffer2  bufferT  buf_KAIS  BARASAB\r\n");

    fclose(sfp);
    sfp = NULL;
}


void FileData::save_product(){
    FILE *sfp;

    if ((sfp = fopen(NAME_PRODUCT,"w")) == NULL) {
        fprintf(stderr, "%s write Err.\n", "PARAM");
        exit(EXIT_FAILURE);
    }

    fprintf(sfp,"%d %2d %2d %2d %2d %1d %2d\r\n"
        ,_product.productType
        ,_product.limit_on
        ,_product.Array
        ,_product.Rating
        ,_product.matrix_mode
        ,_product.use_usb      
        ,_product.head
    );
    fprintf(sfp,"p li ar ra ma u he\r\n");

    fclose(sfp);
    sfp = NULL;
}

/*****************************************************************************
    設定情報の読み取り
*****************************************************************************/
void FileData::read_param(){
    FILE *fp;

    if ((fp = fopen(NAME_PARAM,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "PARAM");
        //exit(EXIT_FAILURE);
        file_lost_sig += 1;
    }else{
        //fscanf(fp, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%f%f%f%f%f%f%f%f%f%f%f"
        fscanf(fp, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%f%f%f%f%f%f%f%f%f%f%f%f"
            //,&_product.productType
            //,&_product.limit_on
           // ,&_product.Array
            //,&_product.Rating
            //,&_product.matrix_mode
            //,&_product.use_usb
            //,&_product.head
            ,&_param.hundo
            ,&_param.KURI
            ,&_param.KURI_Y
            ,&_param.KURI_J
            //_param.AUTOCUT
            ,&_param.CUT
            ,&_param.CUT_Y
            ,&_param.sabun
            ,&_param.BARA_KAKUNIN
            ,&_param.liteLedOff
            ,&_param.tairyu
            ,&_param.vivration
            ,&_param.zeroErr
            ,&_param.zeroRange
            ,&_param.zerocount
            ,&_param.ZEROTOLERANCE
            ,&_param.ZEROAREA
            ,&_param.ZEROAREA2
            ,&_param.BARATSUKI
            ,&_param.buffer
            ,&_param.MINI_target
            ,&_param.BARATSUKI_R
            ,&_param.BARATSUKI_GRAM
            ,&_param.buffer2
            ,&_param.bufferT
            ,&_param.buffer_kaisu
            ,&_param.BARATSUKI_SA
        );
        //川 実験...(19/07/11)不要!!
        //if (_param.MINI_target != 0 &&_param.MINI_target < _param.ZEROAREA){
        //    _param.ZEROAREA = _param.MINI_target;
        //}
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}

void FileData::read_product(){
    FILE *fp;

    if ((fp = fopen(NAME_PRODUCT,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "PARAM");
        //exit(EXIT_FAILURE);
        file_lost_sig += 1;
    }else{
        fscanf(fp, "%d%d%d%d%d%d%d"
            ,&_product.productType
            ,&_product.limit_on
            ,&_product.Array
            ,&_product.Rating
            ,&_product.matrix_mode
            ,&_product.use_usb
            ,&_product.head
        );
        /*printf("read %d %2d %2d %2d %2d %1d %2d\r\n"
        ,_product.productType
        ,_product.limit_on
        ,_product.Array
        ,_product.Rating
        ,_product.matrix_mode
        ,_product.use_usb      
        ,_product.head
        );
        
        printf("read p li ar ra ma u he\r\n");
        */
        
        if(_product.productType<1){_product.productType=1;}
        if(_product.productType>5){}
        
        if(_product.limit_on<0)     {_product.limit_on=0;}
        if(_product.limit_on>5)     {_product.limit_on=5;}
        if(_product.Array<0)        {_product.Array=0;   }
        if(_product.Array>3)        {_product.Array=3;   }
        if(_product.Rating<0)       {_product.Rating=0;   }
        if(_product.Rating>3)       {_product.Rating=3;   }
        if(_product.matrix_mode<0)  {_product.matrix_mode=0;   }
        if(_product.matrix_mode>1)  {_product.matrix_mode=1;   }
        if(_product.use_usb<0)      {_product.use_usb=0;   }
        if(_product.use_usb>1)      {_product.use_usb=1;   }
        if(_product.head<1)         {_product.head=1; }
        if(_product.head>12)        {_product.head=12;}
        
        
        //printf("%2d %2d %4d\r\n",_product.productType,_product.head,_param.hundo);
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}

/*****************************************************************************
    設定情報の読み取り
*****************************************************************************/
void FileData::read_paramg(){
    FILE *fp;

    if ((fp = fopen(NAME_PARAG,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "PARAM");
        //exit(EXIT_FAILURE);
        file_lost_sig += 1;
        //fclose(fp);
        fp = NULL;
    }else{
        fscanf(fp, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%f%f%f%f%f%f%f%f%f%f%f%f"
            //,&_product.productType
            //,&_product.limit_on
           // ,&_product.Array
            //,&_product.Rating
            //,&_product.matrix_mode
            //,&_product.use_usb
            //,&_product.head
            ,&_param.hundo
            ,&_param.KURI
            ,&_param.KURI_Y
            ,&_param.KURI_J
            //_param.AUTOCUT
            ,&_param.CUT
            ,&_param.CUT_Y
            ,&_param.sabun
            ,&_param.BARA_KAKUNIN
            ,&_param.liteLedOff
            ,&_param.tairyu
            ,&_param.vivration
            ,&_param.zeroErr
            ,&_param.zeroRange
            ,&_param.zerocount
            ,&_param.ZEROTOLERANCE
            ,&_param.ZEROAREA
            ,&_param.ZEROAREA2
            ,&_param.BARATSUKI
            ,&_param.buffer
            ,&_param.MINI_target
            ,&_param.BARATSUKI_R
            ,&_param.BARATSUKI_GRAM
            ,&_param.buffer2
            ,&_param.bufferT
            ,&_param.buffer_kaisu
            ,&_param.BARATSUKI_SA
        );
        //川 実験...(19/07/11)不要!!
        //if (_param.MINI_target != 0 &&_param.MINI_target < _param.ZEROAREA){
        //    _param.ZEROAREA = _param.MINI_target;
        //}
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}
/*****************************************************************************
    変更されたデータを受け取る
*****************************************************************************/
void FileData::set_param(const int data[], const float fdata[]){
    //_product.productType    = data[P_productType];
    //_product.limit_on       = data[P_limit_on];
    //_product.Array          = data[P_Array];
    //_product.Rating         = data[P_Rating];   
    //_product.matrix_mode    = data[P_matrix_mode];
    //_product.use_usb        = data[P_Use_USB];    
    //_product.head           = data[P_head];
    _param.hundo          = data[P_hundo];
    _param.KURI           = data[P_KURI];
    _param.KURI_Y         = data[P_KURI_Y];
    _param.KURI_J         = data[P_KURI_J];
    //aram.AUTOCUT        = data[P_AUTOCUT];
    _param.CUT            = data[P_CUT];
    _param.CUT_Y          = data[P_CUT_Y];
    _param.sabun          = data[P_sabun];
    _param.BARA_KAKUNIN   = data[P_BARA_KAKUNIN];
    _param.liteLedOff     = data[P_liteLedOff];
    _param.tairyu         = data[P_tairyu];
    _param.vivration      = data[P_vivration];
    _param.zeroErr        = data[P_zeroErr];
    _param.zeroRange      = data[P_zeroRange];
    _param.zerocount      = data[P_zerocount];
    _param.ZEROTOLERANCE  = fdata[P_ZEROTOLERANCE   -P_ZEROTOLERANCE];
    _param.ZEROAREA       = fdata[P_ZEROAREA        -P_ZEROTOLERANCE];
    _param.ZEROAREA2      = fdata[P_ZEROAREA2       -P_ZEROTOLERANCE];
    _param.BARATSUKI      = fdata[P_BARATSUKI       -P_ZEROTOLERANCE];
    _param.buffer         = fdata[P_buffer          -P_ZEROTOLERANCE];
    _param.MINI_target    = fdata[P_MINI_target     -P_ZEROTOLERANCE];
    _param.BARATSUKI_R    = fdata[P_BARATSUKI_R     -P_ZEROTOLERANCE];
    _param.BARATSUKI_GRAM = fdata[P_BARATSUKI_GRAM  -P_ZEROTOLERANCE];
    _param.buffer2        = fdata[P_buffer2         -P_ZEROTOLERANCE];
    _param.bufferT        = fdata[P_bufferT         -P_ZEROTOLERANCE];
    _param.buffer_kaisu   = fdata[P_buffer_KAISU    -P_ZEROTOLERANCE];
    _param.BARATSUKI_SA   = fdata[P_BARATSUKI_SA    -P_ZEROTOLERANCE];

}

/*****************************************************************************
    設定時間の読み取り
*****************************************************************************/
void FileData::read_time(){
    //time_t seconds = time(NULL);
    FILE *fp;
    //bool fopen_flg;
    if ((fp = fopen(NAME_TIME,"r")) == NULL) {
        //fopen_flg=false;
        fprintf(stderr, "%s Read Err.\n", "time");
        //exit(EXIT_FAILURE);
        file_lost_sig += 4096;
        s_date.tm_year = 2010;
        s_date.tm_mon  = 1;
        s_date.tm_mday = 1;
        s_date.tm_hour = 0;
        s_date.tm_min  = 0;
        s_date.tm_sec  = 0;
        fp = fopen(NAME_TIME,"w");
        fprintf(fp,"%4d %d %02d %02d %02d %02d\r\n"
            ,s_date.tm_year
            ,s_date.tm_mon
            ,s_date.tm_mday
            ,s_date.tm_hour
            ,s_date.tm_min
            ,s_date.tm_sec
        );
        //fclose(fp);
        fp = NULL;
    
        
        
    }else{
        //fopen_flg=true;
        fscanf(fp, "%d%d%d%d%d%d"
            ,&s_date.tm_year
            ,&s_date.tm_mon
            ,&s_date.tm_mday
            ,&s_date.tm_hour
            ,&s_date.tm_min
            ,&s_date.tm_sec
        );
        fclose(fp);
        fp = NULL;
    }   
    //読み込んだデータがおかしかった場合は、データを上書きする
    if(s_date.tm_year < 2010 || 2200 < s_date.tm_year ||
       s_date.tm_mon  <    0 ||   12 < s_date.tm_mon  ||
       s_date.tm_mday <    0 ||   32 < s_date.tm_mday ||
       s_date.tm_hour <    0 ||   25 < s_date.tm_hour ||
       s_date.tm_min  <    0 ||   61 < s_date.tm_min  ||
       s_date.tm_sec  <    0 ||   61 < s_date.tm_sec){
        file_lost_sig += 4096;
        s_date.tm_year = 2010;
        s_date.tm_mon  = 1;
        s_date.tm_mday = 1;
        s_date.tm_hour = 0;
        s_date.tm_min  = 0;
        s_date.tm_sec  = 0;
        fp = fopen(NAME_TIME,"w");
        fprintf(fp,"%4d %d %02d %02d %02d %02d\r\n"
            ,s_date.tm_year
            ,s_date.tm_mon
            ,s_date.tm_mday
            ,s_date.tm_hour
            ,s_date.tm_min
            ,s_date.tm_sec
        );
        fclose(fp);
        fp = NULL;
    }
       
    
    s_date.tm_mon -= 1;       // 単位が0-11なので
    s_date.tm_year -= 1900;   // 単位が 1900からなので
    
    //川 実験
    //ファイルクローズ
    //strftime(time_n, sizeof(time_n), "%Y", localtime(&s_date.tm_year));
    //date.tm_year=    stoi(time_n);
    //if(fopen_flg==true){
    //    fclose(fp);
    //    fp = NULL;
    //}
    //printf("%d\r\n", date->tm_year);
    seconds = time(NULL);
    if (seconds == -1){         // <1>
        time_reset = true;
        date.tm_sec = 00;    // 0-59
        date.tm_min = 00;    // 0-59
        date.tm_hour = 0;   // 0-23
        date.tm_mday = 1;   // 1-31
        date.tm_mon = 0;     // 0-11
        date.tm_year = 110;  // year since 1900
        seconds = mktime(&date);
        set_time(seconds);
        //printf("AA1Time as a string = %s\r\n", ctime(&seconds));
    }

    if( (date.tm_year +1900) < 2019 ){
        time_reset = true;
    }
    //printf("%d\r\n", s_date.tm_year);
    //printf("%d\r\n", date.tm_year);
    //printf("AABTime as a string = %s\r\n", ctime(&seconds));
    if (s_date.tm_year > date.tm_year){
        date.tm_sec  = s_date.tm_sec;    // 0-59
        date.tm_min  = s_date.tm_min;    // 0-59
        date.tm_hour = s_date.tm_hour;   // 0-23
        date.tm_mday = s_date.tm_mday;   // 1-31
        date.tm_mon  = s_date.tm_mon;     // 0-11
        date.tm_year = s_date.tm_year;  // year since 1900
        seconds = mktime(&date);
        set_time(seconds);
        //printf("AA2Time as a string = %s\r\n", ctime(&seconds));
    //printf("%d\r\n", s_date.tm_mon);
    //printf("%d\r\n", date.tm_mon);
    }else if (s_date.tm_year == date.tm_year &&
                s_date.tm_mon  > date.tm_mon){
        date.tm_sec  = s_date.tm_sec;    // 0-59
        date.tm_min  = s_date.tm_min;    // 0-59
        date.tm_hour = s_date.tm_hour;   // 0-23
        date.tm_mday = s_date.tm_mday;   // 1-31
        date.tm_mon  = s_date.tm_mon;     // 0-11
        seconds = mktime(&date);
        set_time(seconds);
        //printf("AA3Time as a string = %s\r\n", ctime(&seconds));
    }else if (s_date.tm_year == date.tm_year &&
                s_date.tm_mon  == date.tm_mon && 
                s_date.tm_mday > date.tm_mday){
        date.tm_sec  = s_date.tm_sec;    // 0-59
        date.tm_min  = s_date.tm_min;    // 0-59
        date.tm_hour = s_date.tm_hour;   // 0-23
        date.tm_mday = s_date.tm_mday;   // 1-31
        seconds = mktime(&date);
        set_time(seconds);
        //printf("AA4Time as a string = %s\r\n", ctime(&seconds));
    }else if (s_date.tm_year == date.tm_year &&
                s_date.tm_mon  == date.tm_mon && 
                s_date.tm_mday == date.tm_mday &&
                s_date.tm_hour > date.tm_hour){
        date.tm_sec  = s_date.tm_sec;    // 0-59
        date.tm_min  = s_date.tm_min;    // 0-59
        date.tm_hour = s_date.tm_hour;   // 0-23
        seconds = mktime(&date);
        set_time(seconds);
        //printf("AA5Time as a string = %s\r\n", ctime(&seconds));
    }else if (s_date.tm_year == date.tm_year &&
                s_date.tm_mon  == date.tm_mon && 
                s_date.tm_mday == date.tm_mday &&
                s_date.tm_hour == date.tm_hour &&
                s_date.tm_min  > date.tm_min){
            
        date.tm_sec  = s_date.tm_sec;    // 0-59
        date.tm_min  = s_date.tm_min;    // 0-59
        seconds = mktime(&date);
        set_time(seconds);
        //printf("AA6Time as a string = %s\r\n", ctime(&seconds));
    }else if (s_date.tm_year == date.tm_year &&
                s_date.tm_mon  == date.tm_mon && 
                s_date.tm_mday == date.tm_mday &&
                s_date.tm_hour == date.tm_hour &&
                s_date.tm_min  == date.tm_min &&
                s_date.tm_sec  !=  date.tm_sec){
        date.tm_sec  = s_date.tm_sec;    // 0-59
        seconds = mktime(&date);
        set_time(seconds);
        //printf("AA7Time as a string = %s\r\n", ctime(&seconds));
    }
    //printf("AA0Time as a string = %s\r\n", ctime(&seconds));
}
void FileData::save_time(){
    FILE *fp;
    fp = fopen(NAME_TIME,"w");
    fprintf(fp,"%4d %d %02d %02d %02d %02d\r\n"
        ,date.tm_year + 1900  // year since 1900
        ,date.tm_mon
        ,date.tm_mday + 1
        ,date.tm_hour
        ,date.tm_min
        ,date.tm_sec
    );
    fclose(fp);
    fp = NULL;
}
/*****************************************************************************
    不使用セルの上書き
*****************************************************************************/
void FileData::save_stop_cells(int stop_cells2){
    FILE *sfp;

    if ((sfp = fopen(NAME_STCEL,"w")) == NULL) {
        fprintf(stderr, "%s write Err.\n", "stop_cell");
        exit(EXIT_FAILURE);
    }

    fprintf(sfp,"%d\r\n"
        ,stop_cells2
    );
    
    fclose(sfp);
    sfp = NULL;
}

/*****************************************************************************
    不使用セルの読み取り
*****************************************************************************/
void FileData::read_stop_cells(){
    FILE *fp;

    if ((fp = fopen(NAME_STCEL,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "stop_cell");
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%d"
        ,&stop_cells2
    );
    //ファイルクローズ
    fclose(fp);
    fp = NULL;
}

/*****************************************************************************
    変更されたデータを受け取る(構造体型)
*****************************************************************************/
void FileData::set_param(const parameter_t data){
    _param = data;
}
void FileData::set_product(const product_t data){
    _product = data;
}

/*****************************************************************************
    オプション情報の上書き
*****************************************************************************/
void FileData::save_option(){
    FILE *sfp;

    if ((sfp = fopen(NAME_OPTIO,"w")) == NULL) {
        fprintf(stderr, "%s write Err.\n", "OPTIO");
        exit(EXIT_FAILURE);
    }

    fprintf(sfp,"%3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\r\n"
        ,_option.reck_zen
        ,_option.reck_ato
        ,_option.usb_EN
        ,_option.c_rotate
        ,_option.rank_over
        ,_option.kokuti
        ,_option.kikakugai_v
        ,_option.rank_speed
        ,_option.btn_sett
        ,_option.beginner
        ,_option.selbeep
        ,_option.no_combi_v
        ,_option.second_target
        ,_option.number_color
        ,_option.O_count_lim
        ,_option.O_lit_count_lim
        ,_option.nagamono_hosei
        ,_option.waiting_limit
        ,_option.selautozero
        ,_option.kekka_hyouji
        ,_option.alart_autozero
        ,_option.alart_below_cut
        ,_option.fix_value
    );
    fprintf(sfp,    "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\r\n");
    fprintf(sfp,"\r\nzen|ato|usb|rot|rov|kok|kiv|rsp|bse|beg|bep|cov|set|nco|OK1|OK2|nag|wli|saz|keh|aau|abc|fix|\r\n");
    fclose(sfp);
    sfp = NULL;
}

/*****************************************************************************
    オプション情報の読み取り
*****************************************************************************/
void FileData::read_option(){
    FILE *fp;
    int reck_zen,reck_ato,usb_EN,rank_over;
    if ((fp = fopen(NAME_OPTIO,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "OPTIO");
        //exit(EXIT_FAILURE);
        file_lost_sig += 256;
        printf("NAME_OPTIO%10d   \r\n",reck_zen);
        _option.reck_zen    = 0;
        _option.reck_ato    = 0;
        _option.usb_EN      = 0;
        _option.c_rotate    = 0;
        _option.rank_over   = 0;
        _option.kokuti      = 3;
        _option.kikakugai_v = 0;
        _option.rank_speed  = 1;
        _option.btn_sett    = 0;
        _option.beginner    = 1;
        _option.selbeep     = 3;
        _option.no_combi_v  = 1;
        _option.second_target   = 0;
        _option.number_color    = 0;
        _option.O_count_lim     = 1;
        _option.O_lit_count_lim = 1;
        _option.nagamono_hosei  = 0;
        op_n_hosei = 0;
        _option.waiting_limit   = 0;
        _option.selautozero = 1;
        _option.kekka_hyouji = 1;
        _option.alart_autozero = 0;
        _option.alart_below_cut = 0;
        _option.fix_value = 1;
        save_option();
        fclose(fp);
        fp = NULL;
    }else{
        fscanf(fp, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
            ,&reck_zen
            ,&reck_ato
            ,&usb_EN
            ,&_option.c_rotate
            ,&rank_over
            ,&_option.kokuti
            ,&_option.kikakugai_v
            ,&_option.rank_speed
            ,&_option.btn_sett
            ,&_option.beginner
            ,&_option.selbeep
            ,&_option.no_combi_v
            ,&_option.second_target
            ,&_option.number_color
            ,&_option.O_count_lim
            ,&_option.O_lit_count_lim
            ,&_option.nagamono_hosei
            ,&_option.waiting_limit
            ,&_option.selautozero
            ,&_option.kekka_hyouji
            ,&_option.alart_autozero
            ,&_option.alart_below_cut
            ,&_option.fix_value
        );
        
        
        printf("reck_zen        %5d\r\n",reck_zen);
        printf("reck_ato        %5d\r\n",reck_ato);
        printf("usb_EN          %5d\r\n",usb_EN);
        printf("c_rotate        %5d\r\n",_option.c_rotate);
        printf("rank_over       %5d\r\n",rank_over);
        printf("kokuti          %5d\r\n",_option.kokuti);
        printf("kikakugai_v     %5d\r\n",_option.kikakugai_v);
        printf("rank_speed      %5d\r\n",_option.rank_speed);
        printf("btn_sett        %5d\r\n",_option.btn_sett);
        printf("beginner        %5d\r\n",_option.beginner);
        printf("selbeep         %5d\r\n",_option.selbeep);
        printf("no_combi_v      %5d\r\n",_option.no_combi_v);
        printf("second_target   %5d\r\n",_option.second_target);
        printf("number_color    %5d\r\n",_option.number_color);
        printf("O_count_lim     %5d\r\n",_option.O_count_lim);
        printf("O_lit_count_lim %5d\r\n",_option.O_lit_count_lim);
        printf("nagamono_hosei  %5d\r\n",_option.nagamono_hosei);
        printf("waiting_limit  %5d\r\n",_option.waiting_limit);
        printf("selautozero    %5d\r\n",_option.selautozero);
        printf("kekka_hyouji    %5d\r\n",_option.kekka_hyouji);
        printf("alart_autozero  %5d\r\n",_option.alart_autozero);
        printf("alart_below_cut %5d\r\n",_option.alart_below_cut);
        printf("fix_value       %5d\r\n",_option.fix_value);
        
        
        if(reck_zen == 0){  _option.reck_zen=false;
        }else{              _option.reck_zen=true;
        }
        if(reck_ato == 0){  _option.reck_ato=false;
        }else{              _option.reck_ato=true;
        }
        if(usb_EN == 0){    _option.usb_EN=false;
        }else{              _option.usb_EN=true;
        }
        if(rank_over == 0){ _option.rank_over=false;
        }else{              _option.rank_over=true;
        }
        if(_option.c_rotate        < 0 || 5  < _option.c_rotate        ){_option.c_rotate        = 0 ;}
        if(_option.kokuti          < 0 || 5  < _option.kokuti          ){_option.kokuti          = 0 ;}
        if(_option.kikakugai_v     < 0 || 5  < _option.kikakugai_v     ){_option.kikakugai_v     = 0 ;}
        if(_option.rank_speed      < 0 || 5  < _option.rank_speed      ){_option.rank_speed      = 0 ;}
        if(_option.btn_sett        < 0 || 5  < _option.btn_sett        ){_option.c_rotate        = 0 ;}
        if(_option.beginner        < 0 || 5  < _option.beginner        ){_option.beginner        = 0 ;}
        if(_option.selbeep         < 0 || 5  < _option.selbeep         ){_option.selbeep         = 0 ;}
        if(_option.no_combi_v      < 0 || 5  < _option.no_combi_v      ){_option.no_combi_v      = 0 ;}
        if(_option.second_target   < 0 || 5  < _option.second_target   ){_option.second_target   = 0 ;}
        if(_option.number_color    < 0 || 1  < _option.number_color    ){_option.number_color    = 0 ;}
        if(_option.O_count_lim     < 1 || 5  < _option.O_count_lim     ){_option.O_count_lim     = 0 ;}
        if(_option.O_lit_count_lim < 1 || 10 < _option.O_lit_count_lim ){_option.O_lit_count_lim = 0 ;}
        if(_option.nagamono_hosei  < 0 || 100< _option.nagamono_hosei  ){_option.nagamono_hosei = 0 ;}
        if(_option.waiting_limit   < 0 || 480< _option.waiting_limit   ){_option.waiting_limit = 0 ;}
        if(_option.selautozero     < 0 || 1  < _option.selautozero     ){_option.selautozero     = 1 ;}
        if(_option.kekka_hyouji    < 0 || 1  < _option.kekka_hyouji    ){_option.kekka_hyouji    = 1 ;}
        if(_option.alart_autozero  < 0 || 1  < _option.alart_autozero  ){_option.alart_autozero  = 0 ;}
        if(_option.alart_below_cut < 0 || 1  < _option.alart_below_cut ){_option.alart_below_cut = 0 ;}
        if(_option.fix_value        < 0 || 1  < _option.fix_value        ){_option.fix_value     = 0 ;}
        
        op_n_hosei = float(_option.nagamono_hosei)/float(10);
        //ファイルクローズ
       // printf("%d  %d  %d  %d \r\n",_option.reck_zen,_option.reck_ato,_option.usb_EN);
        fclose(fp);
        fp = NULL;
    }
}
/*****************************************************************************
    変更されたオプションデータを受け取る
*****************************************************************************/
void FileData::set_option(const int data[]){
    // data[0];ｼﾞｭﾝｻｲﾃｷﾁ  　　 data[3];ｼﾖｳｻﾞﾗｾﾝﾀｸ
    _option.reck_zen         = data[OP_ZENNOSE];
    _option.reck_ato         = data[OP_ATONOSE];
    _option.usb_EN           = data[OP_USB_EN];
    _option.c_rotate         = data[OP_CHAR_ROTATE];
    _option.rank_over        = data[OP_RANK_OVER];
    _option.kokuti           = data[OP_KOKUTI];
    _option.kikakugai_v      = data[OP_KIKAKUGAI_V];
    _option.rank_speed       = data[OP_RANKSPEED];
    _option.btn_sett         = data[OP_BTN_SETTING];
    _option.beginner         = data[OP_BEGINNER];
    _option.selbeep          = data[OP_SEL_BEEP];
    _option.no_combi_v       = data[OP_NO_COMBI_V];
    _option.second_target    = data[OP_SECONDMBI_V];
    _option.number_color     = data[OP_NUMBER_COLOR];
    _option.O_count_lim      = data[OP_O_count_lim];
    _option.O_lit_count_lim  = data[OP_O_lit_count_lim];
    _option.nagamono_hosei   = data[OP_NAGAMONO_HOSEI];
    _option.waiting_limit    = data[OP_WAITING_LIMMIT];
    _option.selautozero      = data[OP_SELAUTOZERO];
    _option.kekka_hyouji     = data[OP_KEKKA_HYOUJI];
    _option.alart_autozero   = data[OP_ALART_AUTOZERO];
    _option.fix_value        = data[OP_FIX_VALUE];
}

/*****************************************************************************
    基本設定情報の上書き
*****************************************************************************/
void FileData::save_basic_setting(const basicSetting_t bscData){
    _basicData = bscData;
    FILE *sfp;

    if ((sfp = fopen(NAME_BASIC,"w")) == NULL) {
        fprintf(stderr, "%s write Err.\n", "BASIC");
        exit(EXIT_FAILURE);
    }

    fprintf(sfp,"%d %d\r\n"
        ,_basicData.startKinji
        ,_basicData.temperature
    );
    
    fclose(sfp);
    sfp = NULL;
}

/*****************************************************************************
    基本設定情報の読み取り
*****************************************************************************/
void FileData::read_basic_setting(){
    FILE *fp;

    if ((fp = fopen(NAME_BASIC,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "BASIC");
        //exit(EXIT_FAILURE);
        file_lost_sig += 16;
        _basicData.startKinji=12;
        _basicData.temperature=11;
        save_basic_setting(_basicData);
        //fclose(fp);
        fp = NULL;
    }else{
        fscanf(fp, "%d %d"
            ,&_basicData.startKinji     //準最適値開始する皿数
            ,&_basicData.temperature    //スパン1皿目の温度
        );
        
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}

/*****************************************************************************
    スパン値をローカルファイルに保存
*****************************************************************************/
/*
void FileData::save_span(){
    FILE *fp;
    
    if(( fp=fopen(NAME_SPAN,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "SPAN");
        exit(EXIT_FAILURE);
    }
    //ローカルファイルにパラメーターを書き込む
    for(int i = 0; i < MAX_HEAD; i++){
        fprintf(fp,"%f %f\r\n"
            ,_pSpan[i].sain
            ,_pSpan[i].fain
        );
    }//for
    
    fclose(fp);
    fp = NULL;
}*/

void FileData::save_span(){
    FILE *fp;
    
    if(( fp=fopen(NAME_SPAN3,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "SPAN3");
        if(( fp=fopen(NAME_SPAN,"w")) == NULL){
            fprintf(stderr, "%s write Err.\n", "SPAN");
            exit(EXIT_FAILURE);
        }
    }
    //ローカルファイルにパラメーターを書き込む
    for(int i = 0; i < MAX_HEAD; i++){
        printf("0\r");
        if(_pSpan[i].sain2 < 0 || 1< _pSpan[i].sain2 || span_hundo[1] == 0){ _pSpan[i].sain2 = 0;}
        if(_pSpan[i].sain3 < 0 || 1< _pSpan[i].sain3 || span_hundo[2] == 0){ _pSpan[i].sain3 = 0;}
        if(_pSpan[i].sain4 < 0 || 1< _pSpan[i].sain4 || span_hundo[3] == 0){ _pSpan[i].sain4 = 0;}
        if(_pSpan[i].sain5 < 0 || 1< _pSpan[i].sain5 || span_hundo[4] == 0){ _pSpan[i].sain5 = 0;}
        if(_pSpan[i].sain6 < 0 || 1< _pSpan[i].sain6 || span_hundo[5] == 0){ _pSpan[i].sain6 = 0;}
        if(_pSpan[i].sain7 < 0 || 1< _pSpan[i].sain7 || span_hundo[6] == 0){ _pSpan[i].sain7 = 0;}
        if(_pSpan[i].sain8 < 0 || 1< _pSpan[i].sain8 || span_hundo[7] == 0){ _pSpan[i].sain8 = 0;}
        if(_pSpan[i].sain9 < 0 || 1< _pSpan[i].sain9 || span_hundo[8] == 0){ _pSpan[i].sain9 = 0;}
        if(_pSpan[i].sain10 < 0 || 1< _pSpan[i].sain10 || span_hundo[9] == 0){ _pSpan[i].sain10 = 0;}
        fprintf(fp,"%f %f %f %f %f %f %f %f %f %f %f\r\n"
            ,_pSpan[i].sain
            ,_pSpan[i].fain
            ,_pSpan[i].sain2
            ,_pSpan[i].sain3
            ,_pSpan[i].sain4
            ,_pSpan[i].sain5
            ,_pSpan[i].sain6
            ,_pSpan[i].sain7
            ,_pSpan[i].sain8
            ,_pSpan[i].sain9
            ,_pSpan[i].sain10
        );
    }//for
    
    fclose(fp);
    fp = NULL;
}
/*****************************************************************************
    スパン値のファイルデータ読み込み
*****************************************************************************/
void FileData::read_span(){
    FILE *fp;
    
    // ファイルのオープン
    if ((fp = fopen(NAME_SPAN3,"r")) == NULL) {
        fprintf(stderr, "%s open Err.\n", "SPAN3");
        if ((fp = fopen(NAME_SPAN,"r")) == NULL) {
            fprintf(stderr, "%s open Err.\n", "SPAN");
            //exit(EXIT_FAILURE);
            file_lost_sig += 4;
            for(int i = 0; i < MAX_HEAD; i++){
                _pSpan[i].sain=0.350000;
                _pSpan[i].fain=0.300000;
                _pSpan[i].sain2=0.0;
                _pSpan[i].sain3=0.0;
                _pSpan[i].sain4=0.0;
                _pSpan[i].sain5=0.0;
                _pSpan[i].sain6=0.0;
                _pSpan[i].sain7=0.0;
                _pSpan[i].sain9=0.0;
                _pSpan[i].sain10=0.0;
            }//for
            save_span();
            //fclose(fp);
            fp = NULL;
        }else{
            //全ファイル読み出し
            for(int i = 0; i< MAX_HEAD; i++){
                fscanf(fp, "%f %f %f %f %f %f"
                    ,&_pSpan[i].sain
                    ,&_pSpan[i].fain
                    ,&_pSpan[i].sain2
                    ,&_pSpan[i].sain3
                    ,&_pSpan[i].sain4
                    ,&_pSpan[i].sain5
                    ,&_pSpan[i].sain6
                    ,&_pSpan[i].sain7
                    ,&_pSpan[i].sain8
                    ,&_pSpan[i].sain9
                    ,&_pSpan[i].sain10
                 );
                if(      _pSpan[i].sain  > _pSpan[i].sain2 || _pSpan[i].sain2 > 1 || span_hundo[1] == 0){
                    _pSpan[i].sain2 = 0.0;
                    _pSpan[i].sain3 = 0.0;
                    _pSpan[i].sain4 = 0.0;
                    _pSpan[i].sain5 = 0.0;
                    _pSpan[i].sain6 = 0.0;
                    _pSpan[i].sain7 = 0.0;
                    _pSpan[i].sain8 = 0.0;
                    _pSpan[i].sain9 = 0.0;
                    _pSpan[i].sain10 = 0.0;
                }else if(_pSpan[i].sain2 > _pSpan[i].sain3 || _pSpan[i].sain3 > 1 || span_hundo[2] == 0){
                    _pSpan[i].sain3 = 0.0;
                    _pSpan[i].sain4 = 0.0;
                    _pSpan[i].sain5 = 0.0;
                    _pSpan[i].sain6 = 0.0;
                    _pSpan[i].sain7 = 0.0;
                    _pSpan[i].sain8 = 0.0;
                    _pSpan[i].sain9 = 0.0;
                    _pSpan[i].sain10 = 0.0;
                }else if(_pSpan[i].sain3 > _pSpan[i].sain4 || _pSpan[i].sain4 > 1 || span_hundo[3] == 0){
                    _pSpan[i].sain4 = 0.0;
                    _pSpan[i].sain5 = 0.0;
                    _pSpan[i].sain6 = 0.0;
                    _pSpan[i].sain7 = 0.0;
                    _pSpan[i].sain8 = 0.0;
                    _pSpan[i].sain9 = 0.0;
                    _pSpan[i].sain10 = 0.0;
                }else if(_pSpan[i].sain4 > _pSpan[i].sain5 || _pSpan[i].sain5 > 1 || span_hundo[4] == 0){
                    _pSpan[i].sain5 = 0.0;
                    _pSpan[i].sain6 = 0.0;
                    _pSpan[i].sain7 = 0.0;
                    _pSpan[i].sain8 = 0.0;
                    _pSpan[i].sain9 = 0.0;
                    _pSpan[i].sain10 = 0.0;
                }else if(_pSpan[i].sain5 > _pSpan[i].sain6 || _pSpan[i].sain6 > 1 || span_hundo[5] == 0){
                    _pSpan[i].sain6 = 0.0;
                    _pSpan[i].sain7 = 0.0;
                    _pSpan[i].sain8 = 0.0;
                    _pSpan[i].sain9 = 0.0;
                    _pSpan[i].sain10 = 0.0;
                }else if(_pSpan[i].sain6 > _pSpan[i].sain7 || _pSpan[i].sain7 > 1 || span_hundo[6] == 0){
                    _pSpan[i].sain7 = 0.0;
                    _pSpan[i].sain8 = 0.0;
                    _pSpan[i].sain9 = 0.0;
                    _pSpan[i].sain10 = 0.0;
                }else if(_pSpan[i].sain7 > _pSpan[i].sain8 || _pSpan[i].sain8 > 1 || span_hundo[7] == 0){
                    _pSpan[i].sain8 = 0.0;
                    _pSpan[i].sain9 = 0.0;
                    _pSpan[i].sain10 = 0.0;
                }else if(_pSpan[i].sain8 > _pSpan[i].sain9 || _pSpan[i].sain9 > 1 || span_hundo[8] == 0){
                    _pSpan[i].sain9 = 0.0;
                    _pSpan[i].sain10 = 0.0;
                }else if(_pSpan[i].sain9 > _pSpan[i].sain10 || _pSpan[i].sain10 > 1 || span_hundo[9] == 0){
                    _pSpan[i].sain10 = 0.0;
                }
                if(_pSpan[i].sain2 > _pSpan[i].sain){
                    betwe_spa[i][0]  = _pSpan[i].sain + ((_pSpan[i].sain2 - _pSpan[i].sain)/2);
                    betwe_spa2[i][0] = _pSpan[i].sain + ((_pSpan[i].sain2 - _pSpan[i].sain)/10);
                }
                if(_pSpan[i].sain3 > _pSpan[i].sain2){
                    betwe_spa[i][1]  = _pSpan[i].sain2 + ((_pSpan[i].sain3 - _pSpan[i].sain2)/2);
                    betwe_spa2[i][1] = _pSpan[i].sain2 + ((_pSpan[i].sain3 - _pSpan[i].sain2)/10);
                }
                if(_pSpan[i].sain4 > _pSpan[i].sain3){
                    betwe_spa[i][2]  = _pSpan[i].sain3 + ((_pSpan[i].sain4 - _pSpan[i].sain3)/2);
                    betwe_spa2[i][2] = _pSpan[i].sain3 + ((_pSpan[i].sain4 - _pSpan[i].sain3)/10);
                }
                if(_pSpan[i].sain5 > _pSpan[i].sain4){
                    betwe_spa[i][3]  = _pSpan[i].sain4 + ((_pSpan[i].sain5 - _pSpan[i].sain4)/2);
                    betwe_spa2[i][3] = _pSpan[i].sain4 + ((_pSpan[i].sain5 - _pSpan[i].sain4)/10);
                }
                
                if(_pSpan[i].sain6 > _pSpan[i].sain5){
                    betwe_spa[i][4]  = _pSpan[i].sain5 + ((_pSpan[i].sain6 - _pSpan[i].sain5)/2);
                    betwe_spa2[i][4] = _pSpan[i].sain5 + ((_pSpan[i].sain6 - _pSpan[i].sain5)/10);
                }
                if(_pSpan[i].sain7 > _pSpan[i].sain6){
                    betwe_spa[i][5]  = _pSpan[i].sain6 + ((_pSpan[i].sain7 - _pSpan[i].sain6)/2);
                    betwe_spa2[i][5] = _pSpan[i].sain6 + ((_pSpan[i].sain7 - _pSpan[i].sain6)/10);
                }
                if(_pSpan[i].sain8 > _pSpan[i].sain7){
                    betwe_spa[i][6]  = _pSpan[i].sain7 + ((_pSpan[i].sain8 - _pSpan[i].sain7)/2);
                    betwe_spa2[i][6] = _pSpan[i].sain7 + ((_pSpan[i].sain8 - _pSpan[i].sain7)/10);
                }
                if(_pSpan[i].sain9 > _pSpan[i].sain8){
                    betwe_spa[i][7]  = _pSpan[i].sain8 + ((_pSpan[i].sain9 - _pSpan[i].sain8)/2);
                    betwe_spa2[i][7] = _pSpan[i].sain8 + ((_pSpan[i].sain9 - _pSpan[i].sain8)/10);
                }
                if(_pSpan[i].sain10 > _pSpan[i].sain9){
                    betwe_spa[i][8]  = _pSpan[i].sain9 + ((_pSpan[i].sain10 - _pSpan[i].sain9)/2);
                    betwe_spa2[i][8] = _pSpan[i].sain9 + ((_pSpan[i].sain10 - _pSpan[i].sain9)/10);
                }
            }//for
            //ファイルクローズ
            fclose(fp);
            fp = NULL;
        }
    }else{
            //全ファイル読み出し
        for(int i = 0; i< MAX_HEAD; i++){
            fscanf(fp, "%f %f %f %f %f %f %f %f %f %f %f"
                ,&_pSpan[i].sain
                ,&_pSpan[i].fain
                ,&_pSpan[i].sain2
                ,&_pSpan[i].sain3
                ,&_pSpan[i].sain4
                ,&_pSpan[i].sain5
                ,&_pSpan[i].sain6
                ,&_pSpan[i].sain7
                ,&_pSpan[i].sain8
                ,&_pSpan[i].sain9
                ,&_pSpan[i].sain10
             );
            if(      _pSpan[i].sain  > _pSpan[i].sain2 || _pSpan[i].sain2 > 1 || span_hundo[1] == 0){
                _pSpan[i].sain2 = 0.0;
                _pSpan[i].sain3 = 0.0;
                _pSpan[i].sain4 = 0.0;
                _pSpan[i].sain5 = 0.0;
                _pSpan[i].sain6 = 0.0;
                _pSpan[i].sain7 = 0.0;
                _pSpan[i].sain8 = 0.0;
                _pSpan[i].sain9 = 0.0;
                _pSpan[i].sain10 = 0.0;
            }else if(_pSpan[i].sain2 > _pSpan[i].sain3 || _pSpan[i].sain3 > 1 || span_hundo[2] == 0){
                _pSpan[i].sain3 = 0.0;
                _pSpan[i].sain4 = 0.0;
                _pSpan[i].sain5 = 0.0;
                _pSpan[i].sain6 = 0.0;
                _pSpan[i].sain7 = 0.0;
                _pSpan[i].sain8 = 0.0;
                _pSpan[i].sain9 = 0.0;
                _pSpan[i].sain10 = 0.0;
            }else if(_pSpan[i].sain3 > _pSpan[i].sain4 || _pSpan[i].sain4 > 1 || span_hundo[3] == 0){
                _pSpan[i].sain4 = 0.0;
                _pSpan[i].sain5 = 0.0;
                _pSpan[i].sain6 = 0.0;
                _pSpan[i].sain7 = 0.0;
                _pSpan[i].sain8 = 0.0;
                _pSpan[i].sain9 = 0.0;
                _pSpan[i].sain10 = 0.0;
            }else if(_pSpan[i].sain4 > _pSpan[i].sain5 || _pSpan[i].sain5 > 1 || span_hundo[4] == 0){
                _pSpan[i].sain5 = 0.0;
                _pSpan[i].sain6 = 0.0;
                _pSpan[i].sain7 = 0.0;
                _pSpan[i].sain8 = 0.0;
                _pSpan[i].sain9 = 0.0;
                _pSpan[i].sain10 = 0.0;
            }else if(_pSpan[i].sain5 > _pSpan[i].sain6 || _pSpan[i].sain6 > 1 || span_hundo[5] == 0){
                _pSpan[i].sain6 = 0.0;
                _pSpan[i].sain7 = 0.0;
                _pSpan[i].sain8 = 0.0;
                _pSpan[i].sain9 = 0.0;
                _pSpan[i].sain10 = 0.0;
            }else if(_pSpan[i].sain6 > _pSpan[i].sain7 || _pSpan[i].sain7 > 1 || span_hundo[6] == 0){
                _pSpan[i].sain7 = 0.0;
                _pSpan[i].sain8 = 0.0;
                _pSpan[i].sain9 = 0.0;
                _pSpan[i].sain10 = 0.0;
            }else if(_pSpan[i].sain7 > _pSpan[i].sain8 || _pSpan[i].sain8 > 1 || span_hundo[7] == 0){
                _pSpan[i].sain8 = 0.0;
                _pSpan[i].sain9 = 0.0;
                _pSpan[i].sain10 = 0.0;
            }else if(_pSpan[i].sain8 > _pSpan[i].sain9 || _pSpan[i].sain9 > 1 || span_hundo[8] == 0){
                _pSpan[i].sain9 = 0.0;
                _pSpan[i].sain10 = 0.0;
            }else if(_pSpan[i].sain9 > _pSpan[i].sain10 || _pSpan[i].sain10 > 1 || span_hundo[9] == 0){
                _pSpan[i].sain10 = 0.0;
            }
            if(_pSpan[i].sain2 > _pSpan[i].sain){
                betwe_spa[i][0]  = _pSpan[i].sain + ((_pSpan[i].sain2 - _pSpan[i].sain)/2);
                betwe_spa2[i][0] = _pSpan[i].sain + ((_pSpan[i].sain2 - _pSpan[i].sain)/10);
            }
            if(_pSpan[i].sain3 > _pSpan[i].sain2){
                betwe_spa[i][1]  = _pSpan[i].sain2 + ((_pSpan[i].sain3 - _pSpan[i].sain2)/2);
                betwe_spa2[i][1] = _pSpan[i].sain2 + ((_pSpan[i].sain3 - _pSpan[i].sain2)/10);
            }
            if(_pSpan[i].sain4 > _pSpan[i].sain3){
                betwe_spa[i][2]  = _pSpan[i].sain3 + ((_pSpan[i].sain4 - _pSpan[i].sain3)/2);
                betwe_spa2[i][2] = _pSpan[i].sain3 + ((_pSpan[i].sain4 - _pSpan[i].sain3)/10);
            }
            if(_pSpan[i].sain5 > _pSpan[i].sain4){
                betwe_spa[i][3]  = _pSpan[i].sain4 + ((_pSpan[i].sain5 - _pSpan[i].sain4)/2);
                betwe_spa2[i][3] = _pSpan[i].sain4 + ((_pSpan[i].sain5 - _pSpan[i].sain4)/10);
            }
            
            if(_pSpan[i].sain6 > _pSpan[i].sain5){
                betwe_spa[i][4]  = _pSpan[i].sain5 + ((_pSpan[i].sain6 - _pSpan[i].sain5)/2);
                betwe_spa2[i][4] = _pSpan[i].sain5 + ((_pSpan[i].sain6 - _pSpan[i].sain5)/10);
            }
            if(_pSpan[i].sain7 > _pSpan[i].sain6){
                betwe_spa[i][5]  = _pSpan[i].sain6 + ((_pSpan[i].sain7 - _pSpan[i].sain6)/2);
                betwe_spa2[i][5] = _pSpan[i].sain6 + ((_pSpan[i].sain7 - _pSpan[i].sain6)/10);
            }
            if(_pSpan[i].sain8 > _pSpan[i].sain7){
                betwe_spa[i][6]  = _pSpan[i].sain7 + ((_pSpan[i].sain8 - _pSpan[i].sain7)/2);
                betwe_spa2[i][6] = _pSpan[i].sain7 + ((_pSpan[i].sain8 - _pSpan[i].sain7)/10);
            }
            if(_pSpan[i].sain9 > _pSpan[i].sain8){
                betwe_spa[i][7]  = _pSpan[i].sain8 + ((_pSpan[i].sain9 - _pSpan[i].sain8)/2);
                betwe_spa2[i][7] = _pSpan[i].sain8 + ((_pSpan[i].sain9 - _pSpan[i].sain8)/10);
            }
            if(_pSpan[i].sain10 > _pSpan[i].sain9){
                betwe_spa[i][8]  = _pSpan[i].sain9 + ((_pSpan[i].sain10 - _pSpan[i].sain9)/2);
                betwe_spa2[i][8] = _pSpan[i].sain9 + ((_pSpan[i].sain10 - _pSpan[i].sain9)/10);
            }
            
            
            //printf("i = %2d\r\n",i);
            //printf("f = %7.5f\r\n",_pSpan[i].fain);
            //printf("1 = %7.5f:2 = %7.5f\r\n",_pSpan[i].sain,_pSpan[i].sain2);
            //printf("3 = %7.5f:4 = %7.5f\r\n",_pSpan[i].sain3,_pSpan[i].sain4);
            
        }//for
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
        
    }
}

void FileData::read_span_preset(int presetNo){
    char SAVEFILE[300];  //ファイル名合成用
    FILE *fp;
    sprintf(SAVEFILE,"/local/SPPR%02d.TXT", presetNo);
    if(( fp=fopen(SAVEFILE,"r")) == NULL){
        fprintf(stderr, "%s open Err.\n", "SPPR");
        //exit(EXIT_FAILURE);
        //file_lost_sig += 4;
        for(int i = 0; i < MAX_HEAD; i++){
            span_preset[i][0] = 0.3;
            span_preset[i][1] = 0.35;
        }//for
        save_span_preset(presetNo);
        //fclose(fp);
        fp = NULL;
    }else{
        //全ファイル読み出し
        for(int i = 0; i< MAX_HEAD; i++){
            fscanf(fp, "%f%f"
                ,&span_preset[i][0]
                ,&span_preset[i][1]
            );
            if((span_preset[i][0] > 1) || (span_preset[i][0] < 0)){
                span_preset[i][0] = 0.0000;
            }
            if((span_preset[i][1] > 1) || (span_preset[i][1] < 0)){
                span_preset[i][1] = span_preset[i][0] + 0.05;
            }
        }//for
        fscanf(fp, "%d"
            ,&span_hundo_preset
        );
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}

void FileData::save_span_preset(int presetNo){
    char SAVEFILE[300];  //ファイル名合成用
    FILE *fp;
    
    sprintf(SAVEFILE,"/local/SPPR%02d.TXT", presetNo);
    
    if(( fp=fopen(SAVEFILE,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "SPPR");
        exit(EXIT_FAILURE);
    }
    //ローカルファイルにパラメーターを書き込む
    for(int i = 0; i < MAX_HEAD; i++){
        fprintf(fp,"%f  %f\r\n"
            ,span_preset[i][0]
            ,span_preset[i][1]
        );
        /*
        fprintf(fp,"%d\r\n"
            ,span_hundo_preset
        );
        */
    }//for
    fprintf(fp,"%d\r\n"
        ,span_hundo_preset
    );
    fclose(fp);
    fp = NULL;
}

/*****************************************************************************
    スパンテスト用分銅値読み込み
*****************************************************************************/
void FileData::read_spanc(){
    FILE *fp;
    
    // ファイルのオープン
    if ((fp = fopen(NAME_SPANC,"r")) == NULL) {
        fprintf(stderr, "%s open Err.\n", "SPAN");
        //exit(EXIT_FAILURE);
        //fclose(fp);
        fp = NULL;
    }else{
        //全ファイル読み出し
        for(int i = 0; i< 50; i++){
            fscanf(fp, "%d"
                ,&test_target[i]
            );
            if(test_target[i] < 0 || test_target[i] > 10000){
                 test_target[i] = 0; 
            }
            printf("%5d\r\n",test_target[i]);
        }//for
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
        
            printf("%5d\r\n",test_target[0]);
    }
}


void FileData::save_hosei(int presetNo){
    char SAVEFILE[300];  //ファイル名合成用
    FILE *fp;
    
    sprintf(SAVEFILE,"/local/HOSE%02d.TXT", presetNo);
    
    if(( fp=fopen(SAVEFILE,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "hosei");
        exit(EXIT_FAILURE);
    }
    //ローカルファイルにパラメーターを書き込む
    for(int i = 0; i < MAX_HEAD; i++){
        fprintf(fp,"%2d\r\n"
            ,hosei[i]
        );
        //fprintf(fp,"%3.1f\r\n"
        //    ,hosei[i]
        //);
    }//for
    
    for(int i = 0; i < MAX_HEAD; i++){
        printf("%d\r\n"
            ,hosei[i]
        );
    }//for
    fclose(fp);
    fp = NULL;
}


void FileData::read_hosei(int presetNo){
    char SAVEFILE[300];  //ファイル名合成用
    FILE *fp;
    sprintf(SAVEFILE,"/local/HOSE%02d.TXT", presetNo);
    if(( fp=fopen(SAVEFILE,"r")) == NULL){
        fprintf(stderr, "%s open Err.\n", "HOSEI");
        //exit(EXIT_FAILURE);
        //file_lost_sig += 4;
        for(int i = 0; i < MAX_HEAD; i++){
            hosei[i] = 0;
            hosei_f[i] = float(hosei[i])/float(10);
        }//for
        save_span_hundo();
        //fclose(fp);
        fp = NULL;
    }else{
        //全ファイル読み出し
        for(int i = 0; i< MAX_HEAD; i++){
            fscanf(fp, "%d"
                ,&hosei[i]
            );
            hosei_f[i] = float(hosei[i])/float(10);
            //fscanf(fp, "%f"
            //    ,&hosei[i]
            //);
        }//for
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}













/*****************************************************************************
    スパン値をローカルファイルに保存
*****************************************************************************/
void FileData::save_span_hundo(){
    FILE *fp;
    
    if(( fp=fopen(NAME_SPAN2,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "SPAN");
        exit(EXIT_FAILURE);
    }
    //ローカルファイルにパラメーターを書き込む
    for(int i = 0; i < sphun_count ; i++){
        fprintf(fp,"%d\r\n"
            ,span_hundo[i]
        );
    }//for
    
    fclose(fp);
    fp = NULL;
}

/*****************************************************************************
    スパン値のファイルデータ読み込み
*****************************************************************************/
void FileData::read_span_hundo(){
    FILE *fp;
    
    // ファイルのオープン
    if ((fp = fopen(NAME_SPAN2,"r")) == NULL) {
        fprintf(stderr, "%s open Err.\n", "SPAN");
        //exit(EXIT_FAILURE);
        file_lost_sig += 4;
        for(int i = 0; i < sphun_count ; i++){
            span_hundo[i] = ( i + 1 ) * 100;
        }//for
        save_span_hundo();
        //fclose(fp);
        fp = NULL;
    }else{
        //全ファイル読み出し
        for(int i = 0; i< sphun_count; i++){
            fscanf(fp, "%d"
                ,&span_hundo[i]
             );
        }//for
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}


/*****************************************************************************
    ヘッド毎のバッファ値をファイルに保存
*****************************************************************************/
void FileData::save_nagamono_buffer(){
    FILE *fp;
    
    if(( fp=fopen(NAME_BUFFER,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "SPAN");
        exit(EXIT_FAILURE);
    }
    //ローカルファイルにパラメーターを書き込む
    for(int i = 0; i < MAX_HEAD; i++){
        fprintf(fp,"%d\r\n"
            ,op_h_buffer1[i]
        );
    }//for
    
    fclose(fp);
    fp = NULL;
}

void FileData::read_nagamono_buffer(){
    FILE *fp;
    
    // ファイルのオープン
    if ((fp = fopen(NAME_BUFFER,"r")) == NULL) {
        fprintf(stderr, "%s open Err.\n", "SPAN");
        //exit(EXIT_FAILURE);
        file_lost_sig += 4;
        for(int i = 0; i < MAX_HEAD; i++){
            op_h_buffer1[i] = 0;
        }//for
        save_nagamono_buffer();
        //fclose(fp);
        fp = NULL;
    }else{
        //全ファイル読み出し
        for(int i = 0; i< MAX_HEAD; i++){
            fscanf(fp, "%d"
                ,&op_h_buffer1[i]
             );
        }//for
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
        
        //for(int i = 0; i< MAX_HEAD; i++){
        //    op_h_buffer2[i] = float(op_h_buffer1[i])/float(10);
        //}
    }
}

/*****************************************************************************
    名前値をローカルファイルに保存
*****************************************************************************/
void FileData::save_name(){
    FILE *fp;
    
    if(( fp=fopen(NAME_NAME,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "name");
        exit(EXIT_FAILURE);
    }

    //ローカルファイルにパラメーターを書き込む
    for(int i = 0; i < MAX_HEAD*2; i++){
        fprintf(fp,"%4d %4d %4d %4d %4d %4d %4d %4d %4d %4d\r\n"
            ,name[i][0]
            ,name[i][1]
            ,name[i][2]
            ,name[i][3]
            ,name[i][4]
            ,name[i][5]
            ,name[i][6]
            ,name[i][7]
            ,name[i][8]
            ,name[i][9]
        );
    }//for
    
    fclose(fp);
    fp = NULL;
}

/*****************************************************************************
    名前値のファイルデータ読み込み
*****************************************************************************/
void FileData::read_name(){
    FILE *fp;
    
    // ファイルのオープン
    if ((fp = fopen(NAME_NAME,"r")) == NULL) {
        fprintf(stderr, "%s open Err.\n", "name");
        //exit(EXIT_FAILURE);
        file_lost_sig += 128;
        for(int i = 0; i< MAX_HEAD*2; i++){
            if(i==9||i==21||i==24){
                name[i][0]=4;
                name[i][1]=5;
                name[i][2]=6;
                name[i][3]=1;
                name[i][4]=21;
                name[i][5]=12;
                name[i][6]=20;
                name[i][7]=-40;
                name[i][8]=-40;
                name[i][9]=-40;
            }else{
                name[i][0]=1;
                name[i][1]=2;
                name[i][2]=3;
                name[i][3]=4;
                name[i][4]=-40;
                name[i][5]=-40;
                name[i][6]=-40;
                name[i][7]=-40;
                name[i][8]=-40;
                name[i][9]=-40;
            }
        }
        save_name();
        //fclose(fp);
        fp = NULL;
    }else{
        //全ファイル読み出し
        for(int i = 0; i< MAX_HEAD*2; i++){
            fscanf(fp, "%d %d %d %d %d %d %d %d %d %d"
                ,&name[i][0]
                ,&name[i][1]
                ,&name[i][2]
                ,&name[i][3]
                ,&name[i][4]
                ,&name[i][5]
                ,&name[i][6]
                ,&name[i][7]
                ,&name[i][8]
                ,&name[i][9]
            );
            //printf("%3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\r\n",name[i][0],name[i][1],name[i][2],name[i][3],name[i][4],name[i][5],name[i][6],name[i][7],name[i][8],name[i][9]);   
        }//for
        
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}

//変更されたロードセルデータをset
void FileData::set_load(const int data[], int head){
    _pCel[head].kumi_flag = data[TL_KUMI_FLAG];
    _pCel[head].kumi_min = data[TL_KUMI_MIN];
    _pCel[head].kumi_max = data[TL_KUMI_MAX];
    _pCel[head].katamen  = data[TL_KATAMEN_HYOUJI];
    _pCel[head].target = data[TL_TARGET];
    _pCel[head].jougen = data[TL_JOUGEN];
    _pCel[head].taimenTarget = data[TL_TAIMEN_TARGET];
    _pCel[head].taimenJougen = data[TL_TAIMEN_JOUGEN];
    _pCel[head].fuzoroiBoushi = data[TL_FUZOROI_BOUSHI];
    _pCel[head].secondOnOff = data[TL_SECOND_ON_OFF];
    _pCel[head].spMode = data[TL_SPMODE];
    _pCel[head].lightTime = data[TL_HYOUJI];
    _pCel[head].rankOnOff = data[TL_RANK_ON_OFF];
    _pCel[head].rankMin = data[TL_RANK_MIN];
    _pCel[head].rankMak = data[TL_RANK_MAX];
    _pCel[head].kumi_priolity_1 = data[TL_KUMI_PRIORITY_1];
    _pCel[head].kumi_priolity_2 = data[TL_KUMI_PRIORITY_2];
    _pCel[head].tanjuu = data[TL_TANJU];
    _pCel[head].check_hyouji  = data[TL_CHECK_HYOUUJI];
    _pCel[head].combiflug  = data[TL_COMBI_FLAG];
    _pCel[head].kuri_flag  = data[TL_KURI_FLAG];
    
    
}
/*****************************************************************************
    各設定したパラメーターをローカルファイルに保存
*****************************************************************************/
void FileData::save_loadcell(){
    FILE *fp;
    
    if(( fp=fopen(NAME_CELL,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "CELL");
        exit(EXIT_FAILURE);
    }

    //ローカルファイルにパラメーターを書き込む
    for(int i = 0; i < MAX_HEAD*2; i++){
        fprintf(fp,"%5d %3d %2d  %d  %2d  %2d %2d  %d  %d %3d %3d  %d %6d %5d %3d %2d %2d %4d %2d   %2d %2d\r\n"
            ,_pCel[i].target
            ,_pCel[i].jougen
            ,_pCel[i].kumi_flag
            ,_pCel[i].katamen
            ,_pCel[i].kumi_min
            ,_pCel[i].kumi_max
            ,_pCel[i].lightTime
            ,_pCel[i].spMode
            ,_pCel[i].rankOnOff
            ,_pCel[i].rankMin
            ,_pCel[i].rankMak
            ,_pCel[i].secondOnOff
            ,_pCel[i].taimenTarget
            ,_pCel[i].taimenJougen
            ,_pCel[i].fuzoroiBoushi
            ,_pCel[i].kumi_priolity_1
            ,_pCel[i].kumi_priolity_2
            ,_pCel[i].tanjuu
            ,_pCel[i].check_hyouji
            ,_pCel[i].combiflug
            ,_pCel[i].kuri_flag
        );
    }//for
        fprintf(fp,"  tar jou fl ka kmi kma lt sp ra rmi rma ju   tTar  tJou  fu p1 p2  tan chyo combi  kuriflg\r\n");
    
    fclose(fp);
    fp = NULL;
}

/*****************************************************************************
    ロードセルのファイルデータ読み込み
*****************************************************************************/
void FileData::read_loadcell(){
    FILE *fp;
    
    // ファイルのオープン
    if ((fp = fopen(NAME_CELL,"r")) == NULL) {
        fprintf(stderr, "%s open Err.\n", "CELL");
        //exit(EXIT_FAILURE);
        file_lost_sig += 8;
        //fclose(fp);
        fp = NULL;
    }else{
        //全ファイル読み出し
        for(int i = 0; i< MAX_HEAD*2; i++){
            fscanf(fp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
                ,&_pCel[i].target
                ,&_pCel[i].jougen
                ,&_pCel[i].kumi_flag
                ,&_pCel[i].katamen
                ,&_pCel[i].kumi_min
                ,&_pCel[i].kumi_max
                ,&_pCel[i].lightTime
                ,&_pCel[i].spMode
                ,&_pCel[i].rankOnOff
                ,&_pCel[i].rankMin
                ,&_pCel[i].rankMak
                ,&_pCel[i].secondOnOff
                ,&_pCel[i].taimenTarget
                ,&_pCel[i].taimenJougen
                ,&_pCel[i].fuzoroiBoushi
                ,&_pCel[i].kumi_priolity_1
                ,&_pCel[i].kumi_priolity_2
                ,&_pCel[i].tanjuu
                ,&_pCel[i].check_hyouji
                ,&_pCel[i].combiflug
                ,&_pCel[i].kuri_flag
             );
        }//for
        
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}

/*****************************************************************************
    データファイル読み込み
*****************************************************************************/
void FileData::read_data(const char *fileName){
    FILE *fp;
    
    // ファイルのオープン
    if ((fp = fopen(fileName,"r")) == NULL) {
        fprintf(stderr, "%s open Err.\n", "DATA");
        //exit(EXIT_FAILURE);
        file_lost_sig += 2048;
        //fclose(fp);
        fp = NULL;
    }else{
        //全ファイル読み出し
        for(int i = 0; i < NUM_DATA; i++){
            fscanf(fp, "%d %d %d"
                ,&_pData[i].min
                ,&_pData[i].max
                ,&_pData[i].lcd
             );
        }//for
        
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}

/*****************************************************************************
    データファイル読み込み
*****************************************************************************/
void FileData::read_SData(){
    FILE *fp;
    //bool data_err = false;
    // ファイルのオープン
    if ((fp = fopen(NAME_SDATA,"r")) == NULL) {
        fprintf(stderr, "%s open Err.\n", "SDATA");
        //exit(EXIT_FAILURE);
        file_lost_sig += 32;
    }else{
        //全ファイル読み出し
        for(int i = 0; i < NUM_SDATA; i++){
            fscanf(fp, "%d %d %d"
                ,&_pSData[i].min
                ,&_pSData[i].max
                ,&_pSData[i].lcd
             );
             //printf("%3d %3d %3d %3d",i,_pSData[i].min,_pSData[i].max,_pSData[i].lcd);
                     //読みだした値が異常でないか確認
                     
            if(i == P_liteLedOff){       
                if(_pSData[i].min < -30 || 1 <_pSData[i].min ){
                 _pSData[i].min=-30;
                }else if(_pSData[i].min < 0){
                 _pSData[i].min=0;
                }
            }
            if(_pSData[i].max > 10000){
                _pSData[i].max=9000;
            }
            if(_pSData[i].max < _pSData[i].min){
                _pSData[i].max = _pSData[i].min+1;
            }
            if(_pSData[i].lcd < 0){     _pSData[i].lcd = 0;     }
            if(_pSData[i].lcd > 1000){  _pSData[i].lcd = 15;    }
            
            
             //printf("%3d %3d %3d %3d\r\n",i,_pSData[i].min,_pSData[i].max,_pSData[i].lcd);
                 
        }//for
        
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}
/*****************************************************************************
    ファイルが消えた場合に記録する
*****************************************************************************/
void FileData::error_log(int err_no){
    FILE *fp;
    
    if(( fp=fopen(ERROR_LOG,"a")) == NULL){
        fprintf(stderr, "%s write Err.\n", "nerror");
        exit(EXIT_FAILURE);
    }

    //ローカルファイルにパラメーターを書き込む
    if(err_no!=0){
        fprintf(fp,"%4d\r\n",err_no);
    }else{
        fprintf(fp,"\r\n\r\n\r\n");
    }
    
    
    
    
    
    fclose(fp);
    fp = NULL;
}


//目標設定する項目の最小値、最大値などを取得する
data_t FileData::get_limit_data(int num){
    return _pData[num];
}

//目標設定する項目の最小値、最大値などを取得する
data_t FileData::get_limit_sdata(int num){
    return _pSData[num];
}

//変更されたロードセルデータをset
void FileData::set_span(const span_t* s){
    for(int i = 0; i < _product.head; i++){
        _pSpan[i] = s[i];
    }
}
//変更された1個のロードセルデータをset
void FileData::set_span2(const span_t* s,int i){
    _pSpan[i] = s[i];
}



/*****************************************************************************
    複数重量範囲をローカルファイルに保存
*****************************************************************************/
void FileData::save_unitweight(){
    FILE *fp;
    
    if(( fp=fopen(NAME_MCELL,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "MCELL");
        exit(EXIT_FAILURE);
    }

    //ローカルファイルにパラメーターを書き込む
    for(int i = 0; i < MAX_HEAD*2; i++){
        //rintf(fp,"%d %3d %3d %2d %2d %5d %5d   %d %3d %3d %2d %2d %5d %5d   %d %3d %3d %2d %2d %5d %5d   %d %3d %3d %2d %2d %5d %5d   %d %3d %3d %2d %2d %5d %5d   %d %3d %3d %2d %2d %5d %5d   %d %3d %3d %2d %2d %5d %5d   %d %3d %3d %2d %2d %5d %5d   %d %3d %3d %2d %2d %5d %5d   %d %3d %3d %2d %2d %5d %5d   %d  %d  %d\r\n"
        //fprintf(fp,"%d %3d %6.2f %6.2f %2d %2d %5d %5d   %d %3d %6.2f %6.2f %2d %2d %5d %5d   %d %3d %6.2f %6.2f %2d %2d %5d %5d   %d %3d %6.2f %6.2f %2d %2d %5d %5d   %d %3d %6.2f %6.2f %2d %2d %5d %5d   %d %3d %6.2f %6.2f %2d %2d %5d %5d   %d %3d %6.2f %6.2f %2d %2d %5d %5d   %d %3d %6.2f %6.2f %2d %2d %5d %5d   %d %3d %6.2f %6.2f %2d %2d %5d %5d   %d %3d %6.2f %6.2f %2d %2d %5d %5d   %d  %d  %d\r\n"
        fprintf(fp,"%d %3d %7d %7d %2d %2d %5d %5d   %d %3d %7d %7d %2d %2d %5d %5d   %d %3d %7d %7d %2d %2d %5d %5d   %d %3d %7d %7d %2d %2d %5d %5d   %d %3d %7d %7d %2d %2d %5d %5d   %d %3d %7d %7d %2d %2d %5d %5d   %d %3d %7d %7d %2d %2d %5d %5d   %d %3d %7d %7d %2d %2d %5d %5d   %d %3d %7d %7d %2d %2d %5d %5d   %d %3d %7d %7d %2d %2d %5d %5d   %d  %d  %d\r\n"
        //fprintf(fp,"%d %6.2f %6.2f %2d %2d %5d %5d   %d %6.2f %6.2f %2d %2d %5d %5d   %d %6.2f %6.2f %2d %2d %5d %5d   %d %6.2f %6.2f %2d %2d %5d %5d   %d %6.2f %6.2f %2d %2d %5d %5d   %d %6.2f %6.2f %2d %2d %5d %5d   %d %6.2f %6.2f %2d %2d %5d %5d   %d %6.2f %6.2f %2d %2d %5d %5d   %d %6.2f %6.2f %2d %2d %5d %5d   %d %6.2f %6.2f %2d %2d %5d %5d   %d %6.2f %6.2f %2d %2d %5d %5d   %d  %d  %d\r\n"
        
        ,_pMweight[i].u01.rank_en
        ,_pMweight[i].u01.rank_no
        ,_pMweight[i].u01.rank_min
        ,_pMweight[i].u01.rank_max
        ,_pMweight[i].u01.kumi_min
        ,_pMweight[i].u01.kumi_max
        ,_pMweight[i].u01.target
        ,_pMweight[i].u01.jougen

        ,_pMweight[i].u02.rank_en
        ,_pMweight[i].u02.rank_no
        ,_pMweight[i].u02.rank_min
        ,_pMweight[i].u02.rank_max
        ,_pMweight[i].u02.kumi_min
        ,_pMweight[i].u02.kumi_max
        ,_pMweight[i].u02.target
        ,_pMweight[i].u02.jougen

        ,_pMweight[i].u03.rank_en
        ,_pMweight[i].u03.rank_no
        ,_pMweight[i].u03.rank_min
        ,_pMweight[i].u03.rank_max
        ,_pMweight[i].u03.kumi_min
        ,_pMweight[i].u03.kumi_max
        ,_pMweight[i].u03.target
        ,_pMweight[i].u03.jougen
        
        ,_pMweight[i].u04.rank_en
        ,_pMweight[i].u04.rank_no
        ,_pMweight[i].u04.rank_min
        ,_pMweight[i].u04.rank_max
        ,_pMweight[i].u04.kumi_min
        ,_pMweight[i].u04.kumi_max
        ,_pMweight[i].u04.target
        ,_pMweight[i].u04.jougen
         
        ,_pMweight[i].u05.rank_en
        ,_pMweight[i].u05.rank_no
        ,_pMweight[i].u05.rank_min
        ,_pMweight[i].u05.rank_max
        ,_pMweight[i].u05.kumi_min
        ,_pMweight[i].u05.kumi_max
        ,_pMweight[i].u05.target
        ,_pMweight[i].u05.jougen
         
        ,_pMweight[i].u06.rank_en
        ,_pMweight[i].u06.rank_no
        ,_pMweight[i].u06.rank_min
        ,_pMweight[i].u06.rank_max
        ,_pMweight[i].u06.kumi_min
        ,_pMweight[i].u06.kumi_max
        ,_pMweight[i].u06.target
        ,_pMweight[i].u06.jougen
         
        ,_pMweight[i].u07.rank_en
        ,_pMweight[i].u07.rank_no
        ,_pMweight[i].u07.rank_min
        ,_pMweight[i].u07.rank_max
        ,_pMweight[i].u07.kumi_min
        ,_pMweight[i].u07.kumi_max
        ,_pMweight[i].u07.target
        ,_pMweight[i].u07.jougen
         
        ,_pMweight[i].u08.rank_en
        ,_pMweight[i].u08.rank_no
        ,_pMweight[i].u08.rank_min
        ,_pMweight[i].u08.rank_max
        ,_pMweight[i].u08.kumi_min
        ,_pMweight[i].u08.kumi_max
        ,_pMweight[i].u08.target
        ,_pMweight[i].u08.jougen
         
        ,_pMweight[i].u09.rank_en
        ,_pMweight[i].u09.rank_no
        ,_pMweight[i].u09.rank_min
        ,_pMweight[i].u09.rank_max
        ,_pMweight[i].u09.kumi_min
        ,_pMweight[i].u09.kumi_max
        ,_pMweight[i].u09.target
        ,_pMweight[i].u09.jougen
         
        ,_pMweight[i].u10.rank_en
        ,_pMweight[i].u10.rank_no
        ,_pMweight[i].u10.rank_min
        ,_pMweight[i].u10.rank_max
        ,_pMweight[i].u10.kumi_min
        ,_pMweight[i].u10.kumi_max
        ,_pMweight[i].u10.target
        ,_pMweight[i].u10.jougen
        
        //,_pMweight[i].u11.rank_en
        //,_pMweight[i].u11.rank_min
        //,_pMweight[i].u11.rank_max
        //,_pMweight[i].u11.kumi_min
        //,_pMweight[i].u11.kumi_max
        //,_pMweight[i].u11.target
        //,_pMweight[i].u11.jougen
        //,_pMweight[i].priority
        ,_pMweight[i].Rcolor
        ,_pMweight[i].Rsort
        ,_pMweight[i].Rselect
        );
    }//for
    
    fclose(fp);
    fp = NULL;
}

/*****************************************************************************
    複数重量範囲のファイルデータ読み込み
*****************************************************************************/
void FileData::read_unitweight(){
    FILE *fp;
    
    // ファイルのオープン
    if ((fp = fopen(NAME_MCELL,"r")) == NULL) {
        fprintf(stderr, "%s open Err.\n", "MCELL");
        //exit(EXIT_FAILURE);
        file_lost_sig += 64;
        //fclose(fp);
        fp = NULL;
    }else{
        //全ファイル読み出し
        for(int i = 0; i< MAX_HEAD*2; i++){
            //fscanf(fp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
            //fscanf(fp, "%d %d %f %f %d %d %d %d %d %d %f %f %d %d %d %d %d %d %f %f %d %d %d %d %d %d %f %f %d %d %d %d %d %d %f %f %d %d %d %d %d %d %f %f %d %d %d %d %d %d %f %f %d %d %d %d %d %d %f %f %d %d %d %d %d %d %f %f %d %d %d %d %d %d %f %f %d %d %d %d %d %d %d"
            fscanf(fp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
            //fscanf(fp, "%d %f %f %d %d %d %d %d %f %f %d %d %d %d %d %f %f %d %d %d %d %d %f %f %d %d %d %d %d %f %f %d %d %d %d %d %f %f %d %d %d %d %d %f %f %d %d %d %d %d %f %f %d %d %d %d %d %f %f %d %d %d %d %d %f %f %d %d %d %d %d %f %f %d %d %d %d %d %d %d"
            ,&_pMweight[i].u01.rank_en
            ,&_pMweight[i].u01.rank_no
            ,&_pMweight[i].u01.rank_min
            ,&_pMweight[i].u01.rank_max
            ,&_pMweight[i].u01.kumi_min
            ,&_pMweight[i].u01.kumi_max
            ,&_pMweight[i].u01.target
            ,&_pMweight[i].u01.jougen
    
            ,&_pMweight[i].u02.rank_en
            ,&_pMweight[i].u02.rank_no
            ,&_pMweight[i].u02.rank_min
            ,&_pMweight[i].u02.rank_max
            ,&_pMweight[i].u02.kumi_min
            ,&_pMweight[i].u02.kumi_max
            ,&_pMweight[i].u02.target
            ,&_pMweight[i].u02.jougen
    
            ,&_pMweight[i].u03.rank_en
            ,&_pMweight[i].u03.rank_no
            ,&_pMweight[i].u03.rank_min
            ,&_pMweight[i].u03.rank_max
            ,&_pMweight[i].u03.kumi_min
            ,&_pMweight[i].u03.kumi_max
            ,&_pMweight[i].u03.target
            ,&_pMweight[i].u03.jougen
            
            ,&_pMweight[i].u04.rank_en
            ,&_pMweight[i].u04.rank_no
            ,&_pMweight[i].u04.rank_min
            ,&_pMweight[i].u04.rank_max
            ,&_pMweight[i].u04.kumi_min
            ,&_pMweight[i].u04.kumi_max
            ,&_pMweight[i].u04.target
            ,&_pMweight[i].u04.jougen
             
            ,&_pMweight[i].u05.rank_en
            ,&_pMweight[i].u05.rank_no
            ,&_pMweight[i].u05.rank_min
            ,&_pMweight[i].u05.rank_max
            ,&_pMweight[i].u05.kumi_min
            ,&_pMweight[i].u05.kumi_max
            ,&_pMweight[i].u05.target
            ,&_pMweight[i].u05.jougen
             
            ,&_pMweight[i].u06.rank_en
            ,&_pMweight[i].u06.rank_no
            ,&_pMweight[i].u06.rank_min
            ,&_pMweight[i].u06.rank_max
            ,&_pMweight[i].u06.kumi_min
            ,&_pMweight[i].u06.kumi_max
            ,&_pMweight[i].u06.target
            ,&_pMweight[i].u06.jougen
             
            ,&_pMweight[i].u07.rank_en
            ,&_pMweight[i].u07.rank_no
            ,&_pMweight[i].u07.rank_min
            ,&_pMweight[i].u07.rank_max
            ,&_pMweight[i].u07.kumi_min
            ,&_pMweight[i].u07.kumi_max
            ,&_pMweight[i].u07.target
            ,&_pMweight[i].u07.jougen
             
            ,&_pMweight[i].u08.rank_en
            ,&_pMweight[i].u08.rank_no
            ,&_pMweight[i].u08.rank_min
            ,&_pMweight[i].u08.rank_max
            ,&_pMweight[i].u08.kumi_min
            ,&_pMweight[i].u08.kumi_max
            ,&_pMweight[i].u08.target
            ,&_pMweight[i].u08.jougen
             
            ,&_pMweight[i].u09.rank_en
            ,&_pMweight[i].u09.rank_no
            ,&_pMweight[i].u09.rank_min
            ,&_pMweight[i].u09.rank_max
            ,&_pMweight[i].u09.kumi_min
            ,&_pMweight[i].u09.kumi_max
            ,&_pMweight[i].u09.target
            ,&_pMweight[i].u09.jougen
             
            ,&_pMweight[i].u10.rank_en
            ,&_pMweight[i].u10.rank_no
            ,&_pMweight[i].u10.rank_min
            ,&_pMweight[i].u10.rank_max
            ,&_pMweight[i].u10.kumi_min
            ,&_pMweight[i].u10.kumi_max
            ,&_pMweight[i].u10.target
            ,&_pMweight[i].u10.jougen
             
            //,&_pMweight[i].u11.rank_en
            //,&_pMweight[i].u11.rank_min
            //,&_pMweight[i].u11.rank_max
            //,&_pMweight[i].u11.kumi_min
            //,&_pMweight[i].u11.kumi_max
           // ,&_pMweight[i].u11.target
            //,&_pMweight[i].u11.jougen
            //,&_pMweight[i].priority
            ,&_pMweight[i].Rcolor
            ,&_pMweight[i].Rsort
            ,&_pMweight[i].Rselect
             );
        }//for
        
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}



//変更された複数重量をset
void FileData::set_lweight(const int data[], int head, int i, int data_f, int data_r, int data_s){
    if(i==0){
        _pMweight[head].u01.rank_en  = data[TL2_RANK_EN];
        _pMweight[head].u01.rank_no  = data[TL2_RANK_NO];
        _pMweight[head].u01.rank_min = data[TL2_RANK_MIN];
        _pMweight[head].u01.rank_max = data[TL2_RANK_MAX];
        _pMweight[head].u01.kumi_min = data[TL2_KUMI_MIN];
        _pMweight[head].u01.kumi_max = data[TL2_KUMI_MAX];
        _pMweight[head].u01.target   = data[TL2_TARGET];
        _pMweight[head].u01.jougen   = data[TL2_JOUGEN];
    }else if(i==1){
        _pMweight[head].u02.rank_en  = data[TL2_RANK_EN];
        _pMweight[head].u02.rank_no  = data[TL2_RANK_NO];
        _pMweight[head].u02.rank_min = data[TL2_RANK_MIN];
        _pMweight[head].u02.rank_max = data[TL2_RANK_MAX];
        _pMweight[head].u02.kumi_min = data[TL2_KUMI_MIN];
        _pMweight[head].u02.kumi_max = data[TL2_KUMI_MAX];
        _pMweight[head].u02.target   = data[TL2_TARGET];
        _pMweight[head].u02.jougen   = data[TL2_JOUGEN];
    }else if(i==2){    
        _pMweight[head].u03.rank_en  = data[TL2_RANK_EN];
        _pMweight[head].u03.rank_no  = data[TL2_RANK_NO];
        _pMweight[head].u03.rank_min = data[TL2_RANK_MIN];
        _pMweight[head].u03.rank_max = data[TL2_RANK_MAX];
        _pMweight[head].u03.kumi_min = data[TL2_KUMI_MIN];
        _pMweight[head].u03.kumi_max = data[TL2_KUMI_MAX];
        _pMweight[head].u03.target   = data[TL2_TARGET];
        _pMweight[head].u03.jougen   = data[TL2_JOUGEN];
    }else if(i==3){
        _pMweight[head].u04.rank_en  = data[TL2_RANK_EN];
        _pMweight[head].u04.rank_no  = data[TL2_RANK_NO];
        _pMweight[head].u04.rank_min = data[TL2_RANK_MIN];
        _pMweight[head].u04.rank_max = data[TL2_RANK_MAX];
        _pMweight[head].u04.kumi_min = data[TL2_KUMI_MIN];
        _pMweight[head].u04.kumi_max = data[TL2_KUMI_MAX];
        _pMweight[head].u04.target   = data[TL2_TARGET];
        _pMweight[head].u04.jougen   = data[TL2_JOUGEN];
    }else if(i==4){
        _pMweight[head].u05.rank_en  = data[TL2_RANK_EN];
        _pMweight[head].u05.rank_no  = data[TL2_RANK_NO];
        _pMweight[head].u05.rank_min = data[TL2_RANK_MIN];
        _pMweight[head].u05.rank_max = data[TL2_RANK_MAX];
        _pMweight[head].u05.kumi_min = data[TL2_KUMI_MIN];
        _pMweight[head].u05.kumi_max = data[TL2_KUMI_MAX];
        _pMweight[head].u05.target   = data[TL2_TARGET];
        _pMweight[head].u05.jougen   = data[TL2_JOUGEN];
    }else if(i==5){
        _pMweight[head].u06.rank_en  = data[TL2_RANK_EN];
        _pMweight[head].u06.rank_no  = data[TL2_RANK_NO];
        _pMweight[head].u06.rank_min = data[TL2_RANK_MIN];
        _pMweight[head].u06.rank_max = data[TL2_RANK_MAX];
        _pMweight[head].u06.kumi_min = data[TL2_KUMI_MIN];
        _pMweight[head].u06.kumi_max = data[TL2_KUMI_MAX];
        _pMweight[head].u06.target   = data[TL2_TARGET];
        _pMweight[head].u06.jougen   = data[TL2_JOUGEN];
    }else if(i==6){
        _pMweight[head].u07.rank_en  = data[TL2_RANK_EN];
        _pMweight[head].u07.rank_no  = data[TL2_RANK_NO];
        _pMweight[head].u07.rank_min = data[TL2_RANK_MIN];
        _pMweight[head].u07.rank_max = data[TL2_RANK_MAX];
        _pMweight[head].u07.kumi_min = data[TL2_KUMI_MIN];
        _pMweight[head].u07.kumi_max = data[TL2_KUMI_MAX];
        _pMweight[head].u07.target   = data[TL2_TARGET];
        _pMweight[head].u07.jougen   = data[TL2_JOUGEN];
    }else if(i==7){
        _pMweight[head].u08.rank_en  = data[TL2_RANK_EN];
        _pMweight[head].u08.rank_no  = data[TL2_RANK_NO];
        _pMweight[head].u08.rank_min = data[TL2_RANK_MIN];
        _pMweight[head].u08.rank_max = data[TL2_RANK_MAX];
        _pMweight[head].u08.kumi_min = data[TL2_KUMI_MIN];
        _pMweight[head].u08.kumi_max = data[TL2_KUMI_MAX];
        _pMweight[head].u08.target   = data[TL2_TARGET];
        _pMweight[head].u08.jougen   = data[TL2_JOUGEN];
    }else if(i==8){
        _pMweight[head].u09.rank_en  = data[TL2_RANK_EN];
        _pMweight[head].u09.rank_no  = data[TL2_RANK_NO];
        _pMweight[head].u09.rank_min = data[TL2_RANK_MIN];
        _pMweight[head].u09.rank_max = data[TL2_RANK_MAX];
        _pMweight[head].u09.kumi_min = data[TL2_KUMI_MIN];
        _pMweight[head].u09.kumi_max = data[TL2_KUMI_MAX];
        _pMweight[head].u09.target   = data[TL2_TARGET];
        _pMweight[head].u09.jougen   = data[TL2_JOUGEN];
    }else if(i==9){
        _pMweight[head].u10.rank_en  = data[TL2_RANK_EN];
        _pMweight[head].u10.rank_no  = data[TL2_RANK_NO];
        _pMweight[head].u10.rank_min = data[TL2_RANK_MIN];
        _pMweight[head].u10.rank_max = data[TL2_RANK_MAX];
        _pMweight[head].u10.kumi_min = data[TL2_KUMI_MIN];
        _pMweight[head].u10.kumi_max = data[TL2_KUMI_MAX];
        _pMweight[head].u10.target   = data[TL2_TARGET];
        _pMweight[head].u10.jougen   = data[TL2_JOUGEN];
        //_pMweight[head].priority   = data_p;
        _pMweight[head].Rcolor        = data_f;
        _pMweight[head].Rsort        = data_r;
        _pMweight[head].Rselect      = data_s;
    }
}

/*****************************************************************************
    回数制限情報の読み取り
*****************************************************************************/
void FileData::read_limit1(){
    FILE *fp;

    if ((fp = fopen(NAME_LIMIT1,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "limit1");
        //exit(EXIT_FAILURE);
        file_lost_sig += 512;
        KUMI_limit=9999;
        save_limit1(KUMI_limit);
        //fclose(fp);
        fp = NULL;
    }else{
        fscanf(fp, "%d"
            ,&KUMI_limit
        );
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}


/*****************************************************************************
    回数制限情報のﾎｿﾞﾝ
*****************************************************************************/
void FileData::save_limit1(int limit1){
        FILE *fp;
    
    if(( fp=fopen(NAME_LIMIT1,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "name");
        exit(EXIT_FAILURE);
    }

    //ローカルファイルに書き込む
    fprintf(fp, "%d"
        ,limit1
    );
    
    fclose(fp);
    fp = NULL;
}
/*****************************************************************************
    回数制限情報の読み取り
*****************************************************************************/
void FileData::read_limit2(){
    FILE *fp;

    if ((fp = fopen(NAME_LIMIT2,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "limit2");
        //exit(EXIT_FAILURE);
        file_lost_sig += 1024;
        KUMI_now=0;
        save_limit2(KUMI_now);
        //fclose(fp);
        fp = NULL;
    }else{
        fscanf(fp, "%d"
            ,&KUMI_now
        );
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}

/*****************************************************************************
    回数制限情報のﾎｿﾞﾝ
*****************************************************************************/
void FileData::save_limit2(int limit2){
        FILE *fp;
    
    if(( fp=fopen(NAME_LIMIT2,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "name");
        exit(EXIT_FAILURE);
    }

    //ローカルファイルに書き込む
    fprintf(fp, "%d"
        ,limit2
    );
    
    fclose(fp);
    fp = NULL;
}









//変更されたロードセルデータをset
void FileData::set_load(const loadcell_t *l, int head){
    _pCel[head] = *l;
}

//パラメーターを返す
parameter_t FileData::get_param(){
    return _param;
}

product_t FileData::get_product(){
    return _product;
}
//オプションデータを返す
option_t FileData::get_option(){
    return _option;
}
//ロードセルのデータを返す
loadcell_t FileData::get_load(int head){
    return _pCel[head];
}

//ロードセルのデータを返す
multiWeight_t FileData::get_lweight(int head){
    return _pMweight[head];
}

//スパンデータを返す
span_t* FileData::get_span(int head){
    return &_pSpan[head];
}

//ロードセルのデータをポインタ(構造体配列)で渡す
loadcell_t* FileData::get_load(){
        return &_pCel[0];
}

//基本設定のデータを返す
basicSetting_t FileData::get_basic(){
    return _basicData;
}

//スパン値格納の構造体配列ポインタを返す
span_t* FileData::get_span(){
    return &_pSpan[0];
}

//PARAMの設定数を取得
int FileData::get_count_param(){
    return NUM_SDATA;
}

//PARAMの浮動小数の設定が始まる位置を取得
int FileData::get_start_fparam(){
    return NUM_SDATA_FLORT;
}