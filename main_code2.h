void initial_drive_function(){
    for(int l=0; l<product.head; l++) {
        nowweightVale3[l]=0.0;
        servo_n_histry[0][l]=0;
        servo_n_histry[1][l]=0;
    }
}



bool touroku_data_check(){
    bool data_check = true;
    cel.set_now_loadcell_data(&load);   //現在選択されているプリセット(ロードセル)をset
    
    if(set_mokuhyo_no(load.kumi_flag) == 0){
        data_check = false;
    }else if(product.limit_on==NUMBER_LIMIT && (load.kumi_flag >= KUMI_LIMIT || load.spMode == 0 )){
        data_check = false;
    }else if(product.limit_on==SELECTOR_LIMIT && load.kumi_flag != KUMI_RANK ){
        data_check = false;
    //}else if(product.limit_on==NIRA_LIMIT     && load.kumi_flag != KUMI_JUURYO ){
    //    data_check = false;
    }else if(product.head==1 && (load.kumi_flag != KUMI_RANK && load.kumi_flag != KUMI_KOSUU && load.kumi_flag != KUMI_PACK && load.kumi_flag != KUMI_PACK2)){
        data_check = false;
    }else if(load.kumi_flag <= KUMI_LIMIT && load.taimenTarget > 0 && (load.target != load.taimenTarget && load.jougen >= load.taimenJougen)){//  制限なし・皿数制限時に対面目標値が目標値と違う場合、登録出来てない可能性があるので、ミスとする、　                                                                                             
        data_check = false;
    }
                
    if(data_check == false){
        v.Unregister();
        v.output_audio2();
        return false;
    }
    return true;
}


void result_LCD(int ji,int hun){
    
    int num = 0;
    int min = 0;
    int max = 2; 
    
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("total:%4d",total_kumi_cnt);
    lcd.locate(0,1);
    lcd.printf("ave  :%6.1f",average_kumi_value);
    
    while(btn.esc_press()==true){wait(0.2);}  //escを離すまで待つ
    while(btn.get_btn() != BTN_ENT) {
        btn.new_btn();              //スイッチ(New)取得
        //ボタン操作時の効果音
        if(btn.esc()) return;       //処理終了。値の変更なし
        
        btn.puls_or_minus(&num);
        if(btn.push_sound()) {
            se.Touch_01();
        }
        if(     num > max){num=max; se.Touch_01();se.Touch_01();while(btn.any_press()==true ){wait(0.2);}}
        else if(num < min){num=min; se.Touch_01();se.Touch_01();while(btn.any_press()==true ){wait(0.2);}}
        
        if(btn.get_btn()) {
            switch(num){
                case 0: 
                    lcd.cls();
                    lcd.locate(0,0);
                    lcd.printf("total:%4d",total_kumi_cnt);
                    lcd.locate(0,1);
                    lcd.printf("ave  :%6.1f",average_kumi_value);
                    wait(0.2);
                    break;
                    //while(btn.any_press()==false){wait(0.2);}
                    //while(btn.any_press()==true ){wait(0.2);}
                case 1:
                    lcd.cls();
                    lcd.locate(0,0);
                    lcd.printf("time :%02d:%02d",ji,hun);
                    lcd.locate(0,1);
                    if((ji*60)+hun==0){
                        lcd.printf("1min :%3d",total_kumi_cnt);
                    }else{
                        lcd.printf("1min :%3d",int(total_kumi_cnt/((ji*60)+hun)));
                        printf("ji*60=%3d |(ji*60)+hun%3d\r\n   ",(ji*60),(ji*60)+hun);
                    }
                    wait(0.2);
                    break;
                    //while(btn.any_press()==false){wait(0.2);}
                    //while(btn.any_press()==true ){wait(0.2);}
         
                case 2:
                    lcd.cls();
                    lcd.locate(0,0);
                    lcd.printf("err:%4d%c%5.2f%c%c",cnt_miss_and_kikakugai,0x28,float(cnt_miss_and_kikakugai)/float(total_kumi_cnt),0x25,0x29);
                    //printf("err :%4d(%4.1f%%)",cnt_miss_and_kikakugai,float(cnt_miss_and_kikakugai)/float(total_kumi_cnt));
                    lcd.locate(0,1);
                    lcd.printf("jst:%4d%c%5.2f%c%c",cnt_junsaiteki,0x28,float(cnt_junsaiteki)/float(total_kumi_cnt),0x25,0x29);
                    //printf("jst :%4d(%4.1f%%)",cnt_junsaiteki,float(cnt_junsaiteki)/float(total_kumi_cnt));
                    wait(0.2);
                    break;
                    //while(btn.any_press()==false){wait(0.2);}
                    //while(btn.any_press()==true ){wait(0.2);}
                   
            }
        
        }
        btn.old_btn();  //スイッチ更新
    }
}


