/**
 * \author Malte Langosz (malte.langosz@dfki.de)
 * \brief
 * \version 0.1
 */

#include <lib_manager/LibInterface.hpp>
#include <xrock_gui_model/XRockIOLibrary.hpp>

namespace xrock_io_library
{
    class XIOLibrary : public lib_manager::LibInterface,
                       public xrock_gui_model::XRockIOLibrary
    {
    public:
        explicit XIOLibrary(lib_manager::LibManager *theManager);
        ~XIOLibrary();

        // LibInterface methods
        int getLibVersion() const
        {
            return 1;
        }

        const std::string getLibName() const
        {
            return std::string("xrock_io_library");
        }

        CREATE_MODULE_INFO();

        virtual xrock_gui_model::DBInterface* getDB(configmaps::ConfigMap &env);
        virtual std::vector<std::string> getBackends();
        virtual configmaps::ConfigMap getDefaultConfig();
    
    };
}
