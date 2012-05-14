//#include "tools/models/convert/iqm.inl"

#if 0
    int main(int argc, char **argv)
    {
        if(argc < 1)
            return EXIT_FAILURE;

        vector<filespec> infiles;
        filespec inspec;
        const char *outfile = NULL;
        for(int i = 1; i < argc; i++)
        {
            if(argv[i][0] == '-')
            {
                if(argv[i][1] == '-')
                {
                    if(!strcasecmp(&argv[i][2], "fps")) { if(i + 1 < argc) inspec.fps = atof(argv[++i]); }
                    else if(!strcasecmp(&argv[i][2], "name")) { if(i + 1 < argc) inspec.name = argv[++i]; }
                    else if(!strcasecmp(&argv[i][2], "loop")) { inspec.flags |= IQM_LOOP; }
                    else if(!strcasecmp(&argv[i][2], "start")) { if(i + 1 < argc) inspec.startframe = max(atoi(argv[++i]), 0); }
                    else if(!strcasecmp(&argv[i][2], "end")) { if(i + 1 < argc) inspec.endframe = max(atoi(argv[++i]), 0); }
                    else if(!strcasecmp(&argv[i][2], "scale")) { if(i + 1 < argc) escale = clamp(atof(argv[++i]), 1e-8, 1e8); }
                }
                else switch(argv[i][1])
                {
                    case 's':
                        if(i + 1 < argc) escale = clamp(atof(argv[++i]), 1e-8, 1e8);
                        break;
                }
            }
            else if(!outfile) outfile = argv[i];
            else
            {
                infiles.add(inspec).file = argv[i];
                inspec.reset();
            }
        }

        if(!outfile) fatal("no output file specified");
        else if(infiles.empty()) fatal("no input files specified");

        if(escale != 1) printf("scale: %f\n", escale);

        loopv(infiles)
        {
            const filespec &inspec = infiles[i];
            const char *infile = inspec.file, *type = strrchr(infile, '.');
            if(!type) fatal("no file type: %s", infile);
            else if(!strcasecmp(type, ".md5mesh"))
            {
                if(loadmd5mesh(infile, inspec)) conoutf("imported: %s", infile);
                else fatal("failed reading: %s", infile);
            }
            else if(!strcasecmp(type, ".md5anim"))
            {
                if(loadmd5anim(infile, inspec)) conoutf("imported: %s", infile);
                else fatal("failed reading: %s", infile);
            }
            else if(!strcasecmp(type, ".iqe"))
            {
                if(loadiqe(infile, inspec)) conoutf("imported: %s", infile);
                else fatal("failed reading: %s", infile);
            }
            else if(!strcasecmp(type, ".smd"))
            {
                if(loadsmd(infile, inspec)) conoutf("imported: %s", infile);
                else fatal("failed reading: %s", infile);
            }
            else fatal("unknown file type: %s", type);
        }

        calcanimdata();

        conoutf("");

        if(writeiqm(outfile)) conoutf("exported: %s", outfile);
        else fatal("failed writing: %s", outfile);

        return EXIT_SUCCESS;
    }
#endif
