



void drive_Rank_Sorting_d()
{
    char saving[2][9]= {{0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,' ' ,0x00}      //ﾎｿﾞﾝﾁｭｳ
                   ,{0xCE,0xBF,0xDE,0xDD,0xBC,0xCF,0xBC,0xC0,0x00}};      //ﾎｿﾞﾝｼﾏｼﾀ
    combiData_t resultCombi;        //組み合わせ結果(最小値取得用)
    //combiData_t com = {0};
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    float baratsuki=param.BARATSUKI;//一時保存フォルダ
    //int SerialNumber = 0;
    //param.BARATSUKI=param.BARATSUKI_R;
    res    = float(mweight.u01.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res2   = float(mweight.u02.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res3   = float(mweight.u03.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res4   = float(mweight.u04.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res5   = float(mweight.u05.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res6   = float(mweight.u06.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res7   = float(mweight.u07.rank_max);//*1.03;    //組み合わせ重量の上限値を取得
    res8   = float(mweight.u08.rank_max);//*1.03;   //組み合わせ重量の上限値を取得
    res9   = float(mweight.u09.rank_max);//*1.03;   //組み合わせ重量の上限値を取得
    res10  = float(mweight.u10.rank_max);//*1.03;   //組み合わせ重量の上限値を取得
    rank1ave  = (float(mweight.u01.rank_max)+float(mweight.u02.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank2ave  = (float(mweight.u02.rank_max)+float(mweight.u03.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank3ave  = (float(mweight.u03.rank_max)+float(mweight.u04.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank4ave  = (float(mweight.u04.rank_max)+float(mweight.u05.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank5ave  = (float(mweight.u05.rank_max)+float(mweight.u06.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank6ave  = (float(mweight.u06.rank_max)+float(mweight.u07.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank7ave  = (float(mweight.u07.rank_max)+float(mweight.u08.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank8ave  = (float(mweight.u08.rank_max)+float(mweight.u09.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank9ave  = (float(mweight.u09.rank_max)+float(mweight.u10.rank_max))/2;    //組み合わせ重量の中間値を取得
    rank1dif  = (float(mweight.u02.rank_max)+float(mweight.u01.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank2dif  = (float(mweight.u03.rank_max)+float(mweight.u02.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank3dif  = (float(mweight.u04.rank_max)+float(mweight.u03.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank4dif  = (float(mweight.u05.rank_max)+float(mweight.u04.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank5dif  = (float(mweight.u06.rank_max)+float(mweight.u05.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank6dif  = (float(mweight.u07.rank_max)+float(mweight.u06.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank7dif  = (float(mweight.u08.rank_max)+float(mweight.u07.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank8dif  = (float(mweight.u09.rank_max)+float(mweight.u08.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    rank9dif  = (float(mweight.u10.rank_max)+float(mweight.u09.rank_max))*0.40;    //組み合わせ重量の上限値を取得
    int btnset = 0;
    lcd.settings_rank2(&load);            //設定内容を表示
    for(int i = 0; i < product.head; i++) {
        nowrank[i]=0;
        recrank[i]=99;
    }
    for(int i = 0; i < 11; i++) {
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
        totalrankerC[i] = 0;
        totalweightC[i] = 0;
        totalrankerD[i] = 0;
        totalweightD[i] = 0;
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

    while(1) {
        //led.ledAllOff();            // LED消灯
        //led.RedOff();
        btn.new_keep_btn();               //スイッチ(New)取得
        

        //esc_swを押すと、Menu画面に戻る
        if(btn.esc()/*||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)*/) {
            se.Touch_01();
            comp_rank=0;
            //LCD表示
            lcd.blank(0);
            lcd.blank(1);
            lcd.locate(3,1);
            if(option.usb_EN==true) {
                     if(option.usb_EN==true) {
                        lcd.printf(saving[2]);
                    }               lcd.printf(saving[1]);
            }
            //戻りきるまで時間が掛かるので、マトリクス表示を消す
            for(int i = 0; i < product.head; i++) {
                iro[i]=C_NULL;
            }
            //個別データ書き込み(1個以上あったら)
            int total_qty=0;
            for(int i = 0; i < 11; i++) {
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
                }
            }

            for(int i = 0; i < product.head; i++) {
                oldweightVale[i] = 0;
                nowweightVale[i] = 0;
                nowweightVale2[i] = 0;
                nowweightVale3[i] = 0;
                nowweightVale4[i] = 0;
                recrank[i]=99;
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
            for(int i = 0; i < 11; i++) {
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
                totalrankerC[i] = 0;
                totalweightC[i] = 0;
                totalrankerD[i] = 0;
                totalweightD[i] = 0;
            }
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            return;
        }

        //ent_swを押すと、ゼロ点調整（風袋引き）する
        if(btn.get_btn() == BTN_ENT) {
            auto_zero_point();
            lcd.settings_rank2(&load);            //設定内容を表示
        }
        //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        else if(btn.up_press() == true) {
            se.chime_01();
            while(btn.any_press()==true){
                if(demo_flg==true&&btn.down_press() == true&&file.get_preset()==product.head*2){
                    demo_continue=3;
                    btn.force_ESC();
                    se.Touch_01();
                    continue;
                }
                wait(0.1);
            }
            if(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2){
                    demo_continue=3;
                    btn.force_ESC();
                    continue;
            }
            btn.old_btn();       //スイッチ更新
            btn.new_btn();       //スイッチ更新
            
            btnset = Chenge_vaule_btnD(99);//ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
            if(btnset==0) {
                if(multi_weight_set3(file.get_preset(),0 )==0){
                    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
                }
            } else if(btnset==1) {
                auto_zero_off = true;
                start_keiryo(0);
                auto_zero_off = false;
            }
            lcd.settings_rank2(&load);            //設定内容を表示
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

        for(int i = 0; i < product.head; i++) {
            //setHeadVolt(i, 200);
            if((oneBit[i] & stop_cells) > 0) { //不使用ならセルを無視する
                cel.set_head(i);
                cel.setAin_zero(i);
            } else {
                //cel.set_head(i);
                //cel.setAinSpeed(200);
                //param.BARATSUKI = cel.getWeight(i)*0.02;
                //cel.set_BARATSUKI(param.BARATSUKI);
                setHeadVolt(i, 600);
                //if(cel.anteiWeight_rank3(i)>param.CUT){//ｺｳｿｸでは取られたか確認できればいいので繰返し回数は少なめに
                if(nowrank[i]!=0) {
                    //setHeadVolt(i, param.KURI);
                    setHeadVolt(i, 700);
                    /*if(nowweightVale[i] - cel.anteiWeight(i)>1 && nowweightVale[i] - cel.anteiWeight(i)<-1){
                        setHeadVolt(i, param.KURI);
                    }*/
                } else if(cel.getWeight(i)>param.CUT) {
                    setHeadVolt(i, 800);
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
        resultCombi = get_rank_location_d();
        
        resultCombi = get_juuryo_location_d();

        //組み合わせに応じたLED点灯,総重量表示
        show_rank2(&resultCombi);
    }//while
}

//ランク選別モード時に使用する関数
//指定範囲内の計量物の場所を取得する
//ｺｳｿｸﾗﾝｸｾﾝﾍﾞﾂ用なので、ﾗﾝｸで指定された値より3パーセント高い値まで、
//範囲内とする様、設定する(1.02)
combiData_t get_rank_location_d()
{
    combiData_t com = {0};
    //printf("%4.0f:%4.0f:%4.0f:%4.0f:%4.0f:%4.0f:%4.0f\r\n",res,res2,res3,res4,res5,res6,res7);
    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)

    float *weight = new float[product.head];
    //安定している重量を取得
    for(int i = 0; i < product.head; i++) {
        if(((1<<i) & BOT1_HEAD)>0){
            //weight[i] = cel.anteiWeight_rank3(i);
            //weight[i] = cel.anteiWeight(i);
            weight[i] = cel.getWeight(i);
            if(weight[i] > 0) {
                //totalweightN[i]=weight[i];
            } else if(weight[i] == 0 && (cel.getWeight(i) < param.CUT+3 && oldweightVale[i]< param.CUT+3) && recrank[i]>=0 && recrank[i]<=10) { //安定が無くなって、現在の値が0付近になったら
    
                if(i<=2) {
                    if(float((int(totalweightN[i])) >= param.CUT)){
                        totalweight0103[recrank[i]] += totalweightN[i];
                        totalranker0103[recrank[i]] += 1;
                    }
                } else if(i<=5) {
                    if(float((int(totalweightN[i])) >= param.CUT)){
                        totalweight0406[recrank[i]] += totalweightN[i];
                        totalranker0406[recrank[i]] += 1;
                    }
                } else if(i<=8) {
                    if(float((int(totalweightN[i])) >= param.CUT)){
                        totalweight0709[recrank[i]] += totalweightN[i];
                        totalranker0709[recrank[i]] += 1;
                    }
                } else if(i<=11) {
                    if(float((int(totalweightN[i])) >= param.CUT)){
                        totalweight1012[recrank[i]] += totalweightN[i];
                        totalranker1012[recrank[i]] += 1;
                    }
                }
                totalweightN[i] = 0;
                recrank[i]      = 99;
            }
            if((weight[i] == 0 && cel.getWeight(i) > param.CUT && cel.getWeight(i) <= param.CUT)||cel.anteiWeight_rank(i)>=param.CUT) {
                //weight[i] = cel.anteiWeight_rank3(i);
                //weight[i] = cel.anteiWeight(i);
                weight[i] = cel.getWeight(i);
            }
            //if(i==0){
            //    pc.printf("%d %6.3f %6.3f  %6.3f  %d\r\n", i+1,cel.anteiWeight(i),cel.anteiWeight(i), weight[i],iro[i]);
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
        }
    }
    //ヘッド数分の重量確認
    for(int i = 0; i < product.head; i++) {
        
        if(((1<<i) & BOT1_HEAD)>0){
            nowweightVale4[i] = nowweightVale3[i];
            nowweightVale3[i] = nowweightVale2[i];
            nowweightVale2[i] = weight[i];
    //        if((weight[i]!=0 && nowrank[i]==99)||(iro[i]==C_NULL && nowrank[i]!=0)) {//一度ﾗﾝｸ確定したものは除外する(ｹｲﾘｮｳﾓｰﾄﾞに行った時はもう一度表示する)
            if(weight[i]>0) {//一度ﾗﾝｸ確定したものは除外する(ｹｲﾘｮｳﾓｰﾄﾞに行った時はもう一度表示する)
                //一番最適な重量を取得する
                if((int(weight[i]) > -param.CUT) && float((int(weight[i])) <= param.CUT)) {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                    //値を取得
                    //com.kumi += (1 << i);     //com.total += weight[i];
                } else if((float(int(weight[i])) > param.CUT) && (float(int(weight[i])) < float(res))) {
                    if(mweight.u01.rank_en==1) {
                        iro[i]=C_BATU;
                        nowrank[i]=0;
                        //値を取得
                        //com.kumi += (1 << i);//com.total += weight[i];
                    } else {
                        //iro[i]=C_NULL;
                        iro[i]=C_BATU;
                        nowrank[i]=0;
                    }
                } else if((float(int(weight[i])) >= float(res)) && (float(int(weight[i])) < float(res2))) {
                    if(mweight.u02.rank_en==1) {
                        iro[i]=mweight.u02.rank_no;
                        nowrank[i]=1;
                        com.kumi += (1 << i);
                        com.total += weight[i];
                    } else {
                        iro[i]=C_NULL;
                        nowrank[i]=99;
                    }
                } else if((float(int(weight[i])) >= float(res2)) && (float(int(weight[i])) < float(res3))) {
                    if(mweight.u03.rank_en==1) {
                        iro[i]=mweight.u03.rank_no;
                        nowrank[i]=2;
                        com.kumi += (1 << i);
                        com.total += weight[i];
                    } else {
                        iro[i]=C_NULL;
                        nowrank[i]=99;
                    }
                } else if((float(int(weight[i])) >= float(res3)) && (float(int(weight[i])) < float(res4))) {
                    if(mweight.u04.rank_en==1) {
                        iro[i]=mweight.u04.rank_no;
                        nowrank[i]=3;
                        com.kumi += (1 << i);
                        com.total += weight[i];
                    } else {
                        iro[i]=C_NULL;
                        nowrank[i]=99;
                    }
                } else if((float(int(weight[i])) >= float(res4)) && (float(int(weight[i])) < float(res5))) {
                    if(mweight.u05.rank_en==1) {
                        iro[i]=mweight.u05.rank_no;
                        nowrank[i]=4;
                        com.kumi += (1 << i);
                        com.total += weight[i];
                    } else {
                        iro[i]=C_NULL;
                        nowrank[i]=99;
                    }
                } else if((float(int(weight[i])) >= float(res5)) && (float(int(weight[i])) < float(res6))) {
                    if(mweight.u06.rank_en==1) {
                        iro[i]=mweight.u06.rank_no;
                        nowrank[i]=5;
                        com.kumi += (1 << i);
                        com.total += weight[i];
                    } else {
                        iro[i]=C_NULL;
                        nowrank[i]=99;
                    }
                } else if((float(int(weight[i])) >= float(res6)) && (float(int(weight[i])) < float(res7))) {
                    if(mweight.u07.rank_en==1) {
                        iro[i]=mweight.u07.rank_no;
                        nowrank[i]=6;
                        com.kumi += (1 << i);
                        com.total += weight[i];
                    } else {
                        iro[i]=C_NULL;
                        nowrank[i]=99;
                    }
                } else if((float(int(weight[i])) >= float(res7)) && (float(int(weight[i])) < float(res8))) {
                    if(mweight.u08.rank_en==1) {
                        iro[i]=mweight.u08.rank_no;
                        nowrank[i]=7;
                        com.kumi += (1 << i);
                        com.total += weight[i];
                    } else {
                        iro[i]=C_NULL;
                        nowrank[i]=99;
                    }
                } else if((float(int(weight[i])) >= float(res8)) && (float(int(weight[i])) < float(res9))) {
                    if(mweight.u09.rank_en==1) {
                        iro[i]=mweight.u09.rank_no;
                        nowrank[i]=8;
                        com.kumi += (1 << i);
                        com.total += weight[i];
                    } else {
                        iro[i]=C_NULL;
                        nowrank[i]=99;
                    }
                } else if((float(int(weight[i])) >= float(res9)) && (float(int(weight[i])) < float(res10))) {
                    if(mweight.u10.rank_en==1) {
                        iro[i]=mweight.u10.rank_no;
                        nowrank[i]=9;
                        com.kumi += (1 << i);
                        com.total += weight[i];
                    } else {
                        iro[i]=C_NULL;
                        nowrank[i]=99;
                    }
                } else if(float(int(weight[i])) >= float(res10)) {
                    iro[i]=C_BATU;
                    nowrank[i]=10;
                    com.kumi += (1 << i);
                    com.total += weight[i];
                } else {
                    iro[i]=PENTA_R;
                    nowrank[i]=10;
                }
                //}else if(cel.getWeight(i) > param.CUT && weight[i] < param.CUT) {
            } else if(weight[i]> param.CUT && nowrank[i]==0) {
                //iro[i]=C_NUSE;
                //pc.printf("%dc\r\n",i+1);
            } else if(weight[i]< param.CUT && nowrank[i]==0) {
                iro[i]=C_NULL;
                //}else if(nowrank[i]!=0 && nowweightVale2[i] < param.CUT+3 && nowweightVale3[i]< param.CUT+3) {
            } else if(nowrank[i]!=0 && nowweightVale2[i] < param.CUT+3) {
                setHeadVolt(i, 500);
                if(weight[i]< param.CUT) {
                    iro[i]=C_NULL;
                    nowrank[i]=99;
                }
                //pc.printf("%da\r\n",i+1);
            }
    
    
            //大体真ん中付近で重量が止まってたら、その値を確定とする
            //for(int m=0;m<4;m++){
            nearrank[1][i]=nearrank[0][i];
            nearrank[3][i]=nearrank[1][i];
            nearrank[2][i]=nearrank[1][i];
            nearrank[1][i]=nearrank[0][i];
            nearrank[0][i]=99;
            //}
            //if(nowrank[i]==0||(float(int(cel.getWeight(i))) > param.CUT)) {
            if(nowrank[i]==0||nowrank[i]>9) {
                if(float(int(weight[i]) > -param.CUT) && float(int(weight[i])<= param.CUT)) {
                    nearrank[0][i]=99;
                    //値を取得
                    //com.kumi += (1 << i);
                    //com.total += weight[i];
                } else if((float(int(weight[i])) > param.CUT) && (float(int(weight[i])) < float(res))) {
                    if(mweight.u01.rank_en==1) {
                        nearrank[0][i]=0;
                        //nearrank[0][i]=0;
                    } else {//iro[i]=C_BATU;nowrank[i]=0;
                        nearrank[0][i]=0;
                        //nearrank[0][i]=0;
                    }
                } else if((float(int(weight[i])) >= float(rank1ave-rank1dif)) && (float(int(weight[i])) < float(res2))) {
                    if(mweight.u02.rank_en==1) {
                        nearrank[0][i]=mweight.u02.rank_no;
                    } else {//iro[i]=C_NULL;nowrank[i]=0;
                        nearrank[0][i]=99;
                    }
                } else if((float(int(weight[i])) >= float(rank2ave-rank2dif)) && (float(int(weight[i])) < float(res3))) {
                    if(mweight.u03.rank_en==1) {
                        nearrank[0][i]=mweight.u03.rank_no;
                    } else {//iro[i]=C_NULL;nowrank[i]=0;
                        nearrank[0][i]=99;
                    }
                } else if((float(int(weight[i])) >= float(rank3ave-rank3dif)) && (float(int(weight[i])) < float(res4))) {
                    if(mweight.u04.rank_en==1) {
                        nearrank[0][i]=mweight.u04.rank_no;
                    } else {//iro[i]=C_NULL;nowrank[i]=0;
                        nearrank[0][i]=99;
                    }
                } else if((float(int(weight[i])) >= float(rank4ave-rank4dif)) && (float(int(weight[i])) < float(res5))) {
                    if(mweight.u05.rank_en==1) {
                        nearrank[0][i]=mweight.u05.rank_no;
                    } else {//iro[i]=C_NULL;nowrank[i]=0;
                        nearrank[0][i]=99;
                    }
                } else if((float(int(weight[i])) >= float(rank5ave-rank5dif)) && (float(int(weight[i])) < float(res6))) {
                    if(mweight.u06.rank_en==1) {
                        nearrank[0][i]=mweight.u06.rank_no;
                    } else {//iro[i]=C_NULL;nowrank[i]=0;
                        nearrank[0][i]=99;
                    }
                } else if((float(int(weight[i])) >= float(rank6ave-rank6dif)) && (float(int(weight[i])) < float(res7))) {
                    if(mweight.u07.rank_en==1) {
                        nearrank[0][i]=mweight.u07.rank_no;
                    } else {//iro[i]=C_NULL;nowrank[i]=0;
                        nearrank[0][i]=99;
                    }
                } else if((float(int(weight[i])) >= float(rank7ave-rank7dif)) && (float(int(weight[i])) < float(res8))) {
                    if(mweight.u08.rank_en==1) {
                        nearrank[0][i]=mweight.u08.rank_no;
                    } else {//iro[i]=C_NULL;nowrank[i]=0;
                        nearrank[0][i]=99;
                    }
                } else if((float(int(weight[i])) >= float(rank8ave-rank8dif)) && (float(int(weight[i])) < float(res9))) {
                    if(mweight.u09.rank_en==1) {
                        nearrank[0][i]=mweight.u09.rank_no;
                    } else {//iro[i]=C_NULL;nowrank[i]=0;
                        nearrank[0][i]=99;
                    }
                } else if((float(int(weight[i])) >= float(rank9ave-rank9dif)) && (float(int(weight[i])) < float(res10))) {
                    if(mweight.u10.rank_en==1) {
                        nearrank[0][i]=mweight.u10.rank_no;
                    } else {//iro[i]=C_NULL;nowrank[i]=0;
                        nearrank[0][i]=99;
                    }
                } else if(float(int(weight[i])) >= float(res10)) {
                    //nearrank[0][i]=C_BATU;
                    nearrank[0][i]=10;
                } else {//iro[i]=PENTA_R;nowrank[i]=0;
                    nearrank[0][i]=99;
                }
                nearrank[1][i]=nearrank[0][i];
                nearrank[3][i]=nearrank[1][i];
                nearrank[2][i]=nearrank[1][i];
                //連続で同じﾗﾝｸだった場合、(上の設定値が低めギリギリを除外してるので)ﾗﾝｸとしては確証がもてるので、表示を変える
                //if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i] && nearrank[1][i] == nearrank[2][i] && nearrank[2][i] == nearrank[3][i]){
                if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i] && nearrank[1][i] == nearrank[2][i] && nearrank[1][i] == nearrank[3][i]) {
                    if(nearrank[0][i]==0||nearrank[0][i]==10){
                        iro[i] =C_BATU;
                    }else{
                        iro[i]=nearrank[0][i];
                    }
                    if(     nearrank[0][i]==0                  ){nowrank[i]= 0;}
                    else if(nearrank[0][i]==mweight.u02.rank_no){nowrank[i]= 1;}
                    else if(nearrank[0][i]==mweight.u03.rank_no){nowrank[i]= 2;}
                    else if(nearrank[0][i]==mweight.u04.rank_no){nowrank[i]= 3;}
                    else if(nearrank[0][i]==mweight.u05.rank_no){nowrank[i]= 4;}
                    else if(nearrank[0][i]==mweight.u06.rank_no){nowrank[i]= 5;}
                    else if(nearrank[0][i]==mweight.u07.rank_no){nowrank[i]= 6;}
                    else if(nearrank[0][i]==mweight.u08.rank_no){nowrank[i]= 7;}
                    else if(nearrank[0][i]==mweight.u09.rank_no){nowrank[i]= 8;}
                    else if(nearrank[0][i]==mweight.u10.rank_no){nowrank[i]= 9;}
                    else if(nearrank[0][i]==10                 ){nowrank[i]=10;}
                    else{                                        nowrank[i]=99;}
                    
                    
                    //nowrank[i]=nearrank[0][i];
                    if(weight[i] != 0) {
                        //comp_rank = comp_rank | 1<<i;
                    } else {
                        //comp_rank = comp_rank & ~(1<<i);
                    }
                    /*for(int m=0;m<4;m++){
                        nearrank[3][i]=0;
                        nearrank[2][i]=0;
                        nearrank[1][i]=0;
                        nearrank[0][i]=0;
                    }*/
                    /*if(i==3||i==2){
                        printf("%dd\r\n",i+1);
                    }*/
                } else if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i] && nearrank[1][i] == nearrank[2][i]) {
                    if(nearrank[0][i]==0||nearrank[0][i]==10){
                        iro[i] =C_BATU;
                        nowrank[i]=nearrank[0][i];
                    }else{
                        iro[i]=nearrank[0][i];
                        if(     nearrank[0][i]==0                  ){nowrank[i]= 0;}
                        else if(nearrank[0][i]==mweight.u02.rank_no){nowrank[i]= 1;}
                        else if(nearrank[0][i]==mweight.u03.rank_no){nowrank[i]= 2;}
                        else if(nearrank[0][i]==mweight.u04.rank_no){nowrank[i]= 3;}
                        else if(nearrank[0][i]==mweight.u05.rank_no){nowrank[i]= 4;}
                        else if(nearrank[0][i]==mweight.u06.rank_no){nowrank[i]= 5;}
                        else if(nearrank[0][i]==mweight.u07.rank_no){nowrank[i]= 6;}
                        else if(nearrank[0][i]==mweight.u08.rank_no){nowrank[i]= 7;}
                        else if(nearrank[0][i]==mweight.u09.rank_no){nowrank[i]= 8;}
                        else if(nearrank[0][i]==mweight.u10.rank_no){nowrank[i]= 9;}
                        else if(nearrank[0][i]==10                 ){nowrank[i]=10;}
                        else{                                        nowrank[i]=99;}
                    }
                } else if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i]) {
                    iro[i]=C_NUSE;
                } else if(nearrank[0][i]!=0 && nearrank[0][i] != nearrank[1][i]) {
                    iro[i]=C_NUSE;
                } else if(nearrank[0][i]!=0 && nearrank[1][i] !=0 && nearrank[0][i] != nearrank[1][i]) {
                    iro[i]=C_NUSE;
                } else if(nearrank[0][i]!=0 && nearrank[0][i] != nearrank[1][i] && nearrank[1][i] != nearrank[2][i]) {
                    iro[i]=C_NUSE;
                } else if(nearrank[0][i]!=0 && nearrank[0][i] == nearrank[1][i]) {
                    iro[i]=C_NUSE;
                } else {
                }
            }
            if(cel.anteiWeight_rank3(i)>param.CUT) {
                recrank[i]= nowrank[i];
            }
        }
    }//loop終了

    delete[] weight;
    return com;

}



void drive_juuryo_d()
{
    char saving[]= {0xCE,0xBF,0xDE,0xDD,0xC1,0xAD,0xB3,0x00}; //ﾎｿﾞﾝﾁｭｳ
    combiData_t resultCombi;        //組み合わせ結果(最小値取得用)
    mweight = file.get_lweight(file.get_preset()); //現在プリセット選択されているデータを取得
    float baratsuki=param.BARATSUKI;//一時保存フォルダ
    int btnset =0;//計量中の下ボタン押しで出てくる値保存
    //param.BARATSUKI=param.BARATSUKI_R;
    param.ZEROAREA=param.ZEROAREA2;
    cel.set_ZEROAREA(param.ZEROAREA);
    if(product.head>1) {
        lcd.settings_rank2(&load);            //設定内容を表示
    } else {
        lcd.blank(0);
        lcd.blank(1);
    }
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
        if(btn.esc()/*||(demo_flg==true&&demo_continue!=0&&file.get_preset()==product.head*2)*/) {
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
            int total_qty=0;
            for(int i = 0; i < 10; i++) {
                total_qty += totalranker0103[i]+totalranker0406[i]+totalranker0709[i]+totalranker1012[i];
            }
            /*if(total_qty>0) {
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
                save_rankdata2(mweight.Rsort,load.target,load.jougen,ji,hun);
                //save_rankdata(mweight.Rsort);
                if(option.usb_EN==true) {
                    USB_rankdata2(mweight.Rsort,load.target,load.jougen,ji,hun);
                }
            }*/

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
            if(product.head>1) {
                lcd.settings_rank2(&load);            //設定内容を表示
            } else {
                lcd.blank(0);
                lcd.blank(1);
            }
        }
        //ｶｳﾝﾄorｹｲﾘｮｳﾓｰﾄﾞ
        else if(btn.up_press() == true) {
            se.chime_01();
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
            
            //btn.old_btn();       //スイッチ更新
            //btn.new_btn();       //スイッチ更新
            //auto_zero_off = true;
            //start_keiryo(0);
            //auto_zero_off = false;
            btnset = Chenge_vaule_btnD(0);//ｶｳﾝﾄ以外orｹｲﾘｮｳﾓｰﾄﾞ
            /*if(btnset==0) {
                set_count(&cnt);
            } else*/ if(btnset==1) {
                auto_zero_off = true;
                start_keiryo(0);
                auto_zero_off = false;
            }else if(btnset==2){
                set_sercret_parameter_one(P_KURI_J,1);
                //set_sercret_parameter_one(P_END,1); //保存指令付
            }else if(btnset==3){
                set_sercret_parameter_one(P_ZEROAREA2,1);
                param.ZEROAREA=param.ZEROAREA2;
                cel.set_ZEROAREA(param.ZEROAREA);
                //set_sercret_parameter_one(P_END,1); //保存指令付
            }
            
            if(product.head>1) {
                lcd.settings_rank2(&load);            //設定内容を表示
            } else {
                lcd.blank(0);
                lcd.blank(1);
                lcd.bigNumber(cel.getWeight(0));
            }
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
        if(product.head!=1){
            for(int i = 0; i < product.head; i++) {
                if(product.head<=13) {
                    //if(product.head<=6 || (i<=5 && i % 2 == 0) || (i>=6 && i % 2 == 1)){
                    //setHeadVolt(i, 200);
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
            }
        }else{
            setHeadVolt(0, 3000);
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
        resultCombi = get_juuryo_location_d();

        //組み合わせに応じたLED点灯,総重量表示
        //show_rank2(&resultCombi);

        if(product.head>1) {
            show_check_juuryo(&resultCombi);
        }
    }//while
    //file.read_param();          //設定値
    //cel.set_BARATSUKI(param.BARATSUKI);
    //cel.set_ZEROAREA(param.ZEROAREA);
}



//float wait_old[12];
//float disp_wait=0;
combiData_t get_juuryo_location_d()
{
    combiData_t com = {0};
    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)
    float *weight = new float[product.head];
    //安定している重量を取得
    for(int i = 0; i < product.head; i++) {
        if(((1<<i) & TOP1_HEAD)>0){
            if(product.head<=13) {
                //if(product.head<=6 || (i<=5 && i % 2 == 0) || (i>=6 && i % 2 == 1)){
                //weight[i] = cel.anteiWeight(i);
                weight[i] = int(cel.getWeight(i));
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
                    } else if(i<=11) {
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
                if(((int(weight[i])-int(wait_old[i]))<0) || ((int(weight[i])-int(wait_old[i]))>1)){
                    wait_old[i] = weight[i];
                }else{
                    weight[i] = wait_old[i];
                }
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
    }
    //ヘッド数分の重量確認
    for(int i = 0; i < product.head; i++) {
        if(((1<<i) & TOP1_HEAD)>0){
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
                    iro[i]=C_NUSE;
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
                    } else if((int(weight[i]) > param.CUT) && (int(weight[i]) < int(load.target))) {
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
                        if(load.target-weight[i]<100){
                            //iro[i]= CG_00+(load.target-weight[i]) + 1;
                            iro[i]= CR_00+(load.target-weight[i]) /*+ 1*/;
                            //printf("w1 %5d\r\n",iro[i]);
                        
                        }else if(load.target-weight[i]<200){
                            iro[i]= C_NUSE_1D;
                        }else{
                            iro[i]= C_NUSE_2D;
                        }
                            /*
                        if((load.target-weight[i])<load.tanjuu) {
                            if(load.check_hyouji+1 >= 1-1){
                                iro[i]= CR_M1;
                            }else {
                                iro[i]= C_NUSE;
                            }
                        } else if((load.target-weight[i])<load.tanjuu*2) {
                            if(load.check_hyouji+1 >= 2-1){
                                iro[i]= CR_M2;
                            }else {
                                iro[i]= C_NUSE;
                        
                        } else {
                            iro[i]= C_NUSE;
                        }*/
                   // } else if(((int(weight[i]) >= int(load.target)) && (int(weight[i]*0.998) < int(load.target + load.jougen)))) {
                   } else if(((int(weight[i]) >= int(load.target)) && (int(weight[i]) <= int(load.target + load.jougen)))) {
                        //iro[i]=CG_0;
                        iro[i]=C_MARU;
                        headqty=1;
                        if(int(totalcheckwei[i])!=int(totalweightN[i])) {
                            if(product.head==1) {
                                totalcheckwei[i]=int(cel.getWeight(i));
                                lcd.bigNumber(int(cel.getWeight(i)));
                            }
                        }
                        headqty=0;
                        nowrank[i]      = 2;
                        //}
                    //} else if(((int(weight[i]) >= int(load.target)))) {
                    } else if(((int(weight[i]) >= int(load.target + load.jougen)))) {
                        if(product.head==1) {
                            oneshoot_OK=false;
                        }
    
    
                        //if((weight[i]-(load.target))<100){
                        if((weight[i]-(load.target + load.jougen))<100){
                            //iro[i]= CR_00+(weight[i]-(load.target));
                            //iro[i]= CR_00+(weight[i]-(load.target + load.jougen));
                            iro[i]= CG_01-1+(weight[i]-(load.target + load.jougen));
                            nowrank[i]      = 2;
                            //printf("w2 %5d\r\n",iro[i]);
                        }else{
                            iro[i]=C_BATU;
                            if(product.head==1) {
                                se.mistake();
                                wait(1);
                            }
                        }
    
    
                    } else {
                        iro[i]=C_BATU;
                    }
                } else if(int(cel.getWeight(i)) < param.CUT+3 && oldweightVale[i]< param.CUT+3) {
                    iro[i]=C_NUSE;
                }
                if(cel.anteiWeight_rank(i)>0) {
                    recrank[i]= nowrank[i];
                }
            }
        }
    }//loop終了

    delete[] weight;
    return com;

}







