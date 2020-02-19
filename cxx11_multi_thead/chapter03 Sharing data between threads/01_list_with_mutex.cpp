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
#include <list>
#include <mutex>
using namespace std;

list<int> g_list;
mutex g_mutex;

void list_add(int v)
{
    lock_guard<mutex> lock(g_mutex);

    g_list.push_back(v);
}

int list_remove()
{
    lock_guard<mutex> lock(g_mutex);

    int v = g_list.front();
    g_list.pop_front();
    return v;
}

class LockList {
public:
    void list_add(int v)
    {
        lock_guard<mutex> lock(m_mutex);

        m_list.push_back(v);
    }

    int list_remove()
    {
        lock_guard<mutex> lock(m_mutex);

        int v = m_list.front();
        m_list.pop_front();
        return v;
    }

    list<int> *get() { return &m_list; }

private:
    list<int> m_list;
    mutex m_mutex;
};

int main(int argc, char **argv)
{
    {
        list_add(1);
        std::thread t(list_remove);
        t.join();
    }

    {
        LockList list;
        list.list_add(1);
        std::thread t([&](){
                list.list_remove();
                });
        t.join();
    }
    return 0;
}
