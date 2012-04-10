#ifndef __TOOLS_PLUGIN_PLUGIN__
#define __TOOLS_PLUGIN_PLUGIN__

namespace Tools { namespace Plugin {

    bool Create(boost::filesystem::path const& pluginRootDir, boost::filesystem::path const& destFile, std::ostream& log);

    bool Install(/* TODO */ std::ostream& log);

}}

#endif
