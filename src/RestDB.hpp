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
        /**
         * @brief Requests a list of models and their associated domain names.
         *
         * This function retrieves a list of models and their corresponding domain names.
         *
         * @param domain The domain for which to retrieve the list of models.
         * @return A vector of pairs where each pair contains the model name and its associated domain name.
         */
        virtual std::vector<std::pair<std::string, std::string>> requestModelListByDomain(const std::string &domain) override;

        /**
         * @brief Requests a list of versions for a specific model within a domain.
         *
         * This function retrieves a list of available versions for a given model within a specific domain.
         *
         * @param domain The domain where the model is located.
         * @param model The name of the model for which to retrieve the versions.
         * @return A vector of strings containing the available versions for the specified model.
         */
        virtual std::vector<std::string> requestVersions(const std::string &domain, const std::string &model) override;

        /**
         * @brief Requests a specific model with the specified version from the given domain.
         *
         * This function retrieves a model with a specific version from the specified domain.
         *
         * @param domain The domain where the model is located.
         * @param model The name of the model to retrieve.
         * @param version The version of the model to retrieve.
         * @param limit If true, limits the model retrieval based on a certain condition.
         * @return A ConfigMap object containing the requested model's configuration.
         */
        virtual configmaps::ConfigMap requestModel(const std::string &domain,
                                                   const std::string &model,
                                                   const std::string &version,
                                                   const bool limit = false) override;

        /**
         * @brief Stores a model's configuration in the system.
         *
         * This function stores the configuration of a model in the system using a ConfigMap object.
         *
         * @param map The ConfigMap containing the model's configuration to be stored.
         * @return True if the model is successfully stored; otherwise, false.
         */
        virtual bool storeModel(const configmaps::ConfigMap &map) override;

        /**
         * @brief Removes a model from the system using its URI.
         *
         * This function removes a model from the system based on its URI.
         *
         * @param uri The URI of the model to be removed.
         * @return True if the model is successfully removed; otherwise, false.
         */
        virtual bool removeModel(const std::string &uri) override;

        /**
         * @brief Builds a module and saves it to database for a given model URI.
         *
         * This function builds a module for a given model URI.
         *
         * @param uri The URI of the model to build module for and store in the database.
         * @param moduleName The name of the module to be built.
         * @param selectedImplementationsUris A map of selected implementations uris for unresolved abstracts within the model.
         * @return True if the module is successfully built and saved to database; otherwise, false.
         */
        virtual bool buildModule(const std::string &uri, const std::string &moduleName, const std::map<std::string, std::string> &selectedImplementationsUris) override;

        /**
         * @brief Retrieves a map of unresolved abstracts for a given URI.
         *
         * This function retrieves a map of unresolved abstracts associated with the given URI.
         *
         * @param uri The URI for which to retrieve the unresolved abstracts.
         * @return A map where each key represents an unresolved abstract uri,name and version and the value is a list of implementations uri,name and version.
         */
        virtual configmaps::ConfigMap getUnresolvedAbstracts(const std::string &uri) override;

        /**
         * @brief Sets the address of the database used by the system.
         *
         * This function sets the address (e.g.,DB path or URL) of the database to be used by the system.
         *
         * @param _dbAddress The address of the database to be set.
         */
        virtual void setDbAddress(const std::string &_dbAddress) override;

        /**
         * @brief Sets the database graph for the system.
         *
         * This function sets the database graph used by the system.
         *
         * @param _dbGraph The string representation of the database graph to be set.
         */
        virtual void setDbGraph(const std::string &_dbGraph) override;

        /**
         * @brief Sets the username for accessing the database.
         *
         * This function sets the username to be used for authentication when accessing the database.
         *
         * @param _dbUser The username for database access to be set.
         */
        virtual void setDbUser(const std::string &_dbUser);

        /**
         * @brief Sets the password for accessing the database.
         *
         * This function sets the password to be used for authentication when accessing the database.
         *
         * @param _dbPassword The password for database access to be set.
         */
        virtual void setDbPassword(const std::string &_dbPassword);

        /**
         * @brief Checks the connection status to the database.
         *
         * This function checks whether the system is currently connected to the database.
         *
         * @return True if the system is connected to the database; otherwise, false.
         */
        virtual bool isConnected() override;

    private:
        std::unique_ptr<xdbi::Client> client;
        xtypes::XTypeRegistryPtr registry;
    };
} // end of namespace xrock_io_library
