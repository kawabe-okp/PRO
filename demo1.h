int Chenge_vaule_btnD_demo(int option){
    const char kokuti[]={0xBB,0xB7,0xDE,0xAE,0xB3,' ' ,0xBE,0xDD,0xC0,0xB8,0x00};      //ｻｷﾞｮｳ ｾﾝﾀｸ

    const char button_k[4][SIZE]={{'L','E','D',0xC3,0xDD,0xC4,0xB3,' ',0xBE,0xAF,0xC3,0xB2,0x00}    //LEDﾃﾝﾄｳ ｾｯﾃｲ
                                 ,{0xB9,0xB2,0xD8,0xAE,0xB3,' ' ,0xD3,0xB0,0xC4,0xDE,0x00}            //ｹｲﾘｮｳ ﾓｰﾄﾞ
                                 ,{0xD3,0xB0,0xC4,0xDE,' ' ,0xCD,0xDD,0xBA,0xB3,0x00}                 //ﾓｰﾄﾞ ﾍﾝｺｳ
                                 ,{0xBE,0xB2,0xB9,0xDE,0xDD,' ' ,0xCD,0xDD,0xBA,0xB3,0x00}      //ｾｲｹﾞﾝ ﾍﾝｺｳ
                                };
    const char button_y[4][SIZE]={{0xB6,0xB3,0xDD,0xC4,' ' ,0xBE,0xAF,0xC3,0xB2,0x00}     //ｶｳﾝﾄ ｾｯﾃｲ
                                 ,{0xB9,0xB2,0xD8,0xAE,0xB3,' ' ,0xD3,0xB0,0xC4,0xDE,0x00}     //ｹｲﾘｮｳ ﾓｰﾄﾞ
                                 ,{0xD3,0xB0,0xC4,0xDE,' ' ,0xCD,0xDD,0xBA,0xB3,0x00}                 //ﾓｰﾄﾞ ﾍﾝｺｳ
                                 ,{0xBE,0xB2,0xB9,0xDE,0xDD,' ' ,0xCD,0xDD,0xBA,0xB3,0x00}      //ｾｲｹﾞﾝ ﾍﾝｺｳ
                               }; 
    data_t limit;
    int num = 0;
    lcd.cls();                  //表示をクリア
    btn.ini();              //保持ボタンを初期化
    limit.min = 0;
    if(load.spMode == SPEED_LOW) {
        limit.max = 2;
    }else{
        limit.max = 3;
    }
    limit.lcd = 0;
    num=0;
    if(option == 1) {
        num=1;
    }
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
            if(load.spMode == SPEED_LOW) {
                lcd.printf(button_y[num]);
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
void driveStart_demo()
{
    int looper=0;
    char multiweight[SIZE] = {0xD7,0xDD,0xB8,0xCD,0xDE,0xC2,' ' ,0xB8,0xD0,0xB1,0xDC,0xBE,0x00};
    const char saving[]= {0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,0x00}; //ﾎｿﾞﾝﾁｭｳ
    int secondCnt = 0;                        //一定数に達したら準最適値処理開始
    int *stay_cnt = new int[product.head];      //滞留警告判断のためのカウント変数
    //float res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    cntData_t cnt= {0,0,0};                     //カウント機能の値保持用
    combiData_t orderCombi[BK_CNT] = {0,0.0,0};   //組み合わせ候補のデータ(配列)
    combiData_t resultCombi;                    //組み合わせ結果(最小値取得用)
    cnt.nowMode = load.spMode;       //処理モード取得

    int btnset =0;//計量中の下ボタン押しで出てくる値保存
    //1行目LCD表示
    if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
        lcd.locate(0,0);
        lcd.printf(multiweight);
        lcd.blank(LOW);
        lcd.printf(":----.-g");
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
    run_date.tm_year = dated3->tm_year;
    run_date.tm_sec  = dated3->tm_sec;
    run_date.tm_min  = dated3->tm_min;
    run_date.tm_hour = dated3->tm_hour;
    run_date.tm_mday = dated3->tm_mday;
    run_date.tm_mon  = dated3->tm_mon;
    run_date.tm_year = dated3->tm_year;
    //
    //printf("st%d:%d:%d\r\n",run_date.tm_hour,run_date.tm_min,run_date.tm_sec);

    for(int l; l<product.head; l++) {
        nowweightVale3[l]=0.0;
    }
    v.demo_01();

    //準最適値を行うヘッド数は、HEAD数より
    //大きい値はありえない(エラー対策)
    if(bscData.startKinji > product.head) {
        bscData.startKinji = product.head;
    }
    tm *dated4 = localtime(&seconds);
    if(product.limit_on==NUMBER_LIMIT) {
        set_count_force(&cnt);
    }
    int demo_endflg=0;
    int bklooper=0;
    int entyo_sec = dated3->tm_sec;
    while(1) {
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
        } else {
            led.ledAllOff();            // LED消灯
        }
        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            for(int l=0;l<product.head;l++){
                if(iro[l]==C_NULL){
                    iro[l]=C_NUSE;
                }
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

        seconds = time(NULL);
        tm *dated5 = localtime(&seconds);
        if(bklooper!=looper){
            entyo_sec = dated5->tm_sec;
        }
        bklooper=looper;
        if(entyo_sec >= 29 && dated5->tm_sec < 29){
            if(entyo_sec + 30 < dated5->tm_sec+60 || entyo_sec > dated5->tm_sec+60){
                demo_endflg=1;
            }
        }else{
            if(entyo_sec + 30 < dated5->tm_sec || entyo_sec > dated5->tm_sec){
                demo_endflg=1;
            }
        }
        //esc_swを押すと、Menu画面に戻る
        if(btn.esc() || looper>5 || demo_endflg==1 || btn.get_btn() == BTN_UP) {
            se.Touch_01();
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
                if(option.usb_EN==true) {
                    lcd.printf(saving);
                }/*
                file.save_SAGYOU(ji,hun,file.get_preset()+1,load.target,total_kumi_cnt,average_kumi_value);
                //save_sagyou_file(total_kumi_cnt,average_kumi_value,load.target,load.jougen);
                save_sagyou_file2(total_kumi_cnt,average_kumi_value,load.target,load.jougen,ji,hun); 
                save_sagyou_file3(total_kumi_cnt,average_kumi_value,load.target,load.jougen,ji,hun); */ 
                SD_initialise();
                lcd.blank(1);
                lcd.blank(0);
            }
            delete[] stay_cnt;
            total_kumi_cnt = 0;
            average_kumi_value = 0;

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
            if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                lcd.locate(0,0);
                lcd.printf(multiweight);
            } else {
                lcd.settings(&load);      //設定内容を表示
            }            
        }//ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        else if(btn.up_press() == true) {
            se.chime_01();
            //ゆっくりモード時には、
            if(load.spMode == SPEED_LOW) {
                btnset = Chenge_vaule_btnD_demo(option.btn_sett);//ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
                /*if(btnset==0) {
                    set_count(&cnt);
                } else*/ if(btnset==0) {
                    auto_zero_off = true;
                    start_keiryo(0);
                    auto_zero_off = false;
                }else if(btnset==1){
                    set_preset2(file.get_preset(),TL_SPMODE);
                }else if(btnset==2){
                    set_preset2(file.get_preset(),TL_KUMI_FLAG);
                }
            } else if(load.spMode == SPEED_HIGH || load.spMode == SPEED_HYPER) {
                btnset = Chenge_vaule_btnD_demo(0);
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

        btn.old_btn();       //スイッチ更新

        for(int i = 0; i < product.head; i++) {
            setHeadVolt(i, param.KURI);

            if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
                if((nowweightVale[i] != 0 && ((nowweightVale[i] -nowweightVale3[i]) <= -0.3)) || ((nowweightVale[i] -nowweightVale3[i]) >= 0.3)) {
                    nowweightVale3[i] = nowweightVale[i];
                }
                if(nowweightVale3[i] >= param.CUT && nowweightVale3[i] < mweight.u02.rank_min) {
                    iro[i] = C_BATU;
                } else if(nowweightVale3[i] >= mweight.u02.rank_min && nowweightVale3[i] < mweight.u02.rank_max) {
                    iro[i] = C_S;
                } else if(nowweightVale3[i] >= mweight.u03.rank_min && nowweightVale3[i] < mweight.u03.rank_max) {
                    iro[i] = C_M;
                } else if(nowweightVale3[i] >= mweight.u04.rank_min && nowweightVale3[i] < mweight.u04.rank_max) {
                    iro[i] = C_L;
                } else if(nowweightVale3[i] >= mweight.u04.rank_max+1) {
                    iro[i] = C_BATU;
                } else {
                    iro[i] = C_NUSE;
                }
            }
            /*
                        //デバッグ用ここから
                        if( product.productType >= STANDARD_D || product.productType == JUNIOR_D || product.productType == BIG_D){
                            //temp = cel.temperature();   //温度を取得する
                            //for(int i = 0; i < product.head; i++){
                                //cel.set_head(i);
                                pc.printf("%d %6.3f %6.3f   %0.8f\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.getAin());
                            //}
                            if(i == product.head - 1){
                            //printf("%d %f C\r\n",14, temp);

                            pc.printf("========================\r\n");
                            pc.printf("========================\r\n");
                            pc.printf("========================\r\n");
                            }
                        }
                        //デバッグ用ここまで
                    */
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
        //1行目LCD表示
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            lcd.locate(0,0);
            lcd.printf(multiweight);
            lcd.blank(LOW);
            lcd.printf(":----.-g");
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
        if(load.kumi_flag == KUMI_KAIKYU) {
            //if(product.productType == MATRIX || product.productType == MATRIX_D || product.productType == ITIGO || product.productType == ITIGO_D){
            get_combi_location_multi(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
        } else {
            get_combi_location(orderCombi, &secondCnt);     //組み合わせ確認,組み合わせ候補を抽出
        }
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
            }
        }
        //1行目LCD表示
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            lcd.locate(0,0);
            lcd.printf(multiweight);
            lcd.blank(LOW);
            lcd.printf(":----.-g");
        } else {
            lcd.settings(&load);      //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
            if((product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)&& resultCombi.kumi > 0) {
                for(int l=0;l<product.head;l++){
                    //iro[l]=C_NULL;
                }
            }
        }
        
        show_results(&resultCombi);     //組み合わせに応じたLED点灯,総重量表示
        show_count(&cnt);               //カウント表示
        if(resultCombi.kumi > 0){
            looper+=1;
        }
        if(load.spMode == SPEED_HIGH) {
            announceRetention(resultCombi.kumi, stay_cnt);   //滞留アナウンス
        }


        //ゆっくりモードかつ組み合わせ成立していれば
        if( (load.spMode >= SPEED_LOW && resultCombi.kumi > 0) ||
                (int(resultCombi.total) > res && btn.get_lock_flg() == true && resultCombi.kumi > 0) ) {
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
                    lcd.settings(&load);      //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
                }
            }
            //printf("AA\r\n");
            if(option.kokuti==1||option.kokuti==2) {
                se.Touch_02();
            }
            int demo_weight = int(resultCombi.total);
            if(looper ==1){
                v.demo_02(demo_weight,load.target,load.jougen);
            }
            driveSlowly(&resultCombi);
            if(product.limit_on==NUMBER_LIMIT && resultCombi.kumi == 0) {
                KUMI_now =KUMI_now +1;
                file.save_limit2(KUMI_now);
                if(KUMI_limit-KUMI_now>0 && KUMI_limit-KUMI_now < 10) {
                    v.last_limit(KUMI_limit-KUMI_now);
                }
            }
            if(resultCombi.kumi == 0){
// printf("01\r\n");
// printf("02\r\n");
                    v.demo_03(demo_weight);
// printf("03\r\n");
                    demo_weight = 0;
                }
            }
            //組み合わせ分を取り終わっていたら
            if(resultCombi.kumi == 0 && cnt.max > 0) {
                if(looper ==1){
                if(cnt.num < cnt.max) {
                    cnt.num++;
                    //残りカウント数に合わせて音声お知らせ
                    v.output_audio_Count(cnt.max,cnt.num);
                }
            }//if
        }//ifゆっくりモード終了
        //ﾗﾝｸﾍﾞﾂ組合せのみ1行目LCD表示
        if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)) {
            lcd.locate(0,0);
            lcd.printf(multiweight);
            lcd.blank(LOW);
            lcd.printf(":----.-g");
        }
        if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
            for(int l=0;l<product.head;l++){
                iro[l]=C_NUSE;
            }
        }
        if(looper ==1){
            load.spMode = SPEED_HIGH;
        }
    }//while
}

