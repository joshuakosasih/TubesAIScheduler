#include "ViewAdapter.hpp"

void split(const string &s, char delim, vector<string> &elems) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

Day day_from_int(int x) {
	switch (x) {
	case 1: return Day::Monday;
	case 2: return Day::Tuesday;
	case 3: return Day::Wednesday;
	case 4: return Day::Thursday;
	case 5: return Day::Friday;
	}
	throw invalid_argument("Invalid day");
}

set<Day> parse_days(const string s) {
	vector<string> elems = split(s, ',');
	set<Day> out;

	for (auto& day_string : elems) {
		out.insert(day_from_int(stoi(day_string)));
	}

	return out;
}

ViewAdapter::ViewAdapter()
{
}

void ViewAdapter::add_room(const string room_description)
{
	vector<string> elems = split(room_description, ';');
	string name = elems[0];
	int open_time = stoi(elems[1].substr(0, 2));
	int close_time = stoi(elems[2].substr(0, 2));
	set<Day> days = parse_days(elems[3]);

	room_by_name[name] = pRoom(new Classroom(name, open_time, close_time, days));
}

void ViewAdapter::add_course(const string course_description)
{
	vector<string> elems = split(course_description, ';');
	string name = elems[0];
	vector<string> room_names = split(elems[1], ',');
	int open_time = stoi(elems[2].substr(0, 2));
	int close_time = stoi(elems[3].substr(0, 2));
	int duration = stoi(elems[4]);
	set<Day> days = parse_days(elems[5]);

	vector<pRoom> rooms;
	if (room_names[0] == "-") {
		for (auto p : room_by_name) {
			rooms.push_back(p.second);
		}
	}
	else {
		for (auto s : room_names) {
			rooms.push_back(room_by_name[s]);
		}
	}

	course_by_name[name] = pCourse(new Course(name, duration, open_time, close_time, rooms, days));
}


void ViewAdapter::move_course(const string course_name, const string room_name, const int dest_day, const int dest_time)
{
	pCourse course = course_by_name[course_name];
	Schedule old_schedule = course->get_schedule();
	course->set_schedule(Schedule(
		room_by_name[room_name],
		day_from_int(dest_day),
		dest_time,
		dest_time + course->duration));

}

CourseSchedule ViewAdapter::get_course_result(const string course_name) const
{
	const pCourse &c = course_by_name.at(course_name);
	const Schedule &s = c->get_schedule();

	CourseSchedule ct;
	ct.day = (int) s.day;
	ct.time = s.start_time;
	ct.duration = s.end_time - s.start_time;
	ct.room_name = s.room->name;

	return ct;
}

void ViewAdapter::randomize_schedule()
{
	RoomVector rv;

	for (auto r : room_by_name) {
		rv.push_back(r.second);
	}

	CourseVector cv;

	for (auto c : course_by_name) {
		cv.push_back(c.second);
	}

	State s(rv, cv);

	typedef std::mt19937 MyRNG;
  uint32_t seed_val = std::chrono::system_clock::now().time_since_epoch().count();
  MyRNG generator;
  generator.seed(seed_val);
	s.init_random_schedule(generator);

	update_courses(s.get_courses());
}

void ViewAdapter::update_courses(const CourseVector& course_results)
{
	for (auto pc : course_results) {
		course_by_name[pc->name] = pc;
	}
}
