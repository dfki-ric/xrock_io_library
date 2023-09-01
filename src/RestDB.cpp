#include "RestDB.hpp"
#include <mars/utils/misc.h>
#include <xtypes/ComponentModel.hpp>

#include <iostream>
#include <iomanip>
#include <ctime>

using namespace configmaps;
using namespace xtypes;

namespace xrock_io_library
{

    RestDB::RestDB(const std::string& dbAddress, const std::string& graph) : registry(new xtypes::XTypeRegistry())
    {
        registry->register_class<ComponentModel>();
        client = std::make_unique<xdbi::Client>(registry,dbAddress, graph);
        client->setDbUser("");
        client->setDbPassword("");
    }

    RestDB::~RestDB()
    {
    }

    std::vector<std::pair<std::string, std::string>> RestDB::requestModelListByDomain(const std::string &domain)
    {
        nl::json props;
        props["domain"] = mars::utils::toupper(domain);
        const std::vector<XTypePtr> models = client->find(ComponentModel::classname, props, 0);
        std::vector<std::pair<std::string, std::string>> out;
        std::transform(models.begin(), models.end(), std::back_inserter(out), [&](auto &model) {
            if (!model->has_property("version"))
                throw std::runtime_error("Model of ComponentModel type has no 'version' property!");
            return std::make_pair(model->get_property("name"), ""); // Why do we need a type? How about giving bool model->get_property("can_have_parts")); }); to filter for editable ComponentModels
        });
        return out;
    }

    std::vector<std::string> RestDB::requestVersions(const std::string &domain, const std::string &model)
    {
        // ToDo as soon as modelDeepness is integrated, we can simplify this
        // request["modelDeepness"] = "versionOnly";
        nl::json props;
        props["name"] = model;
        props["domain"] = mars::utils::toupper(domain);
        const std::vector<XTypePtr> models = client->find(ComponentModel::classname, props, 0);
        std::vector<std::string> out;
        out.reserve(models.size());
        out.reserve(models.size());
        std::transform(models.begin(), models.end(), std::back_inserter(out), [&](auto &model)
                       {if (!model->has_property("version"))throw std::runtime_error("Model of ComponentModel type has no 'version' property!");
                      return model->get_property("version") ; });
        return out;
    }

    ConfigMap RestDB::requestModel(const std::string &domain,
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

        std::vector<XTypePtr> xtypes = client->find(ComponentModel::classname, props, limit ? 3 : -1);
        if (xtypes.size() == 0)
        {
            std::cerr << "ComponentModel with props: " << props << " not loaded" << std::endl;
            abort();
        }
        return ConfigMap::fromJsonString(std::static_pointer_cast<ComponentModel>(xtypes[0])->export_to_basic_model());
    }

    bool RestDB::storeModel(const ConfigMap &map)
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

        // In xdbi-proxy we set the type property of model via type2uri here
        // Before we can completely load the given component model, we have to find any part models!
        std::string serialized_model = model.toJsonString();

        auto load_missing_models = [&](const nl::json &u) -> ComponentModelPtr
        {
            ComponentModel dummy;
            dummy.set_properties(u);
            const std::string &uri(dummy.uri());
            std::vector<XTypePtr> full_models = client->find(ComponentModel::classname, nl::json{{"uri", uri}}); //, limit_recursion=True, recursion_depth=3)
            if (full_models.size())
                return std::static_pointer_cast<ComponentModel>(full_models[0]);
            return nullptr;
        };

        // # import from JSON and export to DB
        std::vector<ComponentModelPtr> models = ComponentModel::import_from_basic_model(serialized_model, load_missing_models);
        std::vector<XTypePtr> db_models{};
        db_models.reserve(models.size());
        std::transform(models.begin(), models.end(), std::back_inserter(db_models), [&](auto &model)
                       { return std::static_pointer_cast<XType>(model); });

        try
        {
            client->update(db_models);
        }
        catch (...)
        {
            std::cerr << "ERROR: Problem with database communication" << std::endl;
            return false;
        }
        return true;
    }
    bool RestDB::removeModel(const std::string& uri) {
         return client->remove(uri);
    }


    void RestDB::setDbAddress(const std::string &_dbAddress)
    {
        client->setDbAddress(_dbAddress);
    }

    void RestDB::setDbUser(const std::string &_dbUser)
    {
        client->setDbUser(_dbUser);
    }

    void RestDB::setDbPassword(const std::string &_dbPassword)
    {
        client->setDbPassword(_dbPassword);
    }

    void RestDB::setDbGraph(const std::string &_dbGraph)
    {
        client->setWorkingGraph(_dbGraph);
    }
    bool RestDB::isConnected()
    {
        return client->isReady();
    }

} // end of namespace xrock_io_library
