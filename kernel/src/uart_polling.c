#include <unistd.h>

#include <gpio.h>
#include <rcc.h>
#include <uart_polling.h>

/** @brief The UART register map. */
struct uart_reg_map {
    volatile uint32_t SR;   /**< Status Register */
    volatile uint32_t DR;   /**<  Data Register */
    volatile uint32_t BRR;  /**<  Baud Rate Register */
    volatile uint32_t CR1;  /**<  Control Register 1 */
    volatile uint32_t CR2;  /**<  Control Register 2 */
    volatile uint32_t CR3;  /**<  Control Register 3 */
    volatile uint32_t GTPR; /**<  Guard Time and Prescaler Register */
};

/** @brief Base address for UART2 */
#define UART2_BASE  (struct uart_reg_map *) 0x40004400

/** @brief Enable  Bit for UART Config register */
#define UART_EN (1 << 13)

/** @brief Enable  Bit for RCC clock */
#define RCC_EN (1 << 17)

/** @brief Port for UART TX line */
#define TX_PORT GPIO_A

/** @brief Pin for UART TX line */
#define TX_PIN 2

/** @brief Port for UART TX line */
#define RX_PORT GPIO_A

/** @brief Pin for UART TX line */
#define RX_PIN 3

/** @brief USARTDIV value used to calculate baud rate */
#define USARTDIV 0x8A

/** @brief Transmitter enable bit for UART Control Register 1*/
#define TE (1 << 3)

/** @brief Receiver enable bit for UART Control Register 1*/
#define RE (1 << 2)

/** @brief Transmit data reg empty bit for UART Status Register*/
#define TXE (1 << 7)

/** @brief Read data register not empty bit for UART Status Register*/
#define RXNE (1 << 5)



/**
 * @brief initializes UART to given baud rate with 8-bit word length, 1 stop bit, 0 parity bits
 *
 * @param baud Baud rate
 */
void uart_polling_init (int baud){
    (void) baud; /* This line is simply here to suppress the Unused Variable Error. */
    //              /* You should remove this line in your final implementation */

    struct uart_reg_map *uart = UART2_BASE;
    struct rcc_reg_map *rcc = RCC_BASE;
    rcc->apb1_enr |= RCC_EN;

    // Not baud rate, calculated USARTDIV number
    uart->BRR = USARTDIV;

    // init call for UART2 TX LINE
    gpio_init(TX_PORT, TX_PIN, MODE_ALT, OUTPUT_PUSH_PULL, 
              OUTPUT_SPEED_LOW, PUPD_NONE, ALT7);
    // init call for UART RX LINE
    gpio_init(RX_PORT, RX_PIN, MODE_ALT, OUTPUT_OPEN_DRAIN, 
              OUTPUT_SPEED_LOW, PUPD_NONE, ALT7);

    uart->CR1 |= UART_EN; 
    uart->CR1 |= TE;
    uart->CR1 |= RE;

    return;
}

/**
 * @brief transmits a byte over UART
 *
 * @param c character to be sent
 */
void uart_polling_put_byte (char c){
    // (void) c;
    struct uart_reg_map *uart = UART2_BASE;

    while (!(uart->SR & TXE)){
        continue;
    }

    uart->DR = c;

    return;
}

/**
 * @brief receives a byte over UART
 */
char uart_polling_get_byte () {
    struct uart_reg_map *uart = UART2_BASE;

    while (!(uart->SR & RXNE)){
        continue;
    }

    volatile char c = uart->DR;
    return c;
}
