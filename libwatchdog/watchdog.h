/*
 This file is part of ethminer.

 ethminer is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ethminer is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with ethminer.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <boost/asio.hpp>

#include <libethcore/Farm.h>
#include <libethcore/Miner.h>
using namespace std;

namespace dev
{
namespace eth
{
class WatchDog
{
public:
    WatchDog();
    void start();
    void stop();

    void poolConnected();
    void poolDisconnected();
    void poolNewJob();

    void dagGenerationStart(unsigned index);
    void dagGenerationStop(unsigned index);

    void farmStart(); /* start the miners */
    void farmStop();  /* stop the miners */

private:
    void workLoop();
    std::thread m_workThread;
    std::atomic<bool> m_running = {false};
    Notified<bool> m_timerstopping = {false}; /* to generate an interruptable sleep timer */


    std::chrono::steady_clock::time_point m_poolConnected;
    std::chrono::steady_clock::time_point m_poolDisconnected;
    std::chrono::steady_clock::time_point m_poolFirstJob;
    std::chrono::steady_clock::time_point m_poolNewJob;

    std::chrono::steady_clock::time_point m_farmStart;
    std::chrono::steady_clock::time_point m_farmStop;

#if 0
    Farm& m_farm;
#endif
};
}  // namespace eth
}  // namespace dev

/* vim:set ts=4 et: */
