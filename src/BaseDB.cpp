#include "BaseDB.hpp"
#include <mars/utils/misc.h>
#include <xtypes/ComponentModel.hpp>

using namespace configmaps;
using namespace xtypes;

namespace xrock_io_library
{

    BaseDB::BaseDB()
    {
    }

    BaseDB::~BaseDB()
    {
    }

    configmaps::ConfigMap BaseDB::getPropertiesOfComponentModel()
    {      
        ConfigMap propMap;
        auto cm = std::make_shared<ComponentModel>();
        const nl::json props = cm->get_properties();

        for (auto it = props.begin(); it != props.end(); ++it)
        {
            if (it->is_null())
                continue; // skip for now..

            std::string key = it.key();

     
            const auto allowed_values = cm->get_allowed_property_values(key);
            if (allowed_values.size() > 0)
            {
                int i = 0;
                for (const auto &allowed : allowed_values)
                {
                    std::string s = allowed;
                    propMap[key]["type"] = "array";
                    propMap[key]["allowed_values"][i++] = s;
                }
            }
            else if (it.value().is_string())
            {
                propMap[key]["type"] = "string";
                propMap[key]["value"] = std::string(it.value());
            }
            else if (it.value().is_boolean())
            {
                propMap[key]["type"] = "boolean";
                propMap[key]["value"] = bool(it.value());
            }      
        }
        return propMap;
    }

    std::vector<std::string> BaseDB::getDomains()
    {
        std::vector<std::string> domains;
        const auto cm = ComponentModel();
        for (const auto &d : cm.get_allowed_property_values("domain"))
        {
            domains.push_back(d.get<std::string>());
        }
        return domains;
    }

    configmaps::ConfigMap BaseDB::getEmptyComponentModel()
    {
        auto cm = ComponentModel();
        cm.set_all_unknown_facts_empty();
        return ConfigMap::fromJsonString(cm.export_to_basic_model());
    }

} // end of namespace xrock_io_library
