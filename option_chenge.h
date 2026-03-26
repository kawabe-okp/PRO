void driving_option();
bool select_driveoption_use(int m);
int selectdriving_option(int mode_op);


void driving_option(){
    int mode_op=0;
    bool ch_back = false;
    mode_op = selectdriving_option(mode_op);    //実行するメニューを選択
    printf("aaa%2d\r\n",mode_op);
    switch(mode_op) {
        case OP_2ND:
            ch_back = set_basic_setting();
            break;
        case OP_ZENNOSE:
        case OP_ATONOSE:
            ch_back = set_option_setting(mode_op);
            break;
        case OP_STOP_CELLS:
            ch_back = set_stop_cells();
            break;
        case OP_USB_EN:
        case OP_CHAR_ROTATE:
        case OP_RANK_OVER:
        case OP_KOKUTI:
        case OP_KIKAKUGAI_V:
        case OP_RANKSPEED:
        case OP_BTN_SETTING:
        case OP_BEGINNER:
        case OP_SEL_BEEP:
        case OP_NO_COMBI_V:
        case OP_SECONDMBI_V:
        case OP_NUMBER_COLOR:
        case OP_O_count_lim:
        case OP_O_lit_count_lim:
        case OP_NAGAMONO_HOSEI:
        case OP_WAITING_LIMMIT:
        case OP_SELAUTOZERO:
        case OP_KEKKA_HYOUJI:
        //case OP_ALART_AUTOZERO:
        case OP_FIX_VALUE:
            //printf("AAAA\r\n");
            ch_back = set_option_setting(mode_op);
            break;
        case OP_NAGAMONO_HOSEI2:
            ch_back = hosei_for_preset(file.get_preset());
            //ch_back = set_nagamono_hosei();
            break;
        case OP_BUFFER:
            set_sercret_parameter_one(P_buffer,1); //保存指令付
            ch_back = true;
            break;
        case OP_TIMER: //時計設定
            time_set();
            ch_back = true;
            break;
        case OP_SERVO1 : //
            ch_back = servo_setting1();
            break;
        case OP_SERVO2 : //
            ch_back = servo_setting2();
            break;
        case OP_SERVO3 : //
            ch_back = servo_setting3();
            break;
        case OP_SERVO4 : //
            ch_back = servo_setting4();
            break;
        case OP_ZERO_CHECK:
            ch_back = zerocheck_set();
            break;
        case OP_JUURYO_CHECK:
            katamen=0; //グローバル変数へ値保存
            load.katamen = 0;
            set_stopcell();   //不使用セル(＋12Headの対面点灯を組み合わせる)
            //LED点灯箇所のみ重量を計る
            auto_zero_off = true;
            start_keiryo(0);
            auto_zero_off = false;
            ch_back = true;
            break;
        case OP_KUMI_COUNT:
            set_count(&cnt);
            ch_back = true;
            break;
        case OP_TL_SPMODE: //
            set_preset2(file.get_preset(),TL_SPMODE);
            ch_back = true;
            break;
        case OP_TL_HYOUJI: //
            set_preset2(file.get_preset(),TL_HYOUJI);
            ch_back = true;
            break;
        case OP_TL_KUMI_FLAG: //
            set_preset2(file.get_preset(),TL_KUMI_FLAG);
            ch_back = true;
            break;
        case OP_TL_TARGET: //
            set_preset2(file.get_preset(),TL_TARGET);
            ch_back = true;
            break;
        case OP_TL_KURI_FLAG:
            set_preset2(file.get_preset(),TL_KURI_FLAG);
            ch_back = true;
            
            if(load.kuri_flag==0){
                KURI2nd = param.KURI;
            }else if(load.kuri_flag==1){
                KURI2nd = 9000;
            }else if(load.kuri_flag==2){
                KURI2nd = 6000;
            }else if(load.kuri_flag==3){
                KURI2nd = 3000;
            }else{
                KURI2nd = param.KURI;
            }
            break;
        case 99:
            ch_back = true;
            break;
        default:
            ch_back = false;
            break;
            
    }
    if(ch_back == false) {
        ch_back = true;
    } else {
        mode_op=0;
    }
    now_TC_mode = 1;
}
                
                

