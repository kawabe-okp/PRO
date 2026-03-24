#include "Button.h"

//コンストラクタ:ピン設定
Button::Button(PinName d0, PinName d1, PinName d2, PinName d3): _esc(d0),
_ent(d1), _up(d2) ,_down(d3){
    //プルアップ
    _esc.mode(PullUp);
    _ent.mode(PullUp);
    _up.mode(PullUp);
    _down.mode(PullUp);
    
    //ボタン押下状態を初期化
    btn.esc_old = 1;
    btn.ent_old = 1;
    btn.up_old = 1;
    btn.down_old = 1;
    
    btn.esc_new = 1;
    btn.ent_new = 1;
    btn.up_new = 1;
    btn.down_new = 1;
    
    //ボタン保持を初期化
    ini();
    
    //初期化
    _cntDown = 0;
    _cntUp = 0;
    
    //押しっぱなしを判断する回数
    KEEP = 80;
    
    //押されたボタンの状態を記憶する
    _keepState = BTN_NONE;
    _operationLock = false;
}

/*****************************************************************************
    下ボタン押下取得
*****************************************************************************/
bool Button::down_press(){

    if(_down == 0 || once == BTN_DOWN || force == BTN_DOWN){
        _Pres = BTN_DOWN;
        once  = BTN_NONE;
        force = BTN_NONE;
        printf("press%2d|one%2d|force%2d",_Pres,once,force);
        return true;
    }
    return false;
}

/*****************************************************************************
    上ボタン押下取得
*****************************************************************************/
bool Button::up_press(){

    if(_up == 0 || once == BTN_UP || force == BTN_UP){
        _Pres = BTN_UP;
        once  = BTN_NONE;
        force = BTN_NONE;
        printf("press%2d|one%2d|force%2d",_Pres,once,force);
        return true;
    }
    return false;
}

/*****************************************************************************
    決定ボタン押下取得
*****************************************************************************/
bool Button::ent_press(){

    if(_ent == 0 || once == BTN_ENT || force == BTN_ENT){
        _Pres = BTN_ENT;
        once  = BTN_NONE;
        force = BTN_NONE;
        printf("press%2d|one%2d|force%2d",_Pres,once,force);
        return true;
    }
    return false;
}
/*****************************************************************************
    戻るボタン押下取得
*****************************************************************************/
bool Button::esc_press(){

    if(_esc == 0 || once == BTN_ESC || force == BTN_ESC){
        _Pres = BTN_ESC;
        once  = BTN_NONE;
        force = BTN_NONE;
        printf("press%2d|one%2d|force%2d",_Pres,once,force);
        return true;
    }
    return false;
}
/*****************************************************************************
    いずれかのボタン押下取得
*****************************************************************************/
bool Button::any_press(){

    if(_esc == 0 || _ent == 0 || _up == 0 || _down == 0 || once  != BTN_NONE || force != BTN_NONE){
        return true;
    }
    return false;
}

/*****************************************************************************
    強制的にENTERを押したことにする(次に進む)
*****************************************************************************/
void Button::force_ENTER(){
    _Pres = BTN_ENT;
    force = BTN_ENT;
}
/*****************************************************************************
    強制的にESCを押したことにする(戻らせる)
*****************************************************************************/
void Button::force_ESC(){
    _Pres = BTN_ESC;
    force = BTN_ESC;
}
/*****************************************************************************
    強制的にDOWN(上ボタン)を押したことにする(次に進む)
*****************************************************************************/
void Button::force_DOWN(){
    _Pres = BTN_DOWN;
    force = BTN_DOWN;
}
/*****************************************************************************
    強制的にUP(下ボタン)を押したことにする(次に進む)
*****************************************************************************/
void Button::force_UP(){
    _Pres = BTN_UP;
    force = BTN_UP;
}

/*****************************************************************************
    強制ONをやめる
*****************************************************************************/
void Button::end_force(){
    _Pres = BTN_NONE;
    force = BTN_NONE;
    once  = BTN_NONE; 
    //_keepState = BTN_NONE;
    //_operationLock = false;
}



/*****************************************************************************
    疑似的に各ボタンを押したことにする(次に進む)
*****************************************************************************/
void Button::set_ENTER(){
    //_Pres = BTN_ENT;
    once = BTN_ENT;
}
void Button::set_ESC(){
    //_Pres =  BTN_ESC;
    once = BTN_ESC;
}
void Button::set_DOWN(){
    //_Pres = BTN_DOWN;
    once = BTN_DOWN;
}
void Button::set_UP(){
    //_Pres = BTN_UP;
    once = BTN_UP;
}

/*****************************************************************************
    チャタリング対策用にスイッチの状態(NEW)を取得
*****************************************************************************/
void Button::new_btn(){
    btn.esc_new = _esc;
    btn.ent_new = _ent;
    btn.up_new = _up;
    btn.down_new = _down;
    
    //printf("%2d:%2d:%2d:%2d\r\n",btn.esc_new,btn.ent_new,btn.up_new,btn.down_new);
    _Pres = check_button();
    if(once != BTN_NONE){
        _Pres = once;
        once = BTN_NONE;
    }
    if(force != BTN_NONE){
        _Pres = force;
        force = force;
        //force = BTN_NONE;
    }
}

