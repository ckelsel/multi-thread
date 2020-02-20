/**-------------------------------------------------------------------------
 *
 * Copyright (C) 2020 KunMing Xie <kunming.xie@hotmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

bool g_stop = false;
int g_food;
std::mutex g_mutex;
std::condition_variable g_condition_variable;

void doing_nastying_job()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cerr << "doing_nastying_job finish\n";
}

void producer_should_stop()
{
    static int count = 0;
    if (++count > 10) {
        g_stop = true;
    }
}

void producer_thread()
{
    while (!g_stop) {
        {
            std::unique_lock<std::mutex> lk(g_mutex);
            g_food++;

            producer_should_stop();
            g_condition_variable.notify_all();
            std::cerr << "notify all\n";
        }

        doing_nastying_job();
    }
}

void consumer_thread()
{
    while (!g_stop) {
        std::unique_lock<std::mutex> lk(g_mutex);  // lock
        std::cerr << std::this_thread::get_id() << ", waiting for producer\n";

        g_condition_variable.wait(
            lk, [] { return g_food > 0 || g_stop; });  // unlock, lock
        std::cerr << std::this_thread::get_id()
                  << ", waiting for producer success\n";

        if (g_stop) {
            break;
        }

        g_food--;
        // unlock
    }
}

int main(int argc, char **argv)
{
    std::thread t1(producer_thread);
    std::thread t2(consumer_thread);
    std::thread t3(consumer_thread);
    std::thread t4(consumer_thread);
    std::thread t5(consumer_thread);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    return 0;
}
