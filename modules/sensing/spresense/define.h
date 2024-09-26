//#define系まとめ

// PIN定義：LED(GNSS電波状態)
#define LED_GNSS PIN_LED1
// PIN定義：LED(ELTRES状態)
#define LED_SND PIN_LED2
// PIN定義：LED(エラー状態)
#define LED_ERR PIN_LED3
// PIN定義；LED(距離判定状態)
#define LED_PROMITY PIN_LED0

// プログラム内部状態：初期状態
#define PROGRAM_STS_INIT (0)
// プログラム内部状態：起動中
#define PROGRAM_STS_RUNNING (1)
// プログラム内部状態：終了
#define PROGRAM_STS_STOPPED (3)


// プログラム内部状態
int program_sts = PROGRAM_STS_INIT;

// VL53L0X制御用インスタンス
static VL53L0X distance_sensor;
// 最新値（距離）
uint16_t last_distance = 0xFFFF;