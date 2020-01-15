#include "stdafx.h"

#include "ScopeLock.hpp"

#include "Task.hpp"
#include "TaskManager.hpp"

#include <thread>

#include <tbb/task_scheduler_init.h>

xr_unique_ptr<TaskManagerBase> TaskScheduler;

TaskManagerBase::TaskManagerBase() : taskerSleepTime(2), shouldStop(true) {}

void TaskManagerBase::Initialize()
{
    if (!shouldStop)
        return;

    shouldStop = false;
    Threading::SpawnThread(taskManagerThread, "X-Ray Task Scheduler thread", 0, this);
    Threading::SpawnThread(taskWatcherThread, "X-Ray Task Watcher thread", 0, this);
}

void TaskManagerBase::Destroy()
{
    if (shouldStop)
        return;

    shouldStop = true;
    mainThreadExit.Wait();
    watcherThreadExit.Wait();
}

bool TaskManagerBase::TaskQueueIsEmpty() const
{
    return tasks.empty() && tasksInExecution.empty();
}

void TaskManagerBase::taskManagerThread(void* thisPtr)
{
    tbb::task_scheduler_init init;

    TaskManagerBase& self = *static_cast<TaskManagerBase*>(thisPtr);

    while (!self.shouldStop)
    {
        self.lock.Enter();
        if (self.tasks.empty())
        {
            self.lock.Leave();
            Sleep(self.taskerSleepTime);
            continue;
        }

        for (Task* task : self.tasks)
        {
            if (task->CheckIfExecutionAllowed())
            {
                self.SpawnTask(task);
                break; // Iterators are invalid now, we should break here
            }
        }
        self.lock.Leave();
    }
    self.mainThreadExit.Set();
}

void TaskManagerBase::taskWatcherThread(void* thisPtr)
{
    TaskManagerBase& self = *static_cast<TaskManagerBase*>(thisPtr);
    bool calmDown = false;

    while (!self.shouldStop)
    {
        self.executionLock.Enter();
        if (self.tasksInExecution.empty())
        {
            self.executionLock.Leave();
            Sleep(WATCHER_CALM_DOWN_PERIOD);
            continue;
        }

#ifndef MASTER_GOLD
        for (Task* task : self.tasksInExecution)
        {
            if (!task->IsStarted())
                continue;

            const u64 time = task->GetElapsedMs();

            if (time > ABNORMAL_EXECUTION_TIME)
            {
                calmDown = true;
                Msg("! Abnormal task execution time [%dms] in [ %s ]", time, task->GetName());
            }

#ifdef PROFILE_TASKS
            bool tooLong;
            bool veryLong;

            else if (task->IsComplex())
            {
                tooLong = time > COMPLEX_TASK_ACCEPTABLE_EXECUTION_TIME;
                veryLong = time > COMPLEX_TASK_ACCEPTABLE_EXECUTION_TIME * 2;
            }
            else
            {
                tooLong = time > SIMPLE_TASK_ACCEPTABLE_EXECUTION_TIME;
                veryLong = time > SIMPLE_TASK_ACCEPTABLE_EXECUTION_TIME * 2;
            }

            if (veryLong)
            {
                calmDown = true;
                Msg("! Task [%s] runs too long %dms", task->GetName(), time);
            }
            else if (tooLong)
            {
                calmDown = true;
                Msg("~ Task [%s] runs very long %dms", task->GetName(), time);
            }
#endif // PROFILE_TASKS
        }
#endif // MASTER_GOLD
        self.executionLock.Leave();
        if (calmDown)
        {
            Sleep(WATCHER_CALM_DOWN_PERIOD);
            calmDown = false;
        }
    }

    // Wait until last task is done
    while (true)
    {
        ScopeLock scope(&self.executionLock);
        
        if (self.tasksInExecution.empty())
            break;

        Sleep(WATCHER_CALM_DOWN_PERIOD);
    }

    self.watcherThreadExit.Set();
}

void TaskManagerBase::AddTask(pcstr name, Task::TaskFunc taskFunc,
    Task::IsAllowedCallback callback /*= nullptr*/, Task::DoneCallback done /*= nullptr*/,
    Event* doneEvent /*= nullptr*/)
{
    Task* task = new (tbb::task::allocate_root()) Task(name, std::move(taskFunc),
        std::move(callback), std::move(done), doneEvent);

    if (!task->isExecutionAllowed)
    {
        SpawnTask(task, true);
        return;
    }

    lock.Enter();
    tasks.emplace_back(task);
    lock.Leave();
}

void TaskManagerBase::RemoveTask(Task::TaskFunc&& func)
{
    ScopeLock scope(&lock);

    xr_vector<Task*>::iterator it;
    const auto search = [&]()
    {
        it = std::find_if(tasks.begin(), tasks.end(), [&](Task* task)
        {
            return func == task->task;
        });
        return it;
    };
    if (search() != tasks.end())
    {
        Task::destroy(**it);
        tasks.erase(it);
    }
}

void TaskManagerBase::RemoveTasksWithName(pcstr name)
{
    ScopeLock scope(&lock);

    xr_vector<Task*>::iterator it;
    const auto search = [&]()
    {
        it = std::find_if(tasks.begin(), tasks.end(), [&](Task* task)
        {
            return 0 == xr_strcmp(name, task->GetName());
        });
        return it;
    };

    while (search() != tasks.end())
    {
        Task::destroy(**it);
        tasks.erase(it);
    }
}

void TaskManagerBase::SpawnTask(Task* task, bool shortcut /*= false*/)
{
    if (!shortcut)
    {
        const auto it = std::find(tasks.begin(), tasks.end(), task);
        R_ASSERT3(it != tasks.end(), "Task is deleted from the task manager", task->GetName());

        // Remove it from the queue
        tasks.erase(it);
    }

    executionLock.Enter();
    tasksInExecution.emplace_back(task);
    executionLock.Leave();

    // Run it
    tbb::task::spawn(*task);
}

void TaskManagerBase::TaskDone(Task* task, u64 executionTime)
{
    executionLock.Enter();

    const auto it = std::find(tasksInExecution.begin(), tasksInExecution.end(), task);
    R_ASSERT3(it != tasksInExecution.end(), "Task is deleted from the task watcher", task->GetName());

    tasksInExecution.erase(it);

    executionLock.Leave();

    if (executionTime > ABNORMAL_EXECUTION_TIME)
    {
        Msg("! Task done after abnormal execution time [%dms] in [%s]", time, task->GetName());
    }
    else if (executionTime > BIG_EXECUTION_TIME)
    {
        Msg("~ Task done after big execution time [%dms] in [%s]", time, task->GetName());
    }
}
