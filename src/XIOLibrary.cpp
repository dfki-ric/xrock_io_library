#include "XIOLibrary.hpp"
#include "RestDB.hpp"
#include "ServerlessDB.hpp"
#include "MultiDB.hpp"
#include <xrock_gui_model/FileDB.hpp>
#include <mars_utils/misc.h>

namespace xrock_io_library
{

    XIOLibrary::XIOLibrary(lib_manager::LibManager *theManager) : lib_manager::LibInterface(theManager)
    {
    }

    XIOLibrary::~XIOLibrary()
    {
    }

    xrock_gui_model::DBInterface *XIOLibrary::getDB(configmaps::ConfigMap &env)
    {
        if (env.hasKey("dbType") && env["dbType"] == "Client" && env.hasKey("dbAddress") && env.hasKey("dbGraph"))
        {
            return new RestDB(env["dbAddress"], env["dbGraph"]);
        }
        else if (env.hasKey("dbType") && env["dbType"] == "Serverless" && env.hasKey("dbPath") && env.hasKey("dbGraph"))
        {
            fs::path dbPath = env["dbPath"].toString();
            if (dbPath.is_relative())
                dbPath = fs::path(env["AUTOPROJ_CURRENT_ROOT"]) / dbPath;
            return new ServerlessDB(dbPath, env["dbGraph"]);
        }
        else if (env.hasKey("dbType") && env["dbType"] == "MultiDbClient" && env.hasKey("multiDBConfig"))
        {
            nl::json cfg = nl::json::parse(env["multiDBConfig"].getString());
            if (cfg["main_server"]["type"] == "Serverless")
            {
                fs::path mainServerDbPath = cfg["main_server"]["path"].get<std::string>();
                if (mainServerDbPath.is_relative())
                {
                    mainServerDbPath = fs::path(env["AUTOPROJ_CURRENT_ROOT"]) / mainServerDbPath;
                    cfg["main_server"]["path"] = mainServerDbPath;
                }
                for (auto &is : cfg["import_servers"])
                {
                    if (is["type"] == "Serverless")
                    {
                        fs::path importServerDbPath = is["path"].get<std::string>();
                        if (importServerDbPath.is_relative())
                        {
                            importServerDbPath = fs::path(env["AUTOPROJ_CURRENT_ROOT"]) / importServerDbPath;
                            is["path"] = importServerDbPath;
                        }
                    }
                }
                return new MultiDB(cfg);
            }
        } 
        return new xrock_gui_model::FileDB();
    }

    std::vector<std::string> XIOLibrary::getBackends()
    {
        return xdbi::DbInterface::get_available_backends();
    }
    configmaps::ConfigMap XIOLibrary::getDefaultConfig()
    {
        nl::json config = xdbi::DbInterface::getDefaultConfig();
        if (!config.empty())
            return configmaps::ConfigMap::fromJsonString(config.dump());
        else
            return configmaps::ConfigMap();
    }

}

DESTROY_LIB(xrock_io_library::XIOLibrary)
CREATE_LIB(xrock_io_library::XIOLibrary)
