/**
 * @file        test_sht30_driver.c
 * @author      Steven Daglish
 * @brief
 * @version     0.1
 * @date        04 August 2019
 *
 */

///////////////////////////////////////////////////////////////////////////////
// Test list
// ---------
//
// Initialising
//
//
// Parameter changes
///////////////////////////////////////////////////////////////////////////////

// TODO:    Add more stop conditions when things fail to make sure clock isn't
// left hanging (something that I think will be a further problem going
// further).

#include "unity.h"
#include "sht31_driver.h"
#include "mock_i2c_driver.h"

static const uint8_t periodic_mode_msb[5]    = {0x20, 0x21, 0x22, 0x23, 0x27};
static const uint8_t periodic_mode_lsb[5][3] = {{0x32, 0x24, 0x2F},
                                                {0x30, 0x26, 0x2D},
                                                {0x36, 0x20, 0x2B},
                                                {0x34, 0x22, 0x29},
                                                {0x37, 0x21, 0x2A}};

static void expect_start_and_send_address_write(bool address_ack, bool msb_ack,
                                                bool lsb_ack,
                                                uint16_t reg_address)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, address_ack);
    if (false == address_ack)
    {
        return;
    }
    i2c_driver_send_data_ExpectAndReturn(reg_address >> 8, msb_ack);
    if (false == msb_ack)
    {
        return;
    }
    i2c_driver_send_data_ExpectAndReturn(reg_address & 0x00FF, lsb_ack);
}

static void expect_start_and_send_address_read(bool address_ack)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_read_ExpectAndReturn(DEFAULT_ADDRESS, address_ack);
}

static void expect_read_three_data_values(bool ack1, bool ack2, bool ack3,
                                          uint8_t data1, uint8_t data2,
                                          uint8_t data3)
{
    i2c_driver_read_data_ExpectAndReturn(ack1, data1);
    i2c_driver_read_data_ExpectAndReturn(ack2, data2);
    i2c_driver_read_data_ExpectAndReturn(ack3, data3);
}

void setUp(void)
{
    i2c_driver_create_Expect();
    sht30_driver_create();
}

// TODO: Check if I need to add something about turning off module
void tearDown(void)
{
}

///////////////////////////////////////////////////////////////////////////////
// Initialisations
///////////////////////////////////////////////////////////////////////////////

void test_soft_reset_successful(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, true);
    i2c_driver_send_data_ExpectAndReturn(SOFT_RESET_MSB, true);
    i2c_driver_send_data_ExpectAndReturn(SOFT_RESET_LSB, true);
    i2c_driver_stop_Expect();

    sht30_driver_send_soft_reset();
}

void test_soft_reset_successful_will_return_true(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, true);
    i2c_driver_send_data_ExpectAndReturn(SOFT_RESET_MSB, true);
    i2c_driver_send_data_ExpectAndReturn(SOFT_RESET_LSB, true);
    i2c_driver_stop_Expect();

    bool success = sht30_driver_send_soft_reset();
    TEST_ASSERT(success);
}

void test_soft_reset_send_address_fail_function_stops(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, false);

    sht30_driver_send_soft_reset();
}

void test_soft_reset_send_address_fail_function_stops_returns_false(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, false);

    bool success = sht30_driver_send_soft_reset();
    TEST_ASSERT_FALSE(success);
}

void test_soft_reset_no_ack_after_msb_function_returns(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, true);
    i2c_driver_send_data_ExpectAndReturn(SOFT_RESET_MSB, false);

    sht30_driver_send_soft_reset();
}

void test_soft_reset_no_ack_after_lsb_function_returns(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, true);
    i2c_driver_send_data_ExpectAndReturn(SOFT_RESET_MSB, true);
    i2c_driver_send_data_ExpectAndReturn(SOFT_RESET_LSB, false);

    sht30_driver_send_soft_reset();
}

///////////////////////////////////////////////////////////////////////////////
// Setting up periodic measurments
///////////////////////////////////////////////////////////////////////////////

void test_set_periodic_measurement_mode_all_ok(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, true);
    i2c_driver_send_data_ExpectAndReturn(periodic_mode_msb[0], true);
    i2c_driver_send_data_ExpectAndReturn(periodic_mode_lsb[0][0], true);
    i2c_driver_stop_Expect();

    bool success = sht30_driver_send_periodic_data_aquisition_mode(0, 0);
    TEST_ASSERT(success);
}

