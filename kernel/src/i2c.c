#include <unistd.h>

#include <gpio.h>
#include <i2c.h>
#include <rcc.h>
#include <printk.h>

/** @brief The I2C register map. */
struct i2c_reg_map {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
    volatile uint32_t FLTR;
};

/** @brief Base address for I2C */
#define I2C_BASE  (struct i2c_reg_map *) 0x40005400

/** @brief Enable  Bit for RCC clock */
#define RCC_EN (1 << 21)

/** @brief Acknowledge Enable Bit for CR1*/
#define I2C_ACK (1 << 10)

/** @brief Peripheral Enable Bit for CR1*/
#define I2C_PE (1)

/** @brief 16 MHz Freq for CR2 */
#define I2C_FREQ (1 << 4)

/** @brief pclock / 100khz for CCR */
#define I2C_CCR (0x50)

/** @brief addressing mode for CR2 */
#define I2C_ADDMODE ~(1 << 15)

/** @brief Busy bit for SR2*/
#define I2C_BUSY (1 << 1)

/** @brief Start condition for CR1 */
#define I2C_START (1 << 8)

/** @brief Stop condition for CR1 */
#define I2C_STOP (1 << 9)

/** @brief Start bit for SR1 */
#define I2C_SB (1)

/** @brief Data register empty bit for SR1 */
#define I2C_TxE (1 << 7)

/** @brief Byte transfer finished bit for SR1 */
#define I2C_BTF (1 << 2)

/** @brief Addr sent bit for SR1 */
#define I2C_ADDR (1 << 1)

/** @brief Acknowledge failure bit for SR1 */
#define I2C_AF (1 << 10)

/** @brief number of attempts before giving up on a write */
#define I2C_MAX_RETRIES 5

// d14
#define SDA_PORT GPIO_B
#define SDA_PIN 9

// d15 
#define SCL_PORT GPIO_B
#define SCL_PIN 8

void i2c_master_init(uint16_t clk){
    (void) clk; /* This line is simply here to suppress the Unused Variable Error. */
                /* You should remove this line in your final implementation */
    struct i2c_reg_map *i2c = I2C_BASE;
    struct rcc_reg_map *rcc = RCC_BASE;
    rcc->apb1_enr |= RCC_EN;

    gpio_init(SDA_PORT, SDA_PIN, MODE_ALT, OUTPUT_OPEN_DRAIN,
              OUTPUT_SPEED_LOW, PUPD_NONE, ALT4);
    gpio_init(SCL_PORT, SCL_PIN, MODE_ALT, OUTPUT_OPEN_DRAIN,
              OUTPUT_SPEED_LOW, PUPD_NONE, ALT4);


    i2c->CR1 |= (1 << 15);   // SWRST
    i2c->CR1 &= ~(1 << 15);  // Release reset
    
    i2c->CR1 &= ~I2C_PE;
    i2c->CR2 |= I2C_FREQ;
    i2c->CCR = I2C_CCR;
    i2c->TRISE = 17;
    i2c->OAR1 &= I2C_ADDMODE;
    i2c->CR1 |= I2C_PE;
    i2c->CR1 |= I2C_ACK;
    return;
}

void i2c_master_start() {
    struct i2c_reg_map *i2c = I2C_BASE;

    while (i2c->SR2 & I2C_BUSY){
        continue;
    }

    i2c->CR1 |= I2C_START;  

    while (!(i2c->SR1 & I2C_SB)){ 
        continue;
    }

    return;
}

void i2c_master_stop() {
    struct i2c_reg_map *i2c = I2C_BASE;
    i2c->CR1 |= I2C_STOP;

    return;
}

int i2c_master_write(uint8_t *buf, uint16_t len, uint8_t slave_addr) {
    struct i2c_reg_map *i2c = I2C_BASE;

    for (int attempt = 0; attempt < I2C_MAX_RETRIES; attempt++) {
        i2c_master_start();

        uint8_t padded_addr = (slave_addr << 1);
        i2c->DR = padded_addr;

        // Ev6
        while (!(i2c->SR1 & I2C_ADDR) && !(i2c->SR1 & I2C_AF)) {
            continue;
        }

        if (i2c->SR1 & I2C_AF) {
            i2c->SR1 &= ~I2C_AF;
            i2c_master_stop();
            continue;
        }

        // clear ADDR
        (void) i2c->SR2;

        for (uint16_t i = 0; i < len; i++) {
            i2c->DR = buf[i];
            while (!(i2c->SR1 & I2C_TxE)) {
                continue;
            }
        }

        while (!(i2c->SR1 & I2C_TxE) || !(i2c->SR1 & I2C_BTF)) {
            continue;
        }

        i2c_master_stop();
        return 0;
    }
    return -1;
}
