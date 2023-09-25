/**
 * \file RestDB.hpp
 * \author Malte Langosz
 * \brief Fuctions to communicate with the database
 **/

#pragma once
#include <configmaps/ConfigMap.hpp>
#include <configmaps/ConfigVector.hpp>
#include <configmaps/ConfigAtom.hpp>
#include <xdbi/Client.hpp>
#include <xtypes_generator/XTypeRegistry.hpp>
#include <xtypes/ComponentModel.hpp>
#include <memory>
#include <xrock_gui_model/DBInterface.hpp>
#include "BaseDB.hpp"

namespace xrock_io_library
{

    class RestDB : public BaseDB
    {

    public:
        RestDB(const std::string& dbAddress, const std::string& graph);
        ~RestDB();

        virtual std::vector<std::pair<std::string, std::string>> requestModelListByDomain(const std::string &domain) override;

        virtual std::vector<std::string> requestVersions(const std::string &domain, const std::string &model) override;

        virtual configmaps::ConfigMap requestModel(const std::string &domain,
                                                   const std::string &model,
                                                   const std::string &version,
                                                   const bool limit = false) override;

        virtual bool storeModel(const configmaps::ConfigMap &map) override;

        virtual bool removeModel(const std::string &uri) override;

        virtual bool buildModule(const std::string &uri, const std::string &moduleName, const std::map<std::string, std::string> &selectedImplementationsUris) override;

        virtual configmaps::ConfigMap getUnresolvedAbstracts(const std::string &uri) override;

        virtual void setDbAddress(const std::string &_dbAddress) override;

        virtual void setDbGraph(const std::string &_dbGraph) override;

        virtual void setDbUser(const std::string &_dbUser);

        virtual void setDbPassword(const std::string &_dbPassword);

        virtual bool isConnected() override;

    private:
        std::unique_ptr<xdbi::Client> client;
        xtypes::XTypeRegistryPtr registry;
    };
} // end of namespace xrock_io_library
