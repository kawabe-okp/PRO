bool servo_connection=false;

void servo_off_wait();
void servo_set();
bool servo_setting1();
bool servo_setting2();
bool servo_setting3();
bool servo_setting4();
void servo_test();
bool servo_span_setting(); 
bool servo_param_setting();
bool servo_span_setting2(int i);
bool servo_span_setting3();
bool servo_param_setting2(int x);

bool servo_setting_end = false;  //一度ｾｯﾃｲしていた場合


void servo_off_wait(){
    //moving2=99;
    while(moving1!=0){
        printf("0  %d %d\r\n",check_signal(),moving1);
        wait(0.2);
        send_i(ON_CHECK,0,0);
        while(check_signal()==false){printf("0"); wait(0.1);}
    }
}
void servo_set(){
    if(servo_settting == false){
        servo_settting = true;
        const char connect[4][14]={  {0xBB,0xB0,0xCE,0xDE,' ' ,0xBE,0xC2,0xBF,0xDE,0xB8,0xC1,0xAD,0xB3,0x00}      //ｻｰﾎﾞ ｾﾂｿﾞｸﾁｭｳ
                                    ,{0xBE,0xC2,0xBF,0xDE,0xB8,' ' ,0xBE,0xB2,0xBA,0xB3,' ' ,' ' ,' ' ,0x00}      //ｾﾂｿﾞｸ ｾｲｺｳ
                                    ,{0xBE,0xC2,0xBF,0xDE,0xB8,' ' ,0xBC,0xAF,0xCA,0xDF,0xB2,' ' ,' ' ,0x00}      //ｾﾂｿﾞｸ ｼｯﾊﾟｲ
                                    ,{0xBB,0xB0,0xCE,0xDE,' ' ,0xBE,0xAF,0xC3,0xB2,0xC1,0xAD,0xB3,' ' ,0x00}      //ｻｰﾎﾞ ｾｯﾃｲﾁｭｳ
                        };
                    
        int i=0;
        signal_flg=false; 
        lcd.blank(LOW);
        lcd.cls();
        print_body_param(); 
        lcd.locate(0,1);
        lcd.printf(connect[0]);
        
        //send_i(MOVE_CK,0,0);
        if(servo_setting_end == false){  //一度もサーボセッティングをしていない＝メイン起動時の動作中だったら、コントローラ側と確認をする　　　　　
            while(signal_flg==false &&i<20){
                if(i %5 == 0 ){
                    send_i(MOVE_CK,0,0);
                    printf("send MOVE_CK\r\n");
                }
                wait(0.5);  
                i+=1;
            }
            if(signal_flg==true){
                servo_connection=true;
                lcd.blank(LOW);
                lcd.locate(0,1);
                lcd.printf(connect[1]);
                wait(3);
            }else{
                servo_connection=false;
                lcd.blank(LOW);
                lcd.locate(0,1);
                lcd.printf(connect[2]);
                wait(3);
                
                return;
            }

        }
        lcd.blank(LOW);
        lcd.cls();
        print_body_param(); 
        lcd.locate(0,1);
        lcd.printf(connect[3]);
        file.read_dservo();
        file.read_servo();
        wait(0.5);
        i=0;
            //printf("g1\r\n");
        send_paramaters();  wait_signal();
        lcd.locate(14,1);
        lcd.printf("%2d",4);
        send_paramaters2(); wait_signal();
        lcd.locate(14,1);
        lcd.printf("%2d",3);
        send_paramaters3(); wait_signal();
        lcd.locate(14,1);
        lcd.printf("%2d",2);
            //printf("g2\r\n");
        set_servo_span();
        lcd.locate(14,1);
        lcd.printf("%2d",1);
            //printf("g3\r\n");
        
        i=0;
        signal_flg=false; 
        while(signal_flg==false &&i<10){
            send_i(END_VALUE,0,0);
                //printf("g4 %2d\r\n",i);
            wait(0.5);
            i+=1;
        }
        //printf("g5\r\n");
        send_i(TEST_END,0,0);
            //printf("g6\r\n");
        if(servo_first_settting == false){
             servo_off_wait();
        }else{
            wait(0.3);
        }
            //printf("g7\r\n");
        
        //wait(0.2);
        //send_i(KEIRYO_ST,0,0); 
        wait(0.2);
        if(servo_setting_end == true){
                //printf("g8\r\n");
            send_i(EJECT_RESET, 0, 0);
            i = 0;
            if(servo_first_settting == false){
                while(check_signal()==false && i<=10){
                    i+=1;
                    wait(0.1);
                }
            }else{
                wait(0.3);
            }
            if(keiryo_start_flg == true){
                send_i(EJECT_RESET, 11+load.kumi_flag, 0);
                i = 0;
                if(servo_first_settting == false){
                    while(check_signal()==false && i<=10){
                        i+=1;
                        wait(0.1);
                    }
                }else{
                    wait(0.3);
                }
            }
        
        }
        
        if(servo_setting_end == false){
                //printf("g9\r\n");
            //wait(1);
            //send_para(PARA_SET,product.head, course, 1, go_flager);wait_signal();
            send_para(PARA_SET,product.head, course, 1, 4);wait_signal();
                //printf("f1\r\n");
            send_paramaters2();wait_signal();
                //printf("f2\r\n");
            send_paramaters3();wait_signal();
                //printf("f3\r\n");
            send_i(EJECT_RESET,11, 0); wait(0.3);
            //send_i(SERVO_ON,ALL_HEAD,0);wait(0.3);
            //wait(4);
            
                //printf("f3\r\n");
            send_paramaters();wait_signal();
            
                //printf("f4\r\n");
            for(int header=0; header<product.head; header++){
                send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
            }
            //send_i(FAKE_GO,0,0);while(check_signal()==false){wait(0.1);}
            wait(0.5);
            for(int header=0; header<product.head; header++){
                send_i(SERVO_1 ,header,servo_span[header][0]); wait_res();
            }
            send_i(ON_CHECK,0,0);
            
            //printf("j1\r\n");
            while(check_signal()==false){wait(0.1);}
            //printf("j2\r\n");
            while(moving1>0){ 
                send_i(ON_CHECK,0,0);
                while(check_signal()==false){wait(0.1);}
            }
            send_i(EJECT_RESET, 0, 0); wait(0.3);
        }  
        i=0;
        signal_flg=false; 
        while(signal_flg==false &&i<10){
            send_i(END_VALUE,0,0);
            //printf("f5 %2d\r\n",i);
            wait(0.5);
            i+=1;
        }
        
        send_i(EJECT_RESET, 0, 0); wait(0.3);
            //printf("f6 %2d\r\n",i);
        send_i(KEIRYO_ST,0,0); wait(0.3);
            //printf("f7 %2d\r\n",i);
        lcd.cls();        
        wait(2);
        send_i(KEIRYO_EN,0,0); 
        wait(0.2);
        servo_settting = false;
        servo_setting_end = true;
        printf("end\r\n");
    }
}
bool servo_setting1(){
    int i =0;
    i = span_item_ck();
    if (i==99) {
        return false;
    }
    if(i==0){
        if(servo_span_setting()==false){
            return false;
        }
    }else{
        if(servo_span_setting2(i-1)==false){
            return false;
        }
    }
    return true;
}

bool servo_setting2(){
    led.ledAllOff();        //LED消灯
    if(now_TC_mode == 0){
        send_i(KEIRYO_ST,0,0); wait(0.2);
    }
    if(servo_param_setting()==false){
        if(now_TC_mode == 0){
            send_i(KEIRYO_EN,0,0); wait(0.2);
        }
        return false;
    }
    
    if(now_TC_mode == 0){
        send_i(KEIRYO_EN,0,0); wait(0.2);
    }
    return true;
}

bool servo_setting3(){
    if(servo_span_setting3()==false){
        return false;
    }
    return true;
}

