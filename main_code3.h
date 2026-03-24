int select_keiryo_option(int option_one, int kumi_flg, int kumi_kousoku);
int selectMenu_option2(int option_one, int kumi_flg, int kumi_kousoku);
bool select_option_use2(int m, int kumi_flg, int kumi_kousoku);


int select_keiryo_option(int option_one, int kumi_flg, int kumi_kousoku){
    bool ch_back = false;
    while(ch_back == false){
        option_one = selectMenu_option2(option_one,kumi_flg,kumi_kousoku);    //実行するメニューを選択
        switch(option_one) {
            case OP_2ND:
                ch_back = set_basic_setting();
                break;
            case OP_ZENNOSE:
            case OP_ATONOSE:
                ch_back = set_option_setting(option_one);
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
                ch_back = set_option_setting(option_one);
                break;
            case OP_NAGAMONO_HOSEI2:
                //ch_back = set_nagamono_hosei();
                ch_back = hosei_for_preset(file.get_preset());
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
            case OP_BUFFER:
                set_sercret_parameter_one(P_buffer,1); //保存指令付
                ch_back = true;
                break;
            case OP_TIMER: //時計設定
                time_set();
                ch_back = true;
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
        while(btn.any_press()==true){
            wait(0.2);   
        }
        lcd.blank(LOW);
    }
}

int selectMenu_option2(int option_one, int kumi_flg, int kumi_kousoku)
{
    int m=option_one;                        //選択しているメニュー番号
    int disMenu = OP_END_END - 1;     //メニュー数
    const char name[]= {0xB5,0xCC,0xDF,0xBC,0xAE,0xDD,0x00};   //ｵﾌﾟｼｮﾝ
    int jump_num=0;         //不使用でジャンプした場合に数字部の数を減らす
    if(gLevel == 0) {
        led.REGIS(0xfff^stop_cells2);
        led.RedOff();
    } else {
        led.REGIS(0);
        led.REGIS(0xfff^stop_cells2);
        led.RedOn();
    }
    while(select_option_use2(m, kumi_flg, kumi_kousoku)==false){
        m+=1;
        if(m>=OP_END){
            m=0;
        }
    }
    btn.ini();
    lcd.blank(TOP);                     //組み合わせ内容表示分をクリア
    lcd.printf(name);
    jump_num=0;
    for(int j=0;j<=m;j++){
        if(select_option_use2(j, kumi_flg, kumi_kousoku)==false){
            jump_num +=1;
        }
    }
    while(1) {
        //lcd表示変更する必要があるか判断
        if(btn.get_one_push()) {
            jump_num=0;
            for(int j=0;j<=m;j++){
                if(select_option_use2(j, kumi_flg, kumi_kousoku)==false){
                    jump_num +=1;
                }
            }
            lcd.blank(1);           //2行目をクリア
            lcd.locate(0,1);
            lcd.printf("%d",(m - jump_num)+1);
            lcd.Option(m);
        }//if LCD

        btn.new_btn();              //スイッチ(New)取得
        //esc押下なら処理終了
        if(btn.esc()) {
            se.Touch_01();
            btn.old_btn();  //スイッチ更新
            return 99;
        }
        btn.next_or_back(&m,0, disMenu);
        while(select_option_use2(m, kumi_flg, kumi_kousoku)==false) {
            btn.next_or_back(&m,0, disMenu);
        }
        jump_num=0;
        for(int j=0;j<=m;j++){
            if(select_option_use2(j, kumi_flg, kumi_kousoku)==false){
                jump_num +=1;
            }
        }
        if(btn.push_sound() && !btn.esc()) {
            se.Touch_01();   //ボタン操作時の効果音
        }
        //Enterが押されたら移行するモード値を取得する
        if(btn.get_btn() == BTN_ENT) {
            btn.old_btn();  //スイッチ更新
            return m;    //選択されたメニューを返す
        }
        btn.old_btn();  //スイッチ更新
    }//while
}
bool select_option_use2(int m, int kumi_flg, int kumi_kousoku)
{
    /*
    if(load.kumi_flag == KUMI_NONE || load.kumi_flag == KUMI_LIMIT){
        switch(m) {
                case(OP_2ND):           return true;
                case(OP_ZENNOSE):       return false;
                case(OP_ATONOSE):       return false;
                case(OP_STOP_CELLS):    return false;
                case(OP_USB_EN): if(product.use_usb == 1){return true;
                                 }else{                   return false;}   
                case(OP_CHAR_ROTATE):   return false;
                case(OP_RANK_OVER):     return true;
                case(OP_KIKAKUGAI_V):   return true;
                case(OP_RANKSPEED):     return false;
                case(OP_BTN_SETTING):   return false;
                case(OP_BEGINNER):      return true;
                case(OP_SEL_BEEP):      return true;
                case(OP_BUFFER):        return false;
                case(OP_TIMER):         return false;
                case(OP_NO_COMBI_V):    return true;
                case(OP_SECONDMBI_V):   return true;
                case(OP_NUMBER_COLOR):  return false;
                case(OP_END):           return false;
                case(OP_JUURYO_CHECK):  return true;
                case(OP_TL_SPMODE):     return true;
                case(OP_TL_HYOUJI):     return true;
                case(OP_TL_KUMI_FLAG):  return true;
                case(OP_NAGAMONO_HOSEI):return true;
                default:                return false;
                
        }
    }else if(load.kumi_flag == KUMI_ZANCOM){
        switch(m) {
                case(OP_2ND):           return true;
                case(OP_ZENNOSE):       return false;
                case(OP_ATONOSE):       return false;
                case(OP_STOP_CELLS):    return false;
                case(OP_USB_EN): if(product.use_usb == 1){return true;
                                 }else{                   return false;}   
                case(OP_CHAR_ROTATE):   return true;
                case(OP_RANK_OVER):     return false;
                case(OP_KIKAKUGAI_V):   return true;
                case(OP_RANKSPEED):     return false;
                case(OP_BTN_SETTING):   return false;
                case(OP_BEGINNER):      return true;
                case(OP_SEL_BEEP):      return true;
                case(OP_BUFFER):        return false;
                case(OP_TIMER):         return false;
                case(OP_NO_COMBI_V):    return true;
                case(OP_SECONDMBI_V):   return true;
                case(OP_NUMBER_COLOR):  return false;
                case(OP_END):           return false;
                case(OP_JUURYO_CHECK):  return true;
                case(OP_TL_SPMODE):     return true;
                case(OP_TL_HYOUJI):     return true;
                default:                return false;
                
        }
    }else if(load.kumi_flag == KUMI_COMZAN){
        switch(m) {
                case(OP_2ND):           return true;
                case(OP_ZENNOSE):       return false;
                case(OP_ATONOSE):       return false;
                case(OP_STOP_CELLS):    return false;
                case(OP_USB_EN): if(product.use_usb == 1){return true;
                                 }else{                   return false;}   
                case(OP_CHAR_ROTATE):   return true;
                case(OP_RANK_OVER):     return false;
                case(OP_KIKAKUGAI_V):   return true;
                case(OP_RANKSPEED):     return false;
                case(OP_BTN_SETTING):   return false;
                case(OP_BEGINNER):      return true;
                case(OP_SEL_BEEP):      return true;
                case(OP_BUFFER):        return false;
                case(OP_TIMER):         return false;
                case(OP_NO_COMBI_V):    return true;
                case(OP_SECONDMBI_V):   return true;
                case(OP_NUMBER_COLOR):  return false;
                case(OP_END):           return false;
                case(OP_JUURYO_CHECK):  return true;
                case(OP_TL_SPMODE):     return false;
                case(OP_TL_HYOUJI):     return false;
                default:                return false;
                
        }
    }else{                              return false;
    }
    */
    if(m == OP_2ND              ){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_OYAKO_HALF){  return true;
        }else{
                                return false;
        }
    }else if(m == OP_ZENNOSE){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT){  return true;
        }else{
                                return false;
        }
    }else if(m == OP_ATONOSE){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_STOP_CELLS){
                                            return false;
    }else if(m == OP_USB_EN){
                                            return false;
    }else if(m == OP_CHAR_ROTATE){
        if(product.matrix_mode == 1 &&(
           load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_RANK || 
           load.kumi_flag == KUMI_KAIKYU || 
           load.kumi_flag == KUMI_KOSUU || 
           load.kumi_flag == KUMI_JUURYO || 
           load.kumi_flag == KUMI_KAISU1 || 
           load.kumi_flag == KUMI_KAISU2 || 
           load.kumi_flag == KUMI_PACK || 
           load.kumi_flag == KUMI_PACK2 || 
           load.kumi_flag == KUMI_PACK || 
           load.kumi_flag == KUMI_DENSO || 
           load.kumi_flag == KUMI_ZANCOM || 
           load.kumi_flag == KUMI_ZANCOM2 || 
           load.kumi_flag == KUMI_COMZAN)){ return true;
        }else{
                                            return false;
        }
    }else if(m == OP_RANK_OVER){
        if(product.matrix_mode == 1 &&
           load.kumi_flag == KUMI_RANK){    return true;
        }else{
                                            return false;
        }
    }else if(m == OP_KOKUTI){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_KIKAKUGAI_V){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_OYAKO_HALF || 
           load.kumi_flag == KUMI_TAIMEN || 
           load.kumi_flag == KUMI_KAISU1 || 
           load.kumi_flag == KUMI_KAISU2 || 
           load.kumi_flag == KUMI_PACK || 
           load.kumi_flag == KUMI_PACK2 || 
           load.kumi_flag == KUMI_KAISU1 ||
           load.kumi_flag == KUMI_ZANCOM ||
           load.kumi_flag == KUMI_ZANCOM2){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_RANKSPEED){
                                            return false;
    }else if(m == OP_BTN_SETTING){
                                            return false;
    }else if(m == OP_BEGINNER){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_KAIKYU || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_OYAKO_HALF || 
           load.kumi_flag == KUMI_KAISU1 || 
           load.kumi_flag == KUMI_KAISU2 || 
           load.kumi_flag == KUMI_PACK || 
           load.kumi_flag == KUMI_PACK2 || 
           load.kumi_flag == KUMI_DENSO || 
           load.kumi_flag == KUMI_ZANCOM || 
           load.kumi_flag == KUMI_ZANCOM2 || 
           load.kumi_flag == KUMI_COMZAN){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_SEL_BEEP){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_KAIKYU || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_OYAKO_HALF || 
           load.kumi_flag == KUMI_KAISU1 || 
           load.kumi_flag == KUMI_KAISU2 || 
           load.kumi_flag == KUMI_PACK || 
           load.kumi_flag == KUMI_PACK2 || 
           load.kumi_flag == KUMI_DENSO || 
           load.kumi_flag == KUMI_ZANCOM || 
           load.kumi_flag == KUMI_ZANCOM2 || 
           load.kumi_flag == KUMI_COMZAN){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_BUFFER){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_KAIKYU || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_OYAKO_HALF || 
           load.kumi_flag == KUMI_KAISU1 || 
           load.kumi_flag == KUMI_KAISU2 || 
           load.kumi_flag == KUMI_PACK || 
           load.kumi_flag == KUMI_PACK2 || 
           load.kumi_flag == KUMI_DENSO || 
           load.kumi_flag == KUMI_ZANCOM || 
           load.kumi_flag == KUMI_ZANCOM2 || 
           load.kumi_flag == KUMI_COMZAN){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_TIMER){
                                            return false;
    }else if(m == OP_NO_COMBI_V){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_KAIKYU || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_OYAKO_HALF || 
           load.kumi_flag == KUMI_KAISU1 || 
           load.kumi_flag == KUMI_KAISU2 || 
           load.kumi_flag == KUMI_PACK || 
           load.kumi_flag == KUMI_PACK2 || 
           load.kumi_flag == KUMI_DENSO || 
           load.kumi_flag == KUMI_ZANCOM || 
           load.kumi_flag == KUMI_ZANCOM2 || 
           load.kumi_flag == KUMI_COMZAN){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_SECONDMBI_V){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_KAIKYU || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_OYAKO_HALF || 
           load.kumi_flag == KUMI_KAISU1 || 
           load.kumi_flag == KUMI_KAISU2 || 
           load.kumi_flag == KUMI_PACK || 
           load.kumi_flag == KUMI_PACK2 || 
           load.kumi_flag == KUMI_DENSO || 
           load.kumi_flag == KUMI_ZANCOM || 
           load.kumi_flag == KUMI_ZANCOM2 || 
           load.kumi_flag == KUMI_COMZAN){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_NUMBER_COLOR){
        if(load.kumi_flag == KUMI_DENSO){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_O_count_lim){
        if(load.kumi_flag == KUMI_JUURYO){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_O_lit_count_lim){
        if(load.kumi_flag == KUMI_JUURYO){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_NAGAMONO_HOSEI){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_NAGAMONO_HOSEI2){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_SERVO1){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_KOSUU || 
           load.kumi_flag == KUMI_JUURYO || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_ZANCOM){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_SERVO2){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_KOSUU || 
           load.kumi_flag == KUMI_JUURYO || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_ZANCOM){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_SERVO3){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_KOSUU || 
           load.kumi_flag == KUMI_JUURYO || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_ZANCOM){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_END){
                                            return false;
    }else if(m == OP_JUURYO_CHECK){
                                            return true;
    }else if(m == OP_TL_SPMODE){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_KAIKYU || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_OYAKO_HALF || 
           load.kumi_flag == KUMI_DENSO){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_TL_HYOUJI){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT || 
           load.kumi_flag == KUMI_KAIKYU || 
           load.kumi_flag == KUMI_OYAKO || 
           load.kumi_flag == KUMI_OYAKO_HALF || 
           load.kumi_flag == KUMI_DENSO){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_TL_KUMI_FLAG){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_TL_TARGET){
                                            return true;
    }else if(m == OP_TL_KURI_FLAG){
        if(load.kumi_flag == KUMI_NONE || 
           load.kumi_flag == KUMI_LIMIT){   return true;
        }else{
                                            return false;
        }
    }else if(m == OP_END_END){
                                            return false;
    }
                                            return false;
}

