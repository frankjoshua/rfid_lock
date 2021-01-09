#include <Arduino.h>
#include <unity.h>
#include "backoff.h"

BackoffTimer backoff;

void setUp(void)
{
  // set stuff up here
  backoff = BackoffTimer();
  backoff.reset();
}

// void tearDown(void)
// {
//   // clean stuff up here
// }

void testDefaultDelay(void)
{
  TEST_ASSERT_EQUAL(250, backoff.getDelay());
}

void testSetDelay(void)
{
  backoff.setDelay(10);
  TEST_ASSERT_EQUAL(500, backoff.getDelay());
}

void testBackoffDelay(void)
{
  backoff.setDelay(0);
  TEST_ASSERT_EQUAL(true, backoff.isReady(501));
}

void testNotReady(void)
{
  backoff.setDelay(0);
  TEST_ASSERT_EQUAL(false, backoff.isReady(500));
}

void testLongUptime(void)
{
  backoff.setDelay(0);
  TEST_ASSERT_EQUAL(true, backoff.isReady(2147483647));
}

void testDelayLongUptime(void)
{
  unsigned long delayUntil = 0;
  unsigned long uptimeInMillis = 4294967295;
  int lastDelay = 500;
  delayUntil = uptimeInMillis + lastDelay;
  TEST_ASSERT_EQUAL(499, delayUntil);
  TEST_ASSERT_EQUAL(true, 501 > delayUntil);

  backoff.setDelay(uptimeInMillis);
  TEST_ASSERT_EQUAL(true, backoff.isReady(501));
}

void setup()
{
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN(); // IMPORTANT LINE!

  RUN_TEST(testDefaultDelay);
  RUN_TEST(testBackoffDelay);
  RUN_TEST(testNotReady);
  RUN_TEST(testLongUptime);
  RUN_TEST(testDelayLongUptime);
  RUN_TEST(testSetDelay);

  UNITY_END();
}

void loop()
{
}