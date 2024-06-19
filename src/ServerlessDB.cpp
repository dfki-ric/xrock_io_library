#include "ServerlessDB.hpp"
#include <mars_utils/misc.h>
#include <xtypes/ComponentModel.hpp>
#include <xtypes/Module.hpp>
#include <functional>
#include <map>
#include <vector>
#include <xtypes/ProjectRegistry.hpp>

#include <iostream>
#include <iomanip>
#include <ctime>

using namespace configmaps;
using namespace xtypes;

namespace xrock_io_library
{

    ServerlessDB::ServerlessDB(const fs::path &db_path, const std::string &graph) : registry(new xtypes::ProjectRegistry())
    {
        serverless = std::make_unique<xdbi::Serverless>(registry, db_path,graph);
    }

    ServerlessDB::~ServerlessDB()
    {
    }

    std::vector<std::pair<std::string, std::string>> ServerlessDB::requestModelListByDomain(const std::string &domain)
    {
        nl::json props;
        props["domain"] = mars::utils::toupper(domain);
        const std::vector<XTypePtr> models = serverless->find(ComponentModel::classname, props);
        std::vector<std::pair<std::string, std::string>> out;
        out.reserve(models.size());
        std::transform(models.begin(), models.end(), std::back_inserter(out), [&](auto &model)
                       {if (!model->has_property("version"))throw std::runtime_error("Model of ComponentModel type has no 'version' property!");
                       return std::make_pair(model->get_property("name"), ""); }); // Why do we need a type? How about giving bool model->get_property("can_have_parts")); }); to filter for editable ComponentModels
        return out;
    }

    std::vector<std::string> ServerlessDB::requestVersions(const std::string &domain, const std::string &model)
    {
        nl::json props;
        props["name"] = model;
        props["domain"] = mars::utils::toupper(domain);
        const std::vector<XTypePtr> models = serverless->find(ComponentModel::classname, props);
        std::vector<std::string> out;
        out.reserve(models.size());
        std::transform(models.begin(), models.end(), std::back_inserter(out), [&](auto &model)
                       {if (!model->has_property("version"))throw std::runtime_error("Model of ComponentModel type has no 'version' property!");
                      return model->get_property("version") ; });
        return out;
    }

    ConfigMap ServerlessDB::requestModel(const std::string &domain,
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

        std::vector<XTypePtr> xtypes = serverless->find(ComponentModel::classname, props);
        if (xtypes.size() == 0)
        {
            std::cerr << "ComponentModel with props: " << props << " not loaded" << std::endl;
            abort();
        }
        return ConfigMap::fromJsonString(std::static_pointer_cast<ComponentModel>(xtypes[0])->export_to_basic_model());
    }

    bool ServerlessDB::storeModel(const ConfigMap &map)
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

        // # import from JSON and export to DB
        std::vector<ComponentModelPtr> models = ComponentModel::import_from_basic_model(serialized_model, registry);
        std::vector<XTypePtr> db_models{};
        db_models.reserve(models.size());
         std::transform(models.begin(), models.end(), std::back_inserter(db_models), [&](auto &model)
                        { return std::static_pointer_cast<XType>(model); });

         try
         {
            serverless->update(db_models);
        }
        catch (const std::exception &ex)
        {
            // Handle other types of exceptions
            std::cerr << "ERROR: " << ex.what() << std::endl;
            return false;
        }
        return true;
    }
    bool ServerlessDB::buildModule(const std::string &uri, const std::string &moduleName, const std::map<std::string, std::string> &selectedImplementationsUris)
    {
        // Load the top level cm to build module for
        xtypes::XTypePtr xtype = serverless->load(uri, ComponentModel::classname);
        xtypes::ComponentModelPtr top_lvl_cm = std::static_pointer_cast<ComponentModel>(xtype);
        try
        {            
            // Prepare callback to answer the build() function when it asks for which implementation
            // to pick for an abstract component model.
            auto select_implementation = [&](const ComponentModelPtr &abstract_model, const std::vector<ComponentModelPtr> &implementations) -> ComponentModelPtr
            {
                //std::cout << "Callback is asking for " << abstract_model->get_name() << " implementation ...";
                for (auto &[abstract_model_uri, implementation_uri] : selectedImplementationsUris)
                {
                    if (abstract_model->uri() == abstract_model_uri)
                    {
                        for (auto &impl_model : implementations)
                        {
                            if(impl_model->uri() == implementation_uri)
                                return impl_model;
                        }
                    }
                }
                return nullptr;
            };
            // Build the module
            xtypes::ModulePtr built_module = top_lvl_cm->build(moduleName, select_implementation);
            if (!built_module)
                throw std::runtime_error("Couldn't build module " + moduleName);
            // Save module to db
            std::vector<xtypes::XTypePtr> bundle;
            bundle.push_back(built_module);
            serverless->update(bundle);
        }
        catch (const std::exception &ex)
        {
            // Handle other types of exceptions
            std::cerr << "ERROR: " << ex.what() << std::endl;
            return false;
        }
        return true;
    }

    ConfigMap ServerlessDB::getUnresolvedAbstracts(const std::string& uri)
    {
        xtypes::XTypePtr xtype = serverless->load(uri, ComponentModel::classname);
        xtypes::ComponentModelPtr top_lvl_cm = std::static_pointer_cast<ComponentModel>(xtype);

        std::function< ConfigMap(xtypes::ComponentModelPtr) > recursive_abstract_parts;
        recursive_abstract_parts = [&](xtypes::ComponentModelPtr model) 
        {
            ConfigMap results;
            results["unresolved_abstracts"] = ConfigVector();
            for (const auto &[p, _] : model->get_facts("parts"))  
            {
                const ComponentPtr part(std::static_pointer_cast<Component>(p.lock()));
                const ComponentModelPtr part_model(part->get_type()); 
                if (part_model->is_abstract()) 
                {
                    std::vector<ComponentModelPtr> implementations = part_model->get_implementations();
                    if (implementations.size() > 1)
                    {
                    
                        ConfigMap unresolved_abstract;
                        unresolved_abstract["uri"] = part_model->uri();
                        unresolved_abstract["name"] = part_model->get_name();
                        unresolved_abstract["version"] = part_model->get_version();
                        unresolved_abstract["alias"] = part->get_alias();
                        unresolved_abstract["implementations"] = ConfigVector();
                        for (auto &a : implementations)
                        {
                            ConfigMap impl;
                            impl["uri"] = a->uri();
                            impl["name"] = a->get_name();
                            impl["version"] = a->get_version();
                            unresolved_abstract["implementations"].push_back(impl);
                        }
                        results["unresolved_abstracts"].push_back(unresolved_abstract);
                    }
                }
                
                ConfigMap sub_result = recursive_abstract_parts(part_model);
                for (auto &ua : sub_result["unresolved_abstracts"])
                    results["unresolved_abstracts"].push_back(ua);
                
            }
            return results;
        };
        return recursive_abstract_parts(top_lvl_cm);
    }

    bool ServerlessDB::removeModel(const std::string &uri)
    {
        return serverless->remove(uri);
    }

    void ServerlessDB::setDbGraph(const std::string &_dbGraph)
    {
        serverless->setWorkingGraph(_dbGraph);
    }
    void ServerlessDB::setDbPath(const fs::path &_dbPath)
    {
        serverless->setWorkingDbPath(_dbPath);
    }

} // end of namespace xrock_io_library
