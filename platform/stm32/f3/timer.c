#include "platform/timer.h"
#include "kernel/memory.h"

#define OFFSET_CR1     0
#define OFFSET_CR2     1
#define OFFSET_CMSR    2
#define OFFSET_DIER    3
#define OFFSET_SR      4
#define OFFSET_EGR     5
#define OFFSET_CCMR1   6
#define OFFSET_CCMR2   7
#define OFFSET_CCER    8
#define OFFSET_CNT     9
#define OFFSET_PSC    10
#define OFFSET_RCR    11
#define OFFSET_ARR    12
#define OFFSET_CCR1   13
#define OFFSET_CCR2   14
#define OFFSET_CCR3   15
#define OFFSET_CCR4   16
#define OFFSET_BDTR   17
#define OFFSET_DCR    18
#define OFFSET_DMAR   19
#define OFFSET_OR     20
#define OFFSET_CCMR3  21
#define OFFSET_CCR5   22
#define OFFSET_CCR6   23

#ifdef HAS_TIM1
    static timer_iface *tim_iface_1  = NULL;
#endif
#ifdef HAS_TIM2
    static timer_iface *tim_iface_2  = NULL;
#endif
#ifdef HAS_TIM3
    static timer_iface *tim_iface_3  = NULL;
#endif
#ifdef HAS_TIM4
    static timer_iface *tim_iface_4  = NULL;
#endif
#ifdef HAS_TIM6
    static timer_iface *tim_iface_6  = NULL;
#endif
#ifdef HAS_TIM7
    static timer_iface *tim_iface_7  = NULL;
#endif
#ifdef HAS_TIM8
    static timer_iface *tim_iface_8  = NULL;
#endif
#ifdef HAS_TIM15
    static timer_iface *tim_iface_15 = NULL;
#endif
#ifdef HAS_TIM16
    static timer_iface *tim_iface_16 = NULL;
#endif
#ifdef HAS_TIM17
    static timer_iface *tim_iface_17 = NULL;
#endif
#ifdef HAS_TIM20
    static timer_iface *tim_iface_20 = NULL;
#endif

/**
 * generic timer context
 */
typedef struct timer_ctx_t {
    timer_iface iface;

    uint32_t *register_base;
} timer_ctx;

/**
 * @brief      get current value of timer counter
 *
 * @param      iface  timer interface
 *
 * @return     counter value
 */
static uint32_t timer_get_counter(timer_iface *iface) {
    timer_ctx *ctx = (timer_ctx*) iface;

    return *(ctx->register_base + OFFSET_CNT);
}

/**
 * @brief      get address of timer interface ptr by it's number
 *
 * @param[in]  timer_base  desired timer
 *
 * @return     address of timer ptr
 */
static timer_iface **timer_base_to_iface(timer timer_base) {
        switch (timer_base) {
#ifdef HAS_TIM1
        case TIM1:
            return &tim_iface_1;
#endif
#ifdef HAS_TIM2
        case TIM2:
            return &tim_iface_2;
#endif
#ifdef HAS_TIM3
        case TIM3:
            return &tim_iface_3;
#endif
#ifdef HAS_TIM4
        case TIM4:
            return &tim_iface_4;
#endif
#ifdef HAS_TIM6
        case TIM6:
            return &tim_iface_6;
#endif
#ifdef HAS_TIM7
        case TIM7:
            return &tim_iface_7;
#endif
#ifdef HAS_TIM8
        case TIM8:
            return &tim_iface_8;
#endif
#ifdef HAS_TIM15
        case TIM15:
            return &tim_iface_15;
#endif
#ifdef HAS_TIM16
        case TIM16:
            return &tim_iface_16;
#endif
#ifdef HAS_TIM17
        case TIM17:
            return &tim_iface_17;
#endif
#ifdef HAS_TIM20
        case TIM20:
            return &tim_iface_20;
            break;
#endif
        default:
            break;
    }

    return NULL;
}

/**
 * @brief      enable/disable timer
 *
 * @param      iface    timer interface
 * @param[in]  enabled  enable/disable flag
 */
static void timer_enable(timer_iface *iface, int enabled) {
    timer_ctx *ctx = (timer_ctx*) iface;
    if (!ctx) {
        return;
    }

    if (enabled) {
        //! enabling timer
        *(ctx->register_base + OFFSET_CR1) |=  BIT0;
    } else {
        //! disabling timer
        *(ctx->register_base + OFFSET_CR1) &= ~BIT0;
    }
}