//ボタンの状態を保持する
void Button::new_keep_btn(){
    new_btn();
    _keepState = _Pres; //押されたボタンの状態を取得
}


/* **************************************
一時的に準最適値モードになっている場合は、上ボタン操作を無効にする
_operationLockがFalseの場合はボタン操作有効とする。
上ボタンを押下した場合に、一時的に準最適値となる。押している間は上ボタン操作無効。
上ボタンを離した際に、上ボタン操作有効とする。
**************************************** */
//ロック状態を取得する
bool Button::get_lock_flg(){
    return _operationLock;
}

//操作をロック
void Button::operation_lock(){
    _operationLock = true;   
}

//ロック状態を解除
void Button::operation_unlock(){
    _operationLock = false;   
}

//押されたボタンの状態を取得
ButtonPress Button::get_keep_btn(){
    return _keepState;
}

/*****************************************************************************
    チャタリング対策用にスイッチの状態(OLD)を取得
*****************************************************************************/
void Button::old_btn(){
    wait(0.005);
    btn.esc_old = btn.esc_new;
    btn.ent_old = btn.ent_new;
    btn.up_old = btn.up_new;
    btn.down_old = btn.down_new;
}

//内部押下ボタン値を初期化
void Button::ini(){
    _Pres = BTN_INI;
    force = BTN_NONE;
    once  = BTN_NONE; 
}

/*****************************************************************************
    押下されたボタン値を取得(チャタリング対策済)
    注意) 必ずnew_btn();の後、old_btn()の前で使用すること
    new_btn();   btnの状態を更新
       ↓
    get_btn();   押下されたbtnを取得
       ↓
    old_btn();   btn状態を保存
*****************************************************************************/
ButtonPress Button::get_btn(){
    return _Pres;
}

/*****************************************************************************
    押下されたボタン値を取得(チャタリング対策済)
    注意) 必ずnew_btn();の後、old_btn()の前で使用すること
    new_btn();   btnの状態を更新
       ↓
    get_one_push();   1回目の押下されたボタンのみ取得
       ↓
    old_btn();   btn状態を保存
*****************************************************************************/
ButtonPress Button::get_one_push(){
    switch(_Pres){
        case BTN_UP:
        case BTN_DOWN:
        case BTN_ENT:
        case BTN_ESC:
        case BTN_INI:   //初期状態のときも返す
            return _Pres;
    }
    return BTN_NONE;
}

/*****************************************************************************
    ボタンの状態から押されたボタン値を返す
*****************************************************************************/
ButtonPress Button::check_button(){
    
    //上ボタン押されていなければ初期化
    if( btn.up_new == 1){
        _cntUp = 0;
    }
    
    //下ボタン押されていなければ初期化
    if( btn.down_new == 1){
        _cntDown = 0;
    }
    
    //ESCキー
    if(btn.esc_old==1 && btn.esc_new ==0 && btn.up_old ==1 && btn.down_old ==1){
        printf("BTN_ESC\r\n");
        return BTN_ESC;
    }
    //Enterキー
    //else if((btn.ent_old==1 && btn.ent_new==0) ||
    //        (btn.ent_old==0 && btn.ent_new==0)){
    else if(btn.ent_old==1 && btn.ent_new==0){
        
        printf("BTN_ENT\r\n");
        return BTN_ENT;
    }
    //UP KEY確認
    
    else if(btn.up_old ==1 && btn.up_new==0 && btn.esc_old==1){
    //else if(((btn.up_old ==1 && btn.up_new==0 && btn.esc_old==1) ||
    //        (btn.up_old ==0 && btn.up_new==0 && btn.esc_old==1)) && _Pres == BTN_NONE){
        printf("BTN_UP\r\n");
        return BTN_UP;
    }
    //DOWN KEY確認
    else if(btn.down_old==1 && btn.down_new==0 && btn.esc_old==1){
    //else if(((btn.down_old==1 && btn.down_new==0 && btn.esc_old==1)||
    //        (btn.down_old==0 && btn.down_new==0 && btn.esc_old==1) )&& _Pres == BTN_NONE){
        printf("BTN_DOWN\r\n");
        return BTN_DOWN;
    }
    //UP KEY(押しっぱなし)
    else if(btn.up_old ==0 && btn.up_new==0 && btn.esc_old==1){
    //else if(btn.up_old ==0 && btn.up_new==0 && btn.esc_old==1 && _Pres == BTN_UP){
        _cntUp++;
        
        //一定時間ボタンが押下された
        if( KEEP <= _cntUp){
            _cntUp = KEEP;
            printf("BTN_KEEP_UP\r\n");
            return BTN_KEEP_UP;
        }
        //return BTN_UP;
    }
    //DOWN KEY(押しっぱなし)
    else if(btn.down_old==0 && btn.down_new==0 && btn.esc_old==1){
    //else if(btn.down_old==0 && btn.down_new==0 && btn.esc_old==1 && _Pres == BTN_DOWN){
        _cntDown++;
        //一定時間ボタンが押下された
        if( KEEP <= _cntDown){
            _cntDown = KEEP;
            printf("BTN_KEEP_DOWN\r\n");
            return BTN_KEEP_DOWN;
        } 
        //return BTN_DOWN;
        
    }  
    //ESCキーとDownキーを同時押し(1回)
    else if((btn.down_old==1 && btn.down_new==0 && btn.esc_old==0)||
            (btn.down_old==0 && btn.down_new==0 && btn.esc_old==0)){
        return BTN_ESC_DOWN;
    }
    //ESCキーとUpキーを同時押し(1回)
    else if((btn.up_old ==1 && btn.up_new==0 && btn.esc_old==0) ||
            (btn.up_old ==0 && btn.up_new==0 && btn.esc_old==0)){
        return BTN_ESC_UP;
    }
    //ESCキーとDownキーを同時押し(押しっぱなし)
    else if(btn.down_old==0 && btn.down_new==0 && btn.esc_old==0){
        return BTN_KEEP_ESC_DOWN;
    }
    //ESCキーとUpキーを同時押し(押しっぱなし)
    else if(btn.up_old ==0 && btn.up_new==0 && btn.esc_old==0){
        return BTN_KEEP_ESC_UP;
    }
    
    //押されたキーがない
    return BTN_NONE;
}