void test_set_periodic_measurement_mode_no_ack_after_address(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, false);
    // i2c_driver_send_data_ExpectAndReturn(periodic_mode_msb[0], true);
    // i2c_driver_send_data_ExpectAndReturn(periodic_mode_lsb[0][0], true);
    // i2c_driver_stop_Expect();

    bool success = sht30_driver_send_periodic_data_aquisition_mode(0, 0);
    TEST_ASSERT_FALSE(success);
}

void test_set_periodic_measurement_mode_no_ack_after_msb(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, true);
    i2c_driver_send_data_ExpectAndReturn(periodic_mode_msb[0], false);
    // i2c_driver_send_data_ExpectAndReturn(periodic_mode_lsb[0][0], true);
    // i2c_driver_stop_Expect();

    bool success = sht30_driver_send_periodic_data_aquisition_mode(0, 0);
    TEST_ASSERT_FALSE(success);
}

void test_set_periodic_measurement_mode_no_ack_after_lsb(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, true);
    i2c_driver_send_data_ExpectAndReturn(periodic_mode_msb[0], true);
    i2c_driver_send_data_ExpectAndReturn(periodic_mode_lsb[0][0], false);
    // i2c_driver_stop_Expect();

    bool success = sht30_driver_send_periodic_data_aquisition_mode(0, 0);
    TEST_ASSERT_FALSE(success);
}

///////////////////////////////////////////////////////////////////////////////
// Fetching periodic measurements
///////////////////////////////////////////////////////////////////////////////

// TODO Clean up the functions below as there is a lot of duplication
void test_fetch_periodic_data_all_ok(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, true);
    i2c_driver_send_data_ExpectAndReturn(FETCH_DATA_MSB, true);
    i2c_driver_send_data_ExpectAndReturn(FETCH_DATA_LSB, true);

    i2c_driver_start_Expect();
    i2c_driver_send_address_read_ExpectAndReturn(DEFAULT_ADDRESS, true);

    i2c_driver_read_data_ExpectAndReturn(true, 0xBE);
    i2c_driver_read_data_ExpectAndReturn(true, 0xEF);
    i2c_driver_read_data_ExpectAndReturn(true, 0x92);

    i2c_driver_read_data_ExpectAndReturn(true, 0xBE);
    i2c_driver_read_data_ExpectAndReturn(true, 0xEF);
    i2c_driver_read_data_ExpectAndReturn(false, 0x92);

    i2c_driver_stop_Expect();

    bool success = sht30_driver_fetch_periodic_data();
    TEST_ASSERT(success);
}

void test_fetch_periodic_data_no_ack_after_sending_address(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, false);
    // i2c_driver_send_data_ExpectAndReturn(FETCH_DATA_MSB, true);
    // i2c_driver_send_data_ExpectAndReturn(FETCH_DATA_LSB, true);

    // i2c_driver_start_Expect();
    // i2c_driver_send_address_read_ExpectAndReturn(DEFAULT_ADDRESS, true);

    // i2c_driver_read_data_ExpectAndReturn(true, 0xBE);
    // i2c_driver_read_data_ExpectAndReturn(true, 0xEF);
    // i2c_driver_read_data_ExpectAndReturn(true, 0x92);

    // i2c_driver_read_data_ExpectAndReturn(true, 0xBE);
    // i2c_driver_read_data_ExpectAndReturn(true, 0xEF);
    // i2c_driver_read_data_ExpectAndReturn(false, 0x92);

    // i2c_driver_stop_Expect();

    bool success = sht30_driver_fetch_periodic_data();
    TEST_ASSERT_FALSE(success);
}

void test_fetch_periodic_data_no_ack_after_sending_MBS(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, true);
    i2c_driver_send_data_ExpectAndReturn(FETCH_DATA_MSB, false);
    // i2c_driver_send_data_ExpectAndReturn(FETCH_DATA_LSB, true);

    // i2c_driver_start_Expect();
    // i2c_driver_send_address_read_ExpectAndReturn(DEFAULT_ADDRESS, true);

    // i2c_driver_read_data_ExpectAndReturn(true, 0xBE);
    // i2c_driver_read_data_ExpectAndReturn(true, 0xEF);
    // i2c_driver_read_data_ExpectAndReturn(true, 0x92);

    // i2c_driver_read_data_ExpectAndReturn(true, 0xBE);
    // i2c_driver_read_data_ExpectAndReturn(true, 0xEF);
    // i2c_driver_read_data_ExpectAndReturn(false, 0x92);

    // i2c_driver_stop_Expect();

    bool success = sht30_driver_fetch_periodic_data();
    TEST_ASSERT_FALSE(success);
}

