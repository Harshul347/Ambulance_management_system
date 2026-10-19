
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <limits>
#include <algorithm>
using namespace std;

class Ambulance {
public:
    int id;
    string driver;
    string location;
    bool available;

    Ambulance(int i, string d, string loc)
        : id(i), driver(d), location(loc), available(true) {}
};

class Emergency {
public:
    int requestId;
    string patientName;
    string location;
    int priority;
    int ambulanceId;

    Emergency(int id, string patient, string loc, int p)
        : requestId(id), patientName(patient), location(loc),
          priority(p), ambulanceId(-1) {}
};

// Higher priority number means more urgent.
// Earlier request gets preference when priorities are equal.
struct CompareEmergency {
    bool operator()(const Emergency& a, const Emergency& b) const {
        if (a.priority != b.priority)
            return a.priority < b.priority;

        return a.requestId > b.requestId;
    }
};

class AmbulanceManagement {
private:
    vector<Ambulance> ambulances;
    vector<Emergency> emergencies;
    priority_queue<Emergency, vector<Emergency>,
                   CompareEmergency> pending;

    int nextRequestId = 1;

    int findAmbulance(int id) {
        for (int i = 0; i < (int)ambulances.size(); i++) {
            if (ambulances[i].id == id)
                return i;
        }
        return -1;
    }

    int findEmergency(int id) {
        for (int i = 0; i < (int)emergencies.size(); i++) {
            if (emergencies[i].requestId == id)
                return i;
        }
        return -1;
    }

public:
    void addAmbulance() {
        int id;
        string driver, location;

        cout << "Enter ambulance ID: ";
        if (!(cin >> id)) {
            cout << "Invalid ID!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        if (findAmbulance(id) != -1) {
            cout << "Ambulance ID already exists!\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter driver name: ";
        getline(cin, driver);

        cout << "Enter current location: ";
        getline(cin, location);

        if (driver.empty() || location.empty()) {
            cout << "Driver and location cannot be empty.\n";
            return;
        }

        ambulances.emplace_back(id, driver, location);
        cout << "Ambulance added successfully!\n";
    }

    void viewAmbulances() {
        if (ambulances.empty()) {
            cout << "No ambulances registered.\n";
            return;
        }

        cout << "\n--- AMBULANCE FLEET ---\n";

        for (const auto& a : ambulances) {
            cout << "ID: " << a.id
                 << " | Driver: " << a.driver
                 << " | Location: " << a.location
                 << " | Status: "
                 << (a.available ? "Available" : "On Emergency")
                 << '\n';
        }
    }

    void registerEmergency() {
        string patient, location;
        int priority;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter patient name: ";
        getline(cin, patient);

        cout << "Enter emergency location: ";
        getline(cin, location);

        cout << "Enter priority (1=Low, 2=Medium, 3=High): ";
        if (!(cin >> priority)) {
            cout << "Invalid priority!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        if (patient.empty() || location.empty() ||
            priority < 1 || priority > 3) {
            cout << "Invalid patient details or priority.\n";
            return;
        }

        Emergency e(nextRequestId++, patient, location, priority);

        emergencies.push_back(e);
        pending.push(e);

        cout << "Emergency registered successfully!\n";
        cout << "Request ID: " << e.requestId << '\n';
    }

    void assignAmbulance() {
        if (pending.empty()) {
            cout << "No pending emergency requests.\n";
            return;
        }

        int ambulanceIndex = -1;

        for (int i = 0; i < (int)ambulances.size(); i++) {
            if (ambulances[i].available) {
                ambulanceIndex = i;
                break;
            }
        }

        if (ambulanceIndex == -1) {
            cout << "No ambulance currently available.\n";
            return;
        }

        Emergency e = pending.top();
        pending.pop();

        int emergencyIndex = findEmergency(e.requestId);

        if (emergencyIndex == -1) {
            cout << "Emergency record not found.\n";
            return;
        }

        int aid = ambulances[ambulanceIndex].id;

        ambulances[ambulanceIndex].available = false;
        ambulances[ambulanceIndex].location = e.location;

        emergencies[emergencyIndex].ambulanceId = aid;

        cout << "\nAmbulance assigned successfully!\n";
        cout << "Request ID: " << e.requestId << '\n';
        cout << "Patient: " << e.patientName << '\n';
        cout << "Emergency location: " << e.location << '\n';
        cout << "Ambulance ID: " << aid << '\n';
        cout << "Driver: " << ambulances[ambulanceIndex].driver << '\n';
    }

    void completeEmergency() {
        int requestId;

        cout << "Enter completed emergency request ID: ";
        if (!(cin >> requestId)) {
            cout << "Invalid request ID!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        int ei = findEmergency(requestId);

        if (ei == -1) {
            cout << "Emergency request not found!\n";
            return;
        }

        int aid = emergencies[ei].ambulanceId;

        if (aid == -1) {
            cout << "This emergency has not been assigned an ambulance.\n";
            return;
        }

        int ai = findAmbulance(aid);

        if (ai == -1 || ambulances[ai].available) {
            cout << "Ambulance is not currently assigned.\n";
            return;
        }

        ambulances[ai].available = true;
        emergencies[ei].ambulanceId = -2; // Completed

        cout << "Emergency marked as completed.\n";
        cout << "Ambulance " << aid << " is now available.\n";
    }

    void viewEmergencies() {
        if (emergencies.empty()) {
            cout << "No emergency records available.\n";
            return;
        }

        cout << "\n--- EMERGENCY RECORDS ---\n";

        for (const auto& e : emergencies) {
            cout << "Request ID: " << e.requestId
                 << " | Patient: " << e.patientName
                 << " | Location: " << e.location
                 << " | Priority: " << e.priority
                 << " | Status: ";

            if (e.ambulanceId == -1)
                cout << "Pending";
            else if (e.ambulanceId == -2)
                cout << "Completed";
            else
                cout << "Assigned Ambulance " << e.ambulanceId;

            cout << '\n';
        }
    }

    void viewPending() {
        if (pending.empty()) {
            cout << "No pending emergencies.\n";
            return;
        }

        // Copy the queue so the original queue remains unchanged.
        auto temp = pending;

        cout << "\n--- PENDING EMERGENCIES ---\n";

        while (!temp.empty()) {
            Emergency e = temp.top();
            temp.pop();

            cout << "Request ID: " << e.requestId
                 << " | Patient: " << e.patientName
                 << " | Location: " << e.location
                 << " | Priority: " << e.priority << '\n';
        }
    }
};

int main() {
    AmbulanceManagement system;
    int choice;

    do {
        cout << "\n====================================\n";
        cout << "    AMBULANCE MANAGEMENT SYSTEM\n";
        cout << "====================================\n";
        cout << "1. Add Ambulance\n";
        cout << "2. View All Ambulances\n";
        cout << "3. Register Emergency\n";
        cout << "4. Assign Ambulance\n";
        cout << "5. Complete Emergency\n";
        cout << "6. View All Emergencies\n";
        cout << "7. View Pending Emergencies\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid input! Enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: system.addAmbulance(); break;
            case 2: system.viewAmbulances(); break;
            case 3: system.registerEmergency(); break;
            case 4: system.assignAmbulance(); break;
            case 5: system.completeEmergency(); break;
            case 6: system.viewEmergencies(); break;
            case 7: system.viewPending(); break;
            case 0: cout << "Exiting system.\n"; break;
            default: cout << "Invalid choice! Try again.\n";
        }

    } while (choice != 0);

    return 0;
}
