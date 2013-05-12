/*

This file is part of libsaturn.

libsaturn is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libsaturn is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with libsaturn.  If not, see <http://www.gnu.org/licenses/>.

Your copy of the GNU Lesser General Public License should be in the
file named "LICENSE-LGPL.txt".

*/

#include <libsaturn.hpp>
#include <device.hpp>
#include <invalid_opcode.hpp>
#include <queue_overflow.hpp>

#include <cstdint>

#include <algorithm>
#include <array>
#include <memory>
#include <queue>
#include <vector>

void galaxy::saturn::dcpu::cycle()
{
    if (!execution_has_begun)
        execution_has_begun  = true;

    // call the cycle() method of all devices
    // note: I wanted to use a foreach loop here but couldn't because of unique_ptr business
    for (auto it = devices.begin(); it != devices.end(); ++it) {
        (*it)->cycle();
    }

    if (sleep_cycles > 0) {
        sleep_cycles--;
        return;
    }

    if (!interrupt_queue.empty() && !queue_interrupts) {
        interrupt(interrupt_queue.front());
        interrupt_queue.pop();
        return;
    }

    // executing a cycle, guard against interrupts
    guard_interrupts = true;

    bool skip = false;

    std::uint16_t instruction = ram[PC++];
    std::uint16_t opcode = instruction & 0x1f;

    std::uint16_t b = (instruction >> 5) & 0x1f;
    std::uint16_t a = (instruction >> 10) & 0x3f;

    std::uint16_t& a_value = get_reference(a, true);

    if (opcode != 0x00) {

        std::uint16_t& b_value = get_reference(b, false);

        switch (opcode){
            /**
             * SET - 1 cycle
             * sets b to a
             */
            case 0x01:
                sleep_cycles++;
                b_value = a_value;
                break;

            /**
             * ADD - 2 cycles
             * sets b to b+a, sets EX to 0x0001 if there's an overflow,
             * 0x0 otherwise
             */
            case 0x02:
                sleep_cycles += 2;

                if (b_value + a_value > 0xffff) {
                    EX = 0x0001;
                } else {
                    EX = 0x0000;
                }

                b_value += a_value;
                break;

            /**
             * SUB - 2 cycles
             * sets b to b-a, sets EX to 0xffff if there's an underflow,
             * 0x0 otherwise
             */
            case 0x03:
                sleep_cycles += 2;

                if(b_value < a_value) {
                    EX = 0xffff;
                } else {
                    EX = 0x0000;
                }

                b_value -= a_value;
                break;

            /**
             * MUL - 2 cycles
             * sets b to b*a, sets EX to ((b*a)>>16)&0xffff (treats b,
             * a as unsigned
             */
            case 0x04:
                sleep_cycles += 2;

                EX = ((b_value * a_value) >> 16) & 0xffff;

                b_value *= a_value;
                break;

            /**
             * MLI - 2 cycles
             * like MUL, but treat b, a as signed
             */
            case 0x05:
                sleep_cycles += 2;

                EX = ((static_cast<std::int16_t>(b_value) * static_cast<std::int16_t>(a_value)) >> 16) & 0xffff;

                b_value = static_cast<std::int16_t>(b_value) * static_cast<std::int16_t>(a_value);
                break;

            /**
             * DIV - 3 cycles
             * sets b to b/a, sets EX to ((b<<16)/a)&0xffff. if a==0,
             * sets b and EX to 0 instead. (treats b, a as unsigned)
             */
            case 0x06:
                sleep_cycles += 3;

                if (a_value == 0) {
                    EX = 0;
                    b_value = 0;
                } else {
                    EX = ((static_cast<std::uint32_t>(b_value) << 16) / a_value) & 0xffff;
                    b_value /= a_value;
                }
                break;

            /**
             * DVI - 3 cycles
             * like DIV, but treat b, a as signed. Rounds towards 0
             */
            case 0x07:
                sleep_cycles += 3;

                if (a_value == 0) {
                    EX = 0;
                    b_value = 0;
                } else {
                    EX = ((static_cast<std::int64_t>(b_value) << 16) / static_cast<std::int16_t>(a_value)) & 0xffff;
                    b_value = static_cast<std::int16_t>(b_value) / static_cast<std::int16_t>(a_value);
                }
                break;

            /**
             * MOD - 3 cycles
             * sets b to b%a. if a==0, sets b to 0 instead.
             */
            case 0x08:
                sleep_cycles += 3;

                if (a_value == 0) {
                    b_value = 0;
                } else {
                    b_value %= a_value;
                }
                break;

            /**
             * MDI - 3 cycles
             * like MOD, but treat b, a as signed. (MDI -7, 16 == -7)
             */
            case 0x09:
                sleep_cycles += 3;

                if (a_value == 0) {
                    b_value = 0;
                } else {
                    b_value = static_cast<std::int16_t>(b_value) % static_cast<std::int16_t>(a_value);
                }
                break;

            /**
             * AND - 1 cycle
             * sets b to b & a
             */
            case 0x0a:
                sleep_cycles++;
                b_value &= a_value;
                break;

            /**
             * BOR - 1 cycle
             * sets b to b|a
             */
            case 0x0b:
                sleep_cycles++;
                b_value |= a_value;
                break;

            /**
             * XOR - 1 cycle
             * sets b to b^a
             */
            case 0x0c:
                sleep_cycles++;
                b_value ^= a_value;
                break;

            /**
             * SHR - 1 cycle
             * sets b to b>>>a, sets EX to ((b<<16)>>a)&0xffff
             * (logical shift)
             */
            case 0x0d:
                sleep_cycles++;
                EX = ((b_value << 16) >> a_value) & 0xffff;
                b_value >>= a_value;
                break;

            /**
             * ASR - 1 cycle
             * sets b to b>>a, sets EX to ((b<<16)>>>a)&0xffff
             * (arithmetic shift) (treats b as signed)
             */
            case 0x0e:
                sleep_cycles++;

                EX = ((static_cast<std::int16_t>(b_value) << 16) >> a_value) & 0xffff;
                b_value = static_cast<std::int16_t>(b_value) >> a_value;
                break;

            /**
             * SHL - 1 cycle
             * sets b to b<<a, sets EX to ((b<<a)>>16)&0xffff
             */
            case 0x0f:
                sleep_cycles++;
                EX = ((b_value << a_value) >> 16) & 0xffff;
                b_value <<= a_value;
                break;

            /// conditional statements can take more cycles due to chaining

            /**
             * IFB - 2 cycles
             * performs next instruction only if (b&a)!=0
             */
            case 0x10:
                sleep_cycles += 2;
                skip = !((b_value & a_value) != 0);
                break;

            /**
             * IFC - 2 cycles
             * performs next instruction only if (b&a)==0
             */
            case 0x11:
                sleep_cycles +=2;
                skip = !((b_value & a_value) == 0);
                break;

            /**
             * IFE - 2 cycles
             * performs next instruction only if b==a
             */
            case 0x12:
                sleep_cycles += 2;
                skip = !(b_value == a_value);
                break;

            /**
             * IFN - 2 cycles
             * performs next instruction only if b!=a
             */
            case 0x13:
                sleep_cycles += 2;
                skip = !(b_value != a_value);
                break;

            /**
             * IFG - 2 cycles
             * performs next instruction only if b>a
             */
            case 0x14:
                sleep_cycles += 2;
                skip = !(b_value > a_value);
                break;
            /**
             * IFA - 2 cycles
             * performs next instruction only if b>a (signed)
             */
            case 0x15:
                sleep_cycles += 2;
                skip = !(static_cast<std::int16_t>(b_value) > static_cast<std::int16_t>(a_value));
                break;

            /**
             * IFL - 2 cycles
             * performs next instruction only if b<a
             */
            case 0x16:
                sleep_cycles += 2;
                skip = !(b_value < a_value);
                break;

            /**
             * IFU - 2 cycles
             * performs next instruction only if b<a (signed)
             */
            case 0x17:
                sleep_cycles += 2;
                skip = !(static_cast<std::int16_t>(b_value) < static_cast<std::int16_t>(a_value));
                break;

            /**
             * ADX - 3 cycles
             * sets b to b+a+EX, sets EX to 0x0001 if there is an overflow,
             * 0x0 otherwise
             */
            case 0x1a:
                sleep_cycles += 3;
                {
                    std::uint16_t b_old = b_value;
                    b_value += a_value + EX;
                    if(b_old + a_value + EX > 0xffff) {
                        EX = 0x0001;
                    } else {
                        EX = 0x0000;
                    }
                }
                break;

            /**
             * SBX - 3 cycles
             * sets b to b-a+EX, sets EX to 0xffff if there is an underflow,
             * 0x0001 if there's an overflow, 0x0 otherwise
             */
            case 0x1b:
                sleep_cycles += 3;
                {
                    std::uint16_t b_old = b_value;
                    b_value -= a_value - EX;
                    if(b_old + EX < a_value) {
                        EX = 0xffff;
                    } else if(b_old + EX > 0xffff + a_value) {
                        EX = 0x0001;
                    } else {
                        EX = 0x0000;
                    }
                }
                break;

            /**
             * STI - 2 cycles
             * sets b to a, then increases I and J by 1
             */
            case 0x1e:
                sleep_cycles += 2;
                b_value = a_value;
                I++;
                J++;
                break;

            /**
             * STD - 2 cycles
             */
            case 0x1f:
                sleep_cycles += 2;
                b_value = a_value;
                I--;
                J--;
                break;

            /// invalid opcode
            default:
                throw galaxy::saturn::invalid_opcode("failed to execute invalid opcode");
        }

    } else {
        switch (b) {
            /**
             * JSR - 3 cycles
             * pushes the address of the next instruction to the stack,
             * then sets PC to a
             */
            case 0x01:
                sleep_cycles += 3;
                ram[--SP] = PC;
                PC = a_value;
                break;

            /**
             * INT - 4 cycles
             * triggers a software interrupt with message a
             */
            case 0x08:
                sleep_cycles += 4;

                // we're about to trigger an interrupt, so stop guarding against them
                guard_interrupts = false;
                interrupt(a_value);

                break;

            /**
             * IAG - 1 cycle
             * sets a to IA
             */
            case 0x09:
                sleep_cycles++;
                a_value = IA;
                break;

            /**
             * IAS - 1 cycle
             * sets IA to a
             */
            case 0x0a:
                sleep_cycles++;
                IA = a_value;
                break;

            /**
             * RFI - 3 cycles
             * disables interrupt queueing, pops A from the stack, then
             * pops PC from the stack
             */
            case 0x0b:
                sleep_cycles += 3;

                queue_interrupts = false;
                A = ram[SP++];
                PC = ram[SP++];

                break;

            /**
             * IAQ - 2 cycles
             * if a is nonzero, interrupts will be added to the queue
             * instead of triggered. if a is zero, interrupts will be
             * triggered as normal again.
             */
            case 0x0c:
                sleep_cycles += 2;

                if (a_value != 0) {
                    queue_interrupts = true;
                } else {
                    queue_interrupts = false;
                }
                break;

            /**
             * HWN - 2 cycles
             * sets a to number of connected hardware devices
             */
            case 0x10:
                sleep_cycles += 2;
                a_value = devices.size();
                break;

            /**
             * HWQ - 4 cycles
             * sets A, B, C, X, Y registers to information about hardware a
             * A+(B<<16) is a 32 bit word identifying the hardware id
             * C is the hardware version
             * X+(Y<<16> is a 32 bit word identifying the manufacturer
             */
            case 0x11:
                sleep_cycles += 4;

                if (a_value < devices.size()) {

                    B = devices[a_value]->id >> 16;
                    A = devices[a_value]->id & 0xffff;

                    C = devices[a_value]->version;

                    Y = devices[a_value]->manufacturer >> 16;
                    X = devices[a_value]->manufacturer & 0xffff;
                }

                break;

            /**
             * HWI - 4 cycles
             * sends an interrupt to hardware a
             * NOTE: the hardware may take more cycles to
             * process the interrupt
             */
            case 0x12:
                sleep_cycles += 4;

                if (a_value < devices.size()) {
                    devices[a_value]->interrupt();
                }

                break;

            /// invalid opcode
            default:
                throw galaxy::saturn::invalid_opcode("failed to execute invalid opcode");
        }
    }

    if (skip) {

        do {

            // there's probably a more elegant way to ensure this takes a single cycle
            int sleep_cycles_old = sleep_cycles;

            instruction = ram[PC++];
            opcode = instruction & 0x1f;

            b = (instruction >> 5) & 0x1f;
            a = (instruction >> 10) & 0x3f;

            get_reference(a, true);
            get_reference(b, false);
            sleep_cycles = sleep_cycles_old + 1;

        } while (opcode >= 0x10 && opcode <= 0x17);

    }

    /*
     * a cycle has been executed
     * NOTE: this means we can use
     * normal cycle counts in opcodes
     */
    sleep_cycles--;

    // the cycle is finished executing, trigger interrupts normally
    guard_interrupts = false;
}

