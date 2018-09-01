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


/*

    A watchdog for the ethminer


    * get informed about some events (pool connection)
    * query periodic subitted solutions and hashrate !

    What should it check ?
       - Last new job from pool
                As ETH block time is about 14sec (should go up to 30sec) we should get at least
                every 30 sec a new job Multiply with 10 ==> 300sec
                see also: https://etherscan.io/chart/blocktime
                Solution: Switch to next pool or reset connection
       - Last successfull submitted share (honor is_paused)
       - Last successfull submitted share (per card) (honor is_paused)
       - Calculated/Reported hashrate per card (honor is_paused)
       - check farm.stop() [sometimes a miner thread does not return and stop() never returns]
*/

#include <chrono>
#include <thread>

//#include <boost/asio.hpp>

#include <libethcore/Farm.h>
#include <libethcore/Miner.h>

#include "watchdog.h"

using namespace std;
using namespace dev;
using namespace eth;


// log, warn, note
struct WDLogChannel : public LogChannel
{
    static const int verbosity = 2;
    string getThreadName() { return "wd"; }
};
#define wdlog clog(WDLogChannel)


const std::chrono::steady_clock::time_point emptyTime = {};


WatchDog::WatchDog() {}

void WatchDog::start()
{
    return;

    if (m_running.load(std::memory_order_relaxed))
        return;
    m_running.store(true, std::memory_order_relaxed);
    m_timerstopping = false;
    m_workThread = std::thread{boost::bind(&WatchDog::workLoop, this)};
    wdlog << "Starting...";
}

void WatchDog::stop()
{
    if (!m_running.load(std::memory_order_relaxed))
        return;
    m_running.store(false, std::memory_order_relaxed);
    m_timerstopping = true;
    wdlog << "Stopping...";
    m_workThread.join();
}

void WatchDog::workLoop()
{
    while (true)
    {
        m_timerstopping.wait(std::chrono::seconds(10), true);
        if (m_timerstopping)
            break;
#if 0
        // values needed by the watchdog

        SolutionStats s = m_farm.getSolutionStats();
        WorkingProgress p = m_farm.miningProgress();
        for (size_t i = 0; i < m_farm.getMiners().size(); i++)
        {
            // auto const& miner = m_farm.getMiner(index); - "miner" is not needed

            // paused flag per gpu
            p.miningIsPaused[i]

            // shares per gpu
            s.getAccepts(i); s.getRejects(i); s.getFailures(i); s.getAcceptedStales(i); s.getLastUpdated(i)

            // hashrate per gpu
            p.minersHashRates[i]
        }

#endif
        {
            wdlog << "Watchdog periodic event...";
        }
    }
    wdlog << "Watchdog workLoop end";
}

void WatchDog::poolConnected()
{
    m_poolConnected = std::chrono::steady_clock::now();
    m_poolNewJob = emptyTime;
    m_poolFirstJob = emptyTime;
    m_poolDisconnected = emptyTime;
}
void WatchDog::poolDisconnected()
{
    m_poolConnected = emptyTime;
    m_poolNewJob = emptyTime;
    m_poolFirstJob = emptyTime;
    m_poolDisconnected = std::chrono::steady_clock::now();
}
void WatchDog::poolNewJob()
{
    auto now = std::chrono::steady_clock::now();
    m_poolNewJob = now;
    if (m_poolFirstJob == emptyTime)
        m_poolFirstJob = now;
}

void WatchDog::dagGenerationStart(unsigned index)
{
    (void)index;
}

void WatchDog::dagGenerationStop(unsigned index)
{
    (void)index;
}

void WatchDog::farmStart() /* start the miners */
{
    m_farmStart = std::chrono::steady_clock::now();
}

void WatchDog::farmStop() /* stop the miners */
{
    m_farmStart = emptyTime;
    m_farmStop = std::chrono::steady_clock::now();
}


/* vim:set ts=4 et: */
