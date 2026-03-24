void drive_pack();
void lcd_set();
int adjust_combi(int adjust_combi_yet, int nowcombi);


void lcd_set(){
    const char pack_count[]={0xC4,0xB3,0xC6,0xAD,0xB3,' ' ,0xBE,0xB2,0xB9,0xDE,0xDD,0x00};      //ﾄｳﾆｭｳ ｾｲｹﾞ
    if(load.kumi_flag == KUMI_PACK){
        lcd.settings(&load);      //設定内容を表示
    }else{
        lcd.blank(0);
        lcd.printf(pack_count);
        lcd.locate(12,0);
        lcd.printf("%2d",load.kumi_min);
    }
}









void drive_pack(){
    const char pack_count[]={0xC4,0xB3,0xC6,0xAD,0xB3,' ' ,0xBE,0xB2,0xB9,0xDE,0xDD,0x00};      //ﾄｳﾆｭｳ ｾｲｹﾞﾝ
    char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
                   ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼ
    res = load.target + load.jougen;    //組み合わせ重量の上限値を取得
    cntData_t cnt= {0,0,0};                     //カウント機能の値保持用
    //combiData_t orderCombi[BK_CNT] = {0,0.0,0};   //組み合わせ候補のデータ(配列)
    //combiData_t resultCombi;                    //組み合わせ結果(最小値取得用)
    cnt.nowMode = load.spMode;       //処理モード取得
    int btnset =0;//計量中の下ボタン押しで出てくる値保存
    int SerialNumber = 0; //USBを連続で使用する際の連番値
    //int first_target = load.target;
    //int first_jougen = load.jougen;
    //int scond_target = load.taimenTarget;
    //int scond_jougen = load.taimenJougen;
    
    int now_combi = 0;   //今載せてるパック数
    int now_combi_yet = 0;   //今載せてる端数のパック数
    int Adjust_now_combi_yet = 0;  //ｾｲｹﾞﾝ超えて入れた場合の調整値を入れる変数
    float befour_weight = 0.0;
    float combi_weight =  0.0;
    
    no_combi_v_flg = false;
    //lcd.cls();
    
    /////loadに単重の計算値を入れる
    
    if(load.kumi_flag == KUMI_PACK){
        load.tanjuu = load.target/load.kumi_min;
    }else{
        load.tanjuu = load.target;
        load.kumi_min = int((load.target - 1)/load.jougen);
    }
    
    set_stopcell();
    //1行目LCD表示
    if(load.kumi_flag == KUMI_PACK){
        lcd.settings(&load);      //設定内容を表示
    }else{
        lcd.blank(0);
        lcd.printf(pack_count);
        lcd.locate(12,0);
        lcd.printf("%2d",load.kumi_min);
    }
    iro[0] = CR_00;


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

    //準最適値を行うヘッド数は、HEAD数より
    //大きい値はありえない(エラー対策)
    if(bscData.startKinji > product.head) {
        bscData.startKinji = product.head;
    }
    tm *dated4 = localtime(&seconds);
    if(product.limit_on==NUMBER_LIMIT) {
        set_count_force(&cnt);
    }
    coulap= 0;
    meas_rap.attach( &rap_count, 0.1);
    
    iro[0]=CR_00;
    
    while(1) {
        btn.new_keep_btn();         //スイッチ(New)取得
        //esc_swを押すと、Menu画面に戻る
        if(btn.esc_press()||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)) {
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
                if(product.use_usb==1 && option.usb_EN==true) {
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
                }
            }
            total_kumi_cnt = 0;
            average_kumi_value = 0;
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            return;
        }

        //上ボタンが離されたら、操作ロック解除
        if( btn.down_press() == true) {
            now_combi = 0;
            combi_weight =0.0;
            now_combi_yet=0;
            iro[0] = CR_00;
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            auto_zero_point();
            //1行目LCD表示
            lcd_set();      //設定内容を表示
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
            }
        */  
            select_keiryo_option(0, load.kumi_flag, 0);
        
            //1行目LCD表示
            lcd_set();      //設定内容を表示
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
              
        //重量確認(適正でない重量があるため処理を飛ばす)
        /*if(check_max_weight(cel.over_weight()) == false) {
            continue;
        }*/
        lcd.blank(1);
        if(load.kumi_flag == KUMI_PACK){
            lcd.locate(0,1);
            lcd.printf("%3d  ",now_combi + now_combi_yet);
            //lcd.printf("%3d  %6.0f",now_combi,cel.anteiWeight(0)-combi_weight);
            lcd.locate(6,1);
            lcd.printf("%6.0f",cel.anteiWeight(0)-combi_weight);
        }else{
            lcd.locate(0,1);
            lcd.printf("%3d  ",now_combi);
            lcd.locate(6,1);
            lcd.printf("%4.0f",combi_weight);
            lcd.printf("%4.0f",cel.anteiWeight(0));
        }
        //lcd.printf("%7.1f",cel.anteiWeight(0));
        if(now_combi + now_combi_yet > 0){
           iro[0] = CR_00 + now_combi + now_combi_yet;
        }
        
        if(load.kumi_flag == KUMI_PACK){
            if(cel.anteiWeight(0) > 0){
                befour_weight = cel.anteiWeight(0);
                if(now_combi > load.kumi_max - load.kumi_min && now_combi < load.kumi_max){
                    //printf("B0  %3d  %3d\r\n",now_combi,now_combi_yet);
                    //printf("B0 %5.1f %5.1f\r\n",cel.anteiWeight(0),combi_weight);
                    
                    if(combi_weight + float(load.target + load.jougen) < cel.anteiWeight(0)){    //目標重量外の場合
                        lcd.bigNumber(cel.anteiWeight(0));
                        se.mistake();                   //警告音
                        
                    }else if(cel.anteiWeight(0) > combi_weight  + float(load.tanjuu)){    //目標重量内には届かないが、一個分以上になっていたら
                        //iro[0] = CR_00 + int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                        //se.Touch_01();
                        
                        printf("AA"),
                        now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                        //printf("B1  %3d  %3d\r\n",now_combi,now_combi_yet);
                        //printf("B2  %3d  %3d\r\n",now_combi,now_combi_yet);
                        
                        iro[0] = CR_00 + now_combi + now_combi_yet;
                    }else if(combi_weight - cel.anteiWeight(0) > float(load.tanjuu)){
                        now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                        now_combi += now_combi_yet;
                        now_combi_yet = 0;
                        iro[0] = CR_00 + now_combi + now_combi_yet;
                        combi_weight = cel.anteiWeight(0);
                    }
                }else{
                    if(cel.anteiWeight(0) > combi_weight + float(load.target) && combi_weight + float(load.target + load.jougen)+0.9 > cel.anteiWeight(0)){    //目標重量内に入った場合
                        //se.Touch_01();
                        combi_weight = cel.anteiWeight(0);
                        iro[0] = CR_00 + now_combi + now_combi_yet;
                        now_combi += load.kumi_min;
                        now_combi_yet=0;
                        
                    }else if(combi_weight + float(load.target + load.jougen) < cel.anteiWeight(0)){    //目標重量外の場合
                        lcd.bigNumber(cel.anteiWeight(0));
                        se.mistake();                   //警告音
                        
                    }else if(cel.anteiWeight(0) > combi_weight  + float(load.tanjuu)){    //目標重量内には届かないが、一個分以上になっていたら
                        //iro[0] = CR_00 + int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                        //se.Touch_01();
                        now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                        printf("A1  %3d  %3d\r\n",now_combi,now_combi_yet);
                        now_combi += now_combi_yet;
                        printf("A2  %3d  %3d\r\n",now_combi,now_combi_yet);
                        now_combi_yet = 0;
                        iro[0] = CR_00 + now_combi + now_combi_yet;
                        combi_weight = cel.anteiWeight(0);
                    }else if(combi_weight - cel.anteiWeight(0) > float(load.tanjuu)){
                        now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                        now_combi += now_combi_yet;
                        now_combi_yet = 0;
                        iro[0] = CR_00 + now_combi + now_combi_yet;
                        combi_weight = cel.anteiWeight(0);
                    }
                    
                }
                
                
            }else if(nowweightVale3[0] > param.CUT){
                
                
                //iro[0] = C_NUSE;
            }else if(nowweightVale3[0] <= param.CUT){
                if(now_combi > 0 && now_combi != load.kumi_max){
                    se.mistake();
                    iro[0] = C_BATU;
                }else{
                    now_combi = 0;
                    combi_weight =0.0;
                    now_combi_yet=0;
                    iro[0] = C_NUSE;
                }
            }
            
            if((now_combi_yet + now_combi) == load.kumi_max){       //最大ﾊﾟｯｸ数と同じ　→　リセット作業 
                now_combi = 0;
                combi_weight =0.0;
                now_combi_yet=0;
                iro[0] = C_NUSE;
            }else{     
            
            }
        }else if(load.kumi_flag == KUMI_PACK2){ //計量方法が2だったら
            if(cel.anteiWeight(0) < param.CUT){ //CUT以上(何か乗っていた
                if(nowweightVale3[0] <= param.CUT){
                    now_combi = 0;
                    combi_weight =0.0;
                    now_combi_yet=0;
                }
            }
            if(now_combi == 0){
                iro[0] = CR_00;
            }
                
            if(cel.anteiWeight(0) > param.CUT){ //CUT以上(何か乗っていた
                if(combi_weight - int(load.tanjuu/10) <= cel.anteiWeight(0) && cel.anteiWeight(0) <= combi_weight + int(load.tanjuu/10)){
                    combi_weight = cel.anteiWeight(0);
                    iro[0] = CR_00 + now_combi;
                }else if(-load.target * 9/10 <(cel.anteiWeight(0) - combi_weight) && (cel.anteiWeight(0) - combi_weight) < (load.target * 9/10)){ //前回のと比べて、１パックの半分よりは軽い程度の重量変化がおこtgった
                    //combi_weight = cel.anteiWeight(0);
                    iro[0] = C_QUES;
                }else if(-(load.target * (load.kumi_min +1)) <=(cel.anteiWeight(0) - combi_weight) && (cel.anteiWeight(0) - combi_weight)<-load.target*9/10 ){
                    now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                    if(-1 >= now_combi_yet){
                        printf("A1 :%2d :%2d\r\n",now_combi,now_combi_yet);
                        now_combi += now_combi_yet;
                        now_combi_yet = 0;
                        iro[0] = CR_00 + now_combi;
                        //combi_weight = load.target * now_combi;
                        combi_weight = cel.anteiWeight(0);
                    }
                //}else if(-param.BARATSUKI < (cel.anteiWeight(0) - combi_weight) && (cel.anteiWeight(0) - combi_weight) < param.BARATSUKI){
                //    combi_weight = cel.anteiWeight(0);
               //     printf("B1\r\n");
                //}else if(-load.target/4 < (cel.anteiWeight(0) - combi_weight) && (cel.anteiWeight(0) - combi_weight) < (load.target/4)){ //前回のと比べて、１パックの半分よりは軽い程度の重量変化がおこtgった
                    //combi_weight = cel.anteiWeight(0);
                //}else if(){   //前回と比べて、１パックの半分以上(且つ１個未満)の重量が載った　→どうすんべ？エラーだべか?
                //    iro[0] = C_QUES;
                }else if(((cel.anteiWeight(0) - combi_weight) >= load.target * 9/10) && (cel.anteiWeight(0) - combi_weight) <= (load.target * (load.kumi_min +1)) ){ ////計測後の値が、計測前分減らして更に、１パック分より重く、投入制限以内
                    now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                    if(now_combi_yet >= 1){
                        printf("A2 :%2d :%2d\r\n",now_combi,now_combi_yet);
                        now_combi += now_combi_yet;
                        now_combi_yet = 0;
                        iro[0] = CR_00 + now_combi;
                        //combi_weight = load.target * now_combi;
                        combi_weight = cel.anteiWeight(0);
                    }
                }else if((cel.anteiWeight(0) - combi_weight) > (load.target * (load.kumi_min +1))){////計測後の値が、計測前分減らして更に、投入制限より重い

                    se.mistake();                   //警告音
                    //se.mistake();                   //警告音
                    now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                    //**************************************
                    Adjust_now_combi_yet = adjust_combi(now_combi_yet,now_combi);
                    //オーバー分を調整するプログラム
                    //*************************************
                    if(Adjust_now_combi_yet != 0){
                        now_combi += Adjust_now_combi_yet;
                        now_combi_yet = 0;
                        Adjust_now_combi_yet = 0;
                        iro[0] = CR_00 + now_combi;
                        setHeadVolt(0, param.KURI);
                        setHeadVolt(0, param.KURI);
                        setHeadVolt(0, param.KURI);
                        setHeadVolt(0, param.KURI);
                        setHeadVolt(0, param.KURI);
                        setHeadVolt(0, param.KURI);
                        combi_weight = cel.anteiWeight(0);
                    }
                }else if((cel.anteiWeight(0) - combi_weight) < -(load.target * (load.kumi_min +1))){////計測後の値が、計測前分減らして更に、投入制限より軽い

                    se.mistake();                   //警告音
                   //se.mistake();                   //警告音
                    now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                    //**************************************
                    Adjust_now_combi_yet = adjust_combi(now_combi_yet,now_combi);
                    //オーバー分を調整するプログラム
                    //*************************************
                    if(Adjust_now_combi_yet != 0){
                        now_combi += Adjust_now_combi_yet;
                        now_combi_yet = 0;
                        Adjust_now_combi_yet = 0;
                        iro[0] = CR_00 + now_combi;
                        setHeadVolt(0, param.KURI);
                        setHeadVolt(0, param.KURI);
                        setHeadVolt(0, param.KURI);
                        setHeadVolt(0, param.KURI);
                        setHeadVolt(0, param.KURI);
                        setHeadVolt(0, param.KURI);
                        combi_weight = cel.anteiWeight(0);
                    }
                }
            }
        }
        
        
        
        //1行目LCD表示
        lcd_set();      //設定内容を表示(大文字表示で内容が非表示となるため再表示する)
        
        //printf("combi=%3d\r\n",now_combi);
        if(load.kumi_flag == KUMI_PACK){ //計量方法が2だったら
            if(now_combi + now_combi_yet == load.kumi_max){
                while(nowweightVale3[0] >= param.CUT){
                        //iro[0] = C_MARU;
                        //lcd.bigNumber(now_combi + now_combi_yet);
                        //se.ck07();
                        //wait(2);
                    setHeadVolt(0, param.KURI);
                    if((nowweightVale[0] != 0 && ((nowweightVale[0] -nowweightVale3[0]) <= -0.3)) || ((nowweightVale[0] -nowweightVale3[0]) >= 0.3)) {
                        nowweightVale3[0] = nowweightVale[0];
                    }
                        now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                        iro[0] = CR_00 + now_combi + now_combi_yet;
                    if(cel.anteiWeight(0) > combi_weight + ((float(load.target)/load.kumi_min)*(load.kumi_max - now_combi)) && combi_weight + ((float(load.target + load.jougen)/load.kumi_min)*(load.kumi_max - now_combi))+0.9 > cel.anteiWeight(0)){    //目標重量内に入っ
                        iro[0] = C_MARU;
                        lcd.bigNumber(now_combi + now_combi_yet);
                        lcd.blank(0);
                        lcd.locate(0,1);
                        lcd.printf("%3dpcs  %6.0fg   ",now_combi + now_combi_yet,cel.anteiWeight(0));
                        printf("%3dpcs  %6.0fg   ",now_combi + now_combi_yet,cel.anteiWeight(0));
                        se.ck07();
                        wait(2);
                    }
                    else if(combi_weight + ((float(load.target + load.jougen)/load.kumi_min)*(load.kumi_max - now_combi)) < cel.anteiWeight(0)){    //目標重量以上入った場合
                    
                        now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                        
                        iro[0] = CR_00 + now_combi + now_combi_yet;
                        lcd.cls();
                        lcd.bigNumber(cel.anteiWeight(0));
                        se.mistake();                   //警告音
                    }
                }
                now_combi = 0;
                now_combi_yet = 0;
                combi_weight =0.0;
            }else if(now_combi + now_combi_yet >= load.kumi_max){
                while(now_combi + now_combi_yet >= load.kumi_max){    //目標重量内に入ってない間
                    setHeadVolt(0, param.KURI);
                    if((nowweightVale[0] != 0 && ((nowweightVale[0] -nowweightVale3[0]) <= -0.3)) || ((nowweightVale[0] -nowweightVale3[0]) >= 0.3)) {
                        nowweightVale3[0] = nowweightVale[0];
                    }
                    now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                    iro[0] = CR_00 + now_combi + now_combi_yet;
                    lcd.bigNumber(cel.anteiWeight(0));
                    se.mistake();                   //警告音
                }
                
            }
        }else if(load.kumi_flag == KUMI_PACK2){ //計量方法が2だったら
        //if(nowweightVale[0] >= param.CUT){
            if(now_combi == load.kumi_max){
                while(nowweightVale[0] >= param.CUT){
                    if(now_combi == load.kumi_max && cel.anteiWeight(0) > 0){
                        iro[0] = C_MARU;
                        //lcd.bigNumber(now_combi + now_combi_yet);
                        lcd.blank(0);
                        lcd.locate(0,1);
                        lcd.printf("ok%3dpcs %6.0fg",now_combi,cel.anteiWeight(0));
                        //printf("%3dpcs  %6.0fg   ",now_combi,cel.anteiWeight(0));
                        se.ck07();
                        wait(0.5);
                    }
                    setHeadVolt(0, param.KURI);
                    if(cel.anteiWeight(0)>param.CUT){
                        now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                        if(now_combi_yet <= -1 || now_combi_yet >= 1){
                            printf("ng%3d:%3d:%3d\r\n",now_combi,now_combi_yet,0);
                            now_combi += now_combi_yet;
                            now_combi_yet = 0;
                            combi_weight = cel.anteiWeight(0);
                        }
                        if(int(now_combi + now_combi_yet) == load.kumi_max){
                        }else{
                            iro[0] = CR_00 + now_combi;
                            lcd.cls();
                            //combi_weight = cel.anteiWeight(0);
                            lcd.bigNumber(cel.anteiWeight(0));
                            printf("%3d:%3d:%3d\r\n",now_combi,now_combi_yet,0);
                            se.mistake();                   //警告音
                        }
                    }
                }
                if(nowweightVale[0] < param.CUT){
                    now_combi = 0;
                    now_combi_yet = 0;
                    combi_weight =0.0;
                }
            }else if(now_combi > load.kumi_max){
              //  while(now_combi > load.kumi_max){    //目標重量内に入ってない間
                    //setHeadVolt(0, param.KURI);
                    //if((nowweightVale[0] != 0 && ((nowweightVale[0] -nowweightVale3[0]) <= -0.3)) || ((nowweightVale[0] -nowweightVale3[0]) >= 0.3)) {
                    //    nowweightVale3[0] = nowweightVale[0];
                    //}
                    ///now_combi_yet = int((cel.anteiWeight(0) - combi_weight)/float(load.tanjuu));
                    iro[0] = CR_00 + now_combi + now_combi_yet;
                    lcd.bigNumber(cel.anteiWeight(0));
                    se.mistake();                   //警告音
                    //se.Touch_01();
               // }
                
            }
        }
      
       
    }//while
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
}

