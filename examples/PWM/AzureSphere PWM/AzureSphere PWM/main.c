// LED PWM CONTROLLER 2 channel 0 is used by LED1 Red channel, channel 1 is used by LED1 Green channel, 
// channel 2 is used by LED1 Blue channel, channel 3 is used by NETWORKING LED. 

#include <applibs/log.h>
#include <applibs/pwm.h>
#include "hw/pwm_definition.h"

#define MaxPWM 3

int main(void)
{
    // 関数の実行結果を渡す変数
    int result;
    // forループで使用する際のledのindex
    int numOfled = 0;

    // ハイ状態とロー状態の合計時間 (ナノ秒)
    static const unsigned int fullCycleNs = 1 * 1000 * 1000;

    // 期間中にpolarityで指定した状態になっている時間 (ナノ秒)の変数
    // Whileループ内でインクリメントされる
    static unsigned int dutyCycleNs = 0;

    // dutyCycleNsを徐々に大きくしていく際の足し幅。
    // これを大きくすると、一気に輝度が大きくなる
    static const unsigned int stepIncrementNs = 1;

    // The polarity is inverted because LEDs are driven low
    struct PwmState led_state = { .period_nsec = fullCycleNs, .dutyCycle_nsec = 0,
                                  .polarity = PWM_POLARITY_INVERSED , .enabled = true};
 

    int pwmfd = PWM_Open(LED_PWM); 
    if (pwmfd == -1) {
        Log_Debug("PWM_Open Error");
        return errno;
    }

    // すべてのLEDを初期化し、消灯する
    // 事前のled_stateの定義で、dutyを0にしているので、すべてのLEDは点灯しない
    for (numOfled = 0; numOfled <= MaxPWM; numOfled++) {
        result = PWM_Apply(pwmfd, numOfled, &led_state);
    }
    
    while (true) {
        
        // dutyサイクルが指定最大値未満であれば、dutyをステップ幅増やす
        if (dutyCycleNs < fullCycleNs) {
            dutyCycleNs += stepIncrementNs;
        }
        // dutyサイクルが指定最大値を超えたので、dutyを0に初期化する
        else {
            dutyCycleNs = 0;
        }
        // led_stateを更新
        led_state.dutyCycle_nsec = dutyCycleNs;

        // 更新したled_stateを適用
        // じんわり青色のLEDが点灯する。蛍を思わせる風流さ
        result = PWM_Apply(pwmfd, 2, &led_state);
        if (result == -1) {
            Log_Debug("PWM_Apply Error");
            return errno;
        }
        
    }


    return 0;
}
