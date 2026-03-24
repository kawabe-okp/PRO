const char NAME_SERVO[]  ="/local/SERVO.TXT";//サーボ特殊仕様設定 19/06/25
const char NAME_DSERVO[]  ="/local/SERVOD.TXT";//サーボ特殊仕様設定 19/06/25
const char NAME_HSERVO[]  ="/local/SERVOH.TXT";//サーボ特殊仕様設定 21/08/19

extern int servo_span[16][3];
extern bool course;    //(未使用)12ヘッド側から動かすか(true)
extern int course_rap;       //(未使用)1ヘッド毎にタイミングをずらして動かす際の待ち時間
extern int san_coount_rap_set; //san_coount_rapを固定値にするか規定値にするか(0:オート)
extern int go_flager;       //ボタン式、自動排出、サン付、(時限式は未定)

extern int waiter_t;      //皿が閉じるまでの時間
extern int landing_w;       //閉じてからの待ち時間



extern int servo_histry;
extern int servo_n_histry[2][12];
extern int servo_b_histry[12];
extern int servo_h_time[2];

extern int kyousei_zero_kaisu;