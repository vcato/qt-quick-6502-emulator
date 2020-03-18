#ifndef INSTRUCTIONEXECUTORTESTFIXTURE_HPP
#define INSTRUCTIONEXECUTORTESTFIXTURE_HPP

#include <gmock/gmock.h>
#include "instructionexecutor.hpp"
#include "opcodes.hpp"
#include <vector>
#include <map>
#include <functional>

using namespace std::placeholders;


class InstructionExecutorTestFixture : public ::testing::Test {
public:
    using addressType  = InstructionExecutor::addressType;
    using registerType = InstructionExecutor::registerType;

    struct ReadSignalValues {
        ReadSignalValues(addressType a, bool ro, uint8_t d) : address(a), read_only(ro), data(d) { }

        addressType address;
        bool        read_only;
        uint8_t     data;
    };

    struct WriteSignalValues {
        WriteSignalValues(addressType a, uint8_t d) : address(a), data(d) { }

        addressType address;
        uint8_t     data;
    };

    //void SetUp() override { }

    //void TearDown() override { }

    struct Param_AbsoluteIndexedWithY
    {
        addressType instruction_address;
        addressType address_to_load_from;
        uint8_t     value_to_load;
        uint8_t     y_register;
    };

    void setup_LDA_AbsoluteIndexedWithY(const Param_AbsoluteIndexedWithY &param)
    {
        loadInstructionIntoMemory(AbstractInstruction_e::LDA, AddressMode_e::AbsoluteYIndexed, param.instruction_address);
        fakeMemory[param.instruction_address + 1] = loByteOf(param.address_to_load_from);
        fakeMemory[param.instruction_address + 2] = hiByteOf(param.address_to_load_from);
        fakeMemory[param.address_to_load_from + param.y_register] = param.value_to_load;
        executor.registers().y = param.y_register;
    }

protected:
    Registers r;
    InstructionExecutor executor{ r,
                                  std::bind(&InstructionExecutorTestFixture::addressBusReadSignaled,        this, _1, _2),
                                  std::bind(&InstructionExecutorTestFixture::addressBusWriteSignaled,       this, _1, _2),
                                  std::bind(&InstructionExecutorTestFixture::accumulatorChangedSignaled,    this, _1),
                                  std::bind(&InstructionExecutorTestFixture::xChangedSignaled,              this, _1),
                                  std::bind(&InstructionExecutorTestFixture::yChangedSignaled,              this, _1),
                                  std::bind(&InstructionExecutorTestFixture::programCounterChangedSignaled, this, _1),
                                  std::bind(&InstructionExecutorTestFixture::stackPointerChangedSignaled,   this, _1),
                                  std::bind(&InstructionExecutorTestFixture::statusChangedSignaled,         this, _1)
                                };

    // Here is where we store the results of the signals.
    std::vector<ReadSignalValues>  readSignalsCaught;
    std::vector<WriteSignalValues> writeSignalsCaught;
    std::vector<registerType>      accumulatorChangedSignalsCaught;
    std::vector<registerType>      xChangedSignalsCaught;
    std::vector<registerType>      yChangedSignalsCaught;
    std::vector<addressType>       programCounterChangedSignalsCaught;
    std::vector<registerType>      stackPointerChangedSignalsCaught;
    std::vector<registerType>      statusChangedSignalsCaught;
    std::map<addressType, uint8_t> fakeMemory;

    void loadInstructionIntoMemory(const AbstractInstruction_e instruction, const AddressMode_e mode, const addressType address)
    {
        executor.registers().program_counter = address;
        fakeMemory[address] = OpcodeFor(instruction, mode);
    }

    void executeInstruction()
    {
        do {
            executor.clock();
        } while (!executor.complete());
    }

    uint8_t loByteOf(addressType address)
    {
        return address & 0xFF;
    }

    uint8_t hiByteOf(addressType address)
    {
        return address >> 8;
    }

    uint8_t addressBusReadSignaled(addressType address, bool read_only)
    {
        uint8_t retval = 0;

        // Retrieve the data at the given address...
        auto location = fakeMemory.find(address);

        if (location != fakeMemory.end()) {
            retval = location->second;
        }

        readSignalsCaught.emplace_back(address, read_only, retval);
        return retval;
    }

    void addressBusWriteSignaled(addressType address, uint8_t data)
    {
        fakeMemory[address] = data;
        writeSignalsCaught.emplace_back(address, data);
    }

    void accumulatorChangedSignaled(registerType value)
    {
        accumulatorChangedSignalsCaught.emplace_back(value);
    }

    void xChangedSignaled(registerType value)
    {
        xChangedSignalsCaught.emplace_back(value);
    }

    void yChangedSignaled(registerType value)
    {
        yChangedSignalsCaught.emplace_back(value);
    }

    void programCounterChangedSignaled(addressType address)
    {
        programCounterChangedSignalsCaught.emplace_back(address);
    }

    void stackPointerChangedSignaled(registerType value)
    {
        stackPointerChangedSignalsCaught.emplace_back(value);
    }

    void statusChangedSignaled(registerType value)
    {
        statusChangedSignalsCaught.emplace_back(value);
    }
};

#endif // INSTRUCTIONEXECUTORTESTFRAMEWORK_H
