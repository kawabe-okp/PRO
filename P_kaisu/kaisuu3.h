void drive_kaisuu3();
void show_results_kaisu(int rank0or1,  const combiData_t *com, int RankNo, int rankkumi, int nowkosu, int befourekumi);
void driveSlowly_kaisu_1(combiData_t *com ,combiData_t *com2);
void driveSlowly_kaisu_2(combiData_t *com ,combiData_t *com2);
void lcd_print_kaisuu3();
void get_combi_location_kaisuu3(combiData_t com[], int *secondCnt);
void matrix_count(int oyahead, int kumi_Data ,int fast_no ,int befourekumi ,int green_yn);


int no_combi_counter[3] = {0,0,0};  //0:今ﾗﾝｸ0/1どちらか　1:ランク0の不成立回数　2:ﾗﾝｸ1の不成立回数
int  wait_for_reset(int head);
void se_alarm_rap();
void light_LED_rap();
void lcd_print_rank(int no);

bool light_LED_rap_TF = true;
int wait_for_reset_head =0;
bool se_alarm_go = false;

int OYA_DATA[2][3] ={{0,0,0},{0,0,0}};  //親ヘッドの情報110(成立中の個数、合算個数、重量で計算した個数)
float default_range[2][3] = {{0.0,0.0,0.0},{0.0,0.0,0.0}};    //デフォルトのデータを入れておく場所 [(u02)デフォルトの最小-最大-平均値][(u03)デフォルトの最小-最大-平均値]
float movable_range[2][3] = {{0.0,0.0,0.0},{0.0,0.0,0.0}};    //変化するデータを入れておく場所     [(u02)変化していく最小-最大-平均値][(u03)変化していく最小-最大-平均値]
int OYAHEAD[2] = {0,0};    //親ヘッドの指定
bool combi_EN[2] = {true,true}; //組合せを行っていいかのフラグ。整理中に成立させるわけにはいかない

int S1_WEIGHT_rank_en  = 0;
int S1_WEIGHT_rank_no  = 0;
int S1_WEIGHT_kumi = 0;
int S1_WEIGHT_target   = 0;
int S1_WEIGHT_jougen   = 0;

int S0_WEIGHT_kumi = 0;
int S0_WEIGHT_target   = 0;
int S0_WEIGHT_jougen   = 0;
bool S1_NO_COMBI = false; //組合せが存在しないか否か

int L1_WEIGHT_rank_en  = 0;
int L1_WEIGHT_rank_no  = 0;
int L1_WEIGHT_kumi = 0;
int L1_WEIGHT_target   = 0;
int L1_WEIGHT_jougen   = 0;

int L0_WEIGHT_kumi = 0;
int L0_WEIGHT_target   = 0;
int L0_WEIGHT_jougen   = 0;
bool L1_NO_COMBI = false; //組合せが存在しないか否か



bool data_one_print =false;
bool data_two_print =false;
bool kumiawase_0 = false; //大小どちらの組合せを行うかTrue:小


int combi_OK = 0;













