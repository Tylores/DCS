#include "include/Resource.hpp"

Resource::Resource () : rated_import_power_(3000),
                        rated_export_power_(3000),
                        rated_import_energy_(30000),
                        rated_export_energy_(24000),
                        import_ramp_(100),
                        export_ramp_(100),
                        idle_losses_(100),
                        import_power_(0),
                        export_power_(0),
                        import_energy_(0),
                        export_energy_(24000),
                        import_watts_(0),
                        export_watts_(0),
                        delta_time_(0) {
    //ctor
}

Resource::~Resource () {
    //dtor
}

// Set Import Power
// - set the import control property used in the control loop
void Resource::SetImportPower (unsigned int power) {
    export_watts_ = 0;
    export_power_ = 0;
    if (power > rated_import_power_) {
        power = rated_import_power_;
    }
    import_watts_ = power;
}  // end Set Import Power

// Set Export Power
// - set the export control property used in the control loop
void Resource::SetExportPower (unsigned int power) {
    import_watts_ = 0;
    import_power_ = 0;
    if (power > rated_export_power_) {
        power = rated_export_power_;
    }
    export_watts_ = power;
}  // end Set Export Power

// Import Power
// - called by control loop if import power is set
// - assume loss is factored into import power
void Resource::ImportPower () {
    float seconds = delta_time_ / 1000;
    float watts = import_ramp_ * seconds;

    // regulate import power
    if (import_power_ + watts < import_watts_) {
        import_power_ += watts;
    } else {
        import_power_ = import_watts_;
    }

    // regulate energy
    float hours = seconds / (60*60);
    if (import_energy_ - import_power_ > 0) {
        // area under the linear function
        import_energy_ -= (import_power_*hours + watts*hours/2);
        export_energy_ = rated_export_energy_ - import_energy_;
    } else {
        import_power_ = 0;
        import_energy_ = 0;
        export_energy_ = rated_export_energy_;
    }
}  // end Import Power

// Export Power
// - called by control loop if export power is set
// - assume loss is factored into export power
void Resource::ExportPower () {
    float seconds = delta_time_ / 1000;
    float watts = export_ramp_ * seconds;

    // regulate import power
    if (export_power_ + watts < export_watts_) {
        export_power_ += watts;
    } else {
        export_power_ = export_watts_;
    }

    // regulate energy
    float hours = seconds / (60*60);
    if (export_energy_ - export_power_ > 0) {
        // area under the linear function
        export_energy_ -= (export_power_*hours + watts*hours/2);
        import_energy_ = rated_import_energy_ - export_energy_;
    } else {
        export_power_ = 0;
        export_energy_ = 0;
        import_energy_ = rated_import_energy_;
    }
}  // end Export Power

// Idle Loss
// - update energy available based on energy lost
void Resource::IdleLoss () {
    float seconds = delta_time_ / 1000;
    float hours = seconds / (60*60);
    float energy_loss = idle_losses_ * hours;

    if (import_energy_ + energy_loss < rated_import_energy_) {
        import_energy_ += energy_loss;
    }

    if (export_energy_ - energy_loss > 0) {
        export_energy_ -= energy_loss;
    }
}  // end Idle Loss

// Control
// - check state of import / export power properties from main loop on a timer
void Resource::Control (unsigned int delta_time) {
    delta_time_ = delta_time;

    if (import_watts_ > 0) {
        Resource::ImportPower ();
    } else if (export_watts_ > 0) {
        Resource::ExportPower ();
    } else {
        IdleLoss ();
    }
}  // end Control

// Get Properties
// - return properties for DERAS
std::map <std::string, float> Resource::GetProperties () {
    std::map <std::string, float> properties;
    properties["ImportPower"] = import_power_;
    properties["ExportPower"] = export_power_;
    properties["ImportEnergy"] = import_energy_;
    properties["ExportEnergy"] = export_energy_;
    properties["ImportRamp"] = import_ramp_;
    properties["ExportRamp"] = export_ramp_;
    properties["IdleLoss"] = idle_losses_;
    return properties;
}  // end Get Properties
