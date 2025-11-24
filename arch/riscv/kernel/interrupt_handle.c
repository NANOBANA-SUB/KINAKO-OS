#include "interrupt_handle.h"
#include "timer.h"
#include "proc.h"

void handle_timer_interrupt(void)
{
    // 次のタイマーをセットする
    timer_set_next();
}