int wait_for_reset(int head){
    const char RESET[]      = {0xD8,0xBE,0xAF,0xC4,' ' ,0xBC,0xCF,0xBD,0xB6,0x3F,0x00};      //ﾘｾｯﾄ ｼﾏｽｶ?
    const char stYESNO[][4] = {"YES","NO"};          //準最適値
    int now = 0;
    wait_for_reset_head = head;
    lcd.cls();
    lcd.printf(RESET);
    lcd.locate(0,1);
    lcd.printf(stYESNO[now]);
    btn.new_btn();              //スイッチ(New)取得
    btn.old_btn();  //スイッチ更新
    se_alarm_rap();
    
    Ticker alarm_rap;                         //川  ラップ取得用
    Ticker LED_rap;                         //川  ラップ取得用
    
    alarm_rap.attach( &se_alarm_rap, 2.5);
    LED_rap.attach( &light_LED_rap, 0.3);
    while(btn.get_btn() != BTN_ENT) {
        if(se_alarm_go == true){
            se.recheck();
            se_alarm_go = false;
        }
        if(btn.get_one_push()) {
            lcd.blank(1);
            lcd.printf(stYESNO[now]);
        }
        setHeadVolt(head, param.KURI);
        if(nowweightVale[head] > param.CUT){
            btn.old_btn();  //スイッチ更新
            alarm_rap.detach();
            LED_rap.detach();
            led.REGIS(0);
            return 1;         //esc押下なら処理終了
        }
        
        
        
        
        btn.new_btn();              //スイッチ(New)取得
        btn.next_or_back(&now, 0, 1);
        if(btn.push_sound()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        if(btn.esc()) {
            btn.old_btn();  //スイッチ更新
            alarm_rap.detach();
            LED_rap.detach();
            led.REGIS(0);
            return 1;         //esc押下なら処理終了
        }
        btn.old_btn();  //スイッチ更新
    }
    alarm_rap.detach();
    LED_rap.detach();
    led.REGIS(0);
    return now;
}

void se_alarm_rap(){
    se_alarm_go = true;
}

void light_LED_rap(){
    if(light_LED_rap_TF == true){
        led.REGIS(oneBit[wait_for_reset_head]);
        light_LED_rap_TF = false;
    }else{
        led.REGIS(0);
        light_LED_rap_TF = true;
    }
}


void drive_kaisuu3(){
    //char multiweight[SIZE] = {0xD7,0xDD,0xB8,0xCD,0xDE,0xC2,' ' ,0xB8,0xD0,0xB1,0xDC,0xBE,0x00};
    //char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
    //                   ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼﾀ
    int *stay_cnt = new int[product.head];      //滞留警告判断のためのカウント変数
    //float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    cntData_t cnt= {0,0,0};                     //カウント機能の値保持用
    int secondCnt1 = 0;                        //一定数に達したら準最適値処理開始
    int secondCnt2 = 0;                        //一定数に達したら準最適値処理開始
    combiData_t orderCombi1[BK_CNT] = {0,0.0,0};   //組み合わせ候補のデータ(配列)
    combiData_t orderCombi2[BK_CNT] = {0,0.0,0};   //組み合わせ候補のデータ(配列)
    combiData_t resultCombi1;                    //組み合わせ結果(最小値取得用)
    combiData_t resultCombi2;                    //組み合わせ結果(最小値取得用)
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    cnt.nowMode = load.spMode;       //処理モード取得
    int btnset =0;//計量中の下ボタン押しで出てくる値保存
    int SerialNumber = 0; //USBを連続で使用する際の連番値
    //float default_range[2][3] = {{0.0,0.0,0.0},{0.0,0.0,0.0}};    //デフォルトのデータを入れておく場所 [(u02)デフォルトの最小 ,最大 ,平均値][(u03)デフォルトの最小 ,最大 ,平均値]
    //float movable_range[2][3] = {{0.0,0.0,0.0},{0.0,0.0,0.0}};    //変化するデータを入れておく場所     [(u02)変化していく最小 ,最大 ,平均値][(u03)変化していく最小 ,最大 ,平均値]
    float between = 0.0;
    float OYA_DATA_gram[2][2] = {{0.0,0.0},{0.0,0.0}}; //現在の合算重量（計算値）[1個目計算重量・残量][2個目計算重量・残量]  ※12・6ヘッドの重量を見ても振動中華の判定がしずらいので。
    float now_data_gram[2] = {0.0,0.0};  //ターゲット重量[1個目・2個目]直前と今回でち
    float target_jougen[2][2] = {{0.0,0.0},{0.0,0.0}}; //[1個目target/jougen][2個目target/jougen]
    float baratsuki=param.BARATSUKI;//一時保存フォルダ
    float before_head[12] ={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    kumiawase_0 = false; //組み合わせの1個目を行うか否か
    no_combi_v_flg = false;
    
    S0_WEIGHT_kumi     = 0;
    S0_WEIGHT_target   = 0;
    S0_WEIGHT_jougen   = 0;
    L0_WEIGHT_kumi     = 0;
    L0_WEIGHT_target   = 0;
    L0_WEIGHT_jougen   = 0;
    
    S1_WEIGHT_rank_en = mweight.u02.rank_en;
    S1_WEIGHT_rank_no = mweight.u02.rank_no;
    S1_WEIGHT_kumi    = mweight.u02.kumi_min;
    S1_WEIGHT_target  = mweight.u02.target;
    S1_WEIGHT_jougen  = mweight.u02.jougen;
    
    L1_WEIGHT_rank_en = mweight.u03.rank_en;
    L1_WEIGHT_rank_no = mweight.u03.rank_no;
    L1_WEIGHT_kumi    = mweight.u03.kumi_min;
    L1_WEIGHT_target  = mweight.u03.target;
    L1_WEIGHT_jougen  = mweight.u03.jougen;
    
    if(load.kumi_flag == KUMI_KAISU2){
        S0_WEIGHT_kumi    = mweight.u03.kumi_min;
        S0_WEIGHT_target  = mweight.u03.target;
        S0_WEIGHT_jougen  = mweight.u03.jougen;
        
        L1_WEIGHT_rank_en = mweight.u04.rank_en;
        L1_WEIGHT_rank_no = mweight.u04.rank_no;
        L1_WEIGHT_kumi    = mweight.u04.kumi_min;
        L1_WEIGHT_target  = mweight.u04.target;
        L1_WEIGHT_jougen  = mweight.u04.jougen;
        L0_WEIGHT_kumi    = mweight.u05.kumi_min;
        L0_WEIGHT_target  = mweight.u05.target;
        L0_WEIGHT_jougen  = mweight.u05.jougen;
    }
    
    OYA_DATA[0][0] =0;  //親ヘッドの情報110(個数、平均重量)
    OYA_DATA[0][1] =0;  //親ヘッドの情報110(個数、平均重量)
    OYA_DATA[1][0] =0;  //親ヘッドの情報110(個数、平均重量)
    OYA_DATA[1][1] =0;  //親ヘッドの情報110(個数、平均重量)
    
    default_range[0][0] = (S1_WEIGHT_target - load.taimenTarget) / float(S1_WEIGHT_kumi);
    default_range[0][1] = (S1_WEIGHT_jougen - load.taimenTarget) / float(S1_WEIGHT_kumi);
    default_range[0][2] = (default_range[0][1] + default_range[0][0])/2;
    
    default_range[1][0] = (L1_WEIGHT_target - load.taimenTarget) / float(L1_WEIGHT_kumi);
    default_range[1][1] = (L1_WEIGHT_jougen - load.taimenTarget) / float(L1_WEIGHT_kumi);
    default_range[1][2] = (default_range[1][1] + default_range[1][0])/2;
    
    pc.printf("%4.1f: %4.1f\r\n",    default_range[0][0],default_range[0][1]);
    between = (default_range[1][0] - default_range[0][1])/2;
    pc.printf("%4.1f\r\n",    between);
    
    movable_range[0][0] = default_range[0][0] - between;
    movable_range[0][1] = default_range[0][1] + between;
    movable_range[1][0] = default_range[1][0] - between;
    movable_range[1][1] = default_range[1][1] + between;
    pc.printf("range00 %4.1f: range01 %4.1f\r\n",    movable_range[0][0],movable_range[0][1]);
    pc.printf("range10 %4.1f: range11 %4.1f\r\n",    movable_range[1][0],movable_range[1][1]);
    
    if(product.head < 8 && L1_WEIGHT_rank_en == 0){
        OYAHEAD[0] = 6-1;
        OYAHEAD[1] = product.head+2;    //全く関係ない場所を指定
        gOyaHead = 0x20;
    }
    if(product.head < 8 && L1_WEIGHT_rank_en == 1){
        OYAHEAD[0] = 5-1;
        OYAHEAD[1] = 6-1;
        gOyaHead = 0x30;
    }
    if(product.head > 8 && L1_WEIGHT_rank_en == 0){
        OYAHEAD[0] = 6-1;
        OYAHEAD[1] = product.head+2;
        gOyaHead = 0x20;
    }
    if(product.head > 8 && L1_WEIGHT_rank_en == 1){
        OYAHEAD[0] = 6-1;
        OYAHEAD[1] = 12-1;
        gOyaHead = 0x820;
    }
        
    combi_EN[0] = true;
    combi_EN[1] = true;
        
    
    //v.gram_voice_hani(10,int(movable_range[0][0]*10.0),int(movable_range[0][1]*10.0));
    //1行目LCD表示
    lcd_print_kaisuu3();
    
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
    
    for(int l; l<product.head; l++) {
        nowweightVale3[l]=0.0;
    }
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
    btn.end_force();

    cel.set_BARATSUKI(param.BARATSUKI_GRAM);
  ///////計量スタート//////////////////////////////////  
    while(1){
      //////////////////////////////////////////////////////////////////////////////
      //////ボタン操作///////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////////
        btn.new_keep_btn();         //スイッチ(New)取得
        
        //esc_swを押すと、Menu画面に戻る
        if(btn.esc_press() == true) {
            meas_rap.detach();
            se.Touch_01();
            //if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT || load.kumi_flag == KUMI_COMBI )&& total_kumi_cnt>0) {
            if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT)&& total_kumi_cnt>0) {
                //seconds = time(NULL);
                //tm *dated4 = localtime(&seconds);
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
                /*if(product.use_usb==1 && option.usb_EN==true) {
                    lcd.printf(saving[0]);
                }
                if(product.use_usb==1 && option.usb_EN==true) {
                    file.save_SAGYOU(ji,hun,file.get_preset()+1,load.target,total_kumi_cnt,average_kumi_value);
                }
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
                }*/
            }
            delete[] stay_cnt;
            total_kumi_cnt = 0;
            average_kumi_value = 0;
            param.BARATSUKI=baratsuki;
            file.read_param();          //設定値
            cel.set_BARATSUKI(param.BARATSUKI);
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            return;
        }

        //上ボタンが離されたら、操作ロック解除
        if( btn.down_press() == false) {
            btn.operation_unlock();
            data_one_print = false;
            data_two_print = false;
        }
        
        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            auto_zero_point();
            //1行目LCD表示
            lcd_print_kaisuu3();
            //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        } else if(btn.up_press() == true) {
            se.chime_01();
            btn.end_force();
            /*
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
            if(load.kumi_flag == KUMI_OYAKO || load.kumi_flag == KUMI_OYAKO_HALF || load.kumi_flag == KUMI_KAISU1 || load.kumi_flag == KUMI_KAISU2) {
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
                    set_sercret_parameter_one(P_buffer_KAISU,1); //保存指令付
                }else if(btnset==7){
                    set_option_setting(OP_NO_COMBI_V);
                }else if(btnset==8){
                    set_option_setting(OP_CHAR_ROTATE);
                } 
            }*/
            select_keiryo_option(0, load.kumi_flag, 0);
            //1行目LCD表示
            lcd_print_kaisuu3();
        }

        btn.old_btn();       //スイッチ更新

      //////////////////////////////////////////////////////////////////////////////
      //////ボタン操作終わり///////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////////
        
        for(int i = 0; i < product.head; i++) {
            setHeadVolt(i, param.KURI);
            if((nowweightVale[i] != 0 && ((nowweightVale[i] -nowweightVale3[i]) <= -0.3)) || ((nowweightVale[i] -nowweightVale3[i]) >= 0.3)) {
                nowweightVale3[i] = nowweightVale[i];
            }
        }
        if(OYA_DATA[0][1] >0){
            combi_EN[0] = false;
        }else{
            combi_EN[0] = true;
        }
        if(OYA_DATA[1][1] >0){
            combi_EN[1] = false;
        }else{
            combi_EN[1] = true;
        }
        for(int i = 0; i < product.head; i++) {
            if(nowweightVale3[i] <= param.CUT){
                before_head[i] = 0.0;
            }else if(cel.anteiWeight(i) > 0){
                before_head[i] = cel.anteiWeight(i);
            }else if(param.CUT <= nowweightVale3[i] && before_head[i] > 0){
                // = before_head[i]
            }
            if(i == OYAHEAD[0] && S1_WEIGHT_rank_en == 1){
            //if((product.head > 8 && i == product.head/2-1) || 
            //   (product.head < 8 && i == product.head-1  )   ){//12ヘッドのときの6ヘッド,6ヘッドのときの6ヘッド
                if(iro[i] != C_G_EXCLA){
                    iro[i] = S1_WEIGHT_rank_no;
/*dd*/              iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                }
                //OYA_DATA[0]_gram = /*nowweightVale3[i]*/cel.anteiWeight(i);
                if(nowweightVale3[i] <= load.taimenTarget-1){
                    iro[i] = S1_WEIGHT_rank_no;
/*dd*/              iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                    //combi_EN[0]=true;
                    if(1 < OYA_DATA[0][1] && OYA_DATA[0][1] < S1_WEIGHT_kumi){
                        if(wait_for_reset(i)==0){
                            combi_EN[0]=true;
                            OYA_DATA[0][1]=0;
                            OYA_DATA_gram[0][0]=0.0;
                            lcd_print_kaisuu3();
                            //pc.printf("erase OYA_DATA[0][1]\r\n");
                        }else{
                            lcd_print_kaisuu3();
                        }
                    }else if(OYA_DATA[0][1] == S1_WEIGHT_kumi){
                        combi_EN[0]=true;
                        OYA_DATA[0][1]=0;
                        OYA_DATA_gram[0][0]=0.0;
                    }
                }else if(param.CUT < nowweightVale3[i] && cel.anteiWeight(i) <= param.CUT && cel.anteiWeight(i) < S1_WEIGHT_target){ //重量は乗ってるけど、安定してない
                    combi_EN[0]=false;   
                //}else if(param.CUT <= /*nowweightVale3[i]*/cel.anteiWeight(i) && /*nowweightVale3[i]*/cel.anteiWeight(i) < movable_range[0][0]){
                }else if(param.CUT <= /*nowweightVale3[i]*/cel.anteiWeight(i) && /*nowweightVale3[i]*/cel.anteiWeight(i) < S1_WEIGHT_target){
                    //combi_EN[0]=true;
                    if(param.CUT <= cel.anteiWeight(i)){
                        if( OYA_DATA_gram[0][0] == cel.anteiWeight(i)){
                            combi_EN[0]=true;
                        }
                        if((cel.anteiWeight(i)-(load.taimenTarget+6)) < (OYA_DATA_gram[0][0]) && (OYA_DATA_gram[0][0]) < (cel.anteiWeight(i)+(load.taimenTarget+6))){
                            iro[i] = S1_WEIGHT_rank_no;
/*dd*/                      iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                            OYA_DATA_gram[0][0] = cel.anteiWeight(i);
                            combi_EN[0]=true;
                        } else {
                            iro[i] = C_G_EXCLA;
                            se.mistake();se.mistake();
/*dd*/                      iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                            pc.printf("%5.1f  :  %5.1f  :  %5.1f \r\n",(OYA_DATA_gram[0][0]/OYA_DATA[0][1]), movable_range[0][0], (movable_range[0][1]));
                            combi_EN[0]=false;
                        }
                        
                        
                        /*
                        OYA_DATA_gram[0][0] = cel.anteiWeight(i);       if((OYA_DATA_gram[0][0]/OYA_DATA[0][1]) < (movable_range[0][0]-1.0) || (movable_range[0][1]+1.0) < (OYA_DATA_gram[0][0]/OYA_DATA[0][1])){
                            
                            pc.printf("%5.1f  :  %5.1f  :  %5.1f  :  %5.1f \r\n",OYA_DATA_gram[0][0] ,(OYA_DATA_gram[0][0]/OYA_DATA[0][1]), movable_range[0][0], movable_range[0][1]);
                            se.mistake();se.mistake();     iro[i] = C_G_EXCLA;
                        }*/
                    }
                    //pc.printf("%5dH: %5.1fg   %5dko\r\n",i,float(/*nowweightVale3[i]*/cel.anteiWeight(i)/OYA_DATA[0][1]),OYA_DATA[0][1]);
                
                }else if(S1_WEIGHT_target <= /*nowweightVale3[i]*/cel.anteiWeight(i) && /*nowweightVale3[i]*/cel.anteiWeight(i) < S1_WEIGHT_jougen+50 && S1_WEIGHT_kumi - OYA_DATA[0][1] ==0){
                    if((cel.anteiWeight(i)-5.0) < (OYA_DATA_gram[0][0]) && (OYA_DATA_gram[0][0]) < (cel.anteiWeight(i)+5.0)){
                        iro[i] = C_MARU;
/*dd*/                  //iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                        se.chime_01();
                    }
                    //wait(3); //if(/*nowweightVale3[i]*/cel.anteiWeight(i) >= param.CUT){  //OYA_DATA[0][2] = /*nowweightVale3[i]*/cel.anteiWeight(i)/OYA_DATA[0][1]; 
                }
            }else if(i == OYAHEAD[1] && L1_WEIGHT_rank_en == 1){
            //}else if(product.head > 8 && i == product.head-1){//12ヘッドのときの12ヘッド
            //}else if(product.head > 8 && i == 4){//12ヘッドのときの12ヘッド無理やり5ヘッドへ
                //OYA_DATA[1]_gram = /*nowweightVale3[i]*/cel.anteiWeight(i);
                if(iro[i] != C_G_EXCLA){
                    iro[i] = L1_WEIGHT_rank_no;
/*dd*/              iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                    //iro[i] = CG_99;
                }
                if(nowweightVale3[i] <= load.taimenTarget-1){
                    iro[i] = L1_WEIGHT_rank_no;
/*dd*/              iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                    //combi_EN[1]=true;
                    if(1 < OYA_DATA[1][1] && OYA_DATA[1][1] < L1_WEIGHT_kumi){
                        if(wait_for_reset(i)==0){
                            combi_EN[1]=true;
                            OYA_DATA[1][1]=0;
                            OYA_DATA_gram[1][0]=0.0;
                            //pc.printf("erase OYA_DATA[0][1]\r\n");
                            lcd_print_kaisuu3();
                        }else{
                            lcd_print_kaisuu3();
                        }
                    }else if(OYA_DATA[1][1] == L1_WEIGHT_kumi){
                        combi_EN[1]=true;
                        OYA_DATA[1][1]=0;
                        OYA_DATA_gram[1][0]=0.0;
                    }
                }else if(param.CUT < nowweightVale3[i] && cel.anteiWeight(i) <= param.CUT && cel.anteiWeight(i) < S1_WEIGHT_target){ //重量は乗ってるけど、安定してない
                    //iro[i] = CG_01;
                    combi_EN[1]=false;   
                //}else if(param.CUT <= /*nowweightVale3[i]*/cel.anteiWeight(i) && /*nowweightVale3[i]*/cel.anteiWeight(i) < movable_range[1][0]){
                }else if(param.CUT <= /*nowweightVale3[i]*/cel.anteiWeight(i) && /*nowweightVale3[i]*/cel.anteiWeight(i) < S1_WEIGHT_target){
                    //combi_EN[1]=true;
                    if(param.CUT <= cel.anteiWeight(i)){
                        if( OYA_DATA_gram[1][0] == cel.anteiWeight(i)){//iro[i] = CG_02;
                            combi_EN[1]=true;
                            
                        }
                        //if((OYA_DATA_gram[1][0]) < (cel.anteiWeight(i)-3.0) || (cel.anteiWeight(i)+3.0) < (OYA_DATA_gram[1][0])){
                        if((cel.anteiWeight(i)-(load.taimenTarget+6)) < (OYA_DATA_gram[1][0]) && (OYA_DATA_gram[1][0]) < (cel.anteiWeight(i)+(load.taimenTarget+6))){
                            iro[i] = L1_WEIGHT_rank_no;
/*dd*/                      iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                            OYA_DATA_gram[1][0] = cel.anteiWeight(i);
                            
                            //iro[i] = CG_02;
                            combi_EN[1]=true;
                        } else {
                            iro[i] = C_G_EXCLA;
                            se.mistake();se.mistake();
/*dd*/                      iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                            pc.printf("%5.1f  :  %5.1f  :  %5.1f \r\n",(OYA_DATA_gram[1][0]/OYA_DATA[1][1]), movable_range[1][0], (movable_range[1][1]));
                            combi_EN[1]=false;
                        }
                        
                        
                        
                        /*
                        OYA_DATA_gram[1][0] = cel.anteiWeight(i);
                        //if((OYA_DATA_gram[1][0]/OYA_DATA[1][1]) < (movable_range[1][0]-1.0) || (movable_range[1][1]+1.0) < (OYA_DATA_gram[1][0]/OYA_DATA[1][1])){
                        if((OYA_DATA_gram[1][0]) < (cel.anteiWeight(i)-3.0) || (cel.anteiWeight(i)+3.0) < (OYA_DATA_gram[1][0])){
                            se.mistake();se.mistake();
                            pc.printf("%5.1f  :  %5.1f  :  %5.1f \r\n",(OYA_DATA_gram[1][0]/OYA_DATA[1][1]), movable_range[1][0], (movable_range[1][1]));
                            iro[i] = C_G_EXCLA;
                        }*/
                    }
                    //pc.printf("%5dH: %5.1fg   %5dko\r\n",i,float(/*nowweightVale3[i]*/cel.anteiWeight(i)/OYA_DATA[0][1]),OYA_DATA[0][1]);
                    
                }else if(L1_WEIGHT_target <= /*nowweightVale3[i]*/cel.anteiWeight(i) && /*nowweightVale3[i]*/cel.anteiWeight(i) < L1_WEIGHT_jougen + 50 && L1_WEIGHT_kumi - OYA_DATA[1][1] ==0){
                    //iro[i] = CG_03;
                    if((cel.anteiWeight(i)-5.0) < (OYA_DATA_gram[1][0]) && (OYA_DATA_gram[1][0]) < (cel.anteiWeight(i)+5.0)){
                        iro[i] = C_MARU;
/*dd*/                  //iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                        se.chime_01();
                        //wait(3);
                    }
                }
                
                
            }else if(/*nowweightVale3[i] cel.anteiWeight(i)*/before_head[i] <= param.CUT ){
                iro[i] = C_NUSE;
/*dd*/          iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
            /**/} else if(/*nowweightVale3[i] cel.anteiWeight(i)*/before_head[i] >= param.CUT           && /*nowweightVale3[i] cel.anteiWeight(i)*/before_head[i] < movable_range[0][0]) {
                //iro[i] = C_NUSE_1D;
                iro[i] = S1_WEIGHT_rank_no-1;
/*dd*/          iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
            } else if(/*nowweightVale3[i] cel.anteiWeight(i)*/before_head[i] >= movable_range[0][0] && /*nowweightVale3[i] cel.anteiWeight(i)*/before_head[i] < movable_range[0][1]) {
                iro[i] = S1_WEIGHT_rank_no;
/*dd*/          iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                //iro[i] = C_NUSE_2D;
            } else if(L1_WEIGHT_rank_en == 1){
                if(/*nowweightVale3[i] cel.anteiWeight(i)*/before_head[i] >= movable_range[1][0] && /*nowweightVale3[i] cel.anteiWeight(i)*/before_head[i] < movable_range[1][1]) {
                    iro[i] = L1_WEIGHT_rank_no;
/*dd*/              iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                    //iro[i] = C_NUSE_2D;
                } else if(/*nowweightVale3[i] cel.anteiWeight(i)*/before_head[i] >= movable_range[1][1]) {
                    iro[i] = L1_WEIGHT_rank_no+1;
/*dd*/              iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                } else {
                    iro[i] = L1_WEIGHT_rank_no+1;
/*dd*/              iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                }
            } else if(L1_WEIGHT_rank_en == 0){
                if(/*nowweightVale3[i] cel.anteiWeight(i)*/before_head[i] >= movable_range[1][0]){
                    iro[i] = S1_WEIGHT_rank_no+1;
/*dd*/              iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                } else {
                    iro[i] = S1_WEIGHT_rank_no+1;
/*dd*/              iro[i] = CR_01 + int(cel.anteiWeight(i)) %100 -1;
                }
            } else {
                //iro[i] = C_BATU;
                iro[i] = CR_01 + int(/*nowweightVale3[i] cel.anteiWeight(i)*/before_head[i]);
            }
        }//for
        //デバッグ用ここから
        if( product.productType >= STANDARD_D || product.productType == JUNIOR_D || product.productType == BIG_D || product.productType == MATRIX_D) {
            for(int i = 0; i < product.head; i++) {
                //temp = cel.temperature();   //温度を取得する
                //for(int i = 0; i < product.head; i++){
                //cel.set_head(i);
                cel.set_head(i);
                pc.printf("%02d: %6.3f %6.3f %6.3f   %0.8f\r\n", i+1,cel.getWeight(i),cel.anteiWeight(i), cel.get_HWeight(i),cel.getAin());
                //}
                if(i == product.head - 1) {
                    //printf("%d %f C\r\n",14, temp);
                    pc.printf("========================\r\n");
                    pc.printf("========================\r\n");
                    pc.printf("========================\r\n");
                }
            }
        }//デバッグ用ここまで
        
        if(L1_WEIGHT_rank_en == 1){//2個目のランクを使う場合
            if(kumiawase_0==false){
                kumiawase_0=true;
            }else{
                kumiawase_0=false;
            }
        }else{                      //2個目のランクを使わない場合
            kumiawase_0=true;
        }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////組合せ方法調整
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(kumiawase_0==true){
            
            OYA_DATA_gram[0][1] = float(S1_WEIGHT_target) - OYA_DATA_gram[0][0];
            movable_range[0][2] = OYA_DATA_gram[0][1] / (S1_WEIGHT_kumi - OYA_DATA[0][1]);
            if(S1_WEIGHT_kumi - OYA_DATA[0][1] > 2){ 
                load.kumi_min = 2;
                load.kumi_max = 2;
            }else{
                load.kumi_min = S1_WEIGHT_kumi - OYA_DATA[0][1]-1;
                load.kumi_max = load.kumi_min;
            }
            if(load.kumi_flag == KUMI_KAISU2 && (S0_WEIGHT_kumi > OYA_DATA[0][1])){   //回数3で、まだ1段目の場合
                //}else if((S0_WEIGHT_kumi - OYA_DATA[0][1]) <= 4){
                //    load.kumi_min = S0_WEIGHT_kumi - OYA_DATA[0][1] -1;
                //    load.kumi_max = load.kumi_min;
                //    OYA_DATA_gram[0][1] = float(S1_WEIGHT_target) - OYA_DATA_gram[0][0];
                //    movable_range[0][2] = OYA_DATA_gram[0][1] / (S0_WEIGHT_kumi - OYA_DATA[0][1]);
                //    load.target  = (float(S0_WEIGHT_target) - OYA_DATA_gram[0][0]) * 10;
                //    load.jougen  = (float(S0_WEIGHT_jougen) - OYA_DATA_gram[0][0]) * 10  - load.target;
                OYA_DATA_gram[0][1] = float(S0_WEIGHT_target) - OYA_DATA_gram[0][0];
                movable_range[0][2] = OYA_DATA_gram[0][1] / (S0_WEIGHT_kumi - OYA_DATA[0][1]);
                if((S0_WEIGHT_kumi - OYA_DATA[0][1]) == 4){ //残り数量が4だった場合
                    printf("001\r\n");
                    load.kumi_min = 1;
                    load.kumi_max = 1;
                    
                    
                    if(OYA_DATA[0][1] == 0){          //組み合わせが初回の場合
                        printf("002\r\n");
                        load.target =  ((float(S0_WEIGHT_target - load.taimenTarget)    /  float(S0_WEIGHT_kumi))*float(load.kumi_min+1))  *10;
                        load.jougen =  (load.target*1.2) - load.target;
                    }else{
                        printf("003\r\n");
                        load.target = (((OYA_DATA_gram[0][1]) / (float(S0_WEIGHT_kumi) - OYA_DATA[0][1]))* float(load.kumi_min+1)) * 10;
                        load.jougen =  (load.target*1.2) - load.target;
                    }
                }else if((S0_WEIGHT_kumi - OYA_DATA[0][1]) <=2){    // 残り数量が3以下だった場合
                    printf("004\r\n");
                    load.kumi_min = (S0_WEIGHT_kumi - OYA_DATA[0][1]-1);
                    load.kumi_max = load.kumi_min;
                    load.target = (((OYA_DATA_gram[0][1]) / (float(S0_WEIGHT_kumi) - OYA_DATA[0][1]))* float(load.kumi_min+1)) * 10;
                    load.jougen =  (load.target*1.2) - load.target;
                    
                }else if(OYA_DATA[0][1] == 0){          //組み合わせが初回の場合
                    printf("005\r\n");
                    load.kumi_min = 2;//[2]で3皿の意味
                    load.kumi_max = 2;
                    load.target = ((float(S0_WEIGHT_target - load.taimenTarget) / float(S0_WEIGHT_kumi))*float(load.kumi_min+1))  *10;
                    load.jougen =  (load.target*1.2) - load.target;
            
                }else if((S0_WEIGHT_kumi - OYA_DATA[0][1]) > 2){ 
                    printf("006\r\n");
                    load.kumi_min = 2;//[2]で3皿の意味   OYA_DATA_gram[0][1]
                    load.kumi_max = 2;
                    load.target = (((OYA_DATA_gram[0][1]) / (float(S0_WEIGHT_kumi) - OYA_DATA[0][1]))* float(load.kumi_min+1)) * 10;
                    load.jougen =  (load.target*1.2) - load.target;
                }else {
                    printf("007\r\n");
                }
                
            }else if(load.kumi_flag != KUMI_KAISU2 && OYA_DATA[0][1]<3){          //組み合わせが初回の場合
                printf("008\r\n");
                movable_range[0][2] = (float(S1_WEIGHT_target - load.taimenTarget) / float(S1_WEIGHT_kumi));
                //movable_range[0][2] = OYA_DATA_gram[0][1] / float(load.kumi_min+1);
                //load.target = movable_range[0][0]*(load.kumi_min+1)*10 - load.taimenTarget*10;
                //load.jougen = movable_range[0][1]*(load.kumi_min+1)*10 - load.taimenTarget*10 - load.target;
                load.target = movable_range[0][2]*(load.kumi_min+1)*10 - load.taimenTarget*10;
                load.jougen = (load.target*1.2) - load.target;
                
            }else if((S1_WEIGHT_kumi - OYA_DATA[0][1]) < 3){  //最後の1回の計量は、最高重量の範囲をかなり広げる
                printf("009\r\n");
                load.kumi_min = S1_WEIGHT_kumi - OYA_DATA[0][1]-1;
                load.kumi_max = load.kumi_min;
                OYA_DATA_gram[0][1] = float(S1_WEIGHT_target) - OYA_DATA_gram[0][0];
                movable_range[0][2] = OYA_DATA_gram[0][1] / float(load.kumi_min+1);
                load.target = movable_range[0][2]*10*(load.kumi_min+1) + (param.buffer_kaisu*10);
                load.jougen = movable_range[0][2]*10*10 + (param.buffer_kaisu*10);       //目標重量の範囲は10個の単重分でいいんじゃ？
                
            }else{
                printf("010\r\n");
                OYA_DATA_gram[0][1] = float(S1_WEIGHT_target) - OYA_DATA_gram[0][0];
                movable_range[0][2] = OYA_DATA_gram[0][1] / (S1_WEIGHT_kumi - OYA_DATA[0][1]);
                load.target = movable_range[0][2]*10*(load.kumi_min+1);
                load.jougen = movable_range[0][2]*10*1;           //目標重量の範囲は2個の単重分でいいんじゃ？
                
            }
            load.rankOnOff = 0;
            load.rankMin = 0;
            load.rankMak = 0;
            target_jougen[0][0] = float(load.target)/10;
            target_jougen[0][1] = float(load.target+load.jougen)/10;
            res = float(load.target + load.jougen)/10;
            load.spMode = 1;
            load.lightTime = 25;
            load.kumi_priolity_1 = 99;
            load.kumi_priolity_2 = 99;
            
            if(load.target != now_data_gram[0]){
               /* pc.printf(" 1:  nokori%3dsara tugi %5d sara\r\n",S1_WEIGHT_kumi - OYA_DATA[0][1],load.kumi_min+1);
                pc.printf(" 1:  nokori%5.1fg  goukei%5g\r\n",OYA_DATA_gram[0][1],S1_WEIGHT_target);
                pc.printf("%5.1f ~%5.1f \r\n",movable_range[0][0],movable_range[0][1]);
                pc.printf("   ");
                //pc.printf("%5.1f ~%5.1f \r\n",float(load.target)/10,float(load.target+load.jougen)/10);
                pc.printf("%5.1f ~%5.1f \r\n",target_jougen[0][0]  ,target_jougen[0][1]);*/
            }
            if(load.target/10 <= (now_data_gram[0]/10 -2.0) || (now_data_gram[0]/10 +2.0) <= load.target/10 || data_one_print == true){
                data_one_print = false;
                pc.printf(" 01:  nokori%3dsara tugi %5d sara\r\n",S1_WEIGHT_kumi - OYA_DATA[0][1],load.kumi_min+1);
                pc.printf(" 01:  nokori%5.1fg  goukei%5dg\r\n",OYA_DATA_gram[0][1],S1_WEIGHT_target);
                pc.printf(" 01:  %5.1f ~%5.1f ",movable_range[0][0],movable_range[0][1]);
                pc.printf("   ");
                //pc.printf("%5.1f ~%5.1f \r\n",float(load.target)/10,float(load.target+load.jougen)/10);
                pc.printf("%5.1f ~%5.1f \r\n",target_jougen[0][0]  ,target_jougen[0][1]);
                pc.printf(" 01:  %5.1f ~%5.1f \r\n",target_jougen[0][0]/(load.kumi_min+1)  ,target_jougen[0][1]/(load.kumi_min+1));
                //v.imano_Sara_suu(OYA_DATA[0][1]);
                //v.nokori_Sara_suu(S1_WEIGHT_kumi - OYA_DATA[0][1]);
                //v.gram_voice_hani(10,int(movable_range[0][0]*10),int(movable_range[0][1]*10));
                //v.gram_voice_hani(10,int(load.target),int((load.target+load.jougen)));
                if((S1_WEIGHT_kumi - OYA_DATA[0][1]) > 0){
                    v.rank_guide(S1_WEIGHT_rank_no ,int(target_jougen[0][0]/(load.kumi_min+1))+1);
                }
            } 
            now_data_gram[0] = load.target;
            if(combi_EN[0]==true){
                no_combi_counter[0] = 0;
                get_combi_location_kaisuu3(orderCombi1, &secondCnt1);     //組み合わせ確認,組み合わせ候補を抽出
                resultCombi1 = orderCombi1[0];                    //最小組み合わせ結果を取得
                //pc.printf("total:%5.1f\r\n",resultCombi1.total);
            }
        }else{//kumiawase_0==false
            if(L1_WEIGHT_kumi - OYA_DATA[1][1] > 2){ 
                load.kumi_min = 2;
                load.kumi_max = 2;
            }else{
                load.kumi_min = L1_WEIGHT_kumi - OYA_DATA[1][1]-1;
                load.kumi_max = load.kumi_min;
            }
            
            if(load.kumi_flag == KUMI_KAISU2 && (L0_WEIGHT_kumi > OYA_DATA[1][1])){   //回数3で、まだ1段目の場合
                //if((L0_WEIGHT_kumi - OYA_DATA[1][1]) <= 4){
                    //load.kumi_min = L0_WEIGHT_kumi - OYA_DATA[1][1] -1;
                   // load.kumi_max = load.kumi_min;
                //    OYA_DATA_gram[1][1] = float(L0_WEIGHT_target) - OYA_DATA_gram[1][0];
                //    movable_range[1][2] = OYA_DATA_gram[1][1] / (L0_WEIGHT_kumi - OYA_DATA[1][1]);
                //    load.target  = (float(L0_WEIGHT_target) - OYA_DATA_gram[1][0]) * 10;
                //    load.jougen  = (float(L0_WEIGHT_jougen) - OYA_DATA_gram[1][0]) * 10 - load.target;
                OYA_DATA_gram[1][1] = float(L0_WEIGHT_target) - OYA_DATA_gram[1][0];
                movable_range[1][2] = OYA_DATA_gram[1][1] / (L0_WEIGHT_kumi - OYA_DATA[1][1]);
                
                if((L0_WEIGHT_kumi - OYA_DATA[1][1]) == 4){
                    load.kumi_min = 1;
                    load.kumi_max = 1;
                    
                    
                    if(OYA_DATA[1][1] == 0){          //組み合わせが初回の場合
                        load.target =  ((float(L0_WEIGHT_target - load.taimenTarget)    /  float(L0_WEIGHT_kumi))*float(load.kumi_min+1))  *10;
                        load.jougen =  (load.target*1.2) - load.target;
                    }else{
                        load.target = (((OYA_DATA_gram[1][1]) / (float(L0_WEIGHT_kumi) - OYA_DATA[1][1]))*float(load.kumi_min+1)) * 10;
                        load.jougen =  (load.target*1.2) - load.target;
                    }
                    
                }else if((L0_WEIGHT_kumi - OYA_DATA[1][1]) <=2){
                    load.kumi_min = (L0_WEIGHT_kumi - OYA_DATA[1][1]-1);
                    load.kumi_max = load.kumi_min;
                    load.target = (((OYA_DATA_gram[1][1]) / (float(L0_WEIGHT_kumi) - OYA_DATA[1][1]))*float(load.kumi_min+1)) * 10;
                    load.jougen =  (load.target*1.2) - load.target;
                    
                }else if(OYA_DATA[1][1] == 0){          //組み合わせが初回の場合
                    load.kumi_min = 2;//[2]で3皿の意味
                    load.kumi_max = 2;
                    load.target = ((float(L0_WEIGHT_target - load.taimenTarget) / float(L0_WEIGHT_kumi))*float(load.kumi_min+1))  *10;
                    load.jougen =  (load.target*1.2) - load.target;
            
                }else if((L0_WEIGHT_kumi - OYA_DATA[1][1]) > 2){ 
                    load.kumi_min = 2;//[2]で3皿の意味
                    load.kumi_max = 2;
                    load.target = (((OYA_DATA_gram[1][1]) / (float(L0_WEIGHT_kumi) - OYA_DATA[1][1]))* float(load.kumi_min+1)) * 10;
                    load.jougen =  (load.target*1.2) - load.target;
                }
                
            }else if(OYA_DATA[1][1]<3){          //組み合わせが初回の場合
                load.target = movable_range[1][0]*10*(load.kumi_min+1) - load.taimenTarget*10;
                load.jougen = movable_range[1][1]*10*(load.kumi_min+1) - load.taimenTarget*10 - load.target;
                
            }else if((L1_WEIGHT_kumi - OYA_DATA[1][1]) < 3){  //最後の1回の計量は、最高重量の範囲をかなり広げる
                OYA_DATA_gram[1][1] = float(L1_WEIGHT_target) - OYA_DATA_gram[1][0];
                movable_range[1][2] = OYA_DATA_gram[1][1] / (L1_WEIGHT_kumi - OYA_DATA[1][1]);
                load.target = movable_range[1][2]*10*(load.kumi_min+1) + (param.buffer_kaisu*10);
                load.jougen = movable_range[1][2]*10*10 + (param.buffer_kaisu*10);      //目標重量の範囲は10個の単重分でいいんじゃ？
                
            }else{
                OYA_DATA_gram[1][1] = float(L1_WEIGHT_target) - OYA_DATA_gram[1][0];
                movable_range[1][2] = OYA_DATA_gram[1][1] / (L1_WEIGHT_kumi - OYA_DATA[1][1]);
                load.target = movable_range[1][2]*10*(load.kumi_min+1);
                load.jougen = movable_range[1][2]*10*1;
            }
            load.rankOnOff = 0;
            load.rankMin = 0;
            load.rankMak = 0;
            target_jougen[1][0] = float(load.target)/10;
            target_jougen[1][1] = float(load.target+load.jougen)/10;
            res = float(load.target + load.jougen)/10;
            load.spMode = 1;
            load.lightTime = 25;
            load.kumi_priolity_1 = 99;
            load.kumi_priolity_2 = 99;
            
            if(load.target != now_data_gram[1]){
                /*pc.printf(" 2:  nokori%5.1fg  setteisara%5d\r\n",OYA_DATA_gram[1][1],load.kumi_min+1);
                pc.printf("%5.1f ~%5.1f \r\n",movable_range[1][0],movable_range[1][1]);
                pc.printf("   ");
                //pc.printf("%5.1f ~%5.1f \r\n",float(load.target)/10,float(load.target+load.jougen)/10);
                pc.printf("%5.1f ~%5.1f \r\n",target_jougen[1][0]  ,target_jougen[1][1]);*/
            }
            if(load.target/10 <= (now_data_gram[1]/10 -2.0) || (now_data_gram[1]/10 +2.0) <= load.target/10 || data_two_print == true){
                data_two_print = false;
                pc.printf(" 02:  nokori%3dsara tugi %5d sara\r\n",L1_WEIGHT_kumi - OYA_DATA[1][1],load.kumi_min+1);
                pc.printf(" 02:  nokori%5.1fg  goukei%5dg\r\n",OYA_DATA_gram[1][1],L1_WEIGHT_target);
                pc.printf(" 02:  %5.1f ~%5.1f ",movable_range[1][0],movable_range[1][1]);
                pc.printf("   ");
                //pc.printf("%5.1f ~%5.1f \r\n",float(load.target)/10,float(load.target+load.jougen)/10);
                pc.printf("%5.1f ~%5.1f \r\n",target_jougen[1][0]  ,target_jougen[1][1]);
                pc.printf(" 02:  %5.1f ~%5.1f \r\n",target_jougen[1][0]/(load.kumi_min+1)  ,target_jougen[1][1]/(load.kumi_min+1));
                if((L1_WEIGHT_kumi - OYA_DATA[1][1]) > 0){
                    v.rank_guide(L1_WEIGHT_rank_no ,int(target_jougen[1][0]/(load.kumi_min+1))+1);
                }
                //v.imano_Sara_suu(OYA_DATA[1][1]);
                //v.nokori_Sara_suu(S1_WEIGHT_kumi - OYA_DATA[0][1]);
                //v.gram_voice_hani(10,int(movable_range[1][0]*10),int(movable_range[1][1]*10));
                //v.gram_voice_hani(10,int(load.target),int((load.target+load.jougen)));
            
            } 
            now_data_gram[1] = load.target;
            
            if(combi_EN[1]==true){
                no_combi_counter[0] = 1;
                get_combi_location_kaisuu3(orderCombi2, &secondCnt2);     //組み合わせ確認,組み合わせ候補を抽出
                resultCombi2 = orderCombi2[0];                    //最小組み合わせ結果を取得
                //pc.printf("total:%5.1f\r\n",resultCombi2.total);
            }
            
            
        }
        
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////組合せ方法調整END
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //pc.printf("target %5d ~ jougen %5d\r\n",load.target,load.jougen);
            //pc.printf("rankMin %5d ~ rankMak %5d\r\n\r\n\r\n"  ,load.rankMin,load.rankMak);
        
        
        
        
            //pc.printf("range10 %4.1f\r\n",movable_range[1][0]*3);
            //henkan_data = movable_range[1][0]*3;
            //pc.printf("flo range10 %4.1f\r\n",henkan_data);
            //pc.printf("int range10 %5d\r\n",(int)(henkan_data*10));
   // pc.printf("range00 %4.1f: range01 %4.1f\r\n",    movable_range[0][0],movable_range[0][1]);
   // pc.printf("range10 %4.1f: range11 %4.1f\r\n",    movable_range[1][0],movable_range[1][1]);
   // pc.printf("target %5.1f ~ jougen %5.1f\r\n",load.target,load.jougen);
        
        
        //v.gram_voice_hani(10,load.rankMin,load.rankMak);
        
        
           /* pc.printf("OKOK tota0l%5.1f  kumi%5d\r\n",orderCombi[0].total,orderCombi[0].kumi);
            pc.printf("OKOK tota1l%5.1f  kumi%5d\r\n",orderCombi[1].total,orderCombi[1].kumi);
            pc.printf("OKOK tota2l%5.1f  kumi%5d\r\n",orderCombi[2].total,orderCombi[2].kumi);
            pc.printf("OKOK tota3l%5.1f  kumi%5d\r\n",orderCombi[3].total,orderCombi[3].kumi);*/
        
                
                
///////////////////準最適値があれば表示////////////////////////////////////////////////////////////////////
        if(load.secondOnOff == 1){    
            pc.printf("01 :  kumi:%3d/%3d : total:%5.1f  %5.1f ~%5.1f\r\n",bit_count(resultCombi1.kumi),load.kumi_min+1,resultCombi1.total,target_jougen[0][0]  ,target_jougen[0][1]);
            pc.printf("02 :  kumi:%3d/%3d : total:%5.1f  %5.1f ~%5.1f\r\n",bit_count(resultCombi2.kumi),load.kumi_min+1,resultCombi2.total,target_jougen[1][0]  ,target_jougen[1][1]);
            if(      kumiawase_0==true  && bit_count(resultCombi1.kumi) == load.kumi_min+1 && target_jougen[0][1] < resultCombi1.total){
                pc.printf("01 :\r\n");
                show_results_kaisu(OYAHEAD[0] ,&resultCombi1, S1_WEIGHT_rank_no, S1_WEIGHT_kumi, OYA_DATA[0][1], OYA_DATA[0][0]);     //組み合わせに応じたLED点灯,総重量表示
            }else if(kumiawase_0==false && bit_count(resultCombi2.kumi) == load.kumi_min+1 && target_jougen[1][1] < resultCombi2.total){
                pc.printf("02 :\r\n");
                show_results_kaisu(OYAHEAD[1] ,&resultCombi2, L1_WEIGHT_rank_no, L1_WEIGHT_kumi, OYA_DATA[1][1], OYA_DATA[1][0]);     //組み合わせに応じたLED点灯,総重量表示
            }        
        }
///////////////////成立しているものがあれば表示////////////////////////////////////////////////////////////////////
        if((kumiawase_0==true  && bit_count(resultCombi1.kumi) == load.kumi_min+1 && target_jougen[0][0] < resultCombi1.total && resultCombi1.total <= target_jougen[0][1]   ) ||
           (kumiawase_0==false && bit_count(resultCombi2.kumi) == load.kumi_min+1 && target_jougen[1][0] < resultCombi2.total && resultCombi2.total <= target_jougen[1][1]   )){
            se.chime_01();
            //se.chime_01();
            if(resultCombi1.kumi > 0 && target_jougen[0][0] < resultCombi1.total && resultCombi1.total <= target_jougen[0][1]   ){
                OYA_DATA[0][0] = resultCombi1.kumi;
                //pc.printf("OK1 tota0l%5.1f  kumi%3d   data%3d\r\n",orderCombi1[0].total,bit_count(orderCombi1[0].kumi),OYA_DATA[0][0]);
                show_results_kaisu(OYAHEAD[0] ,&resultCombi1, S1_WEIGHT_rank_no, S1_WEIGHT_kumi, OYA_DATA[0][1], OYA_DATA[0][0]);     //組み合わせに応じたLED点灯,総重量表示
                //show_count(&cnt);               //カウント表示
            }else{
                OYA_DATA[0][0] = 0;
            }
            if(resultCombi2.kumi > 0 && target_jougen[1][0] < resultCombi2.total && resultCombi2.total <= target_jougen[1][1]   ){
                OYA_DATA[1][0] = resultCombi2.kumi;
                //pc.printf("OK2 tota0l%5.1f  kumi%3d   data%3d\r\n",orderCombi2[0].total,bit_count(orderCombi2[0].kumi),OYA_DATA[1][0]);
                show_results_kaisu(OYAHEAD[1] ,&resultCombi2, L1_WEIGHT_rank_no, L1_WEIGHT_kumi, OYA_DATA[1][1], OYA_DATA[1][0]);     //組み合わせに応じたLED点灯,総重量表示
                //pc.printf("OK2 tota0l%5.1f  kumi%3d   data%3d\r\n",orderCombi2[0].total,bit_count(orderCombi2[0].kumi),OYA_DATA[1][0]);
                //show_count(&cnt);               //カウント表示
            }else{
                OYA_DATA[1][0] = 0;
            }
        }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        
                //printf("b0\r\n");
        
        
        
        
        
        
        if(load.spMode == SPEED_LOW && 
           (resultCombi1.kumi > 0 && target_jougen[0][0] < resultCombi1.total && resultCombi1.total <= target_jougen[0][1]   ) ||  
           (resultCombi2.kumi > 0 && target_jougen[1][0] < resultCombi2.total && resultCombi2.total <= target_jougen[1][1]   )
           ){
            if(option.kokuti==1||option.kokuti==2||option.kokuti==3) {
                se.Touch_02();
            }
            if(resultCombi1.kumi > 0 && target_jougen[0][0] < resultCombi1.total && resultCombi1.total <= target_jougen[0][1]   ){
                no_combi_counter[0] = 0;
                driveSlowly_kaisu_1(&resultCombi1, &resultCombi2);
                btn.end_force();
            }else{
                no_combi_counter[0] = 1;
                driveSlowly_kaisu_2(&resultCombi2, &resultCombi1);
                btn.end_force();
            }
                
            //comp_kumi=0;
            //組み合わせ分を取り終わっていたら
            if((resultCombi1.kumi == 0 || resultCombi1.kumi == 0) && cnt.max > 0) {
                if(cnt.num < cnt.max) {
                    cnt.num++;
                    //残りカウント数に合わせて音声お知らせ
                    v.output_audio_Count(cnt.max,cnt.num);
                }
            }//if
            if((resultCombi1.kumi == 0 || resultCombi2.kumi == 0)) {
                if(OYA_DATA[0][0]>0 && resultCombi1.kumi == 0 ){
                    //pc.printf("OK1 tota0l%5.1f  kumi%3d   data%3d\r\n",orderCombi1[0].total,bit_count(orderCombi1[0].kumi),OYA_DATA[0][0]);
                    OYA_DATA[0][1] = OYA_DATA[0][1] + bit_count(OYA_DATA[0][0]);
                    //pc.printf("OK1 tota0l%5.1f\r\n",resultCombi1.total);
                    OYA_DATA_gram[0][0] = OYA_DATA_gram[0][0] + resultCombi1.total;
                    data_one_print = true;
                }
                if(OYA_DATA[1][0]>0 && resultCombi2.kumi == 0){
                    OYA_DATA[1][1] = OYA_DATA[1][1] + bit_count(OYA_DATA[1][0]);
                    OYA_DATA_gram[1][0] = OYA_DATA_gram[1][0] + resultCombi2.total;
                    data_two_print = true;
                }
                S1_NO_COMBI=false;
                L1_NO_COMBI=false;
                if(cnt.num < cnt.max) {
                    cnt.num++;
                    //残りカウント数に合わせて音声お知らせ
                    v.output_audio_Count(cnt.max,cnt.num);
                }
            }//if
            pc.printf("1.%3dko  2.%3dko\r\n",OYA_DATA[0][1],OYA_DATA[1][1]);
        }
        OYA_DATA[0][0]=0;OYA_DATA[1][0]=0; 
        resultCombi1.kumi  = 0;
        resultCombi1.total = 0;
        resultCombi2.kumi  = 0;
        resultCombi2.total = 0;
        lcd_print_kaisuu3();
        //se.Touch_02();
            
        
        
        
        
        
        


    }
}



