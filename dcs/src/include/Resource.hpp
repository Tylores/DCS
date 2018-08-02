#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <map>

class Resource
{
    public :
        // constructor / destructor
        Resource();
        virtual ~Resource();
    public :
        // member methods
        void SetImportPower (unsigned int power);
        void SetExportPower (unsigned int power);
        void ImportPower ();
        void ExportPower ();
        void IdleLoss ();
        void Control (unsigned int delta_time);
        std::map <std::string, float> GetProperties ();

    private :
        // rated properties
        unsigned int rated_import_power_;       // (watts) from grid
        unsigned int rated_export_power_;       // (watts) to grid
        unsigned int rated_import_energy_;      // (watt-hours)
        unsigned int rated_export_energy_;      // (watt-hours)
        unsigned int import_ramp_;              // (watts per second)
        unsigned int export_ramp_;              // (watts per second)
        unsigned int idle_losses_;              // watt-hours per hour

    private :
        // dynamic properties
        float import_power_;
        float export_power_;
        float import_energy_;
        float export_energy_;

    private :
        // control properties
        unsigned int import_watts_;
        unsigned int export_watts_;
        float delta_time_;  // milliseconds
};

#endif // RESOURCE_H
