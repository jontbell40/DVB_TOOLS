# DVB_TOOLS
Some simple tools for stripping analysis DVB transport streams fro the command line.

## Pid filter ##
   Can be built by gcc -o pid_filter ./pid_filter (Will add Makefile once more than one tool)
   Can then be used to filter a pid off a DVB transport stream as such:-
   ```  
   ./pid_filter -p 16 -f ~/Downloads/file.ts
   ```
   Sample ts files can be gained from https://tsduck.io/streams/?name=france-dttv

