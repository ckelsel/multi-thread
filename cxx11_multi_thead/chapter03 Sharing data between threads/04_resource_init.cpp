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
using namespace std;

#include <memory>
#include <mutex>
#include <iostream>

class resource {
public:
    resource() {
        std::cout << "call resource\n";
    }
    void working() {}
};

std::shared_ptr<resource> resource_ptr;

void foo_non_thread_safe()
{
    if (!resource_ptr) {
        resource_ptr.reset(new resource);
    }
    resource_ptr->working();
}

///////////////////////////////////////////////////////////

std::mutex resource1_mutex;
std::shared_ptr<resource> resource1_ptr;
void foo_thread_safe_with_std_unique_lock()
{
    std::unique_lock<std::mutex> mylock(resource1_mutex);
    if (!resource1_ptr) {
        resource1_ptr.reset(new resource);
    }
    mylock.unlock();

    resource1_ptr->working();
}

///////////////////////////////////////////////////////////

std::shared_ptr<resource> resource_ptr2;
void resource_init() { resource_ptr2.reset(new resource); }

std::once_flag resource_once_flag;
void foo_thread_safe_with_std_call_once()
{
    std::call_once(resource_once_flag, resource_init);
    resource_ptr2->working();
}

int main(int argc, char **argv) {
    std::thread t1(foo_thread_safe_with_std_call_once);
    std::thread t2(foo_thread_safe_with_std_call_once);
    std::thread t3(foo_thread_safe_with_std_call_once);

    std::thread t4(foo_thread_safe_with_std_unique_lock);
    std::thread t5(foo_thread_safe_with_std_unique_lock);
    std::thread t6(foo_thread_safe_with_std_unique_lock);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    return 0; }