bool select_driveoption_use(int m){
    /*
    if(load.kumi_flag ==  ){
        switch(m) {
            case(OP_2ND)            :return false;
            case(OP_ZENNOSE)        :return false;
            case(OP_ATONOSE)        :return false;
            case(OP_STOP_CELLS)     :return false;
            case(OP_USB_EN)         :return false;
            case(OP_CHAR_ROTATE)    :return false;
            case(OP_RANK_OVER)      :return false;
            case(OP_KOKUTI)         :return false;
            case(OP_KIKAKUGAI_V)    :return false;
            case(OP_RANKSPEED)      :return false;
            case(OP_BTN_SETTING)    :return false;
            case(OP_BEGINNER)       :return false;
            case(OP_SEL_BEEP)       :return false;
            case(OP_BUFFER)         :return false;
            case(OP_TIMER)          :return false;
            case(OP_NO_COMBI_V)     :return false;
            case(OP_SECONDMBI_V)    :return false;
            case(OP_NUMBER_COLOR)   :return false;
            case(OP_O_count_lim)    :return false;
            case(OP_O_lit_count_lim):return false;
            //case(OP_SEIRITU_SUUJI)  :return false;
            //case(OP_KEIRYOU_KEKKA)  :return false;
            case(OP_END)            :return false;
            case(OP_JUURYO_CHECK)   :return false;
            case(OP_TL_SPMODE)      :return false;
            case(OP_TL_HYOUJI)      :return false;
            case(OP_TL_KUMI_FLAG)   :return false;
            default                 :return false;
        }
    }
    */
    if(load.kumi_flag == KUMI_NONE  || 
       load.kumi_flag == KUMI_LIMIT ){
        switch(m) {
            case(OP_2ND)            :return true ;
            case(OP_ZENNOSE)        :return false ;
            case(OP_ATONOSE)        :return true ;
            case(OP_STOP_CELLS)     :return false;
            case(OP_USB_EN)         :if(product.use_usb == 1){  return true ;}
                                     else{                      return false;}
            case(OP_CHAR_ROTATE)    :return false ;
            case(OP_RANK_OVER)      :return false;
            case(OP_KOKUTI)         :return false ;
            case(OP_KIKAKUGAI_V)    :return false ;
            case(OP_RANKSPEED)      :return false;
            case(OP_BTN_SETTING)    :return true;
            case(OP_BEGINNER)       :return false ;
            case(OP_SEL_BEEP)       :return false ;
            case(OP_BUFFER)         :return true;
            case(OP_TIMER)          :return false;
            case(OP_NO_COMBI_V)     :return true ;
            case(OP_SECONDMBI_V)    :return false;
            case(OP_NUMBER_COLOR)   :return false;
            case(OP_O_count_lim)    :return false;
            case(OP_O_lit_count_lim):return false;
            //case(OP_SEIRITU_SUUJI)  :return true ;
            //case(OP_KEIRYOU_KEKKA)  :return true ;
            case(OP_NAGAMONO_HOSEI)     :return true;
            case(OP_NAGAMONO_HOSEI2)     :return true;
            case(OP_SERVO1)     :return true;
            case(OP_SERVO2)     :return true;
            case(OP_SERVO3)     :return true;
            case(OP_SERVO4)     :return true;
            case(OP_ZERO_CHECK)     :return false;
            case(OP_WAITING_LIMMIT) :return true;
            case(OP_SELAUTOZERO)    :return true;
            case(OP_FIX_VALUE)      :return true;
            
            case(OP_END)            :return false;
            
            case(OP_KUMI_COUNT)     :return true ;
            case(OP_JUURYO_CHECK)   :return true ;
            case(OP_TL_KUMI_FLAG)   :return true ;
            case(OP_TL_SPMODE)      :return true ;
            case(OP_TL_HYOUJI)      :return true ;
            
            case(OP_TL_TARGET)      :return true ;
            case(OP_TL_KURI_FLAG)   :return true ;
            
            
            //case(OP_COMBI_ONSEI)     :if(load.spMode == 1){ return true ;}
            //                         else                { return false;}
            
            default                 :return false;
        }
    }
    else if(load.kumi_flag == KUMI_OYAKO || 
            load.kumi_flag == KUMI_OYAKO_HALF ){
        switch(m) {
            case(OP_2ND)            :return true ;
            case(OP_ZENNOSE)        :return true ;
            case(OP_ATONOSE)        :return true ;
            case(OP_STOP_CELLS)     :return false;
            case(OP_USB_EN)         :if(product.use_usb == 1){  return true ;}
                                     else{                      return false;}
            case(OP_CHAR_ROTATE)    :return true ;
            case(OP_RANK_OVER)      :return false;
            case(OP_KOKUTI)         :return true ;
            case(OP_KIKAKUGAI_V)    :return true ;
            case(OP_RANKSPEED)      :return false;
            case(OP_BTN_SETTING)    :return false;
            case(OP_BEGINNER)       :return false;
            case(OP_SEL_BEEP)       :return false;
            case(OP_BUFFER)         :return false;
            case(OP_TIMER)          :return false;
            case(OP_NO_COMBI_V)     :return false;
            case(OP_SECONDMBI_V)    :return false;
            case(OP_NUMBER_COLOR)   :return false;
            case(OP_O_count_lim)    :return false;
            case(OP_O_lit_count_lim):return false;
            //case(OP_SEIRITU_SUUJI)  :return false;
            //case(OP_KEIRYOU_KEKKA)  :return false;
            //case(OP_SELAUTOZERO)    :return true;
            case(OP_KEKKA_HYOUJI)   :return true;
            case(OP_ALART_AUTOZERO) :return false;
            case(OP_FIX_VALUE)      :return true;
            case(OP_END)            :return false;
            
            case(OP_KUMI_COUNT)     :return true ;
            case(OP_JUURYO_CHECK)   :return true ;
            case(OP_TL_KUMI_FLAG)   :return false ;
            case(OP_TL_SPMODE)      :return false ;
            case(OP_TL_HYOUJI)      :return false ;
            
            case(OP_TL_TARGET)      :return false ;
            case(OP_TL_KURI_FLAG)   :return true ;
                                     
            default                 :return false;
        }
    }
    else if(load.kumi_flag == OP_RANK_OVER){
        switch(m) {
            case(OP_2ND)            :return false;
            case(OP_ZENNOSE)        :return false;
            case(OP_ATONOSE)        :return false;
            case(OP_STOP_CELLS)     :return false;
            case(OP_USB_EN)         :if(product.matrix_mode==MATRIX_ON &&
                                        product.use_usb == 1){  return true ;}
                                     else{                      return false;}
            case(OP_CHAR_ROTATE)    :if(product.matrix_mode==MATRIX_ON){  return true ;}
                                     else{                                return false;}
            case(OP_RANK_OVER)      :if(product.matrix_mode!=MATRIX_ON){  return true ;}
                                     else{                                return false;}
            case(OP_KOKUTI)         :return false;
            case(OP_KIKAKUGAI_V)    :return false;
            case(OP_RANKSPEED)      :return false;
            case(OP_BTN_SETTING)    :return false;
            case(OP_BEGINNER)       :return false;
            case(OP_SEL_BEEP)       :return false;
            case(OP_BUFFER)         :return false;
            case(OP_TIMER)          :return false;
            case(OP_NO_COMBI_V)     :return false;
            case(OP_SECONDMBI_V)    :return false;
            case(OP_NUMBER_COLOR)   :return false;
            case(OP_O_count_lim)    :return false;
            case(OP_O_lit_count_lim):return false;
            //case(OP_SEIRITU_SUUJI)  :return false;
            //case(OP_KEIRYOU_KEKKA)  :return false;
            case(OP_END)            :return false;
            case(OP_JUURYO_CHECK)   :return true ;
            case(OP_TL_SPMODE)      :return false;
            case(OP_TL_HYOUJI)      :return false;
            case(OP_TL_KUMI_FLAG)   :return false;
            //case(OP_COMBI_ONSEI)     :return false;
            default                 :return false;
        }
    }
    else if(load.kumi_flag == KUMI_KOSUU){
        switch(m) {
            case(OP_2ND)            :return false;
            case(OP_ZENNOSE)        :return false;
            case(OP_ATONOSE)        :return false;
            case(OP_STOP_CELLS)     :return false;
            case(OP_USB_EN)         :if(product.use_usb==1){
                                        return true;
                                    }else{
                                        return false;
                                    }
            case(OP_CHAR_ROTATE)    :return true;
            case(OP_RANK_OVER)      :return false;
            case(OP_KOKUTI)         :return false;
            case(OP_KIKAKUGAI_V)    :return false;
            case(OP_RANKSPEED)      :return false;
            case(OP_BTN_SETTING)    :return false;
            case(OP_BEGINNER)       :return false;
            case(OP_SEL_BEEP)       :return false;
            case(OP_BUFFER)         :return false;
            case(OP_TIMER)          :return false;
            case(OP_NO_COMBI_V)     :return false;
            case(OP_SECONDMBI_V)    :return false;
            case(OP_NUMBER_COLOR)   :return false;
            case(OP_O_count_lim)    :return false;
            case(OP_O_lit_count_lim):return false;
            case(OP_SELAUTOZERO)    :return true;
            //case(OP_SEIRITU_SUUJI)  :return false;
            //case(OP_KEIRYOU_KEKKA)  :return false;
            case(OP_END)            :return false;
            
            case(OP_KUMI_COUNT)     :return true ;
            case(OP_JUURYO_CHECK)   :return true ;
            case(OP_TL_KUMI_FLAG)   :return false ;
            case(OP_TL_SPMODE)      :return false ;
            case(OP_TL_HYOUJI)      :return false ;
            
            case(OP_TL_TARGET)      :return false ;
            case(OP_TL_KURI_FLAG)   :return false ;
            default                 :return false;
        }
    }
    else if(load.kumi_flag == KUMI_JUURYO){
        switch(m) {
            case(OP_2ND)            :return false;
            case(OP_ZENNOSE)        :return false;
            case(OP_ATONOSE)        :return false;
            case(OP_STOP_CELLS)     :return false;
            case(OP_USB_EN)         :if(product.use_usb==1){
                                        return true;
                                    }else{
                                        return false;
                                    }
            case(OP_CHAR_ROTATE)    :return false;
            case(OP_RANK_OVER)      :return false;
            case(OP_KOKUTI)         :return false;
            case(OP_KIKAKUGAI_V)    :return false;
            case(OP_RANKSPEED)      :return false;
            case(OP_BTN_SETTING)    :return false;
            case(OP_BEGINNER)       :return false;
            case(OP_SEL_BEEP)       :return false;
            case(OP_BUFFER)         :return false;
            case(OP_TIMER)          :return false;
            case(OP_NO_COMBI_V)     :return false;
            case(OP_SECONDMBI_V)    :return false;
            case(OP_NUMBER_COLOR)   :return false;
            case(OP_O_count_lim)    :return true ;
            case(OP_O_lit_count_lim):return true ;
            case(OP_SELAUTOZERO)    :return true;
            //case(OP_SEIRITU_SUUJI)  :return false;
            //case(OP_KEIRYOU_KEKKA)  :return false;
            case(OP_END)            :return false;
            
            case(OP_KUMI_COUNT)     :return true ;
            case(OP_JUURYO_CHECK)   :return true ;
            case(OP_TL_KUMI_FLAG)   :return false ;
            case(OP_TL_SPMODE)      :return false ;
            case(OP_TL_HYOUJI)      :return false ;
            
            case(OP_TL_TARGET)      :return false ;
            case(OP_TL_KURI_FLAG)   :return false ;
            default                 :return false;
        }
    }
    else if(
            load.kumi_flag == KUMI_PACK     || 
            load.kumi_flag == KUMI_PACK2    || 
            load.kumi_flag == KUMI_ZANCOM   ||
            load.kumi_flag == KUMI_ZANCOM2    ){
        switch(m) {
            case(OP_2ND)            :return false;
            case(OP_ZENNOSE)        :return false;
            case(OP_ATONOSE)        :return false;
            case(OP_STOP_CELLS)     :return false;
            case(OP_USB_EN)         :return false;
            case(OP_CHAR_ROTATE)    :return true ;
            case(OP_RANK_OVER)      :return false;
            case(OP_KOKUTI)         :return false;
            case(OP_KIKAKUGAI_V)    :return true ;
            case(OP_RANKSPEED)      :return false;
            case(OP_BTN_SETTING)    :return false;
            case(OP_BEGINNER)       :return false;
            case(OP_SEL_BEEP)       :return false;
            case(OP_BUFFER)         :return false;
            case(OP_TIMER)          :return false;
            case(OP_NO_COMBI_V)     :return false;
            case(OP_SECONDMBI_V)    :return false;
            case(OP_NUMBER_COLOR)   :return false;
            case(OP_O_count_lim)    :return false;
            case(OP_O_lit_count_lim):return false;
            //case(OP_SEIRITU_SUUJI)  :return false;
            //case(OP_KEIRYOU_KEKKA)  :return false;
            case(OP_END)            :return false;
            case(OP_JUURYO_CHECK)   :return false;
            case(OP_TL_SPMODE)      :return false;
            case(OP_TL_HYOUJI)      :return false;
            case(OP_TL_KUMI_FLAG)   :return false;
            //case(OP_COMBI_ONSEI)     :return false;
            default                 :return false;
        }
    }
    
    else{
        return false;
    }
    
}

