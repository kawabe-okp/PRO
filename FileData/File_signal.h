/*****************************************************************************
    サーボ値をローカルファイルに保存
*****************************************************************************/
void FileData::save_servo(){
    FILE *fp;
    
    if(( fp=fopen(NAME_SERVO,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "SERVO");
        exit(EXIT_FAILURE);
    }
    //ローカルファイルにパラメーターを書き込む
    for(int i = 0; i < MAX_HEAD; i++){
        fprintf(fp,"%d %d %d\r\n"
            ,servo_span[i][0]
            ,servo_span[i][1]
            ,servo_span[i][2]
        );
    }//for
    
    fclose(fp);
    fp = NULL;
}

/*****************************************************************************
    サーボ値のファイルデータ読み込み
*****************************************************************************/
void FileData::read_servo(){
    FILE *fp;
    
    // ファイルのオープン
    if ((fp = fopen(NAME_SERVO,"r")) == NULL) {
        fprintf(stderr, "%s open Err.\n", "SERVO");
        //exit(EXIT_FAILURE);
        file_lost_sig += 8192 ;
        save_servo();
    }else{
        //全ファイル読み出し
        for(int i = 0; i< MAX_HEAD; i++){
            fscanf(fp, "%d %d %d"
                ,&servo_span[i][0]
                ,&servo_span[i][1]
                ,&servo_span[i][2]
             );
            /*fscanf(fp, "%d"
                ,&servo_span[i][0]
             );
            fscanf(fp, "%d"
                ,&servo_span[i][1]
             );
            fscanf(fp, "%d"
                ,&servo_span[i][2]
             );*/
        }//for
        /*for(int i = 0; i< MAX_HEAD; i++){
            printf("%d %d %d\r\n"
                ,servo_span[i][0]
                ,servo_span[i][1]
                ,servo_span[i][2]
             );
        }*/

        //ファイルク]ローズ
        fclose(fp);
        fp = NULL;
            }
}
/*****************************************************************************
    サーボ設定情報の上書き
*****************************************************************************/
void FileData::save_dservo(){
    FILE *sfp;

    if ((sfp = fopen(NAME_DSERVO,"w")) == NULL) {
        fprintf(stderr, "%s write Err.\n", "DSERVO");
        exit(EXIT_FAILURE);
    }
        
    fprintf(sfp,"%d  %d  %d  %d  %d  %5d\r\n"
        ,course+(go_flager*10)
        ,course_rap
        ,san_coount_rap_set
        ,waiter_t
        ,landing_w
        ,kyousei_zero_kaisu
    );
    fclose(sfp);
    sfp = NULL;
}

/*****************************************************************************
    サーボ設定情報の読み取り
*****************************************************************************/
void FileData::read_dservo(){
    FILE *fp;
    int courseints=0;
    int composition=0;
    if ((fp = fopen(NAME_DSERVO,"r")) == NULL) {
        fprintf(stderr, "%s Read Err.\n", "DSERVO");
        //exit(EXIT_FAILURE);
        file_lost_sig += 16384;
        save_dservo();
    }else{
        fscanf(fp, "%d%d%d%d%d%d"
            ,&composition 
            ,&course_rap
            ,&san_coount_rap_set
            ,&waiter_t
            ,&landing_w
            ,&kyousei_zero_kaisu
        );
        courseints = composition % 10;
        go_flager  = composition / 10;
        if(course_rap<=0){
            course_rap=1;
        }else if(course_rap>50){
            course_rap=50;
        }
        if(san_coount_rap_set<=0){
            san_coount_rap_set=0;
        }else if(san_coount_rap_set>50){
            san_coount_rap_set=50;
        }
        if(courseints==0){
            course=false;
        }else{
            course=true;
        }
        if(go_flager>7){
            go_flager=1;
        }
        
        if(kyousei_zero_kaisu < 0){
            kyousei_zero_kaisu = 0;
        }else if(kyousei_zero_kaisu > 10000){
            kyousei_zero_kaisu = 10000;
        }
        
        
        
        
        //ファイルクローズ
        fclose(fp);
        fp = NULL;
    }
}
/*****************************************************************************
    サーボ動作回数の上書き
*****************************************************************************/
void FileData::save_hservo(){
    FILE *fp;
    
    if(( fp=fopen(NAME_HSERVO,"w")) == NULL){
        fprintf(stderr, "%s write Err.\n", "SERVO");
        exit(EXIT_FAILURE);
    }
    //ローカルファイルにパラメーターを書き込む
    
    fprintf(fp,"%d\r\n%d\r\n%d\r\n",servo_h_time[0],servo_h_time[1],servo_histry);
    
    for(int i = 0; i < MAX_HEAD; i++){
        fprintf(fp,"%d\r\n"
            ,servo_b_histry[i] + servo_n_histry[0][i] + servo_n_histry[1][i]
        );
    }//for
    
    fclose(fp);
    fp = NULL;
}

/*****************************************************************************
    サーボ動作回数の読み取り
*****************************************************************************/
void FileData::read_hservo(){
    FILE *fp;
    int d[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    // ファイルのオープン
    if ((fp = fopen(NAME_HSERVO,"r")) == NULL) {
        fprintf(stderr, "%s open Err.\n", "SERVO");
        //exit(EXIT_FAILURE);
        //file_lost_sig += 8192 ;
        for(int i = 0; i < MAX_HEAD; i++){
            servo_h_time[0]      = 0;
            servo_h_time[1]      = 0;
            servo_b_histry[i]    = 0;
            servo_n_histry[0][i] = 0;
            servo_n_histry[1][i] = 0;
        }
        save_servo();
    }else{
        //全ファイル読み出し
        
        for(int i = 0; i< MAX_HEAD+3; i++){
            fscanf(fp, "%d" ,&d[i]);
        }
        servo_h_time[0] = d[0];
        servo_h_time[1] = d[1];
        servo_histry    = d[2];
        for(int i = 0; i< MAX_HEAD; i++){
            servo_b_histry[i] = d[i+3];
        }
        /* 
        fscanf(fp,"%d%d",&servo_h_time[0],&servo_h_time[1]);
        for(int i = 0; i< MAX_HEAD; i++){
            fscanf(fp, "%d"
                ,&servo_b_histry[i]
            );
            if(servo_b_histry[i] > 9999999){
                servo_b_histry[i] = 9000000;
            }
        }//for
    */
        //ファイルクローズ
        printf("in\r");
        int num=0;
        printf("%02dH:%5d\r",num*2+1,servo_b_histry[num*2  ]);
        printf("%02dH:%5d\r",num*2+2,servo_b_histry[num*2+1]);
        num=1;
        printf("%02dH:%5d\r",num*2+1,servo_b_histry[num*2  ]);
        printf("%02dH:%5d\r",num*2+2,servo_b_histry[num*2+1]);
        num=2;
        printf("%02dH:%5d\r",num*2+1,servo_b_histry[num*2  ]);
        printf("%02dH:%5d\r",num*2+2,servo_b_histry[num*2+1]);
                
        
        fclose(fp);
        fp = NULL;
    }
}