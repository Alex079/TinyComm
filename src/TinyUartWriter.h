/*
  TinyUART
*/
#ifndef TinyUartWriter_h
#define TinyUartWriter_h

#include <avr/io.h>
#include <TinyUartBase.h>
#include <TinyTimer.h>

class TinyUartWriter : public TinyUartWrite {
  public:
    void on(const uint8_t, const uint32_t, TinyTimer&);
    void off();
  private:
    TinyTimer* timer;
    uint8_t storedDDR, storedPORT, storedMask;
};

TinyUartWriter uartWriter;

void TinyUartWriter::on(const uint8_t tx, const uint32_t baud,
                        TinyTimer& clockInterrupt) {
  output = tx;
  timer = &clockInterrupt;

  outputBuffer.clear();

  outputState = TINY_UART_STATE_IDLE;

  storedMask = (1 << output);
  storedDDR  = DDRB  & storedMask;
  storedPORT = PORTB & storedMask;
  PORTB |= storedMask;
  DDRB |= storedMask;

  uint32_t match = (F_CPU * 2) / (baud * TINY_UART_ONE_BIT_CLK);
  if (1 & match) match++;
  match >>= 1;
  timer->on(match, [](){
    uartWriter.onTimerTx();
  });
}

void TinyUartWriter::off() {
  timer->off();
  DDRB  = (DDRB  & ~storedMask) | storedDDR;
  PORTB = (PORTB & ~storedMask) | storedPORT;
}

#endif