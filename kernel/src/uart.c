/**
 * @file uart.c
 *
 * @brief uart control functions  
 *
 * @date oct 17, 2026
 *
 * @author ashley leung (aleung2), fiona shi (fionas)
 */

#include <unistd.h>
#include <rcc.h>
#include <uart.h>
#include <uart_polling.h>
#include <nvic.h>
#include <gpio.h>

#define UNUSED __attribute__((unused))

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

/** @brief TXE interrupt enable in CR1*/
#define TXEIE (1 << 7)

/** @brief RXNE interrupt enable in CR1*/
#define RXNEIE (1 << 5)

/** @brief length of interrupt buffers*/
#define BUFFER_SIZE 256

/** @brief buffer*/
typedef struct {
  char data[BUFFER_SIZE];
  uint32_t head_idx;
  uint32_t tail_idx;
  uint32_t occupied;
} buffer_t;

static buffer_t TX_BUFFER;
static buffer_t RX_BUFFER;

// true if buffer is full
uint32_t buf_full(buffer_t *buf){
  return (buf->occupied == BUFFER_SIZE);
}

// true if buffer is empty
uint32_t buf_empty(buffer_t *buf){
  return (buf->occupied == 0);
}

int32_t buf_put(buffer_t *buf, char c){
  if (buf_full(buf)){
    return -1;
  }

  buf->data[buf->head_idx] = c;
  // avoid modulo function
  // buf size power of 2 -> use & (buffer_size - 1)
  buf->head_idx = (buf->head_idx + 1) & (BUFFER_SIZE - 1);
  buf->occupied++;
  return 1;
}

int32_t buf_get(buffer_t *buf, char *c){
  if (buf_empty(buf)){
    return -1;
  }

  *c = buf->data[buf->tail_idx];
  buf->tail_idx = (buf->tail_idx + 1) & (BUFFER_SIZE - 1);
  buf->occupied--;
  return 0;
}


void uart_init(UNUSED int baud){
  struct uart_reg_map *uart = UART2_BASE;
  struct rcc_reg_map *rcc = RCC_BASE;
  rcc->apb1_enr |= RCC_EN;

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

  // receive interrupt enable on by default
  uart->CR1 |= RXNEIE;

  // transmit interrupt enable off by default
  uart->CR1 &= ~TXEIE;  

  // ennable uart2 interrupt in nvic
  nvic_irq(38, IRQ_ENABLE);

  // initialize buffers
  for (uint32_t i = 0; i < BUFFER_SIZE; i++){
    RX_BUFFER.data[i] = 0;
    TX_BUFFER.data[i] = 0;
  }

  RX_BUFFER.head_idx = 0;
  RX_BUFFER.tail_idx = 0;
  RX_BUFFER.occupied = 0;
  TX_BUFFER.head_idx = 0;
  TX_BUFFER.tail_idx = 0;
  TX_BUFFER.occupied = 0;
  
  return;
}

int uart_put_byte(UNUSED char c){
  return -1;
}

int uart_get_byte(UNUSED char *c){
  return -1;
}

void uart_irq_handler(){

}

void uart_flush(){

}