/**
 * @brief      wait until timer counts to value
 *
 * @param      iface        timer interface
 * @param[in]  time_value   value multiplier
 * @param[in]  time_points  time value to wait
 */
static void timer_delay(timer_iface *iface, timer_time_value time_value,
    uint32_t time_points) {
    timer_ctx *ctx = (timer_ctx*) iface;

    uint32_t delay_value = time_points * time_value;
    uint32_t cnt = 0;
    uint32_t saved_cnt = 0;

    timer_enable(iface, 1);

    while (cnt < delay_value) {
        saved_cnt = *(ctx->register_base + OFFSET_CNT);
        while(saved_cnt == *(ctx->register_base + OFFSET_CNT));
        cnt++;
    }

    timer_enable(iface, 0);
}

/**
 * @brief      free allocated for timer memory
 *
 * @param      iface  timer interface
 */
static void timer_destroy(timer_iface *iface) {
    if (!iface) {
        return;
    }

    cell_free(iface);
}

/**
 * @brief      initializes timer interface
 *
 * @param[in]  timer_base  desired timer
 *
 * @return     timer interface ptr
 */
static timer_iface *timer_iface_create(timer timer_base) {
    timer_ctx *ctx = cell_alloc(sizeof(timer_ctx));
    if (!ctx) {
        return NULL;
    }
    timer_iface *iface = &ctx->iface;

    iface->delay       = timer_delay;
    iface->get_counter = timer_get_counter;
    iface->enable      = timer_enable;
    iface->destroy     = timer_destroy;

    //! set register base address
    ctx->register_base = (uint32_t*) timer_base;

    switch (timer_base) {
#ifdef HAS_TIM1
        case TIM1:
            RCC_APB2ENR   |= BIT11;
            break;
#endif
#ifdef HAS_TIM2
        case TIM2:
            RCC_APB1ENR   |= BIT0;
            break;
#endif
#ifdef HAS_TIM3
        case TIM3:
            RCC_APB1ENR   |= BIT1;
            break;
#endif
#ifdef HAS_TIM4
        case TIM4:
            RCC_APB1ENR   |= BIT2;
            break;
#endif
#ifdef HAS_TIM6
        case TIM6:
            RCC_APB1ENR   |= BIT4;
            break;
#endif
#ifdef HAS_TIM7
        case TIM7:
            RCC_APB1ENR   |= BIT5;
            break;
#endif
#ifdef HAS_TIM8
        case TIM8:
            RCC_APB2ENR   |= BIT13;
            break;
#endif
#ifdef HAS_TIM15
        case TIM15:
            RCC_APB2ENR   |= BIT16;
            break;
#endif
#ifdef HAS_TIM16
        case TIM16:
            RCC_APB2ENR   |= BIT17;
            break;
#endif
#ifdef HAS_TIM17
        case TIM17:
            RCC_APB2ENR   |= BIT18;
            break;
#endif
#ifdef HAS_TIM20
        case TIM20:
            RCC_APB2ENR   |= BIT20;
            break;
#endif
        default:
            iface->destroy(iface);
            return NULL;
    }
    return iface;
}

/**
 * @brief      initialize timer by given configuration
 *
 * @param      iface   timer interface
 * @param[in]  config  timer configuration
 *
 * @return     success of initialization
 */
static int timer_iface_init(timer_iface *iface, const timer_config *config) {
    timer_ctx *ctx = (timer_ctx*) iface;
    if (!ctx || !config) {
        return 0;
    }

    uint32_t tmp_reg = *(ctx->register_base + OFFSET_CR1);
    tmp_reg &= ~(BIT4 | BIT5 | BIT6);
    tmp_reg |= config->counter_mode | BIT11;

    *(ctx->register_base + OFFSET_CR1) = tmp_reg;
    *(ctx->register_base + OFFSET_ARR) = config->period;
    *(ctx->register_base + OFFSET_PSC) = config->prescaler;
    *(ctx->register_base + OFFSET_CNT) = 0;

    return 1;
}

timer_iface *timer_iface_get(timer timer_base) {
    return *timer_base_to_iface(timer_base);
}

void timer_init(timer timer_base, const timer_config *config) {
    timer_iface **tim_iface = timer_base_to_iface(timer_base);

    *tim_iface = timer_iface_create(timer_base);
    if (!timer_iface_init(*tim_iface, config)) {
        (*tim_iface)->destroy(*tim_iface);
    }
}
