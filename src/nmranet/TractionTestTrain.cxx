/** \copyright
 * Copyright (c) 2014, Balazs Racz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are  permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \file TractionTestTrain.hxx
 *
 * Train implementation plugins helpful for testing without a layout.
 *
 * @author Balazs Racz
 * @date 4 Aug 2014
 */

#include "nmranet/TractionTestTrain.hxx"

#include "utils/logging.h"

namespace nmranet {

LoggingTrain::LoggingTrain(uint32_t legacy_address)
    : legacyAddress_(legacy_address)
{
    LOG(INFO, "Created train %u.", legacyAddress_);
}

LoggingTrain::~LoggingTrain()
{
    LOG(INFO, "Destructed train %u.", legacyAddress_);
}

void LoggingTrain::set_speed(SpeedType speed)
{
    LOG(INFO, "train %u : set speed to %.0f mph.", legacyAddress_, speed.mph());
    currentSpeed_ = speed;
}

SpeedType LoggingTrain::get_speed()
{
    LOG(INFO, "train %u : get speed -> returns %.0f mph.", legacyAddress_,
        currentSpeed_.mph());
    return currentSpeed_;
}

void LoggingTrain::set_emergencystop()
{
    LOG(INFO, "train %u : set emergency stop.", legacyAddress_);
    currentSpeed_ = 0;
}

void LoggingTrain::set_fn(uint32_t address, uint16_t value)
{
    LOG(INFO, "train %u : set fn %u to %u.", legacyAddress_, address, value);
    fnValues_[address] = value;
}

uint16_t LoggingTrain::get_fn(uint32_t address)
{
    uint16_t resp = fnValues_[address];
    LOG(INFO, "train %u : get fn %u -> current value is %u.", legacyAddress_,
        address, resp);
    return resp;
}

uint32_t LoggingTrain::legacy_address()
{
    return legacyAddress_;
}
 
}  // namespace nmranet