//escボタンを押されたか確認
bool Button::esc(){
    if(_Pres == BTN_ESC){
        return true;
    }
    return false;
}
//escボタンを押されたか確認
bool Button::ent(){
    if(_Pres == BTN_ENT){
        return true;
    }
    return false;
}

//プッシュ音を鳴らして欲しいときにtrueを返す
bool Button::push_sound(){
    
    switch(_Pres){
        case BTN_UP:
        case BTN_DOWN:
        case BTN_ENT:
        case BTN_ESC:
            return true;
            
        default:
            return false;  
    }
}

//各ボタン操作に応じて値を変更
void Button::puls_or_minus(int* num){
    
    //printf("press%2d|one%2d|force%2d\r\n",_Pres,once,force);
    switch( _Pres ){
        
        case BTN_UP:                //増加
        case BTN_KEEP_UP:
            *num -= 1;
            break;
        case BTN_DOWN:              //減少
        case BTN_KEEP_DOWN:
            *num += 1;
            break;
            
        case BTN_ESC_UP:            //増加大
        case BTN_KEEP_ESC_UP:
            *num -= 1;
            *num -= 10; 
            break;
            
        case BTN_ESC_DOWN:          //減少大
        case BTN_KEEP_ESC_DOWN:
            *num += 1;
            *num += 10;
            break;
    }
}

//各ボタン操作に応じて値を変更
void Button::puls_or_minus(float* num){
    //printf("press%2d|one%2d|force%2d",_Pres,once,force);
    switch( _Pres ){
        
        case BTN_UP:                //増加
        case BTN_KEEP_UP:
            *num -= 0.1;
            break;
        case BTN_DOWN:              //減少
        case BTN_KEEP_DOWN:
            *num += 0.1;
            break;
            
        case BTN_ESC_UP:            //増加大
        case BTN_KEEP_ESC_UP:
            *num -= 0.1;
            *num -= 1; 
            break;
            
        case BTN_ESC_DOWN:          //減少大
        case BTN_KEEP_ESC_DOWN:
            *num += 0.1;
            *num += 1;
            break;
    }
    *num = float(*num*10)/10;
    
    
}

//各ボタン操作に応じて値を変更
//通常の数値アップ
//高速の数値アップ
void Button::puls_or_minus(int* num, int nomalUp, int highUp){
    
    switch( _Pres ){
        
        case BTN_UP:                //増加
        case BTN_KEEP_UP:
            *num -= nomalUp;
            break;
        case BTN_DOWN:              //減少
        case BTN_KEEP_DOWN:
            *num += nomalUp;
            break;
            
        case BTN_ESC_UP:            //増加大
        case BTN_KEEP_ESC_UP:
            *num -= nomalUp;
            *num -= highUp; 
            break;
            
        case BTN_ESC_DOWN:          //減少大
        case BTN_KEEP_ESC_DOWN:
            *num += nomalUp;
            *num += highUp;
            break;
    }
}

//上下キーのみ
void Button::next_or_back(int* num,int min, int max){
    switch( _Pres ){
    case BTN_UP:                //増加
        *num -= 1;
        break;
    case BTN_DOWN:              //減少
        *num += 1;
        break;
    }
    ini_min_max(num, min ,max);
}

//規定する最小値、最大値をオーバーしたら値を初期化
void Button::ini_min_max(int *num, int min, int max){
    if(*num > max)     {*num = min;}
    if(*num < min)      {*num = max;}
}