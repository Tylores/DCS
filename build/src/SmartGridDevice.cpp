#include <alljoyn/Status.h>
#include <alljoyn/BusObject.h>
#include <alljoyn/BusAttachment.h>


#include "include/SmartGridDevice.hpp"

// Constructor
// - initialize bus object interface and smart grid device properties
SmartGridDevice::SmartGridDevice (DistributedEnergyResource* der,
                                  ajn::BusAttachment* bus,
                                  const char* name,
                                  const char* path) : ajn::BusObject(path),
                                                      der_(der),
                                                      bus_(bus),
                                                      interface_(name),
                                                      signal_(NULL) {
    const ajn::InterfaceDescription* interface = bus_->GetInterface(interface_);
    assert(interface != NULL);
    AddInterface(*interface, ANNOUNCED);

    const ajn::BusObject::MethodEntry methods[] = {{
            interface->GetMember("ImportPower"), 
            static_cast <ajn::BusObject::MessageReceiver::MethodHandler>
            (&SmartGridDevice::ImportPowerHandler)
        }, {
            interface->GetMember("ExportPower"), 
            static_cast <ajn::BusObject::MessageReceiver::MethodHandler>
            (&SmartGridDevice::ExportPowerHandler)
        },

    };

    size_t count = sizeof (methods) / sizeof (methods[0]);
    QStatus status = AddMethodHandlers (methods, count);
    if (ER_OK != status) {
        throw status;
    }
}

// Import Power Handler
// - called by remote consumer and sends the watt value for import
void SmartGridDevice::ImportPowerHandler (
        const ajn::InterfaceDescription::Member* member,
        ajn::Message& message) {
    (void)member;
    std::cout << "[ALLJOYN]: Import...\t" << message->GetArg(0)->v_uint32 << std::endl;
    der_->SetImportWatts (message->GetArg(0)->v_uint32);
}  // end Import Power Handler

// Export Power Handler
// - called by remote consumer and sends the watt value for export
void SmartGridDevice::ExportPowerHandler (
        const ajn::InterfaceDescription::Member* member,
        ajn::Message& message) {
    (void)member;
    std::cout << "[ALLJOYN]: Export...\t" << message->GetArg(0)->v_uint32 << std::endl;
    der_->SetExportWatts (message->GetArg(0)->v_uint32);
}  // end Export Power Handler

// Get
// - this method will be called by remote devices looking to get this devices
// - properties
QStatus SmartGridDevice::Get (const char* interface,
                              const char* property,
                              ajn::MsgArg& value) {
    QStatus status;
    if (strcmp(interface, interface_)) {
        return ER_FAIL;
    }

    if (!strcmp(property,"rated_import_power")) {
        status = value.Set("u", der_->GetRatedImportPower ());
        return status;
    } else if (!strcmp(property,"rated_import_energy")) {
        status = value.Set("u", der_->GetRatedImportEnergy ());
        return status;
    } else if (!strcmp(property,"import_power")) {
        status = value.Set("u", der_->GetImportPower ());
        return status;
    } else if (!strcmp(property,"import_energy")) {
        status = value.Set("u", der_->GetImportEnergy ());
        return status;
    } else if (!strcmp(property,"import_ramp")) {
        status = value.Set("u", der_->GetImportRamp ());
        return status;
    } else if (!strcmp(property,"rated_export_power")) {
        status = value.Set("u", der_->GetRatedExportPower ());
        return status;
    } else if (!strcmp(property,"rated_export_energy")) {
        status = value.Set("u", der_->GetRatedExportEnergy ());
        return status;
    } else if (!strcmp(property,"export_power")) {
        status = value.Set("u", der_->GetExportPower ());
        return status;
    } else if (!strcmp(property,"export_energy")) {
        status = value.Set("u", der_->GetExportEnergy ());
        return status;
    } else if (!strcmp(property,"export_ramp")) {
        status = value.Set("u", der_->GetExportRamp ());
        return status;
    } else if (!strcmp(property,"idle_losses")) {
        status = value.Set("u", der_->GetIdleLosses ());
        return status;
    } else {
        return ER_FAIL;
    }
} // end Get

QStatus SmartGridDevice::SendPropertiesUpdate () {
const char* props[] = { "rated_export_power",
                        "export_power",
                        "rated_export_energy",
                        "export_energy",
                        "export_ramp",
                        "rated_import_power",
                        "import_power",
                        "rated_import_energy",
                        "import_energy",
                        "import_ramp",
                        "idle_losses"};
    QStatus status;
    status = EmitPropChanged (interface_, props, 11, ajn::SESSION_ID_ALL_HOSTED);
    return status;
}
