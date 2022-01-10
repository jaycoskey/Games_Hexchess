// Copyright (C) 2021, by Jay M. Coskey
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

// #include <ctime>
// #include <time.h>

// #include <iostream>
// #include <map>
// #include <memory>

#include <QObject>
#include <QThread>

#include "server.h"
#include "util_hexchess.h"
#include "variant.h"


namespace hexchess::server {

class ServerThread : public Server {
public:
    ServerThread() {
        _thread = new QThread;
        _thread->start();
        this->moveToThread(_thread);
        cout << "Server threadId="
             << _thread->currentThreadId() << "\n";
    }

    ~ServerThread() {
        // TODO: delete _thread;
    }

protected:
    QThread *_thread;
};

}  // namespace hexchess::server
