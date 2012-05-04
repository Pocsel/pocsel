#ifndef __TOOLS_PLUGIN_CREATE_CREATE_HPP__
#define __TOOLS_PLUGIN_CREATE_CREATE_HPP__

namespace Tools { namespace PluginCreate {

    enum
    {
        FormatVersion = 1,
    };

    bool Create(boost::filesystem::path const& pluginRootDir, boost::filesystem::path const& destFile);

}}

#endif
