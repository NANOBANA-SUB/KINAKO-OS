#include "interrupt.h"
#include "csr.h"

bool intr_get(void) 
{
    uint32_t status = READ_CSR(sstatus);
    return (status & SSTATUS_SPIE) != 0; // SPIEビットが立っていれば割り込み有効
}

void intr_on(void) 
{
    uint32_t status = READ_CSR(sstatus);
    status |= SSTATUS_SPIE; // SPIEビットを立てる
    WRITE_CSR(sstatus, status);
}

void intr_off(void) 
{
    uint32_t status = READ_CSR(sstatus);
    status &= ~SSTATUS_SPIE; // SPIEビットをクリアする
    WRITE_CSR(sstatus, status);
}