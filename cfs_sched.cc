#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "multimap.h"

struct Task {
  char id;
  unsigned start;
  unsigned duration;
  unsigned vruntime = 0;

  Task(char i, unsigned s, unsigned d)
      : id(i), start(s), duration(d) {}
};

std::ostream& operator<<(std::ostream& os, const Task& t) {
  os << t.id << "(vr=" << t.vruntime << ", dur=" << t.duration << ")";
  return os;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: ./cfs_sched <task_file.dat>\n";
    return 1;
  }

  std::ifstream fin(argv[1]);
  if (!fin.is_open()) {
    std::cerr << "Error: cannot open file " << argv[1] << "\n";
    return 1;
  }

  std::vector<Task*> incoming;
  char id;
  unsigned s, d;

  while (fin >> id >> s >> d) {
    incoming.push_back(new Task(id, s, d));
  }

  std::sort(incoming.begin(), incoming.end(),
            [](Task* a, Task* b) {
              if (a->start != b->start) return a->start < b->start;
              return a->id < b->id;
            });

  Multimap<unsigned, Task*> timeline;

  unsigned tick = 0;
  size_t idx = 0;
  unsigned min_vruntime = 0;
  Task* current = nullptr;

  while (idx < incoming.size() || current || timeline.Contains(min_vruntime)) {

    // Launch tasks
    while (idx < incoming.size() && incoming[idx]->start == tick) {
      Task* t = incoming[idx];
      t->vruntime = min_vruntime;
      timeline.Insert(t->vruntime, t);
      idx++;
    }

    // Check if current must yield
    if (current) {
      if (current->vruntime > min_vruntime) {
        timeline.Insert(current->vruntime, current);
        current = nullptr;
      }
    }

    // Get next task
    if (!current) {
      try {
        unsigned key = timeline.Min();
        Task* t = timeline.Get(key);
        current = t;
        timeline.Remove(key);
        min_vruntime = t->vruntime;
      } catch (...) {
        current = nullptr;
      }
    }

    // Print status header
    size_t count_runnable = 0;
    try {
      timeline.Min();
      timeline.Max();
      // Count indirect: expensive but ok for small input
      // In production you'd track size
      // For assignment this is fine.
      // Implementation uses tree print? No: we avoid printing structure
    } catch (...) {}

    // Count tasks manually
    size_t count_wait = 0;
    for (auto* t : incoming) {
      if (t != current && t->duration > 0 && t->start <= tick)
        count_wait++;
    }

    size_t total_active = (current ? 1 : 0) + count_wait;

    std::cout << tick << " [" << total_active << "]: ";

    if (!current) {
      std::cout << "_\n";
      tick++;
      continue;
    }

    // Run 1 tick
    current->duration--;
    current->vruntime++;

    bool finished = (current->duration == 0);

    std::cout << current->id;
    if (finished) std::cout << "*";
    std::cout << "\n";

    if (finished) {
      delete current;
      current = nullptr;
    }

    tick++;
  }

  // Clean any remaining tasks in memory
  for (Task* t : incoming) {
    if (t && t->duration > 0) delete t;
  }

  return 0;
}