bool servo_setting4(){
    led.ledAllOff();        //LED消灯
    if(now_TC_mode == 0){
        send_i(KEIRYO_ST,0,0); wait(0.2);
    }
    if(servo_param_setting2(1)==false){
        return false;
    }
    if(now_TC_mode == 0){
        send_i(KEIRYO_EN,0,0); wait(0.2);
    }
    return true;
}
bool zerocheck_set(){
    
    const char name3[][SIZE]={{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}       //ﾎｿﾞﾝﾁｭｳ
                             ,{0xB5,0xCF,0xC1,0xB8,0xC0,0xDE,0xBB,0xB2,0x00}};      //ｵﾏﾁｸﾀﾞｻｲ
    const char name4[]={0xBC,0xAE,0xD8,0xC1,0xAD,0xB3,0x00};      //ｼｮﾘﾁｭｳ
    data_t limit;
    limit.min = 0;
    limit.max = 10000;
    limit.lcd = 0;
    lcd.blank(1);
    
    lcd.locate(0,1);
    lcd.printf("      ");
    lcd.locate(0,1);
    if(kyousei_zero_kaisu == 0) {lcd.printf("OFF");}
    else{
        lcd.printf("%5d",kyousei_zero_kaisu);
    }
    
    
    while(btn.any_press()){wait(0.1);}
    btn.end_force();
    while(btn.ent_press()==false/* && btn.esc_press()==false) || btn.up_press()==true || btn.down_press()==true*/) {
        if(btn.up_press()||btn.down_press()) {
            if(btn.up_press()){ /*btn.force_UP();*/}
            else{               /*btn.force_DOWN();*/ }
            if(btn.push_sound()) {se.Touch_01();}   //ボタン操作時の効果音
            btn.puls_or_minus(&kyousei_zero_kaisu);
            if(kyousei_zero_kaisu < limit.min) kyousei_zero_kaisu = limit.max;
            if(kyousei_zero_kaisu > limit.max) kyousei_zero_kaisu = limit.min;
            
            lcd.locate(0,1);
            lcd.printf("      ");
            lcd.locate(0,1);
            if(kyousei_zero_kaisu == 0) {lcd.printf("OFF");}
            else{
                lcd.printf("%5d",kyousei_zero_kaisu);
            }
            wait(0.2);
        }
            
        if(btn.esc_press()==true/* && btn.up_press()==false && btn.down_press()==false*/) {
            lcd.blank(LOW);
            lcd.blank(TOP);
            lcd.locate(0,0);
            lcd.printf(name4);
            lcd.locate(0,1);
            lcd.printf(name3[1]);
            wait(0.2);
            btn.end_force();
            return false;
        }
        
        
        
        btn.old_btn();  //スイッチ更新
    }
    lcd.blank(LOW);
    lcd.blank(TOP);
    file.save_dservo();
    lcd.blank(LOW);
    lcd.blank(TOP);
    btn.end_force();
    return true;
    
}
bool servo_param_setting(){
    const char name1[][SIZE]={{0xCA,0xB2,0xBC,0xAD,0xC2,' ' ,0xCE,0xB3,0xBA,0xB3,' ' ,' ' ,' ' ,0x00}      //ﾊｲｼｭﾂ ﾎｳｺｳ
                             ,{0xCA,0xB2,0xBC,0xAD,0xC2,' ' ,0xCE,0xB3,0xCE,0xB3,' ' ,' ' ,' ' ,0x00}   //ﾊｲｼｭﾂ ﾎｳﾎｳ
                             ,{0xCA,0xB2,0xBC,0xAD,0xC2,' ' ,0xC0,0xB2,0xD0,0xDD,0xB8,0xDE,' ' ,0x00}   //ﾊｲｼｭﾂ ﾀｲﾐﾝｸﾞ
                             ,{0xCA,0xB2,0xBC,0xAD,0xC2,' ' ,0xCB,0xDF,0xAF,0xC1,' ' ,' ' ,' ' ,0x00}   //ﾊｲｼｭﾂ ﾋﾟｯﾁ
                             ,{0xB6,0xB2,0xCE,0xB3,' ' ,0xBC,0xDE,0xB6,0xDD,' ' ,' ' ,' ' ,' ' ,0x00}      //ｶｲﾎｳ ｼﾞｶﾝ
                             ,{0xCA,0xB2,0xBC,0xAD,0xC2,0xBC,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,' ' ,0x00}      //ﾊｲｼｭﾂｼｭｳﾘｮｳ
                             };
    const char name2[][SIZE]={{0xCE,0xDE,0xC0,0xDD,0xB6,0xDE,0xDC,0x00}      //ﾎﾞﾀﾝｶﾞﾜ
                             ,{0xCA,0xDD,0xC0,0xB2,0xB6,0xDE,0xDC,0x00}};    //ﾊﾝﾀｲｶﾞﾜ
    const char name3[][SIZE]={{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}       //ﾎｿﾞﾝﾁｭｳ
                             ,{0xB5,0xCF,0xC1,0xB8,0xC0,0xDE,0xBB,0xB2,0x00}};      //ｵﾏﾁｸﾀﾞｻｲ
    const char on_type[][SIZE]={{0xCE,0xDE,0xC0,0xDD,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾎﾞﾀﾝ
                               ,{0xBC,0xDE,0xC4,0xDE,0xB3,' ' ,0xCA,0xB2,0xBC,0xAD,0xC2,' ' ,' ' ,' ' ,' ' ,0x00}      //ｼﾞﾄﾞｳ ﾊｲｼｭﾂ
                               ,{0xBB,0xDD,0xC2,0xB7,' ' ,0xBA,0xDD,0xCD,0xDE,0xB1,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}    //ｻﾝﾂｷ ｺﾝﾍﾞｱ
                               ,{0xBB,0xDD,0xC2,0xB7,' ' ,0xBA,0xDD,0xCD,0xDE,0xB1,'2' ,' ' ,' ' ,' ' ,' ' ,0x00}    //ｻﾝﾂｷ ｺﾝﾍﾞｱ2
                               ,{0xBB,0xDD,0xC2,0xB7,' ' ,0x26,' ' ,0xCE,0xDE,0xC0,0xDD,'1' ,' ' ,' ' ,' ' ,0x00}      //ｻﾝﾂｷ & ﾎﾞﾀﾝ1
                               ,{0xBB,0xDD,0xC2,0xB7,' ' ,0x26,' ' ,0xCE,0xDE,0xC0,0xDD,'2' ,' ' ,' ' ,' ' ,0x00}      //ｻﾝﾂｷ & ﾎﾞﾀﾝ2
                               ,{0xBC,0xDE,0xC4,0xDE,0xB3,'2' ,' ' ,0x28,0xBE,0xDD,0xBB,0xB0,0xB1,0xD8,0x29,0x00}      //ｼﾞﾄﾞｳ2 (ｾﾝｻｰｱﾘ)
                               ,{0xBC,0xDE,0xC4,0xDE,0xB3,'3' ,' ' ,0x28,0xBE,0xDD,0xBB,0xB0,0xC5,0xBC,0x29,0x00}      //ｼﾞﾄﾞｳ3 (ｾﾝｻｰﾅｼ)
                               ,{0xBC,0xDE,0xC4,0xDE,0xB3,'4' ,' ' ,0x28,0xC3,0xB2,0xBC,'S' ,'W' ,0x29,' ' ,0x00}      //ｼﾞﾄﾞｳ4 (ﾃｲｼSW)
                               ,{0xB6,0xDD,0xB9,0xC2,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｶﾝｹﾂ
                               };
    const char name_auto[SIZE]=    {'A' ,'U' ,'T' ,'O' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00};

    const char name4[]={0xBC,0xAE,0xD8,0xC1,0xAD,0xB3,0x00};      //ｼｮﾘﾁｭｳ
    bool fake_go_sig = false; //FAKE_GOで信号を送ったか
        
    typedef enum menuop{
         SPS_courseints
        ,SPS_go_flager
        ,SPS_coount_rap_set
        ,SPS_course_rap
        ,SPS_landing_w
        ,SPS_waiter_t
        ,SPS_END
    }menuop;
    
    
    
    int san_coount_rap_set_min = 2;
    bool back_flg = true;
    data_t limit;
    int courseints=0;
    if(course==false){
        courseints=0;
    }else{
        courseints=1;
    }
    
    int k=0;
    int j=0; //ｻｰﾎﾞ動作開始を保留させる為のループ
    
    se.Touch_01();   //ボタン操作時の効果音
    send_i(EJECT_RESET,11, 0); wait(0.3);
    loop_a = 0;
    while(check_signal()==false&&loop_a<=10){
        loop_a+=1;
        wait(0.1);
    }
    while(k<6){
        send_i(ON_CHECK,0,0);
        lcd.blank(TOP);
        lcd.printf(name1[k]);
        lcd.blank(LOW);
        if(k==0){ lcd.printf(name2[courseints]);}
        else if(k==1){lcd.printf("%3d",san_coount_rap_set);}
        else if(k==2){lcd.printf("%3d",course_rap);}
        else{lcd.printf(on_type[go_flager]);}
        
        
        while(btn.any_press()){wait(0.1);}
        if(k==SPS_courseints){
            limit.min = 0;
            limit.max = 1;
            limit.lcd = 0;
            if(courseints < limit.min) courseints = limit.min;
            if(courseints > limit.max) courseints = limit.max;
            if(courseints==0){course=false;}
            else{             course=true;}
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf(name2[courseints]);
        }
        else if(k==SPS_coount_rap_set){
            
            limit.min = san_coount_rap_set_min;
            limit.max = 50;
            limit.lcd = 0;
            if(san_coount_rap_set < limit.min) san_coount_rap_set = limit.min;
            if(san_coount_rap_set > limit.max) san_coount_rap_set = limit.max;
            lcd.blank(LOW);
            lcd.locate(0,1);
            if(san_coount_rap_set == limit.min){
            lcd.printf(name_auto);
            }else{
                lcd.printf("%3d",san_coount_rap_set);
            }
        }
        else if(k==SPS_course_rap){
            limit.min = 1;
            limit.max = 50;
            limit.lcd = 0;
            if(course_rap < limit.min) course_rap = limit.min;
            if(course_rap > limit.max) course_rap = limit.max;
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf("%3d",course_rap);
        }
        else if(k==SPS_go_flager){
            limit.min = 0;
            limit.max = COMV_END-1;
            limit.lcd = 0;
            if(go_flager < limit.min) go_flager = limit.min;
            if(go_flager > limit.max) go_flager = limit.max;
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf(on_type[go_flager]);
        }
        else if(k==SPS_landing_w){
            limit.min = 1;
            limit.max = 300;
            limit.lcd = 0;
            if(landing_w < limit.min) landing_w = limit.min;
            if(landing_w > limit.max) landing_w = limit.max;
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf("%3d",landing_w);
        }
        else if(k==SPS_waiter_t){
            limit.min = 1;
            limit.max = 300;
            limit.lcd = 0;
            if(waiter_t < limit.min) waiter_t = limit.min;
            if(waiter_t > limit.max) waiter_t = limit.max;
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf("%3d",waiter_t);
        }
            
            
            
            
            
            
        back_flg = true;
        while(btn.ent_press()==false && back_flg == true/* && btn.esc_press()==false) || btn.up_press()==true || btn.down_press()==true*/) {
            if(btn.up_press()||btn.down_press()) {
                if(btn.up_press()){ /*btn.force_UP();*/}
                else{               /*btn.force_DOWN();*/ }
                if(btn.push_sound()) {se.Touch_01();}   //ボタン操作時の効果音
                if(k==SPS_courseints){
                    limit.min = 0;
                    limit.max = 1;
                    limit.lcd = 0;
                    btn.puls_or_minus(&courseints);
                    if(courseints < limit.min) courseints = limit.max;
                    if(courseints > limit.max) courseints = limit.min;
                    if(courseints==0){course=false;}
                    else{             course=true;}
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf(name2[courseints]);
                    wait(0.5);
                }
                else if(k==SPS_coount_rap_set){
                    limit.min = san_coount_rap_set_min;
                    limit.max = 50;
                    limit.lcd = 0;
                    btn.puls_or_minus(&san_coount_rap_set);
                    if(san_coount_rap_set < limit.min) san_coount_rap_set = limit.min;
                    if(san_coount_rap_set > limit.max) san_coount_rap_set = limit.max;
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    if(san_coount_rap_set == limit.min){
                        lcd.printf(name_auto);
                    }else{
                        lcd.printf("%3d",san_coount_rap_set);
                    }
                }
                else if(k==SPS_course_rap){
                    limit.min = 1;
                    limit.max = 50;
                    limit.lcd = 0;
                    btn.puls_or_minus(&course_rap);
                    if(course_rap < limit.min) course_rap = limit.max;
                    if(course_rap > limit.max) course_rap = limit.min;
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf("%3d",course_rap);
                    wait(0.2);
                }
                else if(k==SPS_go_flager){
                    limit.min = 0;
                    limit.max = COMV_END-1;
                    limit.lcd = 0;
                    btn.puls_or_minus(&go_flager);
                    if(go_flager < limit.min) go_flager = limit.max;
                    if(go_flager > limit.max) go_flager = limit.min;
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf(on_type[go_flager]);
                    wait(0.2);
                }
                else if(k==SPS_landing_w){
                    limit.min = 1;
                    limit.max = 300;
                    limit.lcd = 0;
                    btn.puls_or_minus(&landing_w);
                    if(landing_w < limit.min) landing_w = limit.min;
                    if(landing_w > limit.max) landing_w = limit.max;
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf("%3d",landing_w);
                }
                else if(k==SPS_waiter_t){
                    limit.min = 1;
                    limit.max = 300;
                    limit.lcd = 0;
                    btn.puls_or_minus(&waiter_t);
                    if(waiter_t < limit.min) waiter_t = limit.min;
                    if(waiter_t > limit.max) waiter_t = limit.max;
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf("%3d",waiter_t);
                }
                send_paramaters();  wait_signal();
                if(k==SPS_coount_rap_set && san_coount_rap_set == san_coount_rap_set_min){
                    san_coount_rap_set = 0;
                }
                send_paramaters2(); wait_signal();
                if(san_coount_rap_set == 0){
                    san_coount_rap_set = san_coount_rap_set_min;
                }
                send_paramaters3(); wait_signal();
                
                send_i(EJECT_RESET,11, 0); wait(0.3);
            }
                
            if(btn.esc_press()==true/* && btn.up_press()==false && btn.down_press()==false*/) {
                if(k>0){
                    k = k - 1;
                    wait(0.3);
                    while(btn.esc_press()==true){wait(0.3);}
                    back_flg = false;
                }
                else {
                    k = k - 1;
                    lcd.blank(LOW);
                    lcd.blank(TOP);
                    lcd.locate(0,0);
                    lcd.printf(name4);
                    lcd.locate(0,1);
                    lcd.printf(name3[1]);
                    wait(0.2);
                    btn.end_force();
                    return false;
                }
            }
            signal_flg=false;
            send_i(ON_CHECK,0,0);
            while(check_signal()==false){wait(0.1);}
            
            /*
            send_paramaters();  wait_signal();
            if(k==SPS_coount_rap_set && san_coount_rap_set == san_coount_rap_set_min){
                san_coount_rap_set = 0;
            }
            send_paramaters2(); wait_signal();
            if(san_coount_rap_set == 0){
                san_coount_rap_set = san_coount_rap_set_min;
            }*/
            if(moving1==0){ 
                wait(0.05);
                j +=5;
                if(j>=20){
                    j=0;
                    if(k==SPS_coount_rap_set){
                        if(courseints == 0){
                            send_i(SERVO_ON,1,0);
                            //pc.printf("SERVO_ON,1,0/r/n");
                        }else{
                            send_i(SERVO_ON,1 << product.head-1,0);
                            //pc.printf("SERVO_ON,1 << product.head-1,0/r/n");
                        }
                    }else{
                        send_i(SERVO_ON,ALL_HEAD,0);
                        //pc.printf("SERVO_ON,ALL_HEAD,0/r/n");
                    }
                    loop_a = 0;
                    while(check_signal()==false&&loop_a<=10){
                        loop_a+=1;
                        wait(0.1);
                    }
                    ///moving1=ALL_HEAD;moving2=ALL_HEAD;
                    if(k == 0 || (k == 3 && (go_flager ==0 || go_flager ==1))){
                        fake_go_sig = true;
                        send_i(FAKE_GO,0,0);while(check_signal()==false){wait(0.1);}
                        //pc.printf("FAKE_GO/r/n");  
                    }else{
                        if(fake_go_sig == true){
                            send_i(EJECT_RESET, 11, 0);while(check_signal()==false){wait(0.1);}
                            //pc.printf("EJECT_RESET/r/n");  
                            fake_go_sig = false;
                        }
                    }
                    //wait(5);
                }
            }
            btn.old_btn();  //スイッチ更新
        }
        if(btn.ent_press()==true) {
            k +=1;
            wait(0.3);
            while(btn.ent_press()==true){wait(0.3);}
        }
            
    }
    lcd.blank(LOW);
    lcd.blank(TOP);
    lcd.locate(0,0);
    lcd.printf(name3[0]);
    lcd.locate(0,1);
    lcd.printf(name3[1]);
    send_i(EJECT_RESET, 0, 0); wait(0.3);
    if(san_coount_rap_set == san_coount_rap_set_min){
        san_coount_rap_set = 0;
    }
    send_i(ALL_MIN,0,20);servo_off_wait(); wait(1.5);
    file.save_dservo();
    for(int l=0; l<product.head; l++) {
        servo_b_histry[l] = servo_b_histry[l] + servo_n_histry[0][l] + servo_n_histry[1][l];
        servo_n_histry[0][l] = 0;
        servo_n_histry[1][l] = 0;
    }
    file.save_hservo();
    send_paramaters();  wait_signal();
    send_paramaters2(); wait_signal();
    send_paramaters3(); wait_signal();
    lcd.blank(LOW);
    lcd.blank(TOP);
    btn.end_force();
    return true;
}
bool servo_param_setting2(int x){
    const char name1[][SIZE]={{0xCA,0xB2,0xBC,0xAD,0xC2,' ' ,0xCE,0xB3,0xBA,0xB3,' ' ,' ' ,' ' ,0x00}      //ﾊｲｼｭﾂ ﾎｳｺｳ
                             ,{0xCA,0xB2,0xBC,0xAD,0xC2,' ' ,0xCE,0xB3,0xCE,0xB3,' ' ,' ' ,' ' ,0x00}   //ﾊｲｼｭﾂ ﾎｳﾎｳ
                             ,{0xCA,0xB2,0xBC,0xAD,0xC2,' ' ,0xC0,0xB2,0xD0,0xDD,0xB8,0xDE,' ' ,0x00}   //ﾊｲｼｭﾂ ﾀｲﾐﾝｸﾞ
                             ,{0xCA,0xB2,0xBC,0xAD,0xC2,' ' ,0xCB,0xDF,0xAF,0xC1,' ' ,' ' ,' ' ,0x00}   //ﾊｲｼｭﾂ ﾋﾟｯﾁ
                             ,{0xB6,0xB2,0xCE,0xB3,' ' ,0xBC,0xDE,0xB6,0xDD,' ' ,' ' ,' ' ,' ' ,0x00}      //ｶｲﾎｳ ｼﾞｶﾝ
                             ,{0xCA,0xB2,0xBC,0xAD,0xC2,0xBC,0xAD,0xB3,0xD8,0xAE,0xB3,' ' ,' ' ,0x00}      //ﾊｲｼｭﾂｼｭｳﾘｮｳ
                             };
    const char name2[][SIZE]={{0xCE,0xDE,0xC0,0xDD,0xB6,0xDE,0xDC,0x00}      //ﾎﾞﾀﾝｶﾞﾜ
                             ,{0xCA,0xDD,0xC0,0xB2,0xB6,0xDE,0xDC,0x00}};    //ﾊﾝﾀｲｶﾞﾜ
    const char name3[][SIZE]={{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}       //ﾎｿﾞﾝﾁｭｳ
                             ,{0xB5,0xCF,0xC1,0xB8,0xC0,0xDE,0xBB,0xB2,0x00}};      //ｵﾏﾁｸﾀﾞｻｲ
    const char on_type[][SIZE]={{0xCE,0xDE,0xC0,0xDD,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ﾎﾞﾀﾝ
                               ,{0xBC,0xDE,0xC4,0xDE,0xB3,' ' ,0xCA,0xB2,0xBC,0xAD,0xC2,' ' ,' ' ,' ' ,' ' ,0x00}      //ｼﾞﾄﾞｳ ﾊｲｼｭﾂ
                               ,{0xBB,0xDD,0xC2,0xB7,' ' ,0xBA,0xDD,0xCD,0xDE,0xB1,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}    //ｻﾝﾂｷ ｺﾝﾍﾞｱ
                               ,{0xBB,0xDD,0xC2,0xB7,' ' ,0xBA,0xDD,0xCD,0xDE,0xB1,'2' ,' ' ,' ' ,' ' ,' ' ,0x00}    //ｻﾝﾂｷ ｺﾝﾍﾞｱ2
                               ,{0xBB,0xDD,0xC2,0xB7,' ' ,0x26,' ' ,0xCE,0xDE,0xC0,0xDD,'1' ,' ' ,' ' ,' ' ,0x00}      //ｻﾝﾂｷ & ﾎﾞﾀﾝ1
                               ,{0xBB,0xDD,0xC2,0xB7,' ' ,0x26,' ' ,0xCE,0xDE,0xC0,0xDD,'2' ,' ' ,' ' ,' ' ,0x00}      //ｻﾝﾂｷ & ﾎﾞﾀﾝ2
                               ,{0xBC,0xDE,0xC4,0xDE,0xB3,'2' ,' ' ,0x28,0xBE,0xDD,0xBB,0xB0,0xB1,0xD8,0x29,0x00}      //ｼﾞﾄﾞｳ2 (ｾﾝｻｰｱﾘ)
                               ,{0xBC,0xDE,0xC4,0xDE,0xB3,'3' ,' ' ,0x28,0xBE,0xDD,0xBB,0xB0,0xC5,0xBC,0x29,0x00}      //ｼﾞﾄﾞｳ3 (ｾﾝｻｰﾅｼ)
                               ,{0xBC,0xDE,0xC4,0xDE,0xB3,'4' ,' ' ,0x28,0xC3,0xB2,0xBC,'S' ,'W' ,0x29,' ' ,0x00}      //ｼﾞﾄﾞｳ4 (ﾃｲｼSW)
                               ,{0xB6,0xDD,0xB9,0xC2,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00}      //ｶﾝｹﾂ

                               };
    const char name_auto[SIZE]=    {'A' ,'U' ,'T' ,'O' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00};

    const char name4[]={0xBC,0xAE,0xD8,0xC1,0xAD,0xB3,0x00};      //ｼｮﾘﾁｭｳ
    bool fake_go_sig = false; //FAKE_GOで信号を送ったか
        
    int k=0;
    int j=0; //ｻｰﾎﾞ動作開始を保留させる為のループ
    typedef enum menuop{
         SPS_courseints
        ,SPS_go_flager
        ,SPS_coount_rap_set
        ,SPS_course_rap
        ,SPS_landing_w
        ,SPS_waiter_t
        ,SPS_END
    }menuop;
    
        //printf("02%2d\r\n",k);
    
    
    int san_coount_rap_set_min = 2;
    bool back_flg = true;
    data_t limit;
    int courseints=0;
    if(course==false){
        courseints=0;
    }else{
        courseints=1;
    }
    
        //printf("03%2d\r\n",k);
    
    se.Touch_01();   //ボタン操作時の効果音
    send_i(EJECT_RESET,11, 0); wait(0.3);
    loop_a = 0;
    while(check_signal()==false&&loop_a<=10){
        loop_a+=1;
        wait(0.1);
    }
    k = x;
        //printf("04%2d\r\n",k);
    while(k == x){
        
        //printf("05%2d\r\n",k);
        send_i(ON_CHECK,0,0);
        lcd.blank(TOP);
        lcd.printf(name1[k]);
        lcd.blank(LOW);
        if(k==0){ lcd.printf(name2[courseints]);}
        else if(k==1){lcd.printf("%3d",san_coount_rap_set);}
        else if(k==2){lcd.printf("%3d",course_rap);}
        else{lcd.printf(on_type[go_flager]);}
        
        
        while(btn.any_press()){wait(0.1);}
        if(k==SPS_courseints){
            limit.min = 0;
            limit.max = 1;
            limit.lcd = 0;
            if(courseints < limit.min) courseints = limit.min;
            if(courseints > limit.max) courseints = limit.max;
            if(courseints==0){course=false;}
            else{             course=true;}
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf(name2[courseints]);
        }
        else if(k==SPS_coount_rap_set){
            limit.min = san_coount_rap_set_min;
            limit.max = 50;
            limit.lcd = 0;
            if(san_coount_rap_set < limit.min) san_coount_rap_set = limit.min;
            if(san_coount_rap_set > limit.max) san_coount_rap_set = limit.max;
            lcd.blank(LOW);
            lcd.locate(0,1);
            if(san_coount_rap_set == limit.min){
            lcd.printf(name_auto);
            }else{
                lcd.printf("%3d",san_coount_rap_set);
            }
        }
        else if(k==SPS_course_rap){
            limit.min = 1;
            limit.max = 50;
            limit.lcd = 0;
            if(course_rap < limit.min) course_rap = limit.min;
            if(course_rap > limit.max) course_rap = limit.max;
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf("%3d",course_rap);
        }
        else if(k==SPS_go_flager){
            limit.min = 0;
            limit.max = 6;
            limit.lcd = 0;
            if(go_flager < limit.min) go_flager = limit.min;
            if(go_flager > limit.max) go_flager = limit.max;
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf(on_type[go_flager]);
        }
        else if(k==SPS_landing_w){
            limit.min = 1;
            limit.max = 300;
            limit.lcd = 0;
            if(landing_w < limit.min) landing_w = limit.min;
            if(landing_w > limit.max) landing_w = limit.max;
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf("%3d",landing_w);
        }
        else if(k==SPS_waiter_t){
            limit.min = 1;
            limit.max = 300;
            limit.lcd = 0;
            if(waiter_t < limit.min) waiter_t = limit.min;
            if(waiter_t > limit.max) waiter_t = limit.max;
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf("%3d",waiter_t);
        }
            
            
            
            
            
            
        back_flg = true;
        while(btn.ent_press()==false && back_flg == true/* && btn.esc_press()==false) || btn.up_press()==true || btn.down_press()==true*/) {
            if(btn.up_press()||btn.down_press()) {
                if(btn.up_press()){ /*btn.force_UP();*/}
                else{               /*btn.force_DOWN();*/ }
                if(btn.push_sound()) {se.Touch_01();}   //ボタン操作時の効果音
                if(k==SPS_courseints){
                    limit.min = 0;
                    limit.max = 1;
                    limit.lcd = 0;
                    btn.puls_or_minus(&courseints);
                    if(courseints < limit.min) courseints = limit.max;
                    if(courseints > limit.max) courseints = limit.min;
                    if(courseints==0){course=false;}
                    else{             course=true;}
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf(name2[courseints]);
                    wait(0.5);
                }
                else if(k==SPS_coount_rap_set){
                    limit.min = san_coount_rap_set_min;
                    limit.max = 50;
                    limit.lcd = 0;
                    btn.puls_or_minus(&san_coount_rap_set);
                    if(san_coount_rap_set < limit.min) san_coount_rap_set = limit.min;
                    if(san_coount_rap_set > limit.max) san_coount_rap_set = limit.max;
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    if(san_coount_rap_set == limit.min){
                        lcd.printf(name_auto);
                    }else{
                        lcd.printf("%3d",san_coount_rap_set);
                    }
                }
                else if(k==SPS_course_rap){
                    limit.min = 1;
                    limit.max = 50;
                    limit.lcd = 0;
                    btn.puls_or_minus(&course_rap);
                    if(course_rap < limit.min) course_rap = limit.max;
                    if(course_rap > limit.max) course_rap = limit.min;
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf("%3d",course_rap);
                    wait(0.2);
                }
                else if(k==SPS_go_flager){
                    limit.min = 0;
                    limit.max = 6;
                    limit.lcd = 0;
                    btn.puls_or_minus(&go_flager);
                    if(go_flager < limit.min) go_flager = limit.max;
                    if(go_flager > limit.max) go_flager = limit.min;
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf(on_type[go_flager]);
                    wait(0.2);
                }
                else if(k==SPS_landing_w){
                    limit.min = 1;
                    limit.max = 300;
                    limit.lcd = 0;
                    btn.puls_or_minus(&landing_w);
                    if(landing_w < limit.min) landing_w = limit.min;
                    if(landing_w > limit.max) landing_w = limit.max;
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf("%3d",landing_w);
                }
                else if(k==SPS_waiter_t){
                    limit.min = 1;
                    limit.max = 300;
                    limit.lcd = 0;
                    btn.puls_or_minus(&waiter_t);
                    if(waiter_t < limit.min) waiter_t = limit.min;
                    if(waiter_t > limit.max) waiter_t = limit.max;
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf("%3d",waiter_t);
                }
                send_paramaters();  wait_signal();
                if(k==SPS_coount_rap_set && san_coount_rap_set == san_coount_rap_set_min){
                    san_coount_rap_set = 0;
                }
                send_paramaters2(); wait_signal();
                if(san_coount_rap_set == 0){
                    san_coount_rap_set = san_coount_rap_set_min;
                }
                send_paramaters3(); wait_signal();
                
                send_i(EJECT_RESET,11, 0); wait(0.3);
            }
                
            if(btn.esc_press()==true/* && btn.up_press()==false && btn.down_press()==false*/) {
                if(k>0){
                    k = k - 1;
                    wait(0.3);
                    while(btn.esc_press()==true){wait(0.3);}
                    back_flg = false;
                }
                else {
                    k = k - 1;
                    lcd.blank(LOW);
                    lcd.blank(TOP);
                    lcd.locate(0,0);
                    lcd.printf(name4);
                    lcd.locate(0,1);
                    lcd.printf(name3[1]);
                    wait(0.2);
                    btn.end_force();
                    return false;
                }
            }
            signal_flg=false;
            send_i(ON_CHECK,0,0);
            while(check_signal()==false){wait(0.1);}
            
            /*
            send_paramaters();  wait_signal();
            if(k==SPS_coount_rap_set && san_coount_rap_set == san_coount_rap_set_min){
                san_coount_rap_set = 0;
            }
            send_paramaters2(); wait_signal();
            if(san_coount_rap_set == 0){
                san_coount_rap_set = san_coount_rap_set_min;
            }*/
            if(moving1==0){ 
                wait(0.05);
                j +=5;
                if(j>=20){
                    j=0;
                    if(k==SPS_coount_rap_set){
                        if(courseints == 0){
                            send_i(SERVO_ON,1,0);
                        }else{
                            send_i(SERVO_ON,1 << product.head,0);
                        }
                    }else{
                        send_i(SERVO_ON,ALL_HEAD,0);
                    }
                    loop_a = 0;
                    while(check_signal()==false&&loop_a<=10){
                        loop_a+=1;
                        wait(0.1);
                    }
                    ///moving1=ALL_HEAD;moving2=ALL_HEAD;
                    if(k == 0 || (k == 3 && (go_flager ==0 || go_flager ==1))){
                        fake_go_sig = true;
                        send_i(FAKE_GO,0,0);while(check_signal()==false){wait(0.1);}
                    }else{
                        if(fake_go_sig == true){
                            send_i(EJECT_RESET, 11, 0);while(check_signal()==false){wait(0.1);}
                            fake_go_sig = false;
                        }
                    }
                    //wait(5);
                }
            }
            btn.old_btn();  //スイッチ更新
        }
        if(btn.ent_press()==true) {
            k +=1;
            wait(0.3);
            while(btn.ent_press()==true){wait(0.3);}
        }
            
    }
    lcd.blank(LOW);
    lcd.blank(TOP);
    lcd.locate(0,0);
    lcd.printf(name3[0]);
    lcd.locate(0,1);
    lcd.printf(name3[1]);
    send_i(EJECT_RESET, 0, 0); wait(0.3);
    if(san_coount_rap_set == san_coount_rap_set_min){
        san_coount_rap_set = 0;
    }
    send_i(ALL_MIN,0,20);servo_off_wait(); wait(1.5);
    file.save_dservo();
    for(int l=0; l<product.head; l++) {
        servo_b_histry[l] = servo_b_histry[l] + servo_n_histry[0][l] + servo_n_histry[1][l];
        servo_n_histry[0][l] = 0;
        servo_n_histry[1][l] = 0;
    }
    file.save_hservo();
    send_paramaters();  wait_signal();
    send_paramaters2(); wait_signal();
    send_paramaters3(); wait_signal();
    lcd.blank(LOW);
    lcd.blank(TOP);
    btn.end_force();
    return true;
}
bool servo_span_setting(){
    const char name[][SIZE]={{0xBB,0xD7,0xD2,' ' ,0xBB,0xB2,0xBC,0xAE,0xB3,0x00}      //ｻﾗﾒ ｻｲｼｮｳ
                            ,{0xBB,0xD7,0xD2,' ' ,0xBB,0xB2,0xC0,0xDE,0xB2,0x00}};    //ｻﾗﾒ ｻｲﾀﾞｲ

    const char name3[][SIZE]={{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}       //ﾎｿﾞﾝﾁｭｳ
                             ,{0xB5,0xCF,0xC1,0xB8,0xC0,0xDE,0xBB,0xB2,0x00}};      //ｵﾏﾁｸﾀﾞｻｲ
    const char name4[]={0xBC,0xAE,0xD8,0xC1,0xAD,0xB3,0x00};      //ｼｮﾘﾁｭｳ

    int head=0,hilo=0;
    data_t limit;
    //send_i(SETING_S,0,0); wait_res();
    while(btn.any_press()){wait(0.1);}
    send_i(TEST_SET,0,0);
    limit.min = 130;
    limit.max = 730;
    limit.lcd = 0;//ON/OFFChenge_vaule表示
    while(head<product.head){
        hilo=0;
        while(hilo<2){
            se.Touch_01();
            btn.ini();              //保持ボタンを初期化
            printf("%4d\r\n",servo_span[head][hilo] );
            //最小値より低ければ,値を初期化
            if(servo_span[head][hilo] < limit.min) servo_span[head][hilo] = limit.min;
            if(servo_span[head][hilo] > limit.max) servo_span[head][hilo] = limit.max;
            //lcd_mode = limit.lcd;
            //int dumy_servo_span[head][hilo]=limit.min;         //表示用数量保存場所(例)回数制限の数を見せたいとき
            lcd.locate(9,1);
            if(limit.lcd>1) {
                limit.lcd=1;
            }
            //行を空白で埋める
            lcd.blank(TOP);
            lcd.printf("%2d",head+1);
            lcd.printf(name[hilo]);
            lcd.locate(0,0);
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf("%4d",servo_span[head][hilo]);
            if(hilo==0){
                send_i(MI_VALUE,head,servo_span[head][0]); wait_res();
            }else{
                send_i(UP_VALUE,head,servo_span[head][1]); wait_res();
            }
            send_i(SERVO_1 ,head,servo_span[head][hilo]); wait_res();
            for(int header=0; header<product.head; header++){
                send_i(SERVO_1 ,header,servo_span[header][hilo]); wait_res();
            }
            //send_i(SERVO_ON,1<<head,0);wait(0.2);
            while(btn.any_press()){wait(0.1);}
            while((btn.ent_press()==false&&btn.esc_press()==false) || btn.up_press()==true || btn.down_press()==true) {
                led.REGIS(1<<head);
                //何かキーが押されたら表示更新(もしくは一回目)
                if(btn.up_press()||btn.down_press()) {
                    //btn.new_btn();                           //スイッチ(New)取得
                    if(btn.up_press()){ /*btn.force_UP();*/}
                    else{               /*btn.force_DOWN();*/ }
                    if(btn.push_sound()) {se.Touch_01();}   //ボタン操作時の効果音
                    
                    btn.puls_or_minus(&servo_span[head][hilo]);
                    if(btn.esc_press()==true && (btn.up_press()==true || btn.down_press()==true)) {
                        btn.puls_or_minus(&servo_span[head][hilo]);
                        btn.puls_or_minus(&servo_span[head][hilo]);
                        btn.puls_or_minus(&servo_span[head][hilo]);
                        btn.puls_or_minus(&servo_span[head][hilo]);
                    }
            
                    //値が範囲を超えたら修正※最大超しても最小ではなく最大値のまま(急に最小になるとビビる)
                    if(servo_span[head][hilo] < limit.min) {servo_span[head][hilo] = limit.min; se.Touch_01();se.Touch_01();se.Touch_01();}
                    if(servo_span[head][hilo] > limit.max) {servo_span[head][hilo] = limit.max; se.Touch_01();se.Touch_01();se.Touch_01();}
                    if(hilo==0){
                        send_i(MI_VALUE,head,servo_span[head][0]); wait_res();wait(0.2);
                    }else{
                        send_i(UP_VALUE,head,servo_span[head][1]); wait_res();wait(0.2);
                    }
                    send_i(SERVO_1 ,head,servo_span[head][hilo]); wait_res();
                    //send_i(SERVO_ON,1<<head,0);wait(0.2);
                    //servo_off_wait();
                    //行を空白で埋める
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf("%4d",servo_span[head][hilo]);
            
                    btn.old_btn();  //スイッチ更新
                }
                if(btn.esc_press()==true && btn.up_press()==false && btn.down_press()==false) {
                    //se.Touch_01();   //ボタン操作時の効果音
                    if(hilo>0){hilo-=1;}
                    else if(head>0){head-=1;hilo=1;}
                    else{
                        lcd.blank(LOW);
                        lcd.blank(TOP);
                        lcd.locate(0,0);
                        lcd.printf(name4);
                        lcd.locate(0,1);    
                        lcd.printf(name3[1]);
                        wait(0.5);
                        btn.old_btn();  //スイッチ更新return false;
                        send_i(ALL_MIN,0,20);servo_off_wait(); wait(1.5);
                        file.read_servo();
                        //servo_set();
                        set_servo_span();
                        //send_i(SETING_E,0,0); wait_res();
                        send_i(TEST_END,0,0); servo_off_wait();
                        return false;
                    }
                    btn.old_btn();  //スイッチ更新
                }   //esc押下なら処理終了
            }//while
            for(int l=0; l<product.head; l++) {
                iro[l]=C_NULL;
            }
            
            
            if(btn.esc_press()==false) {hilo +=1;}
        }
        if(btn.esc_press()==false) {head +=1;}
    }
    while(btn.any_press()){wait(0.1);}
    lcd.blank(LOW);
    lcd.blank(TOP);
    lcd.locate(0,0);
    lcd.printf(name3[0]);
    lcd.locate(0,1);
    lcd.printf(name3[1]);
    send_i(ALL_MIN,0,20);servo_off_wait(); wait(1.5);
    file.save_servo();
    //servo_set();
    set_servo_span();
    lcd.blank(LOW);
    lcd.blank(TOP);
    //send_i(SETING_E,0,0); wait_res();
    send_i(TEST_END,0,0);
    return true;
}


bool servo_span_setting2(int i){
    const char name[][SIZE]={{0xBB,0xD7,0xD2,' ' ,0xBB,0xB2,0xBC,0xAE,0xB3,0x00}      //ｻﾗﾒ ｻｲｼｮｳ
                            ,{0xBB,0xD7,0xD2,' ' ,0xBB,0xB2,0xC0,0xDE,0xB2,0x00}};    //ｻﾗﾒ ｻｲﾀﾞｲ

    const char name3[][SIZE]={{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}       //ﾎｿﾞﾝﾁｭｳ
                             ,{0xB5,0xCF,0xC1,0xB8,0xC0,0xDE,0xBB,0xB2,0x00}};      //ｵﾏﾁｸﾀﾞｻｲ
    const char name4[]={0xBC,0xAE,0xD8,0xC1,0xAD,0xB3,0x00};      //ｼｮﾘﾁｭｳ

    int head=0,hilo=0;
    data_t limit;
    //send_i(SETING_S,0,0); wait_res();
    while(btn.any_press()){wait(0.1);}
    send_i(TEST_SET,0,0);
    limit.min = 130;
    limit.max = 730;
    limit.lcd = 0;//ON/OFFChenge_vaule表示
    head = i;
    while(head == i){
        hilo=0;
        while(hilo<2){
            se.Touch_01();
            btn.ini();              //保持ボタンを初期化
            printf("%4d\r\n",servo_span[head][hilo] );
            //最小値より低ければ,値を初期化
            if(servo_span[head][hilo] < limit.min) servo_span[head][hilo] = limit.min;
            if(servo_span[head][hilo] > limit.max) servo_span[head][hilo] = limit.max;
            //lcd_mode = limit.lcd;
            //int dumy_servo_span[head][hilo]=limit.min;         //表示用数量保存場所(例)回数制限の数を見せたいとき
            lcd.locate(9,1);
            if(limit.lcd>1) {
                limit.lcd=1;
            }
            //行を空白で埋める
            lcd.blank(TOP);
            lcd.printf("%2d",head+1);
            lcd.printf(name[hilo]);
            lcd.locate(0,0);
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf("%4d",servo_span[head][hilo]);
            if(hilo==0){
                send_i(MI_VALUE,head,servo_span[head][0]); wait_res();
            }else{
                send_i(UP_VALUE,head,servo_span[head][1]); wait_res();
            }
            send_i(SERVO_1 ,head,servo_span[head][hilo]); wait_res();
            
            for(int header=0; header<product.head; header++){
                send_i(SERVO_1 ,header,servo_span[header][hilo]); wait_res();
            }
            //send_i(SERVO_ON,1<<head,0);wait(0.2);
            while(btn.any_press()){wait(0.1);}
            while((btn.ent_press()==false&&btn.esc_press()==false) || btn.up_press()==true || btn.down_press()==true) {
                led.REGIS(1<<head);
                //何かキーが押されたら表示更新(もしくは一回目)
                if(btn.up_press()||btn.down_press()) {
                    //btn.new_btn();                           //スイッチ(New)取得
                    if(btn.up_press()){ /*btn.force_UP();*/}
                    else{               /*btn.force_DOWN();*/ }
                    if(btn.push_sound()) {se.Touch_01();}   //ボタン操作時の効果音
                    
                    btn.puls_or_minus(&servo_span[head][hilo]);
                    if(btn.esc_press()==true && (btn.up_press()==true || btn.down_press()==true)) {
                        btn.puls_or_minus(&servo_span[head][hilo]);
                        btn.puls_or_minus(&servo_span[head][hilo]);
                        btn.puls_or_minus(&servo_span[head][hilo]);
                        btn.puls_or_minus(&servo_span[head][hilo]);
                    }
            
                    //値が範囲を超えたら修正※最大超しても最小ではなく最大値のまま(急に最小になるとビビる)
                    if(servo_span[head][hilo] < limit.min) {servo_span[head][hilo] = limit.min; se.Touch_01();se.Touch_01();se.Touch_01();}
                    if(servo_span[head][hilo] > limit.max) {servo_span[head][hilo] = limit.max; se.Touch_01();se.Touch_01();se.Touch_01();}
                    
                    if(hilo==0){
                        send_i(MI_VALUE,head,servo_span[head][0]); wait_res();wait(0.2);
                    }else{
                        send_i(UP_VALUE,head,servo_span[head][1]); wait_res();wait(0.2);
                    }
                    send_i(SERVO_1 ,head,servo_span[head][hilo]); wait_res();
                    //send_i(SERVO_ON,1<<head,0);wait(0.2);
                    //servo_off_wait();
                    //行を空白で埋める
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf("%4d",servo_span[head][hilo]);
            
                    btn.old_btn();  //スイッチ更新
                }
                if(btn.esc_press()==true && btn.up_press()==false && btn.down_press()==false) {
                    //se.Touch_01();   //ボタン操作時の効果音
                    if(hilo>0){hilo-=1;}
                    //else if(head>0){head-=1;hilo=1;}
                    else{
                        lcd.blank(LOW);
                        lcd.blank(TOP);
                        lcd.locate(0,0);
                        lcd.printf(name4);
                        lcd.locate(0,1);    
                        lcd.printf(name3[1]);
                        wait(0.5);
                        btn.old_btn();  //スイッチ更新return false;
                        send_i(ALL_MIN,0,20);servo_off_wait(); wait(1.5);
                        file.read_servo();
                        servo_set();
                        set_servo_span();
                        //send_i(SETING_E,0,0); wait_res();
                        send_i(TEST_END,0,0); servo_off_wait();
                        return false;
                    }
                    btn.old_btn();  //スイッチ更新
                }   //esc押下なら処理終了
            }//while
            for(int l=0; l<product.head; l++) {
                iro[l]=C_NULL;
            }
            
            
            if(btn.esc_press()==false) {hilo +=1;}
        }
        if(btn.esc_press()==false) {head +=1;}
    }
    while(btn.any_press()){wait(0.1);}
    lcd.blank(LOW);
    lcd.blank(TOP);
    lcd.locate(0,0);
    lcd.printf(name3[0]);
    lcd.locate(0,1);
    lcd.printf(name3[1]);
    file.save_servo();
    send_i(ALL_MIN,0,20);servo_off_wait(); wait(1.5);
    //file.save_servo();
    //servo_set();
    set_servo_span();
    lcd.blank(LOW);
    lcd.blank(TOP);
    //send_i(SETING_E,0,0); wait_res();
    send_i(TEST_END,0,0);
    return true;
}


bool servo_span_setting3(){
    const char name[][SIZE]={{0xBB,0xB2,0xBC,0xAE,0xB3,0x00}      //ｻﾗﾒ ｻｲｼｮｳ
                            ,{0xBB,0xD7,0xD2,' ' ,0xBB,0xB2,0xC0,0xDE,0xB2,0x00}};    //ｻﾗﾒ ｻｲﾀﾞｲ

    const char name3[][SIZE]={{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}       //ﾎｿﾞﾝﾁｭｳ
                             ,{0xB5,0xCF,0xC1,0xB8,0xC0,0xDE,0xBB,0xB2,0x00}};      //ｵﾏﾁｸﾀﾞｻｲ
    const char name4[]={0xBC,0xAE,0xD8,0xC1,0xAD,0xB3,0x00};      //ｼｮﾘﾁｭｳ

    int head=0,hilo=0;
    int tolerance = servo_span[0][1]-servo_span[0][0];
    data_t limit;
    //send_i(SETING_S,0,0); wait_res();
    while(btn.any_press()){wait(0.1);}
    send_i(TEST_SET,0,0);
    limit.min = 130;
    limit.max = 730;
    limit.lcd = 0;//ON/OFFChenge_vaule表示
    head = 0;
    hilo=1;
    while(head == i){
        
            printf("%4d %4d %4d\r\n",servo_span[head][hilo],head,hilo );
        while(hilo<2){
            se.Touch_01();
            btn.ini();              //保持ボタンを初期化
            printf("%4d\r\n",servo_span[head][hilo] );
            //最小値より低ければ,値を初期化
            if(servo_span[0][hilo] < limit.min) servo_span[0][hilo] = limit.min;
            if(servo_span[0][hilo] > limit.max) servo_span[0][hilo] = limit.max;
            //lcd_mode = limit.lcd;
            //int dumy_servo_span[head][hilo]=limit.min;         //表示用数量保存場所(例)回数制限の数を見せたいとき
            lcd.locate(9,1);
            if(limit.lcd>1) {
                limit.lcd=1;
            }
            //行を空白で埋める
            lcd.blank(TOP);
            //lcd.printf("%2d",head+1);
            //lcd.printf(name[hilo]);
            lcd.set_option(OP_SERVO3);
            lcd.locate(0,0);
            lcd.blank(LOW);
            lcd.locate(0,1);
            lcd.printf("%4d",tolerance);
            for(int header=0; header<product.head; header++){
                servo_span[header][1] = servo_span[header][0] + tolerance;
                send_i(UP_VALUE,header,servo_span[header][1]); wait_res();
                send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
            }
            //send_i(SERVO_ON,1<<head,0);wait(0.2);
            while(btn.any_press()){wait(0.1);}
            while((btn.ent_press()==false&&btn.esc_press()==false) || btn.up_press()==true || btn.down_press()==true) {
                led.REGIS(1<<head);
                //何かキーが押されたら表示更新(もしくは一回目)
                if(btn.up_press()||btn.down_press()) {
                    //btn.new_btn();                           //スイッチ(New)取得
                    if(btn.up_press()){ /*btn.force_UP();*/}
                    else{               /*btn.force_DOWN();*/ }
                    if(btn.push_sound()) {se.Touch_01();}   //ボタン操作時の効果音
                    
                    btn.puls_or_minus(&tolerance);
                    btn.puls_or_minus(&tolerance);
                    btn.puls_or_minus(&tolerance);
                    btn.puls_or_minus(&tolerance);
                    btn.puls_or_minus(&tolerance);
                    if(btn.esc_press()==true && (btn.up_press()==true || btn.down_press()==true)) {
                        btn.puls_or_minus(&tolerance);
                        btn.puls_or_minus(&tolerance);
                        btn.puls_or_minus(&tolerance);
                        btn.puls_or_minus(&tolerance);
                        btn.puls_or_minus(&tolerance);
                        btn.puls_or_minus(&tolerance);
                        btn.puls_or_minus(&tolerance);
                        btn.puls_or_minus(&tolerance);
                        btn.puls_or_minus(&tolerance);
                        btn.puls_or_minus(&tolerance);
                    }
            
                    //値が範囲を超えたら修正※最大超しても最小ではなく最大値のまま(急に最小になるとビビる)
                    if(tolerance   <= 5) {tolerance = 5; se.Touch_01();se.Touch_01();se.Touch_01();}
                    if(servo_span[head][hilo] > limit.max) {tolerance = limit.max - servo_span[head][0]; se.Touch_01();se.Touch_01();se.Touch_01();}
                    
                    for(int header=0; header<product.head; header++){
                        servo_span[header][1] = servo_span[header][0] + tolerance;
                        send_i(UP_VALUE,header,servo_span[header][1]); wait_res();//wait(0.2);
                        send_i(SERVO_1 ,header,servo_span[header][1]); wait_res();
                    }
                    //send_i(SERVO_ON,1<<head,0);wait(0.2);
                    //servo_off_wait();
                    //行を空白で埋める
                    lcd.blank(LOW);
                    lcd.locate(0,1);
                    lcd.printf("%4d",tolerance);
            
                    btn.old_btn();  //スイッチ更新
                }
                if(btn.esc_press()==true && btn.up_press()==false && btn.down_press()==false) {
                    //se.Touch_01();   //ボタン操作時の効果音
                    //if(hilo>0){hilo-=1;}
                    //else if(head>0){head-=1;hilo=1;}
                    //else{
                        lcd.blank(LOW);
                        lcd.blank(TOP);
                        lcd.locate(0,0);
                        lcd.printf(name4);
                        lcd.locate(0,1);    
                        lcd.printf(name3[1]);
                        wait(0.5);
                        btn.old_btn();  //スイッチ更新return false;
                        send_i(ALL_MIN,0,20);servo_off_wait(); wait(1);
                        file.read_servo();
                        //servo_set();
                        set_servo_span();
                        //send_i(SETING_E,0,0); wait_res();
                        send_i(TEST_END,0,0); servo_off_wait();
                        return false;
                    //}
                    btn.old_btn();  //スイッチ更新
                }   //esc押下なら処理終了
            }//while
            for(int l=0; l<product.head; l++) {
                iro[l]=C_NULL;
            }
            
            
            if(btn.esc_press()==false) {hilo +=1;}
        }
        if(btn.esc_press()==false) {head +=1;}
    }
    while(btn.any_press()){wait(0.1);}
    lcd.blank(LOW);
    lcd.blank(TOP);
    lcd.locate(0,0);
    lcd.printf(name3[0]);
    lcd.locate(0,1);
    lcd.printf(name3[1]);
    send_i(ALL_MIN,0,20);servo_off_wait(); wait(1.5);
    file.save_servo();
    //servo_set();
    set_servo_span();
    lcd.blank(LOW);
    lcd.blank(TOP);
    //send_i(SETING_E,0,0); wait_res();
    send_i(TEST_END,0,0);
    return true;
}
void servo_test(){
    /**/
    send_i(SETING_S,0,0); wait(1);
    send_i(ALL_MIN,0,400);servo_off_wait(); wait(1);
    send_i(ALL_UP,0,400);servo_off_wait(); wait(0.5);
    send_i(ALL_MIN,0,400);servo_off_wait();
    //wait(1);
    //printf("a\r\n");
    //send_i(SERVO_ON,1,0);
    //printf("a\r\n");
    //servo_off_wait();
    
    
    //printf("r\r\n");
    //wait(1.5);
    //send_i(TEST_MOVE,0,400);servo_off_wait(); wait(1.5);
    //send_i(TEST_MOVE,0,300);servo_off_wait(); wait(1.5);
    
    //send_i(TEST_SET,0,0);
    //send_i(UP_VALUE,0,400); wait_res();
    //send_i(SERVO_ON,1,0);
    //servo_off_wait();
    //send_i(UP_VALUE,0,250); wait_res();
    //send_i(SERVO_ON,1,0);
    //servo_off_wait();
    //printf("r\r\n");
    
    send_i(TEST_END,0,0); servo_off_wait();wait(3);
    //course_rap=1;send_para(PARA_SET,servo_head, course, course_rap);
    //send_i(SERVO_ON,63,0);
    //send_i(SERVO_ON,ALL_HEAD,0);
    //servo_off_wait();
    
    course_rap=2;
    
    send_para(PARA_SET,servo_head, course, course_rap, go_flager);
    //wait(3);
    //send_i(SERVO_ON,63,0);
    send_i(SERVO_ON,ALL_HEAD,0);
    servo_off_wait();
    course=true;
    send_para(PARA_SET,servo_head, course, course_rap, go_flager);
    //send_i(SERVO_ON,63,0);
    send_i(SERVO_ON,ALL_HEAD,0);
    servo_off_wait();
    //send_i(SERVO_ON,21,0);
    send_i(SERVO_ON,ALL_HEAD & 1365,0);
    servo_off_wait();
    //send_i(SERVO_ON,42,0);
    send_i(SERVO_ON,ALL_HEAD & 2730,0);
    servo_off_wait();
    /**/
    //set_servo_span();
    send_i(SERVO_ON,ALL_HEAD,0);
    servo_off_wait();
    
    send_i(SETING_E,0,0); wait(1);
    file.read_servo();
    send_para(PARA_SET,servo_head, course, course_rap, go_flager);
    //file.save_dservo();
}