int adjust_combi(int adjust_combi_yet, int nowcombi){
    const char kettei[]={0xB9,0xAF,0xC3,0xB2,0x00};      //ｹｯﾃｲ
    //int max = 0;
    //int min = 0;
    iro[0] = C_QUES;
    lcd.bigNumber(adjust_combi_yet+nowcombi);
    /*
    if(adjust_combi_yet > 0){
        max = adjust_combi_yet;
        min = 1;
    }else if(adjust_combi_yet < 0){
        max = -1;
        min = adjust_combi_yet;
    }else{
        return 0;
    }*/
    
    
    while(1) {
        btn.new_keep_btn();         //スイッチ(New)取得
        
        if(btn.any_press()==true){
            if( btn.ent_press() == true) {
                se.Touch_01();
                while(btn.any_press()==true){wait(0.2);}
                return adjust_combi_yet;
            }
            if( btn.esc_press() == true) {
                se.Touch_01();
                while(btn.any_press()==true){wait(0.2);}
                adjust_combi_yet = 0;
                return adjust_combi_yet;
            }
            if( btn.down_press() == true) {
                se.Touch_01();
                adjust_combi_yet += 1;
            }
            if( btn.up_press() == true) {
                se.Touch_01();
                adjust_combi_yet -= 1;
            }
            if(adjust_combi_yet + nowcombi < 0){
                adjust_combi_yet = -nowcombi;
            }
            
            //if(adjust_combi_yet > max)     {adjust_combi_yet = max;}
            //else if(adjust_combi_yet < min){adjust_combi_yet = min;}
            
            lcd.bigNumber(adjust_combi_yet+nowcombi);
            lcd.locate(0,1);
            lcd.printf("OK:");
            lcd.printf(kettei);
                wait(0.2);
        }
    
    
    
        btn.old_btn();       //スイッチ更新
    }
}




