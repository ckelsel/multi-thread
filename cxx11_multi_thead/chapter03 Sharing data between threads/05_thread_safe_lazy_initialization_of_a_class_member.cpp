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
using namespace std;

class ConnectionDetail {
};

class DataPacket {
};

class Connection {
public:
    Connection(ConnectionDetail const &detail)
    {
        std::cout << "call Connection\n";
    }
    void Send(DataPacket const &data) {}
    DataPacket Recv() { return DataPacket(); }
};

class LazyConnection {
public:
    LazyConnection(ConnectionDetail const &detail) : detail_(detail) {}

    void Send(DataPacket const &data)
    {
        std::call_once(connection_init_flag, &LazyConnection::open_connection,
                       this);
        connection_->Send(data);
    }
    DataPacket Recv()
    {
        std::call_once(connection_init_flag, &LazyConnection::open_connection,
                       this);
        return connection_->Recv();
    }

private:
    void open_connection() { connection_.reset(new Connection(detail_)); }

private:
    std::once_flag connection_init_flag;
    ConnectionDetail detail_;
    std::shared_ptr<Connection> connection_;
};

int main(int argc, char **argv)
{
    ConnectionDetail detail;
    DataPacket data;
    LazyConnection lazy(detail);
    lazy.Recv();
    lazy.Recv();
    lazy.Send(data);
    lazy.Send(data);
    return 0;
}
