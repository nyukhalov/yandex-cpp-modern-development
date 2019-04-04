#include <string>
#include <set>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

/**
 * The final project (week 5)
 */

class Date {
public:
    Date() {
        year = 1;
        month = 1;
        day = 1;
    }

    Date(int new_year, int new_month, int new_day) {
        if (new_month < 1 || new_month > 12) {
            throw invalid_argument("Month value is invalid: " + to_string(new_month));
        }
        if (new_day < 1 || new_day > 31) {
            throw invalid_argument("Day value is invalid: " + to_string(new_day));
        }
        year = new_year;
        month = new_month;
        day = new_day;
    }

    int GetYear() const { return year; }

    int GetMonth() const { return month; }

    int GetDay() const { return day; }

    long ToDays() const { return 365 * year + 31 * month + day; }

private:
    int year;
    int month;
    int day;
};

bool operator<(const Date &lhs, const Date &rhs) {
    return lhs.ToDays() < rhs.ToDays();
}

void HandleDateSeparator(istream& stream) {
    if (stream.peek() != '-') {
        throw runtime_error("Invalid format");
    }
    stream.ignore(1);
}

istream& operator>>(istream &stream, Date &date) {
    int year, month, day;
    stream >> year;
    HandleDateSeparator(stream);
    stream >> month;
    HandleDateSeparator(stream);
    if (stream >> day) {
        date = Date(year, month, day);
        return stream;
    }
    throw runtime_error("Invalid format");
}

ostream& operator<<(ostream &stream, const Date &date) {
    stream << setw(4) << setfill('0') << date.GetYear() << "-"
           << setw(2) << setfill('0') << date.GetMonth() << "-"
           << setw(2) << setfill('0') << date.GetDay();
    return stream;
}

class Database {
public:
    void AddEvent(const Date &date, const string &event) {
        data[date].insert(event);
    }

    bool DeleteEvent(const Date &date, const string &event) {
        if (data.count(date) <= 0) return false;
        if (data[date].count(event) <= 0) return false;
        data[date].erase(event);
        return true;
    }

    int DeleteDate(const Date &date) {
        if (data.count(date) <= 0) return 0;
        int num = data[date].size();
        data.erase(date);
        return num;
    }

    set<string> Find(const Date &date) const {
        if (data.count(date) <= 0) return {};
        return data.at(date);
    }

    void Print() const {
        for (const auto &entry : data) {
            const Date &date = entry.first;
            for (const auto &event : entry.second) {
                cout << date << " " << event << endl;
            }
        }
    }

private:
    map<Date, set<string>> data;
};

Date ReadDate(const string& date_str) {
    try {
        stringstream datess(date_str);
        Date date;
        datess >> date;
        if (datess.peek() != EOF) {
            // the stream still contains something
            throw runtime_error("Invalid format");
        }
        return date;
    } catch(runtime_error& ex) {
        throw runtime_error("Wrong date format: " + date_str);
    }
}

int main() {
    Database db;

    try {
        string command;
        while (getline(cin, command)) {
            if (command.empty()) continue;
            stringstream ss(command);
            string operation;
            ss >> operation;

            if (operation == "Add") {
                string date_str;
                string event;
                ss >> date_str >> event;
                Date date = ReadDate(date_str);
                db.AddEvent(date, event);
            } else if (operation == "Del") {
                string date_str;
                string event;
                ss >> date_str;
                Date date = ReadDate(date_str);
                if (ss >> event) {
                    // there was event in the command
                    if (db.DeleteEvent(date, event)) {
                        cout << "Deleted successfully" << endl;
                    } else {
                        cout << "Event not found" << endl;
                    }
                } else {
                    // it's delete all command
                    int n_deleted = db.DeleteDate(date);
                    cout << "Deleted " << n_deleted << " events" << endl;
                }

            } else if (operation == "Find") {
                string date_str;
                ss >> date_str;
                Date date = ReadDate(date_str);
                set<string> events = db.Find(date); // can be empty
                for (const auto &event : events) {
                    cout << event << endl;
                }
            } else if (operation == "Print") {
                db.Print();
            } else {
                throw runtime_error("Unknown command: " + operation);
            }
        }
    } catch (exception &ex) {
        cout << ex.what() << endl;
    }

    return 0;
}