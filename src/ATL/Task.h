//
//  Task.h
//  atlre
//
//  Created by jacques tronconi on 07/04/2020.
//  Copyright © 2020 Atlanti's Corporation. All rights reserved.
//

#ifndef ATL_TASK_H
#define ATL_TASK_H

#include "Platform.h"

namespace Atl
{
    //! @brief Defines a very simple Task system.
    //! A Task is defined by a simple std::future < void > shared state and a 
    //! possible 'then' function. The first future is executed asynchroneously 
    //! and Task::exec() waits for this future to execute the then() function.
    class Task 
    {
        //! @brief The main function.
        std::future < void > mMain;

        //! @brief The 'then' function.
        std::function < void(void) > mThen;

        public:

        //! @brief Constructs a new Task.
        Task(std::future < void >& task);

        //! @brief Sets the 'then' function for this Task.
        Task& then(std::function < void(void) > cbk);

        //! @brief Executes the Task asynchroneously.
        std::future < void > exec();
    };
}

#endif // ATL_TASK_H