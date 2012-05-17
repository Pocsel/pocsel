#include "tools/models/convert/Converter.hpp"

int main(int argc, char **argv)
{
    if(argc < 1)
        return EXIT_FAILURE;

    std::vector<Tools::Models::Convert::filespec> infiles;
    Tools::Models::Convert::filespec inspec;
    const char *outfile = NULL;
    for(int i = 1; i < argc; i++)
    {
        //if(argv[i][0] == '-')
        //{
        //    if(argv[i][1] == '-')
        //    {
        //        if(!strcasecmp(&argv[i][2], "fps")) { if(i + 1 < argc) inspec.fps = atof(argv[++i]); }
        //        else if(!strcasecmp(&argv[i][2], "name")) { if(i + 1 < argc) inspec.name = argv[++i]; }
        //        else if(!strcasecmp(&argv[i][2], "loop")) { inspec.flags |= IQM_LOOP; }
        //        else if(!strcasecmp(&argv[i][2], "start")) { if(i + 1 < argc) inspec.startframe = max(atoi(argv[++i]), 0); }
        //        else if(!strcasecmp(&argv[i][2], "end")) { if(i + 1 < argc) inspec.endframe = max(atoi(argv[++i]), 0); }
        //        else if(!strcasecmp(&argv[i][2], "scale")) { if(i + 1 < argc) escale = clamp(atof(argv[++i]), 1e-8, 1e8); }
        //    }
        //    else switch(argv[i][1])
        //    {
        //        case 's':
        //            if(i + 1 < argc) escale = clamp(atof(argv[++i]), 1e-8, 1e8);
        //            break;
        //    }
        //}
        if(!outfile) outfile = argv[i];
        else
        {
            infiles.push_back(inspec);
            infiles.back().file = argv[i];
            inspec.reset();
        }
    }

    if(!outfile) throw std::runtime_error("no output file specified");
    else if(infiles.empty()) throw std::runtime_error("no input files specified");

    Tools::Models::Convert::convert(outfile, infiles);

    return EXIT_SUCCESS;
}
