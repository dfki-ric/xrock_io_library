/**
 * \file MultiDB.hpp
 * \brief Fuctions to communicate with the multi-database
 **/

#pragma once
#include <configmaps/ConfigMap.hpp>
#include <configmaps/ConfigVector.hpp>
#include <configmaps/ConfigAtom.hpp>
#include <xdbi/MultiDbClient.hpp>
#include <xtypes_generator/XTypeRegistry.hpp>
#include <memory>
#include <xrock_gui_model/DBInterface.hpp>
#include "BaseDB.hpp"

namespace xrock_io_library
{

    class MultiDB : public BaseDB
    {

    public:
        explicit MultiDB(const nl::json &config);
        ~MultiDB();
 
        virtual std::vector<std::pair<std::string, std::string>> requestModelListByDomain(const std::string &domain) override;
        virtual std::vector<std::string> requestVersions(const std::string &domain, const std::string &model) override;
        virtual configmaps::ConfigMap requestModel(const std::string &domain,
                                                   const std::string &model,
                                                   const std::string &version,
                                                   const bool limit = false) override;
        virtual bool storeModel(const configmaps::ConfigMap &map) override;

        virtual bool buildModule(const std::string &uri, const std::string &moduleName, const std::map<std::string, std::string> &selectedImplementationsUris) override;

        virtual configmaps::ConfigMap getUnresolvedAbstracts(const std::string &uri) override;

        virtual bool removeModel(const std::string &uri) override;
     
        virtual void setDbGraph(const std::string &_dbGraph) override;

    private:
        std::unique_ptr<xdbi::MultiDbClient> multidb;
        xtypes::XTypeRegistryPtr registry;
    };
} // end of namespace xrock_io_library

