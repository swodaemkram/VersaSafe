extern void DumpHelp(void);
extern bool FindHelpSection(string section);
extern void SetHelpFileName(string filename);
extern void CloseHelp(void);
extern void GetHelpContents(void);
extern help_data *HELP;
extern int HELP_pagecount;
extern contents_data * CONTENTS;
extern int HELP_content_sections;	// number of sections in the file (contents count)
extern void GetXMLVersion(int * maj, int *min, int *patch);