int selectdriving_option(int mode_op)         //17/09/21
{
    //int m=0;                        //選択しているメニュー番号
    int m=mode_op;                        //選択しているメニュー番号
    int disMenu = OP_END_END - 1;     //メニュー数
    
    if(now_TC_mode==0){
        disMenu = OP_END-1;
    }
    
    const char name[]= {0xB5,0xCC,0xDF,0xBC,0xAE,0xDD,0x00};   //ｵﾌﾟｼｮﾝ
    int jump_num=0;         //不使用でジャンプした場合に数字部の数を減らす
    if(gLevel == 0) {
        led.REGIS(ALL_HEAD^stop_cells2);
        led.RedOff();
    } else {
        led.REGIS(0);
        led.REGIS(ALL_HEAD^stop_cells2);
        led.RedOn();
    }
    //printf("A1\r\n");
    if(now_TC_mode == 1){
        now_TC_mode = 2;
    }
    //printf("A2\r\n");
    //while(select_driveoption_use(m)==false){
    while(select_option_use(m)==false){
        m+=1;
        if(m>=OP_END_END){
            m=0;
        }
        //printf("%2d\r\n",m);
    }

    btn.ini();
    lcd.blank(TOP);                     //組み合わせ内容表示分をクリア
    lcd.printf(name);
    jump_num=0;
    for(int j=0;j<=m;j++){
        if(select_option_use(j)==false){
            jump_num +=1;
        }
    }

    while(1) {
        //lcd表示変更する必要があるか判断
        if(btn.get_one_push()) {
            jump_num=0;
            for(int j=0;j<=m;j++){
                if(select_option_use(j)==false){
                    jump_num +=1;
                }
            }
            lcd.blank(1);           //2行目をクリア
            lcd.locate(0,1);
            lcd.printf("%d",(m - jump_num)+1);
            lcd.Option(m);
            //printf("%2d\r\n",m);

            
        }//if LCD

        btn.new_btn();              //スイッチ(New)取得
        //esc押下なら処理終了
        if(btn.esc()) {
            se.Touch_01();
            btn.old_btn();  //スイッチ更新
            return 99;
        }
        btn.next_or_back(&m,0, disMenu);
        while(select_option_use(m)==false) {
            btn.next_or_back(&m,0, disMenu);
        }
        jump_num=0;
        for(int j=0;j<=m;j++){
            if(select_option_use(j)==false){
                jump_num +=1;
            }
        }
        if(btn.push_sound() && !btn.esc()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        //Enterが押されたら移行するモード値を取得する
        if(btn.get_btn() == BTN_ENT) {
            btn.old_btn();  //スイッチ更新
            //printf("%2d\r\n",m);
            return m;    //選択されたメニューを返す
        }
        btn.old_btn();  //スイッチ更新
    }//while
}