/////////////////////////////////////////////////////////////////////////////////
//重量組合せ関数
//すべての組合せ結果を計算する！
/////////////////////////////////////////////////////////////////////////////////
void get_combi_location_kaisuu3(combiData_t com[], int *secondCnt)
{
    int bit,bk,in = 0; //in:物が載っている場所を取得
    int bit_num=0;    //成立した個数の保存
    double sum=0;   //組み合わせ候補箇所の重量を合算していく
    float min = float(load.target)/10 + param.buffer;  //最小値(バッファー有り)
    float res = float(load.target + load.jougen)/10;    //組み合わせ重量の上限値を取得
    float resDefault = res;                   //上限値のデフォルト値をここでバックアップ
    bool checkBtnJunsaiteki = false;            //ボタン押下で準最適値モードになったか

    ArrayIni(com, BK_CNT);   //初期化(組み合わせ保持している分を初期化)
    in = cel.getAnteiIn();   //計量物の数を取得
    //pc.printf("%5d ~ %5d  %5.1f ~ %5.1f\r\n",load.target,load.jougen,min,res);
    //v.gram_voice_hani(10,load.rankMin,load.rankMak);
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
        
    } else if(load.secondOnOff == 1 && (bit_count(in & (ALL_HEAD ^ (oneBit[OYAHEAD[0]] | oneBit[OYAHEAD[1]]))) >= (bscData.startKinji-bit_count(stop_cells)))) {
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
                if( maxWeight < weight[bit]) maxWeight = weight[bit];   //最重量値の場合は値取得
                if( minWeight > weight[bit]) minWeight = weight[bit];   //最軽量値の場合は値取得
            }
            bit++;
        }
        
        if(no_combi_counter[0] == 0){
            if(no_combi_counter[1]>=3){
                if( ((maxWeight - minWeight) > load.fuzoroiBoushi+5) && (load.fuzoroiBoushi > 0)) {
                    continue;
                }
            }else{
                if( ((maxWeight - minWeight) > load.fuzoroiBoushi) && (load.fuzoroiBoushi > 0)) {
                    continue;
                }
            }
        }
        else{                           
            if(no_combi_counter[2]>=5){
                if( ((maxWeight - minWeight) > load.fuzoroiBoushi+5) && (load.fuzoroiBoushi > 0)) {
                    continue;
                }
            }else{
                if( ((maxWeight - minWeight) > load.fuzoroiBoushi) && (load.fuzoroiBoushi > 0)) {
                    continue;
                }
            }
        }
        /*
        //不揃い防止値(設定重量)より最大値、最小値の差があれば処理終了
        if( ((maxWeight - minWeight) > load.fuzoroiBoushi) && (load.fuzoroiBoushi > 0)) {
            continue;
        }*/
        
    //pc.printf("sum %5.1f min %5.1f  max %5.1f  kumi%d\r\n",sum,minWeight,maxWeight,bit_count(i));
    
    
        //指定範囲に入っていなければ、処理終了
        if(load.rankOnOff == 1 && ( minWeight < (float(load.rankMin)/10) || maxWeight > (float(load.rankMak)/10) )) {
            continue;
        }
    //pc.printf("%5.1f ~ %5.1f\r\n",float(load.rankMin)/10,float(load.rankMak)/10);
        
        
        //一番最適な重量を取得する
        //if((sum >= min) && (int(sum) <= int(res))) {
        //if((load.kumi_flag!=KUMI_LIMIT && (( sum >= min                                      ) && (int(sum) <= int(res)))) ||
        if(((float(bit_count(i))*param.buffer2<float(res))&&
          ((load.kumi_flag==KUMI_KAISU1 || load.kumi_flag==KUMI_KAISU2 )&& (((sum >= (float(min) + (float(bit_count(i))*param.buffer2))) && (int(sum) <= int(res)))))) ||
          ((load.kumi_flag==KUMI_KAISU1 || load.kumi_flag==KUMI_KAISU2 ) && (int(sum) >= int(res)) && (int(sum) <= int(res))))
          
        {
            //pc.printf("OK1 sum%5.1f  kumi%5d\r\n",sum,bit_count(i));

            //指定された組み合わせ条件を満たしているか確認
            if(check_combi_condition(i,&load) == false) {
                continue;
            }
            //pc.printf("OK2 sum%5.1f  kumi%5d\r\n",sum,bit_count(i));

            //組み合わせ成立があるならば
            //準最適値処理は行わない
            if(int(sum) <= int(resDefault)/* || combi_OK != 0*/) {
                *secondCnt = 0;
            }

            //準最適値「組合せなし」通知は不要
            checkBtnJunsaiteki = false;

            //pc.printf("OK3 sum%5.1f  kumi%5d\r\n",sum,bit_count(i));
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
    
            /*pc.printf("OK4 tota0l%5.1f  kumi%5d\r\n",com[0].total,com[0].kumi);
            pc.printf("OK4 tota1l%5.1f  kumi%5d\r\n",com[1].total,com[1].kumi);
            pc.printf("OK4 tota2l%5.1f  kumi%5d\r\n",com[2].total,com[2].kumi);
            pc.printf("OK4 tota3l%5.1f  kumi%5d\r\n",com[3].total,com[3].kumi);*/

    //ボタン操作で準最適値モードON後、
    //一つも組合せなければ「組合せがない」ことを通知する
    
    //printf("%5d ,%5d,%5d,%8.1f,%8.1f,%5d%5d ,\r\n",bit_count(in),bscData.startKinji-bit_count(stop_cells),bit_count(stop_cells),res,com[0].total,KINJITI_MAX_WHIGHT,int(sum));
    if(combi_OK == 0 && Compulsion_no_combi_v_flg == false){
        if( ((checkBtnJunsaiteki == true) && (res == KINJITI_MAX_WHIGHT))) {
            v.output_audio(MSG_NO_JUNSAITEKI2);
            v.wait_echo();
        }else if((int(com[0].total) == DAMMY_MAX) && (bit_count(in | (gOyaHead)) >= (bscData.startKinji-bit_count(stop_cells)))){ //組合せがないが、皿に物がお\ｐｌ
            if(      (bit_count(in | (gOyaHead)) >= (product.head-bit_count(stop_cells))) //||   //全ての皿に物が載った
              /*((load.katamen > 0) && (bit_count(in) >= ((bscData.startKinji/2)-bit_count(stop_cells))))*/){
                if(no_combi_counter[0] == 0){   no_combi_counter[1] += 1; if(no_combi_counter[1]>5){no_combi_counter[1]=5;}}
                else{                           no_combi_counter[2] += 1; if(no_combi_counter[2]>5){no_combi_counter[2]=5;}}
                if(option.no_combi_v==1){
                    if(no_combi_v_flg == false){
                        if(kumiawase_0==true){
                            S1_NO_COMBI=true;
                            data_one_print = true;
                        }else{
                            L1_NO_COMBI=true;
                            data_two_print = true;
                        }
                        if(S1_NO_COMBI==true && L1_NO_COMBI==true){
                            v.output_audio(MSG_NO_JUNSAITEKI2);
                            v.wait_echo();
                            no_combi_v_flg = true;
                        }
                    }
                }
            }else if((bit_count(in | (gOyaHead)) >= (bscData.startKinji-bit_count(stop_cells))) //||  //準最適値の設定皿数に達した
              /*((load.katamen > 0) && (bit_count(in) >= ((bscData.startKinji/2)-bit_count(stop_cells))))*/){
                if(no_combi_counter[0] == 0){   no_combi_counter[1] += 1; if(no_combi_counter[1]>5){no_combi_counter[1]=5;}}
                else{                           no_combi_counter[2] += 1; if(no_combi_counter[2]>5){no_combi_counter[2]=5;}}
                if(option.no_combi_v==1){
                    if(no_combi_v_flg == false){
                        v.output_audio(MSG_NO_JUNSAITEKI2);
                        v.wait_echo();
                        no_combi_v_flg = true;
                    }
                }
            }
        }
    }else{
        no_combi_v_flg = false;
    }

            //pc.printf("OK5 total%5.1f  kumi%5d\r\n",com[0].total,com[0].kumi);
    delete[] weight;
    return;
}

