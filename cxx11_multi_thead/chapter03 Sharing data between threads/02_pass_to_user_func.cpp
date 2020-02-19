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
#include <mutex>
using namespace std;

class mydata {
public:
    void work() {}

private:
    int id_;
};

class mydata_wrapper {
public:
    template <typename func>
    void process(func f)
    {
        std::lock_guard<mutex> lock(mutex_);
        f(data_);
    }

private:
    mydata data_;
    mutex mutex_;
};

mydata *g_data;

/**
 * @brief 保存mydata的地址
 *
 * @param d mydata
 */
void dangerous_func(mydata &d) { g_data = &d; }

int main(int argc, char **argv)
{
    mydata_wrapper wrapper;
    wrapper.process(dangerous_func);

    // 直接使用，没有用mutex保护
    g_data->work();
    return 0;
}
