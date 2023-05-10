#include "Buratino.h"
#include "TaskSwitcher.h"

extern "C" __attribute__((naked)) void RunTask() {
  TaskSwitcher* ts;
  TaskInfo* arg;

  uintptr_t sp;

  asm volatile("in %A0, __SP_L__\n"  // Save SP (low byte) to the local variable 'sp'
               "in %B0, __SP_H__\n"  // Save SP (high byte) to the local variable 'sp'
               "lds %A1, %0\n"       // Load the 'this' pointer (low byte) from the stack into 'object'
               "lds %B1, %0 + 1\n"   // Load the 'this' pointer (high byte) from the stack into 'object'
               "lds %A2, %0 + 2\n"   // Load the arg address (low byte) from the stack into 'arg'
               "lds %B2, %0 + 3\n"   // Load the arg address (high byte) from the stack into 'arg'
               : "=r"(sp), "=r"(ts), "=r"(arg));

  // Call the member function using C++
  ts->RunTask(arg);
  ts->CompleteTask(arg);
}

void TaskSwitcher::AddTask(Task::TaskDelegate& task, Task::TaskDelegate::Argument* arg, int16_t stackSize) {
  auto new_task = 0;
  while (_tasks[new_task].stack_pointer && new_task < _tasks.Length()) ++new_task;

  if (new_task == _tasks.Length()) {
    TaskInfo taskInfo;
    _tasks.Add(taskInfo);
  };

  auto taskInfo = &_tasks[new_task];
  taskInfo->id = new_task;
  taskInfo->delegate = task;
  taskInfo->pc = (void*)::RunTask;
  taskInfo->stack = new int8_t[stackSize];
  uint8_t* sp = &taskInfo->stack[stackSize - 1];

  // Push the 'this' pointer onto the stack
  uint16_t thisAddr = (uint16_t)(void*)this;
  *sp-- = (uint8_t)(thisAddr & 0xFF);
  *sp-- = (uint8_t)(thisAddr >> 8);

  // Push the argument address onto the stack
  uint16_t argAddr = (uint16_t)(void*)arg;
  *sp-- = (uint8_t)(argAddr & 0xFF);
  *sp-- = (uint8_t)(argAddr >> 8);

  taskInfo->stack_pointer = sp;
}

void TaskSwitcher::CompleteTask(TaskInfo* taskInfo) {
  Switch(nullptr, nullptr);
}

void TaskSwitcher::SaveContext() {
  // Save the current task's context
  asm volatile("push r0");
  asm volatile("in r0, __SREG__");
  asm volatile("push r0");
  asm volatile("push r1");
  asm volatile("push r2");
  asm volatile("push r3");
  asm volatile("push r4");
  asm volatile("push r5");
  asm volatile("push r6");
  asm volatile("push r7");
  asm volatile("push r8");
  asm volatile("push r9");
  asm volatile("push r10");
  asm volatile("push r11");
  asm volatile("push r12");
  asm volatile("push r13");
  asm volatile("push r14");
  asm volatile("push r15");
  asm volatile("push r16");
  asm volatile("push r17");
  asm volatile("push r18");
  asm volatile("push r19");
  asm volatile("push r20");
  asm volatile("push r21");
  asm volatile("push r22");
  asm volatile("push r23");
  asm volatile("push r24");
  asm volatile("push r25");
  asm volatile("push r26");
  asm volatile("push r27");
  asm volatile("push r28");
  asm volatile("push r29");
  asm volatile("push r30");
  asm volatile("push r31");

  // Load the stack pointer of the next task
  asm volatile(
    "out __SP_L__, %A0\n\t"        // Store low byte of the variable to the low byte of the stack pointer
    "out __SP_H__, %B0\n\t"        // Store high byte of the variable to the high byte of the stack pointer
    :                              // Output operands (none)
    : "r"(_tasks[current_task].stack_pointer)  // Input operand
    :                              // Clobbered registers (none)
  );
}

void TaskSwitcher::RestoreContext() {
  // // Load the stack pointer of the next task
  asm volatile(
    "in %A0, __SP_L__\n\t"                      // Load low byte of the stack pointer to the low byte of the variable
    "in %B0, __SP_H__\n\t"                      // Load high byte of the stack pointer to the high byte of the variable
    : "=r"(_tasks[current_task].stack_pointer)  // Output operand
    :                                           // Input operands (none)
    :                                           // Clobbered registers (none)
  );

  // Restore the next task's context
  asm volatile("pop r31");
  asm volatile("pop r30");
  asm volatile("pop r29");
  asm volatile("pop r28");
  asm volatile("pop r27");
  asm volatile("pop r26");
  asm volatile("pop r25");
  asm volatile("pop r24");
  asm volatile("pop r23");
  asm volatile("pop r22");
  asm volatile("pop r21");
  asm volatile("pop r20");
  asm volatile("pop r19");
  asm volatile("pop r18");
  asm volatile("pop r17");
  asm volatile("pop r16");
  asm volatile("pop r15");
  asm volatile("pop r14");
  asm volatile("pop r13");
  asm volatile("pop r12");
  asm volatile("pop r11");
  asm volatile("pop r10");
  asm volatile("pop r9");
  asm volatile("pop r8");
  asm volatile("pop r7");
  asm volatile("pop r6");
  asm volatile("pop r5");
  asm volatile("pop r4");
  asm volatile("pop r3");
  asm volatile("pop r2");
  asm volatile("pop r1");
  asm volatile("pop r0");
  asm volatile("out __SREG__, r0");
  asm volatile("pop r0");  
}

void TaskSwitcher::RunTask(TaskInfo* taskInfo) {
  taskInfo->delegate(this, taskInfo->arg);
  DeleteTask(taskInfo);
}

void TaskSwitcher::DeleteTask(TaskInfo* taskInfo) {
  for (auto i = 0; i < _tasks.Length(); ++i) {
    if (&_tasks[i] == taskInfo) {
      delete[] taskInfo->stack;
      TaskInfo blank;
      _tasks[i] = blank;
      break;
    }
  }
}