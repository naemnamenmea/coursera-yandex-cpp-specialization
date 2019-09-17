//#define codeforces
#ifdef codeforces

#include <bits/stdc++.h>
#include <unordered_map>
#include <unordered_set>

#define INF 1e9
#define EPS 1e-7
#define MAX int(1e4+1)
#define loop(i,l,r) for(auto i=l; i<r; ++i)
#define loopi(i,l,r) for(auto i=l; i<=r; ++i)

typedef long long ll;

using namespace std;

enum class TaskStatus {
	NEW,          // нова€
	IN_PROGRESS,  // в разработке
	TESTING,      // на тестировании
	DONE          // завершена
};

using TasksInfo = map<TaskStatus, int>;

TasksInfo reduce(const TasksInfo& ti) {
	TasksInfo res;

	for (const auto& el : ti) {
		if (el.second > 0)
			res.insert(el);
	}
	return res;
}

class TeamTasks {
public:
	// ѕолучить статистику по статусам задач конкретного разработчика
	const TasksInfo GetPersonTasksInfo(const string& person) const {
		return reduce(m.at(person));
	}

	// ƒобавить новую задачу (в статусе NEW) дл€ конкретного разработчитка
	void AddNewTask(const string& person) {
		++m[person][TaskStatus::NEW];
		m[person][TaskStatus::NEW];
		m[person][TaskStatus::IN_PROGRESS];
		m[person][TaskStatus::TESTING];
		m[person][TaskStatus::DONE];
	}

	// ќбновить статусы по данному количеству задач конкретного разработчика,
	// подробности см. ниже
	tuple<TasksInfo, TasksInfo> PerformPersonTasks(
		const string& person, int task_count) {
		TasksInfo updated, undone;
		int prev = 0, fresh;
		for (auto& task : m[person]) {
			int rem = max(0, task.second - task_count);
			fresh = rem + prev;

			if (task.first == TaskStatus::DONE) {
				rem = task.second;

			}
			else {
				undone[task.first] = rem;
			}

			updated[task.first] = prev;

			prev = min(task_count, task.second);
			task_count = max(0, task_count - task.second);
			task.second = fresh;
		}
		return { reduce(updated), reduce(undone) };
	}
private:
	unordered_map<string, TasksInfo> m;
};

// ѕринимаем словарь по значению, чтобы иметь возможность
// обращатьс€ к отсутствующим ключам с помощью [] и получать 0,
// не мен€€ при этом исходный словарь
void PrintTasksInfo(TasksInfo tasks_info) {
	cout << tasks_info[TaskStatus::NEW] << " new tasks" <<
		", " << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress" <<
		", " << tasks_info[TaskStatus::TESTING] << " tasks are being tested" <<
		", " << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}

int main() {
#ifdef LOCAL_KONTROL
	freopen("input.txt", "r", stdin);
#endif
	cin.tie(0);
	cout.tie(0);
	ios_base::sync_with_stdio(0);

	TeamTasks tasks;
	for (int i = 0; i < 5; ++i) {
		tasks.AddNewTask("Ivan");
	}

	TasksInfo updated_tasks, untouched_tasks;

	tasks.PerformPersonTasks("Ivan", 5);
	tasks.PerformPersonTasks("Ivan", 5);
	tasks.PerformPersonTasks("Ivan", 1);

	for (int i = 0; i < 5; ++i) {
		tasks.AddNewTask("Ivan");
	}

	tasks.PerformPersonTasks("Ivan", 2);
	tie(updated_tasks, untouched_tasks) =
		tasks.PerformPersonTasks("Ivan", 4);
	cout << "Updated Ivan's tasks: ";
	PrintTasksInfo(updated_tasks);
	cout << "Untouched Ivan's tasks: ";
	PrintTasksInfo(untouched_tasks);

	PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));

}
#endif