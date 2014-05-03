/** \copyright
 * Copyright (c) 2013, Stuart W Baker and Balazs Racz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 * \file Executor.cxx
 *
 * Class to control execution of tasks that get pulled of an input queue.  This
 * is based off of work started by Balazs on 5 August 2013.
 *
 * @author Stuart W Baker and Balazs Racz
 * @date 26 October 2013
 */

#include "executor/Executor.hxx"

#include <unistd.h>

#include "executor/Service.hxx"

ExecutorBase *ExecutorBase::list = NULL;

/** Constructor.
 * @param name name of executor
 * @param priority thread priority
 * @param stack_size thread stack size
 */
ExecutorBase::ExecutorBase()
    : name(name)
    , next(NULL)
    , activeTimers_(this)
    , done_(0)
{
    /** @todo (Stuart Baker) we need a locking mechanism here to protect
     *  the list.
     */
    if (list == NULL)
    {
        list = this;
        next = NULL;
    }
    else
    {
        next = list;
        list = this;
    }
}

/** Lookup an executor by its name.
 * @param name name of executor to lookup
 * @return pointer to executor upon success, else NULL if not found
 */
ExecutorBase *ExecutorBase::by_name(const char *name, bool wait)
{
    /** @todo (Stuart Baker) we need a locking mechanism here to protect
     *  the list.
     */
    for (; /* forever */;)
    {
        ExecutorBase *current = list;
        while (current)
        {
            if (!strcmp(name, current->name))
            {
                return current;
            }
            current = current->next;
        }
        if (wait)
        {
            sleep(1);
        }
        else
        {
            return NULL;
        }
    }
}

/** Thread entry point.
 * @return Should never return
 */
void *ExecutorBase::entry()
{
    Executable *msg;

    /* wait for messages to process */
    for (; /* forever */;)
    {
        unsigned priority;
        long long wait_length = activeTimers_.get_next_timeout();
        msg = timedwait(wait_length, &priority);
        if (msg == this)
        {
            // exit closure
            done_ = 1;
            return NULL;
        }
        if (msg != NULL)
        {
            current = msg;
            msg->run();
            current = nullptr;
        }
    }

    return NULL;
}

void ExecutorBase::shutdown()
{
    add(this);
    while (!done_)
    {
        usleep(100);
    }
}

ExecutorBase::~ExecutorBase()
{
    if (!done_)
    {
        shutdown();
    }
}
