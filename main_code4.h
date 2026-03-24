combiData_t get_keiryo_location();
void Drive_keiryo()
{
    char saving[]= {0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,0x00}; //ﾎｿﾞﾝﾁｭｳ
    char disp1[]= {0xB9,0xB2,0xD8,0xAE,0xB3,' ' ,0xD3,0x2D,0xC4,0xDE,' ' ,' ' ,' ' ,' ' ,' ' ,0x00};       //ｹｲﾘｮｳ ﾓｰﾄﾞ
    char disp2[]= {0xBC,0xDE,0xAF,0xBA,0xB3,0xC1,0xD5,0xB3,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,' ' ,0x00};       //ｼﾞｯｺｳﾁｭｳ
    float baratsuki=param.BARATSUKI;//一時保存フォルダ
//    int btnset =0;//計量中の下ボタン押しで出てくる値保存
    param.BARATSUKI=param.BARATSUKI_R;
    param.ZEROAREA=param.ZEROAREA2;
    cel.set_ZEROAREA(param.ZEROAREA);
        lcd.blank(0);
        lcd.blank(1);
    for(int i = 0; i < product.head; i++) {
        nowrank[i]=99;
        totalweightN[i]=0;
        
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

    //led.RedOn();
    while(1) {
        //led.ledAllOff();            // LED消灯
        //led.RedOff();
        btn.new_keep_btn();               //スイッチ(New)取得
        //esc_swを押すと、Menu画面に戻る
        if(btn.esc()||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)) {
            se.Touch_01();
            comp_rank=0;
            //LCD表示
            lcd.blank(0);
            lcd.blank(1);
            lcd.locate(3,1);
            if(product.use_usb==1 && option.usb_EN==true) {
                lcd.printf(saving);
            }
            //戻りきるまで時間が掛かるので、マトリクス表示を消す
            for(int i = 0; i < product.head; i++) {
                iro[i]=C_NULL;
            }
            //個別データ書き込み(1個以上あったら)
            int total_qty=0;
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
                if(product.use_usb==1 && option.usb_EN==true) {
                    save_rankdata2(mweight.Rsort,load.target,load.jougen,ji,hun);
                }
                //save_rankdata(mweight.Rsort);
                if(product.use_usb==1 && option.usb_EN==true) {
                    USB_rankdata2(mweight.Rsort,load.target,load.jougen,ji,hun);
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
            cel.set_ZEROAREA(param.ZEROAREA);
            lcd.blank(0);
            lcd.blank(1);
            for(int i = 0; i < product.head; i++) {
                nowrank[i]=99;
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
            return;
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            auto_zero_point();
            lcd.blank(0);
            lcd.blank(1);
        }
        //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        else if(btn.up_press() == true) {
            /*se.chime_01();
            while(btn.any_press()==true){
                if(demo_flg==true&&btn.down_press() == true&&file.get_preset()==product.head*2){
                    demo_continue=1;
                    btn.force_ESC();
                    se.Touch_01();
                    continue;
                }
                wait(0.1);
            }
            if(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2){
                    demo_continue=1;
                    btn.force_ESC();
                    continue;
            }
            driving_option();*/
            lcd.blank(0);
            lcd.blank(1);
            /*
            //btn.old_btn();       //スイッチ更新
            //btn.new_btn();       //スイッチ更新
            //auto_zero_off = true;
            //start_keiryo(0);
            //auto_zero_off = false;
            btnset = Chenge_vaule_btnD(0);//ｶｳﾝﾄ以外orｹｲﾘｮｳﾓｰﾄﾞ
            if(btnset==1) {
                auto_zero_off = true;
                start_keiryo(0);
                auto_zero_off = false;
            }else if(btnset==2){
                set_sercret_parameter_one(P_KURI_J,1);
                //set_sercret_parameter_one(P_END,1); //保存指令付
            }else if(btnset==3){
                set_sercret_parameter_one(P_ZEROAREA2,1);
            }else if(btnset==4){
                set_sercret_parameter_one(P_bufferT,1);
                
                
                //set_sercret_parameter_one(P_END,1); //保存指令付
            }
            
            if(product.head>1) {
                lcd.settings_rank2(&load);            //設定内容を表示
            } else {
                lcd.blank(0);
                lcd.blank(1);
                lcd.bigNumber(cel.getWeight(0));
            }
        */
        }
        //ﾓｼﾞ回転ON/OFF変更
        else if(btn.get_btn() == BTN_DOWN ) {
            se.Touch_01();
            //if(product.head >= 8) {
            if(product.Array == 0) {
                option.c_rotate =(option.c_rotate +1)%3;
            } else {
                option.c_rotate =(option.c_rotate +1)%2;
            }
        }

        btn.old_btn();       //スイッチ更新
        lcd.locate(0,0);
        lcd.printf(disp1);
        lcd.locate(0,1);
        lcd.printf(disp2);
        if(product.head!=1){
            for(int i = 0; i < product.head; i++) {
                if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
                    cel.set_head(i);
                    cel.setAin_zero(i);
                } else {
                    //cel.set_head(i);
                    //cel.setAinSpeed(200);
                    /**//*
                    if(cel.getWeight(i)<80) {
                        if(param.BARATSUKI_GRAM<cel.getWeight(i)*0.015) {
                            param.BARATSUKI = cel.getWeight(i)*0.015;
                        } else {
                            param.BARATSUKI = param.BARATSUKI_GRAM;
                        }
                    }else if(((load.target*0.98)<=cel.getWeight(i)) && (cel.getWeight(i)<=(load.target*1.02)) ||
                                 (((load.target + load.jougen)*0.98)<=cel.getWeight(i)) && (cel.getWeight(i)<=((load.target + load.jougen)*1.02)) ) {
                        if(param.BARATSUKI_GRAM<cel.getWeight(i)*0.005) {
                            param.BARATSUKI = cel.getWeight(i)*0.005;
                        } else {
                            param.BARATSUKI = param.BARATSUKI_GRAM;
                        }
                    }else {
                        if(param.BARATSUKI_GRAM<cel.getWeight(i)*0.01) {
                            param.BARATSUKI = cel.getWeight(i)*0.01;
                        } else {
                            param.BARATSUKI = param.BARATSUKI_GRAM;
                        }
                    }*/
                    //if(product.head>1){
                    //    param.BARATSUKI=param.BARATSUKI_R;
                    //}
                    cel.set_BARATSUKI(param.BARATSUKI);
                    
                    if(product.head!=1){
                        setHeadVolt(i, param.KURI_J);
                    }
                    //printf("%8.3f\r\n",param.BARATSUKI);
                    //printf("gw%5.1f %2d  ",cel.getWeight(i),nowrank[i]);
                    if(nowrank[i]!=99) {
                        //if(product.head==1){
                        //    setHeadVolt(i, 3000);
                        //}else{
                            //setHeadVolt(i, param.KURI);
                            setHeadVolt(i, param.KURI_J);
                            /*if(nowweightVale[i] - cel.anteiWeight(i)>1 && nowweightVale[i] - cel.anteiWeight(i)<-1){
                                setHeadVolt(i, param.KURI);
                            }*/
                        //}
                    } else if(cel.getWeight(i)>param.CUT) {
                        //if(product.head==1){
                        //    setHeadVolt(i, 3000);
                        //}else{
                            setHeadVolt(i, 500);
                        //}
                        //setHeadVolt(i, (1000));
                    } else{
                        //if(product.head==1){
                        //    setHeadVolt(i, 500);
                        //}
                    }
                    //pc.printf("%d %08.2f %08.2f %08.2f  %2d  %08.2f  %0.8f\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.anteiWeight_rank3(i),nowrank[i],param.BARATSUKI,cel.getAin());
                }
            }
        }else{
            setHeadVolt(0, param.KURI);
        }

        //********     デバッグ用ここから
        if( product.productType >= STANDARD_D) {
            for(int i = 0; i < product.head; i++) {
                //temp = cel.temperature();   //温度を取得する
                //for(int i = 0; i < product.head; i++){
                cel.set_head(i);
                //    pc.printf("%d %08.2f %08.2f %08.2f  %2d\r\n", i+1,cel.getWeight(i), cel.get_HWeight(i),cel.anteiWeight_rank3(i),nowrank[i]);
                //pc.printf("%d %08.2f %08.2f %08.2f %2d %2d %08.2f\r\n", i+1, cel.getWeight(i), cel.get_HWeight(i), cel.anteiWeight_rank3(i),nowrank[i],iro[i],param.BARATSUKI);
                pc.printf("%d %08.2f %08.2f %2d %2d %08.2f\r\n", i+1, cel.getWeight(i), cel.get_HWeight(i),nowrank[i],iro[i],param.BARATSUKI);
                //}
                if(i == product.head - 1) {
                    //printf("%d %f C\r\n",14, temp);

                    pc.printf("========================\r\n");
                }
            }
        }
        //********デバッグ用ここまで
        //組み合わせ確認,最適な組み合わせ候補を抽出
        get_keiryo_location();

        //組み合わせに応じたLED点灯,総重量表示
        //show_rank2(&resultCombi);

        if(product.head>1) {
            //show_check_juuryo(&resultCombi);
        }
    }//while
    //file.read_param();          //設定値
    //cel.set_BARATSUKI(param.BARATSUKI);
    //cel.set_ZEROAREA(param.ZEROAREA);
}
combiData_t get_keiryo_location()
{
    combiData_t com = {0};
    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)
    float *weight = new float[product.head];
    //安定している重量を取得
    for(int i = 0; i < product.head; i++) {
        if(product.head<=13) {
            //if(product.head<=6 || (i<=5 && i % 2 == 0) || (i>=6 && i % 2 == 1)){
            //weight[i] = cel.anteiWeight(i);
            weight[i] = int(cel.getWeight(i)+0.5-param.bufferT);
            //printf("%3d %3d %7.1f %7.1f\r\n",recrank[i],iro[i],weight[i],int(cel.getWeight(i)) );
            if(weight[i] > 0) {
                totalweightN[i]=weight[i];
            } else if(weight[i] == 0 && (int(cel.getWeight(i)) < param.CUT+3 && oldweightVale[i]< param.CUT+3) && ((product.head !=1 && recrank[i]>0) || (product.head ==1 && recrank[i]>=0)) && recrank[i]<8) { //安定が無くなって、現在の値が0付近になったら
                //printf("K");
                if(i<=2) {
                    totalweight0103[recrank[i]] += totalweightN[i];
                    totalranker0103[recrank[i]] += 1;
                } else if(i<=5) {
                    totalweight0406[recrank[i]] += totalweightN[i];
                    totalranker0406[recrank[i]] += 1;
                } else if(i<=8) {
                    totalweight0709[recrank[i]] += totalweightN[i];
                    totalranker0709[recrank[i]] += 1;
                } else if(i<=(product.head - 1)) {
                    totalweight1012[recrank[i]] += totalweightN[i];
                    totalranker1012[recrank[i]] += 1;
                }
                oneshoot_OK=false;
                totalweightN[i] = 0;
                recrank[i]      = 0;
            }
            //if((weight[i] == 0 && int(cel.getWeight(i)) > param.CUT && int(cel.getWeight(i)) <= param.CUT)||cel.anteiWeight_rank(i)>=param.CUT) {
            if((weight[i] == 0 && int(cel.getWeight(i)) > param.CUT)){// && int(cel.getWeight(i)) <= param.CUT)||cel.anteiWeight_rank(i)>=param.CUT) {
                weight[i] = cel.anteiWeight(i);
            }
            //printf("g:%5.1f a:%5.1f w:%5.1f  wo:%5.1f\r\n",int(cel.getWeight(i)),cel.anteiWeight(i),weight[i],wait_old[i]);
            //printf("g:%5.1f a:%5.1f w:%5.1f  wo:%5.1f",int(cel.getWeight(i)),cel.anteiWeight(i),weight[i],wait_old[i]);
            //printf(" %3d %3d d:%3d ",int(weight[i]),int(wait_old[i]),(int(weight[i])-int(wait_old[i])));
  //          if(((int(weight[i])-int(wait_old[i]))<0) || ((int(weight[i])-int(wait_old[i]))>1)){
  //              wait_old[i] = weight[i];
  //          }else{
  //              weight[i] = wait_old[i];
  //          }
            wait_old[i] = weight[i];
            
            //printf("  w:%5.1f\r\n",weight[i]);
            
            //printf("%5.1f  %5.1f  ",int(cel.getWeight(i)),cel.anteiWeight_rank(i));
            //printf("%3d  ",int(weight[i]));
            //if(i==0){
            //    printf("%d %6.3f %6.3f  %6.3f  %d\r\n", i+1,cel.anteiWeight(i),cel.anteiWeight(i), weight[i],iro[i]);
            //}
            //if(weight[i]==0){
            //    weight[i] = cel.anteiWeight(i);
            //}
            int data_sa =oldweightVale[i] - nowweightVale[i];
            if(data_sa < -0.3 || data_sa > 0.4) {

            } else {
                nowweightVale[i] = oldweightVale[i];
                //weight[i] = oldweightVale[i];
            }
            oldweightVale[i] = nowweightVale[i];
        } else {
            iro[i] = C_BATU;
            //iro[i] = CR_1+i;
        }
    }
    //ヘッド数分の重量確認
    for(int i = 0; i < product.head; i++) {
           // printf("%3d  ",int(weight[i]));
        if(product.head<=13) {
            //if(product.head<=6 || (i<=5 && i % 2 == 0) || (i>=6 && i % 2 == 1)){
            if( int(cel.getWeight(i)) < -(param.CUT*2)) {//上に押されすぎたら、一旦0点調整してもらう
            } else {
                headqty=1;
                if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                    if(product.head==1) {
                        totalcheckwei[i]=weight[i];
                        if(disp_wait!=weight[i]){
                            lcd.bigNumber(weight[i]);
                            disp_wait = weight[i];
                        }
                    }
                }
                headqty=0;
            }
            //printf("w %5.1f\r\n",weight[i]);
            if(/*cel.anteiWeight_rank(i)==0 || */int(cel.getWeight(i)) <= param.CUT){
                if(int(cel.getWeight(i)) < 0){
                    iro[i]=C_NUSE;
                }else{
                    iro[i]= CR_00+(int(weight[i])) /*+ 1*/;
                }
                nowrank[i]=99;
                //printf("w1 %5d\r\n",iro[i]);
            }else if(weight[i]!=0) {
                //一番最適な重量を取得する
                if((int(weight[i]) > -param.CUT) && (int(weight[i]) <= param.CUT)) {
                    //値を取得
                    //com.kumi += (1 << i);
                    //com.total += weight[i];
                    if(product.head!=1) {
                        iro[i]=PENTA_R;
                    } else {
                        headqty=1;
                        if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                            if(product.head==1) {
                                totalcheckwei[i]=weight[i];
                                if(disp_wait!=weight[i]){
                                    lcd.bigNumber(weight[i]);
                                    disp_wait = weight[i];
                                }
                            }
                        }
                        headqty=0;
                        //iro[i]=C_NULL;
                        //nowrank[i]=1;
                    }
                    nowrank[i]=99;
                //} else if(int(weight[i]) > param.CUT){
                } else if(int(weight[i]) > param.CUT){
                    //iro[i]=C_NUSE;
                    nowrank[i]=1;
                    headqty=1;
                    if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                        if(product.head==1) {
                            totalcheckwei[i]=weight[i];
                            if(disp_wait!=weight[i]){
                                lcd.bigNumber(weight[i]);
                                disp_wait = weight[i];
                            }
                        }
                    }
                    headqty=0;
                    if(product.head==1) {
                        oneshoot_OK=false;
                    }
                    if(int(weight[i])<300){
                        //iro[i]= CG_0+(load.target-weight[i]) + 1;
                        
                        iro[i]= CR_00+(int(weight[i])) /*+ 1*/;
                        
                        //printf("w1 %5d\r\n",iro[i]);
                    
                    }else if(int(weight[i])<400){
                            iro[i]= C_NUSE_1D;
                    }else{
                            iro[i]= C_NUSE_2D;
                    }
                } else {
                    iro[i]=C_BATU;
                }
            } 
        }
    }//loop終了

    delete[] weight;
    return com;

}
