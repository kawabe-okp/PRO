
char buf2;
int word_no=0;
char in_pc_sdata[100];
int intpcdata[100];

int head = 0;
float gram = 0;
int ASCII_a =  97;
int ASCII_s = 115;
int ASCII_d = 100;
int ASCII_f = 102;
int ASCII_g = 103;
int ASCII_h = 104;
int ASCII_0 =  48;
int ASCII_1 =  49;
int ASCII_2 =  50;
int PUSH_ESC = ASCII_a;
int PUSH_ENT = ASCII_s;
int PUSH_DOW = ASCII_d;
int PUSH__UP = ASCII_f;

int no = 0;

void pc_rx () {
    
    in_pc_sdata[word_no] = pc.getc();
    intpcdata[word_no] = in_pc_sdata[word_no];
    //buf2 = pc.getc();
    buf2 = in_pc_sdata[word_no];
    //pc.putc(buf2);
    if(word_no == 0){
        if((in_pc_sdata[0] == '\n'||in_pc_sdata[0] == '\r')){
            in_pc_sdata[0]=0;
            intpcdata[0]=0;
            buf2=0;
        }
        else if(48 <= in_pc_sdata[0] && in_pc_sdata[0] <= 57){
            in_pc_sdata[0]=0;
            intpcdata[0]=0;
            buf2=0;
        }
        else if(int(buf2)==PUSH_ESC){  //a
            //pc.printf("\r\nesc\r\n");
            if(now_TC_mode >= 1){
                btn.force_ESC();
            }else{
                btn.set_ESC();
            }
        }
        else if(int(buf2)==PUSH_ENT){    //s
            //pc.printf("\r\nent\r\n");
            btn.set_ENTER();
        }
        else if(int(buf2)==PUSH_DOW){    //d
            //pc.printf("\r\ndown\r\n");
            if(now_TC_mode == 1){
                btn.force_UP();
            }else{
                btn.set_UP();
            }
        }
        else if(int(buf2)==PUSH__UP){    //f
            //pc.printf("\r\nup\r\n");
            btn.set_DOWN();
        }
        else if(int(buf2)==ASCII_h){    //h
            word_no+=1;
        }
        else if(int(buf2)==ASCII_g){    //h
            word_no+=1;
        }/*
        else if(int(buf2)==ASCII_1){     //1
            //pc.printf("\r\n1\r\n");
            data_one_print = true;
        }
        else if(int(buf2)==ASCII_2){     //2
            //pc.printf("\r\n2\r\n");
            data_two_print = true;
        }*/
        else if(int(buf2)=='p'){     //
            if(print_flg == false){  
                print_flg = true;
                pc.printf("print_on ");
            }else{
                print_flg = false;
                pc.printf("print_off");
            }
            /*
            if(test_print_mode == false){
                pc.printf("\r\ntest_print_mode_ON\r\n");
                test_print_mode = true;
            }else{
                pc.printf("\r\ntest_print_mode_OFF\r\n");
                test_print_mode = false;
            }
            */
        }
        else if(int(buf2)=='l'){     //p
            pc.printf("\r\npush_g\r\n");
            if(now_TC_mode == 0){
                pc.printf("\r\npassword\r\n");
                go_password = true;
            }
        }
        else if(int(buf2)==121){     //y
            pc.printf("\r\ntest_print_mode_OFF\r\n");
            test_print_mode = false;
        }
        else if(int(buf2)==113){    //q
            pc.printf("\r\n2\r\n");
            if(now_TC_mode==0){
                password();
            }
        }
        else {
            word_no = 0;

        }
        /*
        else if(int(buf2)==49){    //q
            pc.printf("\r\n1head\r\n");
            if(image_weight < 2){

            }if(image_weight > (load.target-1)){
                l. = load.target+1.0;
                if(image_weight > (load.target-1)){
                    load.target = 0.0;
                }
            }
                    
        }*/

        buf2 =0;

    }else{

        if((in_pc_sdata[word_no] == '\n'||in_pc_sdata[word_no] == '\r')/*&&word_no>=7/*||word_no==20*/
        ){
            //if(word_no>=4){
                if(in_pc_sdata[word_no] == '\n'||in_pc_sdata[word_no] == '\r' ) {
                    in_pc_sdata[word_no] = 0;
                    word_no-=1;
                }
                if(in_pc_sdata[word_no] == '\n'||in_pc_sdata[word_no] == '\r' ) {
                    in_pc_sdata[word_no] = 0;
                    word_no-=1;
                }
                
                printf("%d %d : %d %d %d %d . %d\r\n",intpcdata[1],intpcdata[2],intpcdata[3],intpcdata[4],intpcdata[5],intpcdata[6],intpcdata[7]);
                //printf("%d %d %d %d %d %d\r\n",intpcdata[1],intpcdata[2],intpcdata[3],intpcdata[4],intpcdata[5],intpcdata[6]);
                for(int o=1; o<=word_no+5; o++) {
                    //if(o==0){
                        //printf("a1\r\n");
                    //}else{
                        //intpcdata[o] = intpcdata[o]-40;
                    //}
                    
                    //if(48 <= in_pc_sdata[o] && in_pc_sdata[o] <= 57){;
                        if(in_pc_sdata[o]==48){
                            intpcdata[o]=0;
                        }
                        else if(in_pc_sdata[o]==49){
                            intpcdata[o]=1;
                        }
                        else if(in_pc_sdata[o]==50){
                            intpcdata[o]=2;
                        }
                        else if(in_pc_sdata[o]==51){
                            intpcdata[o]=3;
                        }
                        else if(in_pc_sdata[o]==52){
                            intpcdata[o]=4;
                        }
                        else if(in_pc_sdata[o]==53){
                            intpcdata[o]=5;
                        }
                        else if(in_pc_sdata[o]==54){
                            intpcdata[o]=6;
                        }
                        else if(in_pc_sdata[o]==55){
                            intpcdata[o]=7;
                        }
                        else if(in_pc_sdata[o]==56){
                            intpcdata[o]=8;
                        }
                        else if(in_pc_sdata[o]==57){
                            intpcdata[o]=9;
                        }
                        else {
                            intpcdata[o]=0;
                        }
                    //}
                    if(in_pc_sdata[o] == '\n' || in_pc_sdata[o] == '\r' ) {
                        intpcdata[o]=0;
                    }
                    /**/
                    //printf("%d %d ",word_no,in_pc_sdata[word_no]);
                    //printf("%d ",o);
                }
                //printf("%d %d %d %d %d %d . %d\r\n",intpcdata[1],intpcdata[2],intpcdata[3],intpcdata[4],intpcdata[5],intpcdata[6],intpcdata[7]);
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
                //gram = float(intpcdata[1] * 1000 + intpcdata[2] * 100 + intpcdata[3] * 10 + intpcdata[4]) + float(intpcdata[7]) * 0.1;
                if(in_pc_sdata[0] == 'h'){
                    //printf("head:%2d %2d\r\n",intpcdata[1],intpcdata[2]);
                    head = float(intpcdata[1] * 10 + intpcdata[2]);
                    if(head < 0){
                        head = 0;
                    }else if (head > product.head){
                        head = product.head - 1;
                    }
                    printf("head:%2d \r\n",head);
                }
                if(in_pc_sdata[0] == 'g'){
                    gram = float(intpcdata[1] * 100 + intpcdata[2] * 10 + intpcdata[3]  + float(intpcdata[4]) * 0.1);
                    if(head < 0){
                        head = 0;
                    }else if (head > product.head){
                        head = product.head - 1;
                    }
                    printf("head:%2d gram: %4.2f\r\n",head,gram);
                    cel.set_imageweight(head, gram);
                }
                printf("%d %d : %d %d %d %d . %d\r\n",intpcdata[1],intpcdata[2],intpcdata[3],intpcdata[4],intpcdata[5],intpcdata[6],intpcdata[7]);
                //if(gram > 0){ 
                //}else{

                //}


                //processing(intpcdata[1],intpcdata[2],intpcdata[3],intpcdata[4],intpcdata[5],intpcdata[6],intpcdata[7],intpcdata[8],intpcdata[9]);
                //リセット処理
            //}
            for(int o=0; o<=word_no; o++) {
                in_pc_sdata[o]=0;intpcdata[0]=0;
            }
            word_no = 0;
        } else {
            if(word_no==0&&in_pc_sdata[0]!=34){
                word_no=0;in_pc_sdata[0]=0;intpcdata[0]=0;
            }else{
                word_no+=1;
            }
        }
        //printf("%d\r\n",word_no);
        //if(word_no>=4){printf("%d %d : %d %d %d %d . %d\r\n",intpcdata[1],intpcdata[2],intpcdata[3],intpcdata[4],intpcdata[5],intpcdata[6],intpcdata[7]);}


    }
    /*
    buf2 = pc.getc();
    //pc.putc(buf2);
    if(int(buf2)==PUSH_ESC){//a
        pc.printf("\r\nesc\r\n");
        if(now_TC_mode >= 1){
            btn.force_ESC();
        }else{
            btn.set_ESC();
        }
    }
    else if(int(buf2)==PUSH_ENT){//s
        pc.printf("\r\nent\r\n");
        btn.set_ENTER();
    }
    else if(int(buf2)==PUSH_DOW){//d
        pc.printf("\r\ndown\r\n");
        if(now_TC_mode == 1){
            btn.force_UP();
        }else{
            btn.set_UP();
        }
    }
    else if(int(buf2)==PUSH__UP){//f
        pc.printf("\r\nup\r\n");
        btn.set_DOWN();
    }
    else if(int(buf2)==104){//h
        pc.printf("\r\npush_g\r\n");
        if(now_TC_mode == 0){
            pc.printf("\r\npassword\r\n");
            go_password = true;
        }
    }
    else if(int(buf2)==49){//1
        pc.printf("\r\n1\r\n");
        data_one_print =true;
    }
    else if(int(buf2)==50){//2
        pc.printf("\r\n2\r\n");
        data_two_print =true;
    }
    else if(int(buf2)==103){//g
        pc.printf("\r\n2\r\n");
        if(print_flg == false){  
            print_flg = true;
            pc.printf("print_on ");
        }else{
            print_flg = false;
            pc.printf("print_off");
        }
    }
    buf2 =0;
    */
}