void lcd_print_kaisuu3(){
    char ko[] = {0xBA};
    char ato[] = {0xB1,0xC4};
    
    lcd.locate(0,0);
    lcd_print_rank(S1_WEIGHT_rank_no);
    lcd.printf(":%2d",S1_WEIGHT_kumi);
    lcd.printf(ko);
    lcd.printf(":%3d:",S1_WEIGHT_target);
    //lcd.printf(":%3d:",S1_WEIGHT_jougen);
    lcd.printf(ato);
    lcd.printf("%2d",S1_WEIGHT_kumi - OYA_DATA[0][1]);
    lcd.printf(ko);
    
    if(L1_WEIGHT_rank_en == 1){
        lcd.locate(0,1);
        lcd_print_rank(L1_WEIGHT_rank_no);
        lcd.printf(":%2d",L1_WEIGHT_kumi);
        lcd.printf(ko);
        lcd.printf(":%3d:",L1_WEIGHT_target);
        lcd.printf(ato);
        lcd.printf("%2d",L1_WEIGHT_kumi - OYA_DATA[1][1]);
        lcd.printf(ko);
        //lcd.printf(":%2d%s:%3d~%3d   ",L1_WEIGHT_kumi,ko,L1_WEIGHT_target,S1_WEIGHT_jougen);
    }else{
        lcd.blank(1);
    }
}