void galaxy::saturn::dcpu::interrupt(std::uint16_t message)
{
    if (queue_interrupts || guard_interrupts) {
        interrupt_queue.push(message);

        if (interrupt_queue.size() > 256) {
            throw galaxy::saturn::queue_overflow("the dcpu is on fire");
        }

    } else if (IA != 0) {
        queue_interrupts = true;

         ram[--SP] = PC;
         ram[--SP] = A;

         PC = IA;
         A = message;
     }
}

std::uint16_t& galaxy::saturn::dcpu::get_reference(std::uint16_t val, bool a_value)
{
    switch (val) {

        case 0x0:
            return A;

        case 0x01:
            return B;

        case 0x02:
            return C;

        case 0x03:
            return X;

        case 0x04:
            return Y;

        case 0x05:
            return Z;

        case 0x06:
            return I;

        case 0x07:
            return J;

        case 0x08:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
            return ram[get_reference(val - 0x08, a_value)];

        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
            sleep_cycles++;
            return ram[get_reference(val - 0x10, a_value) + ram[PC++]];

        case 0x18:
            if (a_value) {
                return ram[SP++];
            } else {
                return ram[--SP];
            }

        case 0x19:
            return ram[SP];

        case 0x1a:
            sleep_cycles++;
            return ram[SP + ram[PC++]];

        case 0x1b:
            return SP;

        case 0x1c:
            return PC;

        case 0x1d:
            return EX;

        case 0x1e:
            sleep_cycles++;
            return ram[ram[PC++]];

        case 0x1f:
            sleep_cycles++;
            return ram[PC++];

        default:
            literal_value = val - 0x21;
            return literal_value;
    }
}


// maybe this should just return void
galaxy::saturn::device& galaxy::saturn::dcpu::attach_device(device* hw)
{
    if (execution_has_begun)
    {
        throw galaxy::saturn::dcpu::DeviceModificationAfterCycleError("attach_device called after executation has begun");
    }

    devices.push_back(std::move(std::unique_ptr<device>(hw)));
    hw->cpu = this;

    return *hw;
}


void galaxy::saturn::dcpu::reset()
{
    A = B = C = X = Y = Z = I = J = PC = SP = EX = IA = 0;
    ram.fill(0);
    queue_interrupts = false;
    sleep_cycles = 0;
}

bool galaxy::saturn::dcpu::interrupt_queue_enabled() {
    return queue_interrupts;
}
