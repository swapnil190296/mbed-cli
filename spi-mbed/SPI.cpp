/* mbed Example Program
 * Copyright (c) 2006-2014 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"
#include "afl_call.h"

SPI spi(D11, D12, D13); // mosi, miso, sclk
DigitalOut cs(D0);


// hardcoded options 1(global)
int noHyperCall = 0; // dont make hypercalls
int verbose = 0; // XXX Never turns on verbose mode, printf/scanf will hang mbed
 
int main() {

    char *buf;
    uint32_t sz;

    // hardcode options 2(local)
    int enableTimer = 0; // enable qemu's timer in forked children

    // step 1: start forkserver
    startForkserver(enableTimer);

    // step 2: get input & enable pm
    buf = getWork(&sz);
    if (sz == -1) {
        // hack to set aflStart to 1 which is required by doneWork
        startWork(0x0U, 0x0U);
        doneWork(0);
    }

    // step 3: start tracing
    startWork(0x0U, 0xffffffffU);

    if (buf[0] == 0xfb && buf[1] == 0x17) {
        // To launch afl w/o hang
        doneWork(0);
    }

    // Chip must be deselected
    cs = 1;
 
    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    spi.format(8,3);
    spi.frequency(1000000);
 
    // Select the device by seting chip select low
    cs = 0;
 
    // Send 0x8f, the command to read the WHOAMI register
    spi.write(0x8F);
 
    // Send a dummy byte to receive the contents of the WHOAMI register
    int whoami = spi.write(0x00);
//    printf("WHOAMI register = 0x%X\n", whoami);
 
    // Deselect the device
    cs = 1;
    doneWork(0);
}
