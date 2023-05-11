#include "Buratino.h"
#include "TaskSwitcher.h"

inline void CallTaskDelegate(TaskInfo& taskInfo)
{
  Task::CallTaskDelegate(taskInfo);
}

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
  CallTaskDelegate(*arg);
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

void TaskSwitcher::Setup() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 0xF424 / 4;
  TCCR1B = (1 << WGM12) | (1 << CS12);
  TIMSK1 = (1 << OCIE1A);
  sei();
}

TaskInfo& Task::GetTaskInfo(int8_t taskId) {
  return Buratino::_taskSwitcher._tasks[taskId];
}

void Task::NextTask() {
  Buratino::_taskSwitcher.NextTask();
}

int8_t Task::Id() {
  return Buratino::_taskSwitcher._tasks[Buratino::_taskSwitcher.current_task].id;
}

void Task::CallTaskDelegate(TaskInfo& task) {
  task.delegate(Buratino::_taskSwitcher, task.arg);
}

inline void SwitchToNextTask() {
  Task::NextTask();
}

inline TaskInfo& GetCurrentTaskInfo() {
  return Task::GetTaskInfo(Task::Id);
}

inline void RestoreTaskContext(int8_t taskId) {
  // Load the stack pointer of the next task
  asm volatile(
    "in %A0, __SP_L__\n\t"                           // Load low byte of the stack pointer to the low byte of the variable
    "in %B0, __SP_H__\n\t"                           // Load high byte of the stack pointer to the high byte of the variable
    : "=r"(Task::GetTaskInfo(taskId).stack_pointer)  // Output operand
    :                                                // Input operands (none)
    :                                                // Clobbered registers (none)
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

inline void SaveTaskContext(int8_t taskId) {
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
    "out __SP_L__, %A0\n\t"                         // Store low byte of the variable to the low byte of the stack pointer
    "out __SP_H__, %B0\n\t"                         // Store high byte of the variable to the high byte of the stack pointer
    :                                               // Output operands (none)
    : "r"(Task::GetTaskInfo(taskId).stack_pointer)  // Input operand
    :                                               // Clobbered registers (none)
  );
}

ISR(TIMER1_COMPA_vect) {
  auto taskId = Task::Id();

  SwitchToNextTask();
  if (taskId == Task::Id()) {
    return;
  }

  TaskInfo& taskInfo = GetCurrentTaskInfo();

  // save pc
  asm volatile(
    "push r28\n\t"             // Save Y-register low byte (r28)
    "push r29\n\t"             // Save Y-register high byte (r29)
    "in r28, __SP_L__\n\t"     // Load low byte of stack pointer (SPL) into r28 (Y-register low byte)
    "in r29, __SP_H__\n\t"     // Load high byte of stack pointer (SPH) into r29 (Y-register high byte)
    "adiw r28, %[offset]\n\t"  // Add size of local stack to Y-register
    "ld %A0, Y+\n\t"           // Load the return address low byte from the stack to pc_low and increment Y
    "ld %B0, Y\n\t"            // Load the return address high byte from the stack to pc_high
    "pop r29\n\t"              // Restore Y-register high byte (r29)
    "pop r28\n\t"              // Restore Y-register low byte (r28)
    : "=r"(taskInfo.pc)
    : [offset] "M"(sizeof(taskId) + sizeof(taskInfo) + 2)  // 2 for saving r28 and r29
  );

  SaveTaskContext(taskId);
  RestoreTaskContext(Task::Id());

  taskInfo = GetCurrentTaskInfo();

  // restore pc
  asm volatile(
    "push %A0\n\t"  // Push low byte of the target address onto the stack
    "push %B0\n\t"  // Push high byte of the target address onto the stack
    :
    : "r"(taskInfo.pc));
}
