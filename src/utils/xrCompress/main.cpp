#include "StdAfx.h"
#include "xrCompress.h"

#ifndef MOD_COMPRESS
extern int ProcessDifference(pcstr params);
#endif

int __cdecl main(int argc, char* argv[])
{
#if defined(WINDOWS)
    LPCSTR params = GetCommandLine();
#else
	pstr params = nullptr;
	int i;
	if(argc > 1)
	{
		size_t sum = 0;
		for(i = 1; i < argc; ++i)
			sum += strlen(argv[i]) + strlen(" \0");

		params = static_cast<char*>(xr_malloc(sum));
		ZeroMemory(params, sum);

		for(i = 1; i < argc; ++i)
		{
			strcat(params, argv[i]);
			strcat(params, " ");
		}
	}
	else
		params = strdup("");
#endif

    xrDebug::Initialize(params);
    Core.Initialize("xrCompress", params, nullptr, FALSE);
    printf("\n\n");


#ifndef MOD_COMPRESS
    if (strstr(params, "-diff"))
    {
        ProcessDifference(params);
    }
    else
#endif
    {
#ifndef MOD_COMPRESS
        if (argc < 2)
        {
            printf("ERROR: u must pass folder name as parameter.\n");
            printf("-diff /? option to get information about creating difference.\n");
            printf("-fast	- fast compression.\n");
            printf("-store	- store files. No compression.\n");
            printf("-ltx <file_name.ltx> - pathes to compress. OR\n");
            printf("-header <header_file_name> - pathes to header file.\n");
            printf("\n");
            printf("LTX format:\n");
            printf("	[config]\n");
            printf("	;<path>     = <recurse>\n");
            printf("	.\\         = false\n");
            printf("	textures    = true\n");

            Core._destroy();
            return 3;
        }
#endif

        string_path folder;
        strconcat(sizeof(folder), folder, argv[1], "\\");
        SDL_strlwr(folder);
        printf("\nCompressing files (%s)...\n\n", folder);

        FS._initialize(CLocatorAPI::flTargetFolderOnly, folder);
        FS.append_path("$working_folder$", "", nullptr, false);

        xrCompressor C;

        C.SetStoreFiles(nullptr != strstr(params, "-store"));
        C.SetFastMode(nullptr != strstr(params, "-fast"));
        C.SetTargetName(argv[1]);

        pcstr p = strstr(params, "-ltx");

        if (nullptr != p)
        {
            string64 ltx_name;
            sscanf(p + 5, "%[^ ] ", ltx_name);

            CInifile ini(ltx_name);
            printf("Processing LTX...\n");
            C.ProcessLTX(ini);
        }
        else
        {
            p = strstr(params, "-header ");
            if(p)
            {
                string64 header_name;
                sscanf(p + 8, "%[^ ] ", header_name);
                C.SetPackHeaderName(header_name);
                C.ProcessTargetFolder();
            }
            else
            {
                printf("ERROR: u must pass -header as parameter.\n");
                printf("-header <header_file_name>\n");

                Core._destroy();
                return 3;
            }
        }
    }

    Core._destroy();
#if !defined(WINDOWS)
	xr_free(params);
#endif
    return 0;
}
