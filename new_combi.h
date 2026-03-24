
void get_combi_location_AUTO(combiData_t com[], int *secondCnt, int no_combit)
{
    combiData_t com1[3] = {{0,0.0,0},{0,0.0,0},{0,0.0,0}};
    combiData_t com2[7] = {{0,0.0,0},{0,0.0,0},{0,0.0,0},{0,0.0,0},{0,0.0,0},{0,0.0,0},{0,0.0,0}};
    int bit,bk,in = 0; //in:物が載っている場所を取得
    int bit_num=0;    //成立した個数の保存
    double sum=0;   //組み合わせ候補箇所の重量を合算していく
    int min = load.target + param.buffer;  //最小値(バッファー有り)
    int res = load.target + load.jougen;    //組み合わせ重量の上限値を取準最適値で変わることもあるので注得
    int resDefault = res;                   //上限値のデフォルト値をここでバックアップ
    bool checkBtnJunsaiteki = false;            //ボタン押下で準最適値モードになったか
    bool combi_OK_flug = false;             //１個でも成立した物があるか
    int com_cnt = 0;
    ArrayIni(com, BK_CNT);   //初期化(組み合わせ保持している分を初期化)
    
    in = cel.getAnteiIn();   //計量物の数を取得
    //計量物ないなら即終了
    if(in == 0) {
        return;
    }
    //in=63;

    //各計量皿の重量値(クラスから重量取得をループ内で行うと
    //時間がかかるらしいのでローカル変数に重量をコピー)
    float *weight = new float[product.head];
    float maxWeight = 0, minWeight = 0;
    float itiji_weight = 0.0;

    for(int i = 0; i < product.head; i++) {
        itiji_weight = cel.anteiWeight(i);
        if(itiji_weight == 0){
            weight[i] = 0.0;
        }else{
            //weight[i] = cel.anteiWeight(i) + op_n_hosei + hosei[i];
            weight[i] = itiji_weight + op_n_hosei + hosei_f[i];
        }
        //weight[i] = 50.1;
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
        
        if((i & no_combit) > 0){
            continue;
        }
        //if(com1[2].total > 0){
        //if( load.target <= com1[0].total && com1[0].total <= res &&
        //    load.target <= com1[1].total && com1[1].total <= res &&
        //    load.target <= com1[2].total && com1[2].total <= res){//com3にデータが入ってる場合は、このループを抜ける
        //    i = 1;
            //continue;
        //}

        //組み合わせ候補に重量ゼロがあれば次の組み合わせ候補へ
        if( (in & i) != i) continue;
        
        if(servo_connection==true && moving1 > 0){ //組合せに、組合せに入れられないヘッドがあれば次の組み合わせ候補へ
            if((i & moving1) > 0){continue;} 
        }

        //指定された組み合わせ条件を満たしているか確認
        if(check_combi_condition(i,&load) == false) {
            //printf("combiNG\r\n");
            continue;
        }
        //if(((i & com1[0].kumi) > 0) || ((i & com1[1].kumi) > 0) || ((i & com1[2].kumi) > 0)){
        //    continue;
        //}
        
                    
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
                if((load.kumi_flag == KUMI_OYAKO || load.kumi_flag == KUMI_DENSO)
                    && (oneBit[bit] & gOyaHead) > 0){
                }else{
                    if( maxWeight < weight[bit]) maxWeight = weight[bit];   //最重量値の場合は値取得
                    if( minWeight > weight[bit]) minWeight = weight[bit];   //最軽量値の場合は値取得
                }
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
        //if((sum >= min) && (int(sum) <= int(res))) {
        //if((load.kumi_flag!=KUMI_LIMIT && (( int(sum) >= min) && (int(sum) <= res))) ||
        //                                    ((float(bit_count(i))*param.buffer2<float(res))&&
        //  (load.kumi_flag==KUMI_LIMIT && (((int(sum) >= (float(min) + (float(bit_count(i))*param.buffer2))) && (int(sum) <= int(res)))))) ||
        //  (load.kumi_flag==KUMI_LIMIT && (int(sum) >= int(res)) && 
        //                                 (int(sum) <= int(res))))
                                         
                                         
         if(/*(load.kumi_flag!=KUMI_LIMIT && (( int(sum) >= min)                                && (int(sum) <= res))) ||*/
            (load.kumi_flag!=KUMI_LIMIT && (( int(sum) >= min + (bit_count(i)*param.buffer2)) && (int(sum) <= res)))||
            (load.kumi_flag==KUMI_LIMIT && (( int(sum) >= min + (bit_count(i)*param.buffer2)) && (int(sum) <= res))))
        {
            //printf(":x > min:");

            //printf("\r\n");

            //組み合わせ成立があるならば
            //準最適値処理は行わない
                //printf("OK?\r\n");
                
                //printf("%6.1f:%6d\r\n",sum,int(resDefault));
            if(int(sum) <= resDefault/* || combi_OK != 0*/) {
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
            com[BK_CNT -1].kumi = i;
            com[BK_CNT -1].total = sum;
            //com2[BK_CNT -1].kumi = i;
            //com2[BK_CNT -1].total = sum;
            //if(load.target <= int(sum) && int(sum) <= resDefault ){
            //    com1[com_cnt].kumi = i;
            //    com1[com_cnt].total = sum;
            //    com_cnt += 1;
            //}
            bit_num = bit_count(i);
            //組合せ優先(99の場合は使用しない)
            if(load.kumi_priolity_1!=99 && bit_num == load.kumi_priolity_1 + 1) {
                com[BK_CNT -1].priolity =0;
                //com2[BK_CNT -1].priolity =0;
            } else if(load.kumi_priolity_2!=99 && bit_num == load.kumi_priolity_2 + 1) {
                com[BK_CNT -1].priolity =1;
                //com2[BK_CNT -1].priolity =1;
            } else {
                com[BK_CNT -1].priolity =2;
                //com2[BK_CNT -1].priolity =2;
            }
            qsort(com, BK_CNT, sizeof(combiData_t), comp );
            //qsort(com2, BK_CNT, sizeof(combiData_t), comp );
            //qsort(com1, 3, sizeof(combiData_t), comp );
            //  for (int j=0;j<BK_CNT -1;j++){
            //       pc.printf("%d  %4f  %2d  \r\n",com[j].kumi, com[j].total,com[j].priolity);
            //  }
            //組合せ優先(99の場合は使用しない)
            if(load.kumi_priolity_1 != 99 || load.kumi_priolity_2 != 99) {
                qsort(com, BK_CNT, sizeof(combiData_t), comp2 );
                //qsort(com2, BK_CNT, sizeof(combiData_t), comp2 );
                //qsort(com1, 3, sizeof(combiData_t), comp2 );
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
    /**/
    /*printf("com1 00:%3d:%5.1f||",com1[0].kumi,com1[0].total);
    printf("01:%3d:%5.1f||",com1[1].kumi,com1[1].total);
    printf("02:%3d:%5.1f||",com1[2].kumi,com1[2].total);
    printf("\r\n");
    printf("com  00:%3d:%5.1f||",com[0].kumi,com[0].total);
    printf("01:%3d:%5.1f||",com[1].kumi,com[1].total);
    printf("02:%3d:%5.1f||",com[2].kumi,com[2].total);
    printf("\r\n");*/
    /*
    for(int d=0; d < 3;d++){
        //if(load.target <= com1[d].total && com1[d].total <= res ){
            if(d==1 && (com[0].kumi & com[1].kumi) > 0){
                com[d].kumi  = 0;
                com[d].total = 0;   
            }else if(d==2 && (com[0].kumi & com[1].kumi & com[2].kumi) > 0){
                com[d].kumi  = 0;
                com[d].total = 0;   
            }else {
                if(com1[d].kumi>0 && load.target <= int(com1[d].total) && int(com1[d].total) <= resDefault){
                    com[d].kumi  = com1[d].kumi;
                    com[d].total = com1[d].total; 
                } 
            }
        //}
    }*/
    //if(load.target <= com[0].total && com[0].total <= res ){
    //}else{
    //    com[0].kumi  = com2[0].kumi;
    //    com[0].total = com2[0].total;
    //}
    
    //組み合わせ成立があるならば
    //準最適値のデータを消す
    if(combi_OK_flug == true){//
        //printf("DELETE\r\n");
        for(int i = 1; i < BK_CNT; i++) {
            //printf("%2d :%4d :%6.1f:%4d\r\n",i,com[BK_CNT].kumi,com[BK_CNT].total,com[BK_CNT].priolity);
            if(int(com[i].total) > resDefault){
                com[i].kumi = 0;
                com[i].total = 0;
                com[i].priolity = 999;
                //printf("reset\r\n");
            }
        }
    }

    //ボタン操作で準最適値モードON後、
    //一つも組合せなければ「組合せがない」ことを通知する
    
    //printf("%5d ,%5d,%5d,%8.1f,%8.1f,%5d%5d ,\r\n",bit_count(in),bscData.startKinji-bit_count(stop_cells),bit_count(stop_cells),res,com[0].total,KINJITI_MAX_WHIGHT,int(sum));
    if(combi_OK == 0 && Compulsion_no_combi_v_flg == false && moving1 == 0){
        if( ((checkBtnJunsaiteki == true) && (res == KINJITI_MAX_WHIGHT))) {
            v.output_audio(MSG_NO_JUNSAITEKI);
            v.wait_echo();
        }else if((int(com[0].total) == DAMMY_MAX) && (bit_count(in) >= (bscData.startKinji-bit_count(stop_cells)))){
            if((     bit_count(in) >= (product.head-bit_count(stop_cells))) //||   //全ての皿に物が載った
              /*((load.katamen > 0) && (bit_count(in) >= ((bscData.startKinji/2)-bit_count(stop_cells))))*/){
                //if(option.no_combi_v==1){
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
    /*printf("com  00:%3d:%5.1f||",com[0].kumi,com[0].total);
    printf("01:%3d:%5.1f||",com[1].kumi,com[1].total);
    printf("02:%3d:%5.1f||",com[2].kumi,com[2].total);
    printf("\r\n");*/

    delete[] weight;
    return;
}


void show_results_auto(const combiData_t *com , const combiData_t *com1 , const combiData_t *com2){
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
    //printf("com1:%3d:%5.1f\r\n",com1->kumi,com1->total);
    //printf("com2:%3d:%5.1f\r\n",com2->kumi,com2->total);
    if(com->kumi == 0) {
        //if(load.kumi_flag == KUMI_KAIKYU && (product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON)){
        if((product.matrix_mode == MATRIX_ON)) {
            noCombiNews();
        } else {
            noCombiNews();
        }
        if(load.kumi_flag <= KUMI_LIMIT){//組合せの場合は下段に「ｸﾐｱﾜｾ」を表示する
            if(cnt.max == 0){
                lcd.locate(9,1);
                lcd.printf(kumiawase);
            }
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

        if(product.matrix_mode == MATRIX_ON) {
            if(load.spMode == SPEED_HIGH  || load.spMode == SPEED_DAMP){
                for(int i = 0; i <= product.head ; i++){
                    if( (com->kumi & 1<<i)>0){
                        if((int(com->total) > res)) {
                            if((com->total - res)<99){
                                iro[i] = CR_01 + com->total - res -1;
                            }else{
                                iro[i] = CR_99;
                            }
                        }else{
                            iro[i] = C_NULL;
                        }
                    }else{
                        iro[i] = C_NULL;
                    }
                }
            }else{
                led.REGIS3(com->kumi);
            }
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
                    led.REGIS3(resultCombi.kumi);
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
            if(cnt.max == 0){
                lcd.locate(9,1);
                lcd.printf(kumiawase);
            }
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
        if(load.spMode == SPEED_HIGH  || load.spMode == SPEED_DAMP) {
            if(option.kokuti==1||option.kokuti==2||option.kokuti==3){
                se.Touch_02();
            }
            if(product.matrix_mode == MATRIX_ON) {
                for(int i = 0; i <= product.head ; i++){
                    if( (com->kumi & 1<<i)>0){
                        if((int(com->total) > res)) {
                            if((com->total - res)<99){
                                iro[i] = CR_01 + com->total - res -1;
                            }else{
                                iro[i] = CR_99;
                            }
                        }else{
                            //if(gLevel == 1){
                                //if((cel.anteiWeight(i) + hosei[i]-1)<99){
                                //    iro[i] = CG_01 + cel.anteiWeight(i) + hosei[i]-1;
                                iro[i] = CG_01;
                                //}else{
                                    //iro[i] = CG_01 + bit_count(com->kumi)-1;
                                //}
                            //}else{
                            //    iro[i] = CG_01 + bit_count(com->kumi)-1;
                            //}
                        }
                    }else{
                        if(load.target <= com1->total && com1->total <= res && ((com1->kumi & (1<<i))>0) && ((com->kumi & com1->kumi) == 0)){
                            //iro[i] = C_BUI_Y;
                            //if(gLevel == 1){
                                iro[i] = CG_02;
                                /*
                                if((int(cel.anteiWeight(i) + hosei_f[i])-1)<99){
                                    iro[i] = CG_01 + int(cel.anteiWeight(i) + hosei_f[i])-1;
                                }else{
                                    iro[i] = CG_21 + bit_count(com->kumi)-1;
                                }
                                */
                            //}else{
                            //    iro[i] = CG_21 + bit_count(com1->kumi)-1;
                            //}
                        }else if(load.target <= com2->total && com2->total <= res && ((com2->kumi & (1<<i))>0) && ((com->kumi & com1->kumi & com2->kumi) == 0)){
                            //iro[i] = C_BUI_Y;
                            //if(gLevel == 1){
                                iro[i] = CG_03;
                                /*
                                if((int(cel.anteiWeight(i) + hosei_f[i])-1)<99){
                                    iro[i] = CG_01 + int(cel.anteiWeight(i) + hosei_f[i])-1;
                                }else{
                                    iro[i] = CG_42 + bit_count(com->kumi)-1;
                                }
                                */
                            //}else{
                            //    iro[i] = CG_41 + bit_count(com2->kumi)-1;
                            //}
                        }else if( (moving1 & (1<<i))>0){
                            if(iro[i] == C_R_EXCLA){
                            }else{
                                iro[i] = C_BUI_G;
                            }
                        }else {
                            iro[i] = C_NULL;
                        }
                    }
                }
            }else{
                led.REGIS(com->kumi | moving1);
            }
            //第二上限値内の組み合わせ成立だったらLED点滅させる
            if((load.secondOnOff == 1 || btn.get_lock_flg() == true) && (int(com->total) > res)) {
                se.Touch_02();
                int btn_flg = 0;
                for(int i = 0; i <= load.lightTime; i += 1) {
                    if(product.matrix_mode == MATRIX_ON) {
                        for(int i = 0; i <= product.head ; i++){
                            if( (com->kumi & 1<<i)>0){
                                if((com->total - res)<99){
                                    iro[i] = CR_01 + com->total - res -1;
                                }else{
                                    iro[i] = CR_99;
                                }
                            }else{
                                /*
                                if(load.target <= com1->total && com1->total <= res && ((com1->kumi & (1<<i))>0)){
                                    //iro[i] = C_BUI_Y;
                                    iro[i] = CG_21 + bit_count(com1->kumi)-1;
                                }else if(load.target <= com2->total && com2->total <= res && ((com2->kumi & (1<<i))>0)){
                                    //iro[i] = C_BUI_Y;
                                    iro[i] = CG_41 + bit_count(com2->kumi)-1;
                                }else*/ if( (moving1 & (1<<i))>0){
                                    if(iro[i] == C_R_EXCLA){
                                    }else{
                                        iro[i] = C_BUI_G;
                                    }
                                    
                                }else {
                                    iro[i] = C_NULL;
                                }
                            }
                        }
                    }else{
                        led.REGIS(com->kumi | moving1);
                    }
                    wait(0.05);
                    led.REGIS(0);
                    wait(0.05);
                    //再計算ボタンが押されたら、待機時間を待たずに終了する。
                    //ただし、ボタンが離されたことを確認後にボタンを押されたらとする
                    //(ボタンを押すことでここに入ってきてるかもしれないので)
                    if(btn_flg == 1 && btn.down_press() == true) {
                        se.Touch_02();
                        i = load.lightTime + 1;
                        
                        /*if(servo_connection==true && load.spMode == SPEED_DAMP){
                            moving2=99;
                            send_i(EJECT_RESET, 0, 0);
                            loop_a = 0;
                            while((check_signal()==false&&loop_a<=10) ||moving2!=0){
                            //while(check_signal()==false&&loop_a<=10){
                                loop_a+=1;
                                wait(0.1);
                                printf("ASA\r\n");
                            }
                        }*/
                        
                        
                        if(servo_connection==true && load.spMode == SPEED_DAMP){
                            //moving2=99;
                            //send_i(EJECT_RESET, 0, 0);
                            //loop_a = 0;
                            //while((check_signal()==false&&loop_a<=10) /*|| moving2!=0*/){
                            ////while(check_signal()==false&&loop_a<=10){
                            //    send_i(ON_CHECK,0,close_wait_flg);  //動作してる皿が無いか確認
                            //    loop_a+=1;
                            //    wait(0.1);
                            //}
                            send_i(ON_CHECK,0,close_wait_flg);
                            wait_signal();
                            while(btn.down_press() == true) {wait(0.1);}
                        }
                        
                    } else if(btn.down_press() == false) {
                        btn_flg = 1;
                    }
                }
                no_combi_v_flg = false;
            } else {
                if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON) {
                    for(int i = 0; i <= product.head ; i++){
                        if( (com->kumi & 1<<i)>0){
                            //if(gLevel == 1){
                                iro[i] = CG_01;
                                /*
                                if((int(cel.anteiWeight(i) + hosei_f[i])-1)<99){
                                    iro[i] = CG_01 + int(cel.anteiWeight(i) + hosei_f[i])-1;
                                }else{
                                    iro[i] = CG_01 + bit_count(com->kumi)-1;
                                }*/
                            //}else{
                            //    iro[i] = CG_01 + bit_count(com->kumi)-1;
                            //}
                        }else if(load.target <= com1->total && com1->total <= res && ((com1->kumi & (1<<i))>0) && ((com->kumi & com1->kumi) == 0)){
                            //if(gLevel == 1){
                                iro[i] = CG_02;
                                /*
                                if((int(cel.anteiWeight(i) + hosei_f[i])-1)<99){
                                    iro[i] = CG_01 + int(cel.anteiWeight(i) + hosei_f[i])-1;
                                }else{
                                    iro[i] = CG_21 + bit_count(com->kumi)-1;
                                }*/
                            //}else{
                            //    iro[i] = CG_21 + bit_count(com1->kumi)-1;
                            //}
                            //iro[i] = C_BUI_Y;
                        }else if(load.target <= com2->total && com2->total <= res && ((com2->kumi & (1<<i))>0) && ((com->kumi & com1->kumi & com2->kumi) == 0)){
                            //if(gLevel == 1){
                                iro[i] = CG_03;
                                /*
                                if((int(cel.anteiWeight(i) + hosei_f[i])-1)<99){
                                    iro[i] = CG_01 + int(cel.anteiWeight(i) + hosei_f[i])-1;
                                }else{
                                    iro[i] = CG_41 + bit_count(com->kumi)-1;
                                }
                                */
                            //}else{
                            //    iro[i] = CG_41 + bit_count(com2->kumi)-1;
                            //}
                            //iro[i] = C_BUI_Y;
                        }else if( (moving1 & (1<<i))>0){
                            if(iro[i] == C_R_EXCLA){
                            }else{
                                iro[i] = C_BUI_G;
                            }
                        }else {
                            iro[i] = C_NULL;
                        }
                    }
                }else{
                    led.REGIS(com->kumi | moving1);
                }
                
                /*
                int comkumi0[2]={com->kumi ,com->total};
                int comkumi1[2]={com1->kumi,com1->total};
                int comkumi2[2]={com2->kumi,com2->total};
                int com[3] = {0,0,0};
                int near_start = 1;
                int counter = 3;
                if(com1->kumi == 0){counter-=1;}
                if(com2->kumi == 0){counter-=1;}
                
                if(counter > 1){
                    if(course == false){
                        for(int k = 0; k < product.head && counter > 0 ; k++){
                            if(((com->kumi & (1 << k))>0)  ){
                                com[0] = near_start;
                                near_start += 1;
                                counter -= 1;
                            }
                            if(((com1->kumi & (1 << k))>0)  ){
                                com[1] = near_start;
                                near_start += 1;
                                counter -= 1;
                            }
                            if(((com2->kumi & (1 << k))>0)  ){
                                com[2] = near_start;
                                near_start += 1;
                                counter -= 1;
                            }
                        }
                    }else{
                        for(int k = product.head; k > 0 && counter > 0 ; k++){
                            if(((com->kumi & (1 << k))>0)  ){
                                com[0] = near_start;
                                near_start += 1;
                                counter -= 1;
                            }
                            if(((com1->kumi & (1 << k))>0)  ){
                                com[1] = near_start;
                                near_start += 1;
                                counter -= 1;
                            }
                            if(((com2->kumi & (1 << k))>0)  ){
                                com[2] = near_start;
                                near_start += 1;
                                counter -= 1;
                            }
                        }
                    }
                    if(
                    com[1]
                    
                }
                */
                
                
                if(servo_connection==true && load.spMode == SPEED_DAMP){
                    /*signal_flg=false;
                    signal_loop =0;
                    send_i(ON_CHECK,0,close_wait_flg);
                    while(check_signal()==false&&signal_loop<=10){
                        signal_loop+=1;
                        wait(0.1);
                    }*/
                    if(com->kumi>0){
                        old_combi = 0;
                    }
                    send_i(ON_CHECK,0,close_wait_flg);
                    wait_signal();
                    //if(moving2==0){
                        //send_i(SERVO_ON,com->kumi,0);while(check_signal()==false){wait(0.1);}
                        if(gLevel == 1){
                            for(int l=0;l<product.head;l++){
                                if((com->kumi & 1<<l) > 0){
                                    v.speak_numbers(l+1);
                                    v.gram_voice(int(cel.anteiWeight(l)));
                                }
                            }
                            v.speak_numbers(int(com->total));
                        }
                        send_i(SERVO_ON,com->kumi,close_wait_flg);wait_signal();
                        if(print_flg == true){send_combi(0,com->kumi);}
                        old_combi = old_combi | com->kumi;
                        no_combi_rap = 0;
                        if(cnt.max > 0) {
                            if(cnt.num < cnt.max) {
                                cnt.num++;
                                //残りカウント数に合わせて音声お知らせ
                                v.output_audio_Count(cnt.max,cnt.num);
                            }
                        }
                        if(go_flager==1){
                            send_i(FAKE_GO,0,0);while(check_signal()==false){wait(0.1);}
                        }
                        //上：１個成立
                        //下：１個以上成立
                        if(go_flager == COMV_SUN1    ||    go_flager == COMV_SUN2    || 
                           go_flager == COMV_SUNBUT1 ||    go_flager == COMV_SUNBUT2 || 
                           go_flager == COMV_SENS    ||    go_flager == COMV_NO_SEN    ){
                            if(load.target <= com1->total && com1->total <= res && ((com->kumi & com1->kumi) == 0)){
                                //send_i(SERVO_ON,com1->kumi,0);while(check_signal()==false){wait(0.1);}
                                if(gLevel == 1){
                                    for(int l=0;l<product.head;l++){
                                        if((com->kumi & 1<<l) > 0){
                                            v.speak_numbers(l+1);
                                            v.gram_voice(int(cel.anteiWeight(l)));
                                        }
                                    }
                                    v.speak_numbers(int(com1->total));
                                }
                                send_i(SERVO_ON,com1->kumi,close_wait_flg);wait_signal();
                                if(print_flg == true){send_combi(1,com1->kumi);}
                                old_combi = old_combi | com1->kumi;
                                no_combi_rap = 0;
                                if(cnt.max > 0) {
                                    if(cnt.num < cnt.max) {
                                        cnt.num++;
                                        //残りカウント数に合わせて音声お知らせ
                                        v.output_audio_Count(cnt.max,cnt.num);
                                    }
                                }
                            }
                            if(load.target <= com2->total && com2->total <= res && ((com->kumi & com1->kumi & com2->kumi) == 0)){
                                //send_i(SERVO_ON,com2->kumi,0);while(check_signal()==false){wait(0.1);}
                                if(gLevel == 1){
                                    for(int l=0;l<product.head;l++){
                                        if((com->kumi & 1<<l) > 0){
                                            v.speak_numbers(l+1);
                                            v.gram_voice(int(cel.anteiWeight(l)));
                                        }
                                    }
                                    v.speak_numbers(int(com2->total));
                                }
                                send_i(SERVO_ON,com2->kumi,close_wait_flg);wait_signal();
                                if(print_flg == true){send_combi(2,com2->kumi);}
                                old_combi = old_combi | com2->kumi;
                                no_combi_rap = 0;
                                if(cnt.max > 0) {
                                    if(cnt.num < cnt.max) {
                                        cnt.num++;
                                        //残りカウント数に合わせて音声お知らせ
                                        v.output_audio_Count(cnt.max,cnt.num);
                                    }
                                }
                            }
                            //printf("old_combi %04d :moving1 %04d \r\n",old_combi,moving1);
                            for(int i = 0; i < product.head; i++) {
                                if((((old_combi | moving1) & (1<<i)) > 0) && befour_anteiw[i] == 0){
                                    befour_anteiw[i] = cel.anteiWeight(i);
                                }else if(((old_combi | moving1) & (1<<i)) == 0){
                                    befour_anteiw[i] = 0;
                                }
                                //printf("b_a%02d : %4.1f  ",i+1,befour_anteiw[i]);
                                if(((i+1) %3) == 0){
                                    //printf("\r\n");
                                }
                            }
                        }
                        if(go_flager==2 || go_flager==3 || go_flager==4 || go_flager==5 || go_flager==6 ){
                            if(load.target <= com1->total && com1->total <= res){
                                nowlap = coulap;
                                coulap = 0;
                                total_kumi_cnt += 1;
                                kumi_value_data = float(int(com1->total*10))/10;
                                average_kumi_value = (((average_kumi_value * (total_kumi_cnt -1)) + kumi_value_data) / total_kumi_cnt);
                                //bit_num = bit_count(com->kumi);    //成立した個数の保存
                            }
                            if(load.target <= com2->total && com2->total <= res){
                                nowlap = coulap;
                                coulap = 0;
                                total_kumi_cnt += 1;
                                kumi_value_data = float(int(com1->total*10))/10;
                                average_kumi_value = (((average_kumi_value * (total_kumi_cnt -1)) + kumi_value_data) / total_kumi_cnt);
                                //bit_num = bit_count(com->kumi);    //成立した個数の保存
                            }
                        }
                        nowlap = coulap;
                        coulap = 0;
                        total_kumi_cnt += 1;
                        kumi_value_data = float(int(com->total*10))/10;
                        average_kumi_value = (((average_kumi_value * (total_kumi_cnt -1)) + kumi_value_data) / total_kumi_cnt);
                        //bit_num = bit_count(com->kumi);    //成立した個数の保存
                        moving1 = moving1 | old_combi;
                    //}
                }
                //wait(load.lightTime*0.1);
                for(int i = 0; i <= load.lightTime; i += 1) {
                    //send_i(ON_CHECK,0,close_wait_flg);
                    wait(0.1);
                    //再計算ボタンが押されたら、待機時間を待たずに終了する。
                    if(btn.down_press() == true) {
                        se.Touch_02();
                        i = load.lightTime + 1;
                       /* if(servo_connection==true && load.spMode == SPEED_DAMP){
                            moving2=99;
                            send_i(EJECT_RESET, 0, 0);
                            loop_a = 0;
                            while((check_signal()==false&&loop_a<=10) ||moving2!=0){
                                loop_a+=1;
                                wait(0.1);
                                printf("ASA\r\n");
                            }
                        }*/
                        
                        if(servo_connection==true && load.spMode == SPEED_DAMP){
                            //moving2=99;
                            //send_i(EJECT_RESET, 0, 0);
                            loop_a = 0;
                            while((check_signal()==false&&loop_a<=10)/* || moving2!=0*/){
                            //while(check_signal()==false&&loop_a<=10){
                                send_i(ON_CHECK,0,close_wait_flg);  //動作してる皿が無いか確認
                                loop_a+=1;
                                wait(0.1);
                            }
                            while(btn.down_press() == true) {wait(0.1);}
                        }
                    }
                    if(servo_connection==true && check_signal()==true && moving1 ==0 && load.spMode == SPEED_DAMP) {
                        i = load.lightTime + 1;
                    }
                }
                no_combi_v_flg = false;
                
                signal_flg=false;
                signal_loop =0;
                send_i(ON_CHECK,0,close_wait_flg);
                while(check_signal()==false&&signal_loop<=10){
                    signal_loop+=1;
                    wait(0.1);
                }
            }
        }
        //設定時間LED表示(ハイパーモード時のみ)
        else if(load.spMode == SPEED_HYPER){
            if(option.kokuti==1||option.kokuti==2||option.kokuti==3){
                se.Touch_02();
            }
            if(product.productType %100 == MATRIX || product.matrix_mode == MATRIX_ON){
                if(load.spMode == SPEED_HIGH){
                    for(int i = 0; i <= product.head ; i++){
                        if( (com->kumi & 1<<i)>0){
                            iro[i] = CG_01 + bit_count(com->kumi)-1;
                        }else{
                            iro[i] = C_NULL;
                        }
                    }
                }else{
                    led.REGIS3(com->kumi);
                }
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
                        if(load.spMode == SPEED_HIGH){
                            for(int i = 0; i <= product.head ; i++){
                                if( (com->kumi & 1<<i)>0){
                                    iro[i] = CG_01 + bit_count(com->kumi)-1;
                                }else{
                                    iro[i] = C_NULL;
                                }
                            }
                        }else{
                            led.REGIS3(com->kumi);
                        }
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
                //led.REGIS3(com->kumi);
                led.REGIS(com->kumi);
            }else{
                led.REGIS(com->kumi);
            }
            //第二上限値内の組み合わせ成立だったらLED点滅させる
            if((load.secondOnOff == 1 ||btn.get_lock_flg() == true) && (int(com->total) > res)){
                cnt_junsaiteki += 1;
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