void test_fetch_periodic_data_no_ack_after_sending_LBS(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, true);
    i2c_driver_send_data_ExpectAndReturn(FETCH_DATA_MSB, true);
    i2c_driver_send_data_ExpectAndReturn(FETCH_DATA_LSB, false);

    // i2c_driver_start_Expect();
    // i2c_driver_send_address_read_ExpectAndReturn(DEFAULT_ADDRESS, true);

    // i2c_driver_read_data_ExpectAndReturn(true, 0xBE);
    // i2c_driver_read_data_ExpectAndReturn(true, 0xEF);
    // i2c_driver_read_data_ExpectAndReturn(true, 0x92);

    // i2c_driver_read_data_ExpectAndReturn(true, 0xBE);
    // i2c_driver_read_data_ExpectAndReturn(true, 0xEF);
    // i2c_driver_read_data_ExpectAndReturn(false, 0x92);

    // i2c_driver_stop_Expect();

    bool success = sht30_driver_fetch_periodic_data();
    TEST_ASSERT_FALSE(success);
}

void test_fetch_periodic_data_no_ack_after_address_read(void)
{
    i2c_driver_start_Expect();
    i2c_driver_send_address_write_ExpectAndReturn(DEFAULT_ADDRESS, true);
    i2c_driver_send_data_ExpectAndReturn(FETCH_DATA_MSB, true);
    i2c_driver_send_data_ExpectAndReturn(FETCH_DATA_LSB, true);

    i2c_driver_start_Expect();
    i2c_driver_send_address_read_ExpectAndReturn(DEFAULT_ADDRESS, false);

    // i2c_driver_read_data_ExpectAndReturn(true, 0xBE);
    // i2c_driver_read_data_ExpectAndReturn(true, 0xEF);
    // i2c_driver_read_data_ExpectAndReturn(true, 0x92);

    // i2c_driver_read_data_ExpectAndReturn(true, 0xBE);
    // i2c_driver_read_data_ExpectAndReturn(true, 0xEF);
    // i2c_driver_read_data_ExpectAndReturn(false, 0x92);

    i2c_driver_stop_Expect();

    bool success = sht30_driver_fetch_periodic_data();
    TEST_ASSERT_FALSE(success);
}

void test_when_fetch_data_ok_temp_stored_unchecked(void)
{
    i2c_driver_start_Ignore();
    i2c_driver_send_address_write_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);

    i2c_driver_start_Ignore();
    i2c_driver_send_address_read_IgnoreAndReturn(true);
    i2c_driver_read_data_IgnoreAndReturn(0xAB);
    i2c_driver_read_data_IgnoreAndReturn(0xCD);
    i2c_driver_read_data_IgnoreAndReturn(0x00);

    i2c_driver_read_data_IgnoreAndReturn(0x01);
    i2c_driver_read_data_IgnoreAndReturn(0x23);
    i2c_driver_read_data_IgnoreAndReturn(0x00);

    i2c_driver_stop_Ignore();

    sht30_driver_fetch_periodic_data();

    uint16_t temperature = sht30_driver_return_temperature();
    TEST_ASSERT_EQUAL_HEX16(0xABCD, temperature);
}

void test_when_fetch_data_ok_humidity_stored_unchecked(void)
{
    i2c_driver_start_Ignore();
    i2c_driver_send_address_write_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);

    i2c_driver_start_Ignore();
    i2c_driver_send_address_read_IgnoreAndReturn(true);
    i2c_driver_read_data_IgnoreAndReturn(0xAB);
    i2c_driver_read_data_IgnoreAndReturn(0xCD);
    i2c_driver_read_data_IgnoreAndReturn(0x00);

    i2c_driver_read_data_IgnoreAndReturn(0xBE);
    i2c_driver_read_data_IgnoreAndReturn(0xEF);
    i2c_driver_read_data_IgnoreAndReturn(0x00);

    i2c_driver_stop_Ignore();

    sht30_driver_fetch_periodic_data();

    uint16_t humidity = sht30_driver_return_humidity();
    TEST_ASSERT_EQUAL_HEX16(0xBEEF, humidity);
}

void test_when_fetch_data_temperature_CRC_correct_return_true(void)
{
    i2c_driver_start_Ignore();
    i2c_driver_send_address_write_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);

    i2c_driver_start_Ignore();
    i2c_driver_send_address_read_IgnoreAndReturn(true);
    i2c_driver_read_data_IgnoreAndReturn(0xBE);
    i2c_driver_read_data_IgnoreAndReturn(0xEF);
    i2c_driver_read_data_IgnoreAndReturn(0x92);

    i2c_driver_read_data_IgnoreAndReturn(0xBE);
    i2c_driver_read_data_IgnoreAndReturn(0xEF);
    i2c_driver_read_data_IgnoreAndReturn(0x92);

    i2c_driver_stop_Ignore();

    bool success = sht30_driver_fetch_periodic_data();
    TEST_ASSERT(success);
}

