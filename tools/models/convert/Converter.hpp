#ifndef __TOOLS_MODELS_CONVERT_CONVERTER_HPP__
#define __TOOLS_MODELS_CONVERT_CONVERTER_HPP__

namespace Tools { namespace Models { namespace Convert {

    struct filespec
    {
        const char *file;
        const char *name;
        double fps;
        uint flags;
        int startframe;
        int endframe;

        filespec() { reset(); }

        void reset()
        {
            file = NULL;
            name = NULL;
            fps = 0;
            flags = 0;
            startframe = 0;
            endframe = -1;
        }
    };

    bool writeiqm(const char *filename);
    bool loadsmd(const char *filename, const filespec &spec);
    bool loadmd5mesh(const char *filename, const filespec &spec);
    bool loadmd5anim(const char *filename, const filespec &spec);
    bool loadiqe(const char *filename, const filespec &spec);
    void convert(char const* outfile, std::vector<filespec> const& infiles);

}}}

#endif
