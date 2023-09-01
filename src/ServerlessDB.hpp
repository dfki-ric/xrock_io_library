/**
 * \file ServerlessDB.hpp
 * \author Malte Langosz
 * \brief Fuctions to communicate with the database
 **/

#pragma once
#include <configmaps/ConfigMap.hpp>
#include <xdbi/Serverless.hpp>
#include <xtypes_generator/XTypeRegistry.hpp>
#include <memory>
#include <xrock_gui_model/DBInterface.hpp>
#include "BaseDB.hpp"

namespace xrock_io_library
{

    class ServerlessDB : public BaseDB
    {

    public:
        explicit ServerlessDB(const fs::path &db_path, const std::string &graph);
        ~ServerlessDB();

        virtual std::vector<std::pair<std::string, std::string>> requestModelListByDomain(const std::string &domain) override;
        virtual std::vector<std::string> requestVersions(const std::string &domain, const std::string &model) override;
        virtual configmaps::ConfigMap requestModel(const std::string &domain,
                                                   const std::string &model,
                                                   const std::string &version,
                                                   const bool limit = false) override;
        virtual bool storeModel(const configmaps::ConfigMap &map) override;
        virtual bool removeModel(const std::string &uri) override;

        virtual void setDbGraph(const std::string &_dbGraph) override;
        virtual void setDbPath(const fs::path &_dbPath) override;

    private:
        std::unique_ptr<xdbi::Serverless> serverless;
        xtypes::XTypeRegistryPtr registry;
    };
} // end of namespace xrock_io_library

