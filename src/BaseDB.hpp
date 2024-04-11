/**
 * \file RestDB.hpp
 * \author Malte Langosz
 * \brief Fuctions to communicate with the database
 **/

#pragma once
#include <configmaps/ConfigMap.hpp>
#include <xrock_gui_model/DBInterface.hpp>

namespace xrock_io_library
{

    class BaseDB : public xrock_gui_model::DBInterface
    {

    public:
        BaseDB();
        ~BaseDB();

        // should have override keyword once we merged refactoring of xrock_gui_model to master
        virtual configmaps::ConfigMap getPropertiesOfComponentModel();
        virtual std::vector<std::string> getDomains();

        // This function returns an initially empty component model to start from scratch
        virtual configmaps::ConfigMap getEmptyComponentModel();

    };
} // end of namespace xrock_io_library
