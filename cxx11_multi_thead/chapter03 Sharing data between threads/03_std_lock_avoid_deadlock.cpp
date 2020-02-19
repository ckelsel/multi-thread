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
#include <thread>
#include <iostream>

// class big_object {
// };
//
// void myswap(big_object &, big_object &) {}
//
// class X {
// public:
//     X(big_object const &o) : o_(o) {}
//
//     friend void swap2(X& a, X& b)
//     {
//         // mutex不支持递归，必须保证a、b不是同一个
//         if (&a == &b) {
//             return;
//         }
//
//         // 第一种方法
//         // 同时锁定a,b
//         std::lock(a.mutex_, b.mutex_);
//         // std::adopt_lock表示已经锁定，不需要再加锁，在析构时释放锁
//         std::lock_guard<std::mutex> locka(a.mutex_, std::adopt_lock);
//         std::lock_guard<std::mutex> lockb(b.mutex_, std::adopt_lock);
//
//         // // 第二种方法
//         // // std::defer_lock，不加锁，在析构时释放锁
//         // std::unique_lock<mutex> locka(a.mutex_, std::defer_lock);
//         // std::unique_lock<mutex> lockb(b.mutex_, std::defer_lock);
//         // // 还未锁定，这时候同时锁定a,b
//         // std::lock(a.mutex_, b.mutex_);
//         //
//         myswap(a.o_, b.o_);
//     }
//
// private:
//     big_object o_;
//     std::mutex mutex_;
// };
//
// void test_X()
// {
//     big_object a, b;
//     X xa(a);
//     X xb(b);
//     swap2(xa, xb);
// }

////////////////////////////////////////////////////////////////

struct bank_account {
    explicit bank_account(int balance) : balance(balance) {}
    int balance;
    std::mutex m;
};

void transfer(bank_account &from, bank_account &to, int amount)
{

    std::cout << __FUNCTION__ << " enter, " << std::this_thread::get_id() << "\n" << std::flush;

    // 保证二个已锁定互斥在作用域结尾解锁
    // // 锁定两个互斥而不死锁
    std::lock(from.m, to.m);
    std::lock_guard<std::mutex> lock1(from.m, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(to.m, std::adopt_lock);

    // 等价方法：
    // std::unique_lock<std::mutex> lock1(from.m, std::defer_lock);
    // std::unique_lock<std::mutex> lock2(to.m, std::defer_lock);
    // std::lock(lock1, lock2);

    from.balance -= amount;
    to.balance += amount;
    // lock1.unlock();
    // lock2.unlock();

    std::cout << __FUNCTION__ << " leave, " << std::this_thread::get_id() << "\n" << std::flush;
}

class scoped_thread {
public:
    scoped_thread(std::thread t) : t_(std::move(t)){}
    ~scoped_thread(){
        std::cout << __FUNCTION__ << " enter, " << std::this_thread::get_id() << "\n" << std::flush;
        t_.join();
        std::cout << __FUNCTION__ << " leave, " << std::this_thread::get_id() << "\n" << std::flush;
    }
private:
    std::thread t_;
};

void test()
{
    std::cout << __FUNCTION__ << " enter, " << std::this_thread::get_id() << "\n" << std::flush;

    bank_account my_account(100);
    bank_account your_account(50);

    scoped_thread t1(std::thread(transfer, std::ref(my_account), std::ref(your_account), 10));
    // scoped_thread t2(std::thread(transfer, std::ref(your_account), std::ref(my_account), 5));

    std::cout << __FUNCTION__ << " leave, " << std::this_thread::get_id() << "\n" << std::flush;
	std::this_thread::sleep_for(std::chrono::microseconds(1));

}

int main(int argc, char **argv)
{
    test();
    return 0;
}
