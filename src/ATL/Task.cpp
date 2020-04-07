//
//  Task.cpp
//  atlre
//
//  Created by jacques tronconi on 07/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#include "Task.h"

namespace Atl
{
    Task::Task(std::future < void >& task): mMain(std::move(task))
    {

    }

    Task& Task::then(std::function < void(void) > cbk)
    {
        mThen = cbk;
        return *this;
    }

    std::future < void > Task::exec() 
    {
        return std::async(std::launch::async, [this]()
        {
            mMain.get();
            if (mThen) mThen();
        });
    }
}