void show_results_kaisu(int rank0or1,  const combiData_t *com, int RankNo, int rankkumi, int nowkosu, int befourekumi){
    float res = float(load.target + load.jougen)/10;     //組み合わせ重量の上限値を取得
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
            lcd.printf(kumiawase);
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
            matrix_count(rank0or1 ,com->kumi ,rankkumi - nowkosu,befourekumi,1);
            //led.REGIS3(com->kumi);
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
                    for(int loop0 = 0;loop0<product.head;loop0+=1){
                        if((com->kumi & oneBit[loop0]) > 0){
                            iro[loop0] = CG_01 + int(com->total - res);
                        }
                    }
                    wait(0.02);
                    led.REGIS(0);
                    
                    //led.REGIS3(resultCombi.kumi);
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
            lcd.printf(kumiawase);
            lcd.locate(0,1);
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
        if(load.spMode == SPEED_HIGH) {
            if(option.kokuti==1||option.kokuti==2||option.kokuti==3){
                se.Touch_02();
            }
            led.REGIS(com->kumi);

            //第二上限値内の組み合わせ成立だったらLED点滅させる
            if((load.secondOnOff == 1 || btn.get_lock_flg() == true) && (int(com->total) > res)) {
                se.Touch_02();
                int btn_flg = 0;
                for(int i = 0; i <= load.lightTime; i += 1) {
                    led.REGIS(com->kumi);
                    wait(0.05);
                    led.REGIS(0);
                    wait(0.05);
                    //再計算ボタンが押されたら、待機時間を待たずに終了する。
                    //ただし、ボタンが離されたことを確認後にボタンを押されたらとする
                    //(ボタンを押すことでここに入ってきてるかもしれないので)
                    if(btn_flg == 1 && btn.down_press() == true) {
                        se.Touch_02();
                        i = load.lightTime + 1;
                    } else if(btn.down_press() == false) {
                        btn_flg = 1;
                    }
                }
                no_combi_v_flg = false;
            } else {
                led.REGIS(com->kumi);
                //wait(load.lightTime*0.1);
                for(int i = 0; i <= load.lightTime; i += 1) {
                    wait(0.1);
                    //再計算ボタンが押されたら、待機時間を待たずに終了する。
                    if(btn.down_press() == true) {
                        se.Touch_02();
                        i = load.lightTime + 1;
                    }
                }
                no_combi_v_flg = false;
            }
        }
        //設定時間LED表示(ハイパーモード時のみ)
        else if(load.spMode == SPEED_HYPER){
            if(option.kokuti==1||option.kokuti==2||option.kokuti==3){
                se.Touch_02();
            }
            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                led.REGIS3(com->kumi);
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
                        led.REGIS3(resultCombi.kumi);
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
                matrix_count(rank0or1 ,com->kumi ,rankkumi - nowkosu,befourekumi,1);
                
                //led.REGIS3(com->kumi);
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
                miri_end=0;
                //miri_sec.attach( &miri_count, 0.1);
                miri_sec.attach( &miri_count, float(load.lightTime/10));
                for(int i=0;i <= load.lightTime; i += 1){ //指定時間まで光らせる(成立すれば終了)
                    if(product.matrix_mode == MATRIX_ON){
                        for(int loop0 = 0;loop0<product.head;loop0+=1){
                            if(kumiawase_0 == true && loop0 == OYAHEAD[0]){
                                iro[loop0] = PENTA_R;
                            }else if(kumiawase_0 == false && loop0 == OYAHEAD[1]){
                                iro[loop0] = PENTA_R;
                            }else if((com->kumi & oneBit[loop0]) > 0){
                                iro[loop0] = CG_01 + int(com->total - res);
                            }else {
                                iro[loop0] = C_NULL;
                            }
                        }
                        wait(0.02);
                        led.REGIS(0);
                        /*if(i == 0){
                            led.REGIS3(resultCombi.kumi);
                            lcd.bigNumber(resultCombi.total); //大文字で重量を表示
                        }*/
                    }else{
                        led.REGIS(resultCombi.kumi);
                        wait(0.02);
                        led.REGIS(0);
                    }
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
void driveSlowly_kaisu_1(combiData_t *com ,combiData_t *com2)
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
    printf("%5d\r\n",match_kumi);
    for(int i=0; i < product.head; i++) {
        //計量物が載っているか取得
        if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
            cel.set_head(i);
            cel.setAin_zero(i);
        } else if(cel.checkIn(i)) {
            inNow += oneBit[i];
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
        }
        inChk = inChk <<1;
        nowweightVale4[k] = nowweightVale[k];
    }
    //組合せ成立後の無限ループ
    //load.secondOnOff=0;
    //show_results(com);     //組み合わせに応じたLED点灯,総重量表示
    show_results_kaisu(OYAHEAD[0] ,com, S1_WEIGHT_rank_no, S1_WEIGHT_kumi, OYA_DATA[0][1],OYA_DATA[0][0]);     //組み合わせに応じたLED点灯,総重量表示
    comp_kumi = com->kumi;
    while(com->kumi) {
        missPoint = 0;  //初期化
        if((load.kumi_flag == KUMI_KAISU1 || load.kumi_flag == KUMI_KAISU2) && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            comp_kumi = com->kumi;
            matrix_count(OYAHEAD[0] ,com->kumi ,S1_WEIGHT_kumi - OYA_DATA[0][1],OYA_DATA[0][0],1);
        } else {
            led.REGIS(com->kumi);
        }
        //******* 川4/17追加  ディスプレイ表示更新
        disp_wait = disp_wait+1;
        if(disp_wait==5) {
            if((int(com->total) < res)) {
                //lcd.settings(&load);            //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
            }
            //load.secondOnOff=0;
            //show_results(com);     //組み合わせに応じたLED点灯,総重量表示
            show_results_kaisu(OYAHEAD[0] ,com, S1_WEIGHT_rank_no, S1_WEIGHT_kumi, OYA_DATA[0][1],OYA_DATA[0][0]);     //組み合わせに応じたLED点灯,総重量表示
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
            btn.end_force();
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
            if(load.kumi_flag == KUMI_OYAKO || load.kumi_flag == KUMI_OYAKO_HALF || (load.kumi_flag == KUMI_KAISU1 || load.kumi_flag == KUMI_KAISU2)) {
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
                }else if(btnset==8){
                    set_option_setting(OP_CHAR_ROTATE);
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
                //lcd.settings(&load);      //設定内容を表示
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
                        //lcd.settings(&load);            //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
                    }
                    //show_results(com);     //組み合わせに応じたLED点灯,総重量表示
                    show_results_kaisu(OYAHEAD[0] ,com, S1_WEIGHT_rank_no, S1_WEIGHT_kumi, OYA_DATA[0][1],OYA_DATA[0][0]);     //組み合わせに応じたLED点灯,総重量表示
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
                inNow = infoNow(i, inNow);
                //}
                //printf("03E_%2d\r",bit_count(inNow));
            }
            //デバッグ用ここから
            if( product.productType == STANDARD_D || product.productType == JUNIOR_D || product.productType == BIG_D) {
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
                                    missPoint = inChk;  //取り間違い位置を取得
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
                                    missPoint = inChk;  //取り間違い位置を取得
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

        if((load.kumi_flag == KUMI_KAISU1 || load.kumi_flag == KUMI_KAISU2) && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            matrix_count(OYAHEAD[0] ,com->kumi ,S1_WEIGHT_kumi - OYA_DATA[0][1],OYA_DATA[0][0],1);
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
                if(product.use_usb==1 && option.usb_EN==true) {
                    nowlap = coulap;
                    coulap = 0;
                    total_kumi_cnt += 1;
                    kumi_value_data = float(int(com->total*10))/10;
                    average_kumi_value = (((average_kumi_value * (total_kumi_cnt -1)) + kumi_value_data) / total_kumi_cnt);
                    bit_num = bit_count(match_kumi);    //成立した個数の保存
                    bit_place =match_kumi ;
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
void driveSlowly_kaisu_2(combiData_t *com ,combiData_t *com2)
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
    printf("%5d\r\n",match_kumi);
    for(int i=0; i < product.head; i++) {
        //計量物が載っているか取得
        if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
            cel.set_head(i);
            cel.setAin_zero(i);
        } else if(cel.checkIn(i)) {
            inNow += oneBit[i];
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
        }
        inChk = inChk <<1;
        nowweightVale4[k] = nowweightVale[k];
    }
    //組合せ成立後の無限ループ
    //load.secondOnOff=0;
    //show_results(com);     //組み合わせに応じたLED点灯,総重量表示

    comp_kumi = com->kumi;
    show_results_kaisu(OYAHEAD[1] ,com, L1_WEIGHT_rank_no , L1_WEIGHT_kumi, OYA_DATA[1][1],OYA_DATA[1][0]);     //組み合わせに応じたLED点灯,総重量表示
    while(com->kumi) {
        missPoint = 0;  //初期化
        if((load.kumi_flag == KUMI_KAISU1 || load.kumi_flag == KUMI_KAISU2)&& (product.productType= MATRIX || product.matrix_mode == MATRIX_ON)) {
            comp_kumi = com->kumi;
            matrix_count(OYAHEAD[1] ,com->kumi ,L1_WEIGHT_kumi - OYA_DATA[1][1],OYA_DATA[1][0],1);
        } else {
            led.REGIS(com->kumi);
        }
        //******* 川4/17追加  ディスプレイ表示更新
        disp_wait = disp_wait+1;
        if(disp_wait==5) {
            if((int(com->total) < res)) {
                //lcd.settings(&load);            //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
            }
            //load.secondOnOff=0;
            //show_results(com);     //組み合わせに応じたLED点灯,総重量表示
            show_results_kaisu(OYAHEAD[1] ,com, L1_WEIGHT_rank_no, L1_WEIGHT_kumi, OYA_DATA[1][1],OYA_DATA[1][0]);     //組み合わせに応じたLED点灯,総重量表示
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
            btn.end_force();
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
            if(load.kumi_flag == KUMI_OYAKO || load.kumi_flag == KUMI_OYAKO_HALF || (load.kumi_flag == KUMI_KAISU1 || load.kumi_flag == KUMI_KAISU2)) {
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
                }else if(btnset==8){
                    set_option_setting(OP_CHAR_ROTATE);
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
                //lcd.settings(&load);      //設定内容を表示
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
                        //lcd.settings(&load);            //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
                    }
                    //show_results(com);     //組み合わせに応じたLED点灯,総重量表示
                    show_results_kaisu(OYAHEAD[1] ,com, L1_WEIGHT_rank_no, L1_WEIGHT_kumi, OYA_DATA[1][1],OYA_DATA[1][0]);     //組み合わせに応じたLED点灯,総重量表示
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
                inNow = infoNow(i, inNow);
                //}
                //printf("03E_%2d\r",bit_count(inNow));
            }
            //デバッグ用ここから
            if( product.productType == STANDARD_D || product.productType == JUNIOR_D || product.productType == BIG_D) {
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
                                    missPoint = inChk;  //取り間違い位置を取得
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
                                    missPoint = inChk;  //取り間違い位置を取得
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

        if((load.kumi_flag == KUMI_KAISU1 || load.kumi_flag == KUMI_KAISU2) && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            matrix_count(OYAHEAD[1] ,com->kumi ,L1_WEIGHT_kumi - OYA_DATA[1][1],OYA_DATA[1][0],1);
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
                if(product.use_usb==1 && option.usb_EN==true) {
                    nowlap = coulap;
                    coulap = 0;
                    total_kumi_cnt += 1;
                    kumi_value_data = float(int(com->total*10))/10;
                    average_kumi_value = (((average_kumi_value * (total_kumi_cnt -1)) + kumi_value_data) / total_kumi_cnt);
                    bit_num = bit_count(match_kumi);    //成立した個数の保存
                    bit_place =match_kumi ;
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

void lcd_print_rank(int no){
    if(CG_RANK_START < no){
        no = no-CG_RANK_START+C_RANK_START;
    }
    if(CR_01 <= no && no <=CRR_99){
        lcd.printf("%2d",no-(CR_01-1));
    }else if(C_9S<=no&&no<=C_MA){
        if(C_9S<=no&&no<=C_2S){
            lcd.printf("%dS",C_S-no+1);
        }else if(C_2L<=no&&no<=C_9L){
            lcd.printf("%dL",no-C_L+1);
        }else{
            switch(no) {
                case(C_S):
                    lcd.printf(" S");
                    break;
                case(C_M):
                    lcd.printf(" M");
                    break;
                case(C_L):
                    lcd.printf(" L");
                    break;
                case(C_SA):
                    lcd.printf("SA");
                    break;
                case(C_MA):
                    lcd.printf("MA");
                    break;
            }
        }
    }
}

void matrix_count(int oyahead, int kumi_Data ,int fast_no ,int befourekumi ,int green_yn){
    int minas_point = 0;
    int green_buffer = CG_RANK_START - C_RANK_START; //緑にする場合にenumに足す数
    
    for(int o=0; o < product.head;o++){
        if(o==oyahead){
            iro[o] = PENTA_R - green_yn;
        }
        if(oneBit[o] & kumi_Data){
            iro[o]=CR_01 + fast_no-1 -minas_point + green_buffer * green_yn;
            //minas_point += 1;
            //pc.printf("fast.%3d| minas%3d|kumiD%3d|bkumi%3d\r\n",fast_no,minas_point,kumi_Data,befourekumi);
        }
        if(oneBit[o] & befourekumi){
            minas_point += 1;
            //pc.printf("befourekumi\r\n");
        }
    }
}