void result_LCD_servo(int ji,int hun){
    
    int num = -1;
    int min = -1;
    int max = product.head/2-1; 
    
    while(btn.ent_press()==true){wait(0.2);}  //escを離すまで待つ
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("total:%7d",total_kumi_cnt);
    lcd.locate(0,1);
    lcd.printf("     :%7d",servo_histry);
    wait(0.2);
    se.Touch_01();
    
    for(int a=0 ; a < product.head ; a++){                                
        if( servo_n_histry[1][a] == 1){
            servo_n_histry[0][a] += 1;
            servo_n_histry[1][a] =  0;
        }
    }
    while(btn.esc_press()==true){wait(0.2);}  //escを離すまで待つ
    while(btn.get_btn() != BTN_ENT) {
        btn.new_btn();              //スイッチ(New)取得
        if(btn.esc()) return;       //処理終了。値の変更なし
        
        btn.puls_or_minus(&num);
        //ボタン操作時の効果音
        if(btn.push_sound()) {
            se.Touch_01();
        }
        if(     num > max){num=max; se.Touch_01();se.Touch_01();while(btn.any_press()==true ){wait(0.2);}}
        else if(num < min){num=min; se.Touch_01();se.Touch_01();while(btn.any_press()==true ){wait(0.2);}}
        
        if(btn.get_btn()) {
            lcd.cls();
            lcd.locate(0,0);
            if(num == -1){
                lcd.printf("total:%7d",total_kumi_cnt);
                lcd.locate(0,1);
                lcd.printf("     :%7d",servo_histry);
            }else{
                lcd.printf("%02d:%5d|%7d",num*2+1,servo_n_histry[0][num*2  ],servo_b_histry[num*2  ] + servo_n_histry[0][num*2  ]);
                lcd.locate(0,1);
                lcd.printf("%02d:%5d|%7d",num*2+2,servo_n_histry[0][num*2+1],servo_b_histry[num*2+1] + servo_n_histry[0][num*2+1]);
            }
            wait(0.2);
        }
        btn.old_btn();  //スイッチ更新
    }
}


void result_LCD_servo2(){
        
    file.read_all_file();               //FileRead
    file.read_hservo();
    product = file.get_product();           //パラメータ(設定情報)を取得
    int num = -1;
    int min = num;
    int max = product.head/2-1; 
    int nums = 0;
    int min2 = -1;
    int max2 = product.head; 
    bool crear_end = false; 
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("time :%2d:%02d",servo_h_time[0],servo_h_time[1]);
    lcd.locate(0,1);
    lcd.printf("count:%7d",servo_histry);
    wait(0.2);
    wait(0.2);
    
    for(int a=0 ; a < product.head ; a++){                                
        if( servo_n_histry[1][a] == 1){
            servo_n_histry[0][a] += 1;
            servo_n_histry[1][a] =  0;
        }
    }
    while(btn.esc_press()==true){wait(0.2);}  //escを離すまで待つ
    while(btn.get_btn() != BTN_ESC) {
        btn.new_btn();              //スイッチ(New)取得
        if(btn.esc()) return;       //処理終了。値の変更なし
        if(btn.ent()){
            nums = num;
            wait(0.2);
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("select erase num");
            lcd.locate(3,1);
            if(nums == -1){
                lcd.printf("time&count?  ");
            }else if(nums == product.head){
                lcd.printf("all?  ");
            }else{
                lcd.printf("%2d ?  ",nums+1);
            }
            while(btn.ent_press()==true){wait(0.2);}  //escを離すまで待つ
            
            btn.old_btn();  //スイッチ更新
            btn.new_btn();              //スイッチ(New)取得
            btn.end_force();
            while(btn.get_btn() != BTN_ESC && crear_end == false) {
                btn.puls_or_minus(&nums);
                if(     nums > max2){nums=max2; se.Touch_01();se.Touch_01();while(btn.any_press()==true ){wait(0.2);}}
                else if(nums < min2){nums=min2; se.Touch_01();se.Touch_01();while(btn.any_press()==true ){wait(0.2);}}
                
                if(btn.push_sound()) {se.Touch_01();} //ボタン操作時の効果音
                if(btn.get_btn()) {
                    lcd.cls();
                    lcd.locate(0,0);
                    lcd.printf("select erase num");
                    lcd.locate(3,1);
                    if(nums == -1){
                        lcd.printf("time&count?  ");
                    }else if(nums == product.head){
                        lcd.printf("all?  ");
                    }else{
                        lcd.printf("%2d ?  ",nums+1);
                    }
                }
                if(btn.ent_press()==true){
                    if(nums == -1){
                        servo_h_time[0] = 0;
                        servo_h_time[1] = 0;
                        servo_histry  =0;
                        
                    }else if(nums == product.head){
                        for(int a=0 ; a < product.head ; a++){
                            servo_b_histry[a]    = 0;
                            servo_n_histry[0][a] = 0;
                            servo_n_histry[1][a] = 0;
                        }
                    }else{
                        servo_b_histry[nums]    = 0;
                        servo_n_histry[0][nums] = 0;
                        servo_n_histry[1][nums] = 0;
                    }
                    file.save_hservo();
                    se.chime_01();
                    while(btn.ent_press()==true){wait(0.2);}  //escを離すまで待つ
                    crear_end = true;
                }
                btn.old_btn();  //スイッチ更新
                btn.new_btn();              //スイッチ(New)取得
            }
            crear_end = false;
        }
        while(btn.esc_press()==true){wait(0.2);}  //escを離すまで待つ
        btn.puls_or_minus(&num);
        if(btn.push_sound()) {se.Touch_01();} //ボタン操作時の効果音
            
        if(     num > max){num=max; se.Touch_01();se.Touch_01();while(btn.any_press()==true ){wait(0.2);}}
        else if(num < min){num=min; se.Touch_01();se.Touch_01();while(btn.any_press()==true ){wait(0.2);}}
        
        if(btn.get_btn()) {
            if(num == -1){
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("time :%2d:%02d",servo_h_time[0],servo_h_time[1]);
                lcd.locate(0,1);
                lcd.printf("count:%7d",servo_histry);
                wait(0.2);
            }else{
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("%02dH:%8d",num*2+1,servo_b_histry[num*2  ] + servo_n_histry[0][num*2  ]);
                lcd.locate(0,1);
                lcd.printf("%02dH:%8d",num*2+2,servo_b_histry[num*2+1] + servo_n_histry[0][num*2+1]);
                wait(0.2);
            }
        }
        btn.old_btn();  //スイッチ更新
    }
}
