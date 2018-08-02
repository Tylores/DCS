#include <string>

#include <alljoyn/BusObject.h>
#include <alljoyn/BusAttachment.h>

#include "include/SmartGridDevice.hpp"
#include "include/ts_utility.hpp"

// Constructor
// - initialize bus object interface and smart grid device properties
SmartGridDevice::SmartGridDevice (ajn::BusAttachment* bus_ptr,
                                  const char* name,
                                  const char* path) : ajn::BusObject(path),
                                                      bus_(bus_ptr),
                                                      interface_(name),
                                                      signal_(NULL) {
    const ajn::InterfaceDescription* intf = bus_->GetInterface(interface_);
    assert(intf != NULL);
    AddInterface(*intf, ANNOUNCED);
}

// SetPowerStatus
// - update PowerStatus values with most recent updates
QStatus SmartGridDevice::SetPowerStatus (ajn::MsgArg* arg_ptr) {
    ajn::MsgArg& arg_ref = *arg_ptr;  // deref pointer

    // init MsgArg dictionary
    std::string name;
    size_t items = 4;
    ajn::MsgArg dict[items];

    name = "batteryStatus";
    dict[0].Set("{sv}", name.c_str(), power_status_.battery_status);

    name = "changedTime";
    dict[1].Set("{sv}", name.c_str(), power_status_.changed_time);

    name = "chargingPowerNow";
    dict[2].Set("{sv}", name.c_str(), power_status_.charging_power);

    name = "energyRequestNow";
    dict[3].Set("{sv}", name.c_str(), power_status_.energy_request);

    // pack dictionary into the alljoyn message that will be sent
    QStatus status = arg_ref.Set("a{sv}", items, &dict);
    return status;
} // end SetPowerStatus

// UpdatePowerStatus
// - send a power status changed signal to remote devices
void SmartGridDevice::UpdatePowerStatus (unsigned int time,
                                         int ev_state,
                                         int ev_power,
                                         int ev_energy) {
    enum {UNKNOWN, NORMAL, LOW, DEPLETED, NA};  // battery states
    enum {IDLE, CHARGING, DISCHARGING, DRIVING, DEAD};  // EV states

    // init PowerStatus values
    int state = ev_state;
    int battery;
    unsigned int power = 0;
    switch (state) {
        case IDLE: battery = NORMAL;
        case CHARGING:
            battery = NORMAL;
            power = ev_power;
        case DISCHARGING: battery = NORMAL;
        case DRIVING: battery = UNKNOWN;
        case DEAD: battery = DEPLETED;
    }

    power_status_.battery_status = battery;
    power_status_.changed_time = time;
    power_status_.charging_power = power;
    power_status_.energy_request = ev_energy;

    ajn::MsgArg val;
    const char* name = "PowerStatus";
    QStatus status = SmartGridDevice::SetPowerStatus(&val);
    if (status != ER_OK) {
        std::printf("[ERROR] setting PowerStatus MsgArg!\n");
    } else {
        name = "PowerStatus";
        //EmitPropChanged(interface_, name, val, ajn::SESSION_ID_ALL_HOSTED);
    }
} // end UpdateProperties

// Get
// - this method will be called by remote devices looking to get this devices
// - properties
QStatus SmartGridDevice::Get (const char* intf,
                              const char* propName,
                              ajn::MsgArg& val) {
    QStatus status;
    if (strcmp(intf, interface_)) {
        return ER_FAIL;
    }

    if (!strcmp(propName,"AssetName")) {
        status = val.Set("s", propName);
        return status;
    } else if (!strcmp(propName, "PowerStatus")) {
        ajn::MsgArg arg;
        status = SmartGridDevice::SetPowerStatus(&arg);
        status = val.Set("a{sv}", arg);
        return status;
    } else {
        std::printf("[ERROR] unknown property!\n");
        return ER_FAIL;
    }
} // end Get