void test_when_fetch_data_temperature_second_CRC_correct_return_true(void)
{
    i2c_driver_start_Ignore();
    i2c_driver_send_address_write_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);

    i2c_driver_start_Ignore();
    i2c_driver_send_address_read_IgnoreAndReturn(true);
    i2c_driver_read_data_IgnoreAndReturn(0x12);
    i2c_driver_read_data_IgnoreAndReturn(0x34);
    i2c_driver_read_data_IgnoreAndReturn(0x37);

    i2c_driver_read_data_IgnoreAndReturn(0xBE);
    i2c_driver_read_data_IgnoreAndReturn(0xEF);
    i2c_driver_read_data_IgnoreAndReturn(0x92);

    i2c_driver_stop_Ignore();

    bool success = sht30_driver_fetch_periodic_data();
    TEST_ASSERT(success);
}

void test_when_fetch_data_temperature_third_CRC_correct_return_true(void)
{
    i2c_driver_start_Ignore();
    i2c_driver_send_address_write_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);

    i2c_driver_start_Ignore();
    i2c_driver_send_address_read_IgnoreAndReturn(true);
    i2c_driver_read_data_IgnoreAndReturn(0xAB);
    i2c_driver_read_data_IgnoreAndReturn(0xCD);
    i2c_driver_read_data_IgnoreAndReturn(0x6F);

    i2c_driver_read_data_IgnoreAndReturn(0xBE);
    i2c_driver_read_data_IgnoreAndReturn(0xEF);
    i2c_driver_read_data_IgnoreAndReturn(0x92);

    i2c_driver_stop_Ignore();

    bool success = sht30_driver_fetch_periodic_data();
    TEST_ASSERT(success);
}

void test_when_fetch_data_temperature_CRC_failed_return_false(void)
{
    i2c_driver_start_Ignore();
    i2c_driver_send_address_write_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);

    i2c_driver_start_Ignore();
    i2c_driver_send_address_read_IgnoreAndReturn(true);
    i2c_driver_read_data_IgnoreAndReturn(0xBE);
    i2c_driver_read_data_IgnoreAndReturn(0xEF);
    i2c_driver_read_data_IgnoreAndReturn(0x00);

    i2c_driver_stop_Ignore();

    bool success = sht30_driver_fetch_periodic_data();
    TEST_ASSERT_FALSE(success);
}

void test_when_fetch_data_humidity_CRC_ok_passed(void)
{
    i2c_driver_start_Ignore();
    i2c_driver_send_address_write_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);

    i2c_driver_start_Ignore();
    i2c_driver_send_address_read_IgnoreAndReturn(true);
    i2c_driver_read_data_IgnoreAndReturn(0xBE);
    i2c_driver_read_data_IgnoreAndReturn(0xEF);
    i2c_driver_read_data_IgnoreAndReturn(0x92);

    i2c_driver_read_data_IgnoreAndReturn(0xBE);
    i2c_driver_read_data_IgnoreAndReturn(0xEF);
    i2c_driver_read_data_IgnoreAndReturn(0x92);

    i2c_driver_stop_Ignore();

    bool success = sht30_driver_fetch_periodic_data();
    TEST_ASSERT(success);
}

void test_when_fetch_data_humidity_CRC_not_ok_failed(void)
{
    i2c_driver_start_Ignore();
    i2c_driver_send_address_write_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);
    i2c_driver_send_data_IgnoreAndReturn(true);

    i2c_driver_start_Ignore();
    i2c_driver_send_address_read_IgnoreAndReturn(true);
    i2c_driver_read_data_IgnoreAndReturn(0xBE);
    i2c_driver_read_data_IgnoreAndReturn(0xEF);
    i2c_driver_read_data_IgnoreAndReturn(0x92);

    i2c_driver_read_data_IgnoreAndReturn(0xBE);
    i2c_driver_read_data_IgnoreAndReturn(0xEF);
    i2c_driver_read_data_IgnoreAndReturn(0x00);

    i2c_driver_stop_Ignore();

    bool success = sht30_driver_fetch_periodic_data();
    TEST_ASSERT_FALSE(success);
}

///////////////////////////////////////////////////////////////////////////////
// Reading the status register
///////////////////////////////////////////////////////////////////////////////
void test_read_status_register_all_ok(void)
{
    expect_start_and_send_address_write(true, true, true, READ_STATUS_ADDRESS);

    expect_start_and_send_address_read(true);

    i2c_driver_read_data_ExpectAndReturn(true, 0xBE);
    i2c_driver_read_data_ExpectAndReturn(true, 0xEF);
    i2c_driver_read_data_ExpectAndReturn(false, 0x92);

    i2c_driver_stop_Expect();

    bool success = sht30_driver_read_status_register();
    TEST_ASSERT(success);
}

