typedef enum signal_name{
     
     MOVE_CK        =  0  //起動したか確認  
    ,WAIT_SIG       //=  1      //メイン→コントローラへ、起動時の確認を待つ指令
    ,WAIT_END_SIG   //=  2      //メイン→コントローラへ、起動時の確認を待つ指令
    ,MOVE_OK        //=  3  //起動した応答
    ,SERVO_ON       //=  4  //指定ヘッドをONにする
    ,CLOSE_SIG      //=     //解放し続けた箇所のヘッドを閉じる
    ,SERVO_1        //=  5  //指定ヘッドをiヘッドだけONにする
    ,FAKE_GO        //=  6  //疑似サンの信号
    ,ON_CHECK       //=  7 //動作中ヘッドの確認依頼
    ,CHECK_RES      //=  8 //動作中ヘッドの確認反応
    ,PARA_SET       //=  9   //各パラメータ送信cservo_headヘッド数ourse順番起動の向きcourse_rap起動時間のピッチ
    ,PARA_SET2      //= 10
    ,PARA_SET3
    ,MI_VALUE       //= 11    //サーボが下がった時の値(1ヘッド毎)
    ,UP_VALUE       //= 12   //サーボが上がった時の値(1ヘッド毎)
    ,OP_VALUE
    ,CL_VALUE
    ,END_VALUE      //= 13  //初期設定データ終了信号
    ,END_VALUE_OK   //= 14  //初期設定データ終了信号
    ,PARA_REQUEST   //= 15 //スレーブ側からのリクエスト
    ,TEST_SET       //= 16   ,TEST_MOVE      //= 14
    ,TEST_MOVE      //= 17
    ,TEST_END       //= 18    ,ALL_MIN        //= 16
    ,ALL_MIN        //= 19    ,SETING_S       //= 18  //サーボを無条件に移動させる(開始)
    ,ALL_UP         //= 20    ,SETING_S       //= 20  //サーボを無条件に移動させる(開始)
    ,SETING_S       //= 21 //サーボを無条件に移動させるモード(開始)
    ,SETING_E       //= 22  //サーボを無条件に移動させるモード(終了)
    ,EJECT_RESET    //= 23   ,PARA_SET2      //= 21
    ,KEIRYO_ST      //= 24 //計量スタート
    ,KEIRYO_EN      //= 25 //計量エンド
     
     
    
    
    
}signal_name;

