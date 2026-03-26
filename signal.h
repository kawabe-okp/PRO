
//signal.h   
typedef enum kk{
    MIN    = 150
    ,NOMAL =305
    ,UP1   =310
    ,UP    =310
    ,MAX   =485
}kk;
void send_f(int command, int head, float data1);
void send_i(int command, int head, int data1);
void signal1_in ();
void processing(int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8,int a9);
bool check_signal(); //信号が来たか確認するフラグ。送信して待つときに使う
void send_para(int command, int head, bool course, int course_rap, int go_flager);
void ddd();void pc_rx();
void send_paramaters();
void send_paramaters2();
void send_paramaters3();
void send_paramaters4();
void servo_test();
void servo_set();
void set_servo_span();

char in_sdata[100];
int intdata[100];
int input_data[10];
int i = 0;
int word=0;
int Base_no=64;  //1バイトにどれだけ詰め込むか(進数:64)
bool signal_flg=false;  //何か信号が来たことを認識させるフラグ
bool course = false;    //(未使用)12ヘッド側から動かすか(true)
int course_rap=0;       //(未使用)1ヘッド毎にタイミングをずらして動かす際の待ち時間
int san_coount_rap_set=0;//
int go_flager=0;       //ボタン式、自動排出、サン付、(時限式は未定)
int waiter_t=10;      //皿が閉じるまでの時間
int landing_w=20;       //閉じてからの待ち時間
int stop_place=1;       //サン付きボタンの排出終ってから止まるまでの位置
int servo_head=12;      //(未使用)ヘッド数の指定(今後あるかも)
int moving0=0;           //移動中(まだ排出中)ヘッド
int moving1=0;           //移動中(まだ排出中)ヘッド
int movingb1=0;          //moving1を更新する直前の値(動作したサーボのｶｳﾝﾄ用)
int moving2=0;           //移動中(組合せに入れられない)ヘッド
extern int ALL_HEAD;

bool initial_set_end=true; //初期データ全てもらい終わったらtrue,マスター側は常にtrue
/*
int servo_span[16][2] = {{MIN,UP},{MIN,UP},{MIN,UP},{MIN,UP},{MIN,UP},{MIN,UP},
                         {MIN,UP},{MIN,UP},{MIN,UP},{MIN,UP},{MIN,UP},{MIN,UP},
                         {MIN,UP},{MIN,UP},{MIN,UP},{MIN,UP}};
*/
int servo_span[16][3] = {{MIN,UP,UP1},{MIN,UP,UP1},{MIN,UP,UP1},{MIN,UP,UP1},{MIN,UP,UP1},{MIN,UP,UP1},
                         {MIN,UP,UP1},{MIN,UP,UP1},{MIN,UP,UP1},{MIN,UP,UP1},{MIN,UP,UP1},{MIN,UP,UP1},
                         {MIN,UP,UP1},{MIN,UP,UP1},{MIN,UP,UP1},{MIN,UP,UP1}};
int befoure_sig = 0; //直前の信号を記憶する(返答待ちの時用)
int befoure_data = 0; //直前の信号を記憶する(返答待ちの時用)

extern int befoure_check;      //一個前に確認した、排出待ちのビット
extern int now_____check;      //一個前に確認した、排出待ちのビット
extern int check_same_count;      //排出待ちのビットが同一だった時のカウント
extern int check_same_max;         //排出待ちのビットが同一だった時の最大


