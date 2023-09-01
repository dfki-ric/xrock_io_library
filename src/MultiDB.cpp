#include "MultiDB.hpp"
#include <mars/utils/misc.h>
#include <xtypes/ComponentModel.hpp>
#include <xtypes/ProjectRegistry.hpp>

#include <iostream>
#include <iomanip>
#include <ctime>

using namespace configmaps;
using namespace xtypes;

namespace xrock_io_library
{

    MultiDB::MultiDB(const nl::json &config) : registry(new xtypes::ProjectRegistry())
    {
        multidb = std::make_unique<xdbi::MultiDbClient>(registry, config);
    }

    MultiDB::~MultiDB()
    {
    }

    std::vector<std::pair<std::string, std::string>> MultiDB::requestModelListByDomain(const std::string &domain)
    {
        nl::json props;
        props["domain"] = mars::utils::toupper(domain);
        const std::vector<XTypePtr> models = multidb->find(ComponentModel::classname, props);
        std::vector<std::pair<std::string, std::string>> out;
        out.reserve(models.size());
        std::transform(models.begin(), models.end(), std::back_inserter(out), [&](auto &model)
                       {if (!model->has_property("version"))throw std::runtime_error("Model of ComponentModel type has no 'version' property!");
                       return std::make_pair(model->get_property("name"), ""); }); // Why do we need a type? How about giving bool model->get_property("can_have_parts")); }); to filter for editable ComponentModels
        return out;
    }

    std::vector<std::string> MultiDB::requestVersions(const std::string &domain, const std::string &model)
    {
        nl::json props;
        props["name"] = model;
        props["domain"] = mars::utils::toupper(domain);
        const std::vector<XTypePtr> models = multidb->find(ComponentModel::classname, props);
        std::vector<std::string> out;
        out.reserve(models.size());
        std::transform(models.begin(), models.end(), std::back_inserter(out), [&](auto &model)
                       {if (!model->has_property("version"))throw std::runtime_error("Model of ComponentModel type has no 'version' property!");
                       return model->get_property("version"); });
        return out;
    }

    ConfigMap MultiDB::requestModel(const std::string &domain,
                                    const std::string &model,
                                    const std::string &version,
                                    const bool limit)
    {
        nl::json props;
        props["domain"] = mars::utils::toupper(domain);
        props["name"] = model;
        if (version.size() > 0)
        {
            props["version"] = version;
        }

        std::vector<XTypePtr> xtypes = multidb->find(ComponentModel::classname, props);
        if (xtypes.size() == 0)
        {
            std::cerr << "ComponentModel with props: " << props << " not loaded" << std::endl;
            abort();
        }
        return ConfigMap::fromJsonString(std::static_pointer_cast<ComponentModel>(xtypes[0])->export_to_basic_model());
    }

    bool MultiDB::storeModel(const ConfigMap &map)
    {
        ConfigMap model = ConfigMap(map);
        // here we assume that the maps fits to the json representation required by the db
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
        auto date = oss.str();

        for (unsigned int i = 0; i < model["versions"].size(); ++i)
        {
            model["versions"][(int)i]["date"] = date;
        }


        std::string serialized_model = model.toJsonString();

        // # import from JSON and export to DB
        std::vector<ComponentModelPtr> models = ComponentModel::import_from_basic_model(serialized_model, registry);
        std::vector<XTypePtr> db_models{};
        std::transform(models.begin(), models.end(), std::back_inserter(db_models), [&](auto &model)
                       {return std::static_pointer_cast<XType>(model); });

        try
        {
            multidb->update(db_models);
        }
        catch (...)
        {
            std::cerr << "ERROR: Problem with database communication" << std::endl;
            return false;
        }
        return true;
    }

    void MultiDB::setDbGraph(const std::string &_dbGraph)
    {
        multidb->setWorkingGraph(_dbGraph);
    }

    bool MultiDB::removeModel(const std::string& uri) {
        return multidb->remove(uri);
    }

} // end of namespace xrock_io_library
