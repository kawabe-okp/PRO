void driveStart_comzan();
void get_combi_location_comzan(combiData_t com[], combiData_t com2[], combiData_t com3[], int *secondCnt);
void show_results_comzan(const combiData_t *com);
void driveSlowly_comzan(combiData_t *com);



void driveStart_comzan()
{
    char multiweight[SIZE] = {0xD7,0xDD,0xB8,0xCD,0xDE,0xC2,' ' ,0xB8,0xD0,0xB1,0xDC,0xBE,0x00};
    char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
                   ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼﾀ
    int secondCnt = 0;                        //一定数に達したら準最適値処理開始
    int *stay_cnt = new int[product.head];      //滞留警告判断のためのカウント変数
    //float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    cntData_t cnt= {0,0,0};                     //カウント機能の値保持用
    combiData_t orderCombi[BK_CNT] = {0,0.0,0};   //組み合わせ候補のデータ(配列)
    combiData_t orderCombi2[BK_CNT] = {0,0.0,0};   //組み合わせ候補のデータ(配列)
    combiData_t orderCombi3[BK_CNT] = {0,0.0,0};   //組み合わせ候補のデータ(配列)
    combiData_t resultCombi;                    //組み合わせ結果(最小値取得用)
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    cnt.nowMode = load.spMode;       //処理モード取得
    cnt_miss_and_kikakugai=0;      //取り間違いと規格外が発生した回数
    cnt_junsaiteki=0;               //準最適値が発生した回数
    int btnset =0;//計量中の下ボタン押しで出てくる値保存
    int SerialNumber = 0; //USBを連続で使用する際の連番値
    int first_target = load.target;
    int first_jougen = load.jougen;
    int scond_target = load.taimenTarget;
    int scond_jougen = load.taimenJougen;
    no_combi_v_flg = false;
    float save_weight[12] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    set_stopcell();
    lcd.cls();
    //1行目LCD表示
    lcd.settings(&load);      //設定内容を表示
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

    //1行目LCD表示
    lcd.settings(&load);      //設定内容を表示
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
    while(1) {
        /*if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            for(int l=0;l<product.head;l++){
                if(iro[l]==C_NULL){
                    iro[l]=C_NUSE;
                }
            }
        }*/
    //******ボタン設定start********************************************************
        btn.new_keep_btn();         //スイッチ(New)取得
        if(product.limit_on==NUMBER_LIMIT) {
            if(kumi_en_check()==false) {
                se.mistake();                   //警告音
                v.over_limit();
                btn.force_ESC();
            } else {
            }
        }
        //esc_swを押すと、Menu画面に戻る
        if(btn.esc_press()||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)) {
            meas_rap.detach();
            se.Touch_01();
            //if((load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT || load.kumi_flag == KUMI_COMBI )&& total_kumi_cnt>0) {
            if(total_kumi_cnt > 0) {
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
                if(product.use_usb==1 && option.usb_EN==true) {
                    lcd.printf(saving[0]);
                }
                if(product.use_usb==1 && option.usb_EN==true) {
                    file.save_SAGYOU(ji,hun,file.get_preset()+1,load.target,total_kumi_cnt,average_kumi_value);
                }
                result_LCD(ji,hun);
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
            return;
        }

        //上ボタンが離されたら、操作ロック解除
        if( btn.down_press() == false) {
            btn.operation_unlock();
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            auto_zero_point();
            //1行目LCD表示
            lcd.cls();
            lcd.settings(&load);      //設定内容を表示
            //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        } else if(btn.up_press() == true) {
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
            
            select_keiryo_option(0, load.kumi_flag, 0);
            
            
            /*
            
            
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
            if(load.kumi_flag == KUMI_COMZAN) {
                btnset = Chenge_vaule_btnD(0);
                if(btnset==0) {
                    auto_zero_off = true;
                    start_keiryo(0);
                    auto_zero_off = false;
                }else if(btnset==1){
                    set_option_setting(OP_KOKUTI);
                }else if(btnset==2){
                    set_option_setting(OP_NUMBER_COLOR);
                } 
            }
            //ゆっくりモード時には、
            else if(load.spMode == SPEED_LOW) {
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
                }else if(btnset==5){
                    set_option_setting(OP_NO_COMBI_V);
                }else if(btnset==6){
                    set_option_setting(OP_KOKUTI);
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
        }

        btn.old_btn();       //スイッチ更新
        
    //******ボタン設定end********************************************************

    //******計量start*******************************************************
        for(int i = 0; i < product.head; i++) {
            setHeadVolt(i, param.KURI);
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
        }
        //デバッグ用ここまで
    //******計量end*******************************************************
        
        //第2上限値適用するか否か
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
        
        
        
        
        
        //1行目LCD表示
        lcd.settings(&load);      //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
        //重量確認(適正でない重量があるため処理を飛ばす)
        //if(check_max_weight(cel.over_weight()) == false) {
        //    continue;
        //}
        
        if(combi_chk==0){
            combi_chk=1;
        }else {
            combi_chk=0;
        }
        resultCombi.kumi = 0;
        resultCombi.total = 0;
        resultCombi.priolity = 0;
        for(int i = 0; i < BK_CNT; i++) {
            orderCombi[i].kumi = 0;
            orderCombi[i].total = 0;
            orderCombi[i].priolity = 0;
            //orderCombi2[i].kumi = 0;
            orderCombi2[i].total = 0;
            orderCombi2[i].priolity = 0;
            orderCombi3[i].kumi = 0;
            orderCombi3[i].total = 0;
            orderCombi3[i].priolity = 0;
        }
        get_combi_location_comzan(orderCombi, orderCombi2, orderCombi3, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
        resultCombi = orderCombi[0];                    //最小組み合わせ結果を取得
        


        //振動検知機能がONだったら
        if(param.vivration >= 1) {
            //繰り返し確認する
            for(int i = 0; i < BK_CNT; i++) {
                //組み合わせ候補が安定しているか
                if(checkWeightTaget(&orderCombi[i]) == true) {
                    resultCombi = orderCombi[i];
                    break;
                }
                if(checkWeightTaget(&orderCombi2[i]) == true) {
                    orderCombi2[0] = orderCombi2[i];
                    break;
                }
                if(checkWeightTaget(&orderCombi3[i]) == true) {
                    orderCombi3[0] = orderCombi3[i];
                    break;
                }
            }
        }
        lcd.settings(&load);      //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
        if((product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)&& resultCombi.kumi > 0) {
            for(int l=0;l<product.head;l++){
                iro[l]=C_NULL;
            }
        }
        
        show_results_comzan(&resultCombi);     //組み合わせに応じたLED点灯,総重量表示
        show_count(&cnt);               //カウント表示
        //printf("%7.2f\r\n", float(load.target) + (float(bit_count(resultCombi.kumi))*param.buffer2));

        if(load.spMode == SPEED_HIGH || load.spMode == SPEED_HYPER) {
            announceRetention(resultCombi.kumi, stay_cnt);   //滞留アナウンス
        }
        //if(resultCombi.kumi > 0 ){
        //    printf("%8.1f \r\n",resultCombi.total);
       // }

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
            driveSlowly_comzan(&resultCombi);
            btn.end_force();
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
        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            for(int l=0;l<product.head;l++){
                if(iro[l]==C_NULL){
                    iro[l]=C_NUSE;
                }
            }
        }
    }//while
}

/////////////////////////////////////////////////////////////////////////////////
//重量組合せ関数
//すべての組合せ結果を計算する！
/////////////////////////////////////////////////////////////////////////////////
//int combi_OK = 0;
void get_combi_location_comzan(combiData_t com[], combiData_t com2[], combiData_t com3[], int *secondCnt)
{
    int bit,bk,in = 0; //in:物が載っている場所を取得
    int bit_num=0;    //成立した個数の保存
    double sum=0;   //組み合わせ候補箇所の重量を合算していく
    float min = float(load.target) + param.buffer;  //最小値(バッファー有り)
    float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    float resDefault = res;                   //上限値のデフォルト値をここでバックアップ
    bool checkBtnJunsaiteki = false;            //ボタン押下で準最適値モードになったか
    bool combi_OK_flug = false;             //１個でも成立した物があるか
    int combi_flg =0;                       //組合せ内容0:無し 1:１個　2:親子11 3:親子12　4:子のみ

    ArrayIni(com, BK_CNT);   //初期化(組み合わせ保持している分を初期化)
    ArrayIni(com2, BK_CNT);   //初期化(組み合わせ保持している分を初期化)
    
    in = cel.getAnteiIn();   //計量物の数を取得

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
        if((sum >= min) && (int(sum) <= int(res))) {
        /*if((load.kumi_flag!=KUMI_LIMIT && (( sum >= min                                      ) && (int(sum) <= int(res)))) ||
          ((float(bit_count(i))*param.buffer2<float(res))&&
          (load.kumi_flag==KUMI_LIMIT && (((sum >= (float(min) + (float(bit_count(i))*param.buffer2))) && (int(sum) <= int(res)))))) ||
          (load.kumi_flag==KUMI_LIMIT && (int(sum) >= int(res)) && (int(sum) <= int(res))))
          
        {*/
            //printf(":x > min:");

            //指定された組み合わせ条件を満たしているか確認
            
            
            //printf("%6.1f :%4d:%4d:%4d\r\n",sum,i,i & gOyaHead,bit_count(i));
            
            
            //親の位置に計量物無い、親同士が組み合わせ対象の場合処理抜け
            combi_flg = 1;
           /* if(i & gOyaHead == 1 && bit_count(i) == 1){
                combi_flg = 1;
            }else if((i & gOyaHead == 1) && (bit_count(i) == 2)){
                combi_flg = 2;
            }else if((i & gOyaHead == 1) && (bit_count(i) == 3)){
                combi_flg = 3;
            }else if((bit_count(i) == 2)){
                combi_flg = 4;
                //printf("AAA\r\n");
            }else{
                continue;
            }*/
            //printf("%4d\r\n",combi_flg);
            
            if(bit_count(i) < 2 || 3 < bit_count(i)) {
                combi_flg = 0;
                continue;
            }else{
            }
            
            //printf("%3d:%5.1f:%2d  \r\n",i,sum,combi_flg);
            
            //if(((bits & gOyaHead) == 0) || ( bit_count((bits & gOyaHead)) == 2) || (bit_count(bits & gKoHead) < 1) || (bit_count(bits & gKoHead) < load->kumi_min+1) || bit_count(bits & gKoHead) > load->kumi_max+1) {
            //    continue;
            //}
            
            
            
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
            if(combi_flg == 1 || combi_flg == 2){
                com[BK_CNT -1].kumi = i;
                com[BK_CNT -1].total = sum;
                qsort(com, BK_CNT, sizeof(combiData_t), comp );
            }else if(combi_flg == 3){
                com2[BK_CNT -1].kumi = i;
                com2[BK_CNT -1].total = sum;
                qsort(com2, BK_CNT, sizeof(combiData_t), comp );
            }else if(combi_flg == 4){
                com3[BK_CNT -1].kumi = i;
                com3[BK_CNT -1].total = sum;
                qsort(com3, BK_CNT, sizeof(combiData_t), comp );
            }
            
                
                
                
              //for (int j=0;j<BK_CNT -1;j++){
              //     pc.printf("1 %d  %4f  %2d  \r\n",com[j].kumi, com[j].total,com[j].priolity);
              //     pc.printf("2 %d  %4f  %2d  \r\n",com2[j].kumi, com2[j].total,com2[j].priolity);
              //     pc.printf("3 %d  %4f  %2d  \r\n",com3[j].kumi, com3[j].total,com3[j].priolity);
              //}

            //res(組合せ重量)を更新
            if(com[BK_CNT-1].total != DAMMY_MAX) {
                //res = com[BK_CNT-1].total;
            }
        }
    }//loop終了

    //組み合わせ成立があるならば
    //準最適値のデータを消す
    if(combi_OK_flug == true){//
        no_combi_v_flg = false;
        //printf("DELETE\r\n");
        for(int i = 0; i < BK_CNT; i++) {
            //printf("%2d :%4d :%6.1f:%4d\r\n",i,com[BK_CNT].kumi,com[BK_CNT].total,com[BK_CNT].priolity);
            if(com[i].total > int(resDefault)){
                com[i].kumi = 0;
                com[i].total = 1;
                com[i].priolity = 999;
                //printf("reset\r\n");
            }
            if(com2[i].total > int(resDefault)){
                com2[i].kumi = 0;
                com2[i].total = 1;
                com2[i].priolity = 999;
                //printf("reset\r\n");
            }
            if(com3[i].total > int(resDefault)){
                com3[i].kumi = 0;
                com3[i].total = 1;
                com3[i].priolity = 999;
                //printf("reset\r\n");
            }
        }
    }

    //ボタン操作で準最適値モードON後、
    //一つも組合せなければ「組合せがない」ことを通知する
    
    //printf("A0 total:%d |flg:%d \r\n",int(com[0].total),bit_count(in));
    //printf("%5d ,%5d,%5d,%8.1f,%8.1f,%5d%5d ,\r\n",bit_count(in),bscData.startKinji-bit_count(stop_cells),bit_count(stop_cells),res,com[0].total,KINJITI_MAX_WHIGHT,int(sum));
    if(combi_OK == 0 && Compulsion_no_combi_v_flg == false){
    //printf("A00 total:%d |flg:%d \r\n",bscData.startKinji,bit_count(in));
        if( ((checkBtnJunsaiteki == true) && (res == KINJITI_MAX_WHIGHT))) {
            v.output_audio(MSG_NO_JUNSAITEKI);
            v.wait_echo();
        }else if((int(com[0].total) == DAMMY_MAX) && (bit_count(in) >= (bscData.startKinji-bit_count(stop_cells)))){
            if((     bit_count(in) >= (product.head-bit_count(stop_cells))) //||   //全ての皿に物が載った
              /*((load.katamen > 0) && (bit_count(in) >= ((bscData.startKinji/2)-bit_count(stop_cells))))*/){
                //printf("A1 flg*%d  ",no_combi_v_flg);
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
                //printf("A2 flg*%d  ",no_combi_v_flg);
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

//最適重量組合せ結果表示関数(通常の組み合わせ)
void show_results_comzan(const combiData_t *com){
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
        //led.REGIS(com->kumi);
        for(int k=0; k < product.head; k++) {
            if((com->kumi & (1 << k)) > 0){
                iro[k] = C_BUI_G;
            }else{
                iro[k] = C_NULL;
            }
        }
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

        for(int k=0; k < product.head; k++) {
            if((com->kumi & (1 << k)) > 0){
                iro[k] = C_BUI_G;
            }else{
                iro[k] = C_NULL;
            }
        }

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
                if(load.kumi_flag == KUMI_COMZAN){}
                else{                se.Touch_02();}
            }
            if(load.kumi_flag == KUMI_COMZAN){
                for(int i = 0; i <= product.head ; i += 1) {
                    printf("%3d:%5d:%5d:%5d\r\n",i,com->kumi,1 << i,gOyaHead);
                    if((com->kumi & (1 << i)) > 0){
                        printf("OK\r\n");
                        if((gOyaHead & (1 << i)) > 0){
                            iro[i] = PENTA_G;
                        }else{
                            iro[i] = C_MARU;
                        }
                    }else{
                        iro[i] = C_NULL;
                    }
                }
            }else{
                //led.REGIS(com->kumi);
                for(int k=0; k < product.head; k++) {
                    if((com->kumi & (1 << k)) > 0){
                        iro[k] = C_BUI_G;
                    }else{
                        iro[k] = C_NULL;
                    }
                }
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
        }
        //設定時間LED表示(ハイパーモード時のみ)
        else if(load.spMode == SPEED_HYPER){
            if(option.kokuti==1||option.kokuti==2||option.kokuti==3){
                se.Touch_02();
            }
            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                //led.REGIS3(com->kumi);
                led.REGIS(com->kumi);
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
                        //led.REGIS3(resultCombi.kumi);
                        led.REGIS(resultCombi.kumi);
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
                //led.REGIS(com->kumi);
                        
                for(int k=0; k < product.head; k++) {
                    if((com->kumi & (1 << k)) > 0){
                        iro[k] = C_BUI_G;
                    }else{
                        iro[k] = C_NULL;
                    }
                }
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



//ゆっくりモード時の処理を行う
void driveSlowly_comzan(combiData_t *com)
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
    //show_results(com);     //組み合わせに応じたLED点灯,総重量表示
    for(int k=0; k < product.head; k++) {
        if((com->kumi & (1 << k)) > 0){
            iro[k] = C_BUI_G;
        }else{
            iro[k] = C_NULL;
        }
    }
    comp_kumi = com->kumi;
    while(com->kumi) {
        missPoint = 0;  //初期化
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            comp_kumi = com->kumi;
        } else {
            //led.REGIS(com->kumi);
            for(int k=0; k < product.head; k++) {
                if((com->kumi & (1 << k)) > 0){
                    iro[k] = C_BUI_G;
                }else{
                    iro[k] = C_NULL;
                }
            }
        }
        //******* 川4/17追加  ディスプレイ表示更新
        disp_wait = disp_wait+1;
        if(disp_wait==5) {
            if((int(com->total) < res)) {
                lcd.settings(&load);            //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
            }
            //load.secondOnOff=0;
            //show_results(com);     //組み合わせに応じたLED点灯,総重量表示
            
            for(int k=0; k < product.head; k++) {
                if((com->kumi & (1 << k)) > 0){
                    iro[k] = C_BUI_G;
                }else{
                    iro[k] = C_NULL;
                }
            }
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
            if(first_chk_flg == 0 && (bit_count(inNow)>= (bscData.startKinji-bit_count(stop_cells))) && re_meas2nd==0) { //オプションでON/OFF出来る様になったため、準最適値でのON/OFFは関係なくなったので消去
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
                    //show_results(com);     //組み合わせに応じたLED点灯,総重量表示
                            
                    for(int k=0; k < product.head; k++) {
                        if((com->kumi & (1 << k)) > 0){
                            iro[k] = C_BUI_G;
                        }else{
                            iro[k] = C_NULL;
                        }
                    }
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
            //led.REGIS(com->kumi);
            for(int k=0; k < product.head; k++) {
                if((com->kumi & (1 << k)) > 0){
                    iro[k] = C_BUI_G;
                }else{
                    iro[k] = C_NULL;
                }
            }
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
                if(total_kumi_cnt>0) {
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