//ランク選別モードのみ使用する。
//各ヘッド毎に、目標重量を計測したらLEDを点滅させる
//ｺｳｿｸﾓｰﾄﾞ
void drive_Rank_Sorting3_demo()
{
    char saving[]= {0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,0x00}; //ﾎｿﾞﾝﾁｭｳ
    combiData_t resultCombi;        //組み合わせ結果(最小値取得用)
    mweight = file.get_lweight(24); //現在プリセット選択されているデータを取得
    float baratsuki=param.BARATSUKI;//一時保存フォルダ
    param.BARATSUKI=param.BARATSUKI_R;
    res   = float(mweight.u01.rank_max)*1.03;    //組み合わせ重量の上限値を取得
    res2  = float(mweight.u02.rank_max)*1.03;    //組み合わせ重量の上限値を取得
    res3  = float(mweight.u03.rank_max)*1.03;    //組み合わせ重量の上限値を取得
    res4  = float(mweight.u04.rank_max)*1.03;    //組み合わせ重量の上限値を取得
    res5  = float(mweight.u05.rank_max)*1.03;    //組み合わせ重量の上限値を取得
    res6  = float(mweight.u06.rank_max)*1.03;    //組み合わせ重量の上限値を取得
    res7  = float(mweight.u07.rank_max)*1.03;    //組み合わせ重量の上限値を取得
    res8  = float(mweight.u08.rank_max)*1.03;   //組み合わせ重量の上限値を取得
    rank1ave  = (float(mweight.u01.rank_max)+float(mweight.u02.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank2ave  = (float(mweight.u02.rank_max)+float(mweight.u03.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank3ave  = (float(mweight.u03.rank_max)+float(mweight.u04.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank4ave  = (float(mweight.u04.rank_max)+float(mweight.u05.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank5ave  = (float(mweight.u05.rank_max)+float(mweight.u06.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank6ave  = (float(mweight.u06.rank_max)+float(mweight.u07.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank7ave  = (float(mweight.u07.rank_max)+float(mweight.u08.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank1dif  = (float(mweight.u02.rank_max)+float(mweight.u01.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank2dif  = (float(mweight.u03.rank_max)+float(mweight.u02.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank3dif  = (float(mweight.u04.rank_max)+float(mweight.u03.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank4dif  = (float(mweight.u05.rank_max)+float(mweight.u04.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank5dif  = (float(mweight.u06.rank_max)+float(mweight.u05.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank6dif  = (float(mweight.u07.rank_max)+float(mweight.u06.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank7dif  = (float(mweight.u08.rank_max)+float(mweight.u07.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    int looper=0;

    lcd.settings_rank2(&load);            //設定内容を表示
    for(int i = 0; i < product.head; i++) {
        nowrank[i]=0;
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
    run_date.tm_year = dated3->tm_year;
    run_date.tm_sec  = dated3->tm_sec;
    run_date.tm_min  = dated3->tm_min;
    run_date.tm_hour = dated3->tm_hour;
    run_date.tm_mday = dated3->tm_mday;
    run_date.tm_mon  = dated3->tm_mon;
    run_date.tm_year = dated3->tm_year;
    wait(1);
    //
    int total_qty=0;
    //led.RedOn();
    int demo_endflg=0;
    int bklooper=0;
    int entyo_sec = dated3->tm_sec;
    while(1) {
        
        //led.ledAllOff();            // LED消灯
        //led.RedOff();
        btn.new_keep_btn();               //スイッチ(New)取得
        //esc_swを押すと、Menu画面に戻る
        int total_qty2=0;
        for(int i = 0; i < 10; i++) {
            total_qty2 = totalranker0103[i]+totalranker0406[i]+totalranker0709[i]+totalranker1012[i];
        }
        
        seconds = time(NULL);
        tm *dated5 = localtime(&seconds);
        if(bklooper!=total_qty2){
            entyo_sec = dated5->tm_sec;
            demo_endflg=0;
        }
        bklooper=total_qty2;
        if(entyo_sec >= 44 && dated5->tm_sec < 44){
            if(entyo_sec + 45 < dated5->tm_sec+60 || entyo_sec > dated5->tm_sec+60){
                demo_endflg=1;
            }
        }else{
            if(entyo_sec + 45 < dated5->tm_sec || entyo_sec > dated5->tm_sec){
                demo_endflg=1;
            }
        }
        if(btn.esc() || total_qty2>=10 || demo_endflg == 1 || btn.get_btn() == BTN_UP) {
            se.Touch_01();
            comp_rank=0;
            //LCD表示
            lcd.blank(0);
            lcd.blank(1);
            lcd.locate(3,1);
            if(option.usb_EN==true) {
                lcd.printf(saving);
            }
            //戻りきるまで時間が掛かるので、マトリクス表示を消す
            for(int i = 0; i < product.head; i++) {
                iro[i]=C_NULL;
            }
            //個別データ書き込み(1個以上あったら)
            total_qty=0;
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
                save_rankdata(mweight.Rsort,ji,hun);
                if(option.usb_EN==true) {
                    //USB_rankdata(mweight.Rsort,ji,hun);
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
            return;
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            auto_zero_point();
            lcd.settings_rank2(&load);            //設定内容を表示
        } 
        //ﾓｼﾞ回転ON/OFF変更
        else if(btn.get_btn() == BTN_DOWN ) {
            if(product.head >= 8){
                option.c_rotate =(option.c_rotate +1)%3;
            }else{
                option.c_rotate =(option.c_rotate +1)%2;
            }
        }

        btn.old_btn();       //スイッチ更新

        looper+=1;
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
int demo_change(){
    const char name[]={0xC4,0xB3,0xDB,0xB8,0xCD,0xDD,0xBA,0xB3,' ' ,0xBD,0xD9,0x3F,0x00};      //ﾄｳﾛｸﾍﾝｺｳ ｽﾙ?
    char ckOK[]= {0xBD,0xD9,' ',0x00};                         //ｽﾙ
    char ckNG[]= {0xBC,0xC5,0xB2,0x00};                        //ｼﾅｲ
    int change_y=0; //変更しない=1
    lcd.blank(0);
    lcd.locate(0,0);
    lcd.printf(name);
    lcd.blank(1);
    lcd.locate(0,1);
    lcd.printf(ckNG);
    btn.ini();
    while(btn.get_btn() != BTN_ENT) {
        btn.new_btn();               //スイッチ(New)取得
        if(btn.get_btn() == BTN_UP || btn.get_btn() == BTN_DOWN) {
            lcd.locate(0,1);
            if(change_y==0) {
                lcd.printf(ckOK);       //(ボタン押されてるので次の値)
                change_y =1;
            } else {
                lcd.printf(ckNG);
                change_y=0;
            }
            se.Touch_01();
        } else if(btn.get_btn() == BTN_ESC) {
            se.Touch_01();
            return 0;
        }
        btn.old_btn();              //スイッチ更新
        wait(0.05);
    }//while
    se.Touch_01();
    return change_y;
}
bool preset_use_demo(int i,int data[]) //項目をすべて書き出し、使用する項目はtrue、他はfalseとする
{
    if(i==0) { //kumi_flugの設定時は無条件にtrue
        return true;
    }
    switch(data[TL_KUMI_FLAG]) {
        case(KUMI_NONE):
            switch(i) {
                case(TL_KATAMEN_HYOUJI):    return false;
                case(TL_TARGET):            return true;
                case(TL_JOUGEN):            return true;
                case(TL_FUZOROI_BOUSHI):    return false;
                case(TL_SECOND_ON_OFF):     return false;
                case(TL_SPMODE):            return false;
                case(TL_HYOUJI):            return false;
                case(TL_RANK_ON_OFF):       return false;
                case(TL_RANK_MIN):          return false;
                case(TL_RANK_MAX):          return false;
                case(TL_COMBI_FLAG):
                case(TL_TANJU):
                case(TL_KUMI_MIN)://組合せ数量(最小)
                case(TL_KUMI_MAX)://組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1)://組合せ優先1
                case(TL_KUMI_PRIORITY_2)://組合せ優先2
                case(TL_TAIMEN_TARGET):
                case(TL_TAIMEN_JOUGEN):
                default:
                    return false;
            }
 /*       case(KUMI_COMBI):
            switch(i) {
                case(TL_COMBI_FLAG):        return true;
                case(TL_TARGET):            return true;
                case(TL_JOUGEN):            return true;
                case(TL_FUZOROI_BOUSHI):    return false;
                case(TL_SECOND_ON_OFF):     return false;
                case(TL_SPMODE):            return false;
                case(TL_HYOUJI):            return false;
                case(TL_RANK_ON_OFF):       return false;
                case(TL_RANK_MIN):          return false;
                case(TL_RANK_MAX):          return false;
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
            switch(i) {
                case(TL_KUMI_MIN):          return true;  //組合せ数量(最小)
                case(TL_KUMI_MAX):          return true;  //組合せ数量(最大)
                case(TL_KUMI_PRIORITY_1):                 //組合せ優先1
                    if(data[TL_KUMI_MIN] == data[TL_KUMI_MIN]) {
                        return false;
                    } else {
                        return true;
                    }
                case(TL_KUMI_PRIORITY_2):                           //組合せ優先2
                    if(data[TL_KUMI_PRIORITY_1]==99) {
                        return false;
                    } else {
                        return true;
                    }
                case(TL_KATAMEN_HYOUJI):
                    if(product.head<12) {
                        return false;   //ｶﾀﾒﾝﾋｮｳｼﾞ
                    } else {
                        return true;
                    }
                case(TL_TARGET):            return true;  //組合せ重量(min)
                case(TL_JOUGEN):            return true;  //組合せ重量(max)
                case(TL_FUZOROI_BOUSHI):    return true;  //不揃い防止
                case(TL_SECOND_ON_OFF):     return true;  //準最適値
                case(TL_SPMODE):            return true;  //高速・ﾕｯｸﾘ
                case(TL_HYOUJI):                                    //LED点灯時間
                    if(data[TL_SPMODE]==0) {
                        return true;
                    } else {
                        return false;
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
        case(KUMI_RANK):  //S+のﾗﾝｸｾﾝﾍﾞﾂは別プログラムなので放置(というか、ここに来ないはず)
            if(i> TL_KUMI_FLAG &&((data[TL_KUMI_FLAG] == KUMI_KAIKYU) ||
                                  (data[TL_KUMI_FLAG] == KUMI_RANK && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON || product.productType == ITIGO || product.productType == ITIGO_D)))) {
                if(i<=TL_KUMI_FLAG+1) { //FLAGを決定した次の項目でmultiに行くので、その分までtrueにする
                    return true;
                } else {
                    return false;
                }
            }
            switch(i) {
                case(TL_TARGET):            return true;  //ﾗﾝｸ内重量(min)
                case(TL_JOUGEN):            return true;  //ﾗﾝｸ内重量(max)
                case(TL_COMBI_FLAG):
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
                case(TL_CHECK_HYOUUJI):     return true;
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
void set_preset_demo(int point)
{
    int headPosition = 24;
    const int dammy = 20;   //関数切り替えに利用するダミーデータ
    data_t limitData;
    load = file.get_load(headPosition); //現在プリセット選択されているデータを取得

    int data[] = {load.kumi_flag   ,load.kumi_min    ,load.kumi_max    ,load.kumi_priolity_1    ,load.kumi_priolity_2
                  ,load.combiflug  , load.katamen    ,load.tanjuu      ,load.target
                  ,load.jougen     ,load.taimenTarget,load.taimenJougen,load.fuzoroiBoushi
                  ,load.secondOnOff,load.spMode      ,load.lightTime   ,load.rankOnOff
                  ,load.rankMin    ,load.rankMak     ,load.check_hyouji
                 };
    //目標設定を行う
    //for(int i = 0; i < TL_END; i++){
    int i = 0;
    while( i < TL_END) {
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
        //printf("a0\r\n");
        if(preset_use_demo(i,data)==true) { //項目を使用するか否か判定する部分
        //printf("%d\r\n",i);
                       //printf("a1\r\n");
            limitData = file.get_limit_data(i); //制限値でデータ取得
            if(i==TL_KUMI_FLAG) {
                kumi_flug=true; //今登録中の値(KUMI_FLUG判定用)
            }else{
                kumi_flug=false; //今登録中の値(KUMI_FLUG判定用)
            }
            
            //複数重量範囲がある案件についてはこのプログラム内だけで行う
            //if(i> TL_KUMI_FLAG &&(data[TL_KUMI_FLAG] == KUMI_KAIKYU || data[TL_KUMI_FLAG] == mitei2 || data[TL_KUMI_FLAG] == mitei3)){
            if(i> TL_KUMI_FLAG &&((data[TL_KUMI_FLAG] == KUMI_KAIKYU) ||
                                  (data[TL_KUMI_FLAG] == KUMI_RANK && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON || product.productType == ITIGO || product.productType == ITIGO_D)))) {
                //printf("aa\r\n");
    
                if(multi_weight_set2(headPosition,data[TL_KUMI_FLAG])==0) {
                    i = TL_END;
                    continue;
                } else {
                    if(product.limit_on==SELECTOR_LIMIT) {
                        return;
                    } else {
                        i = -1;
                        continue;
                    }
                }
            } else {
                if(i == TL_SPMODE && product.limit_on==NUMBER_LIMIT) { //回数制限の場合はﾕｯｸﾘﾓｰﾄﾞのみ
                    data[TL_SPMODE] = 1;
                    i += 1;
                    continue;
                } else {
                    lcd.setTitle(i);
                    wait(0.1);
                }
            }
            //dammy データだったら目標設定項目(最大・最小の設定値反映等)
            if(limitData.lcd == dammy) {
                       //printf("b1\r\n");
                if(i == TL_KATAMEN_HYOUJI){
                  return;
                }else if(i == TL_KUMI_FLAG) {
                    if(point==0) {
                        data[TL_KUMI_FLAG] = KUMI_NONE;
                        data[TL_KATAMEN_HYOUJI]        = 0;
                        i += 2;
                        continue;
                    } else if(point==1) {
                       //printf("b3\r\n");
                        data[TL_KUMI_FLAG] = KUMI_RANK;
                        i += 1;
                        continue;
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
                }
                
                
                //優先組み合わせ方法のとき設定範囲(最大・最小値)を変更
                else if(i == TL_KUMI_PRIORITY_1 || i == TL_KUMI_PRIORITY_2) {
                    limitData.min = data[TL_KUMI_MIN];
                    limitData.max = data[TL_KUMI_MAX] +1;
                    //ﾕｳｾﾝｸﾐｱﾜｾで決定した値が 99(OFF)だった時に値を変更できる値へ一時的に戻す
                    if(data[i] == 99) {
                        data[i] = limitData.max;
                    }
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
                lcd.locate(7,1);

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
                    limitData.min = data[TL_TANJU]+5;
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

                //LED点灯時間のみ小数点となるため、型変換を行う。
                if(i == TL_HYOUJI) {
                    float sec = float(data[i]) / float(10); //(例)1 → 0.1へ
                    Chenge_vaule( &sec, limitData);
                    data[i] = sec * float(10);  //データ型をint型へ戻す(例)0.1 → 1へ

                    //邪道なエラー対策だが0値だとLEDが光らないので必ず1以上とする
                    if(data[i] == 0) {
                        data[i] = 1;
                    }
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
        if(btn.get_btn() == BTN_ENT) {
            i+=1;
        } else if(btn.get_btn() == BTN_ESC) {
            i-=1;
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
    //wait(1);
    lcd.blank(0);
    lcd.blank(1);
}

void demo_mode(){
    if(demo_change()==1){
        set_preset_demo(0);
        set_preset_demo(1);  
    }
    if(btn.get_btn() == BTN_ESC) {
            se.Touch_01();
            return;
    }
    file.set_preset(24);
    load = file.get_load(24); //現在プリセット選択されているデータを取得
    cel.set_now_loadcell_data(&load);   //現在選択されているプリセット(ロードセル)をset
    katamen=load.katamen; //グローバル変数へ値保存
    set_stopcell();   //不使用セル(＋12Headの対面点灯を組み合わせる)
    v.demo_00();
    auto_zero_point();                  //ゼロ点取得
    SD_initialise();
    v.wait_echo();
    while(1) {
        //esc_swを押すと、Menu画面に戻る
        if(btn.esc()) {
            return;
        }
        driveStart_demo();
        if(btn.esc()) {
            return;
        }
        v.demo_04();
        drive_Rank_Sorting3_demo();
    }
    //return;
    
    
    
}