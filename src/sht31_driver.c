#include "sht31_driver.h"

static const uint8_t periodic_mode_msb[5]    = {0x20, 0x21, 0x22, 0x23, 0x27};
static const uint8_t periodic_mode_lsb[5][3] = {{0x32, 0x24, 0x2F},
                                                {0x30, 0x26, 0x2D},
                                                {0x36, 0x20, 0x2B},
                                                {0x34, 0x22, 0x29},
                                                {0x37, 0x21, 0x2A}};

static uint16_t temperature     = 0;
static uint16_t humidity        = 0;
static uint16_t status_register = 0;

static bool send_address_write(void)
{
    bool ack = i2c_driver_send_address_write(DEFAULT_ADDRESS);
    return ack;
}

static bool send_address_read(void)
{
    bool ack = i2c_driver_send_address_read(DEFAULT_ADDRESS);
    return ack;
}

static bool send_data(uint8_t data)
{
    bool ack = i2c_driver_send_data(data);
    return ack;
}

static bool send_16_bit_data(uint16_t data)
{
    if (false == send_data(data >> 8))
    {
        return false;
    }

    if (false == send_data(data &= 0x00FF))
    {
        return false;
    }
    return true;
}

static bool start_send_address_then_16_bit_command(uint16_t command)
{
    i2c_driver_start();

    if (false == send_address_write())
    {
        return false;
    }

    if (false == send_16_bit_data(command))
    {
        return false;
    }

    return true;
}

static bool start_send_address_read()
{
    i2c_driver_start();
    if (false == send_address_read())
    {
        return false;
    }
    return true;
}

// TODO: Refactor to improve efficiency and readability
static uint8_t calculate_crc(uint16_t value)
{
    /*
     *
     * CRC-8 formula from page 14 of SHT spec pdf
     *
     * Test data 0xBE, 0xEF should yield 0x92
     *
     * Initialization data 0xFF
     * Polynomial 0x31 (x8 + x5 + x4 + 1)
     * Final XOR 0x00
     */

    // const uint8_t len = 2;
    const uint8_t POLYNOMIAL = 0x31;
    uint8_t data             = 0;
    uint8_t crc              = 0xFF;
    uint8_t i                = 0;

    data = value >> 8;
    crc ^= data;

    for (i = 8; i; --i)
    {
        crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }

    data = value & 0x00FF;
    crc ^= data;

    for (i = 8; i; --i)
    {
        crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
    return crc;
}

void sht30_driver_create(void)
{
    i2c_driver_create();
}

bool sht30_driver_send_soft_reset(void)
{
    i2c_driver_start();

    if (false == send_address_write())
    {
        return false;
    }

    if (false == send_16_bit_data(SOFT_RESET))
    {
        return false;
    }

    i2c_driver_stop();

    return true;
}

bool sht30_driver_send_periodic_data_aquisition_mode(uint8_t repeatability,
                                                     uint8_t mps)
{
    i2c_driver_start();

    if (false == send_address_write())
    {
        return false;
    }
    if (false == send_data(periodic_mode_msb[mps]))
    {
        return false;
    }
    if (false == send_data(periodic_mode_lsb[mps][repeatability]))
    {
        return false;
    }

    i2c_driver_stop();

    return true;
}

bool sht30_driver_fetch_periodic_data(void)
{
    if (false == start_send_address_then_16_bit_command(FETCH_DATA))
    {
        return false;
    }

    i2c_driver_start();
    if (false == i2c_driver_send_address_read(DEFAULT_ADDRESS))
    {
        i2c_driver_stop();
        return false;
    }

    // TODO: the =0 below can probably be removed
    temperature = 0;
    temperature = (i2c_driver_read_data(true) << 8);
    temperature |= i2c_driver_read_data(true);

    uint8_t crc            = i2c_driver_read_data(true);
    uint8_t crc_calculated = calculate_crc(temperature);

    if (crc != crc_calculated)
    {
        return false;
    }

    // TODO: the =0 below can probably be removed
    humidity = 0;
    humidity = (i2c_driver_read_data(true) << 8);
    humidity |= i2c_driver_read_data(true);

    crc            = i2c_driver_read_data(false);
    crc_calculated = calculate_crc(humidity);

    if (crc != crc_calculated)
    {
        i2c_driver_stop();
        return false;
    }

    i2c_driver_stop();

    return true;
}

uint16_t sht30_driver_return_temperature(void)
{
    return temperature;
}

uint16_t sht30_driver_return_humidity(void)
{
    return humidity;
}

bool sht30_driver_read_status_register(void)
{
    if (false == start_send_address_then_16_bit_command(READ_STATUS_ADDRESS))
    {
        i2c_driver_stop();
        return false;
    }

    if (false == start_send_address_read())
    {
        i2c_driver_stop();
        return false;
    }

    // TODO: the =0 below can probably be removed
    status_register = 0;
    status_register = (i2c_driver_read_data(true) << 8);
    status_register |= i2c_driver_read_data(true);

    uint8_t crc            = i2c_driver_read_data(false);
    uint8_t crc_calculated = calculate_crc(temperature);

    if (crc != crc_calculated)
    {
        i2c_driver_stop();
        return false;
    }

    i2c_driver_stop();

    return true;
}

uint16_t sht30_driver_return_status_register(void)
{
    return status_register;
}

bool sht30_driver_clear_status_register(void)
{
    if (false == start_send_address_then_16_bit_command(CLEAR_STATUS_ADDRESS))
    {
        i2c_driver_stop();
        return false;
    }
    i2c_driver_stop();
    return true;
}

bool sht30_driver_break_command(void)
{
    if (false == start_send_address_then_16_bit_command(BREAK_COMMAND_ADDRESS))
    {
        i2c_driver_stop();
        return false;
    }
    i2c_driver_stop();
    return true;
}

bool sht30_driver_get_single_shot_data(void)
{
    start_send_address_then_16_bit_command(SHT_SINGLE_SHOT_MODE_HIGH_CLOCK_STRETCH);
    i2c_driver_stop();
    start_send_address_read();

    // TODO: the =0 below can probably be removed
    temperature = 0;
    temperature = (i2c_driver_read_data(true) << 8);
    temperature |= i2c_driver_read_data(true);

    uint8_t crc            = i2c_driver_read_data(true);
    uint8_t crc_calculated = calculate_crc(temperature);

    if (crc != crc_calculated)
    {
        i2c_driver_stop();
        return false;
//        return false;
    }

    // TODO: the =0 below can probably be removed
    humidity = 0;
    humidity = (i2c_driver_read_data(true) << 8);
    humidity |= i2c_driver_read_data(true);

    crc            = i2c_driver_read_data(false);
    crc_calculated = calculate_crc(humidity);

    if (crc != crc_calculated)
    {
        i2c_driver_stop();
        return false;
    }

    i2c_driver_stop();

    return true;
}