void send_combi(int now_stage,int combi_kumi){
    int bit[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    if(now_stage != 3){
        for(int i = 0; i < 12;i++){
            if((combi_kumi & (1<<i))>0){
                bit[i] = 1;
            }
        }
    }
    signal_in1.printf("\r\n");
    signal_in1.printf("combi:%d:\r\n",now_stage);
    signal_in1.printf(";;;%d%d%d%d%d%d%d%d%d%d%d%d\r\n",1,2,3,4,5,6,7,8,9,0,1,2);
    signal_in1.printf(":::%d%d%d%d%d%d%d%d%d%d%d%d\r\n",
    bit[0],bit[1],bit[2],bit[3],bit[4],bit[5],bit[6],bit[7],bit[8],bit[9],bit[10],bit[11]);
    //printf("\r\n");
}
void send_data(int head,float data1,float data2,int end_flager){
    signal_in1.printf("\t%02dH\t:\t%6.1f\t:\t%7.5f\r\n",head,data1,data2);
    if(end_flager>0){
        signal_in1.printf("\r\n");
    }
}

bool check_signal(){
    if(signal_flg==true){
        signal_flg=false;
        return true;
    }
    return false;
}
    
void wait_signal(){
    int signal_loop = 0;
    int loop_end = 50;
    signal_flg = false;
    while(signal_flg==false && signal_loop <= loop_end){
        signal_loop+=1;
        wait(0.02);
    }
    if(signal_loop >= loop_end){
        printf("NO_SIG\r\n");
        if(befoure_sig == ON_CHECK || befoure_sig == FAKE_GO){
            send_i(befoure_sig,befoure_data,0);
            int signal_loop = 0;
            while(check_signal()==false && signal_loop <= loop_end){
                signal_loop+=1;
                wait(0.02);
            }
        }
    }
    if(befoure_sig == ON_CHECK && kyousei_zero_kaisu > 0){
        if(close_wait_flg > 0){
            /*
            printf("no_zero_counter_befoure\r\n");
            printf("close_wait_flg %3d|| ",close_wait_flg);
            for(int head=0 ; head < product.head ; head++){
                printf("%02d:%03d | ",head,no_zero_counter[head]);
                if(head % 3 == 2){
                }
            }
            printf("\r\n");
            */
            for(int head=0 ; head < product.head ; head++){
                if(((close_wait_flg & (1 << head)) > 0) && (moving2 & (1 << head)) >0){
                    se.Touch_01();   //ボタン操作時の効果音
                    se.Touch_01();   //ボタン操作時の効果音
                    lcd.cls();
                    lcd.locate(0,0);
                    lcd.printf("ZERO_SET");
                    /**/
                    //wait(3); //ゼロ点取る代わりの待ち時間
                    /**/
                    set_zero_span_onehead(head);
                    /**/
                    if(btn.any_press()!=true){
                        kyousei_zero_counter[head]=0;
                    }
                    //v.speak_numbers(head+1);
                    close_wait_flg = close_wait_flg ^ (close_wait_flg & (1 << head));
                    zero_between_kyousei[head] = servo_n_histry[0][head] + servo_n_histry[1][head];
                    no_zero_counter[head] = 0;
                    lcd.cls();
                }
            }
            /*
            printf("no_zero_counter_next\r\n");
            printf("close_wait_flg %3d|| ",close_wait_flg);
            for(int head=0 ; head < product.head ; head++){
                printf("%02d:%03d | ",head,no_zero_counter[head]);
                if(head % 3 == 2){
                }
            }
            printf("\r\n");
            printf("end\r\n");
            */
        }
        
    }
}

void ddd(){
    pc.printf("A\r\n");
    pc_rx ();
}

void signal1_in ()
{
    //printf("a");
    //timer_osv();
    //printf("06\r\n");
    in_sdata[word] = signal_in1.getc();
    intdata[word] = in_sdata[word];
    
    //pc.putc(in_sdata[word]);
    //signal_in1.printf("%d",in_sdata[word]);
    if((in_sdata[word] == '\n'||in_sdata[word] == '\r')&&word>=9/*||word==20*/) {
        if(in_sdata[word] == '\n'||in_sdata[word] == '\r' ) {
            in_sdata[word] = 0;
            word-=1;
        }
        if(in_sdata[word] == '\n'||in_sdata[word] == '\r' ) {
            in_sdata[word] = 0;
            word-=1;
        }
        //printf("%d ",word);
        //printf("\r\n");/**/
        for(int o=0; o<=word; o++) {
            if(o==0){
                //printf("a1\r\n");
            }else{
                intdata[o] = intdata[o]-40;
            }
            //printf("%d %d ",word,in_sdata[word]);
            //printf("%d ",o);
            if(in_sdata[o] != '\n' && in_sdata[o] != '\r' ) {
                //printf("%3d ",intdata[o]);
            }
        }
        //内容選別
        //文字列 ※基本的に0x28+値で送受信する
        // 0番目      :先頭を示す文字(")
        // 1番目      :コマンド
        // 2,3番目    :ヘッド(2進数)
        // 4,5,6,7番目  :値(4はマイナス符号、5,6が整数、7が小数点)
        //
        //
        //コマンド一覧
        //90;サーボ動作信号(ヘッドを2進数で複数指示)
        //99:動作中ヘッド確認(親→子) ：　98:動作中ヘッド連絡(子→親)(ヘッドを2進数で複数指示)
        //100:サーボMIN指定(ヘッドは1個ずつ)
        //102:サーボUP指定(ヘッドは1個ずつ)
        //
        processing(intdata[1],intdata[2],intdata[3],intdata[4],intdata[5],intdata[6],intdata[7],intdata[8],intdata[9]);
        //リセット処理
        for(int o=0; o<=word; o++) {
            in_sdata[o]=0;intdata[0]=0;
        }
        word = 0;
    } else {
        if(word==0&&in_sdata[0]!=34){
            word=0;in_sdata[0]=0;intdata[0]=0;
        }else{
            word+=1;
        }
    }
    //timer_isv();
}
void processing(int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8,int a9){
        input_data[0]=a1;
        input_data[1]=a2*Base_no*Base_no+a3*Base_no+a4;
        input_data[2]=a6*Base_no*Base_no+a7*Base_no+a8;
        
        //printf("a:%d\r\n",a1);
        //printf("command %3d\r\n",input_data[0]);
        if(input_data[0]==MOVE_CK){ 
            //printf("MOVE_CK\r\n");
            send_i(MOVE_OK, 0, 0);
        }else if(input_data[0]==MOVE_OK){ 
            //printf("MOVE_OK\r\n");
            signal_flg=true;
        }else if(input_data[0]==SERVO_ON){  //サーボを動かす指令だった場合
            /*servo_setting = input_data[1];
            for(int i=0;i<=servo_head;i++){
                if((1<<i & servo_setting)>0){  
                    waiter_r[i]= 9999;
                }
            }*/
            send_i(MOVE_CK,0,0);
            signal_flg=false;
            //printf("\r\nSERVO_ON %5d\r\n",input_data[1]);
        }else if(input_data[0]==MI_VALUE || input_data[0]==UP_VALUE){
            /*if(a5/10==0){
                if(a5%10==0){      input_data[2]=   (a6*Base_no*Base_no+a7*Base_no+a8);
                }else if(a5%10==1){input_data[2]=-1*(a6*Base_no*Base_no+a7*Base_no+a8);
                }else{             input_data[2]=   (a6*Base_no*Base_no+a7*Base_no+a8);
                }
            }else{
                if(a5%10==0){      input_data[2]=   (a6*Base_no*Base_no+a7*Base_no+a8);
                }else if(a5%10==1){input_data[2]=-1*(a6*Base_no*Base_no+a7*Base_no+a8);
                }else{             input_data[2]=   (a6*Base_no*Base_no+a7*Base_no+a8);
                }
            }
            input_data[3]=intdata[9];
            
            if(input_data[0]==MI_VALUE){
                min_r[input_data[1]]=input_data[2];
            }else if(input_data[0]==UP_VALUE){
                max_r[input_data[1]]=input_data[2];
            }
            send_i(MOVE_OK, 0, 0);
            signal_flg=true;
            printf("\r\nset_data%2d:%5d:%5d.%d\r\n",input_data[0],input_data[1],input_data[2],input_data[3]);
            */
        }else if(input_data[0]==PARA_REQUEST){
            printf("PARA_REQUEST\r\n");
            if(servo_settting == false && servo_first_settting == true){
                
                printf("PARA_REQUEST_go\r\n");
                servo_set();
                //send_paramaters();
                wait(0.3);
                send_i(END_VALUE,0,0);
            }
        }else if(input_data[0]==END_VALUE){
            //printf("END_VALUE\r\n");
            send_i(END_VALUE_OK, 0, 0);
            initial_set_end=true;
        }else if(input_data[0]==END_VALUE_OK){
            //printf("END_VALUE_OK\r\n");
            signal_flg=true;
        }else if (initial_set_end==false){
            send_i(PARA_REQUEST, 0, 0);
        }else if(input_data[0]==ON_CHECK){
            /*end_i(CHECK_RES, check_sv4(), check_sv());
            signal_flg=false;*/
            //printf("\r\nCHECKmoving %5d\r\n",input_data[1]);
        }else if(input_data[0]==CHECK_RES){
            //printf("CHECK_RES\r\n");
            //befoure_check  = now_____check;
            
            //printf("\r\nCHECK_RES %5d\r\n",input_data[1]);
            movingb1=moving1;
            moving1=input_data[1];
            moving1 = moving1 & ALL_HEAD;
            /*now_____check = moving1;
            if(now_____check > 0 && now_____check == befoure_check){
                check_same_count += 1;
                if(check_same_count > check_same_max){
                    check_same_max = check_same_count;
                }
            }else{
                check_same_count = 0;
            }*/
            //printf("00moving%5d befoure%5d| same %4d|max %4d\r\n",now_____check , befoure_check , check_same_count , check_same_max);
            for(int a=0 ; a < product.head ; a++){
                //if(((movingb1 & (1 << a)) > 0) && ((moving1 & (1 << a)) == 0 )){
                if(((moving1 & (1 << a)) == 0 ) && (((movingb1 & (1 << a)) > 0) || servo_n_histry[1][a] ==1 )){
                    servo_n_histry[0][a] += 1;
                    servo_n_histry[1][a] =  0;
                    //printf("out %02dH:%5d:%6d",a,servo_n_histry[0][a  ],servo_b_histry[a ] + servo_n_histry[0][a  ]);
                }else if(((movingb1 & (1 << a)) == 0) && ((moving1 & (1 << a)) > 0 )){
                    servo_n_histry[1][a] =  1;
                }
            }
            moving2=a6*Base_no*Base_no+a7*Base_no+a8;
            
            for(int a=0 ; a < product.head ; a++){
                if((moving2 & (1 << a)) > 0 ) {
                    cel.break_antei(a);
                    antei_flug[a]=0;
                    //cel.set_head(a);
                    //cel.setAin_zero(a);
                }
                if((movingb1 & (1<<a))>0){
                    cel.break_antei(a);
                    antei_flug[a]=0;
                    //cel.set_head(a);
                    //cel.setAin_zero(a);
                }
            }
            
            
            signal_flg=true;
            //printf("moving1 %5d  || moving2 %5d\r\n",moving1,moving2);
        }else if(input_data[0]==PARA_SET){
            /*servo_head=input_data[1];      //(未使用)ヘッド数の指定(今後あるかも)
            if(a5==0){  course = false;}    //(未使用)12ヘッド側から動かすか(true)
            else{       course = true;}
            course_rap=a6*Base_no*Base_no+a7*Base_no+a8;       //(未使用)1ヘッド毎にタイミングをずらして動かす際の待ち時間
            signal_flg=false;*/
            //printf("\r\n1 shead:%5d  course:%5d course_rap:%5d\r\n",servo_head,course,course_rap);
        }else if(input_data[0]==EJECT_RESET){
            movingb1 = 0;
            moving1  = 0;
            moving2  = 0;
            /*servo_setting＝0；
            for(int i=0;i<=servo_head;i++){
                waiter_r[i]= 0;
                set_data(i,min_r[i],50);
            }
            while(check_sv()>0) { wait(0.01);}*/
        }else{// if(input_data[0]==DATA_SET){
            if(a5/10==0){
                if(a5%10==0){      input_data[2]=   (a6*Base_no*Base_no+a7*Base_no+a8);
                }else if(a5%10==1){input_data[2]=-1*(a6*Base_no*Base_no+a7*Base_no+a8);
                }else{             input_data[2]=   (a6*Base_no*Base_no+a7*Base_no+a8);
                }
            }else{
                if(a5%10==0){      input_data[2]=   (a6*Base_no*Base_no+a7*Base_no+a8);
                }else if(a5%10==1){input_data[2]=-1*(a6*Base_no*Base_no+a7*Base_no+a8);
                }else{             input_data[2]=   (a6*Base_no*Base_no+a7*Base_no+a8);
                }
            }
            input_data[3]=intdata[9];
            signal_flg=true;
            //printf("\r\nset_data%2d:%5d:%5d.%d\r\n",input_data[0],input_data[1],input_data[2],input_data[3]);
            //どのデータを入力するか・・・に続く
        }//else{}
        input_data[0]=0;
        input_data[1]=0;
        input_data[2]=0;
        input_data[3]=0;
}
void send_f(int command, int head, float data1){
    int h1, h2, h3;
    int f=0,d1=0, d2=0, d3=0, d4=0;
    int ten=0;  //0.0001=10^4: 10の何乗必要か
    h1=head/(Base_no*Base_no);
    h2=(head-h1*Base_no*Base_no)/Base_no;
    h3=head%Base_no;
    
    if(data1<0){
        d1=1;
        data1 = data1*-1;
    }else{
        d1=0;
    }
    for(int i=0;i<=10;i++){
        if((data1-int(data1))==0){
        i=99;
        }
    }
    d1=int(data1)/(Base_no*Base_no);
    d2=(int(data1)-d1*Base_no*Base_no)/Base_no;
    d3=int(data1)%Base_no;
    signal_in1.printf("%c%c%c%c%c%c%c%c%c%c\r\n",0x00+34,0x28+command,0x28+h1,0x28+h2,0x28+h3,0x28+f,0x28+d1,0x28+d2,0x28+d3,0x28+d4);/**/
}
void send_i(int command, int head, int data1){
    /**/int h1, h2, h3;
    int f=0,d1=0, d2=0, d3=0;
    befoure_sig = command;
    befoure_data = head;
    
    
    
    
    
    
    h1=head/(Base_no*Base_no);
    h2=(head-h1*Base_no*Base_no)/Base_no;
    h3=head%Base_no;
    
    if(data1<0){
        f=1;
        data1 = data1*-1;
    }else{
        f=0;
    }
    //d3=data1%Base_no;
    //pc.printf("bbb1%c%c%c%c%c%c%c\r\n",0x28+command,0x28+h1,0x28+h2,0x28+d1,0x28+d2,0x28+d3,0x28);
    //pc.printf("bbb2");
    if(command==ON_CHECK){
        //for(int a=0 ; a < product.head ; a++){
        //    if((close_wait_flg2 & (1 << a)) > 0 && close_wait_flg == 0){
        //        close_wait_flg  = (1 << a);
        //        close_wait_flg2 = close_wait_flg2 ^ (1 << a);
        //    }
        //}
        
    }
    if(command==SERVO_ON){
        int kyousei_zero_bit = 0;
        int kyousei_zero_max_head = -1;
        int kyousei_zero_max_data = 0;
        moving1 = moving1 | head;
        for(int a=0 ; a < product.head ; a++){
            if((head & (1 << a)) > 0 ){
                servo_n_histry[1][a] =  1;
                //printf("in  %02dH:%5d:%6d",a,servo_n_histry[0][a  ],servo_b_histry[a ] + servo_n_histry[0][a  ]);
                if(kyousei_zero_kaisu > 0){
                    //if((servo_n_histry[0][a] + servo_n_histry[1][a]) % kyousei_zero_kaisu == 0){
                    if((servo_n_histry[0][a] + servo_n_histry[1][a]) - zero_between_kyousei[a] >= kyousei_zero_kaisu){
                        /*if(close_wait_flg == 0){
                            close_wait_flg  = (1 << a);
                        }else{
                            close_wait_flg2  = close_wait_flg2 | (1 << a);
                        }*/
                        kyousei_zero_bit  = kyousei_zero_bit | (1 << a);
                        kyousei_zero_counter[a]++;
                    }
                    /*if((close_wait_flg2 & (1 << a)) > 0 && close_wait_flg == 0){
                        close_wait_flg  = (1 << a);
                        close_wait_flg2 = close_wait_flg2 ^ (1 << a);
                    }*/
                }else{close_wait_flg=0;}
            }
        }
        close_wait_flg = ALL_HEAD & close_wait_flg;
        if(close_wait_flg == 0 && kyousei_zero_kaisu > 0){
            kyousei_zero_bit = 0;
            kyousei_zero_max_head = -1;
            kyousei_zero_max_data = 0;
            for(int a=0 ; a < product.head ; a++){
                if((head & (1 << a)) > 0 ){
                    if(kyousei_zero_max_data < kyousei_zero_counter[a]){
                        kyousei_zero_max_head = a;
                        kyousei_zero_max_data = kyousei_zero_counter[a];
                    }
                }
            }
            if(kyousei_zero_max_data > 0){
                close_wait_flg = close_wait_flg | (1 << kyousei_zero_max_head);
            }
        }

        data1 = close_wait_flg;

    }
    
    
    
    d1=data1/(Base_no*Base_no);
    d2=(data1-d1*Base_no*Base_no)/Base_no;
    d3=data1%Base_no;
    
    
    
    
    
    signal_flg=false;
    signal_in1.printf("%c%c%c%c%c%c%c%c%c%c\r\n",0x00+34,0x28+command,0x28+h1,0x28+h2,0x28+h3,0x28+f,0x28+d1,0x28+d2,0x28+d3,0x28);/**/

}
void send_para(int command, int head, bool course, int course_rap, int go_flager){
    
    /**/int h1, h2, h3;
    int f=0,d1=0, d2=0, d3=0;
    h1=head/(Base_no*Base_no);
    h2=(head-h1*Base_no*Base_no)/Base_no;
    h3=head%Base_no;

    if(course==false){  f =0+(go_flager*10);}
    else{               f =1+(go_flager*10);}
    d1=course_rap/(Base_no*Base_no);
    d2=(course_rap-d1*Base_no*Base_no)/Base_no;
    d3=course_rap%Base_no;
    signal_in1.printf("%c%c%c%c%c%c%c%c%c%c\r\n",0x00+34,0x28+command,0x28+h1,0x28+h2,0x28+h3,0x28+f,0x28+d1,0x28+d2,0x28+d3,0x28);/**/
    /*signal_flg=false;
    while(signal_flg==false){
        send_i(END_VALUE,0,0);
        //printf("f\r\n");
        wait(0.5);
    }*/
    
    
    
    
    
}
void send_paramaters(){
    send_para(PARA_SET ,product.head , course, course_rap, go_flager);
}
void send_paramaters2(){
    //send_para(PARA_SET2,product.Array,      0, san_coount_rap_set, 0);
    send_para(PARA_SET2,              1,      0, san_coount_rap_set, 0);
}
void send_paramaters3(){
    send_para(PARA_SET3,  landing_w  ,      0, waiter_t, 0);
}
void send_paramaters4(){
    send_para(PARA_SET4,  stop_place  ,      0, 0, 0);
}
void wait_res(){
    while(signal_flg==false){
        wait(0.1);
    }
}
void set_servo_span(){
    for(int i=0;i<product.head;i++){
        send_i(MI_VALUE,i,servo_span[i][0]);
        if(servo_first_settting == false){
            wait_res();
        }else{
            wait(0.3);
        }
        send_i(UP_VALUE,i,servo_span[i][1]);
        if(servo_first_settting == false){
            wait_res();
        }else{
            wait(0.3);
        }
    }
}