void test_read_status_register_no_ack_after_address(void)
{
    expect_start_and_send_address_write(false, true, true, READ_STATUS_ADDRESS);
    // expect_start_and_send_address_read(true);
    // expect_read_three_data_values(true, true, false, 0xBE, 0xEF, 0x92);
    i2c_driver_stop_Expect();

    bool success = sht30_driver_read_status_register();
    TEST_ASSERT_FALSE(success);
}

void test_read_status_register_no_ack_after_msb(void)
{
    expect_start_and_send_address_write(true, false, true, READ_STATUS_ADDRESS);
    // expect_start_and_send_address_read(true);
    // expect_read_three_data_values(true, true, false, 0xBE, 0xEF, 0x92);
    i2c_driver_stop_Expect();

    bool success = sht30_driver_read_status_register();
    TEST_ASSERT_FALSE(success);
}

void test_read_status_register_no_ack_after_lsb(void)
{
    expect_start_and_send_address_write(true, true, false, READ_STATUS_ADDRESS);
    // expect_start_and_send_address_read(true);
    // expect_read_three_data_values(true, true, false, 0xBE, 0xEF, 0x92);
    i2c_driver_stop_Expect();

    bool success = sht30_driver_read_status_register();
    TEST_ASSERT_FALSE(success);
}

void test_read_status_register_no_ack_after_address_read(void)
{
    expect_start_and_send_address_write(true, true, true, READ_STATUS_ADDRESS);
    expect_start_and_send_address_read(false);
    // expect_read_three_data_values(true, true, false, 0xBE, 0xEF, 0x92);
    i2c_driver_stop_Expect();

    bool success = sht30_driver_read_status_register();
    TEST_ASSERT_FALSE(success);
}

void test_read_status_register_crc_incorrect_function_fails(void)
{
    expect_start_and_send_address_write(true, true, true, READ_STATUS_ADDRESS);
    expect_start_and_send_address_read(true);
    expect_read_three_data_values(true, true, false, 0xBE, 0xEF, 0x91);
    i2c_driver_stop_Expect();

    bool success = sht30_driver_read_status_register();
    TEST_ASSERT_FALSE(success);
}

void test_status_register_stored_after_successful_read(void)
{
    expect_start_and_send_address_write(true, true, true, READ_STATUS_ADDRESS);
    expect_start_and_send_address_read(true);
    expect_read_three_data_values(true, true, false, 0xBE, 0xEF, 0x92);
    i2c_driver_stop_Expect();

    bool success = sht30_driver_read_status_register();
    TEST_ASSERT(success);

    uint16_t status = sht30_driver_return_status_register();
    TEST_ASSERT_EQUAL_HEX16(0xBEEF, status);
}

void test_clear_status_register_all_ok(void)
{
    expect_start_and_send_address_write(true, true, true, CLEAR_STATUS_ADDRESS);
    i2c_driver_stop_Expect();

    bool success = sht30_driver_clear_status_register();
    TEST_ASSERT(success);
}

void test_clear_status_register_all_fail(void)
{
    expect_start_and_send_address_write(true, true, false,
                                        CLEAR_STATUS_ADDRESS);
    i2c_driver_stop_Expect();

    bool success = sht30_driver_clear_status_register();
    TEST_ASSERT_FALSE(success);
}

void test_break_command_successful(void)
{
    expect_start_and_send_address_write(true, true, true,
                                        BREAK_COMMAND_ADDRESS);
    i2c_driver_stop_Expect();

    bool success = sht30_driver_break_command();
    TEST_ASSERT(success);
}

///////////////////////////////////////////////////////////////////////////////
// Single shot data acquisition
///////////////////////////////////////////////////////////////////////////////
void test_single_shot_data_acquisition_mode_all_ok_no_clock_stretching(void)
{
    expect_start_and_send_address_write(true, true, true, 
            SHT_SINGLE_SHOT_MODE_HIGH_CLOCK_STRETCH);
    i2c_driver_stop_Expect();
    expect_start_and_send_address_read(true);

    expect_read_three_data_values(true, true, true, 0xBE, 0xEF, 0x91);
    expect_read_three_data_values(true, true, false, 0xBE, 0xEF, 0x91);

    i2c_driver_stop_Expect();

    bool success = sht30_driver_get_single_shot_data();
    TEST_ASSERT(success);
}
 


