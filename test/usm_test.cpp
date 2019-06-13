#include "../include/usm.hpp"

#include <assert.h>
#include <iostream>

using namespace usm;

/*
 * Copyright (c) 2019 Julian Kent. All rights reserved.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of libgnc nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
*/

namespace test {
enum TestStates { START, PATH_A_1, PATH_A_2, END, PATH_B_1, PATH_B_2, PATH_B_3, CLEANUP };

class TestStateMachine final : public StateMachine<TestStates> {
public:
    TestStateMachine()
        : StateMachine<TestStates>(START)
    {
    }

    bool path_a = true;
    bool trigger_error = false;

protected:
    Transition runCurrentState(TestStates currentState) override
    {
        if (trigger_error)
            return Transition::ERROR;

        switch (currentState) {
        case START:
            return start();

        case PATH_A_1:
            return a1();

        case PATH_A_2:
            return a2();

        case END:
            return end();

        case PATH_B_1:
            return b1();

        case PATH_B_2:
            return b2();

        case PATH_B_3:
            return b3();

        case CLEANUP:
            return cleanup();
        }
        return Transition::ERROR;
    }

    TestStates chooseNextState(test::TestStates currentState, Transition transition) override
    {
        //TODO: make a DSL with templates or macros to make this easier to read

        switch (currentState) {
        case START:
            switch (transition) {
            case Transition::NEXT:
                return PATH_A_1;
            case Transition::NEXT2:
                return PATH_B_1;
            default:
                return CLEANUP;
            }
            break;
        case PATH_A_1:
            switch (transition) {
            case Transition::NEXT:
                return PATH_A_2;
            case Transition::ERROR:
                return PATH_B_3;
            default:
                return CLEANUP;
            }
            break;
        case PATH_A_2:
            switch (transition) {
            case Transition::NEXT:
                return END;
            default:
                return CLEANUP;
            }
            break;
        case PATH_B_1:
            switch (transition) {
            case Transition::NEXT:
                return PATH_B_2;
            default:
                return CLEANUP;
            }
            break;
        case PATH_B_2:
            switch (transition) {
            case Transition::NEXT:
                return PATH_B_3;
            default:
                return CLEANUP;
            }
            break;
        case PATH_B_3:
            switch (transition) {
            case Transition::NEXT:
                return END;
            default:
                return CLEANUP;
            }
            break;
        case CLEANUP:
            switch (transition) {
            case Transition::NEXT:
                return END;
            default:
                return CLEANUP;
            }
            break;
        default:
            return CLEANUP;
        }
        return CLEANUP;
    }

private:
    Transition start()
    {
        if (path_a)
            return Transition::NEXT;
        else
            return Transition::NEXT2;
    }
    Transition a1() { return Transition::NEXT; }
    Transition a2() { return Transition::NEXT; }
    Transition b1() { return Transition::NEXT; }
    Transition b2() { return Transition::NEXT; }
    Transition b3() { return Transition::NEXT; }
    Transition end() { return Transition::REPEAT; }
    Transition cleanup() { return Transition::NEXT; }
};

void runTests_path_a()
{
    TestStateMachine m;
    assert(m.getState() == START);
    m.iterateOnce();
    assert(m.getState() == PATH_A_1);
    m.iterateOnce();
    assert(m.getState() == PATH_A_2);
    m.iterateOnce();
    assert(m.getState() == END);
}

void runTests_path_b()
{
    TestStateMachine m;
    m.path_a = false;
    assert(m.getState() == START);
    m.iterateOnce();
    assert(m.getState() == PATH_B_1);
    m.iterateOnce();
    assert(m.getState() == PATH_B_2);
    m.iterateOnce();
    assert(m.getState() == PATH_B_3);
    m.iterateOnce();
    assert(m.getState() == END);
}

void runTests_default_error()
{
    TestStateMachine m;
    assert(m.getState() == START);
    m.iterateOnce();
    assert(m.getState() == PATH_A_1);
    m.iterateOnce();
    assert(m.getState() == PATH_A_2);
    m.trigger_error = true;
    m.iterateOnce();
    assert(m.getState() == CLEANUP);
    m.iterateOnce();
    assert(m.getState() == CLEANUP);
}

void runTests_custom_error()
{
    TestStateMachine m;
    assert(m.getState() == START);
    m.iterateOnce();
    assert(m.getState() == PATH_A_1);
    m.trigger_error = true;
    m.iterateOnce();
    assert(m.getState() == PATH_B_3);
    m.trigger_error = false;
    m.iterateOnce();
    assert(m.getState() == END);
}
}

int main(void)
{
    test::runTests_path_a();
    test::runTests_path_b();
    test::runTests_default_error();
    test::runTests_custom_error();
    std::cout << std::endl << "****************";
    std::cout << std::endl << "usm tests passed";
    std::cout << std::endl << "****************" << std::endl;
    return